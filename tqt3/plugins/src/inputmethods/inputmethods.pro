TEMPLATE = subdirs

unix {
	!embedded:!mac:CONFIG	   += x11
}
# XIM should be enabled only for X11 platform, but following
# configuration is not working properly yet
#im:x11:SUBDIRS += xim

inputmethod:SUBDIRS += imsw-none imsw-multi simple
inputmethod:SUBDIRS += xim
