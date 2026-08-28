#ifndef FILECHOOSER_H
#define FILECHOOSER_H

#include <ntqwidget.h>
#include <ntqwidgetplugin.h>

class TQLineEdit;
class TQPushButton;

#ifdef FILECHOOSER_IS_WIDGET
#undef QT_WIDGET_PLUGIN_EXPORT
#define QT_WIDGET_PLUGIN_EXPORT
#endif

class QT_WIDGET_PLUGIN_EXPORT FileChooser : public TQWidget
{
    TQ_OBJECT

    TQ_ENUMS( Mode )
    TQ_PROPERTY( Mode mode READ mode WRITE setMode )
    TQ_PROPERTY( TQString fileName READ fileName WRITE setFileName )

public:
    FileChooser( TQWidget *parent = 0, const char *name = 0);

    enum Mode { File, Directory };

    TQString fileName() const;
    Mode mode() const;

public slots:
    void setFileName( const TQString &fn );
    void setMode( Mode m );

signals:
    void fileNameChanged( const TQString & );

private slots:
    void chooseFile();

private:
    TQLineEdit *lineEdit;
    TQPushButton *button;
    Mode md;

};

#endif
