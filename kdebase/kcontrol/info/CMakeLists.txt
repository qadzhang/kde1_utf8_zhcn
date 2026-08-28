add_subdirectory(pics)

include_directories(
    ${KDE1_INCLUDE_DIR}
    )

set(INFO_SRCS
    main.cpp memory.cpp
)

set(INFO_MOC
    memory.h info.h
)
TQT3_WRAP_CPP(MOC_FILES ${INFO_MOC})

add_executable(kcminfo ${INFO_SRCS} ${MOC_FILES})
target_link_libraries(kcminfo
    ${TQT3_LIBRARIES}
    ${X11_LIBRARIES}
    ${KDE1_KDECORE}
    ${KDE1_KDEUI}
    )

install(TARGETS kcminfo RUNTIME DESTINATION ${KDE1_BINDIR})
