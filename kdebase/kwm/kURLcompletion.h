/* 
 * KURLCompletion
 * Completion of URL's (currently just files), to
 * be connected to KLined() which emits signals for completion (Tab/Ctrl-S)
 * and rotation (Ctrl-D)
 *
 * 10/1997 Henner Zeller <zeller@think.de>
 * This code is based on Torben Weis' KFileEntry
 */

#ifndef kurlcompletion_h
#define kurlcompletion_h

#include <qstring.h>
#include <qstrlist.h>
#include <qobject.h>

class KURLCompletion : public QObject
{
    Q_OBJECT

public slots:
    void make_completion ();
    void make_rotation ();
    // [KDE1 Revival 2026] TQLineEdit 的 textChanged 是 TQString 版——槽签名同步
    void edited (const TQString &);
    
signals:
    // [KDE1 Revival 2026] 接收方 TQLineEdit 只有 setText(const TQString&) 槽
    void setText (const TQString &);
	
public:
    KURLCompletion( const char *dir = "");
    ~KURLCompletion();

private:
    bool is_fileurl (QString &url, bool &ambigous_beginning) const;
    void GetList (QString dir, QString & match);
    bool CompleteDir (QString &dir);
    QString directory;

    bool new_guess, completed_dir, self_update;
    QString the_text;
    QStrList possibilityList;
    QString guess;
    QString qual_dir;
};

#endif
