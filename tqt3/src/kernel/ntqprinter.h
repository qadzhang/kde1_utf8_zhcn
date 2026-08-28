/**********************************************************************
**
** Definition of TQPrinter class
**
** Created : 940927
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of the kernel module of the TQt GUI Toolkit.
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

#ifndef TQPRINTER_H
#define TQPRINTER_H

#ifndef QT_H
#include "ntqpaintdevice.h"
#include "ntqstring.h"
#include "ntqstringlist.h"
#endif // QT_H

#ifndef TQT_NO_PRINTER

#if defined(B0)
#undef B0 // Terminal hang-up.  We assume that you do not want that.
#endif

class TQPrinterPrivate;

class TQ_EXPORT TQPrinter : public TQPaintDevice
{
public:
    enum PrinterMode { ScreenResolution, PrinterResolution, HighResolution, Compatible };

    TQPrinter( PrinterMode mode = ScreenResolution );
   ~TQPrinter();

    enum Orientation { Portrait, Landscape };

    enum PageSize    { A4, B5, Letter, Legal, Executive,
		       A0, A1, A2, A3, A5, A6, A7, A8, A9, B0, B1,
		       B10, B2, B3, B4, B6, B7, B8, B9, C5E, Comm10E,
		       DLE, Folio, Ledger, Tabloid, Custom, NPageSize = Custom };

    enum PageOrder   { FirstPageFirst, LastPageFirst };

    enum ColorMode   { GrayScale, Color };

    enum PaperSource { OnlyOne, Lower, Middle, Manual, Envelope,
                       EnvelopeManual, Auto, Tractor, SmallFormat,
                       LargeFormat, LargeCapacity, Cassette, FormSource };

    enum PrintRange   { AllPages,
			Selection,
			PageRange };

    enum PrinterOption { PrintToFile,
			 PrintSelection,
			 PrintPageRange };

    TQString printerName() const;
    virtual void setPrinterName( const TQString &);
    bool outputToFile() const;
    virtual void setOutputToFile( bool );
    TQString outputFileName()const;
    virtual void setOutputFileName( const TQString &);

    TQString printProgram() const;
    virtual void setPrintProgram( const TQString &);

    TQString printerSelectionOption() const;
    virtual void setPrinterSelectionOption( const TQString & );

    TQString docName() const;
    virtual void setDocName( const TQString &);
    TQString creator() const;
    virtual void setCreator( const TQString &);

    Orientation orientation()   const;
    virtual void setOrientation( Orientation );
    PageSize pageSize()      const;
    virtual void setPageSize( PageSize );
#ifdef TQ_WS_WIN
    void setWinPageSize( short winPageSize );
    short winPageSize() const;
#endif
#ifdef TQ_WS_MAC
    bool printSetup();
    bool pageSetup();
#endif
    virtual void setPageOrder( PageOrder );
    PageOrder   pageOrder() const;

    void setResolution( int );
    int resolution() const;

    virtual void setColorMode( ColorMode );
    ColorMode   colorMode() const;

    virtual void        setFullPage( bool );
    bool                fullPage() const;
    TQSize       margins()       const;
    void setMargins( uint top, uint left, uint bottom, uint right );
    void margins( uint *top, uint *left, uint *bottom, uint *right ) const;

    int         fromPage()      const;
    int         toPage()        const;
    virtual void setFromTo( int fromPage, int toPage );
    int         minPage()       const;
    int         maxPage()       const;
    virtual void setMinMax( int minPage, int maxPage );
    int         numCopies()     const;
    virtual void setNumCopies( int );

    bool	collateCopiesEnabled() const;
    void	setCollateCopiesEnabled(bool );

    bool	collateCopies() const;
    void	setCollateCopies( bool );

    PrintRange	printRange() const;
    void 	setPrintRange( PrintRange range );

    bool        newPage();
    bool        abort();
    bool        aborted()       const;

    bool        setup( TQWidget *parent = 0 );

    PaperSource paperSource()   const;
    virtual void setPaperSource( PaperSource );

    void setOptionEnabled( PrinterOption, bool enable );
    bool isOptionEnabled( PrinterOption );

protected:
    bool        cmd( int, TQPainter *, TQPDevCmdParam * );
    int         metric( int ) const;

#if defined(TQ_WS_WIN)
    virtual void        setActive();
    virtual void        setIdle();
#endif

private:
#if defined(TQ_WS_X11) || defined(TQ_WS_QWS)
    TQPaintDevice *pdrv;
    int         pid;
#endif
#if defined(TQ_WS_MAC)
    friend class TQPrinterPrivate;
    PMPageFormat pformat;
    PMPrintSettings psettings;
    PMPrintSession psession;
    bool prepare(PMPrintSettings *);
    bool prepare(PMPageFormat *);
    void interpret(PMPrintSettings *);
    void interpret(PMPageFormat *);
#endif
#if defined(TQ_WS_WIN)
    void        readPdlg( void* );
    void        readPdlgA( void* );
    void	writeDevmode( TQt::HANDLE );
    void	writeDevmodeA( TQt::HANDLE );
    void	reinit();

    bool        viewOffsetDone;
    TQPainter*   painter;
    TQt::HANDLE hdevmode;
    TQt::HANDLE hdevnames;
#endif

    int         state;
    TQString     printer_name;
    TQString     option_string;
    TQString     output_filename;
    bool        output_file;
    TQString     print_prog;
    TQString     doc_name;
    TQString     creator_name;

    PageSize    page_size;
    PaperSource paper_source;
    PageOrder   page_order;
    ColorMode   color_mode;
    Orientation orient;
    uint	to_edge : 1;
    uint	appcolcopies : 1;
    uint	usercolcopies : 1;
    uint	res_set : 1;
    short       from_pg, to_pg;
    short       min_pg,  max_pg;
    short       ncopies;
    int         res;
    TQPrinterPrivate *d;

private:        // Disabled copy constructor and operator=
#if defined(TQ_DISABLE_COPY)
    TQPrinter( const TQPrinter & );
    TQPrinter &operator=( const TQPrinter & );
#endif
};


inline TQString TQPrinter::printerName() const
{ return printer_name; }

inline bool TQPrinter::outputToFile() const
{ return output_file; }

inline TQString TQPrinter::outputFileName() const
{ return output_filename; }

inline TQString TQPrinter::printProgram() const
{ return print_prog; }

inline TQString TQPrinter::docName() const
{ return doc_name; }

inline TQString TQPrinter::creator() const
{ return creator_name; }

inline TQPrinter::PageSize TQPrinter::pageSize() const
{ return page_size; }

inline TQPrinter::Orientation TQPrinter::orientation() const
{ return orient; }

inline int TQPrinter::fromPage() const
{ return from_pg; }

inline int TQPrinter::toPage() const
{ return to_pg; }

inline int TQPrinter::minPage() const
{ return min_pg; }

inline int TQPrinter::maxPage() const
{ return max_pg; }

inline int TQPrinter::numCopies() const
{ return ncopies; }

inline bool TQPrinter::collateCopiesEnabled() const
{ return appcolcopies; }

inline void TQPrinter::setCollateCopiesEnabled(bool v)
{ appcolcopies = v; }

inline bool TQPrinter::collateCopies() const
{ return usercolcopies; }


#endif // TQT_NO_PRINTER

#endif // TQPRINTER_H
