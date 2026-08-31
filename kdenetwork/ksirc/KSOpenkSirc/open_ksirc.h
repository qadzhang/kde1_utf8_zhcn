/**********************************************************************

	--- Qt Architect generated file ---

	File: open_ksirc.h
	Last generated: Wed Jul 29 16:41:26 1998

 *********************************************************************/

#ifndef open_ksirc_included
#define open_ksirc_included

#include "open_ksircData.h"

class open_ksirc : public open_ksircData
{
    Q_OBJECT

public:

    open_ksirc
    (
        QWidget* parent = NULL,
        const char* name = NULL
    );

    virtual ~open_ksirc();

protected slots:
  // [KDE1 Revival 2026] 两槽改收 int：与 activated(int) 连接匹配
  void setGroup( int );
  void setServer( int );
  void clickConnect();
  void clickCancel();
  void clickEdit();

private:
  void insertGroupList();
  void insertServerList( const char * );
  void setServerDesc( QString );

signals:
  void open_ksircprocess( const char *, int, const char * );
  void open_ksircprocess( QString s );

};
#endif // open_ksirc_included
