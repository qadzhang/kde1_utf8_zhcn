include_directories(
    ${TQT3_INCLUDE_DIRS}
    ${KDELIBS_SOURCE_DIR}/kdecore
    )

set(KFM_SRCS kfm.cpp kfmclient_ipc.cpp kfmclient_ipc2.cpp kfmipc.cpp)

set(KFM_MOC kfm.h kfmclient_ipc.h)

TQT3_WRAP_CPP(MOC_FILES ${KFM_MOC})

add_library(kfm SHARED ${KFM_SRCS} ${MOC_FILES})
set_target_properties(kfm PROPERTIES VERSION 2.0.0 SOVERSION 2)
target_link_libraries(kfm kdecore ${TQT3_LIBRARIES})
install(TARGETS kfm LIBRARY DESTINATION ${KDE1_LIBRARIES})

install(FILES kfm.h kfmclient_ipc.h kfmipc.h DESTINATION ${KDE1_INCLUDES})
