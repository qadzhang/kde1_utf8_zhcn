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

#include <ntqapplication.h>
#include <ntqtextstream.h>
#include <ntqscrollview.h>
#include <ntqfile.h>
#include <ntqfont.h>
#include <ntqvbox.h>
#include <ntqlabel.h>
#include <ntqstringlist.h>
#include <ntqlistview.h>
#include <ntqmainwindow.h>
#include <ntqmessagebox.h>
#include <ntqpainter.h>
#ifdef TQ_WS_QWS
#include <qmemorymanager_qws.h>
#endif

#include <cstdlib>


class FontViewItem : public TQListViewItem {
    TQString family;
    int pointSize;
    int weight;
    bool italic;
    TQFont font;

public:
    FontViewItem(const TQString& f, int pt, int w, bool ital, TQListView* parent) :
	TQListViewItem(parent),
	family(f), pointSize(pt), weight(w), italic(ital)
    {
    }

    void renderAndSave()
    {
	font = TQFont(family,pointSize,weight,italic);
#ifdef TQ_WS_QWS
	memorymanager->savePrerenderedFont((TQMemoryManager::FontID)font.handle());
#endif
	setHeight(TQFontMetrics(font).lineSpacing());
	repaint();
    }

    void render(int from, int to)
    {
	font = TQFont(family,pointSize,weight,italic);
#ifdef TQ_WS_QWS
	for (int i=from; i<=to; i++) {
	    if ( memorymanager->inFont((TQMemoryManager::FontID)font.handle(),TQChar(i))) {
		memorymanager->lockGlyph((TQMemoryManager::FontID)font.handle(),TQChar(i));
		memorymanager->unlockGlyph((TQMemoryManager::FontID)font.handle(),TQChar(i));
	    }
	}
#endif
    }

    void save()
    {
	font = TQFont(family,pointSize,weight,italic);
#ifdef TQ_WS_QWS
	memorymanager->savePrerenderedFont((TQMemoryManager::FontID)font.handle(),false);
#endif
	setHeight(TQFontMetrics(font).lineSpacing());
	repaint();
    }

    void paintCell( TQPainter *p, const TQColorGroup & cg,
                            int column, int width, int alignment )
    {
	p->setFont(font);
	TQListViewItem::paintCell(p,cg,column,width,alignment);
    }

    int width( const TQFontMetrics&,
                       const TQListView* lv, int column) const
    {
	TQFontMetrics fm(font);
	return TQListViewItem::width(fm,lv,column);
    }

    TQString text(int col) const
    {
	switch (col) {
	    case 0:
		return family;
	    case 1:
		return TQString::number(pointSize)+"pt";
	    case 2:
		if ( weight < TQFont::Normal ) {
		    return "Light";
		} else if ( weight >= TQFont::Black ) {
		    return "Black";
		} else if ( weight >= TQFont::Bold ) {
		    return "Bold";
		} else if ( weight >= TQFont::DemiBold ) {
		    return "DemiBold";
		} else {
		    return "Normal";
		}
	    case 3:
		return italic ? "Italic" : "Roman";
	}
	return TQString::null;
    }
};

class MakeTQPF : public TQMainWindow
{
    TQ_OBJECT
    TQListView* view;
public:
    MakeTQPF()
    {
	view = new TQListView(this);
	view->addColumn("Family");
	view->addColumn("Size");
	view->addColumn("Weight");
	view->addColumn("Style");
	setCentralWidget(view);
	TQString fontdir = TQString(getenv("TQTDIR")) + "/lib/fonts";
	readFontDir(fontdir);

	connect(view,TQ_SIGNAL(selectionChanged(TQListViewItem*)),
	    this,TQ_SLOT(renderAndSave(TQListViewItem*)));
    }

    void readFontDir(const TQString& fntd)
    {
	TQString fontdir = fntd + "/fontdir";
	TQFile fd(fontdir);
	if ( !fd.open(IO_ReadOnly) ) {
	    TQMessageBox::warning(this, "Read Error",
		"<p>Cannot read "+fontdir);
	    return;
	}
	while ( !fd.atEnd() ) {
	    TQString line;
	    fd.readLine(line,9999);
	    if ( line[0] != '#' ) {
		TQStringList attr = TQStringList::split(" ",line);
		if ( attr.count() >= 7 ) {
		    TQString family = attr[0];
		    int weight = TQString(attr[4]).toInt();
		    bool italic = TQString(attr[3]) == "y";
		    TQStringList sizes = attr[5];
		    if ( sizes[0] == "0" ) {
			if ( attr[7].isNull() )
			    sizes = TQStringList::split(',',attr[6]);
			else
			    sizes = TQStringList::split(',',attr[7]);
		    }
		    for (TQStringList::Iterator it = sizes.begin(); it != sizes.end(); ++it) {
			int pointSize = (*it).toInt()/10;
			if ( pointSize )
			    new FontViewItem(
				family, pointSize, weight, italic, view);
		    }
		}
	    }
	}
    }

    void renderAndSaveAll()
    {
	TQListViewItem* c = view->firstChild();
	while ( c ) {
	    renderAndSave(c);
	    tqApp->processEvents();
	    c = c->nextSibling();
	}
    }

    void renderAndSave(const TQString& family)
    {
	TQListViewItem* c = view->firstChild();
	while ( c ) {
	    if ( c->text(0).lower() == family.lower() )
		renderAndSave(c);
	    c = c->nextSibling();
	}
    }

    void render(const TQString& family, int from, int to)
    {
	TQListViewItem* c = view->firstChild();
	while ( c ) {
	    if ( c->text(0).lower() == family.lower() )
		((FontViewItem*)c)->render(from,to);
	    c = c->nextSibling();
	}
    }

    void save(const TQString& family)
    {
	TQListViewItem* c = view->firstChild();
	while ( c ) {
	    if ( c->text(0).lower() == family.lower() )
		((FontViewItem*)c)->save();
	    c = c->nextSibling();
	}
    }

private slots:
    void renderAndSave(TQListViewItem* i)
    {
	((FontViewItem*)i)->renderAndSave();
    }
};

static void usage()
{
    tqWarning("Usage: maketqpf [-A] [-f spec-file] [font ...]");
    tqWarning("");
    tqWarning("   Saves TQPF font files by rendering and saving fonts.");
    tqWarning("");
    tqWarning("         -A = Render and save all fonts in fontdir");
    tqWarning("         -f = File of lines:");
    tqWarning("                fontname character-ranges");
    tqWarning("              eg.");
    tqWarning("                smoothtimes 0-ff,20a0-20af");
    tqWarning("       font = Font to render and save");
}

int main(int argc, char** argv)
{
    TQApplication app(argc, argv, TQApplication::GuiServer);
    MakeTQPF m;
    if ( argc > 1 ) {
	argv++;
	TQString specfile;
	while ( *argv && argv[0][0] == '-' ) {
	    if (argv[0][1] == 'A' )
		m.renderAndSaveAll();
	    else if (argv[0][1] == 'f' )
		specfile = *++argv;
	    else {
		usage();
		specfile=TQString::null;
		*argv=0;
		break;
	    }
	    argv++;
	}
	if ( !specfile.isNull() ) {
	    TQFile f(specfile);
	    if ( f.open(IO_ReadOnly) ) {
		TQTextStream s(&f);
		while (!s.atEnd()) {
		    TQString family,ranges;
		    s >> family >> ranges;
		    TQStringList r = TQStringList::split(',',ranges);
		    for (TQStringList::Iterator it=r.begin(); it!=r.end(); ++it) {
			TQString rng = *it;
			int dash = rng.find('-');
			int from,to;
			if ( dash==0 ) {
			    from=0;
			    to=rng.mid(1).toInt(0,16);
			} else if ( dash==(int)rng.length()-1 ) {
			    from=rng.left(dash).toInt(0,16);
			    to=65535;
			} else if ( dash<0 ) {
			    from=to=rng.toInt(0,16);
			} else {
			    from=rng.left(dash).toInt(0,16);
			    to=rng.mid(dash+1).toInt(0,16);
			}
			m.render(family,from,to);
		    }
		    m.save(family);
		}
	    } else {
		tqWarning("Cannot open %s",specfile.latin1());
	    }
	} else {
	    while (*argv)
		m.renderAndSave(*argv++);
	}
    } else {
	// Interactive
	app.setMainWidget(&m);
	m.show();
	return app.exec();
    }
}

#include "main.moc"
