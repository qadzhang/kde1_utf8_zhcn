#ifndef CHARTFORM_H
#define CHARTFORM_H

#include "element.h"

#include <ntqmainwindow.h>
#include <ntqstringlist.h>


class CanvasView;

class TQAction;
class TQCanvas;
class TQFont;
class TQPrinter;
class TQString;


class ChartForm: public TQMainWindow
{
    TQ_OBJECT
public:
    enum { MAX_ELEMENTS = 100 };
    enum { MAX_RECENTFILES = 9 }; // Must not exceed 9
    enum ChartType { PIE, VERTICAL_BAR, HORIZONTAL_BAR };
    enum AddValuesType { NO, YES, AS_PERCENTAGE };

    ChartForm( const TQString& filename );
    ~ChartForm();

    int chartType() { return m_chartType; }
    void setChanged( bool changed = true ) { m_changed = changed; }
    void drawElements();

    TQPopupMenu *optionsMenu; // Why public? See canvasview.cpp

protected:
    virtual void closeEvent( TQCloseEvent * );

private slots:
    void fileNew();
    void fileOpen();
    void fileOpenRecent( int index );
    void fileSave();
    void fileSaveAs();
    void fileSaveAsPixmap();
    void filePrint();
    void fileQuit();
    void optionsSetData();
    void updateChartType( TQAction *action );
    void optionsSetFont();
    void optionsSetOptions();
    void helpHelp();
    void helpAbout();
    void helpAboutTQt();
    void saveOptions();

private:
    void init();
    void load( const TQString& filename );
    bool okToClear();
    void drawPieChart( const double scales[], double total, int count );
    void drawVerticalBarChart( const double scales[], double total, int count );
    void drawHorizontalBarChart( const double scales[], double total, int count );

    TQString valueLabel( const TQString& label, double value, double total );
    void updateRecentFiles( const TQString& filename );
    void updateRecentFilesMenu();
    void setChartType( ChartType chartType );

    TQPopupMenu *fileMenu;
    TQAction *optionsPieChartAction;
    TQAction *optionsHorizontalBarChartAction;
    TQAction *optionsVerticalBarChartAction;


    TQString m_filename;
    TQStringList m_recentFiles;
    TQCanvas *m_canvas;
    CanvasView *m_canvasView;
    bool m_changed;
    ElementVector m_elements;
    TQPrinter *m_printer;
    ChartType m_chartType;
    AddValuesType m_addValues;
    int m_decimalPlaces;
    TQFont m_font;
};

#endif
