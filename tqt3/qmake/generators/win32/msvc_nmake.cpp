/****************************************************************************
**
** Implementation of NmakeMakefileGenerator class.
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

#include "msvc_nmake.h"
#include "option.h"
#include <ntqregexp.h>
#include <ntqdict.h>
#include <ntqdir.h>
#include <stdlib.h>
#include <time.h>

NmakeMakefileGenerator::NmakeMakefileGenerator(TQMakeProject *p) : Win32MakefileGenerator(p), init_flag(false)
{

}

bool
NmakeMakefileGenerator::writeMakefile(TQTextStream &t)
{
    writeHeader(t);
    if(!project->variables()["QMAKE_FAILED_REQUIREMENTS"].isEmpty()) {
	{ //write the extra unix targets..
	    TQStringList &qut = project->variables()["QMAKE_EXTRA_WIN_TARGETS"];
	    for(TQStringList::ConstIterator it = qut.begin(); it != qut.end(); ++it)
		t << *it << " ";
	}
	t << "all clean:" << "\n\t"
	  << "@echo \"Some of the required modules ("
	  << var("QMAKE_FAILED_REQUIREMENTS") << ") are not available.\"" << "\n\t"
	  << "@echo \"Skipped.\"" << endl << endl;
	writeMakeQmake(t);
	return true;
    }

    if(project->first("TEMPLATE") == "app" ||
       project->first("TEMPLATE") == "lib") {
	writeNmakeParts(t);
	return MakefileGenerator::writeMakefile(t);
    }
    else if(project->first("TEMPLATE") == "subdirs") {
	writeSubDirs(t);
	return true;
    }
    return false;
}

TQStringList
&NmakeMakefileGenerator::findDependencies(const TQString &file)
{
    TQStringList &aList = MakefileGenerator::findDependencies(file);
    // Note: The QMAKE_IMAGE_COLLECTION file have all images
    // as dependency, so don't add precompiled header then
    if (file == project->first("QMAKE_IMAGE_COLLECTION"))
	return aList;
    for(TQStringList::Iterator it = Option::cpp_ext.begin(); it != Option::cpp_ext.end(); ++it) {
	if(file.endsWith(*it)) {
            if(!precompObj.isEmpty() && !aList.contains(precompObj))
                aList += precompObj;
	    break;
	}
    }
    return aList;
}

void
NmakeMakefileGenerator::writeNmakeParts(TQTextStream &t)
{
    t << "####### Compiler, tools and options" << endl << endl;
    t << "CC		=	" << var("QMAKE_CC") << endl;
    t << "CXX		=	" << var("QMAKE_CXX") << endl;
    t << "LEX		= " << var("QMAKE_LEX") << endl;
    t << "YACC		= " << var("QMAKE_YACC") << endl;
    t << "CFLAGS	=	" << var("QMAKE_CFLAGS") << " "
      << varGlue("PRL_EXPORT_DEFINES","-D"," -D","") << " "
      <<  varGlue("DEFINES","-D"," -D","") << endl;
    t << "CXXFLAGS	=	" << var("QMAKE_CXXFLAGS") << " "
      << varGlue("PRL_EXPORT_DEFINES","-D"," -D","") << " "
      << varGlue("DEFINES","-D"," -D","") << endl;
    t << "LEXFLAGS	=" << var("QMAKE_LEXFLAGS") << endl;
    t << "YACCFLAGS	=" << var("QMAKE_YACCFLAGS") << endl;

    t << "INCPATH	=	";
    TQStringList &incs = project->variables()["INCLUDEPATH"];
    for(TQStringList::Iterator incit = incs.begin(); incit != incs.end(); ++incit) {
	TQString inc = (*incit);
	if (inc.endsWith("\\"))
	    inc.truncate(inc.length()-1);
	if (inc.startsWith("\"") && inc.endsWith("\""))
	    inc = inc.mid(1, inc.length() - 2);
	t << " -I\"" << inc << "\"";
    }
    t << " -I\"" << specdir() << "\""
      << endl;
    if(!project->variables()["QMAKE_APP_OR_DLL"].isEmpty()) {
	t << "LINK	=	" << var("QMAKE_LINK") << endl;
	t << "LFLAGS	=	" << var("QMAKE_LFLAGS");
	if ( !project->variables()["QMAKE_LIBDIR"].isEmpty() )
	  t << " " << varGlue("QMAKE_LIBDIR","/LIBPATH:\"","\" /LIBPATH:\"","\"");
	t << endl;
	t << "LIBS	=	";
	TQStringList &libs = project->variables()["QMAKE_LIBS"];
	for(TQStringList::Iterator libit = libs.begin(); libit != libs.end(); ++libit) {
	    TQString lib = (*libit);
	    if (lib.endsWith("\\"))
		lib.truncate(lib.length()-1);
	    t << " \"" << lib << "\"";
	}
	t << endl;
    }
    else {
	t << "LIB	=	" << var("QMAKE_LIB") << endl;
    }
    t << "MOC		=	" << (project->isEmpty("QMAKE_MOC") ? TQString("moc") :
			      Option::fixPathToTargetOS(var("QMAKE_MOC"), false)) << endl;
    t << "UIC		=	" << (project->isEmpty("QMAKE_UIC") ? TQString("uic") :
			      Option::fixPathToTargetOS(var("QMAKE_UIC"), false)) << endl;
    t << "QMAKE		=	" << (project->isEmpty("QMAKE_QMAKE") ? TQString("qmake") :
			      Option::fixPathToTargetOS(var("QMAKE_QMAKE"), false)) << endl;
    t << "IDC		=	" << (project->isEmpty("QMAKE_IDC") ? TQString("idc") :
			      Option::fixPathToTargetOS(var("QMAKE_IDC"), false)) << endl;
    t << "IDL		=	" << (project->isEmpty("QMAKE_IDL") ? TQString("midl") :
			      Option::fixPathToTargetOS(var("QMAKE_IDL"), false)) << endl;
    t << "ZIP		=	" << var("QMAKE_ZIP") << endl;
    t << "COPY_FILE	=       " << var("QMAKE_COPY") << endl;
    t << "COPY_DIR	=       " << var("QMAKE_COPY") << endl;
    t << "DEL_FILE	=       " << var("QMAKE_DEL_FILE") << endl;
    t << "DEL_DIR	=       " << var("QMAKE_DEL_DIR") << endl;
    t << "MOVE		=       " << var("QMAKE_MOVE") << endl;
    t << "CHK_DIR_EXISTS =	" << var("QMAKE_CHK_DIR_EXISTS") << endl;
    t << "MKDIR		=	" << var("QMAKE_MKDIR") << endl;
    t << "INSTALL_FILE= " << var("QMAKE_INSTALL_FILE") << endl;
    t << "INSTALL_DIR = " << var("QMAKE_INSTALL_DIR") << endl;
    t << endl;

    t << "####### Files" << endl << endl;
    t << "HEADERS =	" << varList("HEADERS") << endl;
    t << "SOURCES =	" << varList("SOURCES") << endl;
    t << "OBJECTS =	" << varList("OBJECTS") << endl;
    t << "FORMS =	" << varList("FORMS") << endl;
    t << "UICDECLS =	" << varList("UICDECLS") << endl;
    t << "UICIMPLS =	" << varList("UICIMPLS") << endl;
    t << "SRCMOC	=	" << varList("SRCMOC") << endl;
    t << "OBJMOC	=	" << varList("OBJMOC") << endl;

    TQString extraCompilerDeps;
    if(!project->isEmpty("QMAKE_EXTRA_WIN_COMPILERS")) {
	t << "OBJCOMP = " << varList("OBJCOMP") << endl;
	extraCompilerDeps += " $(OBJCOMP) ";

	TQStringList &comps = project->variables()["QMAKE_EXTRA_WIN_COMPILERS"];
	for(TQStringList::Iterator compit = comps.begin(); compit != comps.end(); ++compit) {
	    TQStringList &vars = project->variables()[(*compit) + ".variables"];
	    for(TQStringList::Iterator varit = vars.begin(); varit != vars.end(); ++varit) {
		TQStringList vals = project->variables()[(*varit)];
		if(!vals.isEmpty())
		    t << "QMAKE_COMP_" << (*varit) << " = " << valList(vals) << endl;
	    }
	}
    }

    t << "DIST	=	" << varList("DISTFILES") << endl;
    t << "TARGET	=	";
    if( !project->variables()[ "DESTDIR" ].isEmpty() )
	t << varGlue("TARGET",project->first("DESTDIR"),"",project->first("TARGET_EXT"));
    else
	t << project->variables()[ "TARGET" ].first() << project->variables()[ "TARGET_EXT" ].first();
    t << endl;
    t << endl;

    t << "####### Implicit rules" << endl << endl;
    t << ".SUFFIXES: .c";
    TQStringList::Iterator cppit;
    for(cppit = Option::cpp_ext.begin(); cppit != Option::cpp_ext.end(); ++cppit)
	t << " " << (*cppit);
    t << endl << endl;

    if(!project->isActiveConfig("no_batch")) {
	// Batchmode doesn't use the non implicit rules QMAKE_RUN_CXX & QMAKE_RUN_CC
	project->variables().remove("QMAKE_RUN_CXX");
	project->variables().remove("QMAKE_RUN_CC");

	TQDict<void> source_directories;
	source_directories.insert(".", (void*)1);
	TQString directories[] = { TQString("MOC_DIR"), TQString("UI_SOURCES_DIR"), TQString("UI_DIR"), TQString::null };
	for(int y = 0; !directories[y].isNull(); y++) {
	    TQString dirTemp = project->first(directories[y]);
	    if (dirTemp.endsWith("\\"))
		dirTemp.truncate(dirTemp.length()-1);
	    if(!dirTemp.isEmpty())
		source_directories.insert(dirTemp, (void*)1);
	}
	TQString srcs[] = { TQString("SOURCES"), TQString("UICIMPLS"), TQString("SRCMOC"), TQString::null };
	for(int x = 0; !srcs[x].isNull(); x++) {
	    TQStringList &l = project->variables()[srcs[x]];
	    for(TQStringList::Iterator sit = l.begin(); sit != l.end(); ++sit) {
		TQString sep = "\\";
		if((*sit).find(sep) == -1)
		    sep = "/";
		TQString dir = (*sit).section(sep, 0, -2);
		if(!dir.isEmpty() && !source_directories[dir])
		    source_directories.insert(dir, (void*)1);
	    }
	}

	for(TQDictIterator<void> it(source_directories); it.current(); ++it) {
	    if(it.currentKey().isEmpty())
		continue;
	    for(cppit = Option::cpp_ext.begin(); cppit != Option::cpp_ext.end(); ++cppit)
		t << "{" << it.currentKey() << "}" << (*cppit) << "{" << var("OBJECTS_DIR") << "}" << Option::obj_ext << "::\n\t"
		  << var("QMAKE_RUN_CXX_IMP_BATCH").replace( TQRegExp( "\\$@" ), var("OBJECTS_DIR") ) << endl << "\t$<" << endl << "<<" << endl << endl;
	    t << "{" << it.currentKey() << "}" << ".c{" << var("OBJECTS_DIR") << "}" << Option::obj_ext << "::\n\t"
	      << var("QMAKE_RUN_CC_IMP_BATCH").replace( TQRegExp( "\\$@" ), var("OBJECTS_DIR") ) << endl << "\t$<" << endl << "<<" << endl << endl;
	}
    } else {
	for(cppit = Option::cpp_ext.begin(); cppit != Option::cpp_ext.end(); ++cppit)
	    t << (*cppit) << Option::obj_ext << ":\n\t" << var("QMAKE_RUN_CXX_IMP") << endl << endl;
	t << ".c" << Option::obj_ext << ":\n\t" << var("QMAKE_RUN_CC_IMP") << endl << endl;
    }

    t << "####### Build rules" << endl << endl;
    t << "all: " << fileFixify(Option::output.name()) << " " << varGlue("ALL_DEPS"," "," "," ") << "$(TARGET)" << endl << endl;
    t << "$(TARGET): " << var("PRE_TARGETDEPS") << " $(UICDECLS) $(OBJECTS) $(OBJMOC) "
      << extraCompilerDeps << var("POST_TARGETDEPS");
    if(!project->variables()["QMAKE_APP_OR_DLL"].isEmpty()) {
	t << "\n\t" << "$(LINK) $(LFLAGS) /OUT:$(TARGET) @<< " << "\n\t  "
	  << "$(OBJECTS) $(OBJMOC) $(LIBS)";
    } else {
	t << "\n\t" << "$(LIB) /OUT:$(TARGET) @<<" << "\n\t  "
	  << "$(OBJECTS) $(OBJMOC)";
    }
    t << extraCompilerDeps;
    t << endl << "<<" << endl;
    if ( !project->variables()["QMAKE_POST_LINK"].isEmpty() )
	t << "\t" << var( "QMAKE_POST_LINK" ) << endl;
    if(project->isActiveConfig("dll") && !project->variables()["DLLDESTDIR"].isEmpty()) {
	TQStringList dlldirs = project->variables()["DLLDESTDIR"];
	for ( TQStringList::Iterator dlldir = dlldirs.begin(); dlldir != dlldirs.end(); ++dlldir ) {
	    t << "\n\t" << "-$(COPY_FILE) \"$(TARGET)\" " << *dlldir;
	}
    }
    TQString targetfilename = project->variables()["TARGET"].first();
    if(project->isActiveConfig("activeqt")) {
	TQString version = project->variables()["VERSION"].first();
	if ( version.isEmpty() )
	    version = "1.0";

	if ( project->isActiveConfig("dll")) {
	    t << "\n\t" << ("-$(IDC) $(TARGET) /idl " + var("OBJECTS_DIR") + targetfilename + ".idl -version " + version);
	    t << "\n\t" << ("-$(IDL) /nologo " + var("OBJECTS_DIR") + targetfilename + ".idl /tlb " + var("OBJECTS_DIR") + targetfilename + ".tlb");
	    t << "\n\t" << ("-$(IDC) $(TARGET) /tlb " + var("OBJECTS_DIR") + targetfilename + ".tlb");
	    t << "\n\t" << ("-$(IDC) $(TARGET) /regserver" );
	} else {
	    t << "\n\t" << ("-$(TARGET) -dumpidl " + var("OBJECTS_DIR") + targetfilename + ".idl -version " + version);
	    t << "\n\t" << ("-$(IDL) /nologo " + var("OBJECTS_DIR") + targetfilename + ".idl /tlb " + var("OBJECTS_DIR") + targetfilename + ".tlb");
	    t << "\n\t" << ("-$(IDC) $(TARGET) /tlb " + var("OBJECTS_DIR") + targetfilename + ".tlb");
	    t << "\n\t" << "-$(TARGET) -regserver";
	}
    }
    t << endl << endl;

    if(!project->variables()["RC_FILE"].isEmpty()) {
	t << var("RES_FILE") << ": " << var("RC_FILE") << "\n\t"
	  << var("QMAKE_RC") << " " << var("RC_FILE") << endl << endl;
    }

    t << "mocables: $(SRCMOC)" << endl
      << "uicables: $(UICIMPLS) $(UICDECLS)" << endl << endl;

    writeMakeQmake(t);

    TQStringList dist_files = Option::mkfile::project_files;
    if(!project->isEmpty("QMAKE_INTERNAL_INCLUDED_FILES"))
	dist_files += project->variables()["QMAKE_INTERNAL_INCLUDED_FILES"];
    if(!project->isEmpty("TRANSLATIONS"))
	dist_files << var("TRANSLATIONS");
    if(!project->isEmpty("FORMS")) {
	TQStringList &forms = project->variables()["FORMS"];
	for(TQStringList::Iterator formit = forms.begin(); formit != forms.end(); ++formit) {
	    TQString ui_h = fileFixify((*formit) + Option::h_ext.first());
	    if(TQFile::exists(ui_h) )
		dist_files << ui_h;
	}
    }
    t << "dist:" << "\n\t"
      << "$(ZIP) " << var("QMAKE_ORIG_TARGET") << ".zip " << "$(SOURCES) $(HEADERS) $(DIST) $(FORMS) "
      << dist_files.join(" ") << " " << var("TRANSLATIONS") << " " << var("IMAGES") << endl << endl;

    t << "uiclean:"
      << varGlue("UICDECLS" ,"\n\t-$(DEL_FILE) ","\n\t-$(DEL_FILE) ","")
      << varGlue("UICIMPLS" ,"\n\t-$(DEL_FILE) ","\n\t-$(DEL_FILE) ","") << endl;

    t << "mocclean:"
      << varGlue("SRCMOC" ,"\n\t-$(DEL_FILE) ","\n\t-$(DEL_FILE) ","")
      << varGlue("OBJMOC" ,"\n\t-$(DEL_FILE) ","\n\t-$(DEL_FILE) ","") << endl;

    t << "clean: uiclean mocclean"
      << varGlue("OBJECTS","\n\t-$(DEL_FILE) ","\n\t-$(DEL_FILE) ","")
      << varGlue("QMAKE_CLEAN","\n\t-$(DEL_FILE) ","\n\t-$(DEL_FILE) ","\n")
      << varGlue("CLEAN_FILES","\n\t-$(DEL_FILE) ","\n\t-$(DEL_FILE) ","\n");
    if ( project->isActiveConfig("activeqt")) {
	t << ("\n\t-$(DEL_FILE) " + var("OBJECTS_DIR") + targetfilename + ".idl");
	t << ("\n\t-$(DEL_FILE) " + var("OBJECTS_DIR") + targetfilename + ".tlb");
    }
    if(!project->isEmpty("IMAGES"))
	t << varGlue("QMAKE_IMAGE_COLLECTION", "\n\t-$(DEL_FILE) ", "\n\t-$(DEL_FILE) ", "");
    t << endl;

    // user defined targets

    TQStringList::Iterator it;
    TQStringList &qut = project->variables()["QMAKE_EXTRA_WIN_TARGETS"];
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
	if(!project->variables()["QMAKE_NOFORCE"].isEmpty() &&
	   project->variables()[(*it) + ".CONFIG"].findIndex("phony") != -1)
	    deps += TQString(" ") + "FORCE";
	t << "\n\n" << targ << ":" << deps << "\n\t"
	  << cmd;
    }
    t << endl << endl;

    TQStringList &quc = project->variables()["QMAKE_EXTRA_WIN_COMPILERS"];
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
    t << endl;

    if(project->variables()["QMAKE_NOFORCE"].isEmpty())
	t << "FORCE:" << endl << endl;

    t << "distclean: clean"
      << "\n\t-$(DEL_FILE) $(TARGET)"
      << endl << endl;

    // precompiled header
    if(usePCH) {
	TQString precompRule = TQString("-c -Yc -Fp%1 -Fo%2").arg(precompPch).arg(precompObj);
	t << precompObj << ": " << precompH << " " << findDependencies(precompH).join(" \\\n\t\t")
	  << "\n\t" << ("$(CXX) " + precompRule + " $(CXXFLAGS) $(INCPATH) -TP ") << precompH << endl << endl;
    }
}

TQString
NmakeMakefileGenerator::var(const TQString &value)
{
    if (usePCH) {
    	if ((value == "QMAKE_RUN_CXX_IMP_BATCH"
	    || value == "QMAKE_RUN_CXX_IMP"
	    || value == "QMAKE_RUN_CXX")) {
	    TQFileInfo precompHInfo(precompH);
	    TQString precompRule = TQString("-c -FI%1 -Yu%2 -Fp%3")
		.arg(precompHInfo.fileName())
		.arg(precompHInfo.fileName())
		.arg(precompPch);
	    TQString p = MakefileGenerator::var(value);
	    p.replace("-c", precompRule);
	    // Cannot use -Gm with -FI & -Yu, as this gives an
	    // internal compiler error, on the newer compilers
	    p.remove("-Gm");
	    return p;
	} else if (value == "QMAKE_CXXFLAGS") {
	    // Remove internal compiler error option
	    return MakefileGenerator::var(value).remove("-Gm");
	}
    }

    // Normal val
    return MakefileGenerator::var(value);
}

void
NmakeMakefileGenerator::init()
{
    if(init_flag)
	return;
    init_flag = true;

    /* this should probably not be here, but I'm using it to wrap the .t files */
    if(project->first("TEMPLATE") == "app")
	project->variables()["QMAKE_APP_FLAG"].append("1");
    else if(project->first("TEMPLATE") == "lib")
	project->variables()["QMAKE_LIB_FLAG"].append("1");
    else if(project->first("TEMPLATE") == "subdirs") {
	MakefileGenerator::init();
	if(project->variables()["MAKEFILE"].isEmpty())
	    project->variables()["MAKEFILE"].append("Makefile");
	if(project->variables()["QMAKE"].isEmpty())
	    project->variables()["QMAKE"].append("qmake");
	return;
    }

    if(project->isEmpty("QMAKE_INSTALL_FILE"))
	project->variables()["QMAKE_INSTALL_FILE"].append("$(COPY_FILE)");
    if(project->isEmpty("QMAKE_INSTALL_DIR"))
	project->variables()["QMAKE_INSTALL_DIR"].append("$(COPY_DIR)");

    bool is_qt = (project->first("TARGET") == "qt" TQTDLL_POSTFIX || project->first("TARGET") == "tqt-mt" TQTDLL_POSTFIX);
    project->variables()["QMAKE_ORIG_TARGET"] = project->variables()["TARGET"];

    TQString targetfilename = project->variables()["TARGET"].first();
    TQStringList &configs = project->variables()["CONFIG"];
    if (project->isActiveConfig("qt") && project->isActiveConfig("shared"))
	project->variables()["DEFINES"].append("QT_DLL");
    if (project->isActiveConfig("qt_dll"))
	if(configs.findIndex("qt") == -1) configs.append("qt");
    if ( project->isActiveConfig("qtopia") ) {
	if(configs.findIndex("qtopialib") == -1)
	    configs.append("qtopialib");
	if(configs.findIndex("qtopiainc") == -1)
	    configs.append("qtopiainc");
    }
    if ( project->isActiveConfig("qt") ) {
	if ( project->isActiveConfig( "plugin" ) ) {
	    project->variables()["CONFIG"].append("dll");
	    if(project->isActiveConfig("qt"))
		project->variables()["DEFINES"].append("QT_PLUGIN");
	}
	if ( (project->variables()["DEFINES"].findIndex("QT_NODLL") == -1) &&
         ((project->variables()["DEFINES"].findIndex("QT_MAKEDLL") != -1 ||
           project->variables()["DEFINES"].findIndex("QT_DLL") != -1) ||
          (getenv("QT_DLL") && !getenv("QT_NODLL"))) ) {
	    project->variables()["QMAKE_QT_DLL"].append("1");
	    if ( is_qt && !project->variables()["QMAKE_LIB_FLAG"].isEmpty() )
		project->variables()["CONFIG"].append("dll");
	}
	if ( project->isActiveConfig("thread") )
	    project->variables()[is_qt ? "PRL_EXPORT_DEFINES" : "DEFINES"].append("TQT_THREAD_SUPPORT");
	if ( project->isActiveConfig("accessibility" ) )
	    project->variables()[is_qt ? "PRL_EXPORT_DEFINES" : "DEFINES"].append("QT_ACCESSIBILITY_SUPPORT");
	if ( project->isActiveConfig("tablet") )
	    project->variables()[is_qt ? "PRL_EXPORT_DEFINES" : "DEFINES"].append("QT_TABLET_SUPPORT");
    }
    if ( project->isActiveConfig("dll") || !project->variables()["QMAKE_APP_FLAG"].isEmpty() ) {
	project->variables()["CONFIG"].remove("staticlib");
	project->variables()["QMAKE_APP_OR_DLL"].append("1");
    } else {
	project->variables()["CONFIG"].append("staticlib");
    }
    if ( project->isActiveConfig("warn_off") ) {
	project->variables()["QMAKE_CFLAGS"] += project->variables()["QMAKE_CFLAGS_WARN_OFF"];
	project->variables()["QMAKE_CXXFLAGS"] += project->variables()["QMAKE_CXXFLAGS_WARN_OFF"];
    } else if ( project->isActiveConfig("warn_on") ) {
	project->variables()["QMAKE_CFLAGS"] += project->variables()["QMAKE_CFLAGS_WARN_ON"];
	project->variables()["QMAKE_CXXFLAGS"] += project->variables()["QMAKE_CXXFLAGS_WARN_ON"];
    }
    if ( project->isActiveConfig("debug") ) {
        if ( project->isActiveConfig("thread") ) {
	    // use the DLL RT even here
	    if ( project->variables()["DEFINES"].contains("QT_DLL") ) {
		project->variables()["QMAKE_CFLAGS"] += project->variables()["QMAKE_CFLAGS_MT_DLLDBG"];
		project->variables()["QMAKE_CXXFLAGS"] += project->variables()["QMAKE_CXXFLAGS_MT_DLLDBG"];
	    } else {
		project->variables()["QMAKE_CFLAGS"] += project->variables()["QMAKE_CFLAGS_MT_DBG"];
		project->variables()["QMAKE_CXXFLAGS"] += project->variables()["QMAKE_CXXFLAGS_MT_DBG"];
	    }
	}
	project->variables()["QMAKE_CFLAGS"] += project->variables()["QMAKE_CFLAGS_DEBUG"];
	project->variables()["QMAKE_CXXFLAGS"] += project->variables()["QMAKE_CXXFLAGS_DEBUG"];
	project->variables()["QMAKE_LFLAGS"] += project->variables()["QMAKE_LFLAGS_DEBUG"];
    } else {
	if ( project->isActiveConfig("thread") ) {
	    if ( project->variables()["DEFINES"].contains("QT_DLL") ) {
		project->variables()["QMAKE_CFLAGS"] += project->variables()["QMAKE_CFLAGS_MT_DLL"];
		project->variables()["QMAKE_CXXFLAGS"] += project->variables()["QMAKE_CXXFLAGS_MT_DLL"];
	    } else {
		project->variables()["QMAKE_CFLAGS"] += project->variables()["QMAKE_CFLAGS_MT"];
		project->variables()["QMAKE_CXXFLAGS"] += project->variables()["QMAKE_CXXFLAGS_MT"];
	    }
	}
	project->variables()["QMAKE_CFLAGS"] += project->variables()["QMAKE_CFLAGS_RELEASE"];
	project->variables()["QMAKE_CXXFLAGS"] += project->variables()["QMAKE_CXXFLAGS_RELEASE"];
	project->variables()["QMAKE_LFLAGS"] += project->variables()["QMAKE_LFLAGS_RELEASE"];
    }
    if ( project->isActiveConfig("thread") && !project->variables()["DEFINES"].contains("QT_DLL")
	&& !is_qt && project->first("TARGET") != "qtmain") {
	project->variables()["QMAKE_LFLAGS"].append("/NODEFAULTLIB:\"libc\"");
    }

    if ( !project->variables()["QMAKE_INCDIR"].isEmpty())
	project->variables()["INCLUDEPATH"] += project->variables()["QMAKE_INCDIR"];
    if ( project->isActiveConfig("qt") || project->isActiveConfig("opengl") )
	project->variables()["CONFIG"].append("windows");
    if ( project->isActiveConfig("qtopiainc") )
	project->variables()["INCLUDEPATH"] += project->variables()["QMAKE_INCDIR_TQTOPIA"];
    if ( project->isActiveConfig("qtopialib") ) {
	if(!project->isEmpty("QMAKE_LIBDIR_TQTOPIA"))
	    project->variables()["QMAKE_LIBDIR"] += project->variables()["QMAKE_LIBDIR_TQTOPIA"];
	project->variables()["QMAKE_LIBS"] += project->variables()["QMAKE_LIBS_QTOPIA"];
    }
    if ( project->isActiveConfig("qt") ) {
	project->variables()["CONFIG"].append("moc");
	project->variables()["INCLUDEPATH"] +=	project->variables()["QMAKE_INCDIR_TQT"];
	project->variables()["QMAKE_LIBDIR"] += project->variables()["QMAKE_LIBDIR_TQT"];
	if ( !project->isActiveConfig("debug") )
	    project->variables()[is_qt ? "PRL_EXPORT_DEFINES" : "DEFINES"].append("TQT_NO_DEBUG");
	if ( is_qt && !project->variables()["QMAKE_LIB_FLAG"].isEmpty() ) {
	    if ( !project->variables()["QMAKE_QT_DLL"].isEmpty()) {
		project->variables()["DEFINES"].append("QT_MAKEDLL");
		project->variables()["QMAKE_LFLAGS"] += project->variables()["QMAKE_LFLAGS_QT_DLL"];
	    }
	} else {
	    if(project->isActiveConfig("thread"))
		project->variables()["QMAKE_LIBS"] += project->variables()["QMAKE_LIBS_QT_THREAD"];
	    else
		project->variables()["QMAKE_LIBS"] += project->variables()["QMAKE_LIBS_QT"];
	    if ( !project->variables()["QMAKE_QT_DLL"].isEmpty() ) {
		int hver = findHighestVersion(project->first("QMAKE_LIBDIR_TQT"), "qt");
		if ( hver == -1 )
		    hver = findHighestVersion(project->first("QMAKE_LIBDIR_TQT"), "tqt-mt");
		if(hver != -1) {
		    TQString ver;
		    ver.sprintf("qt%s" TQTDLL_POSTFIX "%d.lib", (project->isActiveConfig("thread") ? "-mt" : ""), hver);
		    TQStringList &libs = project->variables()["QMAKE_LIBS"];
		    for(TQStringList::Iterator libit = libs.begin(); libit != libs.end(); ++libit)
			(*libit).replace(TQRegExp("qt(-mt)?\\.lib"), ver);
		}
	    }
	    if ( project->isActiveConfig( "activeqt" ) ) {
		project->variables().remove("QMAKE_LIBS_QT_ENTRY");
		project->variables()["QMAKE_LIBS_QT_ENTRY"] = "qaxserver.lib";
		if ( project->isActiveConfig( "dll" ) )
		    project->variables()["QMAKE_LIBS"] += project->variables()["QMAKE_LIBS_QT_ENTRY"];
	    }
	    if ( !project->isActiveConfig("dll") && !project->isActiveConfig("plugin") ) {
		project->variables()["QMAKE_LIBS"] +=project->variables()["QMAKE_LIBS_QT_ENTRY"];
	    }
	}
    }
    if ( project->isActiveConfig("opengl") ) {
	project->variables()["QMAKE_LIBS"] += project->variables()["QMAKE_LIBS_OPENGL"];
	project->variables()["QMAKE_LFLAGS"] += project->variables()["QMAKE_LFLAGS_OPENGL"];
    }
    if ( project->isActiveConfig("dll") ) {
	project->variables()["QMAKE_CFLAGS_CONSOLE_ANY"] = project->variables()["QMAKE_CFLAGS_CONSOLE_DLL"];
	project->variables()["QMAKE_CXXFLAGS_CONSOLE_ANY"] = project->variables()["QMAKE_CXXFLAGS_CONSOLE_DLL"];
	project->variables()["QMAKE_LFLAGS_CONSOLE_ANY"] = project->variables()["QMAKE_LFLAGS_CONSOLE_DLL"];
	project->variables()["QMAKE_LFLAGS_WINDOWS_ANY"] = project->variables()["QMAKE_LFLAGS_WINDOWS_DLL"];
	if ( !project->variables()["QMAKE_LIB_FLAG"].isEmpty()) {
	    project->variables()["TARGET_EXT"].append(
		TQStringList::split('.',project->first("VERSION")).join("") + ".dll");
	} else {
	    project->variables()["TARGET_EXT"].append(".dll");
	}
    } else {
	project->variables()["QMAKE_CFLAGS_CONSOLE_ANY"] = project->variables()["QMAKE_CFLAGS_CONSOLE"];
	project->variables()["QMAKE_CXXFLAGS_CONSOLE_ANY"] = project->variables()["QMAKE_CXXFLAGS_CONSOLE"];
	project->variables()["QMAKE_LFLAGS_CONSOLE_ANY"] = project->variables()["QMAKE_LFLAGS_CONSOLE"];
	project->variables()["QMAKE_LFLAGS_WINDOWS_ANY"] = project->variables()["QMAKE_LFLAGS_WINDOWS"];
	if ( !project->variables()["QMAKE_APP_FLAG"].isEmpty()) {
	    project->variables()["TARGET_EXT"].append(".exe");
	} else {
	    project->variables()["TARGET_EXT"].append(".lib");
	}
    }
    if ( project->isActiveConfig("windows") ) {
	if ( project->isActiveConfig("console") ) {
	    project->variables()["QMAKE_CFLAGS"] += project->variables()["QMAKE_CFLAGS_CONSOLE_ANY"];
	    project->variables()["QMAKE_CXXFLAGS"] += project->variables()["QMAKE_CXXFLAGS_CONSOLE_ANY"];
	    project->variables()["QMAKE_LFLAGS"] += project->variables()["QMAKE_LFLAGS_CONSOLE_ANY"];
	    project->variables()["QMAKE_LIBS"] += project->variables()["QMAKE_LIBS_CONSOLE"];
	} else {
	    project->variables()["QMAKE_LFLAGS"] += project->variables()["QMAKE_LFLAGS_WINDOWS_ANY"];
	}
	project->variables()["QMAKE_LIBS"] += project->variables()["QMAKE_LIBS_WINDOWS"];
    } else {
	project->variables()["QMAKE_CFLAGS"] += project->variables()["QMAKE_CFLAGS_CONSOLE_ANY"];
	project->variables()["QMAKE_CXXFLAGS"] += project->variables()["QMAKE_CXXFLAGS_CONSOLE_ANY"];
	project->variables()["QMAKE_LFLAGS"] += project->variables()["QMAKE_LFLAGS_CONSOLE_ANY"];
	project->variables()["QMAKE_LIBS"] += project->variables()["QMAKE_LIBS_CONSOLE"];
    }
    if ( project->isActiveConfig("stl") ) {
	project->variables()["QMAKE_CFLAGS"] += project->variables()["QMAKE_CFLAGS_STL_ON"];
	project->variables()["QMAKE_CXXFLAGS"] += project->variables()["QMAKE_CXXFLAGS_STL_ON"];
    } else {
	project->variables()["QMAKE_CFLAGS"] += project->variables()["QMAKE_CFLAGS_STL_OFF"];
	project->variables()["QMAKE_CXXFLAGS"] += project->variables()["QMAKE_CXXFLAGS_STL_OFF"];
    }
    if ( project->isActiveConfig("exceptions") ) {
	project->variables()["QMAKE_CFLAGS"] += project->variables()["QMAKE_CFLAGS_EXCEPTIONS_ON"];
	project->variables()["QMAKE_CXXFLAGS"] += project->variables()["QMAKE_CXXFLAGS_EXCEPTIONS_ON"];
    } else {
	project->variables()["QMAKE_CFLAGS"] += project->variables()["QMAKE_CFLAGS_EXCEPTIONS_OFF"];
	project->variables()["QMAKE_CXXFLAGS"] += project->variables()["QMAKE_CXXFLAGS_EXCEPTIONS_OFF"];
    }
    if ( project->isActiveConfig("rtti") ) {
	project->variables()["QMAKE_CFLAGS"] += project->variables()["QMAKE_CFLAGS_RTTI_ON"];
	project->variables()["QMAKE_CXXFLAGS"] += project->variables()["QMAKE_CXXFLAGS_RTTI_ON"];
    } else {
	project->variables()["QMAKE_CFLAGS"] += project->variables()["QMAKE_CFLAGS_RTTI_OFF"];
	project->variables()["QMAKE_CXXFLAGS"] += project->variables()["QMAKE_CXXFLAGS_RTTI_OFF"];
    }


    if ( project->isActiveConfig("moc") )
	setMocAware(true);
    project->variables()["QMAKE_LIBS"] += project->variables()["LIBS"];

    TQStringList &libList = project->variables()["QMAKE_LIBS"];
    for( TQStringList::Iterator stIt = libList.begin(); stIt != libList.end(); ) {
	TQString s = *stIt;
	if( s.startsWith( "-l" ) ) {
	    stIt = libList.remove( stIt );
	    stIt = libList.insert( stIt, s.mid( 2 ) + ".lib" );
        } else if( s.startsWith( "-L" ) ) {
	    stIt = libList.remove( stIt );
	    project->variables()["QMAKE_LIBDIR"].append(TQDir::convertSeparators(s.mid( 2 )));
	} else {
	    stIt++;
	}
    }

    project->variables()["QMAKE_FILETAGS"] += TQStringList::split(' ',
	"HEADERS SOURCES DEF_FILE RC_FILE TARGET QMAKE_LIBS DESTDIR DLLDESTDIR INCLUDEPATH");
    TQStringList &l = project->variables()["QMAKE_FILETAGS"];
    TQStringList::Iterator it;
    for(it = l.begin(); it != l.end(); ++it) {
	TQStringList &gdmf = project->variables()[(*it)];
	for(TQStringList::Iterator inner = gdmf.begin(); inner != gdmf.end(); ++inner)
	    (*inner) = Option::fixPathToTargetOS((*inner), false);
    }

    if ( !project->variables()["DEF_FILE"].isEmpty() )
	project->variables()["QMAKE_LFLAGS"].append(TQString("/DEF:") + project->first("DEF_FILE"));
    if(!project->isActiveConfig("incremental"))
	project->variables()["QMAKE_LFLAGS"].append(TQString("/incremental:no"));

    if ( !project->variables()["VERSION"].isEmpty() ) {
	TQString version = project->variables()["VERSION"][0];
	int firstDot = version.find( "." );
	TQString major = version.left( firstDot );
	TQString minor = version.right( version.length() - firstDot - 1 );
	minor.replace( ".", "" );
	project->variables()["QMAKE_LFLAGS"].append( "/VERSION:" + major + "." + minor );
    }
    if ( !project->variables()["RC_FILE"].isEmpty()) {
	if ( !project->variables()["RES_FILE"].isEmpty()) {
	    fprintf(stderr, "Both .rc and .res file specified.\n");
	    fprintf(stderr, "Please specify one of them, not both.");
	    exit(666);
	}
	project->variables()["RES_FILE"] = project->variables()["RC_FILE"];
	project->variables()["RES_FILE"].first().replace(".rc",".res");
	project->variables()["POST_TARGETDEPS"] += project->variables()["RES_FILE"];
	project->variables()["CLEAN_FILES"] += project->variables()["RES_FILE"];
    }
    if ( !project->variables()["RES_FILE"].isEmpty())
	project->variables()["QMAKE_LIBS"] += project->variables()["RES_FILE"];

    // Base class init!
    MakefileGenerator::init();

    // Setup PCH variables
    precompH = project->first("PRECOMPILED_HEADER");
    usePCH = !precompH.isEmpty() && project->isActiveConfig("precompile_header");
    if (usePCH) {
	// Created files
	precompObj = var("OBJECTS_DIR") + project->first("TARGET") + "_pch" + Option::obj_ext;
	precompPch = var("OBJECTS_DIR") + project->first("TARGET") + "_pch.pch";
	// Add linking of precompObj (required for whole precompiled classes)
	project->variables()["OBJECTS"]		  += precompObj;
	// Add pch file to cleanup
	project->variables()["QMAKE_CLEAN"]	  += precompPch;
	// Return to variable pool
	project->variables()["PRECOMPILED_OBJECT"] = precompObj;
	project->variables()["PRECOMPILED_PCH"]    = precompPch;
    }

    if ( !project->variables()["VERSION"].isEmpty()) {
	TQStringList l = TQStringList::split('.', project->first("VERSION"));
	project->variables()["VER_MAJ"].append(l[0]);
	project->variables()["VER_MIN"].append(l[1]);
    }

    TQString version = TQStringList::split('.', project->first("VERSION")).join("");
    if(project->isActiveConfig("dll")) {
	project->variables()["QMAKE_CLEAN"].append(project->first("DESTDIR") + project->first("TARGET") + version + ".exp");
    }
    if(project->isActiveConfig("debug")) {
	project->variables()["QMAKE_CLEAN"].append(project->first("DESTDIR") + project->first("TARGET") + version + ".pdb");
	project->variables()["QMAKE_CLEAN"].append(project->first("DESTDIR") + project->first("TARGET") + version + ".ilk");
	project->variables()["QMAKE_CLEAN"].append("vc*.pdb");
	project->variables()["QMAKE_CLEAN"].append("vc*.idb");
    }

    TQStringList &quc = project->variables()["QMAKE_EXTRA_WIN_COMPILERS"];
    for(it = quc.begin(); it != quc.end(); ++it) {
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
