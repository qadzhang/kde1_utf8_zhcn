/****************************************************************************
**
** Implementation of Win32MakefileGenerator class.
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

#include "winmakefile.h"
#include "option.h"
#include "project.h"
#include "meta.h"
#include <ntqtextstream.h>
#include <ntqstring.h>
#include <ntqdict.h>
#include <ntqregexp.h>
#include <ntqstringlist.h>
#include <ntqdir.h>


Win32MakefileGenerator::Win32MakefileGenerator(TQMakeProject *p) : MakefileGenerator(p)
{

}


struct SubDir
{
    TQString directory, profile, target, makefile;
};

void
Win32MakefileGenerator::writeSubDirs(TQTextStream &t)
{
    TQPtrList<SubDir> subdirs;
    {
	TQStringList subdirs_in = project->variables()["SUBDIRS"];
	for(TQStringList::Iterator it = subdirs_in.begin(); it != subdirs_in.end(); ++it) {
	    TQString file = (*it);
	    file = fileFixify(file);
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

    t << "MAKEFILE = " << (project->isEmpty("MAKEFILE") ? TQString("Makefile") : var("MAKEFILE")) << endl;
    t << "QMAKE =	" << (project->isEmpty("QMAKE_QMAKE") ? TQString("qmake") : var("QMAKE_QMAKE")) << endl;
    t << "SUBTARGETS	= ";
    for( it.toFirst(); it.current(); ++it)
	t << " \\\n\t\t" << it.current()->target;
    t << endl << endl;
    t << "all: $(MAKEFILE) $(SUBTARGETS)" << endl << endl;

    for( it.toFirst(); it.current(); ++it) {
	bool have_dir = !(*it)->directory.isEmpty();

        //make the makefile
	TQString mkfile = (*it)->makefile;
	if(have_dir)
	    mkfile.prepend((*it)->directory + Option::dir_sep);
	t << mkfile << ":";
	if(have_dir)
	    t << "\n\t" << "cd " << (*it)->directory;
	t << "\n\t" << "$(QMAKE) " << (*it)->profile << " " << buildArgs();
	t << " -o " << (*it)->makefile;
	if(have_dir) {
	    int subLevels = it.current()->directory.contains(Option::dir_sep) + 1;
	    t << "\n\t" << "@cd ..";
	    for(int i = 1; i < subLevels; i++ )
		t << Option::dir_sep << "..";
	}
	t << endl;

	//now actually build
	t << (*it)->target << ": " << mkfile;
	if(project->variables()["QMAKE_NOFORCE"].isEmpty())
	    t << " FORCE";
	if(have_dir)
	    t << "\n\t" << "cd " << (*it)->directory;
	t << "\n\t" << "$(MAKE)";
        t << " -f " << (*it)->makefile;
	if(have_dir) {
	    int subLevels = it.current()->directory.contains(Option::dir_sep) + 1;
	    t << "\n\t" << "@cd ..";
	    for(int i = 1; i < subLevels; i++ )
		t << Option::dir_sep << "..";
	}
	t << endl << endl;
    }

    if (project->isActiveConfig("ordered")) {   // generate dependencies
        for( it.toFirst(); it.current(); ) {
            TQString tar = it.current()->target;
            ++it;
            if (it.current())
                t << it.current()->target << ": " << tar << endl;
        }
        t << endl;
    }

    if(project->variables()["QMAKE_INTERNAL_QMAKE_DEPS"].findIndex("qmake_all") == -1)
	project->variables()["QMAKE_INTERNAL_QMAKE_DEPS"].append("qmake_all");
    writeMakeQmake(t);

    t << "qmake_all:";
    if ( !subdirs.isEmpty() ) {
	for( it.toFirst(); it.current(); ++it) {
            bool have_dir = !(*it)->directory.isEmpty();
	    TQString subdir = (*it)->directory;
	    TQString profile = (*it)->profile;
	    int subLevels = subdir.contains(Option::dir_sep) + 1;
            t << "\n\t";
            if(have_dir)
	      t << "cd " << subdir << "\n\t";
	    int lastSlash = subdir.findRev(Option::dir_sep);
	    if(lastSlash != -1)
		subdir = subdir.mid( lastSlash + 1 );
	    t << "$(QMAKE) "
	      << ( !profile.isEmpty() ? profile : subdir + ".pro" )
	      << " -o " << (*it)->makefile
	      << " " << buildArgs() << "\n\t";
            if(have_dir) {
	        t << "@cd ..";
                for(int i = 1; i < subLevels; i++ )
                    t << Option::dir_sep << "..";
            }
	}
    } else {
	// Borland make does not like empty an empty command section, so insert
	// a dummy command.
	t << "\n\t" << "@cd .";
    }
    t << endl << endl;

    TQStringList targs;
    targs << "clean" << "install_subdirs" << "mocables" << "uicables" << "uiclean" << "mocclean";
    targs += project->values("SUBDIR_TARGETS");
    for(TQStringList::Iterator targ_it = targs.begin(); targ_it != targs.end(); ++targ_it) {
        t << (*targ_it) << ": qmake_all";
	TQString targ = (*targ_it);
	if(targ == "install_subdirs")
	    targ = "install";
	else if(targ == "uninstall_subdirs")
	    targ = "uninstall";
	if(targ == "clean")
	    t << varGlue("QMAKE_CLEAN","\n\t-$(DEL_FILE) ","\n\t-$(DEL_FILE) ", "");
	if (!subdirs.isEmpty()) {
	    for( it.toFirst(); it.current(); ++it) {
		int subLevels = (*it)->directory.contains(Option::dir_sep) + 1;
		bool have_dir = !(*it)->directory.isEmpty();
		if(have_dir)
		    t << "\n\t" << "cd " << (*it)->directory;
		TQString in_file = " -f " + (*it)->makefile;
		t << "\n\t" << "$(MAKE) " << in_file << " " << targ;
		if(have_dir) {
		    t << "\n\t" << "@cd ..";
		    for(int i = 1; i < subLevels; i++ )
			t << Option::dir_sep << "..";
		}
	    }
	} else {
	    // Borland make does not like empty an empty command section, so
	    // insert a dummy command.
	    t << "\n\t" << "@cd .";
	}
	t << endl << endl;
    }

    //installations
    project->variables()["INSTALLDEPS"]   += "install_subdirs";
    project->variables()["UNINSTALLDEPS"] += "uninstall_subdirs";
    writeInstalls(t, "INSTALLS");

    // user defined targets
    TQStringList &qut = project->variables()["QMAKE_EXTRA_WIN_TARGETS"];
    for(TQStringList::Iterator sit = qut.begin(); sit != qut.end(); ++sit) {
	TQString targ = var((*sit) + ".target"),
		 cmd = var((*sit) + ".commands"), deps;
	if(targ.isEmpty())
	    targ = (*sit);
	TQStringList &deplist = project->variables()[(*sit) + ".depends"];
	for(TQStringList::Iterator dep_it = deplist.begin(); dep_it != deplist.end(); ++dep_it) {
	    TQString dep = var((*dep_it) + ".target");
	    if(dep.isEmpty())
		dep = (*dep_it);
	    deps += " " + dep;
	}
	if(!project->variables()["QMAKE_NOFORCE"].isEmpty() &&
	   project->variables()[(*sit) + ".CONFIG"].findIndex("phony") != -1)
	    deps += TQString(" ") + "FORCE";
	t << targ << ":" << deps << "\n";
        if(!cmd.isEmpty())
            t << "\t" << cmd << endl;
        t << endl;
    }
    t << endl << endl;

    if(project->variables()["QMAKE_NOFORCE"].isEmpty())
	t << "FORCE:" << endl << endl;
}


int
Win32MakefileGenerator::findHighestVersion(const TQString &d, const TQString &stem)
{
    TQString bd = Option::fixPathToLocalOS(d, true);
    if(!TQFile::exists(bd))
	return -1;
    if(!project->variables()["QMAKE_" + stem.upper() + "_VERSION_OVERRIDE"].isEmpty())
	return project->variables()["QMAKE_" + stem.upper() + "_VERSION_OVERRIDE"].first().toInt();

    TQDir dir(bd);
    int biggest=-1;
    TQStringList entries = dir.entryList();
    TQString dllStem = stem + TQTDLL_POSTFIX;
    TQRegExp regx( "(" + dllStem + "([0-9]*)).lib", false );
    for(TQStringList::Iterator it = entries.begin(); it != entries.end(); ++it) {
	if(regx.exactMatch((*it)))
	    biggest = TQMAX(biggest, (regx.cap(1) == dllStem ||
				     regx.cap(2).isEmpty()) ? -1 : regx.cap(2).toInt());
    }
    TQMakeMetaInfo libinfo;
    if(libinfo.readLib(bd + dllStem)) {
	if(!libinfo.isEmpty("QMAKE_PRL_VERSION"))
	    biggest = TQMAX(biggest, libinfo.first("QMAKE_PRL_VERSION").replace(".", "").toInt());
    }
    return biggest;
}

TQString
Win32MakefileGenerator::findDependency(const TQString &dep)
{
    {
	TQStringList &qut = project->variables()["QMAKE_EXTRA_WIN_TARGETS"];
	for(TQStringList::Iterator it = qut.begin(); it != qut.end(); ++it) {
	    TQString targ = var((*it) + ".target");
	    if(targ.isEmpty())
		targ = (*it);
	    if(targ.endsWith(dep))
		return targ;
	}
    }
    {
	TQStringList &quc = project->variables()["QMAKE_EXTRA_WIN_COMPILERS"];
	for(TQStringList::Iterator it = quc.begin(); it != quc.end(); ++it) {
	    TQString tmp_out = project->variables()[(*it) + ".output"].first();
	    TQString tmp_cmd = project->variables()[(*it) + ".commands"].join(" ");
	    if(tmp_out.isEmpty() || tmp_cmd.isEmpty())
		continue;
	    TQStringList &tmp = project->variables()[(*it) + ".input"];
	    for(TQStringList::Iterator it2 = tmp.begin(); it2 != tmp.end(); ++it2) {
		TQStringList &inputs = project->variables()[(*it2)];
		for(TQStringList::Iterator input = inputs.begin(); input != inputs.end(); ++input) {
		    TQString out = tmp_out;
		    TQFileInfo fi(Option::fixPathToLocalOS((*input)));
		    out.replace("${QMAKE_FILE_BASE}", fi.baseName());
		    out.replace("${QMAKE_FILE_NAME}", fi.filePath());
		    if(out.endsWith(dep))
			return out;
		}
	    }
	}
    }
    return MakefileGenerator::findDependency(dep);
}

bool
Win32MakefileGenerator::findLibraries(const TQString &where)
{

    TQStringList &l = project->variables()[where];
    TQPtrList<MakefileDependDir> dirs;
    {
	TQStringList &libpaths = project->variables()["QMAKE_LIBDIR"];
	for(TQStringList::Iterator libpathit = libpaths.begin(); libpathit != libpaths.end(); ++libpathit) {
	    TQString r = (*libpathit), l = r;
	    fixEnvVariables(l);
	    dirs.append(new MakefileDependDir(r.replace("\"",""), l.replace("\"","")));
	}
    }
    dirs.setAutoDelete(true);
    for(TQStringList::Iterator it = l.begin(); it != l.end(); ) {
	TQChar quote;
	bool modified_opt = false, remove = false;
	TQString opt = (*it).stripWhiteSpace();
	if((opt[0] == '\'' || opt[0] == '"') && opt[(int)opt.length()-1] == opt[0]) {
	    quote = opt[0];
	    opt = opt.mid(1, opt.length()-2);
	}
	if(opt.startsWith("/LIBPATH:")) {
            TQString r = opt.mid(9), l = Option::fixPathToLocalOS(r);
            dirs.append(new MakefileDependDir(r.replace("\"",""),
                                              l.replace("\"","")));
        } else if(opt.startsWith("-L") || opt.startsWith("/L")) {
            TQString r = opt.mid(2), l = Option::fixPathToLocalOS(r);
            dirs.append(new MakefileDependDir(r.replace("\"",""),
                                              l.replace("\"","")));
            remove = true; //we eat this switch
        } else if(opt.startsWith("-l") || opt.startsWith("/l")) {
            TQString lib = opt.right(opt.length() - 2), out;
            if(!lib.isEmpty()) {
                for(MakefileDependDir *mdd = dirs.first(); mdd; mdd = dirs.next() ) {
		    TQString extension;
                    int ver = findHighestVersion(mdd->local_dir, lib);
		    if(ver > 0)
			extension += TQString::number(ver);
		    extension += ".lib";
		    if(TQMakeMetaInfo::libExists(mdd->local_dir + Option::dir_sep + lib) ||
		       TQFile::exists(mdd->local_dir + Option::dir_sep + lib + extension)) {
			out = mdd->real_dir + Option::dir_sep + lib + extension;
			break;
		    }
                }
            }
            if(out.isEmpty()) {
                remove = true; //just eat it since we cannot find one..
            } else {
		modified_opt = true;
                (*it) = out;
	    }
        } else if(!TQFile::exists(Option::fixPathToLocalOS(opt))) {
	    TQPtrList<MakefileDependDir> lib_dirs;
	    TQString file = opt;
            int slsh = file.findRev(Option::dir_sep);
            if(slsh != -1) {
                TQString r = file.left(slsh+1), l = r;
		fixEnvVariables(l);
		lib_dirs.append(new MakefileDependDir(r.replace("\"",""), l.replace("\"","")));
                file = file.right(file.length() - slsh - 1);
            } else {
		lib_dirs = dirs;
	    }
#if 0
	    if (!project->variables()["QMAKE_QT_DLL"].isEmpty()) {
		if(file.endsWith(".lib")) {
		    file = file.left(file.length() - 4);
		    if(!file.at(file.length()-1).isNumber()) {
			for(MakefileDependDir *mdd = lib_dirs.first(); mdd; mdd = lib_dirs.next() ) {
			    TQString lib_tmpl(file + "%1" + ".lib");
			    int ver = findHighestVersion(mdd->local_dir, file);
			    if(ver != -1) {
				if(ver)
				    lib_tmpl = lib_tmpl.arg(ver);
				else
				     lib_tmpl = lib_tmpl.arg("");
				if(slsh != -1) {
				    TQString dir = mdd->real_dir;
				    if(!dir.endsWith(Option::dir_sep))
					dir += Option::dir_sep;
				    lib_tmpl.prepend(dir);
				}
				modified_opt = true;
				(*it) = lib_tmpl;
				break;
			    }
			}
		    }
		}
	    }
#endif
        }
        if(remove) {
            it = l.remove(it);
        } else {
	    if(!quote.isNull() && modified_opt)
		(*it) = quote + (*it) + quote;
            ++it;
	}
    }
    return true;
}

void
Win32MakefileGenerator::processPrlFiles()
{
    TQDict<void> processed;
    TQPtrList<MakefileDependDir> libdirs;
    libdirs.setAutoDelete(true);
    {
	TQStringList &libpaths = project->variables()["QMAKE_LIBDIR"];
	for(TQStringList::Iterator libpathit = libpaths.begin(); libpathit != libpaths.end(); ++libpathit) {
	    TQString r = (*libpathit), l = r;
	    fixEnvVariables(l);
	    libdirs.append(new MakefileDependDir(r.replace("\"",""),
						 l.replace("\"","")));
	}
    }
    for(bool ret = false; true; ret = false) {
	//read in any prl files included..
	TQStringList l_out;
	TQString where = "QMAKE_LIBS";
	if(!project->isEmpty("QMAKE_INTERNAL_PRL_LIBS"))
	    where = project->first("QMAKE_INTERNAL_PRL_LIBS");
	TQStringList &l = project->variables()[where];
	for(TQStringList::Iterator it = l.begin(); it != l.end(); ++it) {
	    TQString opt = (*it);
	    if(opt.startsWith("/")) {
		if(opt.startsWith("/LIBPATH:")) {
		    TQString r = opt.mid(9), l = r;
		    fixEnvVariables(l);
		    libdirs.append(new MakefileDependDir(r.replace("\"",""),
							 l.replace("\"","")));
		}
	    } else {
		if(!processed[opt]) {
		    if(processPrlFile(opt)) {
			processed.insert(opt, (void*)1);
			ret = true;
		    } else {
			for(MakefileDependDir *mdd = libdirs.first(); mdd; mdd = libdirs.next() ) {
			    TQString prl = mdd->local_dir + Option::dir_sep + opt;
			    if(processed[prl]) {
				break;
			    } else if(processPrlFile(prl)) {
				processed.insert(prl, (void*)1);
				ret = true;
				break;
			    }
			}
		    }
		}
	    }
	    if(!opt.isEmpty())
		l_out.append(opt);
	}
	if(ret)
	    l = l_out;
	else
	    break;
    }
}
