include_directories(
    ${KDE1_INCLUDE_DIR}
    )

add_executable(helpindex kdeindex.cpp)
target_link_libraries(helpindex
    ${KDE1_KDECORE}
    ${TQT3_LIBRARIES}
    )

install(TARGETS helpindex RUNTIME DESTINATION ${KDE1_CGIDIR})
