add_subdirectory(kcs)

find_package(JPEG)

if(JPEG_FOUND)
    add_subdirectory(ad)
endif()
add_subdirectory(pics)

include_directories(
    ${KDE1_INCLUDE_DIR}
    )

set(DISPLAY_SRCS
backgnd.cpp display.cpp
		colorscm.cpp savescm.cpp scrnsave.cpp
		general.cpp main.cpp widgetcanvas.cpp
		fontchooser.cpp kresourceman.cpp
)

set(DISPLAY_MOC
backgnd.h display.h colorscm.h savescm.h
		scrnsave.h general.h widgetcanvas.h fontchooser.h
		kresourceman.h
)
TQT3_WRAP_CPP(MOC_FILES ${DISPLAY_MOC})

add_executable(kcmdisplay ${DISPLAY_SRCS} ${MOC_FILES})
target_link_libraries(kcmdisplay
    ${TQT3_LIBRARIES}
    ${X11_LIBRARIES}
    ${KDE1_KDECORE}
    ${KDE1_KDEUI}
    ${KDE1_KFILE}
    ${KDE1_KFM}
    ${KDE1_KIMGIO}
    )

install(TARGETS kcmdisplay RUNTIME DESTINATION ${KDE1_BINDIR})
