/****************************************************************************
**
** Definition of the translator class
**
** Created : 980906
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


#ifndef TQTRANSLATOR_H
#define TQTRANSLATOR_H

#ifndef QT_H
#include "ntqobject.h"
#include "ntqvaluelist.h"
#endif // QT_H

#ifndef TQT_NO_TRANSLATION

class TQTranslatorPrivate;

class TQ_EXPORT TQTranslatorMessage
{
public:
    TQTranslatorMessage();
    TQTranslatorMessage( const char * context,
			const char * sourceText,
			const char * comment,
			const TQString& translation = TQString::null );
    TQTranslatorMessage( TQDataStream & );
    TQTranslatorMessage( const TQTranslatorMessage & m );

    TQTranslatorMessage & operator=( const TQTranslatorMessage & m );

    uint hash() const { return h; }
    const char *context() const { return cx; }
    const char *sourceText() const { return st; }
    const char *comment() const { return cm; }

    void setTranslation( const TQString & translation ) { tn = translation; }
    TQString translation() const { return tn; }

    enum Prefix { NoPrefix, Hash, HashContext, HashContextSourceText,
		  HashContextSourceTextComment };
    void write( TQDataStream & s, bool strip = false,
		Prefix prefix = HashContextSourceTextComment ) const;
    Prefix commonPrefix( const TQTranslatorMessage& ) const;

    bool operator==( const TQTranslatorMessage& m ) const;
    bool operator!=( const TQTranslatorMessage& m ) const
    { return !operator==( m ); }
    bool operator<( const TQTranslatorMessage& m ) const;
    bool operator<=( const TQTranslatorMessage& m ) const
    { return !m.operator<( *this ); }
    bool operator>( const TQTranslatorMessage& m ) const
    { return m.operator<( *this ); }
    bool operator>=( const TQTranslatorMessage& m ) const
    { return !operator<( m ); }

private:
    uint h;
    TQCString cx;
    TQCString st;
    TQCString cm;
    TQString tn;

    enum Tag { Tag_End = 1, Tag_SourceText16, Tag_Translation, Tag_Context16,
	       Tag_Hash, Tag_SourceText, Tag_Context, Tag_Comment,
	       Tag_Obsolete1 };
};


class TQ_EXPORT TQTranslator: public TQObject
{
    TQ_OBJECT
public:
    TQTranslator( TQObject * parent = 0, const char * name = 0 );
    ~TQTranslator();

#ifndef TQT_NO_COMPAT
    TQString find( const char *context, const char *sourceText, const char * comment = 0 ) const {
	return findMessage( context, sourceText, comment ).translation();
    }
#endif
    virtual TQTranslatorMessage findMessage( const char *, const char *,
					    const char * = 0 ) const;

    bool load( const TQString & filename,
	       const TQString & directory = TQString::null,
	       const TQString & search_delimiters = TQString::null,
	       const TQString & suffix = TQString::null );
    bool load( const uchar *data, int len ) {
	clear();
	return do_load( data, len );
    }

    void clear();

#ifndef TQT_NO_TRANSLATION_BUILDER
    enum SaveMode { Everything, Stripped };

    bool save( const TQString & filename, SaveMode mode = Everything );

    void insert( const TQTranslatorMessage& );
    void insert( const char *context, const char *sourceText, const TQString &translation ) {
	insert( TQTranslatorMessage(context, sourceText, "", translation) );
    }
    void remove( const TQTranslatorMessage& );
    void remove( const char *context, const char *sourceText ) {
	remove( TQTranslatorMessage(context, sourceText, "") );
    }
    bool contains( const char *, const char *, const char * comment = 0 ) const;

    void squeeze( SaveMode = Everything );
    void unsqueeze();

    TQValueList<TQTranslatorMessage> messages() const;
#endif

    bool isEmpty() const;

private:
#if defined(TQ_DISABLE_COPY)
    TQTranslator( const TQTranslator & );
    TQTranslator &operator=( const TQTranslator & );
#endif

    bool do_load( const uchar *data, int len );

    TQTranslatorPrivate * d;
};

#endif // TQT_NO_TRANSLATION

#endif
