/****************************************************************************
**
** Implementation of ProjectGenerator class.
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

#include "projectgenerator.h"
#include "option.h"
#include <ntqdir.h>
#include <ntqfile.h>
#include <ntqfileinfo.h>
#include <ntqregexp.h>

TQString project_builtin_regx() //calculate the builtin regular expression..
{ 
    TQString ret;
    TQStringList builtin_exts(".c");
    builtin_exts << Option::ui_ext << Option::yacc_ext << Option::lex_ext << ".ts";
    builtin_exts += Option::h_ext + Option::cpp_ext;
    for(TQStringList::Iterator ext_it = builtin_exts.begin(); 
	ext_it != builtin_exts.end(); ++ext_it) {
	if(!ret.isEmpty())
	    ret += "; ";
	ret += TQString("*") + (*ext_it);
    }
    return ret;
}



ProjectGenerator::ProjectGenerator(TQMakeProject *p) : MakefileGenerator(p), init_flag(false)
{
}

void
ProjectGenerator::init()
{
    if(init_flag)
	return;
    int file_count = 0;
    init_flag = true;

    TQMap<TQString, TQStringList> &v = project->variables();
    TQString templ = Option::user_template.isEmpty() ? TQString("app") : Option::user_template;
    if(!Option::user_template_prefix.isEmpty())
	templ.prepend(Option::user_template_prefix);
    v["TEMPLATE_ASSIGN"] += templ;

    //figure out target
    if(Option::output.name() == "-" || Option::output.name().isEmpty())
	v["TARGET"] = TQStringList("unknown");

    //the scary stuff
    if(project->first("TEMPLATE_ASSIGN") != "subdirs") {
	TQString builtin_regex = project_builtin_regx();
	TQStringList dirs = Option::projfile::project_dirs;
	if(Option::projfile::do_pwd) {
	    if(!v["INCLUDEPATH"].contains("."))
		v["INCLUDEPATH"] += ".";
	    dirs.prepend(TQDir::currentDirPath());
	}

	for(TQStringList::Iterator pd = dirs.begin(); pd != dirs.end(); pd++) {
	    TQString dir, regex;
	    bool add_depend = false;
	    if(TQFile::exists((*pd))) {
		TQFileInfo fi((*pd));
		if(fi.isDir()) {
		    dir = (*pd);
		    add_depend = true;
		    if(dir.right(1) != Option::dir_sep)
			dir += Option::dir_sep;
		    if(Option::projfile::do_recursive) {
			TQDir d(dir);
			d.setFilter(TQDir::Dirs);
			for(int i = 0; i < (int)d.count(); i++) {
			    if(d[i] != "." && d[i] != "..") 
				dirs.append(dir + d[i] + TQDir::separator() + builtin_regex);
			}
		    }
		    regex = builtin_regex;
		} else {
		    TQString file = (*pd);
		    int s = file.findRev(Option::dir_sep);
		    if(s != -1)
			dir = file.left(s+1);
		    if(addFile(file)) {
			add_depend = true;
			file_count++;
		    }
		}
	    } else { //regexp
		regex = (*pd);
	    }
	    if(!regex.isEmpty()) {
		int s = regex.findRev(Option::dir_sep);
		if(s != -1) {
		    dir = regex.left(s+1);
		    regex = regex.right(regex.length() - (s+1));
		}
		if(Option::projfile::do_recursive) {
		    TQDir d(dir);
		    d.setFilter(TQDir::Dirs);
		    for(int i = 0; i < (int)d.count(); i++) {
			if(d[i] != "." && d[i] != "..") 
			    dirs.append(dir + d[i] + TQDir::separator() + regex);
		    }
		}
		TQDir d(dir, regex);
		for(int i = 0; i < (int)d.count(); i++) {
		    TQString file = dir + d[i];
		    if (addFile(file)) {
			add_depend = true;
			file_count++;
		    }
		}
	    }
	    if(add_depend && !dir.isEmpty() && !v["DEPENDPATH"].contains(dir)) {
		TQFileInfo fi(dir);
		if(fi.absFilePath() != TQDir::currentDirPath()) 
		    v["DEPENDPATH"] += fileFixify(dir);
	    }
	}
    }
    if(!file_count) { //shall we try a subdir?
	TQStringList dirs = Option::projfile::project_dirs;
	if(Option::projfile::do_pwd)
	    dirs.prepend(".");
	const TQString out_file = fileFixify(Option::output.name());
	for(TQStringList::Iterator pd = dirs.begin(); pd != dirs.end(); pd++) {
	    if(TQFile::exists((*pd))) {
		TQString newdir = (*pd);
		TQFileInfo fi(newdir);
		if(fi.isDir()) {
		    newdir = fileFixify(newdir);
		    TQStringList &subdirs = v["SUBDIRS"];
		    if(TQFile::exists(fi.filePath() + TQDir::separator() + fi.fileName() + ".pro") &&
		       !subdirs.contains(newdir)) {
			subdirs.append(newdir);
		    } else {
			TQDir d(newdir, "*.pro");
			d.setFilter(TQDir::Files);
			for(int i = 0; i < (int)d.count(); i++) {
			    TQString nd = newdir;
			    if(nd == ".")
				nd = "";
			    else if(!nd.isEmpty() && !nd.endsWith(TQString(TQChar(TQDir::separator()))))
				nd += TQDir::separator();
			    nd += d[i];
			    fileFixify(nd);
			    if(d[i] != "." && d[i] != ".." && !subdirs.contains(nd) && !out_file.endsWith(nd)) 
				subdirs.append(nd);
			}
		    }
		    if(Option::projfile::do_recursive) {
			TQDir d(newdir);
			d.setFilter(TQDir::Dirs);
			for(int i = 0; i < (int)d.count(); i++) {
			    TQString nd = fileFixify(newdir + TQDir::separator() + d[i]);
			    if(d[i] != "." && d[i] != ".." && !dirs.contains(nd))
				dirs.append(nd);
			}
		    }
		}
	    } else { //regexp
		TQString regx = (*pd), dir;
		int s = regx.findRev(Option::dir_sep);
		if(s != -1) {
		    dir = regx.left(s+1);
		    regx = regx.right(regx.length() - (s+1));
		}
		TQDir d(dir, regx);
		d.setFilter(TQDir::Dirs);
		TQStringList &subdirs = v["SUBDIRS"];
		for(int i = 0; i < (int)d.count(); i++) {
		    TQString newdir(dir + d[i]);
		    TQFileInfo fi(newdir);
		    if(fi.fileName() != "." && fi.fileName() != "..") {
			newdir = fileFixify(newdir);
			if(TQFile::exists(fi.filePath() + TQDir::separator() + fi.fileName() + ".pro") &&
			   !subdirs.contains(newdir)) {
			   subdirs.append(newdir);
			} else {
			    TQDir d(newdir, "*.pro");
			    d.setFilter(TQDir::Files);
			    for(int i = 0; i < (int)d.count(); i++) {
				TQString nd = newdir + TQDir::separator() + d[i];
				fileFixify(nd);
				if(d[i] != "." && d[i] != ".." && !subdirs.contains(nd)) {
				    if(newdir + d[i] != Option::output_dir + Option::output.name())
					subdirs.append(nd);
				}
			    }
			}
			if(Option::projfile::do_recursive && !dirs.contains(newdir))
			    dirs.append(newdir);
		    }
		}
	    }
	}
	v["TEMPLATE_ASSIGN"] = "subdirs";
	return;
    }

    TQPtrList<MakefileDependDir> deplist;
    deplist.setAutoDelete(true);
    {
	TQStringList &d = v["DEPENDPATH"];
	for(TQStringList::Iterator it = d.begin(); it != d.end(); ++it) {
	    TQString r = (*it), l = Option::fixPathToLocalOS((*it));
	    deplist.append(new MakefileDependDir(r, l));
	}
    }
    TQStringList &h = v["HEADERS"];
    bool no_qt_files = true;
    TQString srcs[] = { "SOURCES", "YACCSOURCES", "LEXSOURCES", "INTERFACES", TQString::null };
    for(int i = 0; !srcs[i].isNull(); i++) {
	TQStringList &l = v[srcs[i]];
	for(TQStringList::Iterator val_it = l.begin(); val_it != l.end(); ++val_it) {
	    if(generateDependencies(deplist, (*val_it), true)) {
		TQStringList &tmp = findDependencies((*val_it));
		if(!tmp.isEmpty()) {
		    for(TQStringList::Iterator dep_it = tmp.begin(); dep_it != tmp.end(); ++dep_it) {
			TQString file_dir = (*dep_it).section(Option::dir_sep, 0, -2),
			    file_no_path = (*dep_it).section(Option::dir_sep, -1);
			if(!file_dir.isEmpty()) {
			    for(MakefileDependDir *mdd = deplist.first(); mdd; mdd = deplist.next()) {
				if(mdd->local_dir == file_dir && !v["INCLUDEPATH"].contains(mdd->real_dir))
				    v["INCLUDEPATH"] += mdd->real_dir;
			    }
			}
			if(no_qt_files && file_no_path.find(TQRegExp("^q[a-z_0-9].h$")) != -1)
			    no_qt_files = false;
			TQString h_ext;
			for(TQStringList::Iterator hit = Option::h_ext.begin(); 
			    hit != Option::h_ext.end(); ++hit) {
			    if((*dep_it).endsWith((*hit))) {
				h_ext = (*hit);
				break;
			    }
			}
			if(!h_ext.isEmpty()) {
			    if((*dep_it).left(1).lower() == "q") {
				TQString qhdr = (*dep_it).lower();
				if(file_no_path == "ntqthread.h")
				    addConfig("thread");
			    }
			    for(TQStringList::Iterator cppit = Option::cpp_ext.begin();
				cppit != Option::cpp_ext.end(); ++cppit) {
				TQString src((*dep_it).left((*dep_it).length() - h_ext.length()) + 
					    (*cppit));
				if(TQFile::exists(src)) {
				    bool exists = false;
				    TQStringList &srcl = v["SOURCES"];
				    for(TQStringList::Iterator src_it = srcl.begin(); 
					src_it != srcl.end(); ++src_it) {
					if((*src_it).lower() == src.lower()) {
					    exists = true;
					    break;
					}
				    }
				    if(!exists)
					srcl.append(src);
				}
			    }
			} else if((*dep_it).endsWith(Option::lex_ext) &&
				  file_no_path.startsWith(Option::lex_mod)) {
			    addConfig("lex_included");
			}
			if(!h.contains((*dep_it))) {
			    if(generateMocList((*dep_it)) && !findMocDestination((*dep_it)).isEmpty())
				h += (*dep_it);
			}
		    }
		}
	    }
	}
    }
    if(h.isEmpty())
	addConfig("moc", false);

    //if we find a file that matches an forms it needn't be included in the project
    TQStringList &u = v["INTERFACES"];
    TQString no_ui[] = { "SOURCES", "HEADERS", TQString::null };
    {
	for(int i = 0; !no_ui[i].isNull(); i++) {
	    TQStringList &l = v[no_ui[i]];
	    for(TQStringList::Iterator val_it = l.begin(); val_it != l.end(); ) {
		bool found = false;
		for(TQStringList::Iterator ui_it = u.begin(); ui_it != u.end(); ++ui_it) {
		    TQString s1 = (*val_it).right((*val_it).length() - ((*val_it).findRev(Option::dir_sep) + 1));
		    if(s1.findRev('.') != -1)
			s1 = s1.left(s1.findRev('.')) + Option::ui_ext;
		    TQString u1 = (*ui_it).right((*ui_it).length() - ((*ui_it).findRev(Option::dir_sep) + 1));
		    if(s1 == u1) {
			found = true;
			break;
		    }
		}
		if(!found && (*val_it).endsWith(Option::cpp_moc_ext))
		    found = true;
		if(found)
		    val_it = l.remove(val_it);
		else
		    ++val_it;
	    }
	}
    }
}


bool
ProjectGenerator::writeMakefile(TQTextStream &t)
{
    t << "######################################################################" << endl;
    t << "# Automatically generated by qmake (" << qmake_version() << ") " << endl;
    t << "######################################################################" << endl << endl;
    TQStringList::Iterator it;
    for(it = Option::before_user_vars.begin(); it != Option::before_user_vars.end(); ++it)
	t << (*it) << endl;
    t << getWritableVar("TEMPLATE_ASSIGN", false);
    if(project->first("TEMPLATE_ASSIGN") == "subdirs") {
	t << endl << "# Directories" << "\n"
	  << getWritableVar("SUBDIRS");
    } else {
	t << getWritableVar("TARGET")
	  << getWritableVar("CONFIG", false)
	  << getWritableVar("CONFIG_REMOVE", false)
	  << getWritableVar("DEPENDPATH")
	  << getWritableVar("INCLUDEPATH") << endl;

	t << "# Input" << "\n";
	t << getWritableVar("HEADERS") 
	  << getWritableVar("INTERFACES") 
	  << getWritableVar("LEXSOURCES") 
	  << getWritableVar("YACCSOURCES") 
	  << getWritableVar("SOURCES")
	  << getWritableVar("TRANSLATIONS");
    }
    for(it = Option::after_user_vars.begin(); it != Option::after_user_vars.end(); ++it)
	t << (*it) << endl;
    return true;
}

bool
ProjectGenerator::addConfig(const TQString &cfg, bool add)
{
    TQString where = "CONFIG";
    if(!add)
	where = "CONFIG_REMOVE";
    if(!project->variables()[where].contains(cfg)) {
	project->variables()[where] += cfg;
	return true;
    }
    return false;
}


bool
ProjectGenerator::addFile(TQString file)
{
    file = fileFixify(file, TQDir::currentDirPath());
    TQString dir;
    int s = file.findRev(Option::dir_sep);
    if(s != -1)
	dir = file.left(s+1);
    if(file.mid(dir.length(), Option::h_moc_mod.length()) == Option::h_moc_mod)
	return false;

    TQString where;
    for(TQStringList::Iterator cppit = Option::cpp_ext.begin(); cppit != Option::cpp_ext.end(); ++cppit) {
	if(file.endsWith((*cppit))) {
	    if(TQFile::exists(file.left(file.length() - (*cppit).length()) + Option::ui_ext))
		return false;
	    else
		where = "SOURCES";
	    break;
	}
    }
    if(where.isEmpty()) {
	for(TQStringList::Iterator hit = Option::h_ext.begin(); hit != Option::h_ext.end(); ++hit) {
	    if(file.endsWith((*hit))) {
		where = "HEADERS";
		break;
	    }
	}
    }
    if(where.isEmpty()) {
	if(file.endsWith(Option::ui_ext))
	    where = "INTERFACES";
	else if(file.endsWith(".c"))
	    where = "SOURCES";
	else if(file.endsWith(Option::lex_ext))
	    where = "LEXSOURCES";
	else if(file.endsWith(Option::yacc_ext))
	    where = "YACCSOURCES";
	else if(file.endsWith(".ts"))
	    where = "TRANSLATIONS";
    }

    TQString newfile = fileFixify(file);
    if(!where.isEmpty() && !project->variables()[where].contains(file)) {
	project->variables()[where] += newfile;
	return true;
    }
    return false;
}


TQString
ProjectGenerator::getWritableVar(const TQString &v, bool fixPath)
{
    TQStringList &vals = project->variables()[v];
    if(vals.isEmpty())
	return "";

    TQString ret;
    if(v.endsWith("_REMOVE"))
	ret = v.left(v.length() - 7) + " -= ";
    else if(v.endsWith("_ASSIGN"))
	ret = v.left(v.length() - 7) + " = ";
    else
	ret = v + " += ";
    TQString join = vals.join(" ");
    if(ret.length() + join.length() > 80) {
	TQString spaces;
	for(unsigned int i = 0; i < ret.length(); i++)
	    spaces += " ";
	join = vals.join(" \\\n" + spaces);
    } 
#if 0
    // ### Commented out for now so that project generation works.
    // Sam: it had to do with trailing \'s (ie considered continuation lines)
    if(fixPath)
	join = join.replace("\\", "/");
#else
    Q_UNUSED(fixPath);
#endif
    return ret + join + "\n";
}

bool
ProjectGenerator::openOutput(TQFile &file) const
{
    TQString outdir;
    if(!file.name().isEmpty()) {
	TQFileInfo fi(file);
	if(fi.isDir())
	    outdir = fi.dirPath() + TQDir::separator();
    }
    if(!outdir.isEmpty() || file.name().isEmpty()) {
	TQString dir = TQDir::currentDirPath();
	int s = dir.findRev('/');
	if(s != -1)
	    dir = dir.right(dir.length() - (s + 1));
	file.setName(outdir + dir + ".pro");
    }
    return MakefileGenerator::openOutput(file);
}
