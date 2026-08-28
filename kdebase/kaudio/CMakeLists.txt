set(OSS "")
if (CMAKE_SYSTEM_NAME MATCHES "NetBSD")
	set(OSS "ossaudio")
endif()

include_directories(
    ${KDE1_INCLUDE_DIR}
    )

set(KAUDIO_SRCS kaudioserver.cpp)
set(MAUDIO_SRCS io_oss.cpp maudio.cpp sample.cpp)
set(KAUDIO_MOC democlient.h kplayaudio.h)
set(KPLAYAUDIO_SRCS kplayaudio.cpp)

TQT3_WRAP_CPP(MOC_FILES kplayaudio.h)
TQT3_WRAP_CPP(DEMO_MOC_FILES democlient.h)

add_executable(maudio ${MAUDIO_SRCS})
target_link_libraries(maudio
    ${TQT3_LIBRARIES}
    ${X11_LIBRARIES}
    ${KDE1_KDECORE}
    ${KDE1_MEDIATOOL}
    ${OSS}
    )

add_executable(kaudioserver ${KAUDIO_SRCS})
target_link_libraries(kaudioserver
    ${TQT3_LIBRARIES}
    ${X11_LIBRARIES}
    ${KDE1_KDECORE}
    ${KDE1_MEDIATOOL}
    )

if(ENABLE_COMPAT AND PULSEAUDIO_FOUND)
    target_compile_definitions(maudio PRIVATE ENABLE_PULSE)
    target_include_directories(maudio PRIVATE ${PULSEAUDIO_INCLUDE_DIR})
    target_link_libraries(maudio pulse-simple ${PULSEAUDIO_LIBRARY})
endif()

add_executable(kplayaudio ${KPLAYAUDIO_SRCS} ${MOC_FILES})
target_link_libraries(kplayaudio
    ${KDE1_KDECORE}
    ${KDE1_KDEUI}
    ${KDE1_KFILE}
    ${KDE1_KFM}
    ${KDE1_MEDIATOOL}
    ${TQT3_LIBRARIES}
    ${X11_LIBRARIES}
    )

install(TARGETS maudio kaudioserver kplayaudio RUNTIME DESTINATION ${KDE1_BINDIR})
install(FILES  maudio.html DESTINATION ${KDE1_HTMLDIR}/en/kmedia)
