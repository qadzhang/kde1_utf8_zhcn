/**********************************************************************
**
** Copyright (C) 2005-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of TQt Designer.
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
** Licensees holding valid TQt Commercial licenses may use this file in
** accordance with the TQt Commercial License Agreement provided with
** the Software.
**
** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE. Trolltech reserves all rights not granted
** herein.
**
**********************************************************************/

#ifndef RC2UI_H
#define RC2UI_H

#include <ntqtextstream.h>
#include <ntqfile.h>
#include <ntqstringlist.h>

class RC2UI
{
public:
    RC2UI( TQTextStream* input );
    ~RC2UI();

    bool parse();
    bool parse ( TQStringList& get );

    TQStringList targetFiles;

protected:
    enum WidgetType { IDWidget, IDPushButton, IDLabel, IDCheckBox, IDRadioButton, 
		      IDGroupBox, IDLineEdit, IDMultiLineEdit, IDIconView, IDListView,
		      IDProgressBar, IDTabWidget, IDSpinBox, IDSlider, IDComboBox, 
		      IDListBox, IDScrollBar, IDCustom, IDUnknown = 0x0100 };
    bool makeDialog();
    bool makeBitmap();
    bool makeStringTable();
    bool makeAccelerator();
    bool makeCursor();
    bool makeHTML();
    bool makeIcon();
    bool makeVersion();

    TQString line;
    TQTextStream *in;
    TQStringList target;

    void indent();
    void undent();

    TQString stripQM( const TQString& string );
    TQString parseNext( TQString& arg, char sep = ',' );
    TQStringList splitStyles( const TQString& styles, char sep = '|' );
    void wi();

    void writeClass( const TQString& name );
    void writeCString( const TQString& name, const TQString& value );
    void writeString( const TQString& name, const TQString& value );
    void writeRect( const TQString& name, int x, int y, int w, int h );
    void writeFont( const TQString& family, int pointsize );
    void writeBool( const TQString& name, bool value );
    void writeNumber( const TQString& name, int value );
    void writeEnum( const TQString& name, const TQString& value );
    void writeSet( const TQString& name, const TQString& value );

    void writeStyles( const TQStringList styles, bool isFrame );
private:
    int indentation;
    bool writeToFile;
    TQTextStream* out;

    TQString useName( const TQString& );

    TQStringList usedNames;

    const TQString blockStart1;
    const TQString blockStart2;
};

#endif
