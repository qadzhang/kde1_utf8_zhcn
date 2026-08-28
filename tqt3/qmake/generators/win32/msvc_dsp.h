/****************************************************************************
**
** Definition of DspMakefileGenerator class.
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

#ifndef __MSVC_DSP_H__
#define __MSVC_DSP_H__

#include "winmakefile.h"
#include <ntqvaluestack.h>

class DspMakefileGenerator : public Win32MakefileGenerator
{
    TQString currentGroup;
    void beginGroupForFile(TQString file, TQTextStream &, const TQString& filter="");
    void endGroups(TQTextStream &);

    bool init_flag;
    bool writeDspParts(TQTextStream &);

    bool writeMakefile(TQTextStream &);
    TQString findTemplate(const TQString &file);
    void init();

public:
    DspMakefileGenerator(TQMakeProject *p);
    ~DspMakefileGenerator();

    bool openOutput(TQFile &file) const;

protected:
    virtual void processPrlVariable(const TQString &, const TQStringList &);
    virtual bool findLibraries();

    TQString precompH, 
	    precompObj, precompPch;
    bool usePCH;
};

inline DspMakefileGenerator::~DspMakefileGenerator()
{ }

inline bool DspMakefileGenerator::findLibraries()
{ return Win32MakefileGenerator::findLibraries("MSVCDSP_LIBS"); }

#endif /* __MSVC_DSP_H__ */
