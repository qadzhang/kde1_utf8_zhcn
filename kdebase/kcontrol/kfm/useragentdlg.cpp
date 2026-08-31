//-----------------------------------------------------------------------------
//
// UserAgent Options
// (c) Kalle Dalheimer 1997
//
// Port to KControl
// (c) David Faure <faure@kde.org> 1998

#include "useragentdlg.h"

#include <kapp.h>
#include <klocale.h>

#include <qlayout.h> //CT

#include "../../kfm/config-kfm.h"

UserAgentOptions::UserAgentOptions( QWidget * parent, const char * name ) :
  KConfigWidget( parent, name )
{

  //CT 12Nov1998 layout management
  QGridLayout *lay = new QGridLayout(this,7,5,10,5);
  lay->addRowSpacing(0,10);
  lay->addRowSpacing(3,25);
  lay->addRowSpacing(6,10);
  lay->addColSpacing(0,10);
  lay->addColSpacing(4,10);

  lay->setRowStretch(0,0);
  lay->setRowStretch(1,0);
  lay->setRowStretch(2,0);
  lay->setRowStretch(3,0);
  lay->setRowStretch(4,0);
  lay->setRowStretch(5,1);
  lay->setRowStretch(6,0);

  lay->setColStretch(0,0);
  lay->setColStretch(1,0);
  lay->setColStretch(2,1);
  lay->setColStretch(3,1);
  lay->setColStretch(4,0);
  //CT

  onserverLA = new QLabel( i18n( "On server:" ), this );
  onserverLA->setAlignment( AlignRight|AlignVCenter );
  //CT 12Nov1998 layout management
  onserverLA->adjustSize();
  onserverLA->setMinimumSize(onserverLA->size());
  lay->addWidget(onserverLA,1,1);
  //CT

  onserverED = new QLineEdit( this );
  //CT 12Nov1998 layout management
  onserverED->adjustSize();
  onserverED->setMinimumSize(onserverED->size());
  lay->addWidget(onserverED,1,2);
  //CT

  // [KDE1 Revival 2026] TQLineEdit 只有 textChanged(const TQString&)——const char* 版连接静默失败
  connect( onserverED, SIGNAL( textChanged( const TQString& ) ),
		   SLOT( textChanged( const TQString& ) ) );
  // Commented out because in a kcontrol module  Return => OK
  // One can use 'Alt+A' instead (English version at least)
  //connect( onserverED, SIGNAL( returnPressed() ),
  //		   SLOT( returnPressed() ) );

  loginasLA = new QLabel( i18n( "login as:" ), this );
  loginasLA->setAlignment( AlignRight|AlignVCenter );
  //CT 12Nov1998 layout management
  loginasLA->adjustSize();
  loginasLA->setMinimumSize(loginasLA->size());
  lay->addWidget(loginasLA,2,1);
  //CT

  loginasED = new QLineEdit( this );
  //CT 12Nov1998 layout management
  loginasED->adjustSize();
  loginasED->setMinimumSize(loginasED->size());
  lay->addWidget(loginasED,2,2);
  //CT

  // [KDE1 Revival 2026] 同上
  connect( loginasED, SIGNAL( textChanged( const TQString& ) ),
		   SLOT( textChanged( const TQString& ) ) );
  // connect( loginasED, SIGNAL( returnPressed() ),
  //  SLOT( returnPressed() ) );

  addPB = new QPushButton( i18n( "&Add" ), this );
  //CT 12Nov1998 layout management
  addPB->adjustSize();
  addPB->setMinimumSize(addPB->size());
  lay->addWidget(addPB,1,3);
  //CT

  addPB->setEnabled( false );
  connect( addPB, SIGNAL( clicked() ), SLOT( addClicked() ) );
  
  deletePB = new QPushButton( i18n( "&Delete" ), this );
  //CT 12Nov1998 layout management
  deletePB->adjustSize();
  deletePB->setMinimumSize(deletePB->size());
  lay->addWidget(deletePB,2,3);
  //CT

  deletePB->setEnabled( false );
  connect( deletePB, SIGNAL( clicked() ), SLOT( deleteClicked() ) );

  bindingsLA = new QLabel( i18n( "Known bindings:" ), this );
  //CT 12Nov1998 layout management
  bindingsLA->adjustSize();
  bindingsLA->setMinimumSize(bindingsLA->size());
  lay->addMultiCellWidget(bindingsLA,4,4,2,3);
  //CT

  bindingsLB = new QListBox( this );
  //CT 12Nov1998 layout management
  bindingsLB->adjustSize();
  bindingsLB->setMinimumSize(bindingsLB->size());
  lay->addMultiCellWidget(bindingsLB,5,5,2,3);

  lay->activate();
  //CT

  bindingsLB->setMultiSelection( false );
  bindingsLB->setScrollBar( true );
  // [KDE1 Revival 2026] TQListBox 仅有 highlighted(int) 信号
  connect( bindingsLB, SIGNAL( highlighted( int ) ),
		   SLOT( listboxHighlighted( int ) ) );

  loadSettings();
  setMinimumSize(480,300);
}


UserAgentOptions::~UserAgentOptions()
{
}

static QStrList settingsList( true ); // convenience var.

void UserAgentOptions::loadSettings()
{
  // read entries for UserAgentDlg
  g_pConfig->setGroup( "Browser Settings/UserAgent" );
  int entries = g_pConfig->readNumEntry( "EntriesCount", 0 );
  settingsList.clear();
  for( int i = 0; i < entries; i++ )
        {
          QString key;
          key.sprintf( "Entry%d", i );
          QString entry = g_pConfig->readEntry( key, "" );
          if (entry.left( 12 ) == "*:Konqueror/") // update version number
            settingsList.append( QString("*:"+DEFAULT_USERAGENT_STRING) );
          else
            settingsList.append( entry );
        }
  if( entries == 0 )
    defaultSettings();
  bindingsLB->clear();
  bindingsLB->insertStrList( &settingsList );
}

void UserAgentOptions::defaultSettings()
{
  bindingsLB->clear();
  bindingsLB->insertItem( QString("*:"+DEFAULT_USERAGENT_STRING) );
}

void UserAgentOptions::applySettings()
{
  saveSettings();
}

void UserAgentOptions::saveSettings()
{
    // write back the entries from UserAgent
    g_pConfig->setGroup("Browser Settings/UserAgent");

    if(!bindingsLB->count())
      defaultSettings();

    g_pConfig->writeEntry( "EntriesCount", bindingsLB->count() );
    for( uint i = 0; i < bindingsLB->count(); i++ ) {
      QString key;
      key.sprintf( "Entry%d", i );
      g_pConfig->writeEntry( key, bindingsLB->text( i ) );
    }
    g_pConfig->sync();
}

void UserAgentOptions::textChanged( const TQString& )
{
  // [KDE1 Revival 2026] 原 const char* 悬垂指针（text() 临时对象语句末析构）一并修掉
  TQString login = loginasED->text();
  TQString server = onserverED->text();

  if( !login.isEmpty() && !server.isEmpty() )
	addPB->setEnabled( true );
  else
	addPB->setEnabled( false );

  deletePB->setEnabled( false );
}

void UserAgentOptions::addClicked()
{
  // no need to check if the fields contain text, the add button is only
  // enabled if they do

  QString text = onserverED->text();
  text += ':';
  text += loginasED->text();
  bindingsLB->insertItem( new QListBoxText( text.data() ), 0);
  onserverED->setText( "" );
  loginasED->setText( "" );
  onserverED->setFocus();
}


void UserAgentOptions::deleteClicked()
{
  if( bindingsLB->count() ) 
	bindingsLB->removeItem( highlighted_item );
  if( !bindingsLB->count() ) // no more items
    listboxHighlighted( 0 );  
}


void UserAgentOptions::listboxHighlighted( int index )
{
  // [KDE1 Revival 2026] TQListBox 仅有 highlighted(int)——槽内按下标取文本
  QString itemtext = bindingsLB->text( index );
  int colonpos = itemtext.find( ':' );
  onserverED->setText( itemtext.left( colonpos ) );
  loginasED->setText( itemtext.right( itemtext.length() - colonpos - 1) );
  deletePB->setEnabled( true );
  addPB->setEnabled( false );

  highlighted_item = bindingsLB->currentItem();
}

#include "useragentdlg.moc"
