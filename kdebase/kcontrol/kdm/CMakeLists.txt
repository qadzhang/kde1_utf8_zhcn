add_subdirectory(doc)
add_subdirectory(pics)

include_directories(
    ${KDE1_INCLUDE_DIR}
    )

set(KDM_SRCS
main.cpp utils.cpp kdm-appear.cpp kdm-font.cpp kdm-bgnd.cpp
	kdm-users.cpp kdm-sess.cpp klangcombo.cpp kdropsite.cpp
)

set(KDM_MOC
kdm-appear.h kdm-font.h kdm-bgnd.h kdm-users.h kdm-sess.h
	klangcombo.h kdropsite.h
)
TQT3_WRAP_CPP(MOC_FILES ${KDM_MOC})

add_executable(kdmconfig ${KDM_SRCS} ${MOC_FILES})
target_link_libraries(kdmconfig
    ${TQT3_LIBRARIES}
    ${X11_LIBRARIES}
    ${KDE1_KDECORE}
    ${KDE1_KDEUI}
    ${KDE1_KFM}
    ${KDE1_KFILE}
    ${KDE1_KIMGIO}
    )

install(TARGETS kdmconfig RUNTIME DESTINATION ${KDE1_BINDIR})
