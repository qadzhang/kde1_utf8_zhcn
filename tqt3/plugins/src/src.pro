TEMPLATE = subdirs

shared {
	SUBDIRS	*= accessible codecs imageformats inputmethods sqldrivers styles
	embedded:SUBDIRS *=  gfxdrivers
}
dll {
	SUBDIRS	*= accessible codecs imageformats inputmethods sqldrivers styles
	embedded:SUBDIRS *=  gfxdrivers
}
