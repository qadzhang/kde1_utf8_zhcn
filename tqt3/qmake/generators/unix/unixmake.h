/****************************************************************************
**
** Definition of UnixMakefileGenerator class.
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of qmake.
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

#ifndef __UNIXMAKE_H__
#define __UNIXMAKE_H__

#include "makefile.h"

class UnixMakefileGenerator : public MakefileGenerator
{
    bool init_flag, include_deps;
    bool writeMakefile(TQTextStream &);
    void writeExtraVariables(TQTextStream &);
    TQString libtoolFileName();
    void writeLibtoolFile();     // for libtool
    TQString pkgConfigPrefix() const;
    TQString pkgConfigFileName();
    TQString pkgConfigFixPath(TQString) const;
    void writePkgConfigFile();   // for pkg-config
    TQStringList combineSetLFlags(const TQStringList &list1, const TQStringList &list2);
    void writePrlFile(TQTextStream &);

public:
    UnixMakefileGenerator(TQMakeProject *p);
    ~UnixMakefileGenerator();

protected:
    virtual bool doPrecompiledHeaders() const { return project->isActiveConfig("precompile_header"); }
    virtual bool doDepends() const { return !include_deps && MakefileGenerator::doDepends(); }
    virtual TQString defaultInstall(const TQString &);
    virtual void processPrlVariable(const TQString &, const TQStringList &);
    virtual void processPrlFiles();

    virtual bool findLibraries();
    virtual TQString findDependency(const TQString &);
    virtual TQStringList &findDependencies(const TQString &);
    virtual void init();

    void writeMakeParts(TQTextStream &);
    void writeSubdirs(TQTextStream &, bool=true);
    
private:
    void init2();
};

inline UnixMakefileGenerator::~UnixMakefileGenerator()
{ }


#endif /* __UNIXMAKE_H__ */
