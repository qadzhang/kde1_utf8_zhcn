/* This file is licensed under the terms of the GPL v2 or v3, as it has been publicly released by
   OpenSUSE as part of their GPLed TQt library disribution */

#ifndef TQTDEINTEGRATION_H
#define TQTDEINTEGRATION_H

#include <ntqstringlist.h>

class TQLibrary;
class TQWidget;
class TQColor;
class TQFont;

class TQTDEIntegration
    {
    public:
        static bool enabled();
// --- 
        static bool initializeIntegration( );
        static TQStringList getOpenFileNames( const TQString& filter, TQString* workingDirectory,
            TQWidget* parent, const char* name, const TQString& caption, TQString* selectedFilter,
            bool multiple );
        static TQString getSaveFileName( const TQString& initialSelection, const TQString& filter,
            TQString* workingDirectory, TQWidget* parent, const char* name, const TQString& caption,
            TQString* selectedFilter );
        static TQString getExistingDirectory( const TQString& initialDirectory, TQWidget* parent,
            const char* name, const TQString& caption );
        static TQColor getColor( const TQColor& color, TQWidget* parent, const char* name );
        static TQFont getFont( bool* ok, const TQFont* def, TQWidget* parent, const char* name );
        static int messageBox1( int type, TQWidget* parent, const TQString& caption,
            const TQString& text, int button0, int button1, int button2 );
        static int information( TQWidget* parent, const TQString& caption, const TQString& text,
            int button0, int button1, int button2 );
        static int question( TQWidget* parent, const TQString& caption, const TQString& text,
            int button0, int button1, int button2 );
        static int warning( TQWidget* parent, const TQString& caption, const TQString& text,
            int button0, int button1, int button2 );
        static int critical( TQWidget* parent, const TQString& caption, const TQString& text,
            int button0, int button1, int button2 );
        static int messageBox2( int type, TQWidget* parent, const TQString& caption,
            const TQString& text, const TQString& button0Text, const TQString& button1Text,
            const TQString& button2Text, int defaultButton, int escapeButton );
        static int information( TQWidget* parent, const TQString& caption, const TQString& text,
            const TQString& button0Text, const TQString& button1Text, const TQString& button2Text,
            int defaultButton, int escapeButton );
        static int question( TQWidget* parent, const TQString& caption, const TQString& text,
            const TQString& button0Text, const TQString& button1Text, const TQString& button2Text,
            int defaultButton, int escapeButton );
        static int warning( TQWidget* parent, const TQString& caption, const TQString& text,
            const TQString& button0Text, const TQString& button1Text, const TQString& button2Text,
            int defaultButton, int escapeButton );
        static int critical( TQWidget* parent, const TQString& caption, const TQString& text,
            const TQString& button0Text, const TQString& button1Text, const TQString& button2Text,
            int defaultButton, int escapeButton );
// ---
    private:
        static void initLibrary();
        static bool inited;
        static bool enable;
    };

#endif
