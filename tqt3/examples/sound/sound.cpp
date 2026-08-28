/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/
//
// Very simple example of TQSound::play(filename)
//
// 99% of this program is just boilerplate TQt code to put up a nice
// window so you think something special is happening.
//

#include "sound.h"
#include <ntqapplication.h>
#include <ntqmessagebox.h>
#include <ntqmenubar.h>

SoundPlayer::SoundPlayer() :
    TQMainWindow(),
    bucket3("sounds/3.wav"),
    bucket4("sounds/4.wav")
{
    if (!TQSound::isAvailable()) {
	// Bail out.  Programs in which sound is not critical
	// could just silently (hehe) ignore the lack of a server.
	//
	TQMessageBox::warning(this,"No Sound",
		"<p><b>Sorry, you are not running the Network Audio System.</b>"
		"<p>If you have the `au' command, run it in the background before this program. "
		"The latest release of the Network Audio System can be obtained from:"
		"<pre>\n"
		" &nbsp;\n"
		"   ftp.ncd.com:/pub/ncd/technology/src/nas\n"
		"   ftp.x.org:/contrib/audio/nas\n"
		"</pre>"
		"<p>Release 1.2 of NAS is also included with the X11R6"
		"contrib distribution."
		"<p>After installing NAS, you will then need to reconfigure TQt with NAS sound support");
    }

    TQPopupMenu *file = new TQPopupMenu;
    file->insertItem("Play &1",  this, TQ_SLOT(doPlay1()), CTRL+Key_1);
    file->insertItem("Play &2",  this, TQ_SLOT(doPlay2()), CTRL+Key_2);
    file->insertItem("Play from bucket &3",  this, TQ_SLOT(doPlay3()), CTRL+Key_3);
    file->insertItem("Play from bucket &4",  this, TQ_SLOT(doPlay4()), CTRL+Key_4);
    file->insertSeparator();
    file->insertItem("Play 3 and 4 together",  this, TQ_SLOT(doPlay34()));
    file->insertItem("Play all together",  this, TQ_SLOT(doPlay1234()));
    file->insertSeparator();
    file->insertItem("E&xit",  tqApp, TQ_SLOT(quit()));
    menuBar()->insertItem("&File", file);
}

void SoundPlayer::doPlay1()
{
    TQSound::play("sounds/1.wav");
}

void SoundPlayer::doPlay2()
{
    TQSound::play("sounds/2.wav");
}

void SoundPlayer::doPlay3()
{
    bucket3.play();
}

void SoundPlayer::doPlay4()
{
    bucket4.play();
}

void SoundPlayer::doPlay34()
{
    // Some sound platforms will only play one sound at a time
    bucket3.play();
    bucket4.play();
}

void SoundPlayer::doPlay1234()
{
    // Some sound platforms will only play one sound at a time
    TQSound::play("sounds/1.wav");
    TQSound::play("sounds/2.wav");
    bucket3.play();
    bucket4.play();
}

int main(int argc, char** argv)
{
    TQApplication app(argc,argv);
    SoundPlayer sp;
    app.setMainWidget(&sp);
    sp.setCaption("TQt Example - Sounds");
    sp.show();
    return app.exec();
}

