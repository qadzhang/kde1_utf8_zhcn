/****************************************************************************
**
** Definition of MakefileGenerator class.
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
#ifndef __MAKEFILE_H__
#define __MAKEFILE_H__

#include "option.h"
#include "project.h"
#include <ntqtextstream.h>

#ifdef Q_OS_WIN32
#define QT_POPEN _popen
#else
#define QT_POPEN popen
#endif

class MakefileGenerator
{
    TQString spec;
    bool init_opath_already, init_already, moc_aware, no_io;
    TQStringList createObjectList(const TQString &var);
    TQString build_args();
    TQString dependencyKey(const TQString &file) const;
    TQMap<TQString, bool> depProcessed;
    TQMap<TQString, TQString> depHeuristics, fileFixed;
    TQMap<TQString, TQString> mocablesToMOC, mocablesFromMOC;
    TQMap<TQString, TQStringList> depends;

protected:
    void writeObj(TQTextStream &, const TQString &obj, const TQString &src);
    void writeUicSrc(TQTextStream &, const TQString &ui);
    void writeMocObj(TQTextStream &, const TQString &obj, const TQString &src);
    void writeMocSrc(TQTextStream &, const TQString &src);
    void writeLexSrc(TQTextStream &, const TQString &lex);
    void writeYaccSrc(TQTextStream &, const TQString &yac);
    void writeInstalls(TQTextStream &t, const TQString &installs);
    void writeImageObj(TQTextStream &t, const TQString &obj);
    void writeImageSrc(TQTextStream &t, const TQString &images);

protected:

    TQMakeProject *project;

    class MakefileDependDir {
    public:
	MakefileDependDir(const TQString &r, const TQString &l) : real_dir(r), local_dir(l) { }
	TQString real_dir, local_dir;
    };
    bool generateDependencies(TQPtrList<MakefileDependDir> &dirs, const TQString &x, bool recurse);

    TQString buildArgs();

    TQString specdir();
    TQString cleanFilePath(const TQString &file) const;
    bool generateMocList(const TQString &fn);

    TQString findMocSource(const TQString &moc_file) const;
    TQString findMocDestination(const TQString &src_file) const;
    virtual TQStringList &findDependencies(const TQString &file);

    void setNoIO(bool o);
    bool noIO() const;

    void setMocAware(bool o);
    bool mocAware() const;
    void logicWarn(const TQString &, const TQString &);

    virtual bool doDepends() const { return Option::mkfile::do_deps; }
    bool writeHeader(TQTextStream &);
    virtual bool writeMakefile(TQTextStream &);
    virtual bool writeMakeQmake(TQTextStream &);
    void initOutPaths();
    virtual void init();

    //for cross-platform dependent directories
    virtual void usePlatformDir();

    //for installs
    virtual TQString defaultInstall(const TQString &);

    //for prl
    bool processPrlFile(TQString &);
    virtual void processPrlVariable(const TQString &, const TQStringList &);
    virtual void processPrlFiles();
    virtual void writePrlFile(TQTextStream &);

    //make sure libraries are found
    virtual bool findLibraries();
    virtual TQString findDependency(const TQString &);

    void setProcessedDependencies(const TQString &file, bool b);
    bool processedDependencies(const TQString &file);

    virtual TQString var(const TQString &var);
    TQString varGlue(const TQString &var, const TQString &before, const TQString &glue, const TQString &after);
    TQString varList(const TQString &var);
    TQString val(const TQStringList &varList);
    TQString valGlue(const TQStringList &varList, const TQString &before, const TQString &glue, const TQString &after);
    TQString valList(const TQStringList &varList);


    TQString fileFixify(const TQString& file, const TQString &out_dir=TQString::null, 
		       const TQString &in_dir=TQString::null, bool force_fix=false, bool canon=true) const;
    TQStringList fileFixify(const TQStringList& files, const TQString &out_dir=TQString::null, 
			   const TQString &in_dir=TQString::null, bool force_fix=false, bool canon=true) const;
public:
    MakefileGenerator(TQMakeProject *p);
    virtual ~MakefileGenerator();

    static MakefileGenerator *create(TQMakeProject *);
    virtual bool write();
    virtual bool openOutput(TQFile &) const;
};

inline TQString MakefileGenerator::findMocSource(const TQString &moc_file) const
{
    TQString tmp = cleanFilePath(moc_file);
    if (mocablesFromMOC.contains(tmp))
	return mocablesFromMOC[tmp];
    else
	return TQString("");
}

inline TQString MakefileGenerator::findMocDestination(const TQString &src_file) const
{
    TQString tmp = cleanFilePath(src_file);
    if (mocablesToMOC.contains(tmp))
	return mocablesToMOC[tmp];
    else
	return TQString("");
}

inline void MakefileGenerator::setMocAware(bool o)
{ moc_aware = o; }

inline bool MakefileGenerator::mocAware() const
{ return moc_aware; }

inline void MakefileGenerator::setNoIO(bool o)
{ no_io = o; }

inline bool MakefileGenerator::noIO() const
{ return no_io; }

inline TQString MakefileGenerator::defaultInstall(const TQString &)
{ return TQString(""); }

inline bool MakefileGenerator::findLibraries()
{ return true; }

inline TQString MakefileGenerator::findDependency(const TQString &)
{ return TQString(""); }

inline MakefileGenerator::~MakefileGenerator()
{ }

TQString mkdir_p_asstring(const TQString &dir);

#endif /* __MAKEFILE_H__ */
