add_subdirectory(data)

include_directories(
    ${KDE1_INCLUDE_DIR}
)

set(KMINES_SRCS
    dialogs.cpp field.cpp status.cpp main.cpp
)

set(KMINES_MOC
    dialogs.h field.h status.h main.h
)

TQT3_WRAP_CPP(MOC_FILES ${KMINES_MOC})

add_executable(kmines1 ${KMINES_SRCS} ${MOC_FILES})
target_link_libraries(kmines1
    ${TQT3_LIBRARIES}
    ${X11_LIBRARIES}
    ${KDE1_KDECORE}
    ${KDE1_KDEUI}
)

install(TARGETS kmines1 RUNTIME DESTINATION ${KDE1_BINDIR})
