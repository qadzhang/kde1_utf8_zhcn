#ifndef REGEXPTESTER_H
#define REGEXPTESTER_H

#include <ntqdialog.h>

class TQCheckBox;
class TQComboBox;
class TQLabel;
class TQPushButton;
class TQStatusBar;
class TQTable;

class RegexpTester : public TQDialog
{
    TQ_OBJECT

public:
    RegexpTester(TQWidget* parent=0, const char* name=0, bool modal=false,
		 WFlags f=0);

    TQLabel *regexLabel;
    TQComboBox *regexComboBox;
    TQLabel *textLabel;
    TQComboBox *textComboBox;
    TQCheckBox *caseSensitiveCheckBox;
    TQCheckBox *minimalCheckBox;
    TQCheckBox *wildcardCheckBox;
    TQTable *resultTable;
    TQPushButton *executePushButton;
    TQPushButton *copyPushButton;
    TQPushButton *quitPushButton;
    TQStatusBar *statusBar;

public slots:
    void copy();
    void execute();

private:
    void languageChange();
};

#endif // REGEXPTESTER_H
