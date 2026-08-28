/****************************************************************************
**
** Implementation of UnixMakefileGenerator class.
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

#include "unixmake.h"
#include "option.h"
#include "meta.h"
#include <ntqregexp.h>
#include <ntqfile.h>
#include <ntqdir.h>
#include <time.h>

TQString mkdir_p_asstring(const TQString &dir);

UnixMakefileGenerator::UnixMakefileGenerator(TQMakeProject *p) : MakefileGenerator(p), init_flag(false), include_deps(false)
{

}

void
UnixMakefileGenerator::writePrlFile(TQTextStream &t)
{
    MakefileGenerator::writePrlFile(t);
    // libtool support
    if(project->isActiveConfig("create_libtool") && project->first("TEMPLATE") == "lib") { //write .la
	if(project->isActiveConfig("compile_libtool"))
	    warn_msg(WarnLogic, "create_libtool specified with compile_libtool can lead to conflicting .la\n"
		     "formats, create_libtool has been disabled\n");
	else
	    writeLibtoolFile();
    }
    // pkg-config support
    if(project->isActiveConfig("create_pc") && project->first("TEMPLATE") == "lib")
	writePkgConfigFile();
}

bool
UnixMakefileGenerator::writeMakefile(TQTextStream &t)
{

    writeHeader(t);
    if(!project->variables()["QMAKE_FAILED_REQUIREMENTS"].isEmpty()) {
	t << "QMAKE    = "	<< (project->isEmpty("QMAKE_QMAKE") ? TQString("tqmake") : var("QMAKE_QMAKE")) << endl;
	{ //write the extra unix targets..
	    TQStringList &qut = project->variables()["QMAKE_EXTRA_UNIX_TARGETS"];
	    for(TQStringList::ConstIterator it = qut.begin(); it != qut.end(); ++it)
		t << *it << " ";
	}
	t << "all clean install distclean mocables uninstall uicables:" << "\n\t"
	  << "@echo \"Some of the required modules ("
	  << var("QMAKE_FAILED_REQUIREMENTS") << ") are not available.\"" << "\n\t"
	  << "@echo \"Skipped.\"" << endl << endl;
	writeMakeQmake(t);
	return true;
    }

    if (project->variables()["TEMPLATE"].first() == "app" ||
	project->variables()["TEMPLATE"].first() == "lib") {
	writeMakeParts(t);
	return MakefileGenerator::writeMakefile(t);
    } else if(project->variables()["TEMPLATE"].first() == "subdirs") {
	writeSubdirs(t);
	return true;
    }
    return false;
}

void
UnixMakefileGenerator::writeExtraVariables(TQTextStream &t)
{
    bool first = true;
    TQMap<TQString, TQStringList> &vars = project->variables();
    TQStringList &exports = project->variables()["QMAKE_EXTRA_UNIX_VARIABLES"];
    for(TQMap<TQString, TQStringList>::Iterator it = vars.begin(); it != vars.end(); ++it) {
	for(TQStringList::Iterator exp_it = exports.begin(); exp_it != exports.end(); ++exp_it) {
	    TQRegExp rx((*exp_it), false, true);
	    if(rx.exactMatch(it.key())) {
		if(first) {
		    t << "\n####### Custom Variables" << endl;
		    first = false;
		}
		t << "EXPORT_" << it.key() << " = " << it.data().join(" ") << endl;
	    }
	}
    }
    if(!first)
	t << endl;
}

void
UnixMakefileGenerator::writeMakeParts(TQTextStream &t)
{
    TQString deps = fileFixify(Option::output.name()), target_deps, prl;
    bool do_incremental = (project->isActiveConfig("incremental") &&
			   !project->variables()["QMAKE_INCREMENTAL"].isEmpty() &&
			   (!project->variables()["QMAKE_APP_FLAG"].isEmpty() ||
			    !project->isActiveConfig("staticlib"))),
	 src_incremental=false, moc_incremental=false;

    t << "####### Compiler, tools and options" << endl << endl;
    if (varGlue("INCLUDEPATH", " -I", " -I", "").find("$(LOCALBASE)") != -1)
    	t << "LOCALBASE?= /usr/local" << endl;
    t << "CC       = ";
    if (project->isActiveConfig("thread") &&
	! project->variables()["QMAKE_CC_THREAD"].isEmpty())
	t << var("QMAKE_CC_THREAD") << endl;
    else
	t << var("QMAKE_CC") << endl;

    t << "CXX      = ";
    if (project->isActiveConfig("thread") &&
	! project->variables()["QMAKE_CXX_THREAD"].isEmpty())
	t << var("QMAKE_CXX_THREAD") << endl;
    else
	t << var("QMAKE_CXX") << endl;

    t << "LEX      = " << var("QMAKE_LEX") << endl;
    t << "YACC     = " << var("QMAKE_YACC") << endl;
    t << "CFLAGS   = " << var("QMAKE_CFLAGS") << " "
      << varGlue("PRL_EXPORT_DEFINES","-D"," -D","") << " "
      << varGlue("DEFINES","-D"," -D","") << endl;
    t << "CXXFLAGS = " << var("QMAKE_CXXFLAGS") << " "
      << varGlue("PRL_EXPORT_DEFINES","-D"," -D","") << " "
      << varGlue("DEFINES","-D"," -D","") << endl;
    t << "LEXFLAGS = " << var("QMAKE_LEXFLAGS") << endl;
    t << "YACCFLAGS= " << var("QMAKE_YACCFLAGS") << endl;
    t << "INCPATH  = " << "-I" << specdir();
    if(!project->isActiveConfig("no_include_pwd")) {
	TQString pwd = fileFixify(TQDir::currentDirPath());
	if(pwd.isEmpty())
	    pwd = ".";
	t << " -I" << pwd;
    }
    t << varGlue("INCLUDEPATH"," -I", " -I", "") << endl;

    if(!project->isActiveConfig("staticlib")) {
	t << "LINK     = ";
	if (project->isActiveConfig("thread") &&
	    ! project->variables()["QMAKE_LINK_THREAD"].isEmpty())
	    t << var("QMAKE_LINK_THREAD") << endl;
	else
	    t << var("QMAKE_LINK") << endl;

	t << "LFLAGS   = " << var("QMAKE_LFLAGS") << endl;
	t << "LIBS     = " << "$(SUBLIBS) " << var("QMAKE_LIBDIR_FLAGS") << " " << var("QMAKE_LIBS") << endl;
    }

    t << "AR       = " << var("QMAKE_AR") << endl;
    t << "RANLIB   = " << var("QMAKE_RANLIB") << endl;
    t << "MOC      = " << var("QMAKE_MOC") << endl;
    t << "UIC      = "	<< var("QMAKE_UIC") << endl;
    t << "QMAKE    = "	<< (project->isEmpty("QMAKE_QMAKE") ? TQString("tqmake") : var("QMAKE_QMAKE")) << endl;
    t << "TAR      = "	<< var("QMAKE_TAR") << endl;
    t << "GZIP     = " << var("QMAKE_GZIP") << endl;
    if(project->isActiveConfig("compile_libtool"))
	t << "LIBTOOL	= " << var("QMAKE_LIBTOOL") << endl;
    t << "COPY     = " << var("QMAKE_COPY") << endl;
    t << "COPY_FILE= " << var("QMAKE_COPY_FILE") << endl;
    t << "COPY_DIR = " << var("QMAKE_COPY_DIR") << endl;
    t << "INSTALL_FILE= " << var("QMAKE_INSTALL_FILE") << endl;
    t << "INSTALL_DIR = " << var("QMAKE_INSTALL_DIR") << endl;

    t << "DEL_FILE = " << var("QMAKE_DEL_FILE") << endl;
    t << "SYMLINK  = " << var("QMAKE_SYMBOLIC_LINK") << endl;
    t << "DEL_DIR  = " << var("QMAKE_DEL_DIR") << endl;
    t << "MOVE     = " << var("QMAKE_MOVE") << endl;
    t << "CHK_DIR_EXISTS= " << var("QMAKE_CHK_DIR_EXISTS") << endl;
    t << "MKDIR    = " << var("QMAKE_MKDIR") << endl;
    t << endl;

    t << "####### Output directory" << endl << endl;
    if (! project->variables()["OBJECTS_DIR"].isEmpty())
	t << "OBJECTS_DIR = " << var("OBJECTS_DIR") << endl;
    else
	t << "OBJECTS_DIR = ./" << endl;
    t << endl;

    /* files */
    t << "####### Files" << endl << endl;
    t << "HEADERS = " << varList("HEADERS") << endl;
    t << "SOURCES = " << varList("SOURCES") << endl;
    if(do_incremental) {
	TQStringList &objs = project->variables()["OBJECTS"], &incrs = project->variables()["QMAKE_INCREMENTAL"], incrs_out;
	t << "OBJECTS = ";
	for(TQStringList::Iterator objit = objs.begin(); objit != objs.end(); ++objit) {
	    bool increment = false;
	    for(TQStringList::Iterator incrit = incrs.begin(); incrit != incrs.end(); ++incrit) {
		if((*objit).find(TQRegExp((*incrit), true, true)) != -1) {
		    increment = true;
		    incrs_out.append((*objit));
		    break;
		}
	    }
	    if(!increment)
		t << "\\\n\t\t" << (*objit);
	}
	if(incrs_out.count() == objs.count()) { //we just switched places, no real incrementals to be done!
	    t << incrs_out.join(" \\\n\t\t") << endl;
	} else if(!incrs_out.count()) {
	    t << endl;
	} else {
	    src_incremental = true;
	    t << endl;
	    t << "INCREMENTAL_OBJECTS = " << incrs_out.join(" \\\n\t\t") << endl;
	}
    } else {
	t << "OBJECTS = " << varList("OBJECTS") << endl;
    }
    t << "FORMS = " << varList("FORMS") << endl;
    t << "UICDECLS = " << varList("UICDECLS") << endl;
    t << "UICIMPLS = " << varList("UICIMPLS") << endl;
    TQString srcMoc = varList("SRCMOC"), objMoc = varList("OBJMOC");
    t << "SRCMOC   = " << srcMoc << endl;
    if(do_incremental) {
	TQStringList &objs = project->variables()["OBJMOC"],
		   &incrs = project->variables()["QMAKE_INCREMENTAL"], incrs_out;
	t << "OBJMOC = ";
	for(TQStringList::Iterator objit = objs.begin(); objit != objs.end(); ++objit) {
	    bool increment = false;
	    for(TQStringList::Iterator incrit = incrs.begin(); incrit != incrs.end(); ++incrit) {
		if((*objit).find(TQRegExp((*incrit), true, true)) != -1) {
		    increment = true;
		    incrs_out.append((*objit));
		    break;
		}
	    }
	    if(!increment)
		t << "\\\n\t\t" << (*objit);
	}
	if(incrs_out.count() == objs.count()) { //we just switched places, no real incrementals to be done!
	    t << incrs_out.join(" \\\n\t\t") << endl;
	} else if(!incrs_out.count()) {
	    t << endl;
	} else {
	    moc_incremental = true;
	    t << endl;
	    t << "INCREMENTAL_OBJMOC = " << incrs_out.join(" \\\n\t\t") << endl;
	}
    } else {
	t << "OBJMOC = " << objMoc << endl;
    }
    if(do_incremental && !moc_incremental && !src_incremental)
	do_incremental = false;
    if(!project->isEmpty("QMAKE_EXTRA_UNIX_COMPILERS")) {
	t << "OBJCOMP = " << varList("OBJCOMP") << endl;
	target_deps += " $(OBJCOMP)";

	TQStringList &comps = project->variables()["QMAKE_EXTRA_UNIX_COMPILERS"];
	for(TQStringList::Iterator compit = comps.begin(); compit != comps.end(); ++compit) {
	    TQStringList &vars = project->variables()[(*compit) + ".variables"];
	    for(TQStringList::Iterator varit = vars.begin(); varit != vars.end(); ++varit) {
		TQStringList vals = project->variables()[(*varit)];
		if(!vals.isEmpty())
		    t << "QMAKE_COMP_" << (*varit) << " = " << valList(vals) << endl;
	    }
	}
    }
    t << "DIST	   = " << valList(fileFixify(project->variables()["DISTFILES"])) << endl;
    t << "QMAKE_TARGET = " << var("QMAKE_ORIG_TARGET") << endl;
    t << "DESTDIR  = " << var("DESTDIR") << endl;
    if(project->isActiveConfig("compile_libtool"))
	t << "TARGETL	= " << var("TARGET_la") << endl;
    t << "TARGET   = " << var("TARGET") << endl;
    if(project->isActiveConfig("plugin") ) {
	t << "TARGETD   = " << var("TARGET") << endl;
    } else if (!project->isActiveConfig("staticlib") && project->variables()["QMAKE_APP_FLAG"].isEmpty()) {
	t << "TARGETA	= " << var("TARGETA") << endl;
	if (!project->isEmpty("QMAKE_OPENBSD_SHLIBS")) {
	    t << "LIB"+var("OBSD_TARGET_NAME")+"_VERSION = " << var("OBSD_TARGET_VERSION") << endl;
	    t << "TARGETD	= " << var("TARGET_x.y") << endl;
	} else {
	    t << "TARGETD	= " << var("TARGET_x.y.z") << endl;
	    t << "TARGET0	= " << var("TARGET_") << endl;
	    t << "TARGET1	= " << var("TARGET_x") << endl;
	    t << "TARGET2	= " << var("TARGET_x.y") << endl;
	}
    }
    writeExtraVariables(t);
    t << endl;

    // blasted includes
    TQStringList &qeui = project->variables()["QMAKE_EXTRA_UNIX_INCLUDES"];
    TQStringList::Iterator it;
    for( it = qeui.begin(); it != qeui.end(); ++it)
	t << "include " << (*it) << endl;

    /* rules */
    t << "first: all" << endl;
    t << "####### Implicit rules" << endl << endl;
    t << ".SUFFIXES: .c " << Option::obj_ext;
    TQStringList::Iterator cppit;
    for(cppit = Option::cpp_ext.begin(); cppit != Option::cpp_ext.end(); ++cppit)
	t << " " << (*cppit);
    t << endl << endl;
    for(cppit = Option::cpp_ext.begin(); cppit != Option::cpp_ext.end(); ++cppit)
	t << (*cppit) << Option::obj_ext << ":\n\t" << var("QMAKE_RUN_CXX_IMP") << endl << endl;
    t << ".c" << Option::obj_ext << ":\n\t" << var("QMAKE_RUN_CC_IMP") << endl << endl;

    if(include_deps) {
	TQString cmd=var("QMAKE_CFLAGS_DEPS") + " ";
	cmd += varGlue("DEFINES","-D"," -D","") + varGlue("PRL_EXPORT_DEFINES"," -D"," -D","");
	if(!project->isEmpty("QMAKE_ABSOLUTE_SOURCE_PATH"))
	    cmd += " -I" + project->first("QMAKE_ABSOLUTE_SOURCE_PATH") + " ";
	cmd += " $(INCPATH) " + varGlue("DEPENDPATH", "-I", " -I", "");
	TQString odir;
	if(!project->variables()["OBJECTS_DIR"].isEmpty())
	    odir = project->first("OBJECTS_DIR");
	t << "###### Dependencies" << endl << endl;
	t << odir << ".deps/%.d: %.cpp\n\t"
	  << "@echo Creating depend for $<" << "\n\t"
	  << "@test -d $(@D) || mkdir -p $(@D)" << "\n\t"
	  << "@$(CXX) " << cmd << " $< | sed \"s,^\\($(*F).o\\):," << odir << "\\1:,g\" >$@" << endl << endl;

	t << odir << ".deps/%.d: %.c\n\t"
	  << "@echo Creating depend for $<" << "\n\t"
	  << "@test -d $(@D) || mkdir -p $(@D)" << "\n\t"
	  << "@$(CC) " << cmd << " $< | sed \"s,^\\($(*F).o\\):," << odir << "\\1:,g\" >$@" << endl << endl;

	TQString src[] = { "SOURCES", "UICIMPLS", "SRCMOC", TQString::null };
	for(int x = 0; !src[x].isNull(); x++) {
	    TQStringList &l = project->variables()[src[x]];
	    for(TQStringList::Iterator it = l.begin(); it != l.end(); ++it) {
		if(!(*it).isEmpty()) {
		    TQString d_file;
		    if((*it).endsWith(".c")) {
			d_file = (*it).left((*it).length() - 2);
		    } else {
			for(TQStringList::Iterator cppit = Option::cpp_ext.begin();
			    cppit != Option::cpp_ext.end(); ++cppit) {
			    if((*it).endsWith((*cppit))) {
				d_file = (*it).left((*it).length() - (*cppit).length());
				break;
			    }
			}
		    }
		    if(!d_file.isEmpty()) {
			d_file = odir + ".deps/" + d_file + ".d";
			TQStringList deps = findDependencies((*it)).grep(TQRegExp(Option::cpp_moc_ext + "$"));
			if(!deps.isEmpty())
			    t << d_file << ": " << deps.join(" ") << endl;
			t << var("QMAKE_CFLAGS_USE_PRECOMPILE") << " " << d_file << endl;
		    }
		}
	    }
	}
    }

    t << "####### Build rules" << endl << endl;
    t << "$(OBJECTS): $(UICDECLS)" << endl << endl;
    if(!project->variables()["SUBLIBS"].isEmpty()) {
	TQString libdir = "tmp/";
	if(!project->isEmpty("SUBLIBS_DIR"))
	    libdir = project->first("SUBLIBS_DIR");
	t << "SUBLIBS= ";
	TQStringList &l = project->variables()["SUBLIBS"];
	for(TQStringList::Iterator it = l.begin(); it != l.end(); ++it)
	    t << libdir << "lib" << (*it) << ".a ";
	t << endl << endl;
    }
    if(project->isActiveConfig("depend_prl") && !project->isEmpty("QMAKE_PRL_INTERNAL_FILES")) {
	TQStringList &l = project->variables()["QMAKE_PRL_INTERNAL_FILES"];
	TQStringList::Iterator it;
	for(it = l.begin(); it != l.end(); ++it) {
	    TQMakeMetaInfo libinfo;
	    if(libinfo.readLib((*it)) && !libinfo.isEmpty("QMAKE_PRL_BUILD_DIR")) {
		TQString dir;
		int slsh = (*it).findRev(Option::dir_sep);
		if(slsh != -1)
		    dir = (*it).left(slsh + 1);
		TQString targ = dir + libinfo.first("QMAKE_PRL_TARGET");
		deps += " " + targ;
		t << targ << ":" << "\n\t"
		  << "@echo \"Creating '" << targ << "'\"" << "\n\t"
		  << "(cd " << libinfo.first("QMAKE_PRL_BUILD_DIR") << ";"
		  << "$(MAKE) )" << endl;
	    }
	}
    }
    if(!project->variables()["QMAKE_APP_FLAG"].isEmpty()) {
	TQString destdir = project->first("DESTDIR");
	if(do_incremental) {
	    //incremental target
	    TQString incr_target = var("TARGET") + "_incremental";
	    if(incr_target.find(Option::dir_sep) != -1)
		incr_target = incr_target.right(incr_target.length() -
						(incr_target.findRev(Option::dir_sep) + 1));
	    TQString incr_deps, incr_objs;
	    if(project->first("QMAKE_INCREMENTAL_STYLE") == "ld") {
		TQString incr_target_dir = var("OBJECTS_DIR") + incr_target + Option::obj_ext;
		//actual target
		t << incr_target_dir << ": $(OBJECTS)" << "\n\t"
		  << "ld -r  -o "<< incr_target_dir << " $(OBJECTS)" << endl;
		//communicated below
		deps.prepend(incr_target_dir + " ");
		incr_deps = "$(UICDECLS) $(INCREMENTAL_OBJECTS) $(INCREMENTAL_OBJMOC) $(OBJMOC)";
		if(!incr_objs.isEmpty())
		    incr_objs += " ";
		incr_objs += incr_target_dir;
	    } else {
		//actual target
		TQString incr_target_dir = var("DESTDIR") + "lib" + incr_target + "." +
					  project->variables()["QMAKE_EXTENSION_SHLIB"].first();
		TQString incr_lflags = var("QMAKE_LFLAGS_SHLIB") + " ";
		if(project->isActiveConfig("debug"))
		    incr_lflags += var("QMAKE_LFLAGS_DEBUG");
		else
		    incr_lflags += var("QMAKE_LFLAGS_RELEASE");
		t << incr_target_dir << ": $(INCREMENTAL_OBJECTS) $(INCREMENTAL_OBJMOC)" << "\n\t";
		if(!destdir.isEmpty())
		    t << "\n\t" << "test -d " << destdir << " || mkdir -p " << destdir << "\n\t";
		t << "$(LINK) " << incr_lflags << " -o "<< incr_target_dir <<
		    " $(INCREMENTAL_OBJECTS) $(INCREMENTAL_OBJMOC)" << endl;
		//communicated below
		if(!destdir.isEmpty()) {
		    if(!incr_objs.isEmpty())
			incr_objs += " ";
		    incr_objs += "-L" + destdir;
		} else {
		    if(!incr_objs.isEmpty())
			incr_objs += " ";
		    incr_objs += "-L" + TQDir::currentDirPath();
		}
		if(!incr_objs.isEmpty())
		    incr_objs += " ";
		incr_objs += " -l" + incr_target;
		deps.prepend(incr_target_dir + " ");
		incr_deps = "$(UICDECLS) $(OBJECTS) $(OBJMOC)";
	    }
	    t << "all: " << deps <<  " " << varGlue("ALL_DEPS",""," "," ") <<  "$(TARGET)"
	      << endl << endl;

	    //real target
	    t << var("TARGET") << ": " << var("PRE_TARGETDEPS") << " " << incr_deps << " " << target_deps
	      << " " << var("POST_TARGETDEPS") << "\n\t";
	    if(!destdir.isEmpty())
		t << "\n\t" << "test -d " << destdir << " || mkdir -p " << destdir << "\n\t";
	    if(!project->isEmpty("QMAKE_PRE_LINK"))
		t << var("QMAKE_PRE_LINK") << "\n\t";
	    t << "$(LINK) $(LFLAGS) -o $(TARGET) " << incr_deps << " " << incr_objs << " $(OBJCOMP) $(LIBS)";
	    if(!project->isEmpty("QMAKE_POST_LINK"))
		t << "\n\t" << var("QMAKE_POST_LINK");
	    t << endl << endl;
	} else {
	    t << "all: " << deps <<  " " << varGlue("ALL_DEPS",""," "," ") <<  "$(TARGET)"
	      << endl << endl;

	    t << "$(TARGET): " << var("PRE_TARGETDEPS") << " $(UICDECLS) $(OBJECTS) $(OBJMOC) "
	      << target_deps << " " << var("POST_TARGETDEPS") << "\n\t";
	    if(!destdir.isEmpty())
		t << "test -d " << destdir << " || mkdir -p " << destdir << "\n\t";
	    if(!project->isEmpty("QMAKE_PRE_LINK"))
		t << var("QMAKE_PRE_LINK") << "\n\t";
	    t << "$(LINK) $(LFLAGS) -o $(TARGET) $(OBJECTS) $(OBJMOC) $(OBJCOMP) $(LIBS)";
	    if(!project->isEmpty("QMAKE_POST_LINK"))
		t << "\n\t" << var("QMAKE_POST_LINK");
	    t << endl << endl;
	}
    } else if(!project->isActiveConfig("staticlib")) {
	TQString destdir = project->first("DESTDIR"), incr_deps;
	if(do_incremental) {
	    TQString s_ext = project->variables()["QMAKE_EXTENSION_SHLIB"].first();
	    TQString incr_target = var("QMAKE_ORIG_TARGET").replace(
		TQRegExp("\\." + s_ext), "").replace(TQRegExp("^lib"), "") + "_incremental";
	    if(incr_target.find(Option::dir_sep) != -1)
		incr_target = incr_target.right(incr_target.length() -
						(incr_target.findRev(Option::dir_sep) + 1));

	    if(project->first("QMAKE_INCREMENTAL_STYLE") == "ld") {
		TQString incr_target_dir = var("OBJECTS_DIR") + incr_target + Option::obj_ext;
		//actual target
		const TQString link_deps = "$(UICDECLS) $(OBJECTS) $(OBJMOC)";
		t << incr_target_dir << ": " << link_deps << "\n\t"
		  << "ld -r  -o " << incr_target_dir << " " << link_deps << endl;
		//communicated below
		TQStringList &cmd = project->variables()["QMAKE_LINK_SHLIB_CMD"];
		cmd.first().replace("$(OBJECTS) $(OBJMOC)",
				    "$(INCREMENTAL_OBJECTS) $(INCREMENTAL_OBJMOC)"); //ick
		cmd.append(incr_target_dir);
		deps.prepend(incr_target_dir + " ");
		incr_deps = "$(INCREMENTAL_OBJECTS) $(INCREMENTAL_OBJMOC)";
	    } else {
		//actual target
		TQString incr_target_dir = var("DESTDIR") + "lib" + incr_target + "." + s_ext;
		TQString incr_lflags = var("QMAKE_LFLAGS_SHLIB") + " ";
		if(!project->isEmpty("QMAKE_LFLAGS_INCREMENTAL"))
		    incr_lflags += var("QMAKE_LFLAGS_INCREMENTAL") + " ";
		if(project->isActiveConfig("debug"))
		    incr_lflags += var("QMAKE_LFLAGS_DEBUG");
		else
		    incr_lflags += var("QMAKE_LFLAGS_RELEASE");
		t << incr_target_dir << ": $(INCREMENTAL_OBJECTS) $(INCREMENTAL_OBJMOC)" << "\n\t";
		if(!destdir.isEmpty())
		    t << "test -d " << destdir << " || mkdir -p " << destdir << "\n\t";
		t << "$(LINK) " << incr_lflags << " -o "<< incr_target_dir <<
		    " $(INCREMENTAL_OBJECTS) $(INCREMENTAL_OBJMOC)" << endl;
		//communicated below
		TQStringList &cmd = project->variables()["QMAKE_LINK_SHLIB_CMD"];
		if(!destdir.isEmpty())
		    cmd.append(" -L" + destdir);
		cmd.append(" -l" + incr_target);
		deps.prepend(incr_target_dir + " ");
		incr_deps = "$(UICDECLS) $(OBJECTS) $(OBJMOC)";
	    }

	    t << "all: " << " " << deps << " " << varGlue("ALL_DEPS",""," ","")
	      << " " <<  var("DESTDIR_TARGET") << endl << endl;

	    //real target
	    t << var("DESTDIR_TARGET") << ": " << var("PRE_TARGETDEPS") << " "
	      << incr_deps << " $(SUBLIBS) " << target_deps << " " << var("POST_TARGETDEPS");
	} else {
	    t << "all: " << deps << " " << varGlue("ALL_DEPS",""," ","") << " " <<
		var("DESTDIR_TARGET") << endl << endl;
	    t << var("DESTDIR_TARGET") << ": " << var("PRE_TARGETDEPS")
	      << " $(UICDECLS) $(OBJECTS) $(OBJMOC) $(SUBLIBS) $(OBJCOMP) " << target_deps
	      << " " << var("POST_TARGETDEPS");
	}
	if(!destdir.isEmpty())
	    t << "\n\t" << "test -d " << destdir << " || mkdir -p " << destdir;
	if(!project->isEmpty("QMAKE_PRE_LINK"))
	    t << "\n\t" << var("QMAKE_PRE_LINK");

	if(project->isActiveConfig("compile_libtool")) {
	    t << "\n\t"
	      << var("QMAKE_LINK_SHLIB_CMD");
	} else if(project->isActiveConfig("plugin")) {
	    t << "\n\t"
	      << "-$(DEL_FILE) $(TARGET)" << "\n\t"
	      << var("QMAKE_LINK_SHLIB_CMD");
	    if(!destdir.isEmpty())
		t << "\n\t"
		  << "-$(MOVE) $(TARGET) " << var("DESTDIR");
	    if(!project->isEmpty("QMAKE_POST_LINK"))
		t << "\n\t" << var("QMAKE_POST_LINK") << "\n\t";
	    t << endl << endl;
	} else {
	    t << "\n\t"
	      << "-$(DEL_FILE) $(TARGET) $(TARGET0) $(TARGET1) $(TARGET2)" << "\n\t"
	      << var("QMAKE_LINK_SHLIB_CMD") << "\n\t";
	    t << varGlue("QMAKE_LN_SHLIB","-"," "," $(TARGET) $(TARGET0)")  << "\n\t"
	      << varGlue("QMAKE_LN_SHLIB","-"," "," $(TARGET) $(TARGET1)") << "\n\t"
	      << varGlue("QMAKE_LN_SHLIB","-"," "," $(TARGET) $(TARGET2)");
	    if(!destdir.isEmpty())
		t << "\n\t"
		  << "-$(DEL_FILE) " << var("DESTDIR") << "$(TARGET)\n\t"
		  << "-$(DEL_FILE) " << var("DESTDIR") << "$(TARGET0)\n\t"
		  << "-$(DEL_FILE) " << var("DESTDIR") << "$(TARGET1)\n\t"
		  << "-$(DEL_FILE) " << var("DESTDIR") << "$(TARGET2)\n\t"
		  << "-$(MOVE) $(TARGET) $(TARGET0) $(TARGET1) $(TARGET2) " << var("DESTDIR");
	    if(!project->isEmpty("QMAKE_POST_LINK"))
		t << "\n\t" << var("QMAKE_POST_LINK");
	    t << endl << endl;
	}
	t << endl << endl;

	if (! project->isActiveConfig("plugin")) {
	    t << "staticlib: $(TARGETA)" << endl << endl;
	    t << "$(TARGETA): " << var("PRE_TARGETDEPS") << " $(UICDECLS) $(OBJECTS) $(OBJMOC) $(OBJCOMP)";
	    if(do_incremental)
		t << " $(INCREMENTAL_OBJECTS) $(INCREMENTAL_OBJMOC)";
	    t << var("POST_TARGETDEPS") << "\n\t"
	      << "-$(DEL_FILE) $(TARGETA) " << "\n\t"
	      << var("QMAKE_AR_CMD");
	    if(do_incremental)
		t << " $(INCREMENTAL_OBJECTS) $(INCREMENTAL_OBJMOC)";
	    if(!project->isEmpty("QMAKE_RANLIB"))
		t << "\n\t" << "$(RANLIB) $(TARGETA)";
	    t << endl << endl;
	}
    } else {
	t << "all: " << deps << " " << varGlue("ALL_DEPS",""," "," ") << var("DESTDIR") << "$(TARGET) "
	  << varGlue("QMAKE_AR_SUBLIBS", var("DESTDIR"), " " + var("DESTDIR"), "") << "\n\n"
	  << "staticlib: " << var("DESTDIR") << "$(TARGET)" << "\n\n";
	if(project->isEmpty("QMAKE_AR_SUBLIBS")) {
	    t << var("DESTDIR") << "$(TARGET): " << var("PRE_TARGETDEPS")
	      << " $(UICDECLS) $(OBJECTS) $(OBJMOC) $(OBJCOMP) " << var("POST_TARGETDEPS") << "\n\t";
	    if(!project->isEmpty("DESTDIR")) {
		TQString destdir = project->first("DESTDIR");
		t << "test -d " << destdir << " || mkdir -p " << destdir << "\n\t";
	    }
	    t << "-$(DEL_FILE) $(TARGET)" << "\n\t"
	      << var("QMAKE_AR_CMD") << "\n";
	    if(!project->isEmpty("QMAKE_POST_LINK"))
		t << "\t" << var("QMAKE_POST_LINK") << "\n";
	    if(!project->isEmpty("QMAKE_RANLIB"))
		t << "\t" << "$(RANLIB) $(TARGET)" << "\n";
	    if(!project->isEmpty("DESTDIR"))
		t << "\t" << "-$(DEL_FILE) " << var("DESTDIR") << "$(TARGET)" << "\n"
		  << "\t" << "-$(MOVE) $(TARGET) " << var("DESTDIR") << "\n";
	} else {
	    int max_files = project->first("QMAKE_MAX_FILES_PER_AR").toInt();
	    TQStringList objs = project->variables()["OBJECTS"] + project->variables()["OBJMOC"] +
			       project->variables()["OBJCOMP"],
			libs = project->variables()["QMAKE_AR_SUBLIBS"];
	    libs.prepend("$(TARGET)");
	    for(TQStringList::Iterator libit = libs.begin(), objit = objs.begin();
		libit != libs.end(); ++libit) {
		TQStringList build;
		for(int cnt = 0; cnt < max_files && objit != objs.end(); ++objit, cnt++)
		    build << (*objit);
		TQString ar;
		if((*libit) == "$(TARGET)") {
		    t << var("DESTDIR") << "$(TARGET): " << var("PRE_TARGETDEPS")
		      << " $(UICDECLS) " << var("POST_TARGETDEPS") << valList(build) << "\n\t";
		    ar = project->variables()["QMAKE_AR_CMD"].first();
		    ar = ar.replace("$(OBJMOC)", "").replace("$(OBJECTS)",
							     build.join(" "));
		} else {
		    t << (*libit) << ": " << valList(build) << "\n\t";
		    ar = "$(AR) " + (*libit) + " " + build.join(" ");
		}
		if(!project->isEmpty("DESTDIR")) {
		    TQString destdir = project->first("DESTDIR");
		    t << "test -d " << destdir << " || mkdir -p " << destdir << "\n\t";
		}
		t << "-$(DEL_FILE) " << (*libit) << "\n\t"
		  << ar << "\n";
		if(!project->isEmpty("QMAKE_POST_LINK"))
		    t << "\t" << var("QMAKE_POST_LINK") << "\n";
		if(!project->isEmpty("QMAKE_RANLIB"))
		    t << "\t" << "$(RANLIB) " << (*libit) << "\n";
		if(!project->isEmpty("DESTDIR"))
		    t << "\t" << "-$(DEL_FILE) " << var("DESTDIR") << (*libit) << "\n"
		      << "\t" << "-$(MOVE) " << (*libit) << " " << var("DESTDIR") << "\n";
	    }
	}
	t << endl << endl;
    }

    t << "mocables: $(SRCMOC)" << endl
      << "uicables: $(UICDECLS) $(UICIMPLS)" << endl << endl;

    if(!project->isActiveConfig("no_mocdepend")) {
	//this is an implicity depend on moc, so it will be built if necesary, however
	//moc itself shouldn't have this dependency - this is a little kludgy but it is
	//better than the alternative for now.
	TQString moc = project->first("QMAKE_MOC"), target = project->first("TARGET"),
	    moc_dir = "$(TQTDIR)/src/moc";
	if(!project->isEmpty("QMAKE_MOC_SRC"))
	    moc_dir = project->first("QMAKE_MOC_SRC");
	fixEnvVariables(target);
	fixEnvVariables(moc);
	if(target != moc)
	    t << "$(MOC): \n\t"
	      << "( cd " << moc_dir << " && $(MAKE) )"  << endl << endl;
    }

    writeMakeQmake(t);
    if(project->isEmpty("QMAKE_FAILED_REQUIREMENTS") && !project->isActiveConfig("no_autoqmake")) {
	TQString meta_files;
	if(project->isActiveConfig("create_libtool") && project->first("TEMPLATE") == "lib" &&
	   !project->isActiveConfig("compile_libtool")) { //libtool
	    if(!meta_files.isEmpty())
		meta_files += " ";
	    meta_files += libtoolFileName();
	}
	if(project->isActiveConfig("create_pc") && project->first("TEMPLATE") == "lib") { //pkg-config
	    if(!meta_files.isEmpty())
		meta_files += " ";
	    meta_files += pkgConfigFileName();
	}
	if(!meta_files.isEmpty()) {
	    TQStringList files = fileFixify(Option::mkfile::project_files);
	    t << meta_files << ": " << "\n\t"
	      << "@$(QMAKE) -prl " << buildArgs() << " " << files.join(" ") << endl;
	}
    }

    if(!project->first("QMAKE_PKGINFO").isEmpty()) {
	TQString pkginfo = project->first("QMAKE_PKGINFO");
	TQString destdir = project->first("DESTDIR");
	t << pkginfo << ": " << "\n\t";
	if(!destdir.isEmpty())
	    t << "@test -d " << destdir << " || mkdir -p " << destdir << "\n\t";
	t << "@$(DEL_FILE) " << pkginfo << "\n\t"
	  << "@echo \"APPL????\" >" << pkginfo << endl;
    }
    if(!project->first("QMAKE_INFO_PLIST").isEmpty()) {
	TQString info_plist = project->first("QMAKE_INFO_PLIST"),
	    info_plist_out = project->first("QMAKE_INFO_PLIST_OUT");
	TQString destdir = project->first("DESTDIR");
	t << info_plist_out << ": " << "\n\t";
	if(!destdir.isEmpty())
	    t << "@test -d " << destdir << " || mkdir -p " << destdir << "\n\t";
	t << "@$(DEL_FILE) " << info_plist_out << "\n\t"
	  << "@sed -e \"s,@ICON@,application.icns,g\" -e \"s,@EXECUTABLE@," << var("QMAKE_ORIG_TARGET")
	  << ",g\" \"" << info_plist << "\" >\"" << info_plist_out << "\"" << endl;
	if(!project->first("RC_FILE").isEmpty()) {
	    TQString dir = destdir + "../Resources/";
	    t << dir << "application.icns: " << fileFixify(var("RC_FILE")) << "\n\t"
	      << "@test -d " << dir << " || mkdir -p " << dir << "\n\t"
	      << "@$(DEL_FILE) " << dir << "application.icns" << "\n\t"
	      << "@$(COPY_FILE) " << fileFixify(var("RC_FILE"))
              << " " << dir << "application.icns" << endl;
	}
    }

    TQString ddir = project->isEmpty("QMAKE_DISTDIR") ? project->first("QMAKE_ORIG_TARGET") :
		   project->first("QMAKE_DISTDIR");
    TQString ddir_c = fileFixify((project->isEmpty("OBJECTS_DIR") ? TQString(".tmp/") :
				 project->first("OBJECTS_DIR")) + ddir);
    t << "dist: " << "\n\t"
      << "@mkdir -p " << ddir_c << " && "
      << "$(COPY_FILE) --parents $(SOURCES) $(HEADERS) $(FORMS) $(DIST) " << ddir_c << Option::dir_sep << " && ";
    if(!project->isEmpty("TRANSLATIONS"))
	t << "$(COPY_FILE) --parents " << var("TRANSLATIONS") << " " << ddir_c << Option::dir_sep << " && ";
    if(!project->isEmpty("IMAGES"))
	t << "$(COPY_FILE) --parents " << var("IMAGES") << " " << ddir_c << Option::dir_sep << " && ";
    if(!project->isEmpty("FORMS")) {
	TQStringList &forms = project->variables()["FORMS"], ui_headers;
	for(TQStringList::Iterator formit = forms.begin(); formit != forms.end(); ++formit) {
	    TQString ui_h = fileFixify((*formit) + Option::h_ext.first());
	    if(TQFile::exists(ui_h) )
	       ui_headers << ui_h;
	}
	if(!ui_headers.isEmpty())
	    t << "$(COPY_FILE) --parents " << val(ui_headers) << " " << ddir_c << Option::dir_sep << " && ";
    }
    t << "( cd `dirname " << ddir_c << "` && "
      << "$(TAR) " << var("QMAKE_ORIG_TARGET") << ".tar " << ddir << " && "
      << "$(GZIP) " << var("QMAKE_ORIG_TARGET") << ".tar ) && "
      << "$(MOVE) `dirname " << ddir_c << "`" << Option::dir_sep << var("QMAKE_ORIG_TARGET") << ".tar.gz . && "
      << "$(DEL_FILE) -r " << ddir_c
      << endl << endl;

    TQString clean_targets;
    t << "mocclean:" << "\n";
    if(mocAware()) {
	if(!objMoc.isEmpty() || !srcMoc.isEmpty() || moc_incremental) {
	    if(!objMoc.isEmpty())
		t << "\t-$(DEL_FILE) $(OBJMOC)" << '\n';
	    if(!srcMoc.isEmpty())
		t << "\t-$(DEL_FILE) $(SRCMOC)" << '\n';
	    if(moc_incremental)
		t << "\t-$(DEL_FILE) $(INCREMENTAL_OBJMOC)" << '\n';
	    clean_targets += " mocclean";
	}
	t << endl;
    }
    t << "uiclean:" << "\n";
    if (!var("UICIMPLS").isEmpty() || !var("UICDECLS").isEmpty()) {
	t << "\t-$(DEL_FILE) $(UICIMPLS) $(UICDECLS)" << "\n";
	clean_targets += " uiclean";
    }
    t << endl;

    t << "yaccclean:" << "\n";
    if(!var("YACCSOURCES").isEmpty()) {
	TQStringList clean, &l = project->variables()["YACCSOURCES"];
	for(TQStringList::Iterator it = l.begin(); it != l.end(); ++it) {
	    TQFileInfo fi((*it));
	    TQString dir;
	    if(fi.dirPath() != ".")
		dir = fi.dirPath() + Option::dir_sep;
	    dir = fileFixify(dir, TQDir::currentDirPath(), Option::output_dir);
	    if(!dir.isEmpty() && dir.right(Option::dir_sep.length()) != Option::dir_sep)
		dir += Option::dir_sep;
	    clean << ( dir + fi.baseName(true) + Option::yacc_mod + Option::cpp_ext.first() );
	    clean << ( dir + fi.baseName(true) + Option::yacc_mod + Option::h_ext.first() );
	}
	if(!clean.isEmpty()) {
	    t << "\t-$(DEL_FILE) " << clean.join(" ") << "\n";
	    clean_targets += " yaccclean";
	}
    }

    t << "lexclean:" << "\n";
    if(!var("LEXSOURCES").isEmpty()) {
	TQStringList clean, &l = project->variables()["LEXSOURCES"];
	for(TQStringList::Iterator it = l.begin(); it != l.end(); ++it) {
	    TQFileInfo fi((*it));
	    TQString dir;
	    if(fi.dirPath() != ".")
		dir = fi.dirPath() + Option::dir_sep;
	    dir = fileFixify(dir, TQDir::currentDirPath(), Option::output_dir);
	    if(!dir.isEmpty() && dir.right(Option::dir_sep.length()) != Option::dir_sep)
		dir += Option::dir_sep;
	    clean << ( dir + fi.baseName(true) + Option::lex_mod + Option::cpp_ext.first() );
	}
	if(!clean.isEmpty()) {
	    t << "\t-$(DEL_FILE) " << clean.join(" ") << "\n";
	    clean_targets += " lexclean";
	}
    }

    if(do_incremental) {
	t << "incrclean:" << "\n";
	if(src_incremental)
	    t << "\t-$(DEL_FILE) $(INCREMENTAL_OBJECTS)" << "\n";
	if(moc_incremental)
	    t << "\t-$(DEL_FILE) $(INCREMENTAL_OBJMOC)" << '\n';
	t << endl;
    }

    t << "clean:" << clean_targets << "\n\t";
    if(!project->isEmpty("OBJECTS")) {
	if(project->isActiveConfig("compile_libtool"))
	    t << "-$(LIBTOOL) --mode=clean $(DEL_FILE) $(OBJECTS)" << "\n\t";
	else
	    t << "-$(DEL_FILE) $(OBJECTS)" << "\n\t";
    }
    if(doPrecompiledHeaders() && !project->isEmpty("PRECOMPILED_HEADER")) {
	TQString header_prefix = project->first("QMAKE_PRECOMP_PREFIX");
	TQString precomph_out_dir = project->first("QMAKE_ORIG_TARGET") + ".gch" + Option::dir_sep;
	t << "-$(DEL_FILE) " << precomph_out_dir << (header_prefix + "c ")
	  << precomph_out_dir << header_prefix << "c++" << "\n\t";
    }
    if(!project->isEmpty("IMAGES"))
	t << varGlue("QMAKE_IMAGE_COLLECTION", "\t-$(DEL_FILE) ", " ", "") << "\n\t";
    if(src_incremental)
	t << "-$(DEL_FILE) $(INCREMENTAL_OBJECTS)" << "\n\t";
    t << varGlue("QMAKE_CLEAN","-$(DEL_FILE) "," ","\n\t")
      << "-$(DEL_FILE) *~ core *.core" << "\n"
      << varGlue("CLEAN_FILES","\t-$(DEL_FILE) "," ","") << endl << endl;
    t << "####### Sub-libraries" << endl << endl;
    if ( !project->variables()["SUBLIBS"].isEmpty() ) {
	TQString libdir = "tmp/";
	if(!project->isEmpty("SUBLIBS_DIR"))
	    libdir = project->first("SUBLIBS_DIR");
	TQStringList &l = project->variables()["SUBLIBS"];
	for(it = l.begin(); it != l.end(); ++it)
	    t << libdir << "lib" << (*it) << ".a" << ":\n\t"
	      << var(TQString("MAKELIB") + (*it)) << endl << endl;
    }

    TQString destdir = project->first("DESTDIR");
    if(!destdir.isEmpty() && destdir.right(1) != Option::dir_sep)
	destdir += Option::dir_sep;
    t << "distclean: " << "clean\n";
    if(project->first("TEMPLATE") == "app" &&
       project->isActiveConfig("resource_fork") && !project->isActiveConfig("console"))
	t << "\t-$(DEL_FILE) -r " << destdir.section(Option::dir_sep, 0, -4) << "\n";
    else if(project->isActiveConfig("compile_libtool"))
	t << "\t-$(LIBTOOL) --mode=clean $(DEL_FILE) " << "$(TARGET)" << "\n";
    else
	t << "\t-$(DEL_FILE) " << destdir << "$(TARGET)" << " " << "$(TARGET)" << "\n";
    if(!project->isActiveConfig("staticlib") && project->variables()["QMAKE_APP_FLAG"].isEmpty() &&
       !project->isActiveConfig("plugin") && !project->isActiveConfig("compile_libtool"))
	t << "\t-$(DEL_FILE) " << destdir << "$(TARGET0) " << destdir << "$(TARGET1) "
	  << destdir << "$(TARGET2) $(TARGETA)" << "\n";
    t << endl << endl;

    if(doPrecompiledHeaders() && !project->isEmpty("PRECOMPILED_HEADER") ) {
	TQString precomph = fileFixify(project->first("PRECOMPILED_HEADER"));
	t << "###### Prefix headers" << endl;
	TQString comps[] = { "C", "CXX", TQString::null };
	for(int i = 0; !comps[i].isNull(); i++) {
	    TQString flags = var("QMAKE_" + comps[i] + "FLAGS_PRECOMPILE");
	    flags += " $(" + comps[i] + "FLAGS)";

	    TQString header_prefix = project->first("QMAKE_PRECOMP_PREFIX");
	    TQString outdir = project->first("QMAKE_ORIG_TARGET") + ".gch" + Option::dir_sep, outfile = outdir;
	    TQString compiler;
	    if(comps[i] == "C") {
		outfile += header_prefix + "c";
		compiler = "$(CC) ";
	    } else {
		outfile += header_prefix + "c++";
		compiler = "$(CXX) ";
	    }
	    t << outfile << ": " << precomph << " " << findDependencies(precomph).join(" \\\n\t\t")
	      << "\n\t" << "test -d " << outdir << " || mkdir -p " << outdir
	      << "\n\t" << compiler << flags << " $(INCPATH) " << precomph << " -o " << outfile << endl << endl;
	}
    }
    if(!project->isEmpty("ALLMOC_HEADER")) {
	TQString outdir = project->first("MOC_DIR");
	TQString precomph = fileFixify(project->first("ALLMOC_HEADER"));
	t << "###### Combined headers" << endl << endl
	  << outdir << "allmoc.cpp: " << precomph << " "
	  << varList("HEADERS_ORIG") << "\n\t"
	  << "echo '#include \"" << precomph << "\"' >" << outdir << "allmoc.cpp" << "\n\t"
	  << "$(CXX) -E -DQT_MOC_CPP -DTQT_NO_STL $(CXXFLAGS) $(INCPATH) >" << outdir << "allmoc.h "
	  << outdir << "allmoc.cpp" << "\n\t"
	  << "$(MOC) -o " << outdir << "allmoc.cpp " << outdir << "allmoc.h" << "\n\t"
	  << "perl -pi -e 's{#include \"allmoc.h\"}{#define QT_H_CPP\\n#include \""
	  << precomph << "\"}' " << outdir << "allmoc.cpp" << "\n\t"
	  << "$(DEL_FILE) " << outdir << "allmoc.h" << endl << endl;
    }

    // user defined targets
    TQStringList &qut = project->variables()["QMAKE_EXTRA_UNIX_TARGETS"];
    for(it = qut.begin(); it != qut.end(); ++it) {
	TQString targ = var((*it) + ".target"),
		 cmd = var((*it) + ".commands"), deps;
	if(targ.isEmpty())
	    targ = (*it);
	TQStringList &deplist = project->variables()[(*it) + ".depends"];
	for(TQStringList::Iterator dep_it = deplist.begin(); dep_it != deplist.end(); ++dep_it) {
	    TQString dep = var((*dep_it) + ".target");
	    if(dep.isEmpty())
		dep = (*dep_it);
	    deps += " " + dep;
	}
	if(project->variables()[(*it) + ".CONFIG"].findIndex("phony") != -1)
	    deps += TQString(" ") + "FORCE";
	t << targ << ":" << deps << "\n\t"
	  << cmd << endl << endl;
    }
    // user defined compilers
    TQStringList &quc = project->variables()["QMAKE_EXTRA_UNIX_COMPILERS"];
    for(it = quc.begin(); it != quc.end(); ++it) {
	TQString tmp_out = project->variables()[(*it) + ".output"].first();
	TQString tmp_cmd = project->variables()[(*it) + ".commands"].join(" ");
	TQString tmp_dep = project->variables()[(*it) + ".depends"].join(" ");
	TQStringList &vars = project->variables()[(*it) + ".variables"];
	if(tmp_out.isEmpty() || tmp_cmd.isEmpty())
	    continue;
	TQStringList &tmp = project->variables()[(*it) + ".input"];
	for(TQStringList::Iterator it2 = tmp.begin(); it2 != tmp.end(); ++it2) {
	    TQStringList &inputs = project->variables()[(*it2)];
	    for(TQStringList::Iterator input = inputs.begin(); input != inputs.end(); ++input) {
		TQFileInfo fi(Option::fixPathToLocalOS((*input)));
		TQString in = Option::fixPathToTargetOS((*input), false),
		       out = tmp_out, cmd = tmp_cmd, deps;
		out.replace("${QMAKE_FILE_BASE}", fi.baseName());
		out.replace("${QMAKE_FILE_NAME}", fi.filePath());
		cmd.replace("${QMAKE_FILE_BASE}", fi.baseName());
		cmd.replace("${QMAKE_FILE_OUT}", out);
		cmd.replace("${QMAKE_FILE_NAME}", fi.filePath());
		for(TQStringList::Iterator it3 = vars.begin(); it3 != vars.end(); ++it3)
		    cmd.replace("$(" + (*it3) + ")", "$(QMAKE_COMP_" + (*it3)+")");
		if(!tmp_dep.isEmpty()) {
		    char buff[256];
		    TQString dep_cmd = tmp_dep;
		    dep_cmd.replace("${QMAKE_FILE_NAME}", fi.filePath());
		    if(FILE *proc = QT_POPEN(dep_cmd.latin1(), "r")) {
			while(!feof(proc)) {
			    int read_in = int(fread(buff, 1, 255, proc));
			    if(!read_in)
				break;
			    int l = 0;
			    for(int i = 0; i < read_in; i++) {
				if(buff[i] == '\n' || buff[i] == ' ') {
				    deps += " " + TQCString(buff+l, (i - l) + 1);
				    l = i;
				}
			    }
			}
			fclose(proc);
		    }
		}
		t << out << ": " << in << deps << "\n\t"
		  << cmd << endl << endl;
	    }
	}
    }
    t <<"FORCE:" << endl << endl;
}

struct SubDir
{
    TQString directory, profile, target, makefile;
};

void
UnixMakefileGenerator::writeSubdirs(TQTextStream &t, bool direct)
{
    // blasted includes
    TQStringList &qeui = project->variables()["QMAKE_EXTRA_UNIX_INCLUDES"];
    for(TQStringList::Iterator qeui_it = qeui.begin(); qeui_it != qeui.end(); ++qeui_it)
	t << "include " << (*qeui_it) << endl;
    writeExtraVariables(t);

    TQPtrList<SubDir> subdirs;
    {
	TQStringList subdirs_in = project->variables()["SUBDIRS"];
	for(TQStringList::Iterator it = subdirs_in.begin(); it != subdirs_in.end(); ++it) {
	    TQString file = (*it);
	    fileFixify(file);
	    SubDir *sd = new SubDir;
	    subdirs.append(sd);
	    sd->makefile = "$(MAKEFILE)";
	    if((*it).right(4) == ".pro") {
		int slsh = file.findRev(Option::dir_sep);
		if(slsh != -1) {
		    sd->directory = file.left(slsh+1);
		    sd->profile = file.mid(slsh+1);
		} else {
		    sd->profile = file;
		}
	    } else {
		if(!file.isEmpty())
		    sd->profile = file.section(Option::dir_sep, -1) + ".pro";
		sd->directory = file;
	    }
	    while(sd->directory.right(1) == Option::dir_sep)
		sd->directory = sd->directory.left(sd->directory.length() - 1);
	    if(!sd->profile.isEmpty()) {
		TQString basename = sd->directory;
		int new_slsh = basename.findRev(Option::dir_sep);
		if(new_slsh != -1)
		    basename = basename.mid(new_slsh+1);
		if(sd->profile != basename + ".pro")
		    sd->makefile += "." + sd->profile.left(sd->profile.length() - 4); //no need for the .pro
	    }
	    sd->target = "sub-" + (*it);
	    sd->target.replace('/', '-');
	    sd->target.replace('.', '_');
	}
    }
    TQPtrListIterator<SubDir> it(subdirs);

    TQString ofile = Option::output.name();
    if(ofile.findRev(Option::dir_sep) != -1)
	ofile = ofile.right(ofile.length() - ofile.findRev(Option::dir_sep) -1);
    t << "MAKEFILE =	" << var("MAKEFILE") << endl;
    t << "QMAKE    =	" << var("QMAKE") << endl;
    t << "DEL_FILE =    " << var("QMAKE_DEL_FILE") << endl;
    t << "CHK_DIR_EXISTS= " << var("QMAKE_CHK_DIR_EXISTS") << endl;
    t << "MKDIR    = " << var("QMAKE_MKDIR") << endl;
    t << "INSTALL_FILE= " << var("QMAKE_INSTALL_FILE") << endl;
    t << "INSTALL_DIR = " << var("QMAKE_INSTALL_DIR") << endl;
    t << "SUBTARGETS =	";     // subdirectory targets are sub-directory
    for( it.toFirst(); it.current(); ++it)
	t << " \\\n\t\t" << it.current()->target;
    t << endl << endl;
    t << "first: all\n\nall: " << ofile << " $(SUBTARGETS)" << endl << endl;

    // generate target rules
    for( it.toFirst(); it.current(); ++it) {
	bool have_dir = !(*it)->directory.isEmpty();
	TQString mkfile = (*it)->makefile, out;
	if(have_dir)
	    mkfile.prepend((*it)->directory + Option::dir_sep);
	if(direct || (*it)->makefile != "$(MAKEFILE)")
	    out = " -o " + (*it)->makefile;
	//qmake it
	t << mkfile << ": " << "\n\t";
	if(have_dir)
	    t << mkdir_p_asstring((*it)->directory) << "\n\t"
	      << "cd " << (*it)->directory << " && ";
	TQString profile = fileFixify((*it)->profile, (*it)->directory, (*it)->directory);
	t << "$(QMAKE) " << profile << buildArgs() << out << endl;
	//actually compile
	t << (*it)->target << ": " << mkfile << " FORCE" << "\n\t";
	if(have_dir)
	    t << "cd " << (*it)->directory << " && ";
	t << "$(MAKE) -f " << (*it)->makefile << endl << endl;
    }

    if (project->isActiveConfig("ordered")) { 	// generate dependencies
	for( it.toFirst(); it.current(); ) {
	    TQString tar = it.current()->target;
	    ++it;
	    if (it.current())
		t << it.current()->target << ": " << tar << endl;
	}
	t << endl;
    }

    writeMakeQmake(t);

    if(project->isEmpty("SUBDIRS")) {
	t << "all qmake_all distclean uicables mocables install_subdirs uninstall_subdirs"
	  << " uiclean mocclean lexclean yaccclean clean " << var("SUBDIR_TARGETS") << ": FORCE" << endl;
    } else {
	t << "all: $(SUBTARGETS)" << endl;
	t << "qmake_all:";
	for( it.toFirst(); it.current(); ++it) {
	    t << " ";
	    if(!(*it)->directory.isEmpty())
		t << (*it)->directory << Option::dir_sep;
	    t << (*it)->makefile;
	}
	for( it.toFirst(); it.current(); ++it) {
	    t << "\n\t ( ";
	    if(!(*it)->directory.isEmpty())
		t << "[ -d " << (*it)->directory << " ] && cd " << (*it)->directory << " ; ";
	    t << "grep \"^qmake_all:\" " << (*it)->makefile
	      << " && $(MAKE) -f " << (*it)->makefile << " qmake_all" << "; ) || true";
	}
	t << endl;
	t << "clean uicables mocables uiclean mocclean lexclean yaccclean "
	  << var("SUBDIR_TARGETS") << ": qmake_all FORCE";
	for( it.toFirst(); it.current(); ++it) {
	    t << "\n\t ( ";
	    if(!(*it)->directory.isEmpty())
		t << "[ -d " << (*it)->directory << " ] && cd " << (*it)->directory << " ; ";
	    t << "$(MAKE) -f " << (*it)->makefile << " $@" << "; ) || true";
	}
	t << endl;
	t << "uninstall_subdirs: qmake_all FORCE";
	for( it.toFirst(); it.current(); ++it) {
	    t << "\n\t ( ";
	    if(!(*it)->directory.isEmpty())
		t << "[ -d " << (*it)->directory << " ] && cd " << (*it)->directory << " ; ";
	    t << "$(MAKE) -f " << (*it)->makefile << " uninstall" << "; ) || true";
	}
	t << endl;
	t << "install_subdirs: qmake_all FORCE";
	for( it.toFirst(); it.current(); ++it) {
	    t << "\n\t ( ";
	    if(!(*it)->directory.isEmpty())
		t << "[ -d " << (*it)->directory << " ] && cd " << (*it)->directory << " ; ";
	    t << "$(MAKE) -f " << (*it)->makefile << " install" << "; ) || true";
	}
	t << endl;
	t << "distclean: qmake_all FORCE";
	for( it.toFirst(); it.current(); ++it) {
	    t << "\n\t ( ";
	    if(!(*it)->directory.isEmpty())
		t << "[ -d " << (*it)->directory << " ] && cd " << (*it)->directory << " ; ";
	    t << "$(MAKE) -f " << (*it)->makefile << " $@; $(DEL_FILE) " << (*it)->makefile << "; ) || true";
	}
	t << endl << endl;
    }

    //installations
    project->variables()["INSTALLDEPS"]   += "install_subdirs";
    project->variables()["UNINSTALLDEPS"] += "uninstall_subdirs";
    writeInstalls(t, "INSTALLS");

    // user defined targets
    TQStringList &qut = project->variables()["QMAKE_EXTRA_UNIX_TARGETS"];
    for(TQStringList::Iterator qut_it = qut.begin(); qut_it != qut.end(); ++qut_it) {
	TQString targ = var((*qut_it) + ".target"),
		 cmd = var((*qut_it) + ".commands"), deps;
	if(targ.isEmpty())
	    targ = (*qut_it);
	TQStringList &deplist = project->variables()[(*qut_it) + ".depends"];
	for(TQStringList::Iterator dep_it = deplist.begin(); dep_it != deplist.end(); ++dep_it) {
	    TQString dep = var((*dep_it) + ".target");
	    if(dep.isEmpty())
		dep = (*dep_it);
	    deps += " " + dep;
	}
	if(project->variables()[(*qut_it) + ".CONFIG"].findIndex("phony") != -1)
	    deps += TQString(" ") + "FORCE";
	t << targ << ":" << deps << "\n";
        if(!cmd.isEmpty())
            t << "\t" << cmd << endl;
        t << endl;
    }
    t <<"FORCE:" << endl << endl;
}

void UnixMakefileGenerator::init2()
{
    //version handling
    if(project->variables()["VERSION"].isEmpty())
	project->variables()["VERSION"].append("1.0." +
					       (project->isEmpty("VER_PAT") ? TQString("0") :
						project->first("VER_PAT")) );
    TQStringList l = TQStringList::split('.', project->first("VERSION"));
    l << "0" << "0"; //make sure there are three
    project->variables()["VER_MAJ"].append(l[0]);
    project->variables()["VER_MIN"].append(l[1]);
    project->variables()["VER_PAT"].append(l[2]);

    if ( !project->variables()["QMAKE_APP_FLAG"].isEmpty() ) {
#if 0
	if ( project->isActiveConfig("dll") ) {
	    project->variables()["TARGET"] += project->variables()["TARGET.so"];
	    if(project->variables()["QMAKE_LFLAGS_SHAPP"].isEmpty())
		project->variables()["QMAKE_LFLAGS_SHAPP"] += project->variables()["QMAKE_LFLAGS_SHLIB"];
	    if(!project->variables()["QMAKE_LFLAGS_SONAME"].isEmpty())
		project->variables()["QMAKE_LFLAGS_SONAME"].first() += project->first("TARGET");
	}
#endif
	project->variables()["TARGET"].first().prepend(project->first("DESTDIR"));
       if ( !project->variables()["QMAKE_CYGWIN_EXE"].isEmpty() )
	    project->variables()["TARGET_EXT"].append(".exe");
    } else if ( project->isActiveConfig("staticlib") ) {
	project->variables()["TARGET"].first().prepend("lib");
	project->variables()["TARGET"].first() += ".a";
	if(project->variables()["QMAKE_AR_CMD"].isEmpty())
	    project->variables()["QMAKE_AR_CMD"].append("$(AR) $(TARGET) $(OBJECTS) $(OBJMOC)");
    } else {
	project->variables()["TARGETA"].append(project->first("DESTDIR") + "lib" + project->first("TARGET") + ".a");
	if( project->isActiveConfig("compile_libtool") )
	    project->variables()["TARGET_la"] = project->first("DESTDIR") + "lib" + project->first("TARGET") + Option::libtool_ext;

	if ( !project->variables()["QMAKE_AR_CMD"].isEmpty() )
	    project->variables()["QMAKE_AR_CMD"].first().replace("(TARGET)","(TARGETA)");
	else
	    project->variables()["QMAKE_AR_CMD"].append("$(AR) $(TARGETA) $(OBJECTS) $(OBJMOC)");
	if( project->isActiveConfig("compile_libtool") ) {
	    project->variables()["TARGET"] = project->variables()["TARGET_la"];
	} else if( project->isActiveConfig("plugin") ) {
	    project->variables()["TARGET_x.y.z"].append("lib" +
							project->first("TARGET") + "." +
							project->first("QMAKE_EXTENSION_PLUGIN"));
	    if(project->isActiveConfig("lib_version_first"))
		project->variables()["TARGET_x"].append("lib" + project->first("TARGET") + "." +
							project->first("VER_MAJ") + "." +
							project->first("QMAKE_EXTENSION_PLUGIN"));
	    else
		project->variables()["TARGET_x"].append("lib" + project->first("TARGET") + "." +
							project->first("QMAKE_EXTENSION_PLUGIN") +
							"." + project->first("VER_MAJ"));

	    project->variables()["TARGET"] = project->variables()["TARGET_x.y.z"];
	    if(project->isActiveConfig("qt"))
		project->variables()["DEFINES"].append("QT_PLUGIN");
	} else if ( !project->isEmpty("QMAKE_AIX_SHLIB") ) {
	    project->variables()["TARGET_"].append("lib" + project->first("TARGET") + ".a");
	    if(project->isActiveConfig("lib_version_first")) {
		project->variables()["TARGET_x"].append("lib" + project->first("TARGET") + "." +
							project->first("VER_MAJ") + "." +
							project->first("QMAKE_EXTENSION_SHLIB"));
		project->variables()["TARGET_x.y"].append("lib" + project->first("TARGET") + "." +
							  project->first("VER_MAJ") +
							  "." + project->first("VER_MIN") + "." +
							  project->first("QMAKE_EXTENSION_SHLIB"));
		project->variables()["TARGET_x.y.z"].append("lib" + project->first("TARGET") + "." +
							    project->first("VER_MAJ") + "." +
							    project->first("VER_MIN") + "." +
							    project->first("VER_PAT") + "." +
							    project->first("QMAKE_EXTENSION_SHLIB"));
	    } else {
		project->variables()["TARGET_x"].append("lib" + project->first("TARGET") + "." +
							project->first("QMAKE_EXTENSION_SHLIB") +
							"." + project->first("VER_MAJ"));
		project->variables()["TARGET_x.y"].append("lib" + project->first("TARGET") + "." +
							  project->first("QMAKE_EXTENSION_SHLIB") +
							  "." + project->first("VER_MAJ") +
							  "." + project->first("VER_MIN"));
		project->variables()["TARGET_x.y.z"].append("lib" + project->first("TARGET") + "." +
							    project->first("QMAKE_EXTENSION_SHLIB") + "." +
							    project->first("VER_MAJ") + "." +
							    project->first("VER_MIN") + "." +
							    project->first("VER_PAT"));
	    }
	    project->variables()["TARGET"] = project->variables()["TARGET_x.y.z"];
	} else {
	    project->variables()["TARGET_"].append("lib" + project->first("TARGET") + "." +
						   project->first("QMAKE_EXTENSION_SHLIB"));
	    if(project->isActiveConfig("lib_version_first")) {
		project->variables()["TARGET_x"].append("lib" + project->first("TARGET") + "." +
							project->first("VER_MAJ") + "." +
							project->first("QMAKE_EXTENSION_SHLIB"));
		project->variables()["TARGET_x.y"].append("lib" + project->first("TARGET") + "." +
							  project->first("VER_MAJ") +
							  "." + project->first("VER_MIN") + "." +
							  project->first("QMAKE_EXTENSION_SHLIB"));
		project->variables()["TARGET_x.y.z"].append("lib" + project->first("TARGET") + "." +
							    project->first("VER_MAJ") + "." +
							    project->first("VER_MIN") +  "." +
							    project->first("VER_PAT") + "." +
							    project->variables()["QMAKE_EXTENSION_SHLIB"].first());
	    } else {
		project->variables()["TARGET_x"].append("lib" + project->first("TARGET") + "." +
							project->first("QMAKE_EXTENSION_SHLIB") +
							"." + project->first("VER_MAJ"));
		if ( !project->variables()["QMAKE_OPENBSD_SHLIBS"].isEmpty() ) {
		    TQString s;
		    s.setNum(project->first("VER_MIN").toInt()*10+ project->first("VER_PAT").toInt());
		    project->variables()["OBSD_TARGET_VERSION"].append(project->first("VER_MAJ") + "." + s);
		    project->variables()["OBSD_TARGET_NAME"].append(project->first("TARGET"));
		    project->variables()["TARGET_x.y"].append("lib" + project->first("TARGET") + "." +
							  project->first("QMAKE_EXTENSION_SHLIB")
							  + ".$(LIB"+ project->first("TARGET") + "_VERSION)");
		} else {
		    project->variables()["TARGET_x.y"].append("lib" + project->first("TARGET") + "." +
							  project->first("QMAKE_EXTENSION_SHLIB")
							  + "." + project->first("VER_MAJ") +
							  "." + project->first("VER_MIN"));
		}
		project->variables()["TARGET_x.y.z"].append("lib" + project->first("TARGET") +
							    "." +
							    project->variables()[
								"QMAKE_EXTENSION_SHLIB"].first() + "." +
							    project->first("VER_MAJ") + "." +
							    project->first("VER_MIN") +  "." +
							    project->first("VER_PAT"));
	    }
	    if ( !project->variables()["QMAKE_OPENBSD_SHLIBS"].isEmpty() )
		project->variables()["TARGET"] = project->variables()["TARGET_x.y"];
	    else
		project->variables()["TARGET"] = project->variables()["TARGET_x.y.z"];
	}
	if(project->isEmpty("QMAKE_LN_SHLIB"))
	    project->variables()["QMAKE_LN_SHLIB"].append("ln -s");
	project->variables()["DESTDIR_TARGET"].append("$(TARGET)");
	if ( !project->variables()["DESTDIR"].isEmpty() )
	    project->variables()["DESTDIR_TARGET"].first().prepend(project->first("DESTDIR"));
	if ( !project->variables()["QMAKE_LFLAGS_SONAME"].isEmpty()) {
	    if(project->isActiveConfig("plugin")) {
		if(!project->variables()["TARGET"].isEmpty() )
		    project->variables()["QMAKE_LFLAGS_SONAME"].first() += project->first("TARGET");
	    } else {
		if(!project->variables()["TARGET_x"].isEmpty() )
		    project->variables()["QMAKE_LFLAGS_SONAME"].first() += project->first("TARGET_x");
	    }
	}
	if ( project->variables()["QMAKE_LINK_SHLIB_CMD"].isEmpty() )
	    project->variables()["QMAKE_LINK_SHLIB_CMD"].append(
		"$(LINK) $(LFLAGS) -o $(TARGET) $(OBJECTS) $(OBJMOC) $(LIBS) $(OBJCOMP)");
    }
    if(project->isEmpty("QMAKE_SYMBOLIC_LINK"))
	project->variables()["QMAKE_SYMBOLIC_LINK"].append("ln -sf");
    if ( !project->variables()["QMAKE_APP_FLAG"].isEmpty() ) {
	project->variables()["QMAKE_CFLAGS"] += project->variables()["QMAKE_CFLAGS_APP"];
	project->variables()["QMAKE_CXXFLAGS"] += project->variables()["QMAKE_CXXFLAGS_APP"];
	project->variables()["QMAKE_LFLAGS"] += project->variables()["QMAKE_LFLAGS_APP"];
    } else if ( project->isActiveConfig("dll") ) {
	if( !project->isActiveConfig("plugin") || !project->isActiveConfig("plugin_no_share_shlib_cflags")) {
	    project->variables()["QMAKE_CFLAGS"] += project->variables()["QMAKE_CFLAGS_SHLIB"];
	    project->variables()["QMAKE_CXXFLAGS"] += project->variables()["QMAKE_CXXFLAGS_SHLIB"];
	}
	if ( project->isActiveConfig("plugin") ) {
	    project->variables()["QMAKE_CFLAGS"] += project->variables()["QMAKE_CFLAGS_PLUGIN"];
	    project->variables()["QMAKE_CXXFLAGS"] += project->variables()["QMAKE_CXXFLAGS_PLUGIN"];
	    project->variables()["QMAKE_LFLAGS"] += project->variables()["QMAKE_LFLAGS_PLUGIN"];
	    if( project->isActiveConfig("plugin_with_soname") && !project->isActiveConfig("compile_libtool"))
		project->variables()["QMAKE_LFLAGS"] += project->variables()["QMAKE_LFLAGS_SONAME"];
	} else {
	    project->variables()["QMAKE_LFLAGS"] += project->variables()["QMAKE_LFLAGS_SHLIB"];
	    if(!project->isEmpty("QMAKE_LFLAGS_COMPAT_VERSION")) {
		if(project->isEmpty("COMPAT_VERSION"))
		    project->variables()["QMAKE_LFLAGS"] += TQString(project->first("QMAKE_LFLAGS_COMPAT_VERSION") +
								    project->first("VER_MAJ") + "." +
								    project->first("VER_MIN"));
		else
		    project->variables()["QMAKE_LFLAGS"] += TQString(project->first("QMAKE_LFLAGS_COMPAT_VERSION") +
								    project->first("COMPATIBILITY_VERSION"));
	    }
	    if(!project->isEmpty("QMAKE_LFLAGS_VERSION")) {
		project->variables()["QMAKE_LFLAGS"] += TQString(project->first("QMAKE_LFLAGS_VERSION") +
								project->first("VER_MAJ") + "." +
								project->first("VER_MIN") + "." +
								project->first("VER_PAT"));
	    }
	    if(!project->isActiveConfig("compile_libtool"))
		project->variables()["QMAKE_LFLAGS"] += project->variables()["QMAKE_LFLAGS_SONAME"];
	}
    }
    TQStringList &quc = project->variables()["QMAKE_EXTRA_UNIX_COMPILERS"];
    for(TQStringList::Iterator it = quc.begin(); it != quc.end(); ++it) {
	TQString tmp_out = project->variables()[(*it) + ".output"].first();
	if(tmp_out.isEmpty())
	    continue;
	TQStringList &tmp = project->variables()[(*it) + ".input"];
	for(TQStringList::Iterator it2 = tmp.begin(); it2 != tmp.end(); ++it2) {
	    TQStringList &inputs = project->variables()[(*it2)];
	    for(TQStringList::Iterator input = inputs.begin(); input != inputs.end(); ++input) {
		TQFileInfo fi(Option::fixPathToLocalOS((*input)));
		TQString in = Option::fixPathToTargetOS((*input), false),
		       out = tmp_out;
		out.replace("${QMAKE_FILE_BASE}", fi.baseName());
		out.replace("${QMAKE_FILE_NAME}", fi.filePath());
		if(project->variables()[(*it) + ".CONFIG"].findIndex("no_link") == -1)
		    project->variables()["OBJCOMP"] += out;
	    }
	}
    }
}

TQString
UnixMakefileGenerator::libtoolFileName()
{
    TQString ret = var("TARGET");
    int slsh = ret.findRev(Option::dir_sep);
    if(slsh != -1)
	ret = ret.right(ret.length() - slsh - 1);
    int dot = ret.find('.');
    if(dot != -1)
	ret = ret.left(dot);
    ret += Option::libtool_ext;
    if(!project->isEmpty("DESTDIR"))
	ret.prepend(var("DESTDIR"));
    return ret;
}

void
UnixMakefileGenerator::writeLibtoolFile()
{
    TQString fname = libtoolFileName(), lname = fname;
    int slsh = lname.findRev(Option::dir_sep);
    if(slsh != -1)
	lname = lname.right(lname.length() - slsh - 1);
    TQFile ft(fname);
    if(!ft.open(IO_WriteOnly))
	return;
    project->variables()["ALL_DEPS"].append(fname);

    TQTextStream t(&ft);
    t << "# " << lname << " - a libtool library file\n";
    t << "# Generated by qmake/libtool (" << qmake_version() << ") (TQt "
      << TQT_VERSION_STR << ")\n";

    t << "# The name that we can dlopen(3).\n"
      << "dlname='" << var(project->isActiveConfig("plugin") ? "TARGET" : "TARGET_x")
      << "'\n\n";

    t << "# Names of this library.\n";
    t << "library_names='";
    if(project->isActiveConfig("plugin")) {
	t << var("TARGET");
    } else {
	t << var("TARGET_x.y.z") << " " << var("TARGET_x.y") << " ";
	t << var("TARGET_x") << " " << var("TARGET_");
    }
    t << "'\n\n";

    t << "# The name of the static archive.\n"
      << "old_library='";
    if(project->isActiveConfig("staticlib")) {
	t << lname.left(lname.length()-Option::libtool_ext.length()) << ".a";
    }
    t << "'\n\n";

    t << "# Libraries that this one depends upon.\n";
    TQStringList libs;
    if(!project->isEmpty("QMAKE_INTERNAL_PRL_LIBS"))
	libs = project->variables()["QMAKE_INTERNAL_PRL_LIBS"];
    else
	libs << "QMAKE_LIBS"; //obvious one
    t << "dependency_libs='";
    for(TQStringList::ConstIterator it = libs.begin(); it != libs.end(); ++it)
	t << project->variables()[(*it)].join(" ") << " ";
    t << "'\n\n";

    t << "# Version information for " << lname << "\n";
    int maj = project->first("VER_MAJ").toInt();
    int min = project->first("VER_MIN").toInt();
    int pat = project->first("VER_PAT").toInt();
    t << "current=" << (10*maj + min) << "\n" // best I can think of
      << "age=0\n"
      << "revision=" << pat << "\n\n";

    t << "# Is this an already installed library.\n"
	"installed=yes\n\n"; // ###

    t << "# Files to dlopen/dlpreopen.\n"
	"dlopen=''\n"
	"dlpreopen=''\n\n";

    TQString install_dir = project->first("target.path");
    if(install_dir.isEmpty())
	install_dir = project->first("DESTDIR");
    t << "# Directory that this library needs to be installed in:\n"
	"libdir='" << Option::fixPathToTargetOS(install_dir, false) << "'\n";
}

TQString
UnixMakefileGenerator::pkgConfigFileName()
{
    TQString ret = var("TARGET");
    int slsh = ret.findRev(Option::dir_sep);
    if(slsh != -1)
	ret = ret.right(ret.length() - slsh - 1);
    if(ret.startsWith("lib"))
	ret = ret.mid(3);
    int dot = ret.find('.');
    if(dot != -1)
	ret = ret.left(dot);
    ret += Option::pkgcfg_ext;
    if(!project->isEmpty("DESTDIR")) {
	ret.prepend(var("DESTDIR"));
	ret = Option::fixPathToLocalOS(fileFixify(ret,TQDir::currentDirPath(), Option::output_dir));
    }
    return ret;
}

TQString
UnixMakefileGenerator::pkgConfigPrefix() const
{
    if(!project->isEmpty("QMAKE_PKGCONFIG_PREFIX"))
	return project->first("QMAKE_PKGCONFIG_PREFIX");
    return tqInstallPath();
}

TQString
UnixMakefileGenerator::pkgConfigFixPath(TQString path) const
{
    TQString prefix = pkgConfigPrefix();
    if(path.startsWith(prefix))
	path = path.replace(prefix, "${prefix}");
    return path;
}

void
UnixMakefileGenerator::writePkgConfigFile()     // ### does make sense only for libtqt so far
{
    TQString fname = pkgConfigFileName(), lname = fname;
    int slsh = lname.findRev(Option::dir_sep);
    if(slsh != -1)
	lname = lname.right(lname.length() - slsh - 1);
    TQFile ft(fname);
    if(!ft.open(IO_WriteOnly))
	return;
    project->variables()["ALL_DEPS"].append(fname);
    TQTextStream t(&ft);

	TQString prefix = pkgConfigPrefix();
	TQString libDir = tqInstallPathLibs();
	if (libDir.isEmpty())
	{
		libDir = prefix + "/lib";
	}
	TQString includeDir = tqInstallPathHeaders();
	if (includeDir.isEmpty())
	{
		includeDir = prefix + "/include";
	}
	TQString pluginsDir = tqInstallPathPlugins();
	if (pluginsDir.isEmpty())
	{
		pluginsDir = prefix + "/plugins";
	}
	TQString translationsDir = tqInstallPathTranslations();
	if (translationsDir.isEmpty())
	{
		translationsDir = prefix + "/translations";
	}

	t << "prefix=" << prefix << endl
	  << "exec_prefix=${prefix}" << endl
	  << "libdir=" << pkgConfigFixPath(libDir) << endl
	  << "includedir=" << pkgConfigFixPath(includeDir) << endl
	  // non-standard entry. Provides path for plugins
	  << "pluginsdir=" << pkgConfigFixPath(pluginsDir) << endl
	  // non-standard entry. Provides path for translations
	  << "translationsdir=" << pkgConfigFixPath(translationsDir) << endl
	  << endl;

    // non-standard entry. Provides useful info normally only
    // contained in the internal .qmake.cache file
    t << varGlue("CONFIG", "qt_config=", " ", "") << endl << endl;

    t << "Name: TQt" << endl;
    TQString desc = project->first("QMAKE_PKGCONFIG_DESCRIPTION");
    if(desc.isEmpty()) {
	desc = project->first("TARGET").lower();
	desc.replace(0, 1, desc[0].upper());
	if(project->first("TEMPLATE") == "lib") {
	    if(project->isActiveConfig("plugin"))
	       desc += " Plugin";
	    else
	       desc += " Library";
	} else if(project->first("TEMPLATE") == "app") {
	    desc += " Application";
	}
    }
    t << "Description: " << desc << endl;
    t << "Version: " << project->first("VERSION") << endl;

    // libs
    TQStringList libs;
    if(!project->isEmpty("QMAKE_INTERNAL_PRL_LIBS"))
	libs = project->variables()["QMAKE_INTERNAL_PRL_LIBS"];
    else
	libs << "QMAKE_LIBS"; //obvious one
    if(project->isActiveConfig("thread"))
	libs << "QMAKE_LFLAGS_THREAD"; //not sure about this one, but what about things like -pthread?
    t << "Libs: -L${libdir} -l" << lname.left(lname.length()-Option::libtool_ext.length()) << " ";
    for(TQStringList::ConstIterator it = libs.begin(); it != libs.end(); ++it)
	t << project->variables()[(*it)].join(" ") << " ";
    t << endl;

    // flags
    // ### too many
    t << "Cflags: "
	// << var("QMAKE_CXXFLAGS") << " "
      << varGlue("PRL_EXPORT_DEFINES","-D"," -D"," ")
      << project->variables()["PRL_EXPORT_CXXFLAGS"].join(" ")
	//      << varGlue("DEFINES","-D"," -D"," ")
      << " -I${includedir}";
    t << endl;
}
