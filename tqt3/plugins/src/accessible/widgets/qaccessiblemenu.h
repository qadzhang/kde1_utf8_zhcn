#ifndef TQACCESSIBLEMENU_H
#define TQACCESSIBLEMENU_H

#include "qaccessiblewidget.h"

class TQPopupMenu;
class TQMenuBar;

class TQAccessiblePopup : public TQAccessibleWidget
{
public:
    TQAccessiblePopup( TQObject *o );

    int		childCount() const;
    TQRESULT	queryChild( int control, TQAccessibleInterface ** ) const;

    TQRect	rect( int control ) const;
    int		controlAt( int x, int y ) const;
    int		navigate( NavDirection direction, int control ) const;

    TQString	text( Text t, int control ) const;
    Role	role( int control ) const;
    State	state( int control ) const;

    bool	doDefaultAction( int control );
    bool	setFocus( int control );

protected:
    TQPopupMenu *popupMenu() const;
};

class TQAccessibleMenuBar : public TQAccessibleWidget
{
public:
    TQAccessibleMenuBar( TQObject *o );

    int		childCount() const;
    TQRESULT	queryChild( int control, TQAccessibleInterface ** ) const;

    TQRect	rect( int control ) const;
    int		controlAt( int x, int y ) const;
    int		navigate( NavDirection direction, int control ) const;

    TQString	text( Text t, int control ) const;
    Role	role( int control ) const;
    State	state( int control ) const;

    bool	doDefaultAction( int control );
    bool	setFocus( int control );

protected:
    TQMenuBar *menuBar() const;
};

#endif // TQACCESSIBLEMENU_H
