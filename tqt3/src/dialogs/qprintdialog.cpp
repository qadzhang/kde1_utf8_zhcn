/****************************************************************************
**
** Implementation of internal print dialog (X11) used by TQPrinter::select().
**
** Created : 950829
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of the dialogs module of the TQt GUI Toolkit.
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

#include "ntqprintdialog.h"

#ifndef TQT_NO_PRINTDIALOG

#include "ntqfiledialog.h"
#include "ntqfile.h"
#include "ntqtextstream.h"
#include "ntqcombobox.h"
#include "ntqframe.h"
#include "ntqlabel.h"
#include "ntqlineedit.h"
#include "ntqpushbutton.h"
#include "ntqprinter.h"
#include "ntqlistview.h"
#include "ntqlayout.h"
#include "ntqbuttongroup.h"
#include "ntqradiobutton.h"
#include "ntqspinbox.h"
#include "ntqapplication.h"
#include "ntqheader.h"
#include "ntqstyle.h"
#include "ntqstring.h"
#include "ntqregexp.h"
#if !defined(TQT_NO_CUPS) || !defined(TQT_NO_NIS)
#include "ntqlibrary.h"
#endif

#ifndef TQT_NO_NIS

#ifndef BOOL_DEFINED
#define BOOL_DEFINED
#endif

#include <rpcsvc/ypclnt.h>
#include <rpcsvc/yp_prot.h>

// Solaris redefines connect -> __xnet_connect with _XOPEN_SOURCE_EXTENDED.
#if defined(connect)
# undef connect
#endif

#endif // TQT_NO_NIS

// UNIX Large File Support redefines open -> open64
#if defined(open)
# undef open
#endif

#include <ctype.h>
#include <stdlib.h>


class TQPrintDialogSpinBox : public TQSpinBox
{
public:
    TQPrintDialogSpinBox(int min, int max, int steps, TQWidget *parent, const char *name)
	: TQSpinBox(min, max, steps, parent, name)
    {}

    void interpretText()
    {
	TQSpinBox::interpretText();
    }
};




enum { Success = 's', Unavail = 'u', NotFound = 'n', TryAgain = 't' };
enum { Continue = 'c', Return = 'r' };

class TQPrintDialogPrivate
{
public:
    TQPrinter * printer;

    TQButtonGroup * printerOrFile;

    bool outputToFile;
    TQListView * printers;
    TQLineEdit * fileName;
    TQPushButton * browse, *ok;

    TQButtonGroup * printRange;
    TQLabel * firstPageLabel;
    TQPrintDialogSpinBox * firstPage;
    TQLabel * lastPageLabel;
    TQPrintDialogSpinBox * lastPage;
    TQRadioButton * printAllButton;
    TQRadioButton * printRangeButton;
    TQRadioButton * printSelectionButton;
    TQRadioButton * printToFileButton;
    TQComboBox *orientationCombo, *sizeCombo;

    TQPrinter::PageSize pageSize;
    TQPrinter::Orientation orientation;

    TQButtonGroup * pageOrder;
    TQPrinter::PageOrder pageOrder2;

    TQButtonGroup * colorMode;
    TQPrinter::ColorMode colorMode2;

    TQPrintDialogSpinBox * copies;
    int numCopies;

    TQBoxLayout *customLayout;

    TQPrinter::PageSize indexToPageSize[TQPrinter::NPageSize];
};


typedef void (*Q_PrintDialogHook)(TQListView *);
static Q_PrintDialogHook addPrinterHook = 0;

void qt_set_printdialog_hook( Q_PrintDialogHook hook )
{
    addPrinterHook = hook;
}

static void isc( TQPrintDialogPrivate * d, const TQString & text,
		 TQPrinter::PageSize ps );

class TQPrinterListViewItem : public TQListViewItem
{
public:
    TQPrinterListViewItem( TQListView * printers, const TQString& name,
			  const TQString& host, const TQString& comment,
			  const TQStringList& aliases )
	: TQListViewItem( printers, name, host, comment ), ali( aliases ) { }

    bool samePrinter( const TQString& name ) {
	return text( 0 ) == name || ali.find( name ) != ali.end();
    }

    TQStringList ali;
};

static void perhapsAddPrinter( TQListView * printers, const TQString &name,
			       TQString host, TQString comment,
			       TQStringList aliases = TQStringList() )
{
    const TQListViewItem * i = printers->firstChild();
    while ( i && !((TQPrinterListViewItem *) i)->samePrinter(name) )
	i = i->nextSibling();
    if ( i )
	return;
    if ( host.isEmpty() )
	host = TQPrintDialog::tr( "locally connected" );
    (void)new TQPrinterListViewItem( printers,
				    name.simplifyWhiteSpace(),
				    host.simplifyWhiteSpace(),
				    comment.simplifyWhiteSpace(), aliases );
}

static void parsePrinterDesc( TQString printerDesc, TQListView * printers )
{
    if ( printerDesc.length() < 1 )
	return;

    printerDesc = printerDesc.simplifyWhiteSpace();
    int i = printerDesc.find( ':' );
    TQString printerName, printerComment, printerHost;
    TQStringList aliases;

    if ( i >= 0 ) {
	// have ':' want '|'
	int j = printerDesc.find( '|' );
	if ( j > 0 && j < i ) {
	    printerName = printerDesc.left( j );
	    aliases = TQStringList::split( '|',
		    printerDesc.mid(j + 1, i - j - 1) );
	    // try extracting a comment from the aliases
	    printerComment = TQPrintDialog::tr( "Aliases: %1" )
			     .arg( aliases.join(", ") );
	} else {
	    printerName = printerDesc.left( i );
	}
	// look for lprng pseudo all printers entry
	i = printerDesc.find( TQRegExp(TQString::fromLatin1(": *all *=")) );
	if ( i >= 0 )
	    printerName = "";
	// look for signs of this being a remote printer
	i = printerDesc.find( TQRegExp(TQString::fromLatin1(": *rm *=")) );
	if ( i >= 0 ) {
	    // point k at the end of remote host name
	    while ( printerDesc[i] != '=' )
		i++;
	    while ( printerDesc[i] == '=' || printerDesc[i].isSpace() )
		i++;
	    j = i;
	    while ( j < (int)printerDesc.length() && printerDesc[j] != ':' )
		j++;

	    // and stuff that into the string
	    printerHost = printerDesc.mid( i, j - i );
	}
    }
    if ( printerName.length() )
	perhapsAddPrinter( printers, printerName, printerHost, printerComment,
			   aliases );
}

static int parsePrintcap( TQListView * printers, const TQString& fileName )
{
    TQFile printcap( fileName );
    if ( !printcap.open( IO_ReadOnly ) )
	return NotFound;

    char * line_ascii = new char[1025];
    line_ascii[1024] = '\0';

    TQString printerDesc;
    bool atEnd = false;

    while ( !atEnd ) {
	if ( printcap.atEnd() || printcap.readLine( line_ascii, 1024 ) <= 0 )
	    atEnd = true;
	TQString line = line_ascii;
	line = line.stripWhiteSpace();
	if ( line.length() >= 1 && line[int(line.length()) - 1] == '\\' )
	    line.truncate( line.length() - 1 );
	if ( line[0] == '#' ) {
	    if ( !atEnd )
		continue;
	} else if ( line[0] == '|' || line[0] == ':' ) {
	    printerDesc += line;
	    if ( !atEnd )
		continue;
	}

	parsePrinterDesc( printerDesc, printers );

	// add the first line of the new printer definition
	printerDesc = line;
    }
    delete[] line_ascii;
    return Success;
}


// solaris, not 2.6
static void parseEtcLpPrinters( TQListView * printers )
{
    TQDir lp( TQString::fromLatin1("/etc/lp/printers") );
    const TQFileInfoList * dirs = lp.entryInfoList();
    if ( !dirs )
	return;

    TQFileInfoListIterator it( *dirs );
    TQFileInfo *printer;
    TQString tmp;
    while ( (printer = it.current()) != 0 ) {
	++it;
	if ( printer->isDir() ) {
	    tmp.sprintf( "/etc/lp/printers/%s/configuration",
			 printer->fileName().ascii() );
	    TQFile configuration( tmp );
	    char * line = new char[1025];
	    TQString remote( TQString::fromLatin1("Remote:") );
	    TQString contentType( TQString::fromLatin1("Content types:") );
	    TQString printerHost;
	    bool canPrintPostscript = false;
	    if ( configuration.open( IO_ReadOnly ) ) {
		while ( !configuration.atEnd() &&
			configuration.readLine( line, 1024 ) > 0 ) {
		    if ( TQString::fromLatin1(line).startsWith( remote ) ) {
			const char * p = line;
			while ( *p != ':' )
			    p++;
			p++;
			while ( isspace((uchar) *p) )
			    p++;
			printerHost = TQString::fromLocal8Bit(p);
			printerHost = printerHost.simplifyWhiteSpace();
		    } else if ( TQString::fromLatin1(line).startsWith( contentType ) ) {
			char * p = line;
			while ( *p != ':' )
			    p++;
			p++;
			char * e;
			while ( *p ) {
			    while ( isspace((uchar) *p) )
				p++;
			    if ( *p ) {
				char s;
				e = p;
				while ( isalnum((uchar) *e) )
				    e++;
				s = *e;
				*e = '\0';
				if ( !qstrcmp( p, "postscript" ) ||
				     !qstrcmp( p, "any" ) )
				    canPrintPostscript = true;
				*e = s;
				if ( s == ',' )
				    e++;
				p = e;
			    }
			}
		    }
		}
		if ( canPrintPostscript )
		    perhapsAddPrinter( printers, printer->fileName(),
				       printerHost, TQString::fromLatin1("") );
	    }
	    delete[] line;
	}
    }
}


// solaris 2.6
static char * parsePrintersConf( TQListView * printers, bool *found = 0 )
{
    TQFile pc( TQString::fromLatin1("/etc/printers.conf") );
    if ( !pc.open( IO_ReadOnly ) ) {
	if ( found )
	    *found = false;
	return 0;
    }
    if ( found )
	*found = true;

    char * line = new char[1025];
    line[1024] = '\0';

    TQString printerDesc;
    int lineLength = 0;

    char * defaultPrinter = 0;

    while ( !pc.atEnd() &&
	    (lineLength=pc.readLine( line, 1024 )) > 0 ) {
	if ( *line == '#' ) {
	    *line = '\0';
	    lineLength = 0;
	}
	if ( lineLength >= 2 && line[lineLength-2] == '\\' ) {
	    line[lineLength-2] = '\0';
	    printerDesc += TQString::fromLocal8Bit(line);
	} else {
	    printerDesc += TQString::fromLocal8Bit(line);
	    printerDesc = printerDesc.simplifyWhiteSpace();
	    int i = printerDesc.find( ':' );
	    TQString printerName, printerHost, printerComment;
	    TQStringList aliases;
	    if ( i >= 0 ) {
		// have : want |
		int j = printerDesc.find( '|', 0 );
		if ( j >= i )
		    j = -1;
		printerName = printerDesc.mid( 0, j < 0 ? i : j );
		if ( printerName == TQString::fromLatin1("_default") ) {
		    i = printerDesc.find(
			TQRegExp( TQString::fromLatin1(": *use *=") ) );
		    while ( printerDesc[i] != '=' )
			i++;
		    while ( printerDesc[i] == '=' || printerDesc[i].isSpace() )
			i++;
		    j = i;
		    while ( j < (int)printerDesc.length() &&
			    printerDesc[j] != ':' && printerDesc[j] != ',' )
			j++;
		    // that's our default printer
		    defaultPrinter =
			tqstrdup( printerDesc.mid( i, j-i ).ascii() );
		    printerName = "";
		    printerDesc = "";
		} else if ( printerName == TQString::fromLatin1("_all") ) {
		    // skip it.. any other cases we want to skip?
		    printerName = "";
		    printerDesc = "";
		}

		if ( j > 0 ) {
		    // try extracting a comment from the aliases
		    aliases = TQStringList::split( '|',
			    printerDesc.mid(j + 1, i - j - 1) );
		    printerComment = TQPrintDialog::tr( "Aliases: %1" )
				     .arg( aliases.join(", ") );
		}
		// look for signs of this being a remote printer
		i = printerDesc.find(
		    TQRegExp( TQString::fromLatin1(": *bsdaddr *=") ) );
		if ( i >= 0 ) {
		    // point k at the end of remote host name
		    while ( printerDesc[i] != '=' )
			i++;
		    while ( printerDesc[i] == '=' || printerDesc[i].isSpace() )
			i++;
		    j = i;
		    while ( j < (int)printerDesc.length() &&
			    printerDesc[j] != ':' && printerDesc[j] != ',' )
			j++;
		    // and stuff that into the string
		    printerHost = printerDesc.mid( i, j-i );
		    // maybe stick the remote printer name into the comment
		    if ( printerDesc[j] == ',' ) {
			i = ++j;
			while ( printerDesc[i].isSpace() )
			    i++;
			j = i;
			while ( j < (int)printerDesc.length() &&
				printerDesc[j] != ':' && printerDesc[j] != ',' )
			    j++;
			if ( printerName != printerDesc.mid( i, j-i ) ) {
			    printerComment =
				TQString::fromLatin1("Remote name: ");
			    printerComment += printerDesc.mid( i, j-i );
			}
		    }
		}
	    }
	    if ( printerComment == ":" )
		printerComment = ""; // for cups
	    if ( printerName.length() )
		perhapsAddPrinter( printers, printerName, printerHost,
				   printerComment, aliases );
	    // chop away the line, for processing the next one
	    printerDesc = "";
	}
    }
    delete[] line;
    return defaultPrinter;
}

#ifndef TQT_NO_NIS

#if defined(Q_C_CALLBACKS)
extern "C" {
#endif

static int foreach( int /* status */, char * /* key */, int /* keyLen */,
		    char * val, int valLen, char * data )
{
    parsePrinterDesc( TQString::fromLatin1(val, valLen), (TQListView *) data );
    return 0;
}

#if defined(Q_C_CALLBACKS)
}
#endif

static int retrieveNisPrinters( TQListView * printers )
{
    typedef int (*WildCast)( int, char *, int, char *, int, char * );
    char printersConfByname[] = "printers.conf.byname";
    char *domain;
    int err;

    TQLibrary lib( "nsl" );
    typedef int (*ypGetDefaultDomain)(char **);
    ypGetDefaultDomain _ypGetDefaultDomain = (ypGetDefaultDomain)lib.resolve( "yp_get_default_domain" );
    typedef int (*ypAll)(const char *, const char *, const struct ypall_callback *);
    ypAll _ypAll = (ypAll)lib.resolve( "yp_all" );

    if ( _ypGetDefaultDomain && _ypAll ) {
	err = _ypGetDefaultDomain( &domain );
	if ( err == 0 ) {
	    ypall_callback cb;
	    // wild cast to support K&R-style system headers
	    (WildCast &) cb.foreach = (WildCast) foreach;
	    cb.data = (char *) printers;
	    err = _ypAll( domain, printersConfByname, &cb );
	}
	if ( !err )
	    return Success;
    }
    return Unavail;
}

#endif // TQT_NO_NIS

static char *parseNsswitchPrintersEntry( TQListView * printers, char *line )
{
#define skipSpaces() \
    while ( isspace((uchar) line[k]) ) \
	k++

    char *defaultPrinter = 0;
    bool stop = false;
    int lastStatus = NotFound;

    int k = 8;
    skipSpaces();
    if ( line[k] != ':' )
	return 0;
    k++;

    char *cp = strchr( line, '#' );
    if ( cp != 0 )
	*cp = '\0';

    while ( line[k] != '\0' ) {
	if ( isspace((uchar) line[k]) ) {
	    k++;
	} else if ( line[k] == '[' ) {
	    k++;
	    skipSpaces();
	    while ( line[k] != '\0' ) {
		char status = tolower( line[k] );
		char action = '?';

		while ( line[k] != '=' && line[k] != ']' &&
			line[k] != '\0' )
		    k++;
		if ( line[k] == '=' ) {
		    k++;
		    skipSpaces();
		    action = tolower( line[k] );
		    while ( line[k] != '\0' && !isspace((uchar) line[k]) && line[k] != ']' )
			k++;
		} else if ( line[k] == ']' ) {
		    k++;
		    break;
		}
		skipSpaces();

		if ( lastStatus == status )
		    stop = ( action == (char) Return );
	    }
	} else {
	    if ( stop )
		break;

	    TQCString source;
	    while ( line[k] != '\0' && !isspace((uchar) line[k]) && line[k] != '[' ) {
		source += line[k];
		k++;
	    }

	    if ( source == "user" ) {
		lastStatus = parsePrintcap( printers,
			TQDir::homeDirPath() + "/.printers" );
	    } else if ( source == "files" ) {
		bool found;
		defaultPrinter = parsePrintersConf( printers, &found );
		if ( found )
		    lastStatus = Success;
#ifndef TQT_NO_NIS
	    } else if ( source == "nis" ) {
		lastStatus = retrieveNisPrinters( printers );
#endif
	    } else {
		// nisplus, dns, etc., are not implemented yet
		lastStatus = NotFound;
	    }
	    stop = ( lastStatus == Success );
	}
    }
    return defaultPrinter;
}

static char *parseNsswitchConf( TQListView * printers )
{
    TQFile nc( TQString::fromLatin1("/etc/nsswitch.conf") );
    if ( !nc.open(IO_ReadOnly) )
	return 0;

    char *defaultPrinter = 0;

    char *line = new char[1025];
    line[1024] = '\0';

    while ( !nc.atEnd() &&
	    nc.readLine(line, 1024) > 0 ) {
	if ( strncmp(line, "printers", 8) == 0 ) {
	    defaultPrinter = parseNsswitchPrintersEntry( printers, line );
	    delete[] line;
	    return defaultPrinter;
	}
    }

    strcpy( line, "printers: user files nis nisplus xfn" );
    defaultPrinter = parseNsswitchPrintersEntry( printers, line );
    delete[] line;
    return defaultPrinter;
}

static void parseEtcLpMember( TQListView * printers )
{
    TQDir lp( TQString::fromLatin1("/etc/lp/member") );
    if ( !lp.exists() )
	return;
    const TQFileInfoList * dirs = lp.entryInfoList();
    if ( !dirs )
	return;

    TQFileInfoListIterator it( *dirs );
    TQFileInfo *printer;
    TQString tmp;
    while ( (printer = it.current()) != 0 ) {
	++it;
	// I haven't found any real documentation, so I'm guessing that
	// since lpstat uses /etc/lp/member rather than one of the
	// other directories, it's the one to use.  I did not find a
	// decent way to locate aliases and remote printers.
	if ( printer->isFile() )
	    perhapsAddPrinter( printers, printer->fileName(),
			       TQPrintDialog::tr("unknown"),
			       TQString::fromLatin1("") );
    }
}

// IRIX 6.x
static void parseSpoolInterface( TQListView * printers )
{
    TQDir lp( TQString::fromLatin1("/usr/spool/lp/interface") );
    if ( !lp.exists() )
	return;
    const TQFileInfoList * files = lp.entryInfoList();
    if( !files )
	return;

    TQFileInfoListIterator it( *files );
    TQFileInfo *printer;
    while ( (printer = it.current()) != 0) {
	++it;

	if ( !printer->isFile() )
	    continue;

	// parse out some information
	TQFile configFile( printer->filePath() );
	if ( !configFile.open( IO_ReadOnly ) )
	    continue;

	TQCString line( 1025 );
	TQString namePrinter;
	TQString hostName;
	TQString hostPrinter;
	TQString printerType;

	TQString nameKey( TQString::fromLatin1("NAME=") );
	TQString typeKey( TQString::fromLatin1("TYPE=") );
	TQString hostKey( TQString::fromLatin1("HOSTNAME=") );
	TQString hostPrinterKey( TQString::fromLatin1("HOSTPRINTER=") );

	while ( !configFile.atEnd() &&
		(configFile.readLine(line.data(), 1024)) > 0 ) {
	    TQString uline = line;
	    if ( uline.startsWith( typeKey )  ) {
		printerType = line.mid( nameKey.length() );
		printerType = printerType.simplifyWhiteSpace();
	    } else if ( uline.startsWith( hostKey ) ) {
		hostName = line.mid( hostKey.length() );
		hostName = hostName.simplifyWhiteSpace();
	    } else if ( uline.startsWith( hostPrinterKey ) ) {
		hostPrinter = line.mid( hostPrinterKey.length() );
		hostPrinter = hostPrinter.simplifyWhiteSpace();
	    } else if ( uline.startsWith( nameKey ) ) {
		namePrinter = line.mid( nameKey.length() );
		namePrinter = namePrinter.simplifyWhiteSpace();
	    }
	}
	configFile.close();

	printerType = printerType.stripWhiteSpace();
	if ( printerType.find("postscript", 0, false) < 0 )
	    continue;

	int ii = 0;
	while ( (ii = namePrinter.find('"', ii)) >= 0 )
	    namePrinter.remove( ii, 1 );

	if ( hostName.isEmpty() || hostPrinter.isEmpty() ) {
	    perhapsAddPrinter( printers, printer->fileName(),
			       TQString::fromLatin1(""), namePrinter );
	} else {
	    TQString comment;
	    comment = namePrinter;
	    comment += " (";
	    comment += hostPrinter;
	    comment += ")";
	    perhapsAddPrinter( printers, printer->fileName(),
			       hostName, comment );
	}
    }
}


// Every unix must have its own.  It's a standard.  Here is AIX.
static void parseQconfig( TQListView * printers )
{
    TQFile qconfig( TQString::fromLatin1("/etc/qconfig") );
    if ( !qconfig.open( IO_ReadOnly ) )
	return;

    TQTextStream ts( &qconfig );
    TQString line;

    TQString stanzaName; // either a queue or a device name
    bool up = true; // queue up?  default true, can be false
    TQString remoteHost; // null if local
    TQString deviceName; // null if remote

    TQRegExp newStanza( TQString::fromLatin1("^[0-z\\-]+:$") );

    // our basic strategy here is to process each line, detecting new
    // stanzas.  each time we see a new stanza, we check if the
    // previous stanza was a valid queue for a) a remote printer or b)
    // a local printer.  if it wasn't, we assume that what we see is
    // the start of the first stanza, or that the previous stanza was
    // a device stanza, or that there is some syntax error (we don't
    // report those).

    do {
	line = ts.readLine();
	bool indented = line[0].isSpace();
	line = line.simplifyWhiteSpace();

        int i = line.find('=');
        if ( indented && i != -1 ) { // line in stanza
	    TQString variable = line.left( i ).simplifyWhiteSpace();
	    TQString value=line.mid( i+1, line.length() ).simplifyWhiteSpace();
	    if ( variable == TQString::fromLatin1("device") )
		deviceName = value;
	    else if ( variable == TQString::fromLatin1("host") )
		remoteHost = value;
	    else if ( variable == TQString::fromLatin1("up") )
		up = !(value.lower() == TQString::fromLatin1("false"));
	} else if ( line[0] == '*' ) { // comment
	    // nothing to do
	} else if ( ts.atEnd() || // end of file, or beginning of new stanza
		    ( !indented && line.find( newStanza ) != -1 ) ) {
	    if ( up && stanzaName.length() > 0 && stanzaName.length() < 21 ) {
		if ( remoteHost.length() ) // remote printer
		    perhapsAddPrinter( printers, stanzaName, remoteHost,
                                       TQString::null );
		else if ( deviceName.length() ) // local printer
		    perhapsAddPrinter( printers, stanzaName, TQString::null,
                                       TQString::null );
	    }
	    line.truncate( line.length()-1 );
	    if ( line.length() >= 1 && line.length() <= 20 )
		stanzaName = line;
	    up = true;
	    remoteHost = TQString::null;
	    deviceName = TQString::null;
	} else {
	    // syntax error?  ignore.
	}
    } while ( !ts.atEnd() );
}


#ifndef TQT_NO_CUPS
#include <cups/cups.h>

static char * parseCupsOutput( TQListView * printers )
{
    char * defaultPrinter = 0;
    int nd;
    cups_dest_t * d;
    TQLibrary lib( "cups" );
    typedef int (*CupsGetDests)(cups_dest_t **dests);
    typedef void (*CupsFreeDests)(int num_dents, cups_dest_t *dests);
    CupsGetDests _cupsGetDests = (CupsGetDests)lib.resolve( "cupsGetDests" );
    CupsFreeDests _cupsFreeDests = (CupsFreeDests)lib.resolve( "cupsFreeDests" );
    if ( _cupsGetDests ) {
	nd = _cupsGetDests( &d );
	if ( nd < 1 )
	    return 0;

	int n = 0;
	while ( n < nd ) {
	    perhapsAddPrinter( printers, d[n].name,
			       TQPrintDialog::tr("Unknown Location"), 0 );
	    if ( d[n].is_default && !defaultPrinter )
		defaultPrinter = tqstrdup( d[n].instance );
	    n++;
	}
	if ( _cupsFreeDests ) {
	    _cupsFreeDests(nd, d);
	}
    }
    return defaultPrinter;
}
#endif

static TQPrintDialog * globalPrintDialog = 0;

static void qpd_cleanup_globaldialog()
{
    if ( globalPrintDialog != 0 )
	delete globalPrintDialog;
    globalPrintDialog = 0;
}

/*!
  \class TQPrintDialog ntqprintdialog.h

  \brief The TQPrintDialog class provides a dialog for specifying
  the printer's configuration.

  \internal

  \warning The use of this class is not recommended since it is not
  present on all platforms; use TQPrinter::setup() instead.

  \omit

  (ingroup dialogs)

  THIS DOCUMENTATION IS Not Revised. It must be revised before
  becoming public API.

  It encompasses both the sort of details needed for doing a simple
  print-out and some print configuration setup.

  The easiest way to use the class is through the static
  function getPrinterSetup().  You can also subclass the TQPrintDialog
  and add some custom buttons with addButton() to extend the
  functionality of the print dialog.

  <img src="qprintdlg-m.png"><br clear=all>
  The printer dialog, on a large screen, in Motif style.
*/


/*! Constructs a new modal printer dialog that configures \a prn and is a
  child of \a parent named \a name.
*/

TQPrintDialog::TQPrintDialog( TQPrinter *prn, TQWidget *parent, const char *name )
    : TQDialog( parent, name, true )
{
    d = new TQPrintDialogPrivate;
    d->numCopies = 1;

    TQBoxLayout * tll = new TQBoxLayout( this, TQBoxLayout::Down, 12, 0 );

    // destination
    TQGroupBox * g;
    g = setupDestination();
    tll->addWidget( g, 1 );

    tll->addSpacing( 12 );

    // printer and paper settings
    TQBoxLayout * lay = new TQBoxLayout( TQBoxLayout::LeftToRight );
    tll->addLayout( lay );

    g = setupPrinterSettings();
    lay->addWidget( g, 1 );

    lay->addSpacing( 12 );

    g = setupPaper();
    lay->addWidget( g );

    tll->addSpacing( 12 );

    // options
    g = setupOptions();
    tll->addWidget( g );
    tll->addSpacing( 12 );

    TQBoxLayout *l = new TQBoxLayout( TQBoxLayout::LeftToRight );
    d->customLayout = new TQBoxLayout( TQBoxLayout::LeftToRight );
    tll->addLayout( l );
    l->addLayout( d->customLayout );
    l->addStretch();
    tll->addSpacing( 12 );

    // buttons
    TQBoxLayout *horiz = new TQBoxLayout( TQBoxLayout::LeftToRight );
    tll->addLayout( horiz );

    bool rightalign =
	bool(style().styleHint(TQStyle::SH_PrintDialog_RightAlignButtons, this));

    if (rightalign)
	horiz->addStretch( 1 );

    d->ok = new TQPushButton( this, "ok" );
    d->ok->setText( tr("OK") );
    d->ok->setDefault( true );
    horiz->addWidget( d->ok );
    if (! rightalign)
	horiz->addStretch( 1 );
    horiz->addSpacing( 6 );

    TQPushButton * cancel = new TQPushButton( this, "cancel" );
    cancel->setText( tr("Cancel") );
    horiz->addWidget( cancel );

    TQSize s1 = d->ok->sizeHint();
    TQSize s2 = cancel->sizeHint();
    s1 = TQSize( TQMAX(s1.width(), s2.width()),
		TQMAX(s1.height(), s2.height()) );

    d->ok->setFixedSize( s1 );
    cancel->setFixedSize( s1 );

    tll->activate();

    connect( d->ok, TQ_SIGNAL(clicked()), TQ_SLOT(okClicked()) );
    connect( cancel, TQ_SIGNAL(clicked()), TQ_SLOT(reject()) );

    TQSize ms( minimumSize() );
    TQSize ss( TQApplication::desktop()->screenGeometry( pos() ).size() );
    if ( ms.height() < 512 && ss.height() >= 600 )
	ms.setHeight( 512 );
    else if ( ms.height() < 460 && ss.height() >= 480 )
	ms.setHeight( 460 );
    resize( ms );

    setPrinter( prn, true );
    d->printers->setFocus();
}


/*! Destroys the object and frees any allocated resources.  Does not
  delete the associated TQPrinter object.
*/

TQPrintDialog::~TQPrintDialog()
{
    if ( this == globalPrintDialog )
	globalPrintDialog = 0;
    delete d;
}

/*!
  This method allows you to specify a global print dialog, given in \a
  pd, that will be used instead of the default dialog provided by TQt.

  This is useful, since there are many different printing systems on
  Unix, and we cannot support all of them. Calling this method before
  using a printer for the first time allows you to set up your own
  print dialog.

  \sa setupPrinters()
*/
void TQPrintDialog::setGlobalPrintDialog( TQPrintDialog *pd )
{
    TQPrintDialog *oldPd = globalPrintDialog;
    globalPrintDialog = pd;
    if ( oldPd )
	delete oldPd;
    else
	tqAddPostRoutine( qpd_cleanup_globaldialog );
    globalPrintDialog->adjustSize();
}

TQGroupBox * TQPrintDialog::setupPrinterSettings()
{
    TQGroupBox * g = new TQGroupBox( 1, Horizontal, tr( "Printer settings"),
				   this, "settings group box" );

    d->colorMode = new TQButtonGroup( this );
    d->colorMode->hide();
    connect( d->colorMode, TQ_SIGNAL(clicked(int)),
	     this, TQ_SLOT(colorModeSelected(int)) );

    TQRadioButton *rb;
    rb = new TQRadioButton( tr( "Print in color if available" ),
			   g, "color" );
    d->colorMode->insert( rb, TQPrinter::Color );
    rb->setChecked( true );

    rb = new TQRadioButton( tr("Print in grayscale"),
			   g, "graysacle" );
    d->colorMode->insert( rb, TQPrinter::GrayScale );

    return g;
}

TQGroupBox * TQPrintDialog::setupDestination()
{
    TQGroupBox * g = new TQGroupBox( 0, Horizontal, tr( "Print destination"),
				   this, "destination group box" );

    TQBoxLayout * tll = new TQBoxLayout( g->layout(), TQBoxLayout::Down );

    d->printerOrFile = new TQButtonGroup( this );
    d->printerOrFile->hide();
    connect( d->printerOrFile, TQ_SIGNAL(clicked(int)),
	     this, TQ_SLOT(printerOrFileSelected(int)) );

    // printer radio button, list
    TQRadioButton * rb = new TQRadioButton( tr( "Print to printer:" ), g,
					  "printer" );
    tll->addWidget( rb );
    d->printerOrFile->insert( rb, 0 );
    rb->setChecked( true );
    d->outputToFile = false;

    TQBoxLayout * horiz = new TQBoxLayout( TQBoxLayout::LeftToRight );
    tll->addLayout( horiz, 3 );
    horiz->addSpacing( 19 );

    d->printers = new TQListView( g, "list of printers" );
    d->printers->setAllColumnsShowFocus( true );
    d->printers->addColumn( tr("Printer"), 125 );
    d->printers->addColumn( tr("Host"), 125 );
    d->printers->addColumn( tr("Comment"), 150 );

#if defined(Q_OS_UNIX)
    char * etcLpDefault = 0;

#ifndef TQT_NO_CUPS
    etcLpDefault = parseCupsOutput( d->printers );
#endif
    if ( d->printers->childCount() == 0 ) {
	// we only use other schemes when cups fails.

	parsePrintcap( d->printers, TQString::fromLatin1("/etc/printcap") );
	parseEtcLpMember( d->printers );
	parseSpoolInterface( d->printers );
	parseQconfig( d->printers );
	if ( addPrinterHook )
	    (*addPrinterHook)( d->printers );

	TQFileInfo f;
	f.setFile( TQString::fromLatin1("/etc/lp/printers") );
	if ( f.isDir() ) {
	    parseEtcLpPrinters( d->printers );
	    TQFile def( TQString::fromLatin1("/etc/lp/default") );
	    if ( def.open( IO_ReadOnly ) ) {
		if ( etcLpDefault )
		    delete[] etcLpDefault;
		etcLpDefault = new char[1025];
		def.readLine( etcLpDefault, 1024 );
		char * p = etcLpDefault;
		while ( p && *p ) {
		    if ( !isprint((uchar) *p) || isspace((uchar) *p) )
			*p = 0;
		    else
			p++;
		}
	    }
	}

	char * def = 0;
	f.setFile( TQString::fromLatin1("/etc/nsswitch.conf") );
	if ( f.isFile() ) {
	    def = parseNsswitchConf( d->printers );
	} else {
	    f.setFile( TQString::fromLatin1("/etc/printers.conf") );
	    if ( f.isFile() )
		def = parsePrintersConf( d->printers );
	}

	if ( def ) {
	    if ( etcLpDefault )
		delete[] etcLpDefault;
	    etcLpDefault = def;
	}
    }

    // all printers hopefully known.  try to find a good default
    TQString dollarPrinter;
    {
	const char * t = getenv( "PRINTER" );
	if ( !t || !*t )
	    t = getenv( "LPDEST" );
	dollarPrinter = TQString::fromLatin1( t );
	if ( !dollarPrinter.isEmpty() )
	    perhapsAddPrinter( d->printers, dollarPrinter,
			       TQPrintDialog::tr("unknown"),
			       TQString::fromLatin1("") );
    }
    int quality = 0;

    // bang the best default into the listview
    const TQListViewItem * lvi = d->printers->firstChild();
    d->printers->setCurrentItem( (TQListViewItem *)lvi );
    while ( lvi ) {
	TQRegExp ps( TQString::fromLatin1("[^a-z]ps(?:[^a-z]|$)") );
	TQRegExp lp( TQString::fromLatin1("[^a-z]lp(?:[^a-z]|$)") );

	if ( quality < 4 && lvi->text(0) == dollarPrinter ) {
	    d->printers->setCurrentItem( (TQListViewItem *)lvi );
	    quality = 4;
	} else if ( quality < 3 && etcLpDefault &&
		    lvi->text(0) == TQString::fromLatin1(etcLpDefault) ) {
	    d->printers->setCurrentItem( (TQListViewItem *)lvi );
	    quality = 3;
	} else if ( quality < 2 &&
		    ( lvi->text(0) == TQString::fromLatin1("ps") ||
		      ps.search(lvi->text(2)) != -1 ) ) {
	    d->printers->setCurrentItem( (TQListViewItem *)lvi );
	    quality = 2;
	} else if ( quality < 1 &&
		    ( lvi->text(0) == TQString::fromLatin1("lp") ||
		      lp.search(lvi->text(2)) > -1 ) ) {
	    d->printers->setCurrentItem( (TQListViewItem *)lvi );
	    quality = 1;
	}
	lvi = lvi->nextSibling();
    }

    if ( d->printers->currentItem() )
	d->printers->setSelected( d->printers->currentItem(), true );

    if ( etcLpDefault )                 // Avoid purify complaint
	delete[] etcLpDefault;
#endif

    int h = fontMetrics().height();
    if ( d->printers->firstChild() )
	h = d->printers->firstChild()->height();
    d->printers->setMinimumSize( d->printers->sizeHint().width(),
				 d->printers->header()->height() +
				 3 * h );
    horiz->addWidget( d->printers, 3 );

    tll->addSpacing( 6 );

    // file radio button, edit/browse
    d->printToFileButton = new TQRadioButton( tr( "Print to file:" ), g, "file" );
    tll->addWidget( d->printToFileButton );
    d->printerOrFile->insert( d->printToFileButton, 1 );

    horiz = new TQBoxLayout( TQBoxLayout::LeftToRight );
    tll->addLayout( horiz );
    horiz->addSpacing( 19 );

    d->fileName = new TQLineEdit( g, "file name" );
    connect( d->fileName, TQ_SIGNAL( textChanged(const TQString&) ),
	     this, TQ_SLOT( fileNameEditChanged(const TQString&) ) );
    horiz->addWidget( d->fileName, 1 );
    horiz->addSpacing( 6 );
    d->browse = new TQPushButton( tr("Browse..."), g, "browse files" );
    d->browse->setAutoDefault( false );
#ifdef TQT_NO_FILEDIALOG
    d->browse->setEnabled( false );
#endif
    connect( d->browse, TQ_SIGNAL(clicked()),
	     this, TQ_SLOT(browseClicked()) );
    horiz->addWidget( d->browse );

    d->fileName->setEnabled( false );
    d->browse->setEnabled( false );

    tll->activate();

    return g;
}


TQGroupBox * TQPrintDialog::setupOptions()
{
    TQGroupBox * g = new TQGroupBox( 0, Horizontal, tr( "Options"),
				   this, "options group box" );

    TQBoxLayout * tll = new TQBoxLayout( g->layout(), TQBoxLayout::Down );

    TQBoxLayout *lay = new TQBoxLayout( TQBoxLayout::LeftToRight );
    tll->addLayout( lay );

    tll = new TQBoxLayout( lay, TQBoxLayout::Down );

    d->printRange = new TQButtonGroup( this );
    d->printRange->hide();
    connect( d->printRange, TQ_SIGNAL(clicked(int)),
	     this, TQ_SLOT(printRangeSelected(int)) );

    d->pageOrder = new TQButtonGroup( this );
    d->pageOrder->hide();
    connect( d->pageOrder, TQ_SIGNAL(clicked(int)),
	     this, TQ_SLOT(pageOrderSelected(int)) );

    d->printAllButton = new TQRadioButton( tr("Print all"), g, "print all" );
    d->printRange->insert( d->printAllButton, 0 );
    tll->addWidget( d->printAllButton );

    d->printSelectionButton = new TQRadioButton( tr("Print selection"),
						g, "print selection" );
    d->printRange->insert( d->printSelectionButton, 1 );
    tll->addWidget( d->printSelectionButton );

    d->printRangeButton = new TQRadioButton( tr("Print range"),
					    g, "print range" );
    d->printRange->insert( d->printRangeButton, 2 );
    tll->addWidget( d->printRangeButton );

    TQBoxLayout * horiz = new TQBoxLayout( TQBoxLayout::LeftToRight );
    tll->addLayout( horiz );

    d->firstPageLabel = new TQLabel( tr("From page:"), g, "first page" );
    horiz->addSpacing( 19 );
    horiz->addWidget( d->firstPageLabel );

    d->firstPage = new TQPrintDialogSpinBox( 1, 9999, 1, g, "first page" );
    d->firstPage->setValue( 1 );
    horiz->addWidget( d->firstPage, 1 );
    connect( d->firstPage, TQ_SIGNAL(valueChanged(int)),
	     this, TQ_SLOT(setFirstPage(int)) );

    horiz = new TQBoxLayout( TQBoxLayout::LeftToRight );
    tll->addLayout( horiz );

    d->lastPageLabel = new TQLabel( tr("To page:"), g, "last page" );
    horiz->addSpacing( 19 );
    horiz->addWidget( d->lastPageLabel );

    d->lastPage = new TQPrintDialogSpinBox( 1, 9999, 1, g, "last page" );
    d->lastPage->setValue( 9999 );
    horiz->addWidget( d->lastPage, 1 );
    connect( d->lastPage, TQ_SIGNAL(valueChanged(int)),
	     this, TQ_SLOT(setLastPage(int)) );

    lay->addSpacing( 25 );
    tll = new TQBoxLayout( lay, TQBoxLayout::Down );

    // print order
    TQRadioButton * rb = new TQRadioButton( tr("Print first page first"),
					  g, "first page first" );
    tll->addWidget( rb );
    d->pageOrder->insert( rb, TQPrinter::FirstPageFirst );
    rb->setChecked( true );

    rb = new TQRadioButton( tr("Print last page first"),
			   g, "last page first" );
    tll->addWidget( rb );
    d->pageOrder->insert( rb, TQPrinter::LastPageFirst );

    tll->addStretch();

    // copies

    horiz = new TQBoxLayout( TQBoxLayout::LeftToRight );
    tll->addLayout( horiz );

    TQLabel * l = new TQLabel( tr("Number of copies:"), g, "Number of copies" );
    horiz->addWidget( l );

    d->copies = new TQPrintDialogSpinBox( 1, 99, 1, g, "copies" );
    d->copies->setValue( 1 );
    horiz->addWidget( d->copies, 1 );
    connect( d->copies, TQ_SIGNAL(valueChanged(int)),
	     this, TQ_SLOT(setNumCopies(int)) );

    TQSize s = d->firstPageLabel->sizeHint()
	      .expandedTo( d->lastPageLabel->sizeHint() )
	      .expandedTo( l->sizeHint() );
    d->firstPageLabel->setMinimumSize( s );
    d->lastPageLabel->setMinimumSize( s );
    l->setMinimumSize( s.width() + 19, s.height() );

    tll->activate();

    return g;
}


void isc( TQPrintDialogPrivate * d,
	  const TQString & text,
	  TQPrinter::PageSize ps )
{
    if ( d && text && ps < TQPrinter::NPageSize ) {
	d->sizeCombo->insertItem( text, -1 );
	int index = d->sizeCombo->count()-1;
	if ( index >= 0 && index < TQPrinter::NPageSize )
	    d->indexToPageSize[index] = ps;
    }
}

TQGroupBox * TQPrintDialog::setupPaper()
{
    TQGroupBox * g = new TQGroupBox( 1, Horizontal, tr( "Paper format"),
				   this, "Paper format" );
    d->pageSize = TQPrinter::A4;

    // page orientation
    d->orientationCombo = new TQComboBox( false, g );
    d->orientationCombo->insertItem( tr( "Portrait" ), -1 );
    d->orientationCombo->insertItem( tr( "Landscape" ), -1 );

    d->orientation = TQPrinter::Portrait;

    g->addSpace( 8 );

    connect( d->orientationCombo, TQ_SIGNAL( activated(int) ),
	     this, TQ_SLOT( orientSelected(int) ) );

    // paper size
    d->sizeCombo = new TQComboBox( false, g );

    int n;
    for( n=0; n<TQPrinter::NPageSize; n++ )
	d->indexToPageSize[n] = TQPrinter::A4;

    isc( d, tr( "A0 (841 x 1189 mm)" ), TQPrinter::A0 );
    isc( d, tr( "A1 (594 x 841 mm)" ), TQPrinter::A1 );
    isc( d, tr( "A2 (420 x 594 mm)" ), TQPrinter::A2 );
    isc( d, tr( "A3 (297 x 420 mm)" ), TQPrinter::A3 );
    isc( d, tr( "A4 (210x297 mm, 8.26x11.7 inches)" ), TQPrinter::A4 );
    isc( d, tr( "A5 (148 x 210 mm)" ), TQPrinter::A5 );
    isc( d, tr( "A6 (105 x 148 mm)" ), TQPrinter::A6 );
    isc( d, tr( "A7 (74 x 105 mm)" ), TQPrinter::A7 );
    isc( d, tr( "A8 (52 x 74 mm)" ), TQPrinter::A8 );
    isc( d, tr( "A9 (37 x 52 mm)" ), TQPrinter::A9 );
    isc( d, tr( "B0 (1000 x 1414 mm)" ), TQPrinter::B0 );
    isc( d, tr( "B1 (707 x 1000 mm)" ), TQPrinter::B1 );
    isc( d, tr( "B2 (500 x 707 mm)" ), TQPrinter::B2 );
    isc( d, tr( "B3 (353 x 500 mm)" ), TQPrinter::B3 );
    isc( d, tr( "B4 (250 x 353 mm)" ), TQPrinter::B4 );
    isc( d, tr( "B5 (176 x 250 mm, 6.93x9.84 inches)" ), TQPrinter::B5 );
    isc( d, tr( "B6 (125 x 176 mm)" ), TQPrinter::B6 );
    isc( d, tr( "B7 (88 x 125 mm)" ), TQPrinter::B7 );
    isc( d, tr( "B8 (62 x 88 mm)" ), TQPrinter::B8 );
    isc( d, tr( "B9 (44 x 62 mm)" ), TQPrinter::B9 );
    isc( d, tr( "B10 (31 x 44 mm)" ), TQPrinter::B10 );
    isc( d, tr( "C5E (163 x 229 mm)" ), TQPrinter::C5E );
    isc( d, tr( "DLE (110 x 220 mm)" ), TQPrinter::DLE );
    isc( d, tr( "Executive (7.5x10 inches, 191x254 mm)" ), TQPrinter::Executive );
    isc( d, tr( "Folio (210 x 330 mm)" ), TQPrinter::Folio );
    isc( d, tr( "Ledger (432 x 279 mm)" ), TQPrinter::Ledger );
    isc( d, tr( "Legal (8.5x14 inches, 216x356 mm)" ), TQPrinter::Legal );
    isc( d, tr( "Letter (8.5x11 inches, 216x279 mm)" ), TQPrinter::Letter );
    isc( d, tr( "Tabloid (279 x 432 mm)" ), TQPrinter::Tabloid );
    isc( d, tr( "US Common #10 Envelope (105 x 241 mm)" ), TQPrinter::Comm10E );

    connect( d->sizeCombo, TQ_SIGNAL( activated(int) ),
	     this, TQ_SLOT( paperSizeSelected(int) ) );

    return g;
}


/*!
  Display a dialog and allow the user to configure the TQPrinter \a
  p for an optional widget \a w. Returns true if the user clicks OK or
  presses Enter, false if the user clicks Cancel or presses Esc.

  getPrinterSetup() remembers the settings and provides the same
  settings the next time the dialog is shown.
*/

bool TQPrintDialog::getPrinterSetup( TQPrinter * p, TQWidget* w  )
{
    if ( !globalPrintDialog ) {
	globalPrintDialog = new TQPrintDialog( 0, 0, "global print dialog" );
#ifndef TQT_NO_WIDGET_TOPEXTRA
	globalPrintDialog->setCaption( TQPrintDialog::tr( "Setup Printer" ) );
#endif
	tqAddPostRoutine( qpd_cleanup_globaldialog );
	globalPrintDialog->setPrinter( p, true );
	globalPrintDialog->adjustSize();
    } else {
	globalPrintDialog->setPrinter( p, true );
    }
    globalPrintDialog->adjustPosition( w );
 #ifndef TQT_NO_WIDGET_TOPEXTRA
    if ( w ) {
	const TQPixmap *pm = w->icon();
	if ( pm && !pm->isNull() )
	    globalPrintDialog->setIcon( *pm );
	else {
	    w = w ? w->topLevelWidget() : 0;
	    pm = w ? w->icon() : 0;
	    if ( pm && !pm->isNull() )
		globalPrintDialog->setIcon( *pm );
	}
    }
#endif
    bool r = globalPrintDialog->exec() == TQDialog::Accepted;
    globalPrintDialog->setPrinter( 0 );
    return r;
}


void TQPrintDialog::printerOrFileSelected( int id )
{
    d->outputToFile = (id != 0);
    if ( d->outputToFile ) {
	d->ok->setEnabled( true );
	fileNameEditChanged( d->fileName->text() );
	if ( !d->fileName->edited() && d->fileName->text().isEmpty() ) {
	    TQString home = TQString::fromLatin1( ::getenv( "HOME" ) );
	    TQString cur = TQDir::currentDirPath();
	    if ( home.at(home.length()-1) != '/' )
		home += '/';
	    if ( cur.at(cur.length()-1) != '/' )
		cur += '/';
	    if ( cur.left( home.length() ) != home )
		cur = home;
#ifdef TQ_WS_X11
	    cur += "print.ps";
#endif
	    d->fileName->setText( cur );
	    d->fileName->setCursorPosition( cur.length() );
	    d->fileName->selectAll();
	}
	d->browse->setEnabled( true );
	d->fileName->setEnabled( true );
	d->fileName->setFocus();
	d->printers->setEnabled( false );
    } else {
	d->ok->setEnabled( d->printers->childCount() != 0 );
	d->printers->setEnabled( true );
	if ( d->fileName->hasFocus() || d->browse->hasFocus() )
	    d->printers->setFocus();
	d->browse->setEnabled( false );
	d->fileName->setEnabled( false );
    }
}


void TQPrintDialog::landscapeSelected( int id )
{
    d->orientation = (TQPrinter::Orientation)id;
}


void TQPrintDialog::paperSizeSelected( int id )
{
    if ( id < TQPrinter::NPageSize )
	d->pageSize = TQPrinter::PageSize( d->indexToPageSize[id] );
}


void TQPrintDialog::orientSelected( int id )
{
    d->orientation = (TQPrinter::Orientation)id;
}


void TQPrintDialog::pageOrderSelected( int id )
{
    d->pageOrder2 = (TQPrinter::PageOrder)id;
}


void TQPrintDialog::setNumCopies( int copies )
{
    d->numCopies = copies;
}


void TQPrintDialog::browseClicked()
{
#ifndef TQT_NO_FILEDIALOG
    TQString fn = TQFileDialog::getSaveFileName( d->fileName->text(), tr( "PostScript Files (*.ps);;All Files (*)" ), this );
    if ( !fn.isNull() )
	d->fileName->setText( fn );
#endif
}


void TQPrintDialog::okClicked()
{
    d->lastPage->interpretText();
    d->firstPage->interpretText();
    d->copies->interpretText();
    if ( d->outputToFile ) {
	d->printer->setOutputToFile( true );
	d->printer->setOutputFileName( d->fileName->text() );
    } else {
	d->printer->setOutputToFile( false );
	TQListViewItem * l = d->printers->currentItem();
	if ( l )
	    d->printer->setPrinterName( l->text( 0 ) );
    }

    d->printer->setOrientation( d->orientation );
    d->printer->setPageSize( d->pageSize );
    d->printer->setPageOrder( d->pageOrder2 );
    d->printer->setColorMode( d->colorMode2 );
    d->printer->setNumCopies( d->numCopies );
    if ( d->printAllButton->isChecked() ) {
	d->printer->setPrintRange(TQPrinter::AllPages);
	d->printer->setFromTo( d->printer->minPage(), d->printer->maxPage() );
    } else {
	if (d->printSelectionButton->isChecked())
	    d->printer->setPrintRange(TQPrinter::Selection);
	else
	    d->printer->setPrintRange(TQPrinter::PageRange);
	d->printer->setFromTo( d->firstPage->value(), d->lastPage->value() );
    }

    accept();
}


void TQPrintDialog::printRangeSelected( int id )
{
    bool enable = (id == 2);
    d->firstPage->setEnabled( enable );
    d->lastPage->setEnabled( enable );
    d->firstPageLabel->setEnabled( enable );
    d->lastPageLabel->setEnabled( enable );
}


void TQPrintDialog::setFirstPage( int fp )
{
    if ( d->printer )
	d->lastPage->setRange( fp, TQMAX(fp, TQPrintDialog::d->printer->maxPage()) );
}


void TQPrintDialog::setLastPage( int lp )
{
    if ( d->printer )
	d->firstPage->setRange( TQMIN(lp, TQPrintDialog::d->printer->minPage()), lp );
}


/*!
  Sets this dialog to configure printer \a p, or no printer if \a p
  is null. If \a pickUpSettings is true, the dialog reads most of
  its settings from \a p. If \a pickUpSettings is false (the
  default) the dialog keeps its old settings.
*/

void TQPrintDialog::setPrinter( TQPrinter * p, bool pickUpSettings )
{
    d->printer = p;

    if ( p && pickUpSettings ) {
	// top to botton in the old dialog.
	// printer or file
	d->printerOrFile->setButton( p->outputToFile() );
	printerOrFileSelected( p->outputToFile() );

	// printer name
	if ( !!p->printerName() ) {
	    TQListViewItem * i = d->printers->firstChild();
	    while ( i && i->text( 0 ) != p->printerName() )
		i = i->nextSibling();
	    if ( i ) {
		d->printers->setSelected( i, true );
		d->ok->setEnabled( true );
	    } else if ( d->fileName->text().isEmpty() ) {
		d->ok->setEnabled( d->printers->childCount() != 0 );
	    }
	}

	// print command does not exist any more

	// file name
	d->printToFileButton->setEnabled( d->printer->isOptionEnabled( TQPrinter::PrintToFile ) );
	d->fileName->setText( p->outputFileName() );

	// orientation
	d->orientationCombo->setCurrentItem( (int)p->orientation() );
	orientSelected( p->orientation() );

	// page size
	int n = 0;
	while ( n < TQPrinter::NPageSize &&
		d->indexToPageSize[n] != p->pageSize() )
	    n++;
	d->sizeCombo->setCurrentItem( n );
	paperSizeSelected( n );

	// New stuff (Options)

	// page order
	d->pageOrder->setButton( (int)p->pageOrder() );
	pageOrderSelected( p->pageOrder() );

	// color mode
	d->colorMode->setButton( (int)p->colorMode() );
	colorModeSelected( p->colorMode() );

	// number of copies
	d->copies->setValue( p->numCopies() );
	setNumCopies( p->numCopies() );
    }

    if( p ) {
	d->printAllButton->setEnabled( true );
	d->printSelectionButton
	    ->setEnabled( d->printer->isOptionEnabled( TQPrinter::PrintSelection ) );
	d->printRangeButton
	    ->setEnabled( d->printer->isOptionEnabled( TQPrinter::PrintPageRange ) );

	TQPrinter::PrintRange range = p->printRange();
	switch ( range ) {
	case TQPrinter::AllPages:
	    d->printAllButton->setChecked(true);
	    printRangeSelected( d->printRange->id( d->printAllButton ) );
	    break;
	case TQPrinter::Selection:
	    d->printSelectionButton->setChecked(true);
	    printRangeSelected( d->printRange->id( d->printSelectionButton ) );
	    break;
	case TQPrinter::PageRange:
	    d->printRangeButton->setChecked(true);
	    printRangeSelected( d->printRange->id( d->printRangeButton ) );
	    break;
	}
    }

    if ( p && p->maxPage() ) {
	d->firstPage->setRange( p->minPage(), p->maxPage() );
	d->lastPage->setRange( p->minPage(), p->maxPage() );
	if ( p->fromPage() || p->toPage() ) {
	    setFirstPage( p->fromPage() );
	    setLastPage( p->toPage() );
	    d->firstPage->setValue(p->fromPage());
	    d->lastPage->setValue(p->toPage());
	}
    }
}


/*!  Returns a pointer to the printer this dialog configures, or 0 if
  this dialog does not operate on any printer. */

TQPrinter * TQPrintDialog::printer() const
{
    return d->printer;
}


void TQPrintDialog::colorModeSelected( int id )
{
    d->colorMode2 = (TQPrinter::ColorMode)id;
}

/*!
  Adds the button \a but to the layout of the print dialog. The added
  buttons are arranged from the left to the right below the
  last groupbox of the printdialog.
*/

void TQPrintDialog::addButton( TQPushButton *but )
{
    d->customLayout->addWidget( but );
}

void TQPrintDialog::fileNameEditChanged( const TQString &text )
{
    if ( d->fileName->isEnabled() )
	d->ok->setEnabled( !text.isEmpty() );
}

#endif
