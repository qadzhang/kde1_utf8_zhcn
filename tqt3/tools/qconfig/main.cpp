/**********************************************************************
** Copyright (C) 1999-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of TQt/Embedded.
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

#include "main.h"
#include <ntqapplication.h>
#include <ntqfile.h>
#include <ntqtextstream.h>
#include <ntqlistview.h>
#include <ntqhbox.h>
#include <ntqlabel.h>
#include <ntqmessagebox.h>
#include <ntqpopupmenu.h>
#include <ntqmenubar.h>
#include <ntqtoolbutton.h>
#include <ntqstatusbar.h>
#include <ntqsplitter.h>
#include <ntqregexp.h>
#if defined(Q_OS_WIN32)
#include <direct.h>
#else
#include <unistd.h>
#endif
#include <stdlib.h>


/* XPM */
static const char * const logo_xpm[] = {
/* width height ncolors chars_per_pixel */
"50 50 17 1",
/* colors */
"  c #000000",
". c #495808",
"X c #2A3304",
"o c #242B04",
"O c #030401",
"+ c #9EC011",
"@ c #93B310",
"# c #748E0C",
"$ c #A2C511",
"% c #8BA90E",
"& c #99BA10",
"* c #060701",
"= c #181D02",
"- c #212804",
"; c #61770A",
": c #0B0D01",
"/ c None",
/* pixels */
"/$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$/",
"$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$",
"$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$",
"$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$",
"$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$",
"$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$",
"$$$$$$$$$$$$$$$$$$$$$$$+++$$$$$$$$$$$$$$$$$$$$$$$$",
"$$$$$$$$$$$$$$$$$$$@;.o=::=o.;@$$$$$$$$$$$$$$$$$$$",
"$$$$$$$$$$$$$$$$+#X*         **X#+$$$$$$$$$$$$$$$$",
"$$$$$$$$$$$$$$$#oO*         O  **o#+$$$$$$$$$$$$$$",
"$$$$$$$$$$$$$&.* OO              O*.&$$$$$$$$$$$$$",
"$$$$$$$$$$$$@XOO            * OO    X&$$$$$$$$$$$$",
"$$$$$$$$$$$@XO OO  O  **:::OOO OOO   X@$$$$$$$$$$$",
"$$$$$$$$$$&XO      O-;#@++@%.oOO      X&$$$$$$$$$$",
"$$$$$$$$$$.O  :  *-#+$$$$$$$$+#- : O O*.$$$$$$$$$$",
"$$$$$$$$$#*OO  O*.&$$$$$$$$$$$$+.OOOO **#$$$$$$$$$",
"$$$$$$$$+-OO O *;$$$$$$$$$$$&$$$$;*     o+$$$$$$$$",
"$$$$$$$$#O*  O .+$$$$$$$$$$@X;$$$+.O    *#$$$$$$$$",
"$$$$$$$$X*    -&$$$$$$$$$$@- :;$$$&-    OX$$$$$$$$",
"$$$$$$$@*O  *O#$$$$$$$$$$@oOO**;$$$#    O*%$$$$$$$",
"$$$$$$$;     -+$$$$$$$$$@o O OO ;+$$-O   *;$$$$$$$",
"$$$$$$$.     ;$$$$$$$$$@-OO OO  X&$$;O    .$$$$$$$",
"$$$$$$$o    *#$$$$$$$$@o  O O O-@$$$#O   *o$$$$$$$",
"$$$$$$+=    *@$$$$$$$@o* OO   -@$$$$&:    =$$$$$$$",
"$$$$$$+:    :+$$$$$$@-      *-@$$$$$$:    :+$$$$$$",
"$$$$$$+:    :+$$$$$@o* O    *-@$$$$$$:    :+$$$$$$",
"$$$$$$$=    :@$$$$@o*OOO      -@$$$$@:    =+$$$$$$",
"$$$$$$$-    O%$$$@o* O O    O O-@$$$#*   OX$$$$$$$",
"$$$$$$$. O *O;$$&o O*O* *O      -@$$;    O.$$$$$$$",
"$$$$$$$;*   Oo+$$;O*O:OO--      Oo@+=    *;$$$$$$$",
"$$$$$$$@*  O O#$$$;*OOOo@@-O     Oo;O*  **@$$$$$$$",
"$$$$$$$$X* OOO-+$$$;O o@$$@-    O O     OX$$$$$$$$",
"$$$$$$$$#*  * O.$$$$;X@$$$$@-O O        O#$$$$$$$$",
"$$$$$$$$+oO O OO.+$$+&$$$$$$@-O         o+$$$$$$$$",
"$$$$$$$$$#*    **.&$$$$$$$$$$@o      OO:#$$$$$$$$$",
"$$$$$$$$$+.   O* O-#+$$$$$$$$+;O    OOO:@$$$$$$$$$",
"$$$$$$$$$$&X  *O    -;#@++@#;=O    O    -@$$$$$$$$",
"$$$$$$$$$$$&X O     O*O::::O      OO    Oo@$$$$$$$",
"$$$$$$$$$$$$@XOO                  OO    O*X+$$$$$$",
"$$$$$$$$$$$$$&.*       **  O      ::    *:#$$$$$$$",
"$$$$$$$$$$$$$$$#o*OO       O    Oo#@-OOO=#$$$$$$$$",
"$$$$$$$$$$$$$$$$+#X:* *     O**X#+$$@-*:#$$$$$$$$$",
"$$$$$$$$$$$$$$$$$$$%;.o=::=o.#@$$$$$$@X#$$$$$$$$$$",
"$$$$$$$$$$$$$$$$$$$$$$$$+++$$$$$$$$$$$+$$$$$$$$$$$",
"$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$",
"$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$",
"$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$",
"$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$",
"$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$",
"/$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$/",
};


#define FIXED_LAYOUT

static const char*back_xpm[]={
"16 16 5 1",
"# c #000000",
"a c #ffffff",
"c c #808080",
"b c #c0c0c0",
". c None",
"................",
".......#........",
"......##........",
".....#a#........",
"....#aa########.",
"...#aabaaaaaaa#.",
"..#aabbbbbbbbb#.",
"...#abbbbbbbbb#.",
"...c#ab########.",
"....c#a#ccccccc.",
".....c##c.......",
"......c#c.......",
".......cc.......",
"........c.......",
"................",
"......................"};

static const char*forward_xpm[]={
"16 16 5 1",
"# c #000000",
"a c #ffffff",
"c c #808080",
"b c #c0c0c0",
". c None",
"................",
"................",
".........#......",
".........##.....",
".........#a#....",
"..########aa#...",
"..#aaaaaaabaa#..",
"..#bbbbbbbbbaa#.",
"..#bbbbbbbbba#..",
"..########ba#c..",
"..ccccccc#a#c...",
"........c##c....",
"........c#c.....",
"........cc......",
"........c.......",
"................",
"................"};

class ChoiceItem : public TQCheckListItem {
public:
    TQString id;
    ChoiceItem(const TQString& i, TQListViewItem* parent) :
	TQCheckListItem(parent,
	    i.mid(6), // strip "TQT_NO_" as we reverse the logic
	    CheckBox),
	id(i)
    {
	setOpen(true);
	label = text(0);
	avl = true;
    }

    // We reverse the logic
    void setDefined(bool y) { setOn(!y); }
    bool isDefined() const { return !isOn(); }

    void setAvailable(bool y)
    {
	if ( avl != y ) {
	    avl = y;
	    repaint();
	}
    }
    bool isAvailable() const { return avl; }

    virtual void setOn(bool y)
    {
	TQCheckListItem::setOn(y);
	setOpen(y);
/*
	for (TQListViewItem* i=firstChild(); i; i = i->nextSibling() ) {
	    ChoiceItem* ci = (ChoiceItem*)i; // all are ChoiceItem
	    if ( ci->isSelectable() != y ) {
		ci->setSelectable(y);
		listView()->repaintItem(ci);
	    }
	}
*/
    }

    void paintBranches( TQPainter * p, const TQColorGroup & cg,
                            int w, int y, int h)
    {
	TQListViewItem::paintBranches(p,cg,w,y,h);
    }

    void paintCell( TQPainter * p, const TQColorGroup & cg,
                               int column, int width, int align )
    {
	if ( !isSelectable() || !isAvailable() ) {
	    TQColorGroup c = cg;
	    c.setColor(TQColorGroup::Text, lightGray);
	    TQCheckListItem::paintCell(p,c,column,width,align);
	} else {
	    TQCheckListItem::paintCell(p,cg,column,width,align);
	}
    }

    void setInfo(const TQString& l, const TQString& d)
    {
	label = l;
	doc = d;
	setText(0,label);
    }

    TQString label;

    TQString info() const
    {
	return "<h2>"+label+"</h2>"+doc;
    }

private:
    TQString doc;
    bool avl;
};

Main::Main()
{
    setIcon( (const char**)logo_xpm );
#ifdef FIXED_LAYOUT
    TQHBox* horizontal = new TQHBox(this);
#else
    TQSplitter* horizontal = new TQSplitter(this);
#endif

    lv = new TQListView(horizontal);
    lv->setSorting(-1);
    lv->setRootIsDecorated(true);
    lv->addColumn("ID");

    info = new Info(horizontal);
    info->setBackgroundMode(PaletteBase);
    info->setMargin(10);
    info->setFrameStyle(TQFrame::WinPanel|TQFrame::Sunken);
    info->setAlignment(AlignTop);

    connect(info, TQ_SIGNAL(idClicked(const TQString&)),
	    this, TQ_SLOT(selectId(const TQString&)));

#ifdef FIXED_LAYOUT
    horizontal->setStretchFactor(info,2);
#endif

    connect(lv, TQ_SIGNAL(pressed(TQListViewItem*)),
	    this, TQ_SLOT(updateAvailability(TQListViewItem*)));
    connect(lv, TQ_SIGNAL(selectionChanged(TQListViewItem*)),
	    this, TQ_SLOT(showInfo(TQListViewItem*)));

    setCentralWidget(horizontal);

    TQToolBar* tb = new TQToolBar( this, "browser controls" );
    tb->setLabel( "Browser Controls" );
    (void)new TQToolButton( TQPixmap(back_xpm), "Back", TQString::null,
                           info, TQ_SLOT(back()), tb, "back" );
    (void)new TQToolButton( TQPixmap(forward_xpm), "Forward", TQString::null,
                           info, TQ_SLOT(forward()), tb, "forward" );

    TQPopupMenu* file = new TQPopupMenu( menuBar() );
    file->insertItem( "&Open",  this, TQ_SLOT(open()), CTRL+Key_O );
    file->insertItem( "&Save", this, TQ_SLOT(save()), CTRL+Key_S );
    file->insertSeparator();
    file->insertItem( "&Test all", this, TQ_SLOT(testAll()), CTRL+Key_T );
    file->insertSeparator();
    file->insertItem( "E&xit",  tqApp, TQ_SLOT(quit()), CTRL+Key_Q );

    menuBar()->insertItem( "&File",file );

    menuBar()->insertSeparator();

    TQPopupMenu *help = new TQPopupMenu( menuBar() );
    help->insertItem( "&About", this, TQ_SLOT(about()) );
    help->insertItem( "About &TQt", this, TQ_SLOT(aboutTQt()) );

    menuBar()->insertItem( "&Help", help );

    statusBar()->message( "Ready" );
}

void Main::open()
{
}

void Main::save()
{
}

void Main::testAll()
{
    TQString qtdir = getenv("TQTDIR");
    chdir((qtdir+"/src").ascii());
    TQString c;
    for (TQStringList::ConstIterator it = choices.begin(); it != choices.end(); ++it)
    {
	c += "Feature: ";
	c += *it;
	c += "\n";

	c += "Section: ";
	c += section[*it];
	c += "\n";

	c += "Requires: ";
	c += dependencies[*it].join(" ");
	c += "\n";

	c += "Name: ";
	c += label[*it];
	c += "\n";

	c += "SeeAlso: ???\n";

	c += "\n";
    }
    TQFile f("features.txt");
    f.open(IO_WriteOnly);
    f.writeBlock(c.ascii(),c.length());
    f.close();
    // system("./feature_size_calculator");

#if 0
    system("mv ../include/ntqconfig.h ../include/ntqconfig.h-orig");
    for (TQStringList::ConstIterator it = choices.begin(); it != choices.end(); ++it)
    {
	TQString choice = *it;
	TQFile f("../include/ntqconfig.h");
	f.open(IO_WriteOnly);
	TQCString s = "#define ";
	s += choice.latin1();
	s += "\n";
	f.writeBlock(s,s.length());
	f.close();
	int err = system("make");
	if ( err != 0 )
	    break;
    }
    system("mv ../include/ntqconfig.h-orig ../include/ntqconfig.h");
#endif
}


// ##### should be in TQMap?
template <class K, class D>
TQValueList<K> keys(TQMap<K,D> map)
{
    TQValueList<K> result;
    for (TQ_TYPENAME TQMap<K,D>::ConstIterator it = map.begin(); it!=map.end(); ++it)
	result.append(it.key());
    return result;
}

void Main::loadFeatures(const TQString& filename)
{
    TQFile file(filename);
    if ( !file.open(IO_ReadOnly) ) {
	TQMessageBox::warning(this,"Warning",
			     "Cannot open file " + filename);
	return;
    }
    TQTextStream s(&file);
    TQRegExp qt_no_xxx("TQT_NO_[A-Z_0-9]*");
    TQStringList sections;

#if 1
    TQString line = s.readLine();
    TQString feature, lab, sec;
    TQStringList deps, seealso;
    TQMap<TQString,TQStringList> sectioncontents;
    while (!s.atEnd()) {
	if ( line.length() <= 1 ) {
	    if ( !feature.isEmpty() ) {
		dependencies[feature] = deps;
		for (TQStringList::ConstIterator it = deps.begin(); it!=deps.end(); ++it)
		    rdependencies[*it].append(feature);
		label[feature] = lab;
		links[feature] = seealso;
		section[feature] = sec;
		sectioncontents[sec].append(feature);
		choices.append(feature);
	    } else {
		tqDebug("Unparsed text");
	    }

	    feature = lab = sec = TQString::null;
	    deps.clear(); seealso.clear();
	    line = s.readLine();
	    continue;
	}

	TQString nextline = s.readLine();
	while ( nextline[0] == ' ' ) {
	    line += nextline;
	    nextline = s.readLine();
	}

	int colon = line.find(':');
	if ( colon < 0 ) {
	    tqDebug("Cannot parse: %s",line.ascii());
	} else {
	    TQString tag = line.left(colon);
	    TQString value = line.mid(colon+1).stripWhiteSpace();
	    if ( tag == "Feature" )
		feature = value;
	    else if ( tag == "Requires" )
		deps = TQStringList::split(TQChar(' '),value);
	    else if ( tag == "Name" )
		lab = value;
	    else if ( tag == "Section" )
		sec = value;
	    else if ( tag == "SeeAlso" )
		seealso = TQStringList::split(TQChar(' '),value);
	}

	line = nextline;
    }
    sections = keys(sectioncontents);

#else
    TQString sec;
    TQString lab;
    TQString doc;
    bool on = false;
    bool docmode = false;
    TQStringList deps;

    do {
	TQString line = s.readLine();
	line.replace(TQRegExp("# *define"),"#define");

	TQStringList token = TQStringList::split(TQChar(' '),line);
	if ( on ) {
	    if ( docmode ) {
		if ( token[0] == "*/" )
		    docmode = false;
		else if ( lab.isEmpty() )
		    lab = line.stripWhiteSpace();
		else
		    doc += line.simplifyWhiteSpace() + "\n";
	    } else if ( token[0] == "//#define" || token[0] == "#define" ) {
		dependencies[token[1]] = deps;
		for (TQStringList::ConstIterator it = deps.begin(); it!=deps.end(); ++it)
		    rdependencies[*it].append(token[1]);
		section[token[1]] = sec;
		documentation[token[1]] = doc;
		label[token[1]] = lab;
		choices.append(token[1]);
		doc = "";
		lab = "";
	    } else if ( token[0] == "/*!" ) {
		docmode = true;
	    } else if ( token[0] == "//" ) {
		token.remove(token.begin());
		sec = token.join(" ");
		sections.append(sec);
	    } else if ( token[0] == "#if" ) {
		Q_ASSERT(deps.isEmpty());
		for (int i=1; i<(int)token.count(); i++) {
		    if ( token[i][0] == 'd' ) {
			int index;
			int len;
			index = qt_no_xxx.match(token[i],0,&len);
			if ( index >= 0 ) {
			    TQString d = token[i].mid(index,len);
			    deps.append(d);
			}
		    }
		}
	    } else if ( token[0] == "#endif" ) {
		deps.clear();
	    } else if ( token[0].isEmpty() ) {
	    } else {
		tqDebug("Cannot parse: %s",token.join(" ").ascii());
	    }
	} else if ( token[0] == "#include" ) {
	    on = true;
	}
    } while (!s.atEnd());
#endif

    lv->clear();
    sections.sort();
    // ##### TQListView default sort order is reverse of insertion order
    for (TQStringList::Iterator se = sections.fromLast(); se != sections.end(); --se) {
	sectionitem[*se] = new TQListViewItem(lv,*se);
    }
    for (TQStringList::Iterator ch = choices.begin(); ch != choices.end(); ++ch) {
	createItem(*ch);
    }

#ifdef FIXED_LAYOUT
    lv->setFixedWidth(lv->sizeHint().width());
#endif
}

void Main::createItem(const TQString& ch)
{
    if ( !item[ch] ) {
	TQStringList deps = dependencies[ch];
	TQString sec = section[ch];
	TQListViewItem* parent = 0;
	for (TQStringList::Iterator dp = deps.begin(); dp != deps.end(); ++dp) {
	    TQString dsec = section[*dp];
	    if ( dsec.isEmpty() )
		tqDebug("No section for %s",(*dp).latin1());
	    if ( !parent && dsec == sec ) {
		createItem(*dp);
		parent = item[*dp];
	    }
	}
	if ( !parent )
	    parent = sectionitem[section[ch]];
	ChoiceItem* ci = new ChoiceItem(ch,parent);
	item[ch] = ci;
	if ( !label[ch].isEmpty() )
	    ci->setInfo(label[ch],documentation[ch]);
    }
}

void Main::loadConfig(const TQString& filename)
{
    TQFile file(filename);
    if ( !file.open(IO_ReadOnly) ) {
	TQMessageBox::warning(this,"Warning",
			     "Cannot open file " + filename);
	return;
    }
    TQTextStream s(&file);
    TQRegExp qt_no_xxx("TQT_NO_[A-Z_0-9]*");

    for (TQStringList::Iterator ch = choices.begin(); ch != choices.end(); ++ch) {
	item[*ch]->setDefined(false);
    }
    do {
	TQString line = s.readLine();
	TQStringList token = TQStringList::split(TQChar(' '),line);
	if ( token[0] == "#define" ) {
	    ChoiceItem* i = item[token[1]];
	    if ( i )
		i->setDefined(true);
	    else
		tqDebug("The item %s is not used by ntqfeatures.h", token[1].latin1());
	}
    } while (!s.atEnd());
}

void Main::updateAvailability(TQListViewItem* i)
{
    if ( !i || !i->parent() ) {
        // section. do nothing for now
    } else {
        ChoiceItem* choice = (ChoiceItem*)i;
	TQStringList deps = rdependencies[choice->id];
	for (TQStringList::ConstIterator it = deps.begin();
		it != deps.end(); ++it)
	{
	    ChoiceItem* d = item[*it];
	    TQStringList ddeps = dependencies[d->id];
	    bool av = true;
	    for (TQStringList::ConstIterator dit = ddeps.begin();
		    av && dit != ddeps.end(); ++dit)
	    {
		ChoiceItem* dd = item[*dit];
		if ( dd ) {
		    if ( dd->isDefined() || !dd->isAvailable() )
			av = false;
		} else
		    tqDebug("%s ???",(*dit).latin1());
	    }
	    if ( d->isAvailable() != av ) {
		d->setAvailable(av);
		updateAvailability(d);
	    }
	}
tqDebug("%s: %d",choice->id.latin1(),choice->isAvailable());
    }
}

void Main::showInfo(TQListViewItem* i)
{
    if ( !i )
	return;
    if ( !i->parent() ) {
	// section. do nothing for now
    } else {
	ChoiceItem* choice = (ChoiceItem*)i;
	TQString i = choice->info();
	TQStringList deps = dependencies[choice->id];
	if ( !deps.isEmpty() ) {
	    i += "<h3>Requires:</h3><ul>";
	    for (TQStringList::ConstIterator it = deps.begin();
		    it != deps.end(); ++it)
	    {
		ChoiceItem* d = item[*it];
		if ( d ) {
		    bool got = d->isAvailable() && !d->isDefined();
		    i += "<li>";
		    if ( !got ) i += "<font color=red>";
		    i += "<a href=id://"+d->id+">"+d->label+"</a>";
		    if ( !got ) i += "</font>";
		}
	    }
	    i += "</ul>";
	}
	TQStringList rdeps = rdependencies[choice->id];
	if ( !rdeps.isEmpty() ) {
	    i += "<h3>Required for:</h3><ul>";
	    for (TQStringList::ConstIterator it = rdeps.begin();
		    it != rdeps.end(); ++it)
	    {
		ChoiceItem* d = item[*it];
		if ( d )
		    i += "<li><a href=id://"+d->id+">"+d->label+"</a>";
	    }
	    i += "</ul>";
	}
	info->setText(i);
    }
}

void Main::selectId(const TQString& id)
{
    TQListViewItem* it = item[id];
    if ( it ) {
	lv->setSelected(it,true);
	lv->ensureItemVisible(it);
    }
}

Info::Info( TQWidget* parent, const char* name ) : 
    TQTextBrowser(parent, name)
{
}

void Info::setSource(const TQString& name)
{
    if ( name.left(5) == "id://" ) {
	emit idClicked(name.mid(5,name.length()-6)); // skip trailing "/" too
    } else {
	TQTextBrowser::setSource(name);
    }
}

void Main::about()
{
    TQMessageBox::about(this, "About qconfig",
	"<p><b><font size=+2>TQt/Embedded build configuration</font></b></p>"
	"<p></p>"
	"<p>Version 1.0</p>"
	"<p>Copyright (C) 2001-2008 Trolltech ASA. All rights reserved.</p>"
	"<p></p>"
	"<p>This program is licensed to you under the terms of the GNU General "
	"Public License Version 2 as published by the Free Software Foundation. This "
	"gives you legal permission to copy, distribute and/or modify this software "
	"under certain conditions. For details, see the file 'LICENSE.GPL' that came with "
	"this software distribution. If you did not get the file, send email to "
	"info@trolltech.com.</p>\n\n<p>The program is provided AS IS with NO WARRANTY "
	"OF ANY KIND, INCLUDING THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS "
	"FOR A PARTICULAR PURPOSE.</p>"
    );
}

void Main::aboutTQt()
{
    TQMessageBox::aboutTQt( this, tr("qconfig") );
}

int main(int argc, char** argv)
{
    TQApplication app(argc,argv);
    Main m;
    TQString qtdir = getenv("TQTDIR");
    TQString qfeatures = qtdir + "/src/tools/qfeatures.txt";
    //TQString qfeatures = qtdir + "/include/ntqfeatures.h";
    TQString qconfig = qtdir + "/include/ntqconfig.h";
    for (int i=1; i<argc; i++) {
	TQString arg = argv[i];
	if ( arg == "-f" && i+i<argc ) {
	    qfeatures = argv[++i];
	} else {
	    qconfig = argv[i];
	}
    }
    m.loadFeatures(qfeatures);
    m.loadConfig(qconfig);
    m.resize(m.sizeHint()+TQSize(500,300));
    app.setMainWidget(&m);
    m.show();
    return app.exec();
}
