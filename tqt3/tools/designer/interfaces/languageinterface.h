 /**********************************************************************
** Copyright (C) 2000-2008 Trolltech ASA.  All rights reserved.
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

#ifndef LANGUAGEINTERFACE_H
#define LANGUAGEINTERFACE_H

//
//  W A R N I N G  --  PRIVATE INTERFACES
//  --------------------------------------
//
// This file and the interfaces declared in the file are not
// public. It exists for internal purpose. This header file and
// interfaces may change from version to version (even binary
// incompatible) without notice, or even be removed.
//
// We mean it.
//
//

#include <private/qcom_p.h>
#include <ntqvaluelist.h>
#include <ntqstringlist.h>
#include <ntqmap.h>
#include <ntqstrlist.h>

// {f208499a-6f69-4883-9219-6e936e55a330}
#ifndef IID_Language
#define IID_Language TQUuid( 0xf208499a, 0x6f69, 0x4883, 0x92, 0x19, 0x6e, 0x93, 0x6e, 0x55, 0xa3, 0x30 )
#endif

struct LanguageInterface : public TQUnknownInterface
{
    struct Function
    {
	TQString name;
	TQString body;
	TQString returnType;
	TQString comments;
	int start;
	int end;
	TQString access;
	bool operator==( const Function &f ) const {
	    return ( name == f.name &&
		     body == f.body &&
		     returnType == f.returnType &&
		     comments == f.comments );
	}
    };

    struct Connection
    {
	TQString sender;
	TQString signal;
	TQString slot;
	bool operator==( const Connection &c ) const {
	    return ( sender == c.sender &&
		     signal == c.signal &&
		     slot == c.slot );
	}
    };

    enum Support
    {
	ReturnType,
	ConnectionsToCustomSlots,
	CompressProject
    };

    virtual void functions( const TQString &code, TQValueList<Function> *funcs ) const = 0;
    virtual void connections( const TQString &code, TQValueList<Connection> *connections ) const = 0;
    virtual TQString createFunctionStart( const TQString &className, const TQString &func,
					 const TQString &returnType, const TQString &access ) = 0;
    virtual TQString createArguments( const TQString &cpp_signature ) = 0;
    virtual TQString createEmptyFunction() = 0;
    virtual TQStringList definitions() const = 0;
    virtual TQStringList definitionEntries( const TQString &definition, TQUnknownInterface *designerIface ) const = 0;
    virtual void setDefinitionEntries( const TQString &definition, const TQStringList &entries, TQUnknownInterface *designerIface ) = 0;
    virtual bool supports( Support s ) const = 0;
    virtual TQStringList fileFilterList() const = 0;
    virtual TQStringList fileExtensionList() const = 0;
    virtual void preferedExtensions( TQMap<TQString, TQString> &extensionMap ) const = 0;
    virtual TQString projectKeyForExtension( const TQString &extension ) const = 0;
    virtual void sourceProjectKeys( TQStringList &keys ) const = 0;
    virtual TQString cleanSignature( const TQString &sig ) = 0;
    virtual void loadFormCode( const TQString &form, const TQString &filename,
			       TQValueList<Function> &functions,
			       TQStringList &vars,
			       TQValueList<Connection> &connections ) = 0;
    virtual TQString formCodeExtension() const = 0;

    virtual bool canConnect( const TQString &signal, const TQString &slot ) = 0;

    virtual 	void compressProject( const TQString &projectFile, const TQString &compressedFile,
				      bool withWarning ) = 0;
    virtual TQString uncompressProject( const TQString &projectFile, const TQString &destDir ) = 0;
    virtual TQString aboutText() const = 0;

    virtual void addConnection( const TQString &sender, const TQString &signal,
				const TQString &receiver, const TQString &slot,
				TQString *code ) = 0;
    virtual void removeConnection( const TQString &sender, const TQString &signal,
				   const TQString &receiver, const TQString &slot,
				   TQString *code ) = 0;
    virtual TQStrList signalNames( TQObject *obj ) const = 0;

};

#endif
