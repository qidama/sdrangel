project(demod)

add_subdirectory(demodlora)
add_subdirectory(demodam)
add_subdirectory(demodbfm)
add_subdirectory(demodnfm)
add_subdirectory(demodssb)
add_subdirectory(udpsrc)
add_subdirectory(demodwfm)
add_subdirectory(chanalyzer)
add_subdirectory(demodatv)

if(LIBDSDCC_FOUND AND LIBMBE_FOUND)
    add_subdirectory(demoddsd)
endif(LIBDSDCC_FOUND AND LIBMBE_FOUND)

find_package(FFmpeg)
if(FFMPEG_FOUND)
    add_subdirectory(demoddatv)
endif()

if (BUILD_DEBIAN)
    add_subdirectory(demoddsd)
endif (BUILD_DEBIAN)
