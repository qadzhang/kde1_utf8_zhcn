// All feature and their dependencies
//
// This list is generated from $TQTDIR/src/tools/qfeatures.txt
//
// Asynchronous I/O
//#define TQT_NO_ASYNC_IO

// Bezier curves
//#define TQT_NO_BEZIER

// Buttons
//#define TQT_NO_BUTTON

// Named colors
//#define TQT_NO_COLORNAMES

// Cursors
//#define TQT_NO_CURSOR

// TQDataStream
//#define TQT_NO_DATASTREAM

// TQDate/TQTime/TQDateTime toString() and fromString()
//#define TQT_NO_DATESTRING

// Dialogs
//#define TQT_NO_DIALOG

// TQDirectPainter
//#define TQT_NO_DIRECTPAINTER

// Special widget effects (fading, scrolling)
//#define TQT_NO_EFFECTS

// Freetype font engine
//#define TQT_NO_FREETYPE

// Dither TQImage to 1-bit image
//#define TQT_NO_IMAGE_DITHER_TO_1

// TQImage::createHeuristicMask()
//#define TQT_NO_IMAGE_HEURISTIC_MASK

// TQImage mirroring
//#define TQT_NO_IMAGE_MIRROR

// Smooth TQImage scaling
//#define TQT_NO_IMAGE_SMOOTHSCALE

// TrueColor TQImage
//#define TQT_NO_IMAGE_TRUECOLOR

// Automatic widget layout
//#define TQT_NO_LAYOUT

// Networking
//#define TQT_NO_NETWORK

// Palettes
//#define TQT_NO_PALETTE

// Alpha-blended cursor
//#define TQT_NO_QWS_ALPHA_CURSOR

// 1-bit monochrome
//#define TQT_NO_QWS_DEPTH_1

// 15 or 16-bit color
//#define TQT_NO_QWS_DEPTH_16

// 24-bit color
//#define TQT_NO_QWS_DEPTH_24

// 32-bit color
//#define TQT_NO_QWS_DEPTH_32

// 4-bit grayscale
//#define TQT_NO_QWS_DEPTH_4

// 8-bit color
//#define TQT_NO_QWS_DEPTH_8

// 8-bit grayscale
//#define TQT_NO_QWS_DEPTH_8GRAYSCALE

// Favour code size over graphics speed
//#define TQT_NO_QWS_GFX_SPEED

// Console keyboard
//#define TQT_NO_QWS_KEYBOARD

// Linux framebuffer
//#define TQT_NO_QWS_LINUXFB

// Mach64 acceleration
//#define TQT_NO_QWS_MACH64

// Window Manager
//#define TQT_NO_QWS_MANAGER

// Matrox MGA acceleration
//#define TQT_NO_QWS_MATROX

// Autodetecting mouse driver
//#define TQT_NO_QWS_MOUSE_AUTO

// Non-autodetecting mouse driver
//#define TQT_NO_QWS_MOUSE_MANUAL

// TQt/Embedded window system properties.
//#define TQT_NO_QWS_PROPERTIES

// Repeater display
//#define TQT_NO_QWS_REPEATER

// Saving of fonts
//#define TQT_NO_QWS_SAVEFONTS

// Shadow frame buffer
//#define TQT_NO_QWS_SHADOWFB

// Virtual frame buffer
//#define TQT_NO_QWS_VFB

// 4-bit VGA
//#define TQT_NO_QWS_VGA_16

// Voodoo3 acceleration
//#define TQT_NO_QWS_VOODOO3

// Range-control widgets
//#define TQT_NO_RANGECONTROL

// Regular expression capture
//#define TQT_NO_REGEXP

// TQSignalMapper
//#define TQT_NO_SIGNALMAPPER

// Playing sounds
//#define TQT_NO_SOUND

// Standard template library compatiblity
//#define TQT_NO_STL

// TQStringList
//#define TQT_NO_STRINGLIST

// Character set conversions
//#define TQT_NO_TEXTCODEC

// TQTextStream
//#define TQT_NO_TEXTSTREAM

// Unicode property tables
//#define TQT_NO_UNICODETABLES

// Input validators
//#define TQT_NO_VALIDATOR

// TQVariant
//#define TQT_NO_VARIANT

// Wheel-mouse events
//#define TQT_NO_WHEELEVENT

// TQWMatrix
//#define TQT_NO_WMATRIX

// Non-Unicode text conversions
#if !defined(TQT_NO_CODECS) && (defined(TQT_NO_TEXTCODEC))
#define TQT_NO_CODECS
#endif

// TQCop IPC
#if !defined(TQT_NO_COP) && (defined(TQT_NO_DATASTREAM))
#define TQT_NO_COP
#endif

// TQFontDatabase
#if !defined(TQT_NO_FONTDATABASE) && (defined(TQT_NO_STRINGLIST))
#define TQT_NO_FONTDATABASE
#endif

// Image formats
#if !defined(TQT_NO_IMAGEIO) && (defined(TQT_NO_REGEXP))
#define TQT_NO_IMAGEIO
#endif

// 16-bit TQImage
#if !defined(TQT_NO_IMAGE_16_BIT) && (defined(TQT_NO_IMAGE_TRUECOLOR))
#define TQT_NO_IMAGE_16_BIT
#endif

// Image file text strings
#if !defined(TQT_NO_IMAGE_TEXT) && (defined(TQT_NO_STRINGLIST))
#define TQT_NO_IMAGE_TEXT
#endif

// Shared library wrapper
#if !defined(TQT_NO_LIBRARY) && (defined(TQT_NO_REGEXP))
#define TQT_NO_LIBRARY
#endif

// Pixmap transformations
#if !defined(TQT_NO_PIXMAP_TRANSFORMATION) && (defined(TQT_NO_WMATRIX))
#define TQT_NO_PIXMAP_TRANSFORMATION
#endif

// Convert UUID to/from string
#if !defined(TQT_NO_QUUID_STRING) && (defined(TQT_NO_STRINGLIST))
#define TQT_NO_QUUID_STRING
#endif

// The "BeOS" style
#if !defined(TQT_NO_QWS_BEOS_WM_STYLE) && (defined(TQT_NO_QWS_MANAGER))
#define TQT_NO_QWS_BEOS_WM_STYLE
#endif

// Visible cursor
#if !defined(TQT_NO_QWS_CURSOR) && (defined(TQT_NO_CURSOR))
#define TQT_NO_QWS_CURSOR
#endif

// 32-bit color, BGR order
#if !defined(TQT_NO_QWS_DEPTH_32_BGR) && (defined(TQT_NO_QWS_DEPTH_32))
#define TQT_NO_QWS_DEPTH_32_BGR
#endif

// The "Hydro" style
#if !defined(TQT_NO_QWS_HYDRO_WM_STYLE) && (defined(TQT_NO_QWS_MANAGER))
#define TQT_NO_QWS_HYDRO_WM_STYLE
#endif

// Keyboard for SHARP Zaurus SL5xxx devices
#if !defined(TQT_NO_QWS_KBD_SHARP) && (defined(TQT_NO_QWS_KEYBOARD))
#define TQT_NO_QWS_KBD_SHARP
#endif

// The "KDE2" style
#if !defined(TQT_NO_QWS_KDE2_WM_STYLE) && (defined(TQT_NO_QWS_MANAGER))
#define TQT_NO_QWS_KDE2_WM_STYLE
#endif

// The "KDE" style
#if !defined(TQT_NO_QWS_KDE_WM_STYLE) && (defined(TQT_NO_QWS_MANAGER))
#define TQT_NO_QWS_KDE_WM_STYLE
#endif

// Multi-process architecture
#if !defined(TQT_NO_QWS_MULTIPROCESS) && (defined(TQT_NO_NETWORK))
#define TQT_NO_QWS_MULTIPROCESS
#endif

// Transformed frame buffer
#if !defined(TQT_NO_QWS_TRANSFORMED) && (defined(TQT_NO_QWS_LINUXFB))
#define TQT_NO_QWS_TRANSFORMED
#endif

// Remote frame buffer (VNC)
#if !defined(TQT_NO_QWS_VNC) && (defined(TQT_NO_NETWORK))
#define TQT_NO_QWS_VNC
#endif

// The "Windows" style
#if !defined(TQT_NO_QWS_WINDOWS_WM_STYLE) && (defined(TQT_NO_QWS_MANAGER))
#define TQT_NO_QWS_WINDOWS_WM_STYLE
#endif

// Regular expression anchors
#if !defined(TQT_NO_REGEXP_ANCHOR_ALT) && (defined(TQT_NO_REGEXP))
#define TQT_NO_REGEXP_ANCHOR_ALT
#endif

// Regular expression back-reference
#if !defined(TQT_NO_REGEXP_BACKREF) && (defined(TQT_NO_REGEXP))
#define TQT_NO_REGEXP_BACKREF
#endif

// Regular expression character-class
#if !defined(TQT_NO_REGEXP_CCLASS) && (defined(TQT_NO_REGEXP))
#define TQT_NO_REGEXP_CCLASS
#endif

// Regular expression escape
#if !defined(TQT_NO_REGEXP_ESCAPE) && (defined(TQT_NO_REGEXP))
#define TQT_NO_REGEXP_ESCAPE
#endif

// Regular expression interval
#if !defined(TQT_NO_REGEXP_INTERVAL) && (defined(TQT_NO_REGEXP))
#define TQT_NO_REGEXP_INTERVAL
#endif

// Regular expression lookahead
#if !defined(TQT_NO_REGEXP_LOOKAHEAD) && (defined(TQT_NO_REGEXP))
#define TQT_NO_REGEXP_LOOKAHEAD
#endif

// Regular expression optimization
#if !defined(TQT_NO_REGEXP_OPTIM) && (defined(TQT_NO_REGEXP))
#define TQT_NO_REGEXP_OPTIM
#endif

// Regular expression wildcard
#if !defined(TQT_NO_REGEXP_WILDCARD) && (defined(TQT_NO_REGEXP))
#define TQT_NO_REGEXP_WILDCARD
#endif

// Semi-modal dialogs
#if !defined(TQT_NO_SEMIMODAL) && (defined(TQT_NO_DIALOG))
#define TQT_NO_SEMIMODAL
#endif

// Session management
#if !defined(TQT_NO_SESSIONMANAGER) && (defined(TQT_NO_STRINGLIST))
#define TQT_NO_SESSIONMANAGER
#endif

// TQString::sprintf()
#if !defined(TQT_NO_SPRINTF) && (defined(TQT_NO_REGEXP))
#define TQT_NO_SPRINTF
#endif

// Scaling and rotation
#if !defined(TQT_NO_TRANSFORMATIONS) && (defined(TQT_NO_WMATRIX))
#define TQT_NO_TRANSFORMATIONS
#endif

// Translations via TQObject::tr()
#if !defined(TQT_NO_TRANSLATION) && (defined(TQT_NO_DATASTREAM))
#define TQT_NO_TRANSLATION
#endif

// Window icon and caption
#if !defined(TQT_NO_WIDGET_TOPEXTRA) && (defined(TQT_NO_IMAGE_HEURISTIC_MASK))
#define TQT_NO_WIDGET_TOPEXTRA
#endif

// Keyboard accelerators and shortcuts
#if !defined(TQT_NO_ACCEL) && (defined(TQT_NO_SPRINTF))
#define TQT_NO_ACCEL
#endif

// Asynchronous image I/O
#if !defined(TQT_NO_ASYNC_IMAGE_IO) && (defined(TQT_NO_IMAGEIO))
#define TQT_NO_ASYNC_IMAGE_IO
#endif

// BDF font files
#if !defined(TQT_NO_BDF) && (defined(TQT_NO_TEXTSTREAM) || defined(TQT_NO_STRINGLIST))
#define TQT_NO_BDF
#endif

// TQDir
#if !defined(TQT_NO_DIR) && (defined(TQT_NO_STRINGLIST) || defined(TQT_NO_REGEXP))
#define TQT_NO_DIR
#endif

// JPEG image I/O
#if !defined(TQT_NO_IMAGEIO_JPEG) && (defined(TQT_NO_IMAGEIO))
#define TQT_NO_IMAGEIO_JPEG
#endif

// MNG image I/O
#if !defined(TQT_NO_IMAGEIO_MNG) && (defined(TQT_NO_IMAGEIO))
#define TQT_NO_IMAGEIO_MNG
#endif

// PNG image I/O
#if !defined(TQT_NO_IMAGEIO_PNG) && (defined(TQT_NO_IMAGEIO))
#define TQT_NO_IMAGEIO_PNG
#endif

// PPM image I/O
#if !defined(TQT_NO_IMAGEIO_PPM) && (defined(TQT_NO_IMAGEIO))
#define TQT_NO_IMAGEIO_PPM
#endif

// XBM image I/O
#if !defined(TQT_NO_IMAGEIO_XBM) && (defined(TQT_NO_IMAGEIO))
#define TQT_NO_IMAGEIO_XBM
#endif

// Image transformations
#if !defined(TQT_NO_IMAGE_TRANSFORMATION) && (defined(TQT_NO_PIXMAP_TRANSFORMATION))
#define TQT_NO_IMAGE_TRANSFORMATION
#endif

// External process invocation.
#if !defined(TQT_NO_PROCESS) && (defined(TQT_NO_STRINGLIST) || defined(TQT_NO_REGEXP))
#define TQT_NO_PROCESS
#endif

// Regular expression capture
#if !defined(TQT_NO_REGEXP_CAPTURE) && (defined(TQT_NO_REGEXP) || defined(TQT_NO_STRINGLIST))
#define TQT_NO_REGEXP_CAPTURE
#endif

// Splash screen widget
#if !defined(TQT_NO_SPLASHSCREEN) && (defined(TQT_NO_IMAGEIO))
#define TQT_NO_SPLASHSCREEN
#endif

// Template classes in TQVariant
#if !defined(TQT_NO_TEMPLATE_VARIANT) && (defined(TQT_NO_VARIANT) || defined(TQT_NO_STRINGLIST))
#define TQT_NO_TEMPLATE_VARIANT
#endif

// Month and day names in dates
#if !defined(TQT_NO_TEXTDATE) && (defined(TQT_NO_STRINGLIST) || defined(TQT_NO_DATESTRING))
#define TQT_NO_TEXTDATE
#endif

// Drawing utility functions
#if !defined(TQT_NO_DRAWUTIL) && (defined(TQT_NO_SPRINTF) || defined(TQT_NO_PALETTE))
#define TQT_NO_DRAWUTIL
#endif

// BMP image I/O
#if !defined(TQT_NO_IMAGEIO_BMP) && (defined(TQT_NO_IMAGEIO) || defined(TQT_NO_DATASTREAM))
#define TQT_NO_IMAGEIO_BMP
#endif

// TQPicture
#if !defined(TQT_NO_PICTURE) && (defined(TQT_NO_DATASTREAM) || defined(TQT_NO_IMAGEIO))
#define TQT_NO_PICTURE
#endif

// Translations via TQObject::trUtf8()
#if !defined(TQT_NO_TRANSLATION_UTF8) && (defined(TQT_NO_TRANSLATION) || defined(TQT_NO_TEXTCODEC))
#define TQT_NO_TRANSLATION_UTF8
#endif

// URL parser
#if !defined(TQT_NO_URL) && (defined(TQT_NO_DIR))
#define TQT_NO_URL
#endif

// Animated images
#if !defined(TQT_NO_MOVIE) && (defined(TQT_NO_ASYNC_IO) || defined(TQT_NO_ASYNC_IMAGE_IO))
#define TQT_NO_MOVIE
#endif

// TQStyle
#if !defined(TQT_NO_STYLE) && (defined(TQT_NO_DRAWUTIL))
#define TQT_NO_STYLE
#endif

// DNS
#if !defined(TQT_NO_DNS) && (defined(TQT_NO_NETWORK) || defined(TQT_NO_STRINGLIST) || defined(TQT_NO_TEXTSTREAM) || defined(TQT_NO_SPRINTF))
#define TQT_NO_DNS
#endif

// Framed widgets
#if !defined(TQT_NO_FRAME) && (defined(TQT_NO_STYLE))
#define TQT_NO_FRAME
#endif

// TQIconSet
#if !defined(TQT_NO_ICONSET) && (defined(TQT_NO_IMAGEIO) || defined(TQT_NO_IMAGE_SMOOTHSCALE) || defined(TQT_NO_PALETTE) || defined(TQT_NO_IMAGE_HEURISTIC_MASK))
#define TQT_NO_ICONSET
#endif

// XPM image I/O
#if !defined(TQT_NO_IMAGEIO_XPM) && (defined(TQT_NO_IMAGEIO) || defined(TQT_NO_SPRINTF) || defined(TQT_NO_TEXTSTREAM))
#define TQT_NO_IMAGEIO_XPM
#endif

// Network file access
#if !defined(TQT_NO_NETWORKPROTOCOL) && (defined(TQT_NO_TEXTCODEC) || defined(TQT_NO_URL))
#define TQT_NO_NETWORKPROTOCOL
#endif

// TQSizeGrip
#if !defined(TQT_NO_SIZEGRIP) && (defined(TQT_NO_STYLE))
#define TQT_NO_SIZEGRIP
#endif

// Motif style
#if !defined(TQT_NO_STYLE_MOTIF) && (defined(TQT_NO_STYLE))
#define TQT_NO_STYLE_MOTIF
#endif

// Windows style
#if !defined(TQT_NO_STYLE_WINDOWS) && (defined(TQT_NO_STYLE))
#define TQT_NO_STYLE_WINDOWS
#endif

// Internal titlebar widget
#if !defined(TQT_NO_TITLEBAR) && (defined(TQT_NO_STYLE))
#define TQT_NO_TITLEBAR
#endif

// XML
#if !defined(TQT_NO_XML) && (defined(TQT_NO_TEXTSTREAM) || defined(TQT_NO_TEXTCODEC) || defined(TQT_NO_REGEXP_CAPTURE))
#define TQT_NO_XML
#endif

// Check-boxes
#if !defined(TQT_NO_CHECKBOX) && (defined(TQT_NO_BUTTON) || defined(TQT_NO_STYLE))
#define TQT_NO_CHECKBOX
#endif

// Dials
#if !defined(TQT_NO_DIAL) && (defined(TQT_NO_RANGECONTROL) || defined(TQT_NO_STYLE))
#define TQT_NO_DIAL
#endif

// TQLabel
#if !defined(TQT_NO_LABEL) && (defined(TQT_NO_FRAME))
#define TQT_NO_LABEL
#endif

// TQLCDNumber
#if !defined(TQT_NO_LCDNUMBER) && (defined(TQT_NO_FRAME))
#define TQT_NO_LCDNUMBER
#endif

// Single-line edits
#if !defined(TQT_NO_LINEEDIT) && (defined(TQT_NO_FRAME))
#define TQT_NO_LINEEDIT
#endif

// MIME
#if !defined(TQT_NO_MIME) && (defined(TQT_NO_DIR) || defined(TQT_NO_IMAGEIO) || defined(TQT_NO_TEXTCODEC))
#define TQT_NO_MIME
#endif

// Progress bars
#if !defined(TQT_NO_PROGRESSBAR) && (defined(TQT_NO_FRAME))
#define TQT_NO_PROGRESSBAR
#endif

// Push-buttons
#if !defined(TQT_NO_PUSHBUTTON) && (defined(TQT_NO_BUTTON) || defined(TQT_NO_STYLE))
#define TQT_NO_PUSHBUTTON
#endif

// Radio-buttons
#if !defined(TQT_NO_RADIOBUTTON) && (defined(TQT_NO_BUTTON) || defined(TQT_NO_STYLE))
#define TQT_NO_RADIOBUTTON
#endif

// Internal resize handler
#if !defined(TQT_NO_RESIZEHANDLER) && (defined(TQT_NO_FRAME))
#define TQT_NO_RESIZEHANDLER
#endif

// Scroll bars
#if !defined(TQT_NO_SCROLLBAR) && (defined(TQT_NO_RANGECONTROL) || defined(TQT_NO_STYLE))
#define TQT_NO_SCROLLBAR
#endif

// Sliders
#if !defined(TQT_NO_SLIDER) && (defined(TQT_NO_RANGECONTROL) || defined(TQT_NO_STYLE))
#define TQT_NO_SLIDER
#endif

// Spinbox control widget
#if !defined(TQT_NO_SPINWIDGET) && (defined(TQT_NO_FRAME))
#define TQT_NO_SPINWIDGET
#endif

// Status bars
#if !defined(TQT_NO_STATUSBAR) && (defined(TQT_NO_LAYOUT) || defined(TQT_NO_STYLE))
#define TQT_NO_STATUSBAR
#endif

// Compact Windows style
#if !defined(TQT_NO_STYLE_COMPACT) && (defined(TQT_NO_STYLE_WINDOWS))
#define TQT_NO_STYLE_COMPACT
#endif

// Interlace-friendly style
#if !defined(TQT_NO_STYLE_INTERLACE) && (defined(TQT_NO_STYLE_MOTIF))
#define TQT_NO_STYLE_INTERLACE
#endif

// Platinum style
#if !defined(TQT_NO_STYLE_PLATINUM) && (defined(TQT_NO_STYLE_WINDOWS))
#define TQT_NO_STYLE_PLATINUM
#endif

// Widget stacks
#if !defined(TQT_NO_WIDGETSTACK) && (defined(TQT_NO_FRAME))
#define TQT_NO_WIDGETSTACK
#endif

// Grid layout widgets
#if !defined(TQT_NO_GRID) && (defined(TQT_NO_LAYOUT) || defined(TQT_NO_FRAME))
#define TQT_NO_GRID
#endif

// Group boxes
#if !defined(TQT_NO_GROUPBOX) && (defined(TQT_NO_FRAME) || defined(TQT_NO_LAYOUT))
#define TQT_NO_GROUPBOX
#endif

// Horizontal box layout widgets
#if !defined(TQT_NO_HBOX) && (defined(TQT_NO_LAYOUT) || defined(TQT_NO_FRAME))
#define TQT_NO_HBOX
#endif

// Menu-oriented widgets
#if !defined(TQT_NO_MENUDATA) && (defined(TQT_NO_ICONSET) || defined(TQT_NO_VARIANT))
#define TQT_NO_MENUDATA
#endif

// Persistent application settings
#if !defined(TQT_NO_SETTINGS) && (defined(TQT_NO_DIR) || defined(TQT_NO_TEXTSTREAM) || defined(TQT_NO_REGEXP_CAPTURE))
#define TQT_NO_SETTINGS
#endif

// Splitters
#if !defined(TQT_NO_SPLITTER) && (defined(TQT_NO_FRAME) || defined(TQT_NO_LAYOUT))
#define TQT_NO_SPLITTER
#endif

// Table-like widgets
#if !defined(TQT_NO_TABLEVIEW) && (defined(TQT_NO_SCROLLBAR))
#define TQT_NO_TABLEVIEW
#endif

// Tool tips
#if !defined(TQT_NO_TOOLTIP) && (defined(TQT_NO_LABEL))
#define TQT_NO_TOOLTIP
#endif

// Cut and paste
#if !defined(TQT_NO_CLIPBOARD) && (defined(TQT_NO_QWS_PROPERTIES) || defined(TQT_NO_MIME))
#define TQT_NO_CLIPBOARD
#endif

// Horizontal group boxes
#if !defined(TQT_NO_HGROUPBOX) && (defined(TQT_NO_GROUPBOX))
#define TQT_NO_HGROUPBOX
#endif

// Properties
#if !defined(TQT_NO_PROPERTIES) && (defined(TQT_NO_VARIANT) || defined(TQT_NO_STRINGLIST) || defined(TQT_NO_ICONSET))
#define TQT_NO_PROPERTIES
#endif

// RichText (HTML) display
#if !defined(TQT_NO_RICHTEXT) && (defined(TQT_NO_STYLE) || defined(TQT_NO_LAYOUT) || defined(TQT_NO_STRINGLIST) || defined(TQT_NO_TEXTSTREAM))
#define TQT_NO_RICHTEXT
#endif

// SQL classes
#if !defined(TQT_NO_SQL) && (defined(TQT_NO_STRINGLIST) || defined(TQT_NO_REGEXP_CAPTURE) || defined(TQT_NO_VARIANT) || defined(TQT_NO_SPRINTF) || defined(TQT_NO_DATESTRING))
#define TQT_NO_SQL
#endif

// CDE style
#if !defined(TQT_NO_STYLE_CDE) && (defined(TQT_NO_STYLE_MOTIF) || defined(TQT_NO_TRANSFORMATIONS))
#define TQT_NO_STYLE_CDE
#endif

// Vertical box layout widgets
#if !defined(TQT_NO_VBOX) && (defined(TQT_NO_HBOX))
#define TQT_NO_VBOX
#endif

// Button groups
#if !defined(TQT_NO_BUTTONGROUP) && (defined(TQT_NO_GROUPBOX) || defined(TQT_NO_BUTTON))
#define TQT_NO_BUTTONGROUP
#endif

// Complex scripts (eg. BiDi)
#if !defined(TQT_NO_COMPLEXTEXT) && (defined(TQT_NO_RICHTEXT))
#define TQT_NO_COMPLEXTEXT
#endif

// Cut and paste non-text
#if !defined(TQT_NO_MIMECLIPBOARD) && (defined(TQT_NO_CLIPBOARD))
#define TQT_NO_MIMECLIPBOARD
#endif

// Printing
#if !defined(TQT_NO_PRINTER) && (defined(TQT_NO_TEXTSTREAM) || defined(TQT_NO_SPRINTF) || defined(TQT_NO_FONTDATABASE) || defined(TQT_NO_DATESTRING) || defined(TQT_NO_REGEXP_CAPTURE))
#define TQT_NO_PRINTER
#endif

// Aqua style
#if !defined(TQT_NO_STYLE_AQUA) && (defined(TQT_NO_STYLE_WINDOWS) || defined(TQT_NO_IMAGE_TRANSFORMATION))
#define TQT_NO_STYLE_AQUA
#endif

// Vertical group boxes
#if !defined(TQT_NO_VGROUPBOX) && (defined(TQT_NO_HGROUPBOX))
#define TQT_NO_VGROUPBOX
#endif

// Horizontal button groups
#if !defined(TQT_NO_HBUTTONGROUP) && (defined(TQT_NO_BUTTONGROUP))
#define TQT_NO_HBUTTONGROUP
#endif

// Server to play sound
#if !defined(TQT_NO_QWS_SOUNDSERVER) && (defined(TQT_NO_SOUND) || defined(TQT_NO_DIR) || defined(TQT_NO_DNS))
#define TQT_NO_QWS_SOUNDSERVER
#endif

// TQHeader
#if !defined(TQT_NO_HEADER) && (defined(TQT_NO_STYLE) || defined(TQT_NO_ICONSET))
#define TQT_NO_HEADER
#endif

// Vertical button groups
#if !defined(TQT_NO_VBUTTONGROUP) && (defined(TQT_NO_HBUTTONGROUP))
#define TQT_NO_VBUTTONGROUP
#endif

// Hebrew Codec
#if !defined(TQT_NO_CODEC_HEBREW) && (defined(TQT_NO_CODECS) || defined(TQT_NO_COMPLEXTEXT))
#define TQT_NO_CODEC_HEBREW
#endif

// HTTP file access
#if !defined(TQT_NO_NETWORKPROTOCOL_HTTP) && (defined(TQT_NO_NETWORKPROTOCOL) || defined(TQT_NO_DNS))
#define TQT_NO_NETWORKPROTOCOL_HTTP
#endif

// Tool-buttons
#if !defined(TQT_NO_TOOLBUTTON) && (defined(TQT_NO_BUTTON) || defined(TQT_NO_ICONSET) || defined(TQT_NO_STYLE))
#define TQT_NO_TOOLBUTTON
#endif

// Big Codecs (eg. CJK)
#if !defined(TQT_NO_BIG_CODECS) && (defined(TQT_NO_CODEC_HEBREW))
#define TQT_NO_BIG_CODECS
#endif

// Experimental internal class
#if !defined(TQT_NO_DIALOGBUTTONS) && (defined(TQT_NO_LAYOUT) || defined(TQT_NO_STYLE) || defined(TQT_NO_PUSHBUTTON))
#define TQT_NO_DIALOGBUTTONS
#endif

// Document Object Model
#if !defined(TQT_NO_DOM) && (defined(TQT_NO_XML) || defined(TQT_NO_MIME))
#define TQT_NO_DOM
#endif

// Scrollable view widgets
#if !defined(TQT_NO_SCROLLVIEW) && (defined(TQT_NO_SCROLLBAR) || defined(TQT_NO_FRAME))
#define TQT_NO_SCROLLVIEW
#endif

// Tab-bars
#if !defined(TQT_NO_TABBAR) && (defined(TQT_NO_TOOLBUTTON))
#define TQT_NO_TABBAR
#endif

// Drag and drop
#if !defined(TQT_NO_DRAGANDDROP) && (defined(TQT_NO_MIME) || defined(TQT_NO_QWS_PROPERTIES) || defined(TQT_NO_IMAGEIO_XPM))
#define TQT_NO_DRAGANDDROP
#endif

// TQGridView
#if !defined(TQT_NO_GRIDVIEW) && (defined(TQT_NO_SCROLLVIEW))
#define TQT_NO_GRIDVIEW
#endif

// Popup-menus
#if !defined(TQT_NO_POPUPMENU) && (defined(TQT_NO_MENUDATA) || defined(TQT_NO_FRAME))
#define TQT_NO_POPUPMENU
#endif

// TQCanvas
#if !defined(TQT_NO_CANVAS) && (defined(TQT_NO_SCROLLVIEW) || defined(TQT_NO_BEZIER))
#define TQT_NO_CANVAS
#endif

// Dynamic module linking
#if !defined(TQT_NO_COMPONENT) && (defined(TQT_NO_QUUID_STRING) || defined(TQT_NO_SETTINGS) || defined(TQT_NO_SPRINTF) || defined(TQT_NO_LIBRARY) || defined(TQT_NO_DATESTRING))
#define TQT_NO_COMPONENT
#endif

// TQListBox
#if !defined(TQT_NO_LISTBOX) && (defined(TQT_NO_SCROLLVIEW) || defined(TQT_NO_STRINGLIST))
#define TQT_NO_LISTBOX
#endif

// Menu bars
#if !defined(TQT_NO_MENUBAR) && (defined(TQT_NO_POPUPMENU))
#define TQT_NO_MENUBAR
#endif

// TQMessageBox
#if !defined(TQT_NO_MESSAGEBOX) && (defined(TQT_NO_DIALOG) || defined(TQT_NO_PUSHBUTTON) || defined(TQT_NO_LABEL))
#define TQT_NO_MESSAGEBOX
#endif

// FTP file access
#if !defined(TQT_NO_NETWORKPROTOCOL_FTP) && (defined(TQT_NO_NETWORKPROTOCOL) || defined(TQT_NO_DNS) || defined(TQT_NO_TEXTDATE))
#define TQT_NO_NETWORKPROTOCOL_FTP
#endif

// Spin boxes
#if !defined(TQT_NO_SPINBOX) && (defined(TQT_NO_RANGECONTROL) || defined(TQT_NO_SPINWIDGET) || defined(TQT_NO_LINEEDIT) || defined(TQT_NO_VALIDATOR))
#define TQT_NO_SPINBOX
#endif

// RichText (HTML) tables and images
#if !defined(TQT_NO_TEXTCUSTOMITEM) && (defined(TQT_NO_RICHTEXT) || defined(TQT_NO_MIME))
#define TQT_NO_TEXTCUSTOMITEM
#endif

// TQDateTimeEdit
#if !defined(TQT_NO_DATETIMEEDIT) && (defined(TQT_NO_RICHTEXT) || defined(TQT_NO_SPINWIDGET) || defined(TQT_NO_DATESTRING))
#define TQT_NO_DATETIMEEDIT
#endif

// TQTextCodecPlugin
#if !defined(TQT_NO_TEXTCODECPLUGIN) && (defined(TQT_NO_COMPONENT) || defined(TQT_NO_TEXTCODEC))
#define TQT_NO_TEXTCODECPLUGIN
#endif

// TQImageFormatPlugin
#if !defined(TQT_NO_IMAGEFORMATPLUGIN) && (defined(TQT_NO_COMPONENT) || defined(TQT_NO_IMAGEIO))
#define TQT_NO_IMAGEFORMATPLUGIN
#endif

// TQSqlForm
#if !defined(TQT_NO_SQL_FORM) && (defined(TQT_NO_SQL) || defined(TQT_NO_PROPERTIES))
#define TQT_NO_SQL_FORM
#endif

// Scalable Vector Graphics (SVG)
#if !defined(TQT_NO_SVG) && (defined(TQT_NO_DOM) || defined(TQT_NO_TRANSFORMATIONS) || defined(TQT_NO_SPRINTF))
#define TQT_NO_SVG
#endif

// TQIconView
#if !defined(TQT_NO_ICONVIEW) && (defined(TQT_NO_SCROLLVIEW) || defined(TQT_NO_IMAGEIO_XPM) || defined(TQT_NO_IMAGE_HEURISTIC_MASK))
#define TQT_NO_ICONVIEW
#endif

// Tab widgets
#if !defined(TQT_NO_TABWIDGET) && (defined(TQT_NO_TABBAR) || defined(TQT_NO_WIDGETSTACK))
#define TQT_NO_TABWIDGET
#endif

// "What's this" help
#if !defined(TQT_NO_WHATSTHIS) && (defined(TQT_NO_TOOLTIP) || defined(TQT_NO_TOOLBUTTON))
#define TQT_NO_WHATSTHIS
#endif

// TQWidgetPlugin
#if !defined(TQT_NO_WIDGETPLUGIN) && (defined(TQT_NO_COMPONENT) || defined(TQT_NO_ICONSET))
#define TQT_NO_WIDGETPLUGIN
#endif

// TQProgressDialog
#if !defined(TQT_NO_PROGRESSDIALOG) && (defined(TQT_NO_SEMIMODAL) || defined(TQT_NO_LABEL) || defined(TQT_NO_PUSHBUTTON) || defined(TQT_NO_PROGRESSBAR))
#define TQT_NO_PROGRESSDIALOG
#endif

// Motif-plus style
#if !defined(TQT_NO_STYLE_MOTIFPLUS) && (defined(TQT_NO_STYLE_MOTIF) || defined(TQT_NO_TRANSFORMATIONS) || defined(TQT_NO_BUTTON) || defined(TQT_NO_SCROLLBAR) || defined(TQT_NO_SLIDER))
#define TQT_NO_STYLE_MOTIFPLUS
#endif

// Rich text edit
#if !defined(TQT_NO_TEXTEDIT) && (defined(TQT_NO_RICHTEXT) || defined(TQT_NO_SCROLLVIEW))
#define TQT_NO_TEXTEDIT
#endif

// TQWizard
#if !defined(TQT_NO_WIZARD) && (defined(TQT_NO_DIALOG) || defined(TQT_NO_WIDGETSTACK) || defined(TQT_NO_PUSHBUTTON) || defined(TQT_NO_LAYOUT) || defined(TQT_NO_LABEL))
#define TQT_NO_WIZARD
#endif

// Multi-line edits
#if !defined(TQT_NO_MULTILINEEDIT) && (defined(TQT_NO_TEXTEDIT))
#define TQT_NO_MULTILINEEDIT
#endif

// Rich text syntax highlighting
#if !defined(TQT_NO_SYNTAXHIGHLIGHTER) && (defined(TQT_NO_TEXTEDIT))
#define TQT_NO_SYNTAXHIGHLIGHTER
#endif

// TQTextView
#if !defined(TQT_NO_TEXTVIEW) && (defined(TQT_NO_TEXTEDIT))
#define TQT_NO_TEXTVIEW
#endif

// TQTabDialog
#if !defined(TQT_NO_TABDIALOG) && (defined(TQT_NO_DIALOG) || defined(TQT_NO_PUSHBUTTON) || defined(TQT_NO_LAYOUT) || defined(TQT_NO_TABWIDGET))
#define TQT_NO_TABDIALOG
#endif

// TQTextBrowser
#if !defined(TQT_NO_TEXTBROWSER) && (defined(TQT_NO_TEXTVIEW) || defined(TQT_NO_MIME))
#define TQT_NO_TEXTBROWSER
#endif

// TQListView
#if !defined(TQT_NO_LISTVIEW) && (defined(TQT_NO_SCROLLVIEW) || defined(TQT_NO_HEADER) || defined(TQT_NO_LINEEDIT))
#define TQT_NO_LISTVIEW
#endif

// TQComboBox
#if !defined(TQT_NO_COMBOBOX) && (defined(TQT_NO_LISTBOX) || defined(TQT_NO_LINEEDIT) || defined(TQT_NO_POPUPMENU))
#define TQT_NO_COMBOBOX
#endif

// TQColorDialog
#if !defined(TQT_NO_COLORDIALOG) && (defined(TQT_NO_DIALOG) || defined(TQT_NO_LABEL) || defined(TQT_NO_PUSHBUTTON) || defined(TQT_NO_LINEEDIT) || defined(TQT_NO_VALIDATOR) || defined(TQT_NO_GRIDVIEW) || defined(TQT_NO_LAYOUT))
#define TQT_NO_COLORDIALOG
#endif

// Tool box
#if !defined(TQT_NO_TOOLBOX) && (defined(TQT_NO_ICONSET) || defined(TQT_NO_SCROLLVIEW) || defined(TQT_NO_TOOLTIP) || defined(TQT_NO_LAYOUT) || defined(TQT_NO_TOOLBUTTON))
#define TQT_NO_TOOLBOX
#endif

// SGI style
#if !defined(TQT_NO_STYLE_SGI) && (defined(TQT_NO_STYLE_MOTIF) || defined(TQT_NO_TRANSFORMATIONS) || defined(TQT_NO_BUTTON) || defined(TQT_NO_SCROLLBAR) || defined(TQT_NO_SLIDER) || defined(TQT_NO_LINEEDIT) || defined(TQT_NO_MENUBAR))
#define TQT_NO_STYLE_SGI
#endif

// TQErrorMessage
#if !defined(TQT_NO_ERRORMESSAGE) && (defined(TQT_NO_DIALOG) || defined(TQT_NO_PUSHBUTTON) || defined(TQT_NO_LABEL) || defined(TQT_NO_CHECKBOX) || defined(TQT_NO_TEXTVIEW))
#define TQT_NO_ERRORMESSAGE
#endif

// Main-windows
#if !defined(TQT_NO_MAINWINDOW) && (defined(TQT_NO_STRINGLIST) || defined(TQT_NO_POPUPMENU) || defined(TQT_NO_TITLEBAR) || defined(TQT_NO_RESIZEHANDLER) || defined(TQT_NO_TOOLBUTTON) || defined(TQT_NO_STATUSBAR))
#define TQT_NO_MAINWINDOW
#endif

// Toolbars
#if !defined(TQT_NO_TOOLBAR) && (defined(TQT_NO_MAINWINDOW))
#define TQT_NO_TOOLBAR
#endif

// TQAction
#if !defined(TQT_NO_ACTION) && (defined(TQT_NO_TOOLBUTTON) || defined(TQT_NO_COMBOBOX))
#define TQT_NO_ACTION
#endif

// TQTable
#if !defined(TQT_NO_TABLE) && (defined(TQT_NO_COMBOBOX) || defined(TQT_NO_HEADER) || defined(TQT_NO_CHECKBOX))
#define TQT_NO_TABLE
#endif

// TQFontDialog
#if !defined(TQT_NO_FONTDIALOG) && (defined(TQT_NO_DIALOG) || defined(TQT_NO_FONTDATABASE) || defined(TQT_NO_COMBOBOX) || defined(TQT_NO_LABEL) || defined(TQT_NO_CHECKBOX) || defined(TQT_NO_PUSHBUTTON) || defined(TQT_NO_VGROUPBOX) || defined(TQT_NO_VALIDATOR))
#define TQT_NO_FONTDIALOG
#endif

// TQInputDialog
#if !defined(TQT_NO_INPUTDIALOG) && (defined(TQT_NO_DIALOG) || defined(TQT_NO_COMBOBOX) || defined(TQT_NO_LABEL) || defined(TQT_NO_PUSHBUTTON) || defined(TQT_NO_SPINBOX) || defined(TQT_NO_WIDGETSTACK) || defined(TQT_NO_LAYOUT))
#define TQT_NO_INPUTDIALOG
#endif

// SQL value editor widgets
#if !defined(TQT_NO_SQL_EDIT_WIDGETS) && (defined(TQT_NO_SQL) || defined(TQT_NO_SPINBOX) || defined(TQT_NO_COMBOBOX) || defined(TQT_NO_CHECKBOX) || defined(TQT_NO_DATETIMEEDIT))
#define TQT_NO_SQL_EDIT_WIDGETS
#endif

// TQPrintDialog
#if !defined(TQT_NO_PRINTDIALOG) && (defined(TQT_NO_DIALOG) || defined(TQT_NO_LISTVIEW) || defined(TQT_NO_PRINTER) || defined(TQT_NO_COMBOBOX) || defined(TQT_NO_LABEL) || defined(TQT_NO_BUTTONGROUP) || defined(TQT_NO_SPINBOX) || defined(TQT_NO_RADIOBUTTON) || defined(TQT_NO_PUSHBUTTON) || defined(TQT_NO_DIR))
#define TQT_NO_PRINTDIALOG
#endif

// TQFileDialog
#if !defined(TQT_NO_FILEDIALOG) && (defined(TQT_NO_MESSAGEBOX) || defined(TQT_NO_LISTVIEW) || defined(TQT_NO_NETWORKPROTOCOL) || defined(TQT_NO_COMBOBOX) || defined(TQT_NO_SEMIMODAL) || defined(TQT_NO_REGEXP_CAPTURE) || defined(TQT_NO_TOOLBUTTON) || defined(TQT_NO_BUTTONGROUP) || defined(TQT_NO_VBOX) || defined(TQT_NO_SPLITTER) || defined(TQT_NO_PROGRESSBAR) || defined(TQT_NO_WIDGETSTACK) || defined(TQT_NO_DATESTRING))
#define TQT_NO_FILEDIALOG
#endif

// SQL table widgets
#if !defined(TQT_NO_SQL_VIEW_WIDGETS) && (defined(TQT_NO_SQL_FORM) || defined(TQT_NO_SQL_EDIT_WIDGETS) || defined(TQT_NO_TABLE))
#define TQT_NO_SQL_VIEW_WIDGETS
#endif

// TQWorkSpace
#if !defined(TQT_NO_WORKSPACE) && (defined(TQT_NO_SCROLLBAR) || defined(TQT_NO_VBOX) || defined(TQT_NO_TITLEBAR) || defined(TQT_NO_RESIZEHANDLER) || defined(TQT_NO_POPUPMENU) || defined(TQT_NO_LABEL) || defined(TQT_NO_TOOLBUTTON) || defined(TQT_NO_MAINWINDOW) || defined(TQT_NO_TOOLBAR) || defined(TQT_NO_MENUBAR))
#define TQT_NO_WORKSPACE
#endif

// Input method
#if !defined(TQT_NO_IM) && (defined(TQT_NO_STRINGLIST))
#define TQT_NO_IM
#endif

// Input method extensions
#if !defined(TQT_NO_IM_EXTENSIONS) && (defined(TQT_NO_IM) || defined(TQT_NO_STRINGLIST))
#define TQT_NO_IM_EXTENSIONS
#endif

