include_directories(
    ${KDE1_INCLUDE_DIR}
    )

set(KFMWARN_SRCS main.cpp)

TQT3_WRAP_CPP(MOC_FILES main.h)

add_executable(kfmexec ${KFMWARN_SRCS} ${MOC_FILES})
target_link_libraries(kfmexec
    ${KDE1_KDECORE}
    ${KDE1_KFM}
    ${TQT3_LIBRARIES}
    ${X11_LIBRARIES}
    )

install(TARGETS kfmexec RUNTIME DESTINATION ${KDE1_BINDIR})

