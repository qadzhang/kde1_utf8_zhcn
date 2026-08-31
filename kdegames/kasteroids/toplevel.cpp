
#include <kapp.h>
#include <qaccel.h>
#include <qmsgbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <kstatusbar.h>
#include <ktoolbar.h>
#include <ktopwidget.h>
#include <kmenubar.h>
#include <klocale.h>

// sound support
extern "C" {
#include <mediatool.h>
}
#include <kaudio.h>

#include "kfixedtopwidget.h"
#include "toplevel.h"
#include "version.h"

#include "toplevel.moc"

#include <X11/Xlib.h>

#define SB_SCORE	1
#define SB_LEVEL	2
#define SB_SHIPS	3


struct SLevel
{
    int    nrocks;
    double rockSpeed;
};

#define MAX_LEVELS	16

SLevel levels[MAX_LEVELS] =
{
    { 1, 0.6 },
    { 1, 0.8 },
    { 2, 0.7 },
    { 2, 0.8 },
    { 2, 0.9 },
    { 3, 0.7 },
    { 3, 0.8 },
    { 3, 0.9 },
    { 4, 0.7 },
    { 4, 0.8 },
    { 4, 0.9 },
    { 5, 0.7 },
    { 5, 0.8 },
    { 5, 0.9 },
    { 5, 1.0 }
};

const char *soundEvents[] = 
{
    "ShipDestroyed",
    "RockDestroyed",
    0
};

const char *soundDefaults[] = 
{
    "Explosion.wav",
    "ploop.wav",
    0
};

KAstTopLevel::KAstTopLevel() : KFixedTopWidget()
{
    setCaption( kapp->getCaption() );
    /* [2026-08-31] 音效字典接管元素生命周期（深拷贝入存，见 readSettings） */
    soundDict.setAutoDelete( TRUE );

    QWidget *mainWin = new QWidget( this );

    view = new KAsteroidsView( mainWin );
    connect( view, SIGNAL( shipKilled() ), SLOT( slotShipKilled() ) );
    connect( view, SIGNAL( rockHit(int) ), SLOT( slotRockHit(int) ) );
    connect( view, SIGNAL( rocksRemoved() ), SLOT( slotRocksRemoved() ) );

    QVBoxLayout *vb = new QVBoxLayout( mainWin );
    QHBoxLayout *hb = new QHBoxLayout;
    vb->addLayout( hb );

    QFont labelFont;              /* [2026-08-31] 默认族（helvetica 无 CJK 字形） */
    /* [2026-08-31] 24pt→14pt：1999 年 helvetica-24 位图字体实高仅 ~26px，
       TQt3/Xft 的 24pt 实高 48px（翻倍）——HUD 行、freeze 尺寸全按旧高度
       设计，调多大行高都跟随不及；14pt 实高 ~26px 与 1999 年视觉等价，
       布局回到原设计点 */
    labelFont.setPointSize( 14 );
    QColorGroup grp( darkGreen, black, QColor( 128, 128, 128 ),
	    QColor( 64, 64, 64 ), black, darkGreen, black );
    QPalette pal( grp, grp, grp );

    mainWin->setPalette( pal );

    hb->addSpacing( 10 );

    QLabel *label;
    label = new QLabel( klocale->translate("Score"), mainWin );
    label->setFont( labelFont );
    label->setPalette( pal );
    label->setFixedWidth( label->sizeHint().width() );
    hb->addWidget( label );

    scoreLCD = new QLCDNumber( 6, mainWin );
    scoreLCD->setFrameStyle( QFrame::NoFrame );
    scoreLCD->setSegmentStyle( QLCDNumber::Flat );
    scoreLCD->setFixedWidth( 150 );
    scoreLCD->setPalette( pal );
    hb->addWidget( scoreLCD );
    hb->addStretch( 10 );

    label = new QLabel( klocale->translate("Level"), mainWin );
    label->setFont( labelFont );
    label->setPalette( pal );
    label->setFixedWidth( label->sizeHint().width() );
    hb->addWidget( label );

    levelLCD = new QLCDNumber( 2, mainWin );
    levelLCD->setFrameStyle( QFrame::NoFrame );
    levelLCD->setSegmentStyle( QLCDNumber::Flat );
    levelLCD->setFixedWidth( 70 );
    levelLCD->setPalette( pal );
    hb->addWidget( levelLCD );
    hb->addStretch( 10 );

    label = new QLabel( klocale->translate("Ships"), mainWin );
    label->setFont( labelFont );
    label->setFixedWidth( label->sizeHint().width() );
    label->setPalette( pal );
    hb->addWidget( label );

    shipsLCD = new QLCDNumber( 1, mainWin );
    shipsLCD->setFrameStyle( QFrame::NoFrame );
    shipsLCD->setSegmentStyle( QLCDNumber::Flat );
    shipsLCD->setFixedWidth( 40 );
    shipsLCD->setPalette( pal );
    hb->addWidget( shipsLCD );

    /* [2026-08-31] 行高按字体度量计算：1999 年固定 strut=30 勉强装下
     位图 helvetica-24；Xft 抗锯齿字体下 24pt 中文标签约 40px——
     行高不足时整行（标签+七段数码管）被垂直居中、上缘裁进菜单栏
     （实测中文 HUD 只剩下半截）。取字体行高与 30 的较大者 + 边距 */
  QFontMetrics labelMetrics( labelFont );
  /* [2026-08-31] freeze 高度同步：原 640x480 按 30px HUD 行设计，
     行加高后总高不够 → 布局压缩 HUD 行（中文标签上半截被裁）。
     按新行高等比补足冻结高度 */
  int strutH = QMAX( 30, labelMetrics.height() + 6 );
  hb->addStrut( strutH );

    QFrame *sep = new QFrame( mainWin );
    sep->setMaximumHeight( 5 );
    sep->setFrameStyle( QFrame::HLine | QFrame::Raised );
    sep->setPalette( pal );

    vb->addWidget( sep );

    vb->addWidget( view, 10 );
    /* [2026-08-31] 原 vb->freeze(640,480) 把 mainWin 连菜单一起冻死在
       1999 年尺寸（按 30px HUD 行设计）：行加高后 KTMainWindow 的
       updateRects 仍按冻结尺寸布局，菜单栏叠进 mainWin 顶部，HUD 行
       上半截被压在菜单栏下。改为视图定尺寸 + 布局自适应：
       view 固定 640x480，行高由 strut 决定，窗口总高随之自然撑开 */
    view->setFixedSize( 640, 480 );
    /* [2026-08-31] 原 freeze 拆成两半：view 与 mainWin 均定死尺寸——
       KFixedTopWidget::updateRects 末尾的"固定尺寸视图"分支检测到
       min==max 后会把顶层窗口撑到 菜单栏+内容 的总高（1999 年 freeze
       在 TQt3 布局下不再撑窗，菜单栏叠进 HUD 行裁掉文字上半截） */
    mainWin->setFixedSize( 640, strutH + 5 + 480 );

    createMenuBar();

    menu->show();
    setView( mainWin );
    setMenu( menu );
    /* [2026-08-31] setMenu 只连接 moved 信号不触发布局——updateRects 从未
       在菜单可见的状态下跑过，kmainwidget 一直停留在 +1+1（菜单栏高度
       未计入），HUD 行上缘被压在菜单栏下。显式触发一次完整布局 */
    updateRects();

    setFocusPolicy( StrongFocus );
    setFocus();

    readSettings();

    kas = new KAudio;

    if ( kas->serverStatus() )
	sound = false;
}

KAstTopLevel::~KAstTopLevel()
{
    delete kas;
}

void KAstTopLevel::createMenuBar()
{
    menu = new KMenuBar( this );
    CHECK_PTR( menu );
    /* [2026-08-31] 显式置顶：默认位置解析为 Flat 时 KTMainWindow::
       updateRects 未把菜单高度计入内容区起点，菜单栏叠进 HUD 行
       （中文标签上半截被压在菜单栏下）——Top 分支保证 t += mh */
    menu->setMenuBarPos( KMenuBar::Top );

    QPopupMenu *fileMenu = new QPopupMenu;
    CHECK_PTR( fileMenu );
    fileMenu->insertItem( klocale->translate( "&New Game" ), this,
	SLOT(slotNewGame()), CTRL+Key_N );
    fileMenu->insertSeparator( );
    fileMenu->insertItem( klocale->translate( "&Quit" ), this, SLOT(slotQuit()),
	CTRL+Key_Q );

    QPopupMenu *help = kapp->getHelpMenu(true, QString(i18n("Asteroids"))
                                         + " " + KASTEROIDS_VERSION
                                         + i18n("\n\nby Martin R. Jones")
                                         + " (mjones@kde.org)"); 

    menu->insertItem( klocale->translate( "&File" ), fileMenu );
    menu->insertSeparator();
    menu->insertItem( klocale->translate( "&Help" ), help );
}

void KAstTopLevel::readSettings()
{
    KConfig *config = KApplication::getKApplication()->getConfig();
    config->setGroup( "Sounds" );

    QString qs;

    for ( int i = 0; soundEvents[i]; i++ )
    {
	/* [2026-08-31] 深拷贝入字典：原写法把 TQString 隐式转换的 const char*
	   缓冲地址存进 QDict<char>——qs 下轮重赋值即析构旧缓冲，播放时读到
	   已释放堆块（TQt3 下 data()/ascii() 缓冲随 TQStringData 生命周期） */
	qs = config->readEntry( soundEvents[i] );
	if ( !qs.isEmpty() )
	    soundDict.insert( soundEvents[i], qstrdup( qs ) );
	else
	    soundDict.insert( soundEvents[i], qstrdup( soundDefaults[i] ) );
    }

    qs = config->readEntry( "PlaySounds" );
    if ( qs.isEmpty() || qs != "Yes" )
	sound = false;
    else
	sound = true;
}

void KAstTopLevel::playSound( const char *snd )
{
    kas->stop();
    QString filename = kapp->kde_datadir();
    filename += "/kasteroids/";
    filename += soundDict[ snd ];
    kas->play( filename );
}

void KAstTopLevel::keyPressEvent( QKeyEvent *event )
{
    switch ( event->key() )
    {
	case Key_Left:
	    view->rotateLeft( true );
	    event->accept();
	    break;

	case Key_Right:
	    view->rotateRight( true );
	    event->accept();
	    break;

	case Key_Up:
	    view->thrust( true );
	    event->accept();
	    break;

	case Key_Space:
	    view->shoot( true );
	    event->accept();
	    break;

	default:
	    event->ignore();
    }
}

void KAstTopLevel::keyReleaseEvent( QKeyEvent *event )
{
    switch ( event->key() )
    {
	case Key_Left:
	    view->rotateLeft( false );
	    event->accept();
	    break;

	case Key_Right:
	    view->rotateRight( false );
	    event->accept();
	    break;

	case Key_Up:
	    view->thrust( false );
	    event->accept();
	    break;

	case Key_Return:
	    if ( waitShip )
	    {
		view->newShip();
		event->accept();
		waitShip = false;
		view->hideText();
	    }
	    break;

	case Key_Space:
	    view->shoot( false );
	    event->accept();
	    break;

	default:
	    event->ignore();
    }
}

void KAstTopLevel::focusInEvent( QFocusEvent * )
{
    XAutoRepeatOff( qt_xdisplay() );
}

void KAstTopLevel::focusOutEvent( QFocusEvent * )
{
    XAutoRepeatOn( qt_xdisplay() );
}

void KAstTopLevel::slotNewGame()
{
    score = 0;
    scoreLCD->display( 0 );
    level = 0;
    levelLCD->display( level );
    shipsRemain = 5;
    shipsLCD->display( shipsRemain-1 );
    view->newGame();
    view->setRockSpeed( levels[0].rockSpeed );
    view->addRocks( levels[0].nrocks );
    view->showText( klocale->translate( "Press Enter to launch." ), yellow );
    waitShip = true;
}

void KAstTopLevel::slotQuit()
{
    kapp->quit();
}

void KAstTopLevel::slotShipKilled()
{
    shipsRemain--;
    shipsLCD->display( shipsRemain-1 );

    playSound( "ShipDestroyed" );

    if ( shipsRemain )
    {
	waitShip = true;
	view->showText( klocale->translate( "Ship Destroyed.  Press Enter to launch."), yellow );
    }
    else
    {
	QMessageBox::message(klocale->translate("KAsteriods"),
	    klocale->translate("Game Over!"));
	view->endGame();
    }
}

void KAstTopLevel::slotRockHit( int size )
{
    switch ( size )
    {
	case 0:
	    score += 10;
	    break;

	case 1:
	    score += 20;
	    break;

	default:
	    score += 40;
    }

    playSound( "RockDestroyed" );

    scoreLCD->display( score );
}

void KAstTopLevel::slotRocksRemoved()
{
    level++;

    if ( level >= MAX_LEVELS )
	level = MAX_LEVELS - 1;

    view->setRockSpeed( levels[level-1].rockSpeed );
    view->addRocks( levels[level-1].nrocks );

    levelLCD->display( level );
}

