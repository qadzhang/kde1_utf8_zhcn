include_directories(
    ${KDE1_INCLUDE_DIR}
    )

set(KIOSLAVE_SRCS
kioslave_ipc.cpp kioslave_ipc2.cpp main.cpp xview.cpp
	ipc.cpp protocol.cpp http.cpp file.cpp ftp.cpp manage.cpp icon.cpp
	slave.cpp tar.cpp subprotocol.cpp cgi.cpp gzip.cpp ftp_proxy.cpp ../debug.cpp)

set(KIOSLAVE_MOC kioslave_ipc.h main.h http.h file.h tar.h ftp.h protocol.h subprotocol.h cgi.h gzip.h icon.h ftp_proxy.h)

TQT3_WRAP_CPP(MOC_FILES ${KIOSLAVE_MOC})

add_executable(kioslave ${KIOSLAVE_SRCS} ${MOC_FILES})
target_link_libraries(kioslave
    ${KDE1_KDECORE}
    ${KDE1_KDEUI}
    ${KDE1_KIMGIO}
    ${TQT3_LIBRARIES}
    ${X11_LIBRARIES}
    OpenSSL::SSL
    )

install(TARGETS kioslave RUNTIME DESTINATION ${KDE1_BINDIR})
