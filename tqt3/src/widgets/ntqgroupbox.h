/**********************************************************************
**
** Definition of TQGroupBox widget class
**
** Created : 950203
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of the widgets module of the TQt GUI Toolkit.
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

#ifndef TQGROUPBOX_H
#define TQGROUPBOX_H

#ifndef QT_H
#include "ntqframe.h"
#endif // QT_H

#ifndef TQT_NO_GROUPBOX


class TQAccel;
class TQGroupBoxPrivate;
class TQVBoxLayout;
class TQGridLayout;
class TQSpacerItem;

class TQ_EXPORT TQGroupBox : public TQFrame
{
    TQ_OBJECT
    TQ_PROPERTY( TQString title READ title WRITE setTitle )
    TQ_PROPERTY( Alignment alignment READ alignment WRITE setAlignment )
    TQ_PROPERTY( Orientation orientation READ orientation WRITE setOrientation DESIGNABLE false )
    TQ_PROPERTY( int columns READ columns WRITE setColumns DESIGNABLE false )
    TQ_PROPERTY( bool flat READ isFlat WRITE setFlat )
#ifndef TQT_NO_CHECKBOX
    TQ_PROPERTY( bool checkable READ isCheckable WRITE setCheckable )
    TQ_PROPERTY( bool checked READ isChecked WRITE setChecked )
#endif
public:
    TQGroupBox( TQWidget* parent=0, const char* name=0 );
    TQGroupBox( const TQString &title,
	       TQWidget* parent=0, const char* name=0 );
    TQGroupBox( int strips, Orientation o,
	       TQWidget* parent=0, const char* name=0 );
    TQGroupBox( int strips, Orientation o, const TQString &title,
	       TQWidget* parent=0, const char* name=0 );
    ~TQGroupBox();

    virtual void setColumnLayout(int strips, Orientation o);

    TQString title() const { return str; }
    virtual void setTitle( const TQString &);

    int alignment() const { return align; }
    virtual void setAlignment( int );

    int columns() const;
    void setColumns( int );

    Orientation orientation() const { return dir; }
    void setOrientation( Orientation );

    int insideMargin() const;
    int insideSpacing() const;
    void setInsideMargin( int m );
    void setInsideSpacing( int s );

    void addSpace( int );
    TQSize sizeHint() const;

    bool isFlat() const;
    void setFlat( bool b );
    bool isCheckable() const;
#ifndef TQT_NO_CHECKBOX
    void setCheckable( bool b );
#endif
    bool isChecked() const;
    void setEnabled(bool on);

#ifndef TQT_NO_CHECKBOX
public slots:
    void setChecked( bool b );

signals:
    void toggled( bool );
#endif
protected:
    bool event( TQEvent * );
    void childEvent( TQChildEvent * );
    void resizeEvent( TQResizeEvent * );
    void paintEvent( TQPaintEvent * );
    void focusInEvent( TQFocusEvent * );
    void fontChange( const TQFont & );

private slots:
    void fixFocus();
    void setChildrenEnabled( bool b );

private:
    void skip();
    void init();
    void calculateFrame();
    void insertWid( TQWidget* );
    void setTextSpacer();
#ifndef TQT_NO_CHECKBOX
    void updateCheckBoxGeometry();
#endif
    TQString str;
    int align;
    int lenvisible;
#ifndef TQT_NO_ACCEL
    TQAccel * accel;
#endif
    TQGroupBoxPrivate * d;

    TQVBoxLayout *vbox;
    TQGridLayout *grid;
    int row;
    int col : 30;
    uint bFlat : 1;
    int nRows, nCols;
    Orientation dir;
    int spac, marg;

private:	// Disabled copy constructor and operator=
#if defined(TQ_DISABLE_COPY)
    TQGroupBox( const TQGroupBox & );
    TQGroupBox &operator=( const TQGroupBox & );
#endif
};


#endif // TQT_NO_GROUPBOX

#endif // TQGROUPBOX_H
