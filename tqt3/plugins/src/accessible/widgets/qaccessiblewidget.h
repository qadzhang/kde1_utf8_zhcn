#ifndef TQACCESSIBLEWIDGET_H
#define TQACCESSIBLEWIDGET_H

#include <ntqaccessible.h>

class TQButton;
class TQScrollView;
class TQHeader;
class TQSpinWidget;
class TQScrollBar;
class TQSlider;
class TQListBox;
class TQListView;
class TQTextEdit;
class TQTabBar;
class TQComboBox;
class TQTitleBar;
class TQWidgetStack;

#ifndef TQT_NO_ICONVIEW
class TQIconView;
#endif


TQString buddyString( TQWidget * );
TQString stripAmp( const TQString& );
TQString hotKey( const TQString& );

class TQAccessibleWidget : public TQAccessibleObject
{
public:
    TQAccessibleWidget( TQObject *o, Role r = Client, TQString name = TQString::null, 
	TQString description = TQString::null, TQString value = TQString::null, 
	TQString help = TQString::null, TQString defAction = TQString::null,
	TQString accelerator = TQString::null, State s = Normal );

    ~TQAccessibleWidget();

    int		controlAt( int x, int y ) const;
    TQRect	rect( int control ) const;
    int		navigate( NavDirection direction, int startControl ) const;
    int		childCount() const;
    TQRESULT	queryChild( int control, TQAccessibleInterface ** ) const;
    TQRESULT	queryParent( TQAccessibleInterface ** ) const;

    TQString	text( Text t, int control ) const;
    void	setText( Text t, int control, const TQString &text );
    Role	role( int control ) const;
    State	state( int control ) const;

    bool	doDefaultAction( int control );
    bool	setFocus( int control );
    bool	setSelected( int control, bool on, bool extend );
    void	clearSelection();
    TQMemArray<int> selection() const;

    static ulong objects;

protected:
    TQWidget *widget() const;

private:
    Role role_;
    State state_;
    TQString name_;
    TQString description_;
    TQString value_;
    TQString help_;
    TQString defAction_;
    TQString accelerator_;
};

class TQAccessibleWidgetStack : public TQAccessibleWidget
{
public:
    TQAccessibleWidgetStack( TQObject *o );

    int		controlAt( int x, int y ) const;
    TQRESULT	queryChild( int control, TQAccessibleInterface ** ) const;

protected:
    TQWidgetStack *widgetStack() const;
};

class TQAccessibleButton : public TQAccessibleWidget
{
public:
    TQAccessibleButton( TQObject *o, Role r, TQString description = TQString::null,
	TQString help = TQString::null );

    TQString	text( Text t, int control ) const;
    State	state( int control ) const;

    bool	doDefaultAction( int control );

protected:
    TQButton *button() const;
};

class TQAccessibleRangeControl : public TQAccessibleWidget
{
public:
    TQAccessibleRangeControl( TQObject *o, Role role, TQString name = TQString::null, 
	TQString description = TQString::null, TQString help = TQString::null, 
	TQString defAction = TQString::null, TQString accelerator = TQString::null );

    TQString	text( Text t, int control ) const;
};

class TQAccessibleSpinWidget : public TQAccessibleRangeControl
{
public:
    TQAccessibleSpinWidget( TQObject *o );

    int		controlAt( int x, int y ) const;
    TQRect	rect( int control ) const;
    int		navigate( NavDirection direction, int startControl ) const;
    int		childCount() const;
    TQRESULT	queryChild( int control, TQAccessibleInterface ** ) const;

    TQString	text( Text t, int control ) const;
    Role	role( int control ) const;
    State	state( int control ) const;

    bool	doDefaultAction( int control );
};

class TQAccessibleScrollBar : public TQAccessibleRangeControl
{
public:
    TQAccessibleScrollBar( TQObject *o, TQString name = TQString::null, 
	TQString description = TQString::null, TQString help = TQString::null, 
	TQString defAction = TQString::null, TQString accelerator = TQString::null );

    int		controlAt( int x, int y ) const;
    TQRect	rect( int control ) const;
    int		navigate( NavDirection direction, int startControl ) const;
    int		childCount() const;
    TQRESULT	queryChild( int control, TQAccessibleInterface ** ) const;

    TQString	text( Text t, int control ) const;
    Role	role( int control ) const;

    bool	doDefaultAction( int control );

protected:
    TQScrollBar *scrollBar() const;
};

class TQAccessibleSlider : public TQAccessibleRangeControl
{
public:
    TQAccessibleSlider( TQObject *o, TQString name = TQString::null, 
	TQString description = TQString::null, TQString help = TQString::null, 
	TQString defAction = TQString::null, TQString accelerator = TQString::null );

    int		controlAt( int x, int y ) const;
    TQRect	rect( int control ) const;
    int		navigate( NavDirection direction, int startControl ) const;
    int		childCount() const;
    TQRESULT	queryChild( int control, TQAccessibleInterface ** ) const;

    TQString	text( Text t, int control ) const;
    Role	role( int control ) const;

    bool	doDefaultAction( int control );

protected:
    TQSlider *slider() const;
};

class TQAccessibleText : public TQAccessibleWidget
{
public:
    TQAccessibleText( TQObject *o, Role role, TQString name = TQString::null, 
	TQString description = TQString::null, TQString help = TQString::null, 
	TQString defAction = TQString::null, TQString accelerator = TQString::null );

    TQString	text( Text t, int control ) const;
    State	state( int control ) const;
    void        setText(Text t, int control, const TQString &text);
};

class TQAccessibleDisplay : public TQAccessibleWidget
{
public:
    TQAccessibleDisplay( TQObject *o, Role role, TQString description = TQString::null, 
	TQString value = TQString::null, TQString help = TQString::null, 
	TQString defAction = TQString::null, TQString accelerator = TQString::null );

    TQString	text( Text t, int control ) const;
    Role	role( int control ) const;
};

class TQAccessibleHeader : public TQAccessibleWidget
{
public:
    TQAccessibleHeader( TQObject *o, TQString description = TQString::null, 
	TQString value = TQString::null, TQString help = TQString::null, 
	TQString defAction = TQString::null, TQString accelerator = TQString::null );

    int		controlAt( int x, int y ) const;
    TQRect	rect( int control ) const;
    int		navigate( NavDirection direction, int startControl ) const;
    int		childCount() const;
    TQRESULT	queryChild( int control, TQAccessibleInterface ** ) const;

    TQString	text( Text t, int control ) const;

    Role role( int control ) const;
    State state( int control ) const;

protected:
    TQHeader *header() const;
};

class TQAccessibleTabBar : public TQAccessibleWidget
{
public:
    TQAccessibleTabBar( TQObject *o, TQString description = TQString::null, 
	TQString value = TQString::null, TQString help = TQString::null, 
	TQString defAction = TQString::null, TQString accelerator = TQString::null );

    int		controlAt( int x, int y ) const;
    TQRect	rect( int control ) const;
    int		navigate( NavDirection direction, int startControl ) const;
    int		childCount() const;
    TQRESULT	queryChild( int control, TQAccessibleInterface ** ) const;

    TQString	text( Text t, int control ) const;

    Role	role( int control ) const;
    State	state( int control ) const;

    bool	doDefaultAction( int control );
    bool	setSelected( int control, bool on, bool extend );
    void	clearSelection();
    TQMemArray<int> selection() const;    

protected:
    TQTabBar *tabBar() const;
};

class TQAccessibleComboBox : public TQAccessibleWidget
{
public:
    TQAccessibleComboBox( TQObject *o );

    int		controlAt( int x, int y ) const;
    TQRect	rect( int control ) const;
    int		navigate( NavDirection direction, int startControl ) const;
    int		childCount() const;
    TQRESULT	queryChild( int control, TQAccessibleInterface ** ) const;

    TQString	text( Text t, int control ) const;

    Role	role( int control ) const;
    State	state( int control ) const;

    bool	doDefaultAction( int control );

protected:
    TQComboBox *comboBox() const;
};

class TQAccessibleTitleBar : public TQAccessibleWidget
{
public:
    TQAccessibleTitleBar( TQObject *o );

    int		controlAt( int x, int y ) const;
    TQRect	rect( int control ) const;
    int		navigate( NavDirection direction, int startControl ) const;
    int		childCount() const;
    TQRESULT	queryChild( int control, TQAccessibleInterface ** ) const;

    TQString	text( Text t, int control ) const;
    Role	role( int control ) const;
    State	state( int control ) const;

    bool	doDefaultAction( int control );

protected:
    TQTitleBar *titleBar() const;
};

class TQAccessibleScrollView : public TQAccessibleWidget
{
public:
    TQAccessibleScrollView( TQObject *o, Role role, TQString name = TQString::null,
	TQString description = TQString::null, TQString value = TQString::null, 
	TQString help = TQString::null, TQString defAction = TQString::null, 
	TQString accelerator = TQString::null );

    TQString	text( Text t, int control ) const;

    virtual int itemAt( int x, int y ) const;
    virtual TQRect itemRect( int item ) const;
    virtual int itemCount() const;
};

class TQAccessibleViewport : public TQAccessibleWidget
{
public:
    TQAccessibleViewport( TQObject *o, TQObject *sv );

    int		controlAt( int x, int y ) const;
    TQRect	rect( int control ) const;
    int		navigate( NavDirection direction, int startControl ) const;
    int		childCount() const;

    TQString	text( Text t, int control ) const;
    Role	role( int control ) const;
    State	state( int control ) const;

    bool	doDefaultAction( int control );
    bool	setFocus( int control );
    bool	setSelected( int control, bool on, bool extend );
    void	clearSelection();
    TQMemArray<int> selection() const;

protected:
    TQAccessibleScrollView *scrollView() const;
    TQScrollView *scrollview;
};

class TQAccessibleListBox : public TQAccessibleScrollView
{
public:
    TQAccessibleListBox( TQObject *o );

    int		itemAt( int x, int y ) const;
    TQRect	itemRect( int item ) const;
    int		itemCount() const;

    TQString	text( Text t, int control ) const;
    Role	role( int control ) const;
    State	state( int control ) const;

    bool	setFocus( int control );
    bool	setSelected( int control, bool on, bool extend );
    void	clearSelection();
    TQMemArray<int> selection() const;

protected:
    TQListBox *listBox() const;
};

class TQAccessibleListView : public TQAccessibleScrollView
{
public:
    TQAccessibleListView( TQObject *o );

    int		itemAt( int x, int y ) const;
    TQRect	itemRect( int item ) const;
    int		itemCount() const;

    TQString	text( Text t, int control ) const;
    Role	role( int control ) const;
    State	state( int control ) const;

    bool	setFocus( int control );
    bool	setSelected( int control, bool on, bool extend );
    void	clearSelection();
    TQMemArray<int> selection() const;

protected:
    TQListView *listView() const;
};

#ifndef TQT_NO_ICONVIEW
class TQAccessibleIconView : public TQAccessibleScrollView
{
public:
    TQAccessibleIconView( TQObject *o );

    int		itemAt( int x, int y ) const;
    TQRect	itemRect( int item ) const;
    int		itemCount() const;

    TQString	text( Text t, int control ) const;
    Role	role( int control ) const;
    State	state( int control ) const;

    bool	setFocus( int control );
    bool	setSelected( int control, bool on, bool extend );
    void	clearSelection();
    TQMemArray<int> selection() const;

protected:
    TQIconView *iconView() const;
};
#endif

class TQAccessibleTextEdit : public TQAccessibleScrollView
{
public:
    TQAccessibleTextEdit( TQObject *o );

    int		itemAt( int x, int y ) const;
    TQRect	itemRect( int item ) const;
    int		itemCount() const;

    TQString	text( Text t, int control ) const;
    void        setText(Text t, int control, const TQString &text);
    Role	role( int control ) const;

protected:
    TQTextEdit *textEdit() const;
};

#endif // Q_ACESSIBLEWIDGET_H
