/****************************************************************************
**
** Implementation of TQStyle class
**
** Created : 981231
**
** Copyright (C) 1998-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of the kernel module of the TQt GUI Toolkit.
**
** This file may be used under the terms of the GNU General
** Public License versions 2.0 or 3.0 as published by the Free
** Software Foundation and appearing in the files LICENSE.GPL2
** and LICENSE.GPL3 included in the packaging of this file.
** Alternatively you may (at your option) use any later version
** of the GNU General Public License if such license has been
** publicly approved by Trolltech ASA (or its successors, if any)
** and the KDE Free TQt Foundation.
**
** Please review the following information to ensure GNU General
** Public Licensing requirements will be met:
** http://trolltech.com/products/qt/licenses/licensing/opensource/.
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://trolltech.com/products/qt/licenses/licensing/licensingoverview
** or contact the sales department at sales@trolltech.com.
**
** This file may be used under the terms of the Q Public License as
** defined by Trolltech ASA and appearing in the file LICENSE.TQPL
** included in the packaging of this file.  Licensees holding valid TQt
** Commercial licenses may use this file in accordance with the TQt
** Commercial License Agreement provided with the Software.
**
** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE. Trolltech reserves all rights not granted
** herein.
**
**********************************************************************/

#include "ntqstyle.h"
#ifndef TQT_NO_STYLE
#include "ntqapplication.h"
#include "ntqpainter.h"
#include "ntqbitmap.h"
#include "ntqpixmapcache.h"
#include "ntqframe.h"
#include "ntqlayout.h"
#include "ntqlistview.h"
#include "ntqpopupmenu.h"
#include "ntqpushbutton.h"
#include "ntqobjectlist.h"
#include "ntqwidgetlist.h"

#include <limits.h>

class TQStylePrivate
{
public:
    TQStylePrivate()
    {
    }
};

TQStyleOption::TQStyleOption(StyleOptionDefault) :
	def(true),
	tb(NULL),
	i1(-1),
	i2(-1),
	i3(-1),
	i4(-1),
	i5(-1),
	cli(NULL),
	tbh(NULL) {
	//
}

TQStyleOption::TQStyleOption(int in1) :
	def(false),
	tb(NULL),
	i1(in1),
	i2(-1),
	i3(-1),
	i4(-1),
	i5(-1),
	cli(NULL),
	tbh(NULL) {
	//
}

TQStyleOption::TQStyleOption(int in1, int in2) :
	def(false),
	tb(NULL),
	i1(in1),
	i2(in2),
	i3(-1),
	i4(-1),
	i5(-1),
	cli(NULL),
	tbh(NULL) {
	//
}

TQStyleOption::TQStyleOption(int in1, int in2, int in3, int in4) :
	def(false),
	tb(NULL),
	i1(in1),
	i2(in2),
	i3(in3),
	i4(in4),
	i5(-1),
	cli(NULL),
	tbh(NULL) {
	//
}

TQStyleOption::TQStyleOption(TQMenuItem* m) :
	def(false),
	mi(m),
	tb(NULL),
	i1(-1),
	i2(-1),
	i3(-1),
	i4(-1),
	i5(-1),
	cli(NULL),
	tbh(NULL) {
	//
}

TQStyleOption::TQStyleOption(TQMenuItem* m, int in1) :
	def(false),
	mi(m),
	tb(NULL),
	i1(in1),
	i2(-1),
	i3(-1),
	i4(-1),
	i5(-1),
	cli(NULL),
	tbh(NULL) {
	//
}

TQStyleOption::TQStyleOption(TQMenuItem* m, int in1, int in2) :
	def(false),
	mi(m),
	tb(NULL),
	i1(in1),
	i2(in2),
	i3(-1),
	i4(-1),
	i5(-1),
	cli(NULL),
	tbh(NULL) {
	//
}

TQStyleOption::TQStyleOption(const TQColor& c) :
	def(false),
	tb(NULL),
	cl(&c),
	i1(-1),
	i2(-1),
	i3(-1),
	i4(-1),
	i5(-1),
	cli(NULL),
	tbh(NULL) {
	//
}

TQStyleOption::TQStyleOption(TQTab* t) :
	def(false),
	tb(t),
	i1(-1),
	i2(-1),
	i3(-1),
	i4(-1),
	i5(-1),
	cli(NULL),
	tbh(NULL) {
	//
}

TQStyleOption::TQStyleOption(TQListViewItem* i) :
	def(false),
	tb(NULL),
	li(i),
	i1(-1),
	i2(-1),
	i3(-1),
	i4(-1),
	i5(-1),
	cli(NULL),
	tbh(NULL) {
	//
}

TQStyleOption::TQStyleOption(TQCheckListItem* i) :
	def(false),
	tb(NULL),
	i1(-1),
	i2(-1),
	i3(-1),
	i4(-1),
	i5(-1),
	cli(i),
	tbh(NULL) {
	//
}

TQStyleOption::TQStyleOption(TQt::ArrowType a) :
	def(false),
	tb(NULL),
	i1((int)a),
	i2(-1),
	i3(-1),
	i4(-1),
	i5(-1),
	cli(NULL),
	tbh(NULL) {
	//
}

TQStyleOption::TQStyleOption(const TQRect& r) :
	def(false),
	tb(NULL),
	i1(r.x()),
	i2(r.y()),
	i3(r.width()),
	i4(r.height()),
	i5(-1),
	cli(NULL),
	tbh(NULL) {
	//
}

TQStyleOption::TQStyleOption(TQWidget *w) :
	def(false),
	tb(NULL),
	i1(-1),
	i2(-1),
	i3(-1),
	i4(-1),
	i5(-1),
	cli(NULL),
	p1((void*)w),
	tbh(NULL) {
	//
}

/*!
    \class TQStyleOption ntqstyle.h
    \brief The TQStyleOption class specifies optional parameters for TQStyle functions.
    \ingroup appearance

    Some TQStyle functions take an optional argument specifying extra
    information that is required for a paritical primitive or control.
    So that the TQStyle class can be extended, TQStyleOption is used to
    provide a variable-argument for these options.

    The TQStyleOption class has constructors for each type of optional
    argument, and this set of constructors may be extended in future
    TQt releases. There are also corresponding access functions that
    return the optional arguments: these too may be extended.

    For each constructor, you should refer to the documentation of the
    TQStyle functions to see the meaning of the arguments.

    When calling TQStyle functions from your own widgets, you must only
    pass the default TQStyleOption or the argument that TQStyle is
    documented to accept. For example, if the function expects
    TQStyleOption(TQMenuItem *, int), passing TQStyleOption(TQMenuItem *)
    leaves the optional integer argument uninitialized.

    When subclassing TQStyle, you must similarly only expect the
    default or documented arguments. The other arguments will have
    uninitialized values.

    If you make your own TQStyle subclasses and your own widgets, you
    can make a subclass of TQStyleOption to pass additional arguments
    to your TQStyle subclass. You will need to cast the "const
    TQStyleOption&" argument to your subclass, so be sure your style
    has been called from your widget.
*/

/*!
    \enum TQStyleOption::StyleOptionDefault

    This enum value can be passed as the optional argument to any
    TQStyle function.

    \value Default
*/

/*!
    \fn TQStyleOption::TQStyleOption(StyleOptionDefault)

    The default option. This can always be passed as the optional
    argument to TQStyle functions.
*/

/*!
    \fn TQStyleOption::TQStyleOption(int)

    Pass one integer, \a in1. For example, headerSection.
*/

/*!
    \fn TQStyleOption::TQStyleOption(int, int)

    Pass two integers, \a in1 and \a in2. For example, linewidth and
    midlinewidth.
*/

/*!
    \fn TQStyleOption::TQStyleOption(int, int, int, int)

    Pass four integers, \a in1, \a in2, \a in3 and \a in4.
*/

/*!
    \fn TQStyleOption::TQStyleOption(TQMenuItem*)

    Pass a menu item, \a m.
*/

/*!
    \fn TQStyleOption::TQStyleOption(TQMenuItem*, int)

    Pass a menu item and an integer, \a m and \a in1.
*/

/*!
    \fn TQStyleOption::TQStyleOption(TQMenuItem*, int, int)

    Pass a menu item and two integers, \a m, \a in1 and \a in2.
*/

/*!
    \fn TQStyleOption::TQStyleOption(const TQColor&)

    Pass a color, \a c.
*/

/*!
    \fn TQStyleOption::TQStyleOption(TQTab*)

    Pass a TQTab, \a t.
*/

/*!
    \fn TQStyleOption::TQStyleOption(TQListViewItem*)

    Pass a TQListViewItem, \a i.
*/

/*!
    \fn TQStyleOption::TQStyleOption(TQt::ArrowType)

    Pass an TQt::ArrowType, \a a.
*/

/*!
    \fn TQStyleOption::TQStyleOption(TQCheckListItem* i)

    Pass a TQCheckListItem, \a i.
*/

/*!
    \fn TQStyleOption::TQStyleOption( const TQRect &r )

    Pass a TQRect, \a r.
*/

/*!
    \fn TQStyleOption::TQStyleOption( TQWidget *w )

    Pass a TQWidget, \a w.
*/

/*!
    \fn bool TQStyleOption::isDefault() const

    Returns true if the option was constructed with the default
    constructor; otherwise returns false.
*/

/*!
    \fn int TQStyleOption::day() const

    Returns the index of the day in the month if the appropriate
    constructor was called; otherwise the return value is undefined.
*/

/*!
    \fn int TQStyleOption::lineWidth() const

    Returns the line width if the appropriate constructor was called;
    otherwise the return value is undefined.
*/

/*!
    \fn int TQStyleOption::midLineWidth() const

    Returns the mid-line width if the appropriate constructor was
    called; otherwise the return value is undefined.
*/

/*!
    \fn int TQStyleOption::frameShape() const

    Returns a TQFrame::Shape value if the appropriate constructor was
    called; otherwise the return value is undefined.
*/

/*!
    \fn int TQStyleOption::frameShadow() const

    Returns a TQFrame::Shadow value if the appropriate constructor was
    called; otherwise the return value is undefined.
*/

/*!
    \fn TQMenuItem* TQStyleOption::menuItem() const

    Returns a menu item if the appropriate constructor was called;
    otherwise the return value is undefined.
*/

/*!
    \fn int TQStyleOption::maxIconWidth() const

    Returns the maximum width of the menu item check area if the
    appropriate constructor was called; otherwise the return value is
    undefined.
*/

/*!
    \fn int TQStyleOption::tabWidth() const

    Returns the tab indent width if the appropriate constructor was
    called; otherwise the return value is undefined.
*/

/*!
    \fn int TQStyleOption::headerSection() const

    Returns the header section if the appropriate constructor was
    called; otherwise the return value is undefined.
*/

/*!
    \fn const TQColor& TQStyleOption::color() const

    Returns a color if the appropriate constructor was called;
    otherwise the return value is undefined.
*/

/*!
    \fn TQTab* TQStyleOption::tab() const

    Returns a TQTabBar tab if the appropriate constructor was called;
    otherwise the return value is undefined.
*/

/*!
    \fn TQListViewItem* TQStyleOption::listViewItem() const

    Returns a TQListView item if the appropriate constructor was
    called; otherwise the return value is undefined.
*/

/*!
    \fn TQt::ArrowType TQStyleOption::arrowType() const

    Returns an arrow type if the appropriate constructor was called;
    otherwise the return value is undefined.
*/

/*!
    \fn TQCheckListItem* TQStyleOption::checkListItem() const

    Returns a check list item if the appropriate constructor was
    called; otherwise the return value is undefined.
*/

/*!
    \fn TQRect TQStyleOption::rect() const

    Returns a rectangle if the appropriate constructor was called;
    otherwise the return value is undefined.
*/

/*!
    \fn TQWidget* TQStyleOption::widget() const

    Returns a pointer to a widget if the appropriate constructor was called;
    otherwise the return value is undefined.
*/

/*!
    \class TQStyle ntqstyle.h
    \brief The TQStyle class specifies the look and feel of a GUI.
    \ingroup appearance

    A large number of GUI elements are common to many widgets. The
    TQStyle class allows the look of these elements to be modified
    across all widgets that use the TQStyle functions. It also
    provides two feel options: Motif and Windows.

    Although it is not possible to fully enumerate the look of
    graphical elements and the feel of widgets in a GUI, TQStyle
    provides a considerable amount of control and customisability.

    In TQt 1.x the look and feel option for widgets was specified by a
    single value: the GUIStyle. Starting with TQt 2.0, this notion was
    expanded to allow the look to be specified by virtual drawing
    functions.

    Derived classes may reimplement some or all of the drawing
    functions to modify the look of all widgets that use those
    functions.

    Languages written from right to left (such as Arabic and Hebrew)
    usually also mirror the whole layout of widgets. If you design a
    style, you should take special care when drawing asymmetric
    elements to make sure that they also look correct in a mirrored
    layout. You can start your application with \c -reverse to check
    the mirrored layout. Also notice, that for a reversed layout, the
    light usually comes from top right instead of top left.

    The actual reverse layout is performed automatically when
    possible. However, for the sake of flexibility, the translation
    cannot be performed everywhere. The documentation for each
    function in the TQStyle API states whether the function
    expects/returns logical or screen coordinates. Using logical
    coordinates (in ComplexControls, for example) provides great
    flexibility in controlling the look of a widget. Use visualRect()
    when necessary to translate logical coordinates into screen
    coordinates for drawing.

    In TQt versions prior to 3.0, if you wanted a low level route into
    changing the appearance of a widget, you would reimplement
    polish(). With the new 3.0 style engine the recommended approach
    is to reimplement the draw functions, for example drawItem(),
    drawPrimitive(), drawControl(), drawControlMask(),
    drawComplexControl() and drawComplexControlMask(). Each of these
    functions is called with a range of parameters that provide
    information that you can use to determine how to draw them, e.g.
    style flags, rectangle, color group, etc.

    For information on changing elements of an existing style or
    creating your own style see the \link customstyles.html Style
    overview\endlink.

    Styles can also be created as \link plugins-howto.html
    plugins\endlink.
*/

/*!
  \enum TQt::GUIStyle

  \obsolete

  \value WindowsStyle
  \value MotifStyle
  \value MacStyle
  \value Win3Style
  \value PMStyle
*/

/*!
    \enum TQt::UIEffect

    \value UI_General
    \value UI_AnimateMenu
    \value UI_FadeMenu
    \value UI_AnimateCombo
    \value UI_AnimateTooltip
    \value UI_FadeTooltip
    \value UI_AnimateToolBox Reserved
*/

/*!
    Constructs a TQStyle.
*/
TQStyle::TQStyle()
{
    m_eventHandlerInstallationHook = NULL;
    m_eventHandlerRemovalHook = NULL;
    m_widgetActionRequestHook = NULL;
    m_applicationActionRequestHook = NULL;
    conditionalAcceleratorsEnabled = false;
    d = new TQStylePrivate;
}

/*!
    Destroys the style and frees all allocated resources.
*/
TQStyle::~TQStyle()
{
    delete d;
    d = 0;
}

/*
  \fn GUIStyle TQStyle::guiStyle() const
  \obsolete

  Returns an indicator to the additional "feel" component of a
  style. Current supported values are TQt::WindowsStyle and TQt::MotifStyle.
*/

/*!
    Initializes the appearance of a widget.

    This function is called for every widget at some point after it
    has been fully created but just \e before it is shown the very
    first time.

    Reasonable actions in this function might be to call
    TQWidget::setBackgroundMode() for the widget. An example of highly
    unreasonable use would be setting the geometry! Reimplementing
    this function gives you a back-door through which you can change
    the appearance of a widget. With TQt 3.0's style engine you will
    rarely need to write your own polish(); instead reimplement
    drawItem(), drawPrimitive(), etc.

    The TQWidget::inherits() function may provide enough information to
    allow class-specific customizations. But be careful not to
    hard-code things too much because new TQStyle subclasses are
    expected to work reasonably with all current and \e future
    widgets.

    \sa unPolish()
*/
void TQStyle::polish( TQWidget *widget ) {
	const TQStyleControlElementData &ceData = populateControlElementDataFromWidget(widget, TQStyleOption());
	polish(ceData, getControlElementFlagsForObject(widget, TQStyleOption()), widget);
}

/*!
    Undoes the initialization of a widget's appearance.

    This function is the counterpart to polish. It is called for every
    polished widget when the style is dynamically changed. The former
    style has to unpolish its settings before the new style can polish
    them again.

    \sa polish()
*/
void TQStyle::unPolish( TQWidget *widget ) {
	const TQStyleControlElementData &ceData = populateControlElementDataFromWidget(widget, TQStyleOption());
	unPolish(ceData, getControlElementFlagsForObject(widget, TQStyleOption()), widget);
}

/*!
    Initializes the appearance of a widget.

    This function is called for every widget at some point after it
    has been fully created but just \e before it is shown the very
    first time.

    Reasonable actions in this function might be to install a widget
    event handler for the style. An example of highly unreasonable
    use would be setting the geometry! With TQt 3.0's style engine
    you will rarely need to write your own polish(); instead reimplement
    drawItem(), drawPrimitive(), etc.

    The \a objectTypes object may provide enough information to
    allow class-specific customizations. But be careful not to
    hard-code things too much because new TQStyle subclasses are
    expected to work reasonably with all current and \e future
    widgets.

    You may specify either a TQWidget pointer or a custom pointer.
    If a custom pointer is specified, you must be careful to intercept any event
    handler installation/removal calls via setEventHandlerInstallationHook and
    setEventHandlerRemovalHook.

    \sa unPolish()
*/
void TQStyle::polish( const TQStyleControlElementData &ceData, ControlElementFlags, void *ptr ) {
	if (ceData.widgetObjectTypes.contains("TQWidget")) {
		// Enable dynamic hide/show of accelerator shortcuts
		TQWidget* widget = reinterpret_cast<TQWidget*>(ptr);
		widget->installEventFilter(this);
	}
}

/*!
    Undoes the initialization of a widget's appearance.

    This function is the counterpart to polish. It is called for every
    polished widget when the style is dynamically changed. The former
    style has to unpolish its settings before the new style can polish
    them again.

    \sa polish()
*/
void TQStyle::unPolish( const TQStyleControlElementData &ceData, ControlElementFlags, void *ptr ) {
	if (ceData.widgetObjectTypes.contains("TQWidget")) {
		// Disable dynamic hide/show of accelerator shortcuts
		TQWidget* widget = reinterpret_cast<TQWidget*>(ptr);
		widget->installEventFilter(this);
	}
}

/*!
    \overload
    Late initialization of the TQApplication object.

    \sa unPolish()
*/
void TQStyle::polish( TQApplication *app ) {
	const TQStyleControlElementData &ceData = populateControlElementDataFromApplication(app, TQStyleOption());
	applicationPolish(ceData, getControlElementFlagsForObject(app, TQStyleOption()), app);
}

/*!
    \overload

    Undoes the application polish.

    \sa polish()
*/
void TQStyle::unPolish( TQApplication *app ) {
	const TQStyleControlElementData &ceData = populateControlElementDataFromApplication(app, TQStyleOption());
	applicationUnPolish(ceData, getControlElementFlagsForObject(app, TQStyleOption()), app);
}

/*!
    \overload
    Late initialization of the TQApplication object or other global application object.

    You may specify either a TQApplication pointer or a custom pointer.
    If a custom pointer is specified, you must be careful to intercept any event
    handler installation/removal calls via setEventHandlerInstallationHook and
    setEventHandlerRemovalHook.

    \sa unPolish()
*/
void TQStyle::applicationPolish( const TQStyleControlElementData&, ControlElementFlags, void * ) {
	//
}

/*!
    \overload

    Undoes the application polish.

    You may specify either a TQApplication pointer or a custom pointer.
    If a custom pointer is specified, you must be careful to intercept any event
    handler installation/removal calls via setEventHandlerInstallationHook and
    setEventHandlerRemovalHook.

    \sa polish()
*/
void TQStyle::applicationUnPolish( const TQStyleControlElementData&, ControlElementFlags, void * ) {
	//
}

/*!
    \overload

    The style may have certain requirements for color palettes. In
    this function it has the chance to change the palette according to
    these requirements.

    \sa TQPalette, TQApplication::setPalette()
*/
void TQStyle::polish( TQPalette&)
{
}

/*!
    Polishes the popup menu according to the GUI style. This usually
    means setting the mouse tracking
    (\l{TQPopupMenu::setMouseTracking()}) and whether the menu is
    checkable by default (\l{TQPopupMenu::setCheckable()}).
*/
void TQStyle::polishPopupMenu( TQPopupMenu *pm ) {
	const TQStyleControlElementData &ceData = populateControlElementDataFromWidget(pm, TQStyleOption());
	polish(ceData, getControlElementFlagsForObject(pm, TQStyleOption()), pm);
}

/*!
    Polishes the popup menu according to the GUI style. This usually
    means setting the mouse tracking
    (\l{TQPopupMenu::setMouseTracking()}) and whether the menu is
    checkable by default (\l{TQPopupMenu::setCheckable()}).
*/
void TQStyle::polishPopupMenu( const TQStyleControlElementData&, ControlElementFlags, void * )
{
}

/*!
    Returns the appropriate area (see below) within rectangle \a r in
    which to draw the \a text or \a pixmap using painter \a p. If \a
    len is -1 (the default) all the \a text is drawn; otherwise only
    the first \a len characters of \a text are drawn. The text is
    aligned in accordance with the alignment \a flags (see
    \l{TQt::AlignmentFlags}). The \a enabled bool indicates whether or
    not the item is enabled.

    If \a r is larger than the area needed to render the \a text the
    rectangle that is returned will be offset within \a r in
    accordance with the alignment \a flags. For example if \a flags is
    \c AlignCenter the returned rectangle will be centered within \a
    r. If \a r is smaller than the area needed the rectangle that is
    returned will be \e larger than \a r (the smallest rectangle large
    enough to render the \a text or \a pixmap).

    By default, if both the text and the pixmap are not null, the
    pixmap is drawn and the text is ignored.
*/
TQRect TQStyle::itemRect( TQPainter *p, const TQRect &r,
			int flags, bool enabled, const TQPixmap *pixmap,
			const TQString& text, int len ) const
{
    TQRect result;
    int x = r.x();
    int y = r.y();
    int w = r.width();
    int h = r.height();
    GUIStyle gs = (GUIStyle)styleHint( SH_GUIStyle );

    if ( pixmap ) {
	if ( (flags & TQt::AlignVCenter) == TQt::AlignVCenter )
	    y += h/2 - pixmap->height()/2;
	else if ( (flags & TQt::AlignBottom) == TQt::AlignBottom)
	    y += h - pixmap->height();
	if ( (flags & TQt::AlignRight) == TQt::AlignRight )
	    x += w - pixmap->width();
	else if ( (flags & TQt::AlignHCenter) == TQt::AlignHCenter )
	    x += w/2 - pixmap->width()/2;
	else if ( (flags & TQt::AlignLeft) != TQt::AlignLeft && TQApplication::reverseLayout() )
	    x += w - pixmap->width();
	result = TQRect(x, y, pixmap->width(), pixmap->height());
    } else if ( !text.isNull() && p ) {
	result = p->boundingRect( x, y, w, h, flags, text, len );
	if ( gs == TQt::WindowsStyle && !enabled ) {
	    result.setWidth(result.width()+1);
	    result.setHeight(result.height()+1);
	}
    } else {
	result = TQRect(x, y, w, h);
    }

    return result;
}


/*!
    Draws the \a text or \a pixmap in rectangle \a r using painter \a
    p and color group \a g. The pen color is specified with \a
    penColor. The \a enabled bool indicates whether or not the item is
    enabled; when reimplementing this bool should influence how the
    item is drawn. If \a len is -1 (the default) all the \a text is
    drawn; otherwise only the first \a len characters of \a text are
    drawn. The text is aligned and wrapped according to the alignment
    \a flags (see \l{TQt::AlignmentFlags}).

    By default, if both the text and the pixmap are not null, the
    pixmap is drawn and the text is ignored.
*/
void TQStyle::drawItem( TQPainter *p, const TQRect &r,
		       int flags, const TQColorGroup &g, bool enabled,
		       const TQPixmap *pixmap, const TQString& text, int len,
		       const TQColor* penColor ) const
{
    int x = r.x();
    int y = r.y();
    int w = r.width();
    int h = r.height();
    GUIStyle gs = (GUIStyle)styleHint( SH_GUIStyle );

    p->setPen( penColor?*penColor:g.foreground() );
    if ( pixmap ) {
	TQPixmap  pm( *pixmap );
	bool clip = (flags & TQt::DontClip) == 0;
	if ( clip ) {
	    if ( pm.width() < w && pm.height() < h ) {
		clip = false;
	    } else {
		p->save();
		TQRegion cr = TQRect(x, y, w, h);
		if (p->hasClipping())
		    cr &= p->clipRegion(TQPainter::CoordPainter);
		p->setClipRegion(cr);
	    }
	}
	if ( (flags & TQt::AlignVCenter) == TQt::AlignVCenter )
	    y += h/2 - pm.height()/2;
	else if ( (flags & TQt::AlignBottom) == TQt::AlignBottom)
	    y += h - pm.height();
	if ( (flags & TQt::AlignRight) == TQt::AlignRight )
	    x += w - pm.width();
	else if ( (flags & TQt::AlignHCenter) == TQt::AlignHCenter )
	    x += w/2 - pm.width()/2;
	else if ( ((flags & TQt::AlignLeft) != TQt::AlignLeft) && TQApplication::reverseLayout() ) // AlignAuto && rightToLeft
	    x += w - pm.width();

	if ( !enabled ) {
	    if ( pm.mask() ) {			// pixmap with a mask
		if ( !pm.selfMask() ) {		// mask is not pixmap itself
		    TQPixmap pmm( *pm.mask() );
		    pmm.setMask( *((TQBitmap *)&pmm) );
		    pm = pmm;
		}
	    } else if ( pm.depth() == 1 ) {	// monochrome pixmap, no mask
		pm.setMask( *((TQBitmap *)&pm) );
#ifndef TQT_NO_IMAGE_HEURISTIC_MASK
	    } else {				// color pixmap, no mask
		TQString k;
		k.sprintf( "$qt-drawitem-%x", pm.serialNumber() );
		TQPixmap *mask = TQPixmapCache::find(k);
		bool del=false;
		if ( !mask ) {
		    mask = new TQPixmap( pm.createHeuristicMask() );
		    mask->setMask( *((TQBitmap*)mask) );
		    del = !TQPixmapCache::insert( k, mask );
		}
		pm = *mask;
		if (del) delete mask;
#endif
	    }
	    if ( gs == TQt::WindowsStyle ) {
		p->setPen( g.light() );
		p->drawPixmap( x+1, y+1, pm );
		p->setPen( g.text() );
	    }
	}
	p->drawPixmap( x, y, pm );
	if ( clip )
	    p->restore();
    } else if ( !text.isNull() ) {
	if ( gs == TQt::WindowsStyle && !enabled ) {
	    p->setPen( g.light() );
	    p->drawText( x+1, y+1, w, h, flags, text, len );
	    p->setPen( g.text() );
	}
	p->drawText( x, y, w, h, flags, text, len );
    }
}

/*!
    \enum TQStyle::PrimitiveElement

    This enum represents the PrimitiveElements of a style. A
    PrimitiveElement is a common GUI element, such as a checkbox
    indicator or pushbutton bevel.

    \value PE_ButtonCommand  button used to initiate an action, for
	example, a TQPushButton.
    \value PE_ButtonDefault  this button is the default button, e.g.
	in a dialog.
    \value PE_ButtonBevel  generic button bevel.
    \value PE_ButtonTool  tool button, for example, a TQToolButton.
    \value PE_ButtonDropDown  drop down button, for example, a tool
	button that displays a popup menu, for example, TQPopupMenu.


    \value PE_FocusRect  generic focus indicator.


    \value PE_ArrowUp  up arrow.
    \value PE_ArrowDown  down arrow.
    \value PE_ArrowRight  right arrow.
    \value PE_ArrowLeft  left arrow.


    \value PE_SpinWidgetUp  up symbol for a spin widget, for example a
	TQSpinBox.
    \value PE_SpinWidgetDown down symbol for a spin widget.
    \value PE_SpinWidgetPlus  increase symbol for a spin widget.
    \value PE_SpinWidgetMinus  decrease symbol for a spin widget.


    \value PE_Indicator  on/off indicator, for example, a TQCheckBox.
    \value PE_IndicatorMask  bitmap mask for an indicator.
    \value PE_ExclusiveIndicator  exclusive on/off indicator, for
	example, a TQRadioButton.
    \value PE_ExclusiveIndicatorMask  bitmap mask for an exclusive indicator.


    \value PE_DockWindowHandle  tear off handle for dock windows and
	toolbars, for example \l{TQDockWindow}s and \l{TQToolBar}s.
    \value PE_DockWindowSeparator  item separator for dock window and
	toolbar contents.
    \value PE_DockWindowResizeHandle  resize handle for dock windows.

    \value PE_Splitter  splitter handle; see also TQSplitter.


    \value PE_Panel  generic panel frame; see also TQFrame.
    \value PE_PanelPopup  panel frame for popup windows/menus; see also
	TQPopupMenu.
    \value PE_PanelMenuBar  panel frame for menu bars.
    \value PE_PanelDockWindow  panel frame for dock windows and toolbars.
    \value PE_PanelTabWidget  panel frame for tab widgets.
    \value PE_PanelLineEdit  panel frame for line edits.
    \value PE_PanelGroupBox  panel frame for group boxes.

    \value PE_TabBarBase  area below tabs in a tab widget, for example,
	TQTab.


    \value PE_HeaderSection  section of a list or table header; see also
	TQHeader.
    \value PE_HeaderArrow arrow used to indicate sorting on a list or table
	header
    \value PE_StatusBarSection  section of a status bar; see also
	TQStatusBar.


    \value PE_GroupBoxFrame  frame around a group box; see also
	TQGroupBox.
    \value PE_WindowFrame  frame around a MDI window or a docking window


    \value PE_Separator  generic separator.


    \value PE_SizeGrip  window resize handle; see also TQSizeGrip.


    \value PE_CheckMark  generic check mark; see also TQCheckBox.


    \value PE_ScrollBarAddLine  scrollbar line increase indicator
	(i.e. scroll down); see also TQScrollBar.
    \value PE_ScrollBarSubLine  scrollbar line decrease indicator (i.e. scroll up).
    \value PE_ScrollBarAddPage  scrollbar page increase indicator (i.e. page down).
    \value PE_ScrollBarSubPage  scrollbar page decrease indicator (i.e. page up).
    \value PE_ScrollBarSlider  scrollbar slider
    \value PE_ScrollBarFirst  scrollbar first line indicator (i.e. home).
    \value PE_ScrollBarLast  scrollbar last line indicator (i.e. end).


    \value PE_ProgressBarChunk  section of a progress bar indicator; see
	also TQProgressBar.

    \value PE_CheckListController controller part of a listview item
    \value PE_CheckListIndicator checkbox part of a listview item
    \value PE_CheckListExclusiveIndicator radiobutton part of a listview item
    \value PE_RubberBand rubber band used in such things as iconview

    \value PE_CustomBase  base value for custom PrimitiveElements.
	All values above this are reserved for custom use. Custom
	values must be greater than this value.

    \sa drawPrimitive()
*/
/*! \enum TQStyle::SFlags
    \internal
*/
/*! \enum TQStyle::SCFlags
    \internal
*/

/*!
    \enum TQStyle::StyleFlags

    This enum represents flags for drawing PrimitiveElements. Not all
    primitives use all of these flags. Note that these flags may mean
    different things to different primitives. For an explanation of
    the relationship between primitives and their flags, as well as
    the different meanings of the flags, see the \link
    customstyles.html Style overview\endlink.

    \value Style_Default
    \value Style_Enabled
    \value Style_Raised
    \value Style_Sunken
    \value Style_Off
    \value Style_NoChange
    \value Style_On
    \value Style_Down
    \value Style_Horizontal
    \value Style_HasFocus
    \value Style_Top
    \value Style_Bottom
    \value Style_FocusAtBorder
    \value Style_AutoRaise
    \value Style_MouseOver
    \value Style_Up
    \value Style_Selected
    \value Style_HasFocus
    \value Style_Active
    \value Style_ButtonDefault

    \sa drawPrimitive()
*/

/*!
    \fn void TQStyle::drawPrimitive( PrimitiveElement pe, TQPainter *p, const TQStyleControlElementData &ceData, ControlElementFlags elementFlags, const TQRect &r, const TQColorGroup &cg, SFlags flags, const TQStyleOption& opt) const

    Draws the style PrimitiveElement \a pe using the painter \a p in
    the area \a r. Colors are used from the color group \a cg.

    \a ceData and \a elementFlags provide additional information about
    the widget for which the PrimitiveElement is being drawn.

    The rect \a r should be in screen coordinates.

    The \a flags argument is used to control how the PrimitiveElement
    is drawn. Multiple flags can be OR'ed together.

    For example, a pressed button would be drawn with the flags \c
    Style_Enabled and \c Style_Down.

    The \a opt argument can be used to control how various
    PrimitiveElements are drawn. Note that \a opt may be the default
    value even for PrimitiveElements that make use of extra options.
    When \a opt is non-default, it is used as follows:

    \table
    \header \i PrimitiveElement \i Options \i Notes
    \row \i \l PE_FocusRect
	 \i \l TQStyleOption ( const \l TQColor & bg )
	    \list
	    \i opt.\link TQStyleOption::color() color\endlink()
	    \endlist
	 \i \e bg is the background color on which the focus rect is being drawn.
    \row \i12 \l PE_Panel
	 \i12 \l TQStyleOption ( int linewidth, int midlinewidth )
		\list
		\i opt.\link TQStyleOption::lineWidth() lineWidth\endlink()
		\i opt.\link TQStyleOption::midLineWidth() midLineWidth\endlink()
		\endlist
	 \i \e linewidth is the line width for drawing the panel.
    \row \i \e midlinewidth is the mid-line width for drawing the panel.
    \row \i12 \l PE_PanelPopup
	 \i12 \l TQStyleOption ( int linewidth, int midlinewidth )
		\list
		\i opt.\link TQStyleOption::lineWidth() lineWidth\endlink()
		\i opt.\link TQStyleOption::midLineWidth() midLineWidth\endlink()
		\endlist
	 \i \e linewidth is the line width for drawing the panel.
    \row \i \e midlinewidth is the mid-line width for drawing the panel.
    \row \i12 \l PE_PanelMenuBar
	 \i12 \l TQStyleOption ( int linewidth, int midlinewidth )
		\list
		\i opt.\link TQStyleOption::lineWidth() lineWidth\endlink()
		\i opt.\link TQStyleOption::midLineWidth() midLineWidth\endlink()
		\endlist
	 \i \e linewidth is the line width for drawing the panel.
    \row \i \e midlinewidth is the mid-line width for drawing the panel.
    \row \i12 \l PE_PanelDockWindow
	 \i12 \l TQStyleOption ( int linewidth, int midlinewidth )
		\list
		\i opt.\link TQStyleOption::lineWidth() lineWidth\endlink()
		\i opt.\link TQStyleOption::midLineWidth() midLineWidth\endlink()
		\endlist
	 \i \e linewidth is the line width for drawing the panel.
    \row \i \e midlinewidth is the mid-line width for drawing the panel.
    \row \i14 \l PE_GroupBoxFrame
	 \i14 \l TQStyleOption ( int linewidth, int midlinewidth, int shape, int shadow )
		\list
		\i opt.\link TQStyleOption::lineWidth() lineWidth\endlink()
		\i opt.\link TQStyleOption::midLineWidth() midLineWidth\endlink()
		\i opt.\link TQStyleOption::frameShape() frameShape\endlink()
		\i opt.\link TQStyleOption::frameShadow() frameShadow\endlink()
		\endlist
	 \i \e linewidth is the line width for the group box.
    \row \i \e midlinewidth is the mid-line width for the group box.
    \row \i \e shape is the \link TQFrame::frameShape frame shape \endlink
	    for the group box.
    \row \i \e shadow is the \link TQFrame::frameShadow frame shadow \endlink
	    for the group box.
    \endtable


    For all other \link TQStyle::PrimitiveElement
    PrimitiveElements\endlink, \a opt is unused.

    \sa StyleFlags
*/

/*!
    \enum TQStyle::ControlElement

    This enum represents a ControlElement. A ControlElement is part of
    a widget that performs some action or displays information to the
    user.

    \value CE_PushButton  the bevel and default indicator of a TQPushButton.
    \value CE_PushButtonLabel  the label (iconset with text or pixmap)
	of a TQPushButton.

    \value CE_CheckBox  the indicator of a TQCheckBox.
    \value CE_CheckBoxLabel  the label (text or pixmap) of a TQCheckBox.

    \value CE_RadioButton  the indicator of a TQRadioButton.
    \value CE_RadioButtonLabel  the label (text or pixmap) of a TQRadioButton.

    \value CE_TabBarTab  the tab within a TQTabBar (a TQTab).
    \value CE_TabBarLabel  the label within a TQTab.

    \value CE_ProgressBarGroove  the groove where the progress
	indicator is drawn in a TQProgressBar.
    \value CE_ProgressBarContents  the progress indicator of a TQProgressBar.
    \value CE_ProgressBarLabel  the text label of a TQProgressBar.

    \value CE_PopupMenuItem  a menu item in a TQPopupMenu.
    \value CE_PopupMenuScroller scrolling areas in a popumenu when the
	style supports scrolling.
    \value CE_PopupMenuHorizontalExtra extra frame area set aside with PM_PopupMenuFrameHorizontalExtra
    \value CE_PopupMenuVerticalExtra extra frame area set aside with PM_PopupMenuFrameVerticalExtra

    \value CE_MenuBarItem  a menu item in a TQMenuBar.

    \value CE_ToolButtonLabel a tool button's label.

    \value CE_MenuBarEmptyArea the empty area of a TQMenuBar.
    \value CE_DockWindowEmptyArea the empty area of a TQDockWindow.

    \value CE_ToolBoxTab the toolbox's tab area
    \value CE_HeaderLabel the header's label

    \value CE_CustomBase  base value for custom ControlElements. All values above
	    this are reserved for custom use. Therefore, custom values must be
	    greater than this value.

    \sa drawControl()
*/

/*!
    \fn void TQStyle::drawControl( ControlElement element, TQPainter *p, const TQStyleControlElementData &ceData, ControlElementFlags elementFlags, const TQRect &r, const TQColorGroup &cg, SFlags how, const TQStyleOption& opt, const TQWidget *widget) const

    Draws the ControlElement \a element using the painter \a p in the
    area \a r. Colors are used from the color group \a cg.

    The rect \a r should be in screen coordinates.

    \a ceData and \a elementFlags provide additional information about
    the widget for which the PrimitiveElement is being drawn.

    The \a how argument is used to control how the ControlElement is
    drawn. Multiple flags can be OR'ed together. See the table below
    for an explanation of which flags are used with the various
    ControlElements.

    The \a widget argument is a pointer to a TQWidget or one of its
    subclasses. Note that usage of the widget argument is deprecated
    in favor of specifying widget parameters via \a ceData and \a elementFlags.
    The widget can be cast to the appropriate type based
    on the value of \a element. The \a opt argument can be used to
    pass extra information required when drawing the ControlElement.
    Note that \a opt may be the default value even for ControlElements
    that can make use of the extra options. See the table below for
    the appropriate \a widget and \a opt usage:

    \table
    \header \i ControlElement<br>\& Widget Cast
	    \i Style Flags
	    \i Notes
	    \i Options
	    \i Notes

    \row \i16 \l{CE_PushButton}(const \l TQPushButton *)

	 and

	    \l{CE_PushButtonLabel}(const \l TQPushButton *)
	 \i \l Style_Enabled \i Set if the button is enabled.
	 \i16 Unused.
	 \i16 &nbsp;
    \row \i \l Style_HasFocus \i Set if the button has input focus.
    \row \i \l Style_Raised \i Set if the button is not down, not on and not flat.
    \row \i \l Style_On \i Set if the button is a toggle button and toggled on.
    \row \i \l Style_Down \i Set if the button is down (i.e., the mouse button or
			    space bar is pressed on the button).
    \row \i \l Style_ButtonDefault \i Set if the button is a default button.

    \row \i16 \l{CE_CheckBox}(const \l TQCheckBox *)

	 and

	 \l{CE_CheckBoxLabel}(const \l TQCheckBox *)

	 \i \l Style_Enabled \i Set if the checkbox is enabled.
	 \i16 Unused.
	 \i16 &nbsp;
    \row \i \l Style_HasFocus \i Set if the checkbox has input focus.
    \row \i \l Style_On \i Set if the checkbox is checked.
    \row \i \l Style_Off \i Set if the checkbox is not checked.
    \row \i \l Style_NoChange \i Set if the checkbox is in the NoChange state.
    \row \i \l Style_Down \i Set if the checkbox is down (i.e., the mouse button or
			    space bar is pressed on the button).

    \row \i15 \l{CE_RadioButton}(const TQRadioButton *)

	and

	\l{CE_RadioButtonLabel}(const TQRadioButton *)
	\i \l Style_Enabled \i Set if the radiobutton is enabled.
	\i15 Unused.
	\i15 &nbsp;
    \row \i \l Style_HasFocus \i Set if the radiobutton has input focus.
    \row \i \l Style_On \i Set if the radiobutton is checked.
    \row \i \l Style_Off \i Set if the radiobutton is not checked.
    \row \i \l Style_Down \i Set if the radiobutton is down (i.e., the mouse
			    button or space bar is pressed on the radiobutton).

    \row \i12 \l{CE_TabBarTab}(const \l TQTabBar *)

	 and

	 \l{CE_TabBarLabel}(const \l TQTabBar *)

	 \i \l Style_Enabled \i Set if the tabbar and tab is enabled.
	 \i12 \l TQStyleOption ( \l TQTab *t )
		\list
		\i opt.\link TQStyleOption::tab() tab\endlink()
		\endlist
	 \i12 \e t is the TQTab being drawn.
    \row \i \l Style_Selected \i Set if the tab is the current tab.

    \row \i12 \l{CE_ProgressBarGroove}(const TQProgressBar *)

	 and

	 \l{CE_ProgressBarContents}(const TQProgressBar *)

	 and

	 \l{CE_ProgressBarLabel}(const TQProgressBar *)

	 \i \l Style_Enabled \i Set if the progressbar is enabled.
	 \i12 Unused.
	 \i12 &nbsp;
    \row \i \l Style_HasFocus \i Set if the progressbar has input focus.

    \row \i13 \l{CE_PopupMenuItem}(const \l TQPopupMenu *)
	 \i \l Style_Enabled \i Set if the menuitem is enabled.
	 \i13 \l TQStyleOption ( TQMenuItem *mi, int tabwidth, int maxpmwidth )
		\list
		\i opt.\link TQStyleOption::menuItem() menuItem\endlink()
		\i opt.\link TQStyleOption::tabWidth() tabWidth\endlink()
		\i opt.\link TQStyleOption::maxIconWidth() maxIconWidth\endlink()
		\endlist
	 \i \e mi is the menu item being drawn. TQMenuItem is currently an
	    internal class.
    \row \i \l Style_Active \i Set if the menuitem is the current item.
	 \i \e tabwidth is the width of the tab column where key accelerators
	    are drawn.
    \row \i \l Style_Down \i Set if the menuitem is down (i.e., the mouse button
			    or space bar is pressed).
	 \i \e maxpmwidth is the maximum width of the check column where
	    checkmarks and iconsets are drawn.

    \row \i14 \l{CE_MenuBarItem}(const \l TQMenuBar *)
	 \i \l Style_Enabled \i Set if the menuitem is enabled
	 \i14 \l TQStyleOption ( TQMenuItem *mi )
		\list
		\i opt.\link TQStyleOption::menuItem() menuItem\endlink()
		\endlist
	 \i14 \e mi is the menu item being drawn.
    \row \i \l Style_Active \i Set if the menuitem is the current item.
    \row \i \l Style_Down \i Set if the menuitem is down (i.e., a mouse button or
			    the space bar is pressed).
    \row \i \l Style_HasFocus \i Set if the menubar has input focus.

    \row \i17 \l{CE_ToolButtonLabel}(const \l TQToolButton *)
	 \i \l Style_Enabled \i Set if the toolbutton is enabled.
	 \i17 \l TQStyleOption ( \l ArrowType t )
		\list
		\i opt.\link TQStyleOption::arrowType() arrowType\endlink()
		\endlist
	 \i17 When the tool button only contains an arrow, \e t is the
	    arrow's type.
    \row \i \l Style_HasFocus \i Set if the toolbutton has input focus.
    \row \i \l Style_Down \i Set if the toolbutton is down (i.e., a
	    mouse button or the space is pressed).
    \row \i \l Style_On \i Set if the toolbutton is a toggle button
	and is toggled on.
    \row \i \l Style_AutoRaise \i Set if the toolbutton has auto-raise enabled.
    \row \i \l Style_MouseOver \i Set if the mouse pointer is over the toolbutton.
    \row \i \l Style_Raised \i Set if the button is not down, not on and doesn't
	contain the mouse when auto-raise is enabled.
    \endtable

    \sa ControlElement, StyleFlags
*/

/*!
    \fn void TQStyle::drawControlMask( ControlElement element, TQPainter *p, const TQStyleControlElementData &ceData, ControlElementFlags elementFlags, const TQRect &r, const TQStyleOption& opt, const TQWidget *widget) const

    Draw a bitmask for the ControlElement \a element using the painter
    \a p in the area \a r. See drawControl() for an explanation of the
    use of the \a widget and \a opt arguments.

    \a ceData and \a elementFlags provide additional information about
    the widget for which the PrimitiveElement is being drawn.

    The rect \a r should be in screen coordinates.

    \a widget is deprecated and should not be used.

    \sa drawControl(), ControlElement
*/

/*!
    \enum TQStyle::SubRect

    This enum represents a sub-area of a widget. Style implementations
    would use these areas to draw the different parts of a widget.

    \value SR_PushButtonContents  area containing the label (iconset
	with text or pixmap).
    \value SR_PushButtonFocusRect  area for the focus rect (usually
	larger than the contents rect).

    \value SR_CheckBoxIndicator  area for the state indicator (e.g. check mark).
    \value SR_CheckBoxContents  area for the label (text or pixmap).
    \value SR_CheckBoxFocusRect  area for the focus indicator.


    \value SR_RadioButtonIndicator  area for the state indicator.
    \value SR_RadioButtonContents  area for the label.
    \value SR_RadioButtonFocusRect  area for the focus indicator.


    \value SR_ComboBoxFocusRect  area for the focus indicator.


    \value SR_SliderFocusRect  area for the focus indicator.


    \value SR_DockWindowHandleRect  area for the tear-off handle.


    \value SR_ProgressBarGroove  area for the groove.
    \value SR_ProgressBarContents  area for the progress indicator.
    \value SR_ProgressBarLabel  area for the text label.


    \value SR_ToolButtonContents area for the tool button's label.

    \value SR_DialogButtonAccept area for a dialog's accept button.
    \value SR_DialogButtonReject area for a dialog's reject button.
    \value SR_DialogButtonApply  area for a dialog's apply button.
    \value SR_DialogButtonHelp area for a dialog's help button.
    \value SR_DialogButtonAll area for a dialog's all button.
    \value SR_DialogButtonRetry area for a dialog's retry button.
    \value SR_DialogButtonAbort area for a dialog's abort button.
    \value SR_DialogButtonIgnore area for a dialog's ignore button.
    \value SR_DialogButtonCustom area for a dialog's custom widget area (in button row).

    \value SR_ToolBoxTabContents area for a toolbox tab's icon and label

    \value SR_CustomBase  base value for custom ControlElements. All values above
	    this are reserved for custom use. Therefore, custom values must be
	    greater than this value.

    \sa subRect()
*/

/*!
    \fn TQRect TQStyle::subRect( SubRect subrect, const TQStyleControlElementData &ceData, const ControlElementFlags elementFlags, const TQWidget *widget ) const;

    Returns the sub-area \a subrect for the \a widget in logical
    coordinates.

    \a ceData and \a elementFlags provide additional information about
    the widget for which the PrimitiveElement is being drawn.

    The \a widget argument is a pointer to a TQWidget or one of its
    subclasses. Note that usage of \a widget is deprecated in favor
    of \a ceData and \a elementFlags. The widget can be cast to the
    appropriate type based on the value of \a subrect. See the table
    below for the appropriate \a widget casts:

    \table
    \header \i SubRect \i Widget Cast
    \row \i \l SR_PushButtonContents	\i (const \l TQPushButton *)
    \row \i \l SR_PushButtonFocusRect	\i (const \l TQPushButton *)
    \row \i \l SR_CheckBoxIndicator	\i (const \l TQCheckBox *)
    \row \i \l SR_CheckBoxContents	\i (const \l TQCheckBox *)
    \row \i \l SR_CheckBoxFocusRect	\i (const \l TQCheckBox *)
    \row \i \l SR_RadioButtonIndicator	\i (const \l TQRadioButton *)
    \row \i \l SR_RadioButtonContents	\i (const \l TQRadioButton *)
    \row \i \l SR_RadioButtonFocusRect	\i (const \l TQRadioButton *)
    \row \i \l SR_ComboBoxFocusRect	\i (const \l TQComboBox *)
    \row \i \l SR_DockWindowHandleRect	\i (const \l TQWidget *)
    \row \i \l SR_ProgressBarGroove	\i (const \l TQProgressBar *)
    \row \i \l SR_ProgressBarContents	\i (const \l TQProgressBar *)
    \row \i \l SR_ProgressBarLabel	\i (const \l TQProgressBar *)
    \endtable

    The tear-off handle (SR_DockWindowHandleRect) for TQDockWindow
    is a private class. Use TQWidget::parentWidget() to access the
    TQDockWindow:

    \code
	if ( !widget->parentWidget() )
	    return;
	const TQDockWindow *dw = (const TQDockWindow *) widget->parentWidget();
    \endcode

    \sa SubRect
*/

/*!
    \enum TQStyle::ComplexControl

    This enum represents a ComplexControl. ComplexControls have
    different behaviour depending upon where the user clicks on them
    or which keys are pressed.

    \value CC_SpinWidget
    \value CC_ComboBox
    \value CC_ScrollBar
    \value CC_Slider
    \value CC_ToolButton
    \value CC_TitleBar
    \value CC_ListView


    \value CC_CustomBase  base value for custom ControlElements. All
	values above this are reserved for custom use. Therefore,
	custom values must be greater than this value.

    \sa SubControl drawComplexControl()
*/

/*!
    \enum TQStyle::SubControl

    This enum represents a SubControl within a ComplexControl.

    \value SC_None   special value that matches no other SubControl.


    \value SC_ScrollBarAddLine  scrollbar add line (i.e. down/right
	arrow); see also TQScrollbar.
    \value SC_ScrollBarSubLine  scrollbar sub line (i.e. up/left arrow).
    \value SC_ScrollBarAddPage  scrollbar add page (i.e. page down).
    \value SC_ScrollBarSubPage  scrollbar sub page (i.e. page up).
    \value SC_ScrollBarFirst	scrollbar first line (i.e. home).
    \value SC_ScrollBarLast	scrollbar last line (i.e. end).
    \value SC_ScrollBarSlider	scrollbar slider handle.
    \value SC_ScrollBarGroove	special subcontrol which contains the
	area in which the slider handle may move.


    \value SC_SpinWidgetUp  spinwidget up/increase; see also TQSpinBox.
    \value SC_SpinWidgetDown  spinwidget down/decrease.
    \value SC_SpinWidgetFrame  spinwidget frame.
    \value SC_SpinWidgetEditField  spinwidget edit field.
    \value SC_SpinWidgetButtonField  spinwidget button field.


    \value SC_ComboBoxEditField  combobox edit field; see also TQComboBox.
    \value SC_ComboBoxArrow  combobox arrow
    \value SC_ComboBoxFrame combobox frame
    \value SC_ComboBoxListBoxPopup combobox list box

    \value SC_SliderGroove  special subcontrol which contains the area
	in which the slider handle may move.
    \value SC_SliderHandle  slider handle.
    \value SC_SliderTickmarks  slider tickmarks.


    \value SC_ToolButton  tool button; see also TQToolbutton.
    \value SC_ToolButtonMenu subcontrol for opening a popup menu in a
	tool button; see also TQPopupMenu.


    \value SC_TitleBarSysMenu	system menu button (i.e. restore, close, etc.).
    \value SC_TitleBarMinButton  minimize button.
    \value SC_TitleBarMaxButton  maximize button.
    \value SC_TitleBarCloseButton  close button.
    \value SC_TitleBarLabel  window title label.
    \value SC_TitleBarNormalButton  normal (restore) button.
    \value SC_TitleBarShadeButton  shade button.
    \value SC_TitleBarUnshadeButton  unshade button.


    \value SC_ListView  the list view area.
    \value SC_ListViewBranch  (internal)
    \value SC_ListViewExpand  expand item (i.e. show/hide child items).


    \value SC_All  special value that matches all SubControls.


    \sa ComplexControl
*/

/*!
    \fn void TQStyle::drawComplexControl( ComplexControl control, TQPainter *p, const TQStyleControlElementData &ceData, ControlElementFlags elementFlags, const TQRect &r, const TQColorGroup &cg, SFlags how, SCFlags sub, SCFlags subActive, const TQStyleOption& opt, const TQWidget *widget ) const

    Draws the ComplexControl \a control using the painter \a p in the
    area \a r. Colors are used from the color group \a cg. The \a sub
    argument specifies which SubControls to draw. Multiple SubControls
    can be OR'ed together. The \a subActive argument specifies which
    SubControl is active.

    The rect \a r should be in logical coordinates. Reimplementations
    of this function should use visualRect() to change the logical
    coordinates into screen coordinates when using drawPrimitive() and
    drawControl().

    \a ceData and \a elementFlags provide additional information about
    the widget for which the PrimitiveElement is being drawn.

    The \a how argument is used to control how the ComplexControl is
    drawn. Multiple flags can OR'ed together. See the table below for
    an explanation of which flags are used with the various
    ComplexControls.

    The \a widget argument is a pointer to a TQWidget or one of its
    subclasses. The widget can be cast to the appropriate type based
    on the value of \a control. Note that usage of \a widget is
    deprecated in favor of \a ceData and \a elementFlags.  The \a opt
    argument can be used to pass extra information required when
    drawing the ComplexControl. Note that \a opt may be the default
    value even for ComplexControls that can make use of the extra
    options. See the table below for the appropriate \a widget and
    \a opt usage:

    \table
    \header \i ComplexControl<br>\& Widget Cast
	    \i Style Flags
	    \i Notes
	    \i Options
	    \i Notes

    \row \i12 \l{CC_SpinWidget}(const TQSpinWidget *)
	 \i \l Style_Enabled \i Set if the spinwidget is enabled.
	 \i12 Unused.
	 \i12 &nbsp;
    \row \i \l Style_HasFocus \i Set if the spinwidget has input focus.

    \row \i12 \l{CC_ComboBox}(const \l TQComboBox *)
	 \i \l Style_Enabled \i Set if the combobox is enabled.
	 \i12 Unused.
	 \i12 &nbsp;
    \row \i \l Style_HasFocus \i Set if the combobox has input focus.

    \row \i12 \l{CC_ScrollBar}(const \l TQScrollBar *)
	 \i \l Style_Enabled \i Set if the scrollbar is enabled.
	 \i12 Unused.
	 \i12 &nbsp;
    \row \i \l Style_HasFocus \i Set if the scrollbar has input focus.

    \row \i12 \l{CC_Slider}(const \l TQSlider *)
	 \i \l Style_Enabled \i Set if the slider is enabled.
	 \i12 Unused.
	 \i12 &nbsp;

    \row \i \l Style_HasFocus \i Set if the slider has input focus.

    \row \i16 \l{CC_ToolButton}(const \l TQToolButton *)
	 \i \l Style_Enabled \i Set if the toolbutton is enabled.
	 \i16 \l TQStyleOption ( \l ArrowType t )
		\list
		\i opt.\link TQStyleOption::arrowType() arrowType\endlink()
		\endlist
	 \i16 When the tool button only contains an arrow, \e t is the
		arrow's type.
    \row \i \l Style_HasFocus \i Set if the toolbutton has input focus.
    \row \i \l Style_Down \i Set if the toolbutton is down (ie. mouse
	button or space pressed).
    \row \i \l Style_On \i Set if the toolbutton is a toggle button
	and is toggled on.
    \row \i \l Style_AutoRaise \i Set if the toolbutton has auto-raise enabled.
    \row \i \l Style_Raised \i Set if the button is not down, not on and doesn't
	contain the mouse when auto-raise is enabled.

    \row \i \l{CC_TitleBar}(const \l TQWidget *)
	 \i \l Style_Enabled \i Set if the titlebar is enabled.
	 \i Unused.
	 \i &nbsp;

    \row \i \l{CC_ListView}(const \l TQListView *)
	 \i \l Style_Enabled \i Set if the titlebar is enabled.
	 \i \l TQStyleOption ( \l TQListViewItem *item )
	    \list
	    \i opt.\link TQStyleOption::listViewItem() listViewItem\endlink()
	    \endlist
	 \i \e item is the item that needs branches drawn
    \endtable

    \sa ComplexControl, SubControl
*/

/*!
    \fn void TQStyle::drawComplexControlMask( ComplexControl control, TQPainter *p, const TQStyleControlElementData &ceData, const ControlElementFlags elementFlags, const TQRect &r, const TQStyleOption& opt, const TQWidget *widget) const

    Draw a bitmask for the ComplexControl \a control using the painter
    \a p in the area \a r. See drawComplexControl() for an explanation
    of the use of the \a widget and \a opt arguments.

    \a ceData and \a elementFlags provide additional information about
    the widget for which the PrimitiveElement is being drawn.

    The rect \a r should be in logical coordinates. Reimplementations
    of this function should use visualRect() to change the logical
    corrdinates into screen coordinates when using drawPrimitive() and
    drawControl().

    Note that usage of \a widget is deprecated in favor of \a ceData and \a elementFlags.

    \sa drawComplexControl() ComplexControl
*/

/*!
    \fn TQRect TQStyle::querySubControlMetrics( ComplexControl control, const TQStyleControlElementData &ceData, ControlElementFlags elementFlags, SubControl subcontrol, const TQStyleOption& opt = TQStyleOption::Default, const TQWidget *widget = 0 ) const;

    Returns the rect for the SubControl \a subcontrol for \a widget in
    logical coordinates.

    \a ceData and \a elementFlags provide additional information about
    the widget for which the PrimitiveElement is being drawn.

    The \a widget argument is a pointer to a TQWidget or one of its
    subclasses. The widget can be cast to the appropriate type based
    on the value of \a control. The \a opt argument can be used to
    pass extra information required when drawing the ComplexControl.
    Note that \a opt may be the default value even for ComplexControls
    that can make use of the extra options. See drawComplexControl()
    for an explanation of the \a widget and \a opt arguments.

    Note that usage of \a widget is deprecated in favor of \a ceData and \a elementFlags.

    \sa drawComplexControl(), ComplexControl, SubControl
*/

/*!
    \fn SubControl TQStyle::querySubControl( ComplexControl control, const TQStyleControlElementData &ceData, ControlElementFlags elementFlags, const TQPoint &pos, const TQStyleOption& opt = TQStyleOption::Default, const TQWidget *widget = 0 ) const;

    Returns the SubControl for \a widget at the point \a pos. The \a
    widget argument is a pointer to a TQWidget or one of its
    subclasses. The widget can be cast to the appropriate type based
    on the value of \a control. The \a opt argument can be used to
    pass extra information required when drawing the ComplexControl.
    Note that \a opt may be the default value even for ComplexControls
    that can make use of the extra options. See drawComplexControl()
    for an explanation of the \a widget and \a opt arguments.

    \a ceData and \a elementFlags provide additional information about
    the widget for which the PrimitiveElement is being drawn.

    Note that \a pos is passed in screen coordinates. When using
    querySubControlMetrics() to check for hits and misses, use
    visualRect() to change the logical coordinates into screen
    coordinates.

    Note that usage of \a widget is deprecated in favor of \a ceData and \a elementFlags.

    \sa drawComplexControl(), ComplexControl, SubControl, querySubControlMetrics()
*/

/*!
    \enum TQStyle::PixelMetric

    This enum represents a PixelMetric. A PixelMetric is a style
    dependent size represented as a single pixel value.

    \value PM_ButtonMargin  amount of whitespace between pushbutton
	labels and the frame.
    \value PM_ButtonDefaultIndicator  width of the default-button indicator frame.
    \value PM_MenuButtonIndicator  width of the menu button indicator
	proportional to the widget height.
    \value PM_ButtonShiftHorizontal  horizontal contents shift of a
	button when the button is down.
    \value PM_ButtonShiftVertical  vertical contents shift of a button when the
	button is down.

    \value PM_DefaultFrameWidth  default frame width, usually 2.
    \value PM_SpinBoxFrameWidth  frame width of a spin box.
    \value PM_MDIFrameWidth frame width of an MDI window.
    \value PM_MDIMinimizedWidth width of a minimized MSI window.

    \value PM_MaximumDragDistance  Some feels require the scrollbar or
	other sliders to jump back to the original position when the
	mouse pointer is too far away while dragging. A value of -1
	disables this behavior.

    \value PM_ScrollBarExtent  width of a vertical scrollbar and the
	height of a horizontal scrollbar.
    \value PM_ScrollBarSliderMin the minimum height of a vertical
	scrollbar's slider and the minimum width of a horiztonal
	scrollbar slider.

    \value PM_SliderThickness  total slider thickness.
    \value PM_SliderControlThickness  thickness of the slider handle.
    \value PM_SliderLength length of the slider.
    \value PM_SliderTickmarkOffset the offset between the tickmarks
	and the slider.
    \value PM_SliderSpaceAvailable  the available space for the slider to move.

    \value PM_DockWindowSeparatorExtent  width of a separator in a
	horiztonal dock window and the height of a separator in a
	vertical dock window.
    \value PM_DockWindowHandleExtent  width of the handle in a
	horizontal dock window and the height of the handle in a
	vertical dock window.
    \value PM_DockWindowFrameWidth  frame width of a dock window.

    \value PM_MenuBarFrameWidth  frame width of a menubar.

    \value PM_MenuBarItemSpacing  spacing between menubar items.
    \value PM_ToolBarItemSpacing  spacing between toolbar items.

    \value PM_TabBarTabOverlap number of pixels the tabs should overlap.
    \value PM_TabBarTabHSpace extra space added to the tab width.
    \value PM_TabBarTabVSpace extra space added to the tab height.
    \value PM_TabBarBaseHeight height of the area between the tab bar
	and the tab pages.
    \value PM_TabBarBaseOverlap number of pixels the tab bar overlaps
	the tab bar base.
    \value PM_TabBarScrollButtonWidth
    \value PM_TabBarTabShiftHorizontal horizontal pixel shift when a
        tab is selected.
    \value PM_TabBarTabShiftVertical vertical pixel shift when a
        tab is selected.

    \value PM_ProgressBarChunkWidth  width of a chunk in a progress bar indicator.

    \value PM_SplitterWidth  width of a splitter.

    \value PM_TitleBarHeight height of the title bar.
    \value PM_PopupMenuFrameHorizontalExtra additional border, e.g. for panels
    \value PM_PopupMenuFrameVerticalExtra additional border, e.g. for panels

    \value PM_IndicatorWidth  width of a check box indicator.
    \value PM_IndicatorHeight  height of a checkbox indicator.
    \value PM_ExclusiveIndicatorWidth  width of a radio button indicator.
    \value PM_ExclusiveIndicatorHeight  height of a radio button indicator.

    \value PM_PopupMenuScrollerHeight height of the scroller area in a popupmenu.
    \value PM_PopupMenuScrollerHeight height of the scroller area in a popupmenu.
    \value PM_CheckListButtonSize area (width/height) of the
	checkbox/radiobutton in a TQCheckListItem
    \value PM_CheckListControllerSize area (width/height) of the
	controller in a TQCheckListItem

    \value PM_DialogButtonsSeparator distance between buttons in a dialog buttons widget.
    \value PM_DialogButtonsButtonWidth minimum width of a button in a dialog buttons widget.
    \value PM_DialogButtonsButtonHeight minimum height of a button in a dialog buttons widget.

    \value PM_HeaderMarkSize
    \value PM_HeaderGripMargin
    \value PM_HeaderMargin

    \value PM_CustomBase  base value for custom ControlElements. All
	values above this are reserved for custom use. Therefore,
	custom values must be greater than this value.


    \sa pixelMetric()
*/

/*!
    \fn int TQStyle::pixelMetric( PixelMetric metric, const TQStyleControlElementData &ceData, ControlElementFlags elementFlags, const TQWidget *widget = 0 ) const;

    Returns the pixel metric for \a metric.

    \a ceData and \a elementFlags provide additional information about
    the widget for which the PrimitiveElement is being drawn.

    The \a widget argument is a pointer to a TQWidget or one of its
    subclasses. The widget can be cast to the appropriate type based
    on the value of \a metric. Note that \a widget may be zero even
    for PixelMetrics that can make use of \a widget. Note also that
    usage of \a widget is deprecated in favor of \a ceData and
    \a elementFlags.  See the table below for the appropriate
    \a widget casts:

    \table
    \header \i PixelMetric \i Widget Cast
    \row \i \l PM_SliderControlThickness    \i (const \l TQSlider *)
    \row \i \l PM_SliderLength		    \i (const \l TQSlider *)
    \row \i \l PM_SliderTickmarkOffset	    \i (const \l TQSlider *)
    \row \i \l PM_SliderSpaceAvailable	    \i (const \l TQSlider *)
    \row \i \l PM_TabBarTabOverlap	    \i (const \l TQTabBar *)
    \row \i \l PM_TabBarTabHSpace	    \i (const \l TQTabBar *)
    \row \i \l PM_TabBarTabVSpace	    \i (const \l TQTabBar *)
    \row \i \l PM_TabBarBaseHeight	    \i (const \l TQTabBar *)
    \row \i \l PM_TabBarBaseOverlap	    \i (const \l TQTabBar *)
    \endtable
*/

/*!
    \enum TQStyle::ContentsType

    This enum represents a ContentsType. It is used to calculate sizes
    for the contents of various widgets.

    \value CT_PushButton
    \value CT_CheckBox
    \value CT_RadioButton
    \value CT_ToolButton
    \value CT_ComboBox
    \value CT_Splitter
    \value CT_DockWindow
    \value CT_ProgressBar
    \value CT_PopupMenuItem
    \value CT_TabBarTab
    \value CT_Slider
    \value CT_Header
    \value CT_LineEdit
    \value CT_MenuBar
    \value CT_SpinBox
    \value CT_SizeGrip
    \value CT_TabWidget
    \value CT_DialogButtons

    \value CT_CustomBase  base value for custom ControlElements. All
	values above this are reserved for custom use. Custom values
	must be greater than this value.

    \sa sizeFromContents()
*/

/*!
    \fn TQSize TQStyle::sizeFromContents( ContentsType contents, const TQStyleControlElementData &ceData, ControlElementFlags elementFlags, const TQSize &contentsSize, const TQStyleOption& opt = TQStyleOption::Default, const TQWidget *widget = 0 ) const;

    Returns the size of \a widget based on the contents size \a
    contentsSize.

    \a ceData and \a elementFlags provide additional information about
    the widget for which the PrimitiveElement is being drawn.

    The \a widget argument is a pointer to a TQWidget or one of its
    subclasses. The widget can be cast to the appropriate type based
    on the value of \a contents. The \a opt argument can be used to
    pass extra information required when calculating the size. Note
    that \a opt may be the default value even for ContentsTypes that
    can make use of the extra options. Note that usage of \a widget
    is deprecated in favor of \a ceData and \a elementFlags. See the
    table below for the appropriate \a widget and \a opt usage:

    \table
    \header \i ContentsType \i Widget Cast \i Options \i Notes
    \row \i \l CT_PushButton \i (const \l TQPushButton *) \i Unused. \i &nbsp;
    \row \i \l CT_CheckBox \i (const \l TQCheckBox *) \i Unused. \i &nbsp;
    \row \i \l CT_RadioButton \i (const \l TQRadioButton *) \i Unused. \i &nbsp;
    \row \i \l CT_ToolButton \i (const \l TQToolButton *) \i Unused. \i &nbsp;
    \row \i \l CT_ComboBox \i (const \l TQComboBox *) \i Unused. \i &nbsp;
    \row \i \l CT_Splitter \i (const \l TQSplitter *) \i Unused. \i &nbsp;
    \row \i \l CT_DockWindow \i (const \l TQDockWindow *) \i Unused. \i &nbsp;
    \row \i \l CT_ProgressBar \i (const \l TQProgressBar *) \i Unused. \i &nbsp;
    \row \i \l CT_PopupMenuItem \i (const TQPopupMenu *)
	\i \l TQStyleOption ( TQMenuItem *mi )
	    \list
	    \i opt.\link TQStyleOption::menuItem() menuItem\endlink()
	    \endlist
	\i \e mi is the menu item to use when calculating the size.
	    TQMenuItem is currently an internal class.
    \endtable
*/

/*!
    \enum TQStyle::StyleHint

    This enum represents a StyleHint. A StyleHint is a general look
    and/or feel hint.

    \value SH_EtchDisabledText disabled text is "etched" like Windows.

    \value SH_GUIStyle the GUI style to use.

    \value SH_ScrollBar_BackgroundMode  the background mode for a
	TQScrollBar. Possible values are any of those in the \link
	TQt::BackgroundMode BackgroundMode\endlink enum.

    \value SH_ScrollBar_MiddleClickAbsolutePosition  a boolean value.
	If true, middle clicking on a scrollbar causes the slider to
	jump to that position. If false, the middle clicking is
	ignored.

    \value SH_ScrollBar_LeftClickAbsolutePosition  a boolean value.
	If true, left clicking on a scrollbar causes the slider to
	jump to that position. If false, the left clicking will
	behave as appropriate for each control.

    \value SH_ScrollBar_ScrollWhenPointerLeavesControl  a boolean
	value. If true, when clicking a scrollbar SubControl, holding
	the mouse button down and moving the pointer outside the
	SubControl, the scrollbar continues to scroll. If false, the
	scrollbar stops scrolling when the pointer leaves the
	SubControl.

    \value SH_TabBar_Alignment  the alignment for tabs in a
	TQTabWidget. Possible values are \c TQt::AlignLeft, \c
	TQt::AlignCenter and \c TQt::AlignRight.

    \value SH_Header_ArrowAlignment the placement of the sorting
	indicator may appear in list or table headers. Possible values
	are \c TQt::Left or \c TQt::Right.

    \value SH_Slider_SnapToValue  sliders snap to values while moving,
	like Windows

    \value SH_Slider_SloppyKeyEvents  key presses handled in a sloppy
	manner, i.e. left on a vertical slider subtracts a line.

    \value SH_ProgressDialog_CenterCancelButton  center button on
	progress dialogs, like Motif, otherwise right aligned.

    \value SH_ProgressDialog_TextLabelAlignment TQt::AlignmentFlags --
	text label alignment in progress dialogs; Center on windows,
	Auto|VCenter otherwise.

    \value SH_PrintDialog_RightAlignButtons  right align buttons in
	the print dialog, like Windows.

    \value SH_MainWindow_SpaceBelowMenuBar 1 or 2 pixel space between
	the menubar and the dockarea, like Windows.

    \value SH_FontDialog_SelectAssociatedText select the text in the
	line edit, or when selecting an item from the listbox, or when
	the line edit receives focus, like Windows.

    \value SH_PopupMenu_AllowActiveAndDisabled  allows disabled menu
	items to be active.

    \value SH_PopupMenu_SpaceActivatesItem  pressing Space activates
	the item, like Motif.

    \value SH_PopupMenu_SubMenuPopupDelay  the number of milliseconds
	to wait before opening a submenu; 256 on windows, 96 on Motif.

    \value SH_PopupMenu_Scrollable whether popupmenu's must support
	scrolling.

    \value SH_PopupMenu_SloppySubMenus whether popupmenu's must support
	sloppy submenu; as implemented on Mac OS.

    \value SH_ScrollView_FrameOnlyAroundContents  whether scrollviews
	draw their frame only around contents (like Motif), or around
	contents, scrollbars and corner widgets (like Windows).

    \value SH_MenuBar_AltKeyNavigation  menubars items are navigable
	by pressing Alt, followed by using the arrow keys to select
	the desired item.

    \value SH_ComboBox_ListMouseTracking  mouse tracking in combobox
	dropdown lists.

    \value SH_PopupMenu_MouseTracking  mouse tracking in popup menus.

    \value SH_MenuBar_MouseTracking  mouse tracking in menubars.

    \value SH_ItemView_ChangeHighlightOnFocus  gray out selected items
	when losing focus.

    \value SH_Widget_ShareActivation turn on sharing activation with
	floating modeless dialogs.

    \value SH_TabBar_SelectMouseType which type of mouse event should
	cause a tab to be selected.

    \value SH_ListViewExpand_SelectMouseType which type of mouse event should
	cause a listview expansion to be selected.

    \value SH_TabBar_PreferNoArrows whether a tabbar should suggest a size
        to prevent scroll arrows.

    \value SH_ComboBox_Popup allows popups as a combobox dropdown
	menu.

    \value SH_Workspace_FillSpaceOnMaximize the workspace should
	maximize the client area.

    \value SH_TitleBar_NoBorder the titlebar has no border

    \value SH_ScrollBar_StopMouseOverSlider stops autorepeat when
	slider reaches mouse

    \value SH_BlinkCursorWhenTextSelected whether cursor should blink
	when text is selected

    \value SH_RichText_FullWidthSelection whether richtext selections
	should extend the full width of the document.

    \value SH_GroupBox_TextLabelVerticalAlignment how to vertically align a
        groupbox's text label.

    \value SH_GroupBox_TextLabelColor how to paint a groupbox's text label.

    \value SH_DialogButtons_DefaultButton which buttons gets the
        default status in a dialog's button widget.

    \value SH_CustomBase  base value for custom ControlElements. All
	values above this are reserved for custom use. Therefore,
	custom values must be greater than this value.

    \value SH_ToolButton_Uses3D indicates whether TQToolButtons should
    use a 3D frame when the mouse is over them

    \value SH_ToolBox_SelectedPageTitleBold Boldness of the selected
    page title in a TQToolBox.

    \value SH_LineEdit_PasswordCharacter The TQChar Unicode character
    to be used for passwords.

    \value SH_Table_GridLineColor

    \value SH_UnderlineAccelerator whether accelerators are underlined

    \sa styleHint()
*/

/*!
    \fn int TQStyle::styleHint( StyleHint stylehint, const TQStyleControlElementData &ceData, ControlElementFlags elementFlags, const TQStyleOption &opt = TQStyleOption::Default, TQStyleHintReturn *returnData = 0, const TQWidget *widget = 0 ) const;

    Returns the style hint \a stylehint for \a widget. Currently, \a
    widget, \a opt, and \a returnData are unused; they're included to
    allow for future enhancements.

    \a ceData and \a elementFlags provide additional information about
    the widget for which the PrimitiveElement is being drawn.

    Note that usage of \a widget is deprecated in favor of \a ceData
    and \a elementFlags.

    For an explanation of the return value see \l StyleHint.
*/

/*!
    \enum TQStyle::StylePixmap

    This enum represents a StylePixmap. A StylePixmap is a pixmap that
    can follow some existing GUI style or guideline.


    \value SP_TitleBarMinButton  minimize button on titlebars. For
	example, in a TQWorkspace.
    \value SP_TitleBarMaxButton  maximize button on titlebars.
    \value SP_TitleBarCloseButton  close button on titlebars.
    \value SP_TitleBarNormalButton  normal (restore) button on titlebars.
    \value SP_TitleBarShadeButton  shade button on titlebars.
    \value SP_TitleBarUnshadeButton  unshade button on titlebars.
    \value SP_MessageBoxInformation the 'information' icon.
    \value SP_MessageBoxWarning the 'warning' icon.
    \value SP_MessageBoxCritical the 'critical' icon.
    \value SP_MessageBoxQuestion the 'question' icon.


    \value SP_DockWindowCloseButton  close button on dock windows;
	see also TQDockWindow.


    \value SP_CustomBase  base value for custom ControlElements. All
	values above this are reserved for custom use. Therefore,
	custom values must be greater than this value.

    \sa stylePixmap()
*/

/*!
    \fn TQPixmap TQStyle::stylePixmap( StylePixmap stylepixmap, const TQStyleControlElementData &ceData, ControlElementFlags elementFlags, const TQStyleOption& opt = TQStyleOption::Default, const TQWidget *widget = 0 ) const;

    Returns a pixmap for \a stylepixmap.

    \a ceData and \a elementFlags provide additional information about
    the widget for which the PrimitiveElement is being drawn.

    The \a opt argument can be used to pass extra information required
    when drawing the ControlElement. Note that \a opt may be the
    default value even for StylePixmaps that can make use of the extra
    options. Currently, the \a opt argument is unused.

    The \a widget argument is a pointer to a TQWidget or one of its
    subclasses. The widget can be cast to the appropriate type based
    on the value of \a stylepixmap. Note that usage of \a widget is
    deprecated in favor of \a ceData and \a elementFlags.See the table
    below for the appropriate \a widget casts:

    \table
    \header \i StylePixmap \i Widget Cast
    \row \i \l SP_TitleBarMinButton	\i (const \l TQWidget *)
    \row \i \l SP_TitleBarMaxButton	\i (const \l TQWidget *)
    \row \i \l SP_TitleBarCloseButton	\i (const \l TQWidget *)
    \row \i \l SP_TitleBarNormalButton	\i (const \l TQWidget *)
    \row \i \l SP_TitleBarShadeButton	\i (const \l TQWidget *)
    \row \i \l SP_TitleBarUnshadeButton \i (const \l TQWidget *)
    \row \i \l SP_DockWindowCloseButton \i (const \l TQDockWindow *)
    \endtable

    \sa StylePixmap
*/

/*!
    \fn TQRect TQStyle::visualRect( const TQRect &logical, const TQWidget *w );

    Returns the rect \a logical in screen coordinates. The bounding
    rect for widget \a w is used to perform the translation. This
    function is provided to aid style implementors in supporting
    right-to-left mode.

    Note that this function is deprecated in favor of visualRect( const TQRect &logical, const TQStyleControlElementData &ceData, const ControlElementFlags elementFlags );

    \sa TQApplication::reverseLayout()
*/
TQRect TQStyle::visualRect( const TQRect &logical, const TQWidget *w )
{
    TQStyleControlElementData ceData;
    ceData.rect = w->rect();
    return visualRect(logical, ceData, CEF_None);
}

/*!
    \fn TQRect TQStyle::visualRect( const TQRect &logical, const TQStyleControlElementData &ceData, const ControlElementFlags elementFlags );

    Returns the rect \a logical in screen coordinates. The bounding
    rect for the widget described by \a ceData and \a elementFlags
    is used to perform the translation. This function is provided to
    aid style implementors in supporting
    right-to-left mode.

    \sa TQApplication::reverseLayout()
*/
TQRect TQStyle::visualRect( const TQRect &logical, const TQStyleControlElementData &ceData, const ControlElementFlags elementFlags )
{
    Q_UNUSED(elementFlags)

    TQRect boundingRect = ceData.rect;
    TQRect r = logical;
    if ( TQApplication::reverseLayout() )
	r.moveBy( 2*(boundingRect.right() - logical.right()) +
		  logical.width() - boundingRect.width(), 0 );
    return r;
}

/*!
    \overload TQRect TQStyle::visualRect( const TQRect &logical, const TQRect &bounding );

    Returns the rect \a logical in screen coordinates. The rect \a
    bounding is used to perform the translation. This function is
    provided to aid style implementors in supporting right-to-left
    mode.

    \sa TQApplication::reverseLayout()
*/
TQRect TQStyle::visualRect( const TQRect &logical, const TQRect &boundingRect )
{
    TQRect r = logical;
    if ( TQApplication::reverseLayout() )
	r.moveBy( 2*(boundingRect.right() - logical.right()) +
		  logical.width() - boundingRect.width(), 0 );
    return r;
}

/*!
    \fn void TQStyle::installObjectEventHandler( const TQStyleControlElementData &ceData, ControlElementFlags elementFlags, void* source, TQStyle* handler );

    Intercepts events generated by \a source and sends them to \a handler via
    the bool TQStyle::objectEventHandler( const TQStyleControlElementData &ceData, ControlElementFlags elementFlags, void* source, TQEvent *e ) virtual method.

    \sa void TQStyle::removeObjectEventHandler( TQObject* source, TQStyle* handler )
    \sa bool TQStyle::objectEventHandler( const TQStyleControlElementData &ceData, ControlElementFlags elementFlags, void* source, TQEvent *e )
*/
void TQStyle::installObjectEventHandler( const TQStyleControlElementData &ceData, ControlElementFlags elementFlags, void* source, TQStyle* handler ) {
	bool cbret = false;
	if (m_eventHandlerInstallationHook) {
		cbret = (*m_eventHandlerInstallationHook)(ceData, elementFlags, source, handler);
	}
	if (!cbret) {
		if (ceData.widgetObjectTypes.contains("TQObject")) {
			TQObject* o = reinterpret_cast<TQObject*>(source);
			o->installEventFilter(this);
			m_objectEventSourceToHandlerMap[source] = handler;
			m_objectEventSourceDataToHandlerMap[source] = ceData;
			m_objectEventSourceFlagsToHandlerMap[source] = elementFlags;
		}
	}
}

/*!
    \fn void TQStyle::removeObjectEventHandler( const TQStyleControlElementData &ceData, ControlElementFlags elementFlags, void* source, TQStyle* handler );

    Stops intercepting events generated by \a source.

    \sa void TQStyle::installObjectEventHandler( const TQStyleControlElementData &ceData, ControlElementFlags elementFlags, void* source, TQStyle* handler )
*/
void TQStyle::removeObjectEventHandler( const TQStyleControlElementData &ceData, ControlElementFlags elementFlags, void* source, TQStyle* handler ) {
	bool cbret = false;
	if (m_eventHandlerRemovalHook) {
		cbret = (*m_eventHandlerRemovalHook)(ceData, elementFlags, source, handler);
	}
	if (!cbret) {
		if (ceData.widgetObjectTypes.contains("TQObject")) {
			TQObject* o = reinterpret_cast<TQObject*>(source);
			m_objectEventSourceToHandlerMap.remove(source);
			m_objectEventSourceDataToHandlerMap.remove(source);
			m_objectEventSourceFlagsToHandlerMap.remove(source);
			o->removeEventFilter(this);
		}
	}
}

/*!
    \fn void TQStyle::setEventHandlerInstallationHook( EventHandlerInstallationHook hook );

    Sets a callback function \a hook which will be called whenever a new intercept request
    is made via the TQStyle::installObjectEventHandler method.  The callback function must
    use this definition: bool callbackFunction( const TQStyleControlElementData &ceData, ControlElementFlags elementFlags, void* source, TQStyle* handler ).

    \sa void TQStyle::installObjectEventHandler( const TQStyleControlElementData &ceData, ControlElementFlags elementFlags, void* source, TQStyle* handler )
*/
void TQStyle::setEventHandlerInstallationHook( EventHandlerInstallationHook hook ) {
	m_eventHandlerInstallationHook = hook;
}

/*!
    \fn void TQStyle::setEventHandlerRemovalHook( EventHandlerRemovalHook hook );

    Sets a callback function \a hook which will be called whenever a new intercept deactivation request
    is made via the TQStyle::removeObjectEventHandler method.  The callback function must
    use this definition: bool callbackFunction( const TQStyleControlElementData &ceData, ControlElementFlags elementFlags, void* source, TQStyle* handler ).

    \sa void TQStyle::removeObjectEventHandler( const TQStyleControlElementData &ceData, ControlElementFlags elementFlags, void* source, TQStyle* handler )
*/
void TQStyle::setEventHandlerRemovalHook( EventHandlerRemovalHook hook ) {
	m_eventHandlerRemovalHook = hook;
}

/*!
    \fn bool TQStyle::objectEventHandler( const TQStyleControlElementData &ceData, ControlElementFlags elementFlags, void* source, TQEvent *e );

    Override this virtual function to intercept events requested by a previous call to
    TQStyle::installObjectEventHandler.

    \sa void TQStyle::installObjectEventHandler( const TQStyleControlElementData &ceData, ControlElementFlags elementFlags, void* source, TQStyle* handler )
    \sa void TQStyle::removeObjectEventHandler( const TQStyleControlElementData &ceData, ControlElementFlags elementFlags, void* source, TQStyle* handler )
*/
bool TQStyle::objectEventHandler( const TQStyleControlElementData &ceData, ControlElementFlags elementFlags, void* source, TQEvent *e ) {
	Q_UNUSED(ceData);
	Q_UNUSED(elementFlags);
	Q_UNUSED(source);
	Q_UNUSED(e);
	return false;
}

/*!
    \fn bool TQStyle::eventFilter(TQObject *o, TQEvent *e);
    \internal
*/
bool TQStyle::eventFilter(TQObject *o, TQEvent *e) {
	acceleratorKeypressEventMonitor(o, e);

	if (m_objectEventSourceToHandlerMap.contains(o)) {
		TQStyle* handler = m_objectEventSourceToHandlerMap[o];
		const TQStyleControlElementData &ceData = m_objectEventSourceDataToHandlerMap[o];
		ControlElementFlags elementFlags = m_objectEventSourceFlagsToHandlerMap[o];
		bool ret;
		TQWidget* w = dynamic_cast<TQWidget*>(o);
		if (w) {
			// Update ceData as widget parameters may have changed
			// If not done glitches may appear such as those present in Bug 1816
			populateControlElementDataFromWidget(w, TQStyleOption());
			const TQStyleControlElementData &widgetCEData = *(w->controlElementDataObject());
			if (e->type() == TQEvent::Paint) {
				TQPainter p(w);
				TQPainter* activePainterOrig = widgetCEData.activePainter;
				const_cast<TQStyleControlElementData&>(widgetCEData).activePainter = &p;
				ret = handler->objectEventHandler(widgetCEData, elementFlags, o, e);
				const_cast<TQStyleControlElementData&>(widgetCEData).activePainter = activePainterOrig;
			}
			else {
				ret = handler->objectEventHandler(widgetCEData, elementFlags, o, e);
			}
		}
		else {
			ret = handler->objectEventHandler(ceData, elementFlags, o, e);
		}
		if (ret) {
			return ret;
		}
		else {
			return TQObject::eventFilter(o, e);
		}
	}
	else {
		return TQObject::eventFilter(o, e);
	}
}

/*!
    \fn void TQStyle::setWidgetActionRequestHook( WidgetActionRequestHook hook );

    Sets a callback function \a hook which will be called whenever a new widget action request
    is made via the TQStyle::installObjectEventHandler method.  The callback function must
    use this definition: bool callbackFunction( const TQStyleControlElementData &ceData, ControlElementFlags elementFlags, void* source, TQStyle* handler ).

    \sa void TQStyle::installObjectEventHandler( const TQStyleControlElementData &ceData, ControlElementFlags elementFlags, void* source, TQStyle* handler )
*/
void TQStyle::setWidgetActionRequestHook( WidgetActionRequestHook hook ) {
	m_widgetActionRequestHook = hook;
}

/*!
    \fn bool widgetActionRequestHandler( const TQStyleControlElementData &ceData, ControlElementFlags elementFlags, void* source, WidgetActionRequest request );

    Handles widget action requests.  Return false to continue processing in base classes, true to eat the request and halt processing.
*/
bool TQStyle::widgetActionRequest( const TQStyleControlElementData &ceData, ControlElementFlags elementFlags, void* source, WidgetActionRequest request, TQStyleWidgetActionRequestData requestData ) {
	bool cbret = false;
	if (m_widgetActionRequestHook) {
		cbret = (*m_widgetActionRequestHook)(ceData, elementFlags, source, request, requestData);
	}
	if (!cbret) {
		if (ceData.widgetObjectTypes.contains("TQWidget")) {
			TQWidget* widget = reinterpret_cast<TQWidget*>(source);
			if (request == WAR_Repaint) {
				widget->repaint(false);
			}
			else if (request == WAR_RepaintRect) {
				widget->repaint(requestData.rect, false);
			}
			else if (request == WAR_EnableMouseTracking) {
				widget->setMouseTracking(true);
			}
			else if (request == WAR_DisableMouseTracking) {
				widget->setMouseTracking(false);
			}
			else if (request == WAR_SetAutoMask) {
				widget->setAutoMask(true);
			}
			else if (request == WAR_UnSetAutoMask) {
				widget->setAutoMask(false);
			}
			else if (request == WAR_SetCheckable) {
				TQPopupMenu *pm = dynamic_cast<TQPopupMenu*>(widget);
				if (pm) {
					pm->setCheckable(true);
				}
			}
			else if (request == WAR_UnSetCheckable) {
				TQPopupMenu *pm = dynamic_cast<TQPopupMenu*>(widget);
				if (pm) {
					pm->setCheckable(false);
				}
			}
			else if (request == WAR_FrameSetStyle) {
				TQFrame* frame = dynamic_cast<TQFrame*>(widget);
				if (frame) {
					frame->setFrameStyle(requestData.metric1);
				}
			}
			else if (request == WAR_FrameSetLineWidth) {
				TQFrame* frame = dynamic_cast<TQFrame*>(widget);
				if (frame) {
					frame->setLineWidth(requestData.metric1);
				}
			}
			else if (request == WAR_SetLayoutMargin) {
				TQLayout* layout = widget->layout();
				if (layout) {
					layout->setMargin(requestData.metric1);
				}
			}
			else if (request == WAR_SetPalette) {
				widget->setPalette(requestData.palette);
			}
			else if (request == WAR_SetBackgroundMode) {
				widget->setBackgroundMode((TQt::BackgroundMode)requestData.metric1);
			}
			else if (request == WAR_SetBackgroundOrigin) {
				widget->setBackgroundOrigin((TQWidget::BackgroundOrigin)requestData.metric1);
			}
			else if (request == WAR_SetFont) {
				widget->setFont(requestData.font);
			}
			else if (request == WAR_RepaintAllAccelerators) {
				TQWidgetList *list = TQApplication::topLevelWidgets();
				TQWidgetListIt it( *list );
				TQWidget * widget;
				while ((widget=it.current()) != 0) {
					++it;

					TQObjectList *l = widget->queryList("TQWidget");
					TQObjectListIt it2( *l );
					TQWidget *w;
					while ( (w = (TQWidget*)it2.current()) != 0 ) {
						++it2;
						if (w->isTopLevel() || !w->isVisible() || w->style().styleHint(SH_UnderlineAccelerator, TQStyleControlElementData(), CEF_None, w)) {
							l->removeRef(w);
						}
					}

					// Repaint all relevant widgets
					it2.toFirst();
					while ( (w = (TQWidget*)it2.current()) != 0 ) {
						++it2;
						w->repaint(false);
					}
					delete l;
				}
    				delete list;
			}
			else if (request == WAR_SetDefault) {
				TQPushButton *button = dynamic_cast<TQPushButton*>(widget);
				if (button) {
					button->setDefault(true);
				}
			}
			else if (request == WAR_UnSetDefault) {
				TQPushButton *button = dynamic_cast<TQPushButton*>(widget);
				if (button) {
					button->setDefault(false);
				}
			}
			else if (request == WAR_SendPaintEvent) {
				static_cast<TQObject*>(widget)->event(requestData.paintEvent);
			}
			return true;
		}
	}
	return true;
}

/*!
    \fn void TQStyle::setApplicationActionRequestHook( ApplicationActionRequestHook hook );

    Sets a callback function \a hook which will be called whenever a new application action request
    is made via the TQStyle::installObjectEventHandler method.  The callback function must
    use this definition: bool callbackFunction( const TQStyleControlElementData &ceData, ControlElementFlags elementFlags, void* source, TQStyle* handler ).

    \sa void TQStyle::installObjectEventHandler( const TQStyleControlElementData &ceData, ControlElementFlags elementFlags, void* source, TQStyle* handler )
*/
void TQStyle::setApplicationActionRequestHook( ApplicationActionRequestHook hook ) {
	m_applicationActionRequestHook = hook;
}

/*!
    \fn bool applicationActionRequestHandler( const TQStyleControlElementData &ceData, ControlElementFlags elementFlags, void* source, ApplicationActionRequest request );

    Handles application action requests.  Return false to continue processing in base classes, true to eat the request and halt processing.
*/
bool TQStyle::applicationActionRequest( const TQStyleControlElementData &ceData, ControlElementFlags elementFlags, void* source, ApplicationActionRequest request, TQStyleApplicationActionRequestData requestData ) {
	bool cbret = false;
	if (m_applicationActionRequestHook) {
		cbret = (*m_applicationActionRequestHook)(ceData, elementFlags, source, request, requestData);
	}
	if (!cbret) {
		if (ceData.widgetObjectTypes.contains("TQApplication")) {
			TQApplication* application = reinterpret_cast<TQApplication*>(source);
			if (request == AAR_SetPalette) {
				application->setPalette(requestData.palette, requestData.bool1, requestData.cstr);
			}
			else if (request == AAR_SetFont) {
				application->setFont(requestData.font, requestData.bool1, requestData.cstr);
			}
			return true;
		}
	}
	return true;
}

void TQStyle::acceleratorKeypressEventMonitor( TQObject *o, TQEvent *e ) {
	if (styleHint(SH_HideUnderlineAcceleratorWhenAltUp, TQStyleControlElementData(), CEF_None, TQStyleOption::Default, NULL, NULL) != 0) {
		TQWidget *widget = dynamic_cast<TQWidget*>(o);
		if (widget) {
			switch(e->type()) {
				case TQEvent::KeyPress:
					if (((TQKeyEvent*)e)->key() == Key_Alt) {
						conditionalAcceleratorsEnabled = true;
						widgetActionRequest(TQStyleControlElementData(), CEF_None, o, WAR_RepaintAllAccelerators);
					}
					break;
				case TQEvent::KeyRelease:
					if (((TQKeyEvent*)e)->key() == Key_Alt) {
						conditionalAcceleratorsEnabled = false;
						widgetActionRequest(TQStyleControlElementData(), CEF_None, o, WAR_RepaintAllAccelerators);
					}
					break;
				default:
					break;
			}
		}
	}
	else {
		conditionalAcceleratorsEnabled = false;
	}
}

bool TQStyle::acceleratorsShown() const {
	if (styleHint(SH_HideUnderlineAcceleratorWhenAltUp, TQStyleControlElementData(), CEF_None, TQStyleOption::Default, NULL, NULL) != 0) {
			return conditionalAcceleratorsEnabled;
	}
	else {
		return true;
	}
}

TQStyleWidgetActionRequestData::TQStyleWidgetActionRequestData() {
	//
}
TQStyleWidgetActionRequestData::TQStyleWidgetActionRequestData(int param1, int param2) {
	metric1 = param1;
	metric2 = param2;
}

TQStyleWidgetActionRequestData::TQStyleWidgetActionRequestData(TQPalette param, bool informWidgets, const char* className) {
	palette = param;
	bool1 = informWidgets;
	cstr = className;
}

TQStyleWidgetActionRequestData::TQStyleWidgetActionRequestData(TQFont param, bool informWidgets, const char* className) {
	font = param;
	bool1 = informWidgets;
	cstr = className;
}

TQStyleWidgetActionRequestData::TQStyleWidgetActionRequestData(TQRect param) {
	rect = param;
}

TQStyleWidgetActionRequestData::TQStyleWidgetActionRequestData(TQPaintEvent* param) {
	paintEvent = param;
}

TQStyleWidgetActionRequestData::~TQStyleWidgetActionRequestData() {
	//
}

// ============================================================================================
// Interface methods
// ============================================================================================

void TQStyle::drawPrimitive( PrimitiveElement pe,
				  TQPainter *p,
				  const TQRect &r,
				  const TQColorGroup &cg,
				  SFlags flags,
				  const TQStyleOption& opt ) const
{
	const TQWidget* widget = 0;

	// Determine if the request needs widget information and set widget appropriately
	// FIXME
	if ((pe == PE_CheckListController) || (pe == PE_CheckListExclusiveIndicator) || (pe == PE_CheckListIndicator)) {
		TQCheckListItem *item = opt.checkListItem();
		if (item) {
			TQListView *lv = item->listView();
			widget = dynamic_cast<TQWidget*>(lv);
		}
	}

	const TQStyleControlElementData &ceData = populateControlElementDataFromWidget(widget, opt);
	drawPrimitive(pe, p, ceData, getControlElementFlagsForObject(widget, TQStyleOption()), r, cg, flags, opt);
}

void TQStyle::drawControl( ControlElement element,
				TQPainter *p,
				const TQWidget *widget,
				const TQRect &r,
				const TQColorGroup &cg,
				SFlags flags,
				const TQStyleOption& opt ) const
{
#if defined(QT_CHECK_STATE)
	if (!widget) {
		tqWarning("TQStyle::drawControl: widget parameter cannot be zero!");
		return;
	}
#endif

	const TQStyleControlElementData &ceData = populateControlElementDataFromWidget(widget, opt);
	drawControl(element, p, ceData, getControlElementFlagsForObject(widget, opt), r, cg, flags, opt, widget);
}

void TQStyle::drawControlMask( ControlElement control,
				    TQPainter *p,
				    const TQWidget *widget,
				    const TQRect &r,
				    const TQStyleOption& opt ) const
{
	const TQStyleControlElementData &ceData = populateControlElementDataFromWidget(widget, opt);
	drawControlMask(control, p, ceData, getControlElementFlagsForObject(widget, opt), r, opt, widget);
}

TQRect TQStyle::subRect(SubRect r, const TQWidget *widget) const
{
#if defined(QT_CHECK_STATE)
	if (! widget) {
		tqWarning("TQStyle::subRect: widget parameter cannot be zero!");
		return TQRect();
	}
#endif

	const TQStyleControlElementData &ceData = populateControlElementDataFromWidget(widget, TQStyleOption());
	return subRect(r, ceData, getControlElementFlagsForObject(widget, TQStyleOption()), widget);
}

void TQStyle::drawComplexControl( ComplexControl control,
				       TQPainter *p,
				       const TQWidget *widget,
				       const TQRect &r,
				       const TQColorGroup &cg,
				       SFlags flags,
				       SCFlags controls,
				       SCFlags active,
				       const TQStyleOption& opt ) const
{
#if defined(QT_CHECK_STATE)
	if (! widget) {
		tqWarning("TQStyle::drawComplexControl: widget parameter cannot be zero!");
		return;
	}
#endif

	const TQStyleControlElementData &ceData = populateControlElementDataFromWidget(widget, opt);
	drawComplexControl(control, p, ceData, getControlElementFlagsForObject(widget, TQStyleOption()), r, cg, flags, controls, active, opt, widget);
}

void TQStyle::drawComplexControlMask( ComplexControl control,
					   TQPainter *p,
					   const TQWidget *widget,
					   const TQRect &r,
					   const TQStyleOption& opt ) const
{
	const TQStyleControlElementData &ceData = populateControlElementDataFromWidget(widget, opt);
	drawComplexControlMask(control, p, ceData, getControlElementFlagsForObject(widget, TQStyleOption()), r, opt, widget);
}

TQRect TQStyle::querySubControlMetrics( ComplexControl control,
					    const TQWidget *widget,
					    SubControl sc,
					    const TQStyleOption &opt ) const
{
#if defined(QT_CHECK_STATE)
	if (! widget) {
		tqWarning("TQStyle::querySubControlMetrics: widget parameter cannot be zero!");
		return TQRect();
	}
#endif

	const TQStyleControlElementData &ceData = populateControlElementDataFromWidget(widget, opt, false);
	return querySubControlMetrics(control, ceData, getControlElementFlagsForObject(widget, TQStyleOption()), sc, opt, widget);
}

TQStyle::SubControl TQStyle::querySubControl(ComplexControl control,
						 const TQWidget *widget,
						 const TQPoint &pos,
						 const TQStyleOption& opt ) const
{
	const TQStyleControlElementData &ceData = populateControlElementDataFromWidget(widget, opt);
	return querySubControl(control, ceData, getControlElementFlagsForObject(widget, opt), pos, opt, widget);
}

int TQStyle::pixelMetric(PixelMetric m, const TQWidget *widget) const
{
	const TQStyleControlElementData &ceData = populateControlElementDataFromWidget(widget, TQStyleOption(), false);
	return pixelMetric(m, ceData, getControlElementFlagsForObject(widget, TQStyleOption()), widget);
}

TQSize TQStyle::sizeFromContents(ContentsType contents,
				     const TQWidget *widget,
				     const TQSize &contentsSize,
				     const TQStyleOption& opt ) const
{
	TQSize sz(contentsSize);

#if defined(QT_CHECK_STATE)
	if (! widget) {
		tqWarning("TQStyle::sizeFromContents: widget parameter cannot be zero!");
		return sz;
	}
#endif

	const TQStyleControlElementData &ceData = populateControlElementDataFromWidget(widget, opt);
	return sizeFromContents(contents, ceData, getControlElementFlagsForObject(widget, TQStyleOption()), contentsSize, opt, widget);
}

int TQStyle::styleHint(StyleHint sh, const TQWidget * w, const TQStyleOption &so, TQStyleHintReturn *shr) const
{
	bool ceDataNotNeeded = false;
	if (sh == SH_Widget_ShareActivation) {
		ceDataNotNeeded = true;
	}
	const TQStyleControlElementData &ceData = populateControlElementDataFromWidget(w, so, false, ceDataNotNeeded);
	return styleHint(sh, ceData, getControlElementFlagsForObject(w, TQStyleOption(), false), so, shr, w);
}

TQPixmap TQStyle::stylePixmap(StylePixmap sp, const TQWidget *w, const TQStyleOption &so) const
{
	const TQStyleControlElementData &ceData = populateControlElementDataFromWidget(w, so);
	return stylePixmap(sp, ceData, getControlElementFlagsForObject(w, TQStyleOption()), so, w);
}

/*!
  \fn int TQStyle::defaultFrameWidth() const
  \obsolete
*/

/*!
  \fn void TQStyle::tabbarMetrics( const TQWidget *, int &, int &, int & ) const
  \obsolete
*/

/*!
  \fn TQSize TQStyle::scrollBarExtent() const
  \obsolete
*/

#ifndef TQT_NO_COMPAT
int TQStyle::defaultFrameWidth() const
{
	TQStyleControlElementData ceData;
	return pixelMetric( PM_DefaultFrameWidth, ceData, CEF_None );
}

void TQStyle::tabbarMetrics( const TQWidget* t, int& hf, int& vf, int& ov ) const
{
	TQStyleControlElementData ceData;
	hf = pixelMetric( PM_TabBarTabHSpace, ceData, CEF_None, t );
	vf = pixelMetric( PM_TabBarTabVSpace, ceData, CEF_None, t );
	ov = pixelMetric( PM_TabBarBaseOverlap, ceData, CEF_None, t );
}

TQSize TQStyle::scrollBarExtent() const
{
	TQStyleControlElementData ceData;
	ceData.orientation = TQt::Horizontal;
	return TQSize(pixelMetric(PM_ScrollBarExtent, ceData, CEF_None),
			pixelMetric(PM_ScrollBarExtent, ceData, CEF_None));
}
#endif

TQStyleControlElementData::TQStyleControlElementData() {
	isNull = true;
	activePainter = 0;
	tickMarkSetting = 0;
	comboBoxLineEditFlags = 0;
	frameStyle = 0;
	comboBoxListBoxFlags = 0;
	parentWidgetFlags = 0;
	topLevelWidgetFlags = 0;
}

TQStyleControlElementData::~TQStyleControlElementData() {
	//
}

TQStyleControlElementData::TQStyleControlElementData(const TQStyleControlElementData& orig) {
	isNull = orig.isNull;
	widgetObjectTypes = orig.widgetObjectTypes;
	allDataPopulated = orig.allDataPopulated;
	wflags = orig.wflags;
	windowState = orig.windowState;
	bgPixmap = orig.bgPixmap;
	bgBrush = orig.bgBrush;
	bgColor = orig.bgColor;
	bgOffset = orig.bgOffset;
	backgroundMode = orig.backgroundMode;
	fgPixmap = orig.fgPixmap;
	fgColor = orig.fgColor;
	colorGroup = orig.colorGroup;
	geometry = orig.geometry;
	rect = orig.rect;
	pos = orig.pos;
	icon = orig.icon;
	iconSet = orig.iconSet;
	text = orig.text;
	orientation = orig.orientation;
	activeItemPaletteBgColor = orig.activeItemPaletteBgColor;
	palette = orig.palette;
	totalSteps = orig.totalSteps;
	currentStep = orig.currentStep;
	tickMarkSetting = orig.tickMarkSetting;
	tickInterval = orig.tickInterval;
	minSteps = orig.minSteps;
	maxSteps = orig.maxSteps;
	startStep = orig.startStep;
	pageStep = orig.pageStep;
	lineStep = orig.lineStep;
	dlgVisibleButtons = orig.dlgVisibleButtons;
	dlgVisibleSizeHints = orig.dlgVisibleSizeHints;
	progressText = orig.progressText;
	textLabel = orig.textLabel;
	font = orig.font;
	percentageVisible = orig.percentageVisible;
	dwData = orig.dwData;
	toolButtonTextPosition = orig.toolButtonTextPosition;
	popupDelay = orig.popupDelay;
	titleBarData = orig.titleBarData;
	spinWidgetData = orig.spinWidgetData;
	parentWidgetData = orig.parentWidgetData;
	viewportData = orig.viewportData;
	listViewData = orig.listViewData;
	tabBarData = orig.tabBarData;
	checkListItemData = orig.checkListItemData;
	comboBoxLineEditFlags = orig.comboBoxLineEditFlags;
	frameStyle = orig.frameStyle;
	sliderRect = orig.sliderRect;
	activePainter = orig.activePainter;
	toolBarData = orig.toolBarData;
	comboBoxListBoxFlags = orig.comboBoxListBoxFlags;
	paletteBgColor = orig.paletteBgColor;
	parentWidgetFlags = orig.parentWidgetFlags;
	name = orig.name;
	caption = orig.caption;
	topLevelWidgetData = orig.topLevelWidgetData;
	topLevelWidgetFlags = orig.topLevelWidgetFlags;
	paletteBgPixmap = orig.paletteBgPixmap;
}

#endif // TQT_NO_STYLE
