/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include <ntqapplication.h>
#include <ntqfiledialog.h>
#include <ntqpushbutton.h>
#include <ntqlabel.h>
#include <ntqpainter.h>
#include <ntqmessagebox.h>
#include <ntqmovie.h>
#include <ntqvbox.h>


class MovieScreen : public TQFrame {
    TQ_OBJECT
    TQMovie movie;
    TQString filename;
    TQSize sh;

public:
    MovieScreen(const char* fname, TQMovie m, TQWidget* p=0, const char* name=0, WFlags f=0) :
        TQFrame(p, name, f),
	sh(100,100)
    {
        setCaption(fname);
        filename = fname;
        movie = m;

        // Set a frame around the movie.
        setFrameStyle(TQFrame::WinPanel|TQFrame::Sunken);

        // No background needed, since we draw on the whole widget.
        movie.setBackgroundColor(backgroundColor());
        setBackgroundMode(NoBackground);

        // Get the movie to tell use when interesting things happen.
        movie.connectUpdate(this, TQ_SLOT(movieUpdated(const TQRect&)));
        movie.connectResize(this, TQ_SLOT(movieResized(const TQSize&)));
        movie.connectStatus(this, TQ_SLOT(movieStatus(int)));

	setSizePolicy(TQSizePolicy(TQSizePolicy::Expanding,TQSizePolicy::Expanding));
    }

    TQSize sizeHint() const
    {
	return sh;
    }

protected:

    // Draw the contents of the TQFrame - the movie and on-screen-display
    void drawContents(TQPainter* p)
    {
        // Get the current movie frame.
        TQPixmap pm = movie.framePixmap();

        // Get the area we have to draw in.
        TQRect r = contentsRect();

	if ( !pm.isNull() ) {
	    // Only rescale is we need to - it can take CPU!
	    if ( r.size() != pm.size() ) {
		TQWMatrix m;
		m.scale((double)r.width()/pm.width(),
			(double)r.height()/pm.height());
		pm = pm.xForm(m);
	    }

	    // Draw the [possibly scaled] frame.  movieUpdated() below calls
	    // repaint with only the changed area, so clipping will ensure we
	    // only do the minimum amount of rendering.
	    //
	    p->drawPixmap(r.x(), r.y(), pm);
	}


        // The on-screen display

        const char* message = 0;

        if (movie.paused()) {
            message = "PAUSED";
        } else if (movie.finished()) {
            message = "THE END";
        } else if (movie.steps() > 0) {
            message = "FF >>";
        }

        if (message) {
            // Find a good font size...
            p->setFont(TQFont("Helvetica", 24));

            TQFontMetrics fm = p->fontMetrics();
            if ( fm.width(message) > r.width()-10 )
                p->setFont(TQFont("Helvetica", 18));

            fm = p->fontMetrics();
            if ( fm.width(message) > r.width()-10 )
                p->setFont(TQFont("Helvetica", 14));

            fm = p->fontMetrics();
            if ( fm.width(message) > r.width()-10 )
                p->setFont(TQFont("Helvetica", 12));

            fm = p->fontMetrics();
            if ( fm.width(message) > r.width()-10 )
                p->setFont(TQFont("Helvetica", 10));

            // "Shadow" effect.
            p->setPen(black);
            p->drawText(1, 1, width()-1, height()-1, AlignCenter, message);
            p->setPen(white);
            p->drawText(0, 0, width()-1, height()-1, AlignCenter, message);
        }
    }

public slots:
    void restart()
    {
	movie.restart();
        repaint();
    }

    void togglePause()
    {
	if ( movie.paused() )
	    movie.unpause();
	else
	    movie.pause();
        repaint();
    }

    void step()
    {
	movie.step();
        repaint();
    }

    void step10()
    {
	movie.step(10);
        repaint();
    }

private slots:
    void movieUpdated(const TQRect& area)
    {
        if (!isVisible())
            show();

        // The given area of the movie has changed.

        TQRect r = contentsRect();

        if ( r.size() != movie.framePixmap().size() ) {
            // Need to scale - redraw whole frame.
            repaint( r );
        } else {
            // Only redraw the changed area of the frame
            repaint( area.x()+r.x(), area.y()+r.x(),
                     area.width(), area.height() );
        }
    }

    void movieResized(const TQSize& size)
    {
        // The movie changed size, probably from its initial zero size.

        int fw = frameWidth();
        sh = TQSize( size.width() + fw*2, size.height() + fw*2 );
	updateGeometry();
	if ( parentWidget() && parentWidget()->isHidden() )
	    parentWidget()->show();
    }

    void movieStatus(int status)
    {
        // The movie has sent us a status message.

        if (status < 0) {
	    TQString msg;
	    msg.sprintf("Could not play movie \"%s\"", (const char*)filename);
	    TQMessageBox::warning(this, "movies", msg);
	    parentWidget()->close();
        } else if (status == TQMovie::Paused || status == TQMovie::EndOfMovie) {
            repaint(); // Ensure status text is displayed
        }
    }
};

class MoviePlayer : public TQVBox {
    MovieScreen* movie;
public:
    MoviePlayer(const char* fname, TQMovie m, TQWidget* p=0, const char* name=0, WFlags f=0) :
	TQVBox(p,name,f)
    {
	movie = new MovieScreen(fname, m, this);
	TQHBox* hb = new TQHBox(this);
	TQPushButton* btn;
	btn = new TQPushButton("<<", hb);
	connect(btn, TQ_SIGNAL(clicked()), movie, TQ_SLOT(restart()));
	btn = new TQPushButton("||", hb);
	connect(btn, TQ_SIGNAL(clicked()), movie, TQ_SLOT(togglePause()));
	btn = new TQPushButton(">|", hb);
	connect(btn, TQ_SIGNAL(clicked()), movie, TQ_SLOT(step()));
	btn = new TQPushButton(">>|", hb);
	connect(btn, TQ_SIGNAL(clicked()), movie, TQ_SLOT(step10()));
    }
};


// A TQFileDialog that chooses movies.
//
class MovieStarter: public TQFileDialog {
    TQ_OBJECT
public:
    MovieStarter(const char *dir);

public slots:
    void startMovie(const TQString& filename);
    // TQDialog's method - normally closes the file dialog.
    // We want it left open, and we want Cancel to quit everything.
    void done( int r );
};


MovieStarter::MovieStarter(const char *dir)
    : TQFileDialog(dir, "*.gif *.mng")
{
    //behave as in getOpenFilename
    setMode( ExistingFile );
    // When a file is selected, show it as a movie.
    connect(this, TQ_SIGNAL(fileSelected(const TQString&)),
	    this, TQ_SLOT(startMovie(const TQString&)));
}


void MovieStarter::startMovie(const TQString& filename)
{
    if ( filename ) // Start a new movie - have it delete when closed.
	(new MoviePlayer( filename, TQMovie(filename), 0, 0,
			       WDestructiveClose))->show();
}

void MovieStarter::done( int r )
{
    if (r != Accepted)
	tqApp->quit(); // end on Cancel
    setResult( r );

    // And don't hide.
}


int main(int argc, char **argv)
{
    TQApplication a(argc, argv);

    if (argc > 1) {
        // Commandline mode - show movies given on the command line
        //
	bool gui=true;
        for (int arg=1; arg<argc; arg++) {
	    if ( TQString(argv[arg]) == "-i" )
		gui = !gui;
	    else if ( gui )
		(void)new MoviePlayer(argv[arg], TQMovie(argv[arg]), 0, 0,
				      TQt::WDestructiveClose);
	    else
		(void)new MovieScreen(argv[arg], TQMovie(argv[arg]), 0, 0,
				      TQt::WDestructiveClose);
	}
        TQObject::connect(tqApp, TQ_SIGNAL(lastWindowClosed()), tqApp, TQ_SLOT(quit()));
    } else {
        // "GUI" mode - open a chooser for movies
        //
        MovieStarter* fd = new MovieStarter(".");
        fd->show();
    }

    // Go!
    return a.exec();
}

#include "main.moc"
