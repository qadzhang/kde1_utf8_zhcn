/****************************************************************************
**
** Implementation of ProjectBuilderMakefileGenerator class.
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

#include "pbuilder_pbx.h"
#include "option.h"
#include "meta.h"
#include <ntqdir.h>
#include <ntqdict.h>
#include <ntqregexp.h>
#include <stdlib.h>
#include <time.h>
#include "qtmd5.h"
#ifdef Q_OS_UNIX
#  include <sys/types.h>
#  include <sys/stat.h>
#endif

// Note: this is fairly hacky, but it does the job...

ProjectBuilderMakefileGenerator::ProjectBuilderMakefileGenerator(TQMakeProject *p) : UnixMakefileGenerator(p)
{

}

bool
ProjectBuilderMakefileGenerator::writeMakefile(TQTextStream &t)
{
    if(!project->variables()["QMAKE_FAILED_REQUIREMENTS"].isEmpty()) {
	/* for now just dump, I need to generated an empty xml or something.. */
	fprintf(stderr, "Project file not generated because all requirements not met:\n\t%s\n",
		var("QMAKE_FAILED_REQUIREMENTS").latin1());
	return true;
    }

    project->variables()["MAKEFILE"].clear();
    project->variables()["MAKEFILE"].append("Makefile");
    if(project->first("TEMPLATE") == "app" || project->first("TEMPLATE") == "lib")
	return writeMakeParts(t);
    else if(project->first("TEMPLATE") == "subdirs")
	return writeSubdirs(t, false);
    return false;
}

bool
ProjectBuilderMakefileGenerator::writeSubdirs(TQTextStream &t, bool direct)
{
    TQString mkwrap = fileFixify(pbx_dir + Option::dir_sep + ".." + Option::dir_sep + project->first("MAKEFILE"),
				TQDir::currentDirPath());
    TQFile mkwrapf(mkwrap);
    if(mkwrapf.open(IO_WriteOnly | IO_Translate)) {
	debug_msg(1, "pbuilder: Creating file: %s", mkwrap.latin1());
	TQTextStream mkwrapt(&mkwrapf);
	UnixMakefileGenerator::writeSubdirs(mkwrapt, direct);
    }

    //HEADER
    const int pbVersion = pbuilderVersion();
    t << "// !$*UTF8*$!" << "\n"
      << "{" << "\n"
      << "\t" << "archiveVersion = 1;" << "\n"
      << "\t" << "classes = {" << "\n" << "\t" << "};" << "\n"
      << "\t" << "objectVersion = " << pbVersion << ";" << "\n"
      << "\t" << "objects = {" << endl;

    //SUBDIRS
    TQStringList subdirs = project->variables()["SUBDIRS"];
    TQString oldpwd = TQDir::currentDirPath();
    TQMap<TQString, TQStringList> groups;
    for(TQStringList::Iterator it = subdirs.begin(); it != subdirs.end(); ++it) {
	TQFileInfo fi(Option::fixPathToLocalOS((*it), true));
	if(fi.exists()) {
	    if(fi.isDir()) {
		TQString profile = (*it);
		if(!profile.endsWith(Option::dir_sep))
		    profile += Option::dir_sep;
		profile += fi.baseName() + ".pro";
		subdirs.append(profile);
	    } else {
		TQMakeProject tmp_proj;
		TQString dir = fi.dirPath(), fn = fi.fileName();
		if(!dir.isEmpty()) {
		    if(!TQDir::setCurrent(dir))
			fprintf(stderr, "Cannot find directory: %s\n", dir.latin1());
		}
		if(tmp_proj.read(fn, oldpwd)) {
		    if(Option::debug_level) {
			TQMap<TQString, TQStringList> &vars = tmp_proj.variables();
			for(TQMap<TQString, TQStringList>::Iterator it = vars.begin();
			    it != vars.end(); ++it) {
			    if(it.key().left(1) != "." && !it.data().isEmpty())
				debug_msg(1, "%s: %s === %s", fn.latin1(), it.key().latin1(),
					  it.data().join(" :: ").latin1());
			}
		    }
		    if(tmp_proj.first("TEMPLATE") == "subdirs") {
			subdirs += fileFixify(tmp_proj.variables()["SUBDIRS"]);
		    } else if(tmp_proj.first("TEMPLATE") == "app" || tmp_proj.first("TEMPLATE") == "lib") {
			TQString pbxproj = TQDir::currentDirPath() + Option::dir_sep + tmp_proj.first("TARGET") + projectSuffix();
			if(!TQFile::exists(pbxproj)) {
			    warn_msg(WarnLogic, "Ignored (not found) '%s'", pbxproj.latin1());
			    goto nextfile; // # Dirty!
			}
			project->variables()["QMAKE_PBX_SUBDIRS"] += pbxproj;
			//PROJECTREF
			{
			    bool in_root = true;
			    TQString name = TQDir::currentDirPath();
			    TQString project_key = keyFor(pbxproj + "_PROJECTREF");
			    if(project->isActiveConfig("flat")) {
				TQString flat_file = fileFixify(name, oldpwd, Option::output_dir, true);
				if(flat_file.find(Option::dir_sep) != -1) {
				    TQStringList dirs = TQStringList::split(Option::dir_sep, flat_file);
				    name = dirs.back();
				}
			    } else {
				TQString flat_file = fileFixify(name, oldpwd, Option::output_dir, true);
				if(TQDir::isRelativePath(flat_file) && flat_file.find(Option::dir_sep) != -1) {
				    TQString last_grp("QMAKE_PBX_HEIR_GROUP");
				    TQStringList dirs = TQStringList::split(Option::dir_sep, flat_file);
				    name = dirs.back();
				    for(TQStringList::Iterator dir_it = dirs.begin(); dir_it != dirs.end(); ++dir_it) {
					TQString new_grp(last_grp + Option::dir_sep + (*dir_it)), new_grp_key(keyFor(new_grp));
					if(dir_it == dirs.begin()) {
					    if(!groups.contains(new_grp))
						project->variables()["QMAKE_PBX_GROUPS"].append(new_grp_key);
					} else {
					    if(!groups[last_grp].contains(new_grp_key))
						groups[last_grp] += new_grp_key;
					}
					last_grp = new_grp;
				    }
				    groups[last_grp] += project_key;
				    in_root = false;
				}
			    }
			    if(in_root)
				project->variables()["QMAKE_PBX_GROUPS"] += project_key;
			    t << "\t\t" << project_key << " = {" << "\n"
			      << "\t\t\t" << "isa = PBXFileReference;" << "\n"
			      << "\t\t\t" << "name = " << tmp_proj.first("TARGET") << ";" << "\n"
			      << "\t\t\t" << "path = " << pbxproj << ";" << "\n"
			      << "\t\t\t" << "refType = 0;" << "\n"
			      << "\t\t\t" << "sourceTree = \"<absolute>\";" << "\n"
			      << "\t\t" << "};" << "\n";
			    //PRODUCTGROUP
			    t << "\t\t" << keyFor(pbxproj + "_PRODUCTGROUP") << " = {" << "\n"
			      << "\t\t\t" << "children = (" << "\n"
			      << "\t\t\t" << ");" << "\n"
			      << "\t\t\t" << "isa = PBXGroup;" << "\n"
			      << "\t\t\t" << "name = Products;" << "\n"
			      << "\t\t\t" << "refType = 4;" << "\n"
			      << "\t\t\t" << "sourceTree = \"<group>\";" << "\n"
			      << "\t\t" << "};" << "\n";
			}
		    }
		}
nextfile:
		TQDir::setCurrent(oldpwd);
	    }
	}
    }
    for(TQMap<TQString, TQStringList>::Iterator grp_it = groups.begin(); grp_it != groups.end(); ++grp_it) {
	t << "\t\t" << keyFor(grp_it.key()) << " = {" << "\n"
	  << "\t\t\t" << "isa = PBXGroup;" << "\n"
	  << "\t\t\t" << "children = (" << "\n"
	  << valGlue(grp_it.data(), "\t\t\t\t", ",\n\t\t\t\t", "\n")
	  << "\t\t\t" << ");" << "\n"
	  << "\t\t\t" << "name = \"" << grp_it.key().section(Option::dir_sep, -1) << "\";" << "\n"
	  << "\t\t\t" << "refType = 4;" << "\n"
	  << "\t\t" << "};" << "\n";
    }

    //DUMP EVERYTHING THAT TIES THE ABOVE TOGETHER
    //BUILDSTYLE
    TQString active_buildstyle;
#if 0
    for(int as_release = 0; as_release < 2; as_release++)
#else
	bool as_release = !project->isActiveConfig("debug");
#endif
    {
        TQMap<TQString, TQString> settings;
        settings.insert("COPY_PHASE_STRIP", (as_release ? "YES" : "NO"));
        if(as_release)
            settings.insert("GCC_GENERATE_DEBUGGING_SYMBOLS", "NO");
        TQString name;
        if(pbVersion >= 42)
            name = (as_release ? "Release" : "Debug");
        else
            name = (as_release ? "Deployment" : "Development");

        if(pbVersion >= 42) {
            TQString key = keyFor("QMAKE_SUBDIR_PBX_BUILDCONFIG_" + name);
            project->variables()["QMAKE_SUBDIR_PBX_BUILDCONFIGS"].append(key);
            t << "\t\t" << key << " = {" << "\n"
              << "\t\t\t" << "isa = XCBuildConfiguration;" << "\n"
              << "\t\t\t" << "buildSettings = {" << "\n";
            for(TQMap<TQString, TQString>::Iterator set_it = settings.begin(); set_it != settings.end(); ++set_it)
                t << "\t\t\t\t" << set_it.key() << " = \"" << set_it.data() << "\";\n";
            t << "\t\t\t" << "};" << "\n"
              << "\t\t\t" << "name = " << name << ";" << "\n"
              << "\t\t" << "};" << "\n";
        }

        TQString key = keyFor("QMAKE_SUBDIR_PBX_BUILDSTYLE_" + name);
        if(project->isActiveConfig("debug") != (bool)as_release) {
            project->variables()["QMAKE_SUBDIR_PBX_BUILDSTYLES"].append(key);
            active_buildstyle = name;
        } else if(pbVersion >= 42) {
            project->variables()["QMAKE_SUBDIR_PBX_BUILDSTYLES"].append(key);
        }
        t << "\t\t" << key << " = {" << "\n"
          << "\t\t\t" << "buildRules = (" << "\n"
          << "\t\t\t" << ");" << "\n"
          << "\t\t\t" << "buildSettings = {" << "\n";
        for(TQMap<TQString, TQString>::Iterator set_it = settings.begin(); set_it != settings.end(); ++set_it)
            t << "\t\t\t\t" << set_it.key() << " = \"" << set_it.data() << "\";\n";
        t << "\t\t\t" << "};" << "\n"
          << "\t\t\t" << "isa = PBXBuildStyle;" << "\n"
          << "\t\t\t" << "name = " << name << ";" << "\n"
          << "\t\t" << "};" << "\n";
    }
    if(pbVersion >= 42) {
        t << "\t\t" << keyFor("QMAKE_SUBDIR_PBX_BUILDCONFIG_LIST") << " = {" << "\n"
          << "\t\t\t" << "isa = XCConfigurationList;" << "\n"
          << "\t\t\t" << "buildConfigurations = (" << "\n"
          << varGlue("QMAKE_SUBDIR_PBX_BUILDCONFIGS", "\t\t\t\t", ",\n\t\t\t\t", "\n")
          << "\t\t\t" << ");" << "\n"
          << "\t\t\t" << "defaultConfigurationIsVisible = 0;" << "\n"
          << "\t\t\t" << "defaultConfigurationIsName = " << active_buildstyle << ";" << "\n"
          << "\t\t" << "};" << "\n";
    }

#ifdef GENERATE_AGGREGRATE_SUBDIR
    //target
    t << "\t\t" << keyFor("QMAKE_SUBDIR_PBX_AGGREGATE_TARGET") << " = {" << "\n"
      << "\t\t\t" << "buidPhases = (" << "\n"
      << "\t\t\t" << ");" << "\n"
      << "\t\t\t" << "buildSettings = {" << "\n"
      << "\t\t\t\t" << "PRODUCT_NAME = " << project->variables()["TARGET"].first() << ";" << "\n"
      << "\t\t\t" << "};" << "\n"
      << "\t\t\t" << "dependencies = (" << "\n";
    {
        const TQStringList &qmake_subdirs = project->variables()["QMAKE_PBX_SUBDIRS"];
        for(int i = 0; i < qmake_subdirs.count(); i++)
            t << "\t\t\t\t" << keyFor(qmake_subdirs[i] + "_TARGETREF") << "," << "\n";
    }
    t << "\t\t\t" << ");" << "\n"
      << "\t\t\t" << "isa = PBXAggregateTarget;" << "\n"
      << "\t\t\t" << "name = " << project->variables()["TARGET"].first() << ";" << "\n"
      << "\t\t\t" << "productName = " << project->variables()["TARGET"].first() << ";" << "\n"
      << "\t\t" << "};" << "\n";
#endif

    //ROOT_GROUP
    t << "\t\t" << keyFor("QMAKE_PBX_ROOT_GROUP") << " = {" << "\n"
      << "\t\t\t" << "children = (" << "\n"
      << varGlue("QMAKE_PBX_GROUPS", "\t\t\t\t", ",\n\t\t\t\t", "\n")
      << "\t\t\t" << ");" << "\n"
      << "\t\t\t" << "isa = PBXGroup;" << "\n"
      << "\t\t\t" << "refType = 4;" << "\n"
      << "\t\t\t" << "sourceTree = \"<group>\";" << "\n"
      << "\t\t" << "};" << "\n";

    //ROOT
    t << "\t\t" << keyFor("QMAKE_PBX_ROOT") << " = {" << "\n"
      << "\t\t\t" << "buildSettings = {" << "\n"
      << "\t\t\t" << "};" << "\n"
      << "\t\t\t" << "buildStyles = (" << "\n"
      << varGlue("QMAKE_PBX_BUILDSTYLES", "\t\t\t\t", ",\n\t\t\t\t", "\n")
      << "\t\t\t" << ");" << "\n"
      << "\t\t\t" << "isa = PBXProject;" << "\n"
      << "\t\t\t" << "mainGroup = " << keyFor("QMAKE_PBX_ROOT_GROUP") << ";" << "\n"
      << "\t\t\t" << "projectDirPath = \"\";" << "\n";
    if(pbVersion >= 42)
        t << "\t\t\t" << "buildConfigurationList = " << keyFor("QMAKE_SUBDIR_PBX_BUILDCONFIG_LIST") << ";" << "\n";
    t << "\t\t\t" << "projectReferences = (" << "\n";
    {
	TQStringList &libdirs = project->variables()["QMAKE_PBX_SUBDIRS"];
	for(TQStringList::Iterator it = libdirs.begin(); it != libdirs.end(); ++it)
	    t << "\t\t\t\t" << "{" << "\n"
	      << "\t\t\t\t\t" << "ProductGroup = " << keyFor((*it) + "_PRODUCTGROUP") << ";" << "\n"
	      << "\t\t\t\t\t" << "ProjectRef = " << keyFor((*it) + "_PROJECTREF") << ";" << "\n"
	      << "\t\t\t\t" << "}," << "\n";
    }
    t << "\t\t\t" << ");" << "\n"
      << "\t\t\t" << "targets = (" << "\n"
      << "\t\t\t" << ");" << "\n"
      << "\t\t" << "};" << "\n";

    //FOOTER
    t << "\t" << "};" << "\n"
      << "\t" << "rootObject = " << keyFor("QMAKE_PBX_ROOT") << ";" << "\n"
      << "}" << endl;

    return true;
}

bool
ProjectBuilderMakefileGenerator::writeMakeParts(TQTextStream &t)
{
    int i;
    TQStringList tmp;
    bool did_preprocess = false;

    //HEADER
    const int pbVersion = pbuilderVersion();
    t << "// !$*UTF8*$!" << "\n"
      << "{" << "\n"
      << "\t" << "archiveVersion = 1;" << "\n"
      << "\t" << "classes = {" << "\n" << "\t" << "};" << "\n"
      << "\t" << "objectVersion = " << pbVersion << ";" << "\n"
      << "\t" << "objects = {" << endl;

    //MAKE QMAKE equivelant
    if(!project->isActiveConfig("no_autoqmake") && project->projectFile() != "(stdin)") {
	TQString mkfile = pbx_dir + Option::dir_sep + "qt_makeqmake.mak";
	TQFile mkf(mkfile);
	if(mkf.open(IO_WriteOnly | IO_Translate)) {
	    debug_msg(1, "pbuilder: Creating file: %s", mkfile.latin1());
	    TQTextStream mkt(&mkf);
	    writeHeader(mkt);
	    mkt << "QMAKE    = "	<<
		(project->isEmpty("QMAKE_QMAKE") ? TQString("$(TQTDIR)/bin/tqmake") :
		 var("QMAKE_QMAKE")) << endl;
	    writeMakeQmake(mkt);
	    mkf.close();
	}
	TQString phase_key = keyFor("QMAKE_PBX_MAKEQMAKE_BUILDPHASE");
	mkfile = fileFixify(mkfile, TQDir::currentDirPath());
	project->variables()["QMAKE_PBX_PRESCRIPT_BUILDPHASES"].append(phase_key);
	t << "\t\t" << phase_key << " = {" << "\n"
	  << "\t\t\t" << "buildActionMask = 2147483647;" << "\n"
	  << "\t\t\t" << "files = (" << "\n"
	  << "\t\t\t" << ");" << "\n"
	  << "\t\t\t" << "generatedFileNames = (" << "\n"
	  << "\t\t\t" << ");" << "\n"
	  << "\t\t\t" << "isa = PBXShellScriptBuildPhase;" << "\n"
	  << "\t\t\t" << "name = \"TQt Qmake\";" << "\n"
	  << "\t\t\t" << "neededFileNames = (" << "\n"
	  << "\t\t\t" << ");" << "\n"
	  << "\t\t\t" << "shellPath = /bin/sh;" << "\n"
	  << "\t\t\t" << "shellScript = \"make -C " << TQDir::currentDirPath() <<
	    " -f " << mkfile << "\";" << "\n"
	  << "\t\t" << "};" << "\n";
    }

    //DUMP SOURCES
    TQMap<TQString, TQStringList> groups;
    TQString srcs[] = { "HEADERS", "SOURCES", "SRCMOC", "UICIMPLS", "QMAKE_IMAGE_COLLECTION",
		       "FORMS", "QMAKE_INTERNAL_INCLUDED_FILES", TQString::null };
    for(i = 0; !srcs[i].isNull(); i++) {
	tmp = project->variables()[srcs[i]];
	if(srcs[i] == "QMAKE_INTERNAL_INCLUDED_FILES") {
	    TQString pfile = project->projectFile();
	    if(pfile != "(stdin)")
		tmp.prepend(pfile);
	}
	TQStringList &src_list = project->variables()["QMAKE_PBX_" + srcs[i]];
	TQStringList &root_group_list = project->variables()["QMAKE_PBX_GROUPS"];

	//hard coded groups..
	TQString src_group;
	if(srcs[i] == "SOURCES")
	    src_group = "Sources";
	else if(srcs[i] == "HEADERS")
	    src_group = "Headers";
	else if(srcs[i] == "SRCMOC")
	    src_group = "Sources [moc]";
	else if(srcs[i] == "UICIMPLS" || srcs[i] == "FORMS")
	    src_group = "Sources [uic]";
	else if(srcs[i] == "QMAKE_IMAGE_COLLECTION")
	    src_group = "Sources [images]";
	else if(srcs[i] == "QMAKE_INTERNAL_INCLUDED_FILES")
	    src_group = "Sources [qmake]";

	for(TQStringList::Iterator it = tmp.begin(); it != tmp.end(); ++it) {
	    TQStringList files = (*it);
	    bool buildable = true;
	    if(srcs[i] == "FORMS") {
		TQString form_dot_h = (*it) + Option::h_ext.first();
		if(TQFile::exists(form_dot_h))
		    files += form_dot_h;
		buildable = false;
	    } else if(srcs[i] == "HEADERS" || srcs[i] == "QMAKE_INTERNAL_INCLUDED_FILES") {
		buildable = false;
	    }

	    files = fileFixify(files);
	    for(TQStringList::Iterator file_it = files.begin(); file_it != files.end(); ++file_it) {
		TQString file = (*file_it);
		if(file.length() >= 2 && (file[0] == '"' || file[0] == '\'') && file[(int) file.length()-1] == file[0])
		    file = file.mid(1, file.length()-2);
		if(file.endsWith(Option::cpp_moc_ext) || file.endsWith(Option::prl_ext))
		    continue;
		bool in_root = true;
		TQString src_key = keyFor(file), name = file;
		if(project->isActiveConfig("flat")) {
		    TQString flat_file = fileFixify(file, TQDir::currentDirPath(), Option::output_dir, true);
		    if(flat_file.find(Option::dir_sep) != -1) {
			TQStringList dirs = TQStringList::split(Option::dir_sep, flat_file);
			name = dirs.back();
		    }
		} else {
		    TQString flat_file = fileFixify(file, TQDir::currentDirPath(), Option::output_dir, true);
		    if(TQDir::isRelativePath(flat_file) && flat_file.find(Option::dir_sep) != -1) {
			TQString last_grp("QMAKE_PBX_" + src_group + "_HEIR_GROUP");
			TQStringList dirs = TQStringList::split(Option::dir_sep, flat_file);
			name = dirs.back();
			dirs.pop_back(); //remove the file portion as it will be added via src_key
			for(TQStringList::Iterator dir_it = dirs.begin(); dir_it != dirs.end(); ++dir_it) {
			    TQString new_grp(last_grp + Option::dir_sep + (*dir_it)), new_grp_key(keyFor(new_grp));
			    if(dir_it == dirs.begin()) {
				if(!src_list.contains(new_grp_key))
				    src_list.append(new_grp_key);
			    } else {
				if(!groups[last_grp].contains(new_grp_key))
				    groups[last_grp] += new_grp_key;
			    }
			    last_grp = new_grp;
			}
			groups[last_grp] += src_key;
			in_root = false;
		    }
		}
		if(in_root)
		    src_list.append(src_key);
		//source reference
		t << "\t\t" << src_key << " = {" << "\n"
		  << "\t\t\t" << "isa = PBXFileReference;" << "\n"
		  << "\t\t\t" << "name = \"" << name << "\";" << "\n"
		  << "\t\t\t" << "path = \"" << file << "\";" << "\n"
		  << "\t\t\t" << "refType = " << reftypeForFile(file) << ";" << "\n";
                if(pbVersion >= 38) {
		    TQString filetype;
		    for(TQStringList::Iterator cppit = Option::cpp_ext.begin(); cppit != Option::cpp_ext.end(); ++cppit) {
			if(file.endsWith((*cppit))) {
			    filetype = "sourcecode.cpp.cpp";
			    break;
			}
		    }
		    if(!filetype.isNull())
			t << "\t\t\t" << "lastKnownFileType = " << filetype << ";" << "\n";
		}
		t << "\t\t" << "};" << "\n";
		if(buildable) { //build reference
		    TQString obj_key = file + ".o";
		    obj_key = keyFor(obj_key);
		    t << "\t\t" << obj_key << " = {" << "\n"
		      << "\t\t\t" << "fileRef = " << src_key << ";" << "\n"
		      << "\t\t\t" << "isa = PBXBuildFile;" << "\n"
		      << "\t\t\t" << "settings = {" << "\n"
		      << "\t\t\t\t" << "ATTRIBUTES = (" << "\n"
		      << "\t\t\t\t" << ");" << "\n"
		      << "\t\t\t" << "};" << "\n"
		      << "\t\t" << "};" << "\n";
		    project->variables()["QMAKE_PBX_OBJ"].append(obj_key);
		}
	    }
	}
	if(!src_list.isEmpty()) {
	    if(srcs[i] == "SOURCES") {
		if(project->first("TEMPLATE") == "app" && !project->isEmpty("RC_FILE")) { //Icon
		    TQString icns_file = keyFor("ICNS_FILE");
		    src_list.append(icns_file);
		    t << "\t\t" << icns_file << " = {" << "\n"
		      << "\t\t\t" << "isa = PBXFileReference;" << "\n"
		      << "\t\t\t" << "path = \"" << project->first("RC_FILE") << "\";" << "\n"
		      << "\t\t\t" << "refType = " << reftypeForFile(project->first("RC_FILE")) << ";" << "\n"
		      << "\t\t" << "};" << "\n";
		    t << "\t\t" << keyFor("ICNS_FILE_REFERENCE") << " = {" << "\n"
		      << "\t\t\t" << "fileRef = " << icns_file << ";" << "\n"
		      << "\t\t\t" << "isa = PBXBuildFile;" << "\n"
		      << "\t\t\t" << "settings = {" << "\n"
		      << "\t\t\t" << "};" << "\n"
		      << "\t\t" << "};" << "\n";
		}
	    }

	    TQString src_group_key = keyFor(src_group);
	    if(root_group_list.findIndex(src_group_key) == -1)
		root_group_list += src_group_key;

            TQStringList &group = groups[src_group];
            for(TQStringList::Iterator src_it = src_list.begin(); src_it != src_list.end(); ++src_it) {
                if(group.findIndex((*src_it)) == -1)
                    group += (*src_it);
            }
	}
    }
    for(TQMap<TQString, TQStringList>::Iterator grp_it = groups.begin(); grp_it != groups.end(); ++grp_it) {
	t << "\t\t" << keyFor(grp_it.key()) << " = {" << "\n"
	  << "\t\t\t" << "isa = PBXGroup;" << "\n"
	  << "\t\t\t" << "children = (" << "\n"
	  << valGlue(grp_it.data(), "\t\t\t\t", ",\n\t\t\t\t", "\n")
	  << "\t\t\t" << ");" << "\n"
	  << "\t\t\t" << "name = \"" << grp_it.key().section(Option::dir_sep, -1) << "\";" << "\n"
	  << "\t\t\t" << "refType = 4;" << "\n"
	  << "\t\t" << "};" << "\n";
    }

    //PREPROCESS BUILDPHASE (just a makefile)
    if(!project->isEmpty("UICIMPLS") || !project->isEmpty("SRCMOC") || !project->isEmpty("IMAGES") ||
	!project->isEmpty("YACCSOURCES") || !project->isEmpty("LEXSOURCES")) {
	TQString mkfile = pbx_dir + Option::dir_sep + "qt_preprocess.mak";
	TQFile mkf(mkfile);
	if(mkf.open(IO_WriteOnly | IO_Translate)) {
	    did_preprocess = true;
	    debug_msg(1, "pbuilder: Creating file: %s", mkfile.latin1());
	    TQTextStream mkt(&mkf);
	    writeHeader(mkt);
	    mkt << "MOC       = " << Option::fixPathToTargetOS(var("QMAKE_MOC")) << endl;
	    mkt << "UIC       = " << Option::fixPathToTargetOS(var("QMAKE_UIC")) << endl;
	    mkt << "LEX       = " << var("QMAKE_LEX") << endl;
	    mkt << "LEXFLAGS  = " << var("QMAKE_LEXFLAGS") << endl;
	    mkt << "YACC      = " << var("QMAKE_YACC") << endl;
	    mkt << "YACCFLAGS = " << var("QMAKE_YACCFLAGS") << endl;
	    mkt << "DEL_FILE  = " << var("QMAKE_DEL_FILE") << endl;
	    mkt << "MOVE      = " << var("QMAKE_MOVE") << endl << endl;
	    mkt << "FORMS = " << varList("UICIMPLS") << endl;
	    mkt << "IMAGES = " << varList("QMAKE_IMAGE_COLLECTION") << endl;
	    mkt << "MOCS = " << varList("SRCMOC") << endl;
	    mkt << "PARSERS =";
	    if(!project->isEmpty("YACCSOURCES")) {
		TQStringList &yaccs = project->variables()["YACCSOURCES"];
		for(TQStringList::Iterator yit = yaccs.begin(); yit != yaccs.end(); ++yit) {
		    TQFileInfo fi((*yit));
		    mkt << " " << fi.dirPath() << Option::dir_sep << fi.baseName(true)
			<< Option::yacc_mod << Option::cpp_ext.first();
		}
	    }
	    if(!project->isEmpty("LEXSOURCES")) {
		TQStringList &lexs = project->variables()["LEXSOURCES"];
		for(TQStringList::Iterator lit = lexs.begin(); lit != lexs.end(); ++lit) {
		    TQFileInfo fi((*lit));
		    mkt << " " << fi.dirPath() << Option::dir_sep << fi.baseName(true)
			<< Option::lex_mod << Option::cpp_ext.first();
		}
	    }
	    mkt << "\n";
	    mkt << "preprocess: $(FORMS) $(MOCS) $(PARSERS) $(IMAGES)" << endl;
	    mkt << "clean preprocess_clean: mocclean uiclean parser_clean" << endl << endl;
	    mkt << "mocclean:" << "\n";
	    if(!project->isEmpty("SRCMOC"))
		mkt << "\t-rm -f $(MOCS)" << "\n";
	    mkt << "uiclean:" << "\n";
	    if(!project->isEmpty("UICIMPLS"))
		mkt << "\t-rm -f $(FORMS)" << "\n";
	    if(!project->isEmpty("QMAKE_IMAGE_COLLECTION"))
		mkt << "\t-rm -f $(IMAGES)" << "\n";
	    mkt << "parser_clean:" << "\n";
	    if(!project->isEmpty("YACCSOURCES") || !project->isEmpty("LEXSOURCES"))
		mkt << "\t-rm -f $(PARSERS)" << "\n";
	    writeUicSrc(mkt, "FORMS");
	    writeMocSrc(mkt, "HEADERS");
	    writeMocSrc(mkt, "SOURCES");
	    writeMocSrc(mkt, "UICDECLS");
	    writeYaccSrc(mkt, "YACCSOURCES");
	    writeLexSrc(mkt, "LEXSOURCES");
	    writeImageSrc(mkt, "QMAKE_IMAGE_COLLECTION");
	    mkf.close();
	}
	mkfile = fileFixify(mkfile, TQDir::currentDirPath());
	TQString phase_key = keyFor("QMAKE_PBX_PREPROCESS_TARGET");
//	project->variables()["QMAKE_PBX_BUILDPHASES"].append(phase_key);
	project->variables()["QMAKE_PBX_PRESCRIPT_BUILDPHASES"].append(phase_key);
	t << "\t\t" << phase_key << " = {" << "\n"
	  << "\t\t\t" << "buildActionMask = 2147483647;" << "\n"
	  << "\t\t\t" << "files = (" << "\n"
	  << "\t\t\t" << ");" << "\n"
	  << "\t\t\t" << "generatedFileNames = (" << "\n"
	  << varGlue("QMAKE_PBX_OBJ", "\t\t\t\t", ",\n\t\t\t\t", "\n")
	  << "\t\t\t" << ");" << "\n"
	  << "\t\t\t" << "isa = PBXShellScriptBuildPhase;" << "\n"
	  << "\t\t\t" << "name = \"TQt Preprocessors\";" << "\n"
	  << "\t\t\t" << "neededFileNames = (" << "\n"
	  << varGlue("QMAKE_PBX_OBJ", "\t\t\t\t", ",\n\t\t\t\t", "\n")
	  << "\t\t\t" << ");" << "\n"
	  << "\t\t\t" << "shellPath = /bin/sh;" << "\n"
	  << "\t\t\t" << "shellScript = \"make -C " << TQDir::currentDirPath() <<
	    " -f " << mkfile << "\";" << "\n"
	  << "\t\t" << "};" << "\n";
   }

    //SOURCE BUILDPHASE
    if(!project->isEmpty("QMAKE_PBX_OBJ")) {
	TQString grp = "Build Sources", key = keyFor(grp);
	project->variables()["QMAKE_PBX_BUILDPHASES"].append(key);
	t << "\t\t" << key << " = {" << "\n"
	  << "\t\t\t" << "buildActionMask = 2147483647;" << "\n"
	  << "\t\t\t" << "files = (" << "\n"
	  << varGlue("QMAKE_PBX_OBJ", "\t\t\t\t", ",\n\t\t\t\t", "\n")
	  << "\t\t\t" << ");" << "\n"
	  << "\t\t\t" << "isa = PBXSourcesBuildPhase;" << "\n"
	  << "\t\t\t" << "name = \"" << grp << "\";" << "\n"
	  << "\t\t" << "};" << "\n";
    }

    if(!project->isActiveConfig("staticlib")) { //DUMP LIBRARIES
	TQStringList &libdirs = project->variables()["QMAKE_PBX_LIBPATHS"];
	TQString libs[] = { "QMAKE_LFLAGS", "QMAKE_LIBDIR_FLAGS", "QMAKE_LIBS", TQString::null };
	for(i = 0; !libs[i].isNull(); i++) {
	    tmp = project->variables()[libs[i]];
	    for(TQStringList::Iterator it = tmp.begin(); it != tmp.end();) {
		bool remove = false;
		TQString library, name, opt = (*it).stripWhiteSpace();
		if(opt.length() >= 2 && (opt[0] == '"' || opt[0] == '\'') && opt[(int) opt.length()-1] == opt[0])
		    opt = opt.mid(1, opt.length()-2);
		if(opt.startsWith("-L")) {
		    TQString r = opt.right(opt.length() - 2);
		    fixEnvVariables(r);
		    libdirs.append(r);
		} else if(opt == "-prebind") {
		    project->variables()["QMAKE_DO_PREBINDING"].append("TRUE");
		    remove = true;
		} else if(opt.startsWith("-l")) {
		    name = opt.right(opt.length() - 2);
		    TQString lib("lib" + name);
		    for(TQStringList::Iterator lit = libdirs.begin(); lit != libdirs.end(); ++lit) {
			if(project->isActiveConfig("link_prl")) {
			    /* This isn't real nice, but it is real usefull. This looks in a prl
			       for what the library will ultimately be called so we can stick it
			       in the ProjectFile. If the prl format ever changes (not likely) then
			       this will not really work. However, more concerning is that it will
			       encode the version number in the Project file which might be a bad
			       things in days to come? --Sam
			    */
			    TQString lib_file = (*lit) + Option::dir_sep + lib;
			    if(TQMakeMetaInfo::libExists(lib_file)) {
				TQMakeMetaInfo libinfo;
				if(libinfo.readLib(lib_file)) {
				    if(!libinfo.isEmpty("QMAKE_PRL_TARGET")) {
					library = (*lit) + Option::dir_sep + libinfo.first("QMAKE_PRL_TARGET");
					debug_msg(1, "pbuilder: Found library (%s) via PRL %s (%s)",
						  opt.latin1(), lib_file.latin1(), library.latin1());
					remove = true;
				    }
				}
			    }
			}
			if(!remove) {
			    TQString extns[] = { ".dylib", ".so", ".a", TQString::null };
			    for(int n = 0; !remove && !extns[n].isNull(); n++) {
				TQString tmp =  (*lit) + Option::dir_sep + lib + extns[n];
				if(TQFile::exists(tmp)) {
				    library = tmp;
				    debug_msg(1, "pbuilder: Found library (%s) via %s",
					      opt.latin1(), library.latin1());
				    remove = true;
				}
			    }
			}
		    }
		} else if(opt == "-framework") {
		    ++it;
		    if(it == tmp.end())
			break;
		    TQStringList &fdirs = project->variables()["QMAKE_FRAMEWORKDIR"];
		    if(fdirs.isEmpty())
			fdirs.append("/System/Library/Frameworks/");
		    for(TQStringList::Iterator fit = fdirs.begin(); fit != fdirs.end(); ++fit) {
			if(TQFile::exists((*fit) + TQDir::separator() + (*it) + ".framework")) {
			    --it;
			    it = tmp.remove(it);
			    remove = true;
			    library = (*fit) + Option::dir_sep + (*it) + ".framework";
			    break;
			}
		    }
		} else if(opt.left(1) != "-") {
                    if(TQFile::exists(opt)) {
                        remove = true;
                        library = opt;
                    }
		}
		if(!library.isEmpty()) {
		    if(name.isEmpty()) {
			int slsh = library.findRev(Option::dir_sep);
			if(slsh != -1)
			    name = library.right(library.length() - slsh - 1);
		    }
		    library = fileFixify(library);
		    TQString key = keyFor(library);
		    bool is_frmwrk = (library.endsWith(".framework"));
		    t << "\t\t" << key << " = {" << "\n"
		      << "\t\t\t" << "isa = " << (is_frmwrk ? "PBXFrameworkReference" : "PBXFileReference") << ";" << "\n"
		      << "\t\t\t" << "name = \"" << name << "\";" << "\n"
		      << "\t\t\t" << "path = \"" << library << "\";" << "\n"
		      << "\t\t\t" << "refType = " << reftypeForFile(library) << ";" << "\n"
		      << "\t\t" << "};" << "\n";
		    project->variables()["QMAKE_PBX_LIBRARIES"].append(key);
		    TQString obj_key = library + ".o";
		    obj_key = keyFor(obj_key);
		    t << "\t\t" << obj_key << " = {" << "\n"
		      << "\t\t\t" << "fileRef = " << key << ";" << "\n"
		      << "\t\t\t" << "isa = PBXBuildFile;" << "\n"
		      << "\t\t\t" << "settings = {" << "\n"
		      << "\t\t\t" << "};" << "\n"
		      << "\t\t" << "};" << "\n";
		    project->variables()["QMAKE_PBX_BUILD_LIBRARIES"].append(obj_key);
		}
		if(remove)
		    it = tmp.remove(it);
		else
		    ++it;
	    }
	    project->variables()[libs[i]] = tmp;
	}
    }
    //SUBLIBS BUILDPHASE (just another makefile)
    if(!project->isEmpty("SUBLIBS")) {
	TQString mkfile = pbx_dir + Option::dir_sep + "qt_sublibs.mak";
	TQFile mkf(mkfile);
	if(mkf.open(IO_WriteOnly | IO_Translate)) {
	    debug_msg(1, "pbuilder: Creating file: %s", mkfile.latin1());
	    TQTextStream mkt(&mkf);
	    writeHeader(mkt);
	    mkt << "SUBLIBS= ";
	    tmp = project->variables()["SUBLIBS"];
	    TQStringList::Iterator it;
	    for(it = tmp.begin(); it != tmp.end(); ++it)
		t << "tmp/lib" << (*it) << ".a ";
	    t << endl << endl;
	    mkt << "sublibs: $(SUBLIBS)" << endl << endl;
	    tmp = project->variables()["SUBLIBS"];
	    for(it = tmp.begin(); it != tmp.end(); ++it)
		t << "tmp/lib" << (*it) << ".a" << ":\n\t"
		  << var(TQString("MAKELIB") + (*it)) << endl << endl;
	    mkf.close();
	}
	TQString phase_key = keyFor("QMAKE_PBX_SUBLIBS_BUILDPHASE");
	mkfile = fileFixify(mkfile, TQDir::currentDirPath());
	project->variables()["QMAKE_PBX_PRESCRIPT_BUILDPHASES"].append(phase_key);
	t << "\t\t" << phase_key << " = {" << "\n"
	  << "\t\t\t" << "buildActionMask = 2147483647;" << "\n"
	  << "\t\t\t" << "files = (" << "\n"
	  << "\t\t\t" << ");" << "\n"
	  << "\t\t\t" << "generatedFileNames = (" << "\n"
	  << "\t\t\t" << ");" << "\n"
	  << "\t\t\t" << "isa = PBXShellScriptBuildPhase;" << "\n"
	  << "\t\t\t" << "name = \"TQt Sublibs\";" << "\n"
	  << "\t\t\t" << "neededFileNames = (" << "\n"
	  << "\t\t\t" << ");" << "\n"
	  << "\t\t\t" << "shellPath = /bin/sh;" << "\n"
	  << "\t\t\t" << "shellScript = \"make -C " << TQDir::currentDirPath() <<
	    " -f " << mkfile << "\";" << "\n"
	  << "\t\t" << "};" << "\n";
    }
    //LIBRARY BUILDPHASE
    if(!project->isEmpty("QMAKE_PBX_LIBRARIES")) {
	tmp = project->variables()["QMAKE_PBX_LIBRARIES"];
	if(!tmp.isEmpty()) {
	    TQString grp("External Frameworks and Libraries"), key = keyFor(grp);
	    project->variables()["QMAKE_PBX_GROUPS"].append(key);
	    t << "\t\t" << key << " = {" << "\n"
	      << "\t\t\t" << "children = (" << "\n"
	      << varGlue("QMAKE_PBX_LIBRARIES", "\t\t\t\t", ",\n\t\t\t\t", "\n")
	      << "\t\t\t" << ");" << "\n"
	      << "\t\t\t" << "isa = PBXGroup;" << "\n"
	      << "\t\t\t" << "name = \"" << grp << "\"" << ";" << "\n"
	      << "\t\t\t" << "path = \"\";" << "\n"
	      << "\t\t\t" << "refType = 4;" << "\n"
	      << "\t\t" << "};" << "\n";
	}
    }
    {
	TQString grp("Frameworks & Libraries"), key = keyFor(grp);
	project->variables()["QMAKE_PBX_BUILDPHASES"].append(key);
	t << "\t\t" << key << " = {" << "\n"
	  << "\t\t\t" << "buildActionMask = 2147483647;" << "\n"
	  << "\t\t\t" << "files = (" << "\n"
	  << varGlue("QMAKE_PBX_BUILD_LIBRARIES", "\t\t\t\t", ",\n\t\t\t\t", "\n")
	  << "\t\t\t" << ");" << "\n"
	  << "\t\t\t" << "isa = PBXFrameworksBuildPhase;" << "\n"
	  << "\t\t\t" << "name = \"" << grp << "\";" << "\n"
	  << "\t\t" << "};" << "\n";
    }
    if(!project->isActiveConfig("console") && project->first("TEMPLATE") == "app") { //BUNDLE RESOURCES
	TQString grp("Bundle Resources"), key = keyFor(grp);
	project->variables()["QMAKE_PBX_BUILDPHASES"].append(key);
	t << "\t\t" << key << " = {" << "\n"
	  << "\t\t\t" << "buildActionMask = 2147483647;" << "\n"
	  << "\t\t\t" << "files = (" << "\n"
	  << (!project->isEmpty("RC_FILE") ? keyFor("ICNS_FILE_REFERENCE") : TQString(""))
	  << "\t\t\t" << ");" << "\n"
	  << "\t\t\t" << "isa = PBXResourcesBuildPhase;" << "\n"
	  << "\t\t\t" << "name = \"" << grp << "\";" << "\n"
	  << "\t\t" << "};" << "\n";
    }
    { //INSTALL BUILDPHASE (sh script)
        TQString phase_key = keyFor("QMAKE_PBX_TARGET_COPY_PHASE");
        TQString destDir = Option::output_dir;
        if (!project->isEmpty("QMAKE_ORIG_DESTDIR"))
            destDir = project->first("QMAKE_ORIG_DESTDIR");
        fixEnvs(destDir);
        destDir = TQFileInfo(Option::fixPathToLocalOS(destDir)).absFilePath();
        project->variables()["QMAKE_PBX_PRESCRIPT_BUILDPHASES"].append(phase_key);
        t << "\t\t" << phase_key << " = {\n"
          << "\t\t\tbuildActionMask = 2147483647;\n"
          << "\t\t\tdstPath = " << destDir << ";\n"
          << "\t\t\tdstSubfolderSpec = 0;\n"
          << "\t\t\tfiles = (\n"
          << "\t\t\t" << keyFor("QMAKE_PBX_TARGET_COPY_FILE") << ",\n"
          << "\t\t\t);\n"
          << "\t\t\tisa = PBXCopyFilesBuildPhase;\n"
          << "\t\t\trunOnlyForDeploymentPostprocessing = 0;\n"
          << "\t\t};\n"
          << "\t\t" << keyFor("QMAKE_PBX_TARGET_COPY_FILE")  << " = {\n"
          << "\t\t\tfileRef =  " <<  keyFor(pbx_dir + "QMAKE_PBX_REFERENCE") << ";\n"
          << "\t\t\tisa = PBXBuildFile;\n"
          << "\t\t\tsettings = {\n"
          << "\t\t\t};\n"
          << "\t\t};\n";
    }
    if(/*ideType() == MAC_XCODE &&*/ !project->isEmpty("QMAKE_PBX_PRESCRIPT_BUILDPHASES") && 0) {
	// build reference
	t << "\t\t" << keyFor("QMAKE_PBX_PRESCRIPT_BUILDREFERENCE") << " = {" << "\n"
	  << "\t\t\t" << "includeInIndex = 0;" << "\n"
	  << "\t\t\t" << "isa = PBXFileReference;" << "\n"
	  << "\t\t\t" << "path = preprocessor.out;" << "\n"
	  << "\t\t\t" << "refType = 3;" << "\n"
	  << "\t\t\t" << "sourceTree = BUILT_PRODUCTS_DIR;" << "\n"
	  << "\t\t" << "};" << "\n";
	project->variables()["QMAKE_PBX_PRODUCTS"].append(keyFor("QMAKE_PBX_PRESCRIPTS_BUILDREFERENCE"));
	//build phase
        TQString prescript_key = keyFor("QMAKE_PBX_PRESCRIPTS_BUILDPHASE");
        project->variables()["QMAKE_PBX_TARGETS"].append(prescript_key);
	t << "\t\t" << prescript_key << " = {" << "\n"
	  << "\t\t\t" << "buildPhases = (" << "\n"
	  << varGlue("QMAKE_PBX_PRESCRIPT_BUILDPHASES", "\t\t\t\t", ",\n\t\t\t\t", "\n")
	  << "\t\t\t" << ");" << "\n"
	  << "\t\t\t" << "buildRules = (" << "\n"
	  << "\t\t\t" << ");" << "\n"
	  << "\t\t\t" << "buildSettings = {" << "\n"
	  << "\t\t\t" << "};" << "\n"
	  << "\t\t\t" << "dependencies = (" << "\n"
	  << "\t\t\t" << ");" << "\n"
	  << "\t\t\t" << "isa = PBXNativeTarget;" << "\n"
	  << "\t\t\t" << "name = \"TQt Preprocessor Steps\";" << "\n"
	  << "\t\t\t" << "productName = \"TQt Preprocessor Steps\";" << "\n"
	  << "\t\t\t" << "productReference = " << keyFor("QMAKE_PBX_PRESCRIPTS_BUILDREFERENCE") << ";" << "\n"
	  << "\t\t\t" << "productType = \"com.apple.product-type.tool\";" << "\n"
	  << "\t\t" << "};" << "\n";
	//dependency
	t << "\t\t" << keyFor("QMAKE_PBX_PRESCRIPTS_DEPENDENCY") << " = {" << "\n"
	  << "\t\t\t" << "isa = PBXTargetDependency;" << "\n"
	  << "\t\t\t" << "target = " << keyFor("QMAKE_PBX_PRESCRIPTS_BUILDPHASE") << ";" << "\n"
	  << "\t\t" << "};" << "\n";
	project->variables()["QMAKE_PBX_TARGET_DEPENDS"].append(keyFor("QMAKE_PBX_PRESCRIPTS_DEPENDENCY"));
	project->variables()["QMAKE_PBX_PRESCRIPT_BUILDPHASES"].clear(); //these are already consumed above
    }

    //DUMP EVERYTHING THAT TIES THE ABOVE TOGETHER
    //ROOT_GROUP
    t << "\t\t" << keyFor("QMAKE_PBX_ROOT_GROUP") << " = {" << "\n"
      << "\t\t\t" << "children = (" << "\n"
      << varGlue("QMAKE_PBX_GROUPS", "\t\t\t\t", ",\n\t\t\t\t", "\n")
      << "\t\t\t" << ");" << "\n"
      << "\t\t\t" << "isa = PBXGroup;" << "\n"
      << "\t\t\t" << "name = " << project->first("QMAKE_ORIG_TARGET") << ";" << "\n"
      << "\t\t\t" << "path = \"\";" << "\n"
      << "\t\t\t" << "refType = 4;" << "\n"
      << "\t\t" << "};" << "\n";
    //REFERENCE
    project->variables()["QMAKE_PBX_PRODUCTS"].append(keyFor(pbx_dir + "QMAKE_PBX_REFERENCE"));
    t << "\t\t" << keyFor(pbx_dir + "QMAKE_PBX_REFERENCE") << " = {" << "\n"
      << "\t\t\t" << "fallbackIsa = PBXFileReference;" << "\n";
    if(project->first("TEMPLATE") == "app") {
	TQString targ = project->first("QMAKE_ORIG_TARGET");
	if(project->isActiveConfig("resource_fork") && !project->isActiveConfig("console")) {
	    targ += ".app";
	    t << "\t\t\t" << "isa = PBXApplicationReference;" << "\n";
	} else {
	    t << "\t\t\t" << "isa = PBXExecutableFileReference;" << "\n";
	}
	TQString app = (!project->isEmpty("DESTDIR") ? project->first("DESTDIR") + project->first("QMAKE_ORIG_TARGET") :
		       TQDir::currentDirPath()) + Option::dir_sep + targ;
	t << "\t\t\t" << "name = " <<  targ << ";" << "\n"
	  << "\t\t\t" << "path = \"" << targ << "\";" << "\n"
	  << "\t\t\t" << "refType = " << reftypeForFile(app) << ";" << "\n";
    } else {
	TQString lib = project->first("QMAKE_ORIG_TARGET");
	if(project->isActiveConfig("staticlib")) {
	    lib = project->first("TARGET");
	} else if(!project->isActiveConfig("frameworklib")) {
	    if(project->isActiveConfig("plugin"))
		lib = project->first("TARGET");
	    else
		lib = project->first("TARGET_");
	}
	int slsh = lib.findRev(Option::dir_sep);
	if(slsh != -1)
	    lib = lib.right(lib.length() - slsh - 1);
	t << "\t\t\t" << "isa = PBXLibraryReference;" << "\n"
	  << "\t\t\t" << "expectedFileType = \"compiled.mach-o.dylib\";" << "\n"
	  << "\t\t\t" << "path = " << lib << ";\n"
	  << "\t\t\t" << "refType = " << 3/*reftypeForFile(lib)*/ << ";" << "\n"
	  << "\t\t\t" << "sourceTree = BUILT_PRODUCTS_DIR" << ";" << "\n";
    }
    t << "\t\t" << "};" << "\n";
    { //Products group
	TQString grp("Products"), key = keyFor(grp);
	project->variables()["QMAKE_PBX_GROUPS"].append(key);
	t << "\t\t" << key << " = {" << "\n"
	  << "\t\t\t" << "children = (" << "\n"
	  << varGlue("QMAKE_PBX_PRODUCTS", "\t\t\t\t", ",\n\t\t\t\t", "\n")
	  << "\t\t\t" << ");" << "\n"
	  << "\t\t\t" << "isa = PBXGroup;" << "\n"
	  << "\t\t\t" << "name = Products;" << "\n"
	  << "\t\t\t" << "refType = 4;" << "\n"
	  << "\t\t" << "};" << "\n";
    }
    //TARGET
    TQString target_key = keyFor("QMAKE_PBX_TARGET");
    project->variables()["QMAKE_PBX_TARGETS"].append(target_key);
    t << "\t\t" << target_key << " = {" << "\n"
      << "\t\t\t" << "buildPhases = (" << "\n"
      << varGlue("QMAKE_PBX_PRESCRIPT_BUILDPHASES", "\t\t\t\t", ",\n\t\t\t\t", ",\n")
      << varGlue("QMAKE_PBX_BUILDPHASES", "\t\t\t\t", ",\n\t\t\t\t", "\n")
      << "\t\t\t" << ");" << "\n"
      << "\t\t\t" << "buildSettings = {" << "\n"
      << "\t\t\t\t" << "CC = \"" << fixEnvsList("QMAKE_CC") << "\";" << "\n"
      << "\t\t\t\t" << "CPLUSPLUS = \"" << fixEnvsList("QMAKE_CXX") << "\";" << "\n"
      << "\t\t\t\t" << "FRAMEWORK_SEARCH_PATHS = \"\";" << "\n"
      << "\t\t\t\t" << "HEADER_SEARCH_PATHS = \"" << fixEnvsList("INCLUDEPATH") << " " << fixEnvs(specdir()) << "\";" << "\n"
      << "\t\t\t\t" << "LIBRARY_SEARCH_PATHS = \"" << var("QMAKE_PBX_LIBPATHS") << "\";" << "\n"
      << "\t\t\t\t" << "OPTIMIZATION_CFLAGS = \"\";" << "\n"
      << "\t\t\t\t" << "GCC_GENERATE_DEBUGGING_SYMBOLS = " <<
        (project->isActiveConfig("debug") ? "YES" : "NO") << ";" << "\n"
      << "\t\t\t\t" << "OTHER_CFLAGS = \"" <<
	fixEnvsList("QMAKE_CFLAGS") << fixQuotes(varGlue("PRL_EXPORT_DEFINES"," -D"," -D","")) <<
	fixQuotes(varGlue("DEFINES"," -D"," -D","")) << "\";" << "\n"
      << "\t\t\t\t" << "LEXFLAGS = \"" << var("QMAKE_LEXFLAGS") << "\";" << "\n"
      << "\t\t\t\t" << "YACCFLAGS = \"" << var("QMAKE_YACCFLAGS") << "\";" << "\n"
      << "\t\t\t\t" << "OTHER_CPLUSPLUSFLAGS = \"" <<
	fixEnvsList("QMAKE_CXXFLAGS") << fixQuotes(varGlue("PRL_EXPORT_DEFINES"," -D"," -D","")) <<
	fixQuotes(varGlue("DEFINES"," -D"," -D","")) << "\";" << "\n"
      << "\t\t\t\t" << "OTHER_REZFLAGS = \"\";" << "\n"
      << "\t\t\t\t" << "SECTORDER_FLAGS = \"\";" << "\n"
      << "\t\t\t\t" << "WARNING_CFLAGS = \"\";" << "\n"
      << "\t\t\t\t" << "PREBINDING = " << (project->isEmpty("QMAKE_DO_PREBINDING") ? "NO" : "YES") << ";" << "\n";
    if(project->isActiveConfig("debug"))
        t << "\t\t\t\t" << "GCC_OPTIMIZATION_LEVEL = 0" << ";" << "\n";
    if(!project->isEmpty("PRECOMPILED_HEADER")) {
        if(pbVersion >= 38) {
            t << "\t\t\t\t" << "GCC_PRECOMPILE_PREFIX_HEADER = \"YES\";" << "\n"
                << "\t\t\t\t" << "GCC_PREFIX_HEADER = \"" <<  project->first("PRECOMPILED_HEADER") << "\";" << "\n";
        } else {
            t << "\t\t\t\t" << "PRECOMPILE_PREFIX_HEADER = \"YES\";" << "\n"
                << "\t\t\t\t" << "PREFIX_HEADER = \"" <<  project->first("PRECOMPILED_HEADER") << "\";" << "\n";
        }
    }
    if(project->first("TEMPLATE") == "app") {
        TQString plist = fileFixify(project->first("QMAKE_INFO_PLIST"));
        if(plist.isEmpty())
            plist = specdir() + TQDir::separator() + "Info.plist." + project->first("TEMPLATE");
	if(TQFile::exists(plist)) {
	    TQFile plist_in_file(plist);
	    if(plist_in_file.open(IO_ReadOnly)) {
		TQTextStream plist_in(&plist_in_file);
		TQString plist_in_text = plist_in.read();
                plist_in_text = plist_in_text.replace("@ICON@", (project->isEmpty("RC_FILE") ? TQString("") : project->first("RC_FILE").section(Option::dir_sep, -1)));
                plist_in_text = plist_in_text.replace("@EXECUTABLE@", project->first("QMAKE_ORIG_TARGET"));
		TQFile plist_out_file("Info.plist");
		if(plist_out_file.open(IO_WriteOnly | IO_Translate)) {
		    TQTextStream plist_out(&plist_out_file);
		    plist_out << plist_in_text;
		    t << "\t\t\t\t" << "INFOPLIST_FILE = \"Info.plist\";" << "\n";
		}
	    }
	}
    }
#if 1
    t << "\t\t\t\t" << "BUILD_ROOT = \"" << TQDir::currentDirPath() << "\";" << "\n";
#endif
    if(!project->isActiveConfig("staticlib"))
	t << "\t\t\t\t" << "OTHER_LDFLAGS = \"" << fixEnvsList("SUBLIBS") << " " <<
	    fixEnvsList("QMAKE_LFLAGS") << " " << fixEnvsList("QMAKE_LIBDIR_FLAGS") <<
	    " " << fixEnvsList("QMAKE_LIBS") << "\";" << "\n";
    if(!project->isEmpty("DESTDIR")) {
	TQString dir = project->first("DESTDIR");
	if (TQDir::isRelativePath(dir))
	    dir.prepend(TQDir::currentDirPath() + Option::dir_sep);
	t << "\t\t\t\t" << "INSTALL_DIR = \"" << dir << "\";" << "\n";
    }
    if ( project->first("TEMPLATE") == "lib") {
	t << "\t\t\t\t" << "INSTALL_PATH = \"" <<  "\";" << "\n";
    }
    if(!project->isEmpty("VERSION") && project->first("VERSION") != "0.0.0") {
	t << "\t\t\t\t" << "DYLIB_CURRENT_VERSION = \"" << project->first("VER_MAJ") << "."
	  << project->first("VER_MIN") << "." << project->first("VER_PAT")  << "\";" << "\n";
	if(project->isEmpty("COMPAT_VERSION"))
	    t << "\t\t\t\t" << "DYLIB_COMPATIBILITY_VERSION = \"" << project->first("VER_MAJ") << "."
	      << project->first("VER_MIN")  << "\";" << "\n";
    }
    if(!project->isEmpty("COMPAT_VERSION"))
        t << "\t\t\t\t" << "DYLIB_COMPATIBILITY_VERSION = \"" << project->first("COMPAT_VERSION") << "\";" << "\n";
    if(!project->isEmpty("QMAKE_MACOSX_DEPLOYMENT_TARGET"))
        t << "\t\t\t\t" << "MACOSX_DEPLOYMENT_TARGET = \""
          << project->first("QMAKE_MACOSX_DEPLOYMENT_TARGET") << "\";" << "\n";
    if(pbVersion >= 38) {
        if(!project->isEmpty("OBJECTS_DIR"))
            t << "\t\t\t\t" << "OBJROOT = \"" << project->first("OBJECTS_DIR") << "\";" << "\n";
    }
#if 0
    if(!project->isEmpty("DESTDIR"))
	t << "\t\t\t\t" << "SYMROOT = \"" << project->first("DESTDIR") << "\";" << "\n";
    else
	t << "\t\t\t\t" << "SYMROOT = \"" << TQDir::currentDirPath() << "\";" << "\n";
#endif
    if(project->first("TEMPLATE") == "app") {
        if(pbVersion < 38 && !project->isActiveConfig("console"))
            t << "\t\t\t\t" << "WRAPPER_SUFFIX = app;" << "\n";
        t << "\t\t\t\t" << "PRODUCT_NAME = " << project->first("QMAKE_ORIG_TARGET") << ";" << "\n";
    } else {
	if(!project->isActiveConfig("plugin") && project->isActiveConfig("staticlib")) {
	    t << "\t\t\t\t" << "LIBRARY_STYLE = STATIC;" << "\n";
	} else {
	    t << "\t\t\t\t" << "LIBRARY_STYLE = DYNAMIC;" << "\n";
	}
	TQString lib = project->first("QMAKE_ORIG_TARGET");
	if (!project->isActiveConfig("frameworklib") && !project->isActiveConfig("staticlib"))
	    lib.prepend("lib");
	t << "\t\t\t\t" << "PRODUCT_NAME = " << lib << ";" << "\n";
    }
    tmp = project->variables()["QMAKE_PBX_VARS"];
    for(TQStringList::Iterator it = tmp.begin(); it != tmp.end(); ++it) {
	TQString var = (*it), val = getenv(var);
	if(!val && var == "TB")
	    val = "/usr/bin/";
	t << "\t\t\t\t" << var << " = \"" << val << "\";" << "\n";
    }
    t << "\t\t\t" << "};" << "\n"
      << "\t\t\t" << "conditionalBuildSettings = {" << "\n"
      << "\t\t\t" << "};" << "\n"
      << "\t\t\t" << "dependencies = (" << "\n"
      << varGlue("QMAKE_PBX_TARGET_DEPENDS", "\t\t\t\t", ",\n\t\t\t\t", "\n")
      << "\t\t\t" << ");" << "\n"
      << "\t\t\t" << "productReference = " << keyFor(pbx_dir + "QMAKE_PBX_REFERENCE") << ";" << "\n"
      << "\t\t\t" << "shouldUseHeadermap = 1;" << "\n";
    if(pbVersion >= 38)
        t << "\t\t\t" << "isa = PBXNativeTarget;" << "\n";
    if(project->first("TEMPLATE") == "app") {
        if(project->isActiveConfig("console")) {
            if(pbVersion >= 38)
                t << "\t\t\t" << "productType = \"com.apple.product-type.tool\";" << "\n";
            else
                t << "\t\t\t" << "isa = PBXToolTarget;" << "\n";
        } else {
            if(pbVersion >= 38)
                t << "\t\t\t" << "productType = \"com.apple.product-type.application\";" << "\n";
            else
                t << "\t\t\t" << "isa = PBXApplicationTarget;" << "\n";
            t << "\t\t\t" << "productSettingsXML = \"";
            bool read_plist = false;
            if(TQFile::exists("Info.plist")) {
                TQFile plist("Info.plist");
                if(plist.open(IO_ReadOnly)) {
                    read_plist = true;
                    TQTextStream stream(&plist);
                    while(!stream.eof())
                        t << stream.readLine().replace('"', "\\\"") << endl;
                }
            }
            if(!read_plist) {
                t << "<?xml version="
                  << "\\\"1.0\\\" encoding=" << "\\\"UTF-8\\\"" << "?>" << "\n"
                  << "\t\t\t\t" << "<!DOCTYPE plist SYSTEM \\\"file://localhost/System/"
                  << "Library/DTDs/PropertyList.dtd\\\">" << "\n"
                  << "\t\t\t\t" << "<plist version=\\\"0.9\\\">" << "\n"
                  << "\t\t\t\t" << "<dict>" << "\n"
                  << "\t\t\t\t\t" << "<key>CFBundleDevelopmentRegion</key>" << "\n"
                  << "\t\t\t\t\t" << "<string>English</string>" << "\n"
                  << "\t\t\t\t\t" << "<key>CFBundleExecutable</key>" << "\n"
                  << "\t\t\t\t\t" << "<string>" << project->first("QMAKE_ORIG_TARGET") << "</string>" << "\n"
                  << "\t\t\t\t\t" << "<key>CFBundleIconFile</key>" << "\n"
                  << "\t\t\t\t\t" << "<string>" << var("RC_FILE").section(Option::dir_sep, -1) << "</string>" << "\n"
                  << "\t\t\t\t\t" << "<key>CFBundleInfoDictionaryVersion</key>"  << "\n"
                  << "\t\t\t\t\t" << "<string>6.0</string>" << "\n"
                  << "\t\t\t\t\t" << "<key>CFBundlePackageType</key>" << "\n"
                  << "\t\t\t\t\t" << "<string>APPL</string>" << "\n"
                  << "\t\t\t\t\t" << "<key>CFBundleSignature</key>" << "\n"
                    //Although the output below looks strange it is to avoid the trigraph ??<
                  << "\t\t\t\t\t" << "<string>????" << "</string>" << "\n"
                  << "\t\t\t\t\t" << "<key>CFBundleVersion</key>" << "\n"
                  << "\t\t\t\t\t" << "<string>0.1</string>" << "\n"
                  << "\t\t\t\t\t" << "<key>CSResourcesFileMapped</key>" << "\n"
                  << "\t\t\t\t\t" << "<true/>" << "\n"
                  << "\t\t\t\t" << "</dict>" << "\n"
                  << "\t\t\t\t" << "</plist>";
            }
	}
        t << "\";" << "\n";
	t << "\t\t\t" << "name = \"" << project->first("QMAKE_ORIG_TARGET") << "\";" << "\n"
	  << "\t\t\t" << "productName = " << project->first("QMAKE_ORIG_TARGET") << ";" << "\n";
    } else {
        TQString lib = project->first("QMAKE_ORIG_TARGET");
        if(!project->isActiveConfig("frameworklib") && !project->isActiveConfig("staticlib"))
           lib.prepend("lib");
        t << "\t\t\t" << "name = \"" << lib << "\";" << "\n"
          << "\t\t\t" << "productName = " << lib << ";" << "\n";
        if(pbVersion >= 38) {
            if(project->isActiveConfig("staticlib"))
                t << "\t\t\t" << "productType = \"com.apple.product-type.library.static\";" << "\n";
            else
                t << "\t\t\t" << "productType = \"com.apple.product-type.library.dynamic\";" << "\n";
        } else {
            t << "\t\t\t" << "isa = PBXLibraryTarget;" << "\n";
        }
    }
    t << "\t\t\t" << "startupPath = \"<<ProjectDirectory>>\";" << "\n";
    if(!project->isEmpty("DESTDIR"))
	t << "\t\t\t" << "productInstallPath = \"" << project->first("DESTDIR") << "\";" << "\n";
    t << "\t\t" << "};" << "\n";
    //DEBUG/RELEASE
    TQString active_buildstyle;
#if 0
    for(int as_release = 0; as_release < 2; as_release++)
#else
	bool as_release = !project->isActiveConfig("debug");
#endif
    {
        TQMap<TQString, TQString> settings;
        settings.insert("COPY_PHASE_STRIP", (as_release ? "YES" : "NO"));
        if(as_release)
            settings.insert("GCC_GENERATE_DEBUGGING_SYMBOLS", "NO");
        TQString name;
        if(pbVersion >= 42)
            name = (as_release ? "Release" : "Debug");
        else
            name = (as_release ? "Deployment" : "Development");

        if(pbVersion >= 42) {
            TQString key = keyFor("QMAKE_PBX_BUILDCONFIG_" + name);
            project->variables()["QMAKE_PBX_BUILDCONFIGS"].append(key);
            t << "\t\t" << key << " = {" << "\n"
              << "\t\t\t" << "isa = XCBuildConfiguration;" << "\n"
              << "\t\t\t" << "buildSettings = {" << "\n";
            for(TQMap<TQString, TQString>::Iterator set_it = settings.begin(); set_it != settings.end(); ++set_it)
                t << "\t\t\t\t" << set_it.key() << " = \"" << set_it.data() << "\";\n";
            t << "\t\t\t" << "};" << "\n"
              << "\t\t\t" << "name = " << name << ";" << "\n"
              << "\t\t" << "};" << "\n";
        }

        TQString key = keyFor("QMAKE_PBX_BUILDSTYLE_" + name);
        if(project->isActiveConfig("debug") != (bool)as_release) {
            project->variables()["QMAKE_PBX_BUILDSTYLES"].append(key);
            active_buildstyle = name;
        } else if(pbVersion >= 42) {
            project->variables()["QMAKE_PBX_BUILDSTYLES"].append(key);
        }
        t << "\t\t" << key << " = {" << "\n"
          << "\t\t\t" << "buildRules = (" << "\n"
          << "\t\t\t" << ");" << "\n"
          << "\t\t\t" << "buildSettings = {" << "\n";
        for(TQMap<TQString, TQString>::Iterator set_it = settings.begin(); set_it != settings.end(); ++set_it)
            t << "\t\t\t\t" << set_it.key() << " = \"" << set_it.data() << "\";\n";
        t << "\t\t\t" << "};" << "\n"
          << "\t\t\t" << "isa = PBXBuildStyle;" << "\n"
          << "\t\t\t" << "name = " << name << ";" << "\n"
          << "\t\t" << "};" << "\n";
    }
    if(pbVersion >= 42) {
        t << "\t\t" << keyFor("QMAKE_PBX_BUILDCONFIG_LIST") << " = {" << "\n"
          << "\t\t\t" << "isa = XCConfigurationList;" << "\n"
          << "\t\t\t" << "buildConfigurations = (" << "\n"
          << varGlue("QMAKE_PBX_BUILDCONFIGS", "\t\t\t\t", ",\n\t\t\t\t", "\n")
          << "\t\t\t" << ");" << "\n"
          << "\t\t\t" << "defaultConfigurationIsVisible = 0;" << "\n"
          << "\t\t\t" << "defaultConfigurationIsName = " << active_buildstyle << ";" << "\n"
          << "\t\t" << "};" << "\n";
    }
    //ROOT
    t << "\t\t" << keyFor("QMAKE_PBX_ROOT") << " = {" << "\n"
      << "\t\t\t" << "buildStyles = (" << "\n"
      << varGlue("QMAKE_PBX_BUILDSTYLES", "\t\t\t\t", ",\n\t\t\t\t", "\n")
      << "\t\t\t" << ");" << "\n"
      << "\t\t\t" << "hasScannedForEncodings = 1;" << "\n"
      << "\t\t\t" << "isa = PBXProject;" << "\n"
      << "\t\t\t" << "mainGroup = " << keyFor("QMAKE_PBX_ROOT_GROUP") << ";" << "\n"
      << "\t\t\t" << "projectDirPath = \"\";" << "\n"
      << "\t\t\t" << "targets = (" << "\n"
      << varGlue("QMAKE_PBX_TARGETS", "\t\t\t\t", ",\n\t\t\t\t", "\n")
      << "\t\t\t" << ");" << "\n"
      << "\t\t" << "};" << "\n";

    //FOOTER
    t << "\t" << "};" << "\n"
      << "\t" << "rootObject = " << keyFor("QMAKE_PBX_ROOT") << ";" << "\n"
      << "}" << endl;

    if(project->isActiveConfig("generate_pbxbuild_makefile")) {
	TQString mkwrap = fileFixify(pbx_dir + Option::dir_sep + ".." + Option::dir_sep + project->first("MAKEFILE"),
				    TQDir::currentDirPath());
	TQFile mkwrapf(mkwrap);
	if(mkwrapf.open(IO_WriteOnly | IO_Translate)) {
	    debug_msg(1, "pbuilder: Creating file: %s", mkwrap.latin1());
	    TQTextStream mkwrapt(&mkwrapf);
	    writeHeader(mkwrapt);
	    const char *cleans = "uiclean mocclean preprocess_clean ";
	    mkwrapt << "#This is a makefile wrapper for PROJECT BUILDER\n"
		    << "all:" << "\n\t"
		    << "cd " << project->first("QMAKE_ORIG_TARGET") << projectSuffix() << "/ && " << pbxbuild() << "\n"
		    << "install: all" << "\n\t"
		    << "cd " << project->first("QMAKE_ORIG_TARGET") << projectSuffix() << "/ && " << pbxbuild() << " install\n"
		    << "distclean clean: preprocess_clean" << "\n\t"
		    << "cd " << project->first("QMAKE_ORIG_TARGET") << projectSuffix() << "/ && " << pbxbuild() << " clean" << "\n"
		    << (!did_preprocess ? cleans : "") << ":" << "\n";
	    if(did_preprocess)
		mkwrapt << cleans << ":" << "\n\t"
			<< "make -f "
			<< pbx_dir << Option::dir_sep << "qt_preprocess.mak $@" << endl;
	}
    }
    return true;
}

TQString
ProjectBuilderMakefileGenerator::fixQuotes(const TQString &val)
{
    TQString ret(val);
    ret = ret.replace(TQRegExp("('|\\\\|\")"), "\\\\1");
    return ret;
}

TQString
ProjectBuilderMakefileGenerator::fixEnvs(const TQString &file)
{
    TQRegExp reg_var("\\$\\((.*)\\)");
    for(int rep = 0; (rep = reg_var.search(file, rep)) != -1; ) {
	if(project->variables()["QMAKE_PBX_VARS"].findIndex(reg_var.cap(1)) == -1)
	    project->variables()["QMAKE_PBX_VARS"].append(reg_var.cap(1));
	rep += reg_var.matchedLength();
    }
    return file;
}

TQString
ProjectBuilderMakefileGenerator::fixEnvsList(const TQString &where)
{
    TQString ret;
    const TQStringList &l = project->variables()[where];
    for(TQStringList::ConstIterator it = l.begin(); it != l.end(); ++it) {
	fixEnvs((*it));
	if(!ret.isEmpty())
	    ret += " ";
	ret += fixQuotes((*it));
    }
    return ret;
}

TQString
ProjectBuilderMakefileGenerator::keyFor(const TQString &block)
{
#if 1 //This make this code much easier to debug..
    if(project->isActiveConfig("no_pb_munge_key"))
       return block;
#endif
    TQString ret;
    if(!keys.contains(block)) {
	ret = qtMD5(block.utf8()).left(24).upper();
	keys.insert(block, ret);
    } else {
	ret = keys[block];
    }
    return ret;
}

bool
ProjectBuilderMakefileGenerator::openOutput(TQFile &file) const
{
    if(TQDir::isRelativePath(file.name()))
	file.setName(Option::output_dir + file.name()); //pwd when qmake was run
    TQFileInfo fi(file);
    if(fi.extension() != "pbxproj" || file.name().isEmpty()) {
	TQString output = file.name();
	if(fi.isDir())
	    output += TQDir::separator();
	if(!output.endsWith(projectSuffix())) {
	    if(file.name().isEmpty() || fi.isDir())
		output += project->first("TARGET");
	    output += projectSuffix() + TQDir::separator();
	} else if(output[(int)output.length() - 1] != TQDir::separator()) {
	    output += TQDir::separator();
	}
	output += TQString("project.pbxproj");
	file.setName(output);
    }
    bool ret = UnixMakefileGenerator::openOutput(file);
    ((ProjectBuilderMakefileGenerator*)this)->pbx_dir = Option::output_dir.section(Option::dir_sep, 0, -1);
    Option::output_dir = pbx_dir.section(Option::dir_sep, 0, -2);
    return ret;
}

/* This function is such a hack it is almost pointless, but it
   eliminates the warning message from ProjectBuilder that the project
   file is for an older version. I guess this could be used someday if
   the format of the output is dependant upon the version of
   ProjectBuilder as well.
*/
int
ProjectBuilderMakefileGenerator::pbuilderVersion() const
{
    TQString ret;
    if(project->isEmpty("QMAKE_PBUILDER_VERSION")) {
        TQString version, version_plist = project->first("QMAKE_PBUILDER_VERSION_PLIST");
        if(version_plist.isEmpty()) {
            if(TQFile::exists("/Developer/Applications/Xcode.app/Contents/version.plist"))
                version_plist = "/Developer/Applications/Xcode.app/Contents/version.plist";
            else
                version_plist = "/Developer/Applications/Project Builder.app/Contents/version.plist";
        } else {
            version_plist = version_plist.replace(TQRegExp("\""), "");
        }
        TQFile version_file(version_plist);
        if (version_file.open(IO_ReadOnly)) {
            debug_msg(1, "pbuilder: version.plist: Reading file: %s", version_plist.latin1());
            TQTextStream plist(&version_file);

            bool in_dict = false;
            TQString current_key;
            TQRegExp keyreg("^<key>(.*)</key>$"), stringreg("^<string>(.*)</string>$");
            while(!plist.atEnd()) {
                TQString line = plist.readLine().stripWhiteSpace();
                if(line == "<dict>")
                    in_dict = true;
                else if(line == "</dict>")
                    in_dict = false;
                else if(in_dict) {
                    if(keyreg.exactMatch(line))
                        current_key = keyreg.cap(1);
                    else if(current_key == "CFBundleShortVersionString" && stringreg.exactMatch(line))
                        version = stringreg.cap(1);
                }
            }
            version_file.close();
        } else { debug_msg(1, "pbuilder: version.plist: Failure to open %s", version_plist.latin1()); }
        if(version.isEmpty() && version_plist.contains("Xcode")) {
            ret = "39";
        } else {
            if(version.startsWith("2."))
                ret = "42";
            else if(version == "1.5")
                ret = "39";
            else if(version == "1.1")
                ret = "34";
        }
    } else {
        ret = project->first("QMAKE_PBUILDER_VERSION");
    }
    if(!ret.isEmpty()) {
        bool ok;
        int int_ret = ret.toInt(&ok);
        if(ok) {
            debug_msg(1, "pbuilder: version.plist: Got version: %d", int_ret);
            return int_ret;
        }
    }
    debug_msg(1, "pbuilder: version.plist: Fallback to default version");
    return 34; //my fallback
}

int
ProjectBuilderMakefileGenerator::reftypeForFile(const TQString &where)
{
    int ret = 0; //absolute is the default..
    if(TQDir::isRelativePath(where))
	ret = 4; //relative
    return ret;
}

TQString
ProjectBuilderMakefileGenerator::projectSuffix() const
{
    const int pbVersion = pbuilderVersion();
    if(pbVersion >= 42)
        return ".xcodeproj";
    else if(pbVersion >= 38)
        return ".xcode";
    return ".pbproj";
}

TQString
ProjectBuilderMakefileGenerator::pbxbuild()
{
    if(TQFile::exists("/usr/bin/pbbuild"))
	return "pbbuild";
    if(TQFile::exists("/usr/bin/xcodebuild"))
       return "xcodebuild";
    return (pbuilderVersion() >= 38 ? "xcodebuild" : "pbxbuild");
}

