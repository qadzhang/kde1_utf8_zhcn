/* This file is licensed under the terms of the GPL v2 or v3, as it has been publicly released by
   OpenSUSE as part of their GPLed TQt library disribution */

#include "tqttdeintegration_x11_p.h"

#include <ntqcolordialog.h>
#include <ntqfiledialog.h>
#include <ntqfontdialog.h>
#include <ntqlibrary.h>
#include <ntqregexp.h>
#include <ntqmessagebox.h>
#include <stdlib.h>

bool TQTDEIntegration::inited = false;
bool TQTDEIntegration::enable = false;

bool TQTDEIntegration::enabled()
    {
    if( !inited )
        initLibrary();
    return enable;
    }

static TQCString findLibrary()
    {
    if( getenv( "TQT_NO_TDE_INTEGRATION" ) == NULL
        || getenv( "TQT_NO_TDE_INTEGRATION" )[ 0 ] == '0' )
        {
        return TQCString() + tqInstallPathPlugins() + "/integration/libtqttde";
        }
    return "";
    }

inline static long widgetToWinId( const TQWidget* w )
    {
    return w != NULL ? w->winId() : 0;
    }

inline static TQFont fontPtrToFontRef( const TQFont* f )
    {
    return f != NULL ? *f : TQFont();
    }

// ---
static bool (*tqttde_initializeIntegration)( );
static TQStringList (*tqttde_getOpenFileNames)( const TQString& filter, TQString* workingDirectory,
    long parent, const TQCString& name, const TQString& caption, TQString* selectedFilter,
    bool multiple );
static TQString (*tqttde_getSaveFileName)( const TQString& initialSelection, const TQString& filter,
    TQString* workingDirectory, long parent, const TQCString& name, const TQString& caption,
    TQString* selectedFilter );
static TQString (*tqttde_getExistingDirectory)( const TQString& initialDirectory, long parent,
    const TQCString& name, const TQString& caption );
static TQColor (*tqttde_getColor)( const TQColor& color, long parent, const TQCString& name );
static TQFont (*tqttde_getFont)( bool* ok, const TQFont& def, long parent, const TQCString& name );
static int (*tqttde_messageBox1)( int type, long parent, const TQString& caption, const TQString& text,
    int button0, int button1, int button2 );
static int (*tqttde_messageBox2)( int type, long parent, const TQString& caption, const TQString& text,
    const TQString& button0Text, const TQString& button1Text, const TQString& button2Text,
    int defaultButton, int escapeButton );

void TQTDEIntegration::initLibrary()
    {
    if( !inited )
        {
        enable = false;
        inited = true;
        TQString libpath = findLibrary();
        if( libpath.isEmpty())
            return;
        TQLibrary lib( libpath );
        lib.setAutoUnload( false );
        tqttde_initializeIntegration = (
            bool (*)( )
            )
            lib.resolve("initializeIntegration");
        if( tqttde_initializeIntegration == NULL )
            return;
        tqttde_getOpenFileNames = (
            TQStringList (*)( const TQString& filter, TQString* workingDirectory, long parent,
                const TQCString& name, const TQString& caption, TQString* selectedFilter,
                bool multiple )
            )
            lib.resolve("getOpenFileNames");
        if( tqttde_getOpenFileNames == NULL )
            return;
        tqttde_getSaveFileName = (
            TQString (*)( const TQString& initialSelection, const TQString& filter, TQString* workingDirectory,
                long parent, const TQCString& name, const TQString& caption, TQString* selectedFilter )
            )
            lib.resolve("getSaveFileName");
        if( tqttde_getSaveFileName == NULL )
            return;
        tqttde_getExistingDirectory = (
            TQString (*)( const TQString& initialDirectory, long parent, const TQCString& name,
                const TQString& caption )
            )
            lib.resolve("getExistingDirectory");
        if( tqttde_getExistingDirectory == NULL )
            return;
        tqttde_getColor = (
            TQColor (*)( const TQColor& color, long parent, const TQCString& name )
            )
            lib.resolve("getColor");
        if( tqttde_getColor == NULL )
            return;
        tqttde_getFont = (
            TQFont (*)( bool* ok, const TQFont& def, long parent, const TQCString& name )
            )
            lib.resolve("getFont");
        if( tqttde_getFont == NULL )
            return;
        tqttde_messageBox1 = (
            int (*)( int type, long parent, const TQString& caption, const TQString& text,
                int button0, int button1, int button2 )
            )
            lib.resolve("messageBox1");
        if( tqttde_messageBox1 == NULL )
            return;
        tqttde_messageBox2 = (
            int (*)( int type, long parent, const TQString& caption, const TQString& text,
                const TQString& button0Text, const TQString& button1Text, const TQString& button2Text,
                int defaultButton, int escapeButton )
            )
            lib.resolve("messageBox2");
        if( tqttde_messageBox2 == NULL )
            return;
        enable = tqttde_initializeIntegration();
        }
    }

bool TQTDEIntegration::initializeIntegration( )
    {
    return tqttde_initializeIntegration(
         );
    }
TQStringList TQTDEIntegration::getOpenFileNames( const TQString& filter, TQString* workingDirectory,
    TQWidget* parent, const char* name, const TQString& caption, TQString* selectedFilter,
    bool multiple )
    {
    return tqttde_getOpenFileNames(
        filter, workingDirectory, widgetToWinId( parent ), name, caption, selectedFilter, multiple );
    }
TQString TQTDEIntegration::getSaveFileName( const TQString& initialSelection, const TQString& filter,
    TQString* workingDirectory, TQWidget* parent, const char* name, const TQString& caption,
    TQString* selectedFilter )
    {
    return tqttde_getSaveFileName(
        initialSelection, filter, workingDirectory, widgetToWinId( parent ), name, caption, selectedFilter );
    }
TQString TQTDEIntegration::getExistingDirectory( const TQString& initialDirectory, TQWidget* parent,
    const char* name, const TQString& caption )
    {
    return tqttde_getExistingDirectory(
        initialDirectory, widgetToWinId( parent ), name, caption );
    }
TQColor TQTDEIntegration::getColor( const TQColor& color, TQWidget* parent, const char* name )
    {
    return tqttde_getColor(
        color, widgetToWinId( parent ), name );
    }
TQFont TQTDEIntegration::getFont( bool* ok, const TQFont* def, TQWidget* parent, const char* name )
    {
    return tqttde_getFont(
        ok, fontPtrToFontRef( def ), widgetToWinId( parent ), name );
    }
int TQTDEIntegration::messageBox1( int type, TQWidget* parent, const TQString& caption,
    const TQString& text, int button0, int button1, int button2 )
    {
    return tqttde_messageBox1(
        type, widgetToWinId( parent ), caption, text, button0, button1, button2 );
    }
int TQTDEIntegration::messageBox2( int type, TQWidget* parent, const TQString& caption,
    const TQString& text, const TQString& button0Text, const TQString& button1Text, const TQString& button2Text,
    int defaultButton, int escapeButton )
    {
    return tqttde_messageBox2(
        type, widgetToWinId( parent ), caption, text, button0Text, button1Text, button2Text, defaultButton, escapeButton );
    }
// ---

int TQTDEIntegration::information( TQWidget* parent, const TQString& caption,
    const TQString& text, int button0, int button1, int button2 )
    {
    return tqttde_messageBox1(
        TQMessageBox::Information, widgetToWinId( parent ), caption, text, button0, button1, button2 );
    }

int TQTDEIntegration::question( TQWidget* parent, const TQString& caption,
    const TQString& text, int button0, int button1, int button2 )
    {
    return tqttde_messageBox1(
        TQMessageBox::Question, widgetToWinId( parent ), caption, text, button0, button1, button2 );
    }

int TQTDEIntegration::warning( TQWidget* parent, const TQString& caption,
    const TQString& text, int button0, int button1, int button2 )
    {
    return tqttde_messageBox1(
        TQMessageBox::Warning, widgetToWinId( parent ), caption, text, button0, button1, button2 );
    }

int TQTDEIntegration::critical( TQWidget* parent, const TQString& caption,
    const TQString& text, int button0, int button1, int button2 )
    {
    return tqttde_messageBox1(
        TQMessageBox::Critical, widgetToWinId( parent ), caption, text, button0, button1, button2 );
    }

int TQTDEIntegration::information( TQWidget* parent, const TQString& caption,
    const TQString& text, const TQString& button0Text, const TQString& button1Text, const TQString& button2Text,
    int defaultButton, int escapeButton )
    {
    return tqttde_messageBox2(
        TQMessageBox::Information, widgetToWinId( parent ), caption, text, button0Text, button1Text, button2Text, defaultButton, escapeButton );
    }

int TQTDEIntegration::question( TQWidget* parent, const TQString& caption,
    const TQString& text, const TQString& button0Text, const TQString& button1Text, const TQString& button2Text,
    int defaultButton, int escapeButton )
    {
    return tqttde_messageBox2(
        TQMessageBox::Question, widgetToWinId( parent ), caption, text, button0Text, button1Text, button2Text, defaultButton, escapeButton );
    }

int TQTDEIntegration::warning( TQWidget* parent, const TQString& caption,
    const TQString& text, const TQString& button0Text, const TQString& button1Text, const TQString& button2Text,
    int defaultButton, int escapeButton )
    {
    return tqttde_messageBox2(
        TQMessageBox::Warning, widgetToWinId( parent ), caption, text, button0Text, button1Text, button2Text, defaultButton, escapeButton );
    }

int TQTDEIntegration::critical( TQWidget* parent, const TQString& caption,
    const TQString& text, const TQString& button0Text, const TQString& button1Text, const TQString& button2Text,
    int defaultButton, int escapeButton )
    {
    return tqttde_messageBox2(
        TQMessageBox::Critical, widgetToWinId( parent ), caption, text, button0Text, button1Text, button2Text, defaultButton, escapeButton );
    }
