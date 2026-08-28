include_directories(
    ${KDE1_INCLUDE_DIR}
    )

set(KEYS_SRCS
main.cpp keyconfig.cpp savescm.cpp
)

set(KEYS_MOC
keyconfig.h savescm.h  
)
TQT3_WRAP_CPP(MOC_FILES ${KEYS_MOC})

add_executable(kcmkeys ${KEYS_SRCS} ${MOC_FILES})
target_link_libraries(kcmkeys
    ${TQT3_LIBRARIES}
    ${X11_LIBRARIES}
    ${KDE1_KDECORE}
    ${KDE1_KDEUI}
    )

install(TARGETS kcmkeys RUNTIME DESTINATION ${KDE1_BINDIR})
