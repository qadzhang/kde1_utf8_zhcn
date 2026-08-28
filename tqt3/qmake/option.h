/****************************************************************************
**
** Definition of Option class.
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

#ifndef __OPTION_H__
#define __OPTION_H__

#include "project.h"
#include <ntqstring.h>
#include <ntqstringlist.h>
#include <ntqfile.h>

#define QMAKE_VERSION_MAJOR 1
#define QMAKE_VERSION_MINOR 7
#define QMAKE_VERSION_PATCH 0
const char *qmake_version();

void fixEnvVariables(TQString &x);
void debug_msg(int level, const char *fmt, ...);
enum TQMakeWarn {
    WarnNone    = 0x00,
    WarnParser  = 0x01,
    WarnLogic   = 0x02,
    WarnAll     = 0xFF
};
void warn_msg(TQMakeWarn t, const char *fmt, ...);

struct Option
{
    //simply global convenience
    static TQString libtool_ext;
    static TQString pkgcfg_ext;
    static TQString prf_ext;
    static TQString prl_ext;
    static TQString ui_ext;
    static TQStringList h_ext;
    static TQStringList cpp_ext;
    static TQString h_moc_ext;
    static TQString cpp_moc_ext;
    static TQString obj_ext;
    static TQString lex_ext;
    static TQString yacc_ext;
    static TQString h_moc_mod;
    static TQString cpp_moc_mod;
    static TQString lex_mod;
    static TQString yacc_mod;
    static TQString dir_sep;
    //both of these must be called..
    static bool parseCommandLine(int argc, char **argv); //parse cmdline
    static bool postProcessProject(TQMakeProject *); 

    //and convenience functions    
    static TQString fixPathToLocalOS(const TQString& in, bool fix_env=true, bool canonical=true);
    static TQString fixPathToTargetOS(const TQString& in, bool fix_env=true, bool canonical=true);

    //global qmake mode, can only be in one mode per invocation!
    enum QMAKE_MODE { QMAKE_GENERATE_NOTHING, QMAKE_GENERATE_PROJECT, QMAKE_GENERATE_MAKEFILE, 
		      QMAKE_GENERATE_PRL, QMAKE_SET_PROPERTY, QMAKE_QUERY_PROPERTY };
    static QMAKE_MODE qmake_mode;

    //all modes
    static TQFile output;
    static TQString output_dir;
    static int debug_level;
    static int warn_level;
    static TQStringList before_user_vars, after_user_vars;
    enum TARG_MODE { TARG_UNIX_MODE, TARG_WIN_MODE, TARG_MACX_MODE, TARG_MAC9_MODE, TARG_QNX6_MODE };
    static TARG_MODE target_mode;
    static TQString user_template, user_template_prefix;

    
    //QMAKE_*_PROPERTY options
    struct prop {
	static TQStringList properties;
    };

    //QMAKE_GENERATE_PROJECT options
    struct projfile {
	static bool do_pwd;
	static bool do_recursive;
	static TQStringList project_dirs;
    };

    //QMAKE_GENERATE_MAKEFILE options
    struct mkfile {
	static TQString qmakespec;
	static bool do_cache;
	static bool do_deps;
	static bool do_mocs;
	static bool do_dep_heuristics;
	static bool do_preprocess;
	static TQString cachefile;
	static int cachefile_depth;
	static TQStringList project_files;
	static TQString qmakespec_commandline;
    };

private:
    static int internalParseCommandLine(int, char **, int=0);
};


#endif /* __OPTION_H__ */
