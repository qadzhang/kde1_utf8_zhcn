#ifndef SIZEAWARE_H
#define SIZEAWARE_H
#include <ntqdialog.h>

class SizeAware : public TQDialog
{
    TQ_OBJECT
    TQ_PROPERTY( TQString company READ company WRITE setCompany )
    TQ_PROPERTY( TQString settingsFile READ settingsFile WRITE setSettingsFile )
public:
    SizeAware( TQDialog *parent = 0, const char *name = 0, bool modal = false );
    ~SizeAware();
    void setCompany( TQString company ) { m_company = company; }
    TQString company() const { return m_company; } 
    void setSettingsFile( TQString settingsFile ) { m_settingsFile = settingsFile; }
    TQString settingsFile() const { return m_settingsFile; }
public slots:
    void destroy();
private:
    TQString m_company;
    TQString m_settingsFile;
};
#endif
