///////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2015 Edouard Griffiths, F4EXB                                   //
//                                                                               //
// This program is free software; you can redistribute it and/or modify          //
// it under the terms of the GNU General Public License as published by          //
// the Free Software Foundation as version 3 of the License, or                  //
//                                                                               //
// This program is distributed in the hope that it will be useful,               //
// but WITHOUT ANY WARRANTY; without even the implied warranty of                //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the                  //
// GNU General Public License V3 for more details.                               //
//                                                                               //
// You should have received a copy of the GNU General Public License             //
// along with this program. If not, see <http://www.gnu.org/licenses/>.          //
///////////////////////////////////////////////////////////////////////////////////

#include <QUdpSocket>
#include <QDebug>
#include <unistd.h>
#include "sdrdaemonudphandler.h"
#include "sdrdaemoninput.h"

const int SDRdaemonUDPHandler::m_rateDivider = 1000/SDRDAEMON_THROTTLE_MS;
const int SDRdaemonUDPHandler::m_udpPayloadSize = 512;

SDRdaemonUDPHandler::SDRdaemonUDPHandler(SampleFifo *sampleFifo, MessageQueue *outputMessageQueueToGUI) :
	m_mutex(QMutex::Recursive),
	m_sdrDaemonBuffer(m_udpPayloadSize),
	m_dataSocket(0),
	m_dataAddress(QHostAddress::LocalHost),
	m_dataPort(9090),
	m_dataConnected(false),
	m_udpBuf(0),
	m_udpReadBytes(0),
	m_buf(0),
	m_bufsize(0),
	m_chunksize(0),
	m_sampleFifo(sampleFifo),
	m_samplerate(0),
	m_centerFrequency(0),
	m_tv_sec(0),
	m_tv_usec(0),
	m_outputMessageQueueToGUI(outputMessageQueueToGUI),
	m_tickCount(0),
	m_samplesCount(0)
{
    m_udpBuf = new char[m_udpPayloadSize];
}

SDRdaemonUDPHandler::~SDRdaemonUDPHandler()
{
	stop();
	delete[] m_udpBuf;
}

void SDRdaemonUDPHandler::start()
{
	if (!m_dataSocket)
	{
		m_dataSocket = new QUdpSocket(this);
	}

	if (!m_dataConnected)
	{
		if (m_dataSocket->bind(m_dataAddress, m_dataPort))
		{
			qDebug("SDRdaemonUDPHandler::start: bind data socket to port %d", m_dataPort);
			connect(this, SIGNAL(dataReady()), this, SLOT(processData()));
			connect(m_dataSocket, SIGNAL(readyRead()), this, SLOT(dataReadyRead()), Qt::QueuedConnection); // , Qt::QueuedConnection
			m_dataConnected = true;
		}
		else
		{
			qWarning("SDRdaemonUDPHandler::start: cannot bind data port %d", m_dataPort);
			m_dataConnected = false;
		}
	}
}

void SDRdaemonUDPHandler::stop()
{
	qDebug("SDRdaemonUDPHandler::stop");

	if (m_dataConnected) {
		disconnect(m_dataSocket, SIGNAL(readyRead()), this, SLOT(dataReadyRead()));
		disconnect(this, SIGNAL(dataReady()), this, SLOT(processData));
		m_dataConnected = false;
	}

	if (m_dataSocket)
	{
		delete m_dataSocket;
		m_dataSocket = 0;
	}
}

void SDRdaemonUDPHandler::dataReadyRead()
{
	//qDebug() << "SDRdaemonUDPHandler::dataReadyRead";

	while (m_dataSocket->hasPendingDatagrams())
	{
		qint64 pendingDataSize = m_dataSocket->pendingDatagramSize();
		m_udpReadBytes = m_dataSocket->readDatagram(m_udpBuf, pendingDataSize, 0, 0);
		emit dataReady();
	}
}

void SDRdaemonUDPHandler::processData()
{
	if (m_udpReadBytes < 0)
	{
		qDebug() << "SDRdaemonThread::processData: read failed";
	}
	else if (m_udpReadBytes > 0)
	{
		QMutexLocker ml(&m_mutex);

		m_sdrDaemonBuffer.updateBlockCounts(m_udpReadBytes);

		if (m_sdrDaemonBuffer.readMeta(m_udpBuf, m_udpReadBytes))
		{
			const SDRdaemonBuffer::MetaData& metaData =  m_sdrDaemonBuffer.getCurrentMeta();
			bool change = false;
			m_tv_sec = metaData.m_tv_sec;
			m_tv_usec = metaData.m_tv_usec;

			if (m_samplerate != metaData.m_sampleRate)
			{
				setSamplerate(metaData.m_sampleRate);
				m_samplerate = metaData.m_sampleRate;
				change = true;
			}

			if (m_centerFrequency != metaData.m_centerFrequency)
			{
				m_centerFrequency = metaData.m_centerFrequency;
				change = true;
			}

			if (change)
			{
				SDRdaemonInput::MsgReportSDRdaemonStreamData *report = SDRdaemonInput::MsgReportSDRdaemonStreamData::create(
					metaData.m_sampleRate,
					metaData.m_centerFrequency,
					metaData.m_tv_sec,
					metaData.m_tv_usec);
				m_outputMessageQueueToGUI->push(report);
			}
		}
		else if (m_sdrDaemonBuffer.isSync())
		{
			m_sdrDaemonBuffer.writeData(m_udpBuf, m_udpReadBytes);
		}
	}
}

void SDRdaemonUDPHandler::setSamplerate(uint32_t samplerate)
{
	qDebug() << "SDRdaemonUDPHandler::setSamplerate:"
			<< " new:" << samplerate
			<< " old:" << m_samplerate;

	QMutexLocker ml(&m_mutex);

	m_samplerate = samplerate;
	m_chunksize = (m_samplerate / m_rateDivider)*4; // TODO: implement FF and slow motion here. 4 corresponds to live. 2 is half speed, 8 is doulbe speed
	m_bufsize = m_chunksize;

	if (m_buf == 0)	{
		qDebug() << "  - Allocate buffer";
		m_buf = (quint8*) malloc(m_bufsize);
	} else {
		qDebug() << "  - Re-allocate buffer";
		m_buf = (quint8*) realloc((void*) m_buf, m_bufsize);
	}

	qDebug() << "SDRdaemonUDPHandler::setSamplerate:"
			<< " size: " << m_bufsize
			<< " #samples: " << (m_bufsize/4);
}

void SDRdaemonUDPHandler::connectTimer(const QTimer& timer)
{
	qDebug() << "SDRdaemonUDPHandler::connectTimer";
	connect(&timer, SIGNAL(timeout()), this, SLOT(tick()));
}

void SDRdaemonUDPHandler::tick()
{
	// read samples directly feeding the SampleFifo (no callback)
	m_sampleFifo->write(reinterpret_cast<quint8*>(m_sdrDaemonBuffer.readData(m_chunksize)), m_chunksize);
	m_samplesCount += m_chunksize / 4;

	if (m_tickCount < m_rateDivider)
	{
		m_tickCount++;
	}
	else
	{
		m_tickCount = 0;
		SDRdaemonInput::MsgReportSDRdaemonStreamTiming *report = SDRdaemonInput::MsgReportSDRdaemonStreamTiming::create(
			m_tv_sec,
			m_tv_usec);
		m_outputMessageQueueToGUI->push(report);
	}
}


