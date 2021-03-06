/**
 * SDRangel
 * This is the web REST/JSON API of SDRangel SDR software. SDRangel is an Open Source Qt5/OpenGL 3.0+ (4.3+ in Windows) GUI and server Software Defined Radio and signal analyzer in software. It supports Airspy, BladeRF, HackRF, LimeSDR, PlutoSDR, RTL-SDR, SDRplay RSP1 and FunCube     ---   Limitations and specifcities:       * In SDRangel GUI the first Rx device set cannot be deleted. Conversely the server starts with no device sets and its number of device sets can be reduced to zero by as many calls as necessary to /sdrangel/deviceset with DELETE method.   * Stopping instance i.e. /sdrangel with DELETE method is a server only feature. It allows stopping the instance nicely.   * Preset import and export from/to file is a server only feature.   * Device set focus is a GUI only feature.   * The following channels are not implemented (status 501 is returned): ATV demodulator, Channel Analyzer, Channel Analyzer NG, LoRa demodulator, TCP source   * The content type returned is always application/json except in the following cases:     * An incorrect URL was specified: this document is returned as text/html with a status 400    --- 
 *
 * OpenAPI spec version: 4.0.0
 * Contact: f4exb06@gmail.com
 *
 * NOTE: This class is auto generated by the swagger code generator program.
 * https://github.com/swagger-api/swagger-codegen.git
 * Do not edit the class manually.
 */


#include "SWGSDRPlayReport_intermediateFrequencies.h"

#include "SWGHelpers.h"

#include <QJsonDocument>
#include <QJsonArray>
#include <QObject>
#include <QDebug>

namespace SWGSDRangel {

SWGSDRPlayReport_intermediateFrequencies::SWGSDRPlayReport_intermediateFrequencies(QString* json) {
    init();
    this->fromJson(*json);
}

SWGSDRPlayReport_intermediateFrequencies::SWGSDRPlayReport_intermediateFrequencies() {
    intermediate_frequency = 0;
    m_intermediate_frequency_isSet = false;
}

SWGSDRPlayReport_intermediateFrequencies::~SWGSDRPlayReport_intermediateFrequencies() {
    this->cleanup();
}

void
SWGSDRPlayReport_intermediateFrequencies::init() {
    intermediate_frequency = 0;
    m_intermediate_frequency_isSet = false;
}

void
SWGSDRPlayReport_intermediateFrequencies::cleanup() {

}

SWGSDRPlayReport_intermediateFrequencies*
SWGSDRPlayReport_intermediateFrequencies::fromJson(QString &json) {
    QByteArray array (json.toStdString().c_str());
    QJsonDocument doc = QJsonDocument::fromJson(array);
    QJsonObject jsonObject = doc.object();
    this->fromJsonObject(jsonObject);
    return this;
}

void
SWGSDRPlayReport_intermediateFrequencies::fromJsonObject(QJsonObject &pJson) {
    ::SWGSDRangel::setValue(&intermediate_frequency, pJson["intermediateFrequency"], "qint32", "");
    
}

QString
SWGSDRPlayReport_intermediateFrequencies::asJson ()
{
    QJsonObject* obj = this->asJsonObject();

    QJsonDocument doc(*obj);
    QByteArray bytes = doc.toJson();
    delete obj;
    return QString(bytes);
}

QJsonObject*
SWGSDRPlayReport_intermediateFrequencies::asJsonObject() {
    QJsonObject* obj = new QJsonObject();
    if(m_intermediate_frequency_isSet){
        obj->insert("intermediateFrequency", QJsonValue(intermediate_frequency));
    }

    return obj;
}

qint32
SWGSDRPlayReport_intermediateFrequencies::getIntermediateFrequency() {
    return intermediate_frequency;
}
void
SWGSDRPlayReport_intermediateFrequencies::setIntermediateFrequency(qint32 intermediate_frequency) {
    this->intermediate_frequency = intermediate_frequency;
    this->m_intermediate_frequency_isSet = true;
}


bool
SWGSDRPlayReport_intermediateFrequencies::isSet(){
    bool isObjectUpdated = false;
    do{
        if(m_intermediate_frequency_isSet){ isObjectUpdated = true; break;}
    }while(false);
    return isObjectUpdated;
}
}

