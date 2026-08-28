include_directories(
    ${TQT3_INCLUDE_DIRS}
    ${KDELIBS_SOURCE_DIR}/kdecore
    )

set(MEDIATOOL_SRCS
    chunk.c eventcounter.c connect.c filename.c playlist.c kaudio.cpp
    )

set(MEDIATOOL_MOC
    kaudio.h
    )

TQT3_WRAP_CPP(MOC_FILES ${MEDIATOOL_MOC})

add_library(mediatool SHARED ${MEDIATOOL_SRCS} ${MOC_FILES})
set_target_properties(mediatool PROPERTIES VERSION 2.0.0 SOVERSION 2)
target_link_libraries(mediatool kdecore ${TQT3_LIBRARIES} ${X11_LIBRARIES})
install(TARGETS mediatool LIBRARY DESTINATION ${KDE1_LIBRARIES})

install(FILES mediatool.h kaudio.h DESTINATION ${KDE1_INCLUDES})
