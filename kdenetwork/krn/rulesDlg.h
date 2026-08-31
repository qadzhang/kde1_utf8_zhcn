#ifndef RULES_DLG_H
#define RULES_DLG_H

#include <qdialog.h>
#include "rules.h"

class QPushButton;
class QComboBox;
class QCheckBox;
class QLineEdit;
class QListBox;
class KTypeLayout;
class KIntLineEdit;
class rulesDlg : public QDialog
{
    Q_OBJECT
        
public:
    
    rulesDlg();
    
    virtual ~rulesDlg();
    Rule *rule;
    QListBox *list;
    QListBox *list2;
    QListBox *globalList;
    KIntLineEdit *gvalue;

public slots:
    // [KDE1 Revival 2026] 槽签名 TQString 化：与 highlighted(const TQString&) 连接匹配
    void editRule(const TQString &name);
    void saveRuleAs();
    void saveRule();
    void deleteRule();
    void addGlobal();
    void deleteGlobal();
private:
    KTypeLayout *l;
};

#endif