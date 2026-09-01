//
// KFM  Options
//
// (c) Martin R. Jones 1996
// (c) Bernd Wuebben 1998

// KControl port & modifications
// (c) Torben Weis 1998
// End of the KControl port, added 'kfmclient configure' call.
// (c) David Faure 1998

#include <ntqfontdatabase.h>
#include <ntqfontmetrics.h>
#include <ntqstringlist.h>
#include <qbttngrp.h>
#include <qchkbox.h>
#include <qcolor.h>
#include <qcombo.h>
#include <qlabel.h>
#include <qlayout.h>//CT - 12Nov1998
#include <qpushbt.h>
#include <qradiobt.h>
#include <kapp.h>
#include <kconfig.h>
#include <X11/Xlib.h>

#include "htmlopts.h"

#include "../../kfm/config-kfm.h" // include default values directly from kfm

//-----------------------------------------------------------------------------

KFontOptions::KFontOptions( QWidget *parent, const char *name )
    : KConfigWidget( parent, name )
{
    QLabel *label;

    //CT 12Nov1998 layout management
    QGridLayout *lay = new QGridLayout(this,8,5,10,5);
    lay->addRowSpacing(0,10);
    lay->addRowSpacing(4,10);
    lay->addRowSpacing(0,10);
    lay->addRowSpacing(3,10);

    lay->setRowStretch(0,0);
    lay->setRowStretch(1,1);
    lay->setRowStretch(2,1);
    lay->setRowStretch(3,0);
    lay->setRowStretch(4,0);
    lay->setRowStretch(5,0);
    lay->setRowStretch(6,0);
    lay->setRowStretch(7,10);

    lay->setColStretch(0,0);
    lay->setColStretch(1,1);
    lay->setColStretch(2,2);
    lay->setColStretch(3,0);
    //CT

    QButtonGroup *bg = new QButtonGroup( i18n("Font Size"), this );
    QGridLayout *bgLay = new QGridLayout(bg,2,3,10,5);
    bgLay->addRowSpacing(0,10);
    bgLay->setRowStretch(0,0);
    bgLay->setRowStretch(1,1);
    bg->setExclusive( TRUE );
  
    m_pSmall = new QRadioButton( i18n("Small"), bg );
    m_pSmall->adjustSize();
    m_pSmall->setMinimumSize(m_pSmall->size());
    bgLay->addWidget(m_pSmall,1,0);

    m_pMedium = new QRadioButton( i18n("Medium"), bg );
    m_pMedium->adjustSize();
    m_pMedium->setMinimumSize(m_pMedium->size());
    bgLay->addWidget(m_pMedium,1,1);

    m_pLarge = new QRadioButton( i18n("Large"), bg );
    m_pLarge->adjustSize();
    m_pLarge->setMinimumSize(m_pLarge->size());
    bgLay->addWidget(m_pLarge,1,2);

    bgLay->activate();
    lay->addMultiCellWidget(bg,1,1,1,2);


    label = new QLabel( i18n("Standard Font"), this );    label->adjustSize();
    label->setMinimumSize(label->size());
    lay->addWidget(label,3,1);

    m_pStandard = new QComboBox( false, this );
    m_pStandard->adjustSize();
    m_pStandard->setMinimumSize(m_pStandard->size());
    lay->addWidget(m_pStandard,3,2);

    getFontList( standardFonts, "-*-*-*-*-*-*-*-*-*-*-p-*-*-*" );
    m_pStandard->insertStrList( &standardFonts );
    // [KDE1 Revival 2026] TQComboBox 仅有 activated(int) 信号——槽内取文本
    connect( m_pStandard, SIGNAL( activated( int ) ),
             SLOT( slotStandardFont( int ) ) );
  
    label = new QLabel( i18n( "Fixed Font"), this );
    label->adjustSize();
    label->setMinimumSize(label->size());
    lay->addWidget(label,4,1);

    m_pFixed = new QComboBox( false, this );
    m_pFixed->adjustSize();
    m_pFixed->setMinimumSize(m_pFixed->size());
    lay->addWidget(m_pFixed,4,2);
    m_pFixed->setGeometry( 120, 130, 180, 25 );
    getFontList( fixedFonts, "-*-*-*-*-*-*-*-*-*-*-m-*-*-*" );
    m_pFixed->insertStrList( &fixedFonts );
  
    connect( m_pFixed, SIGNAL( activated( int ) ),
             SLOT( slotFixedFont( int ) ) );
  
    // default charset Lars Knoll 17Nov98 (moved by David)
    label = new QLabel( i18n( "Default Charset"), this );
    label->adjustSize();
    label->setMinimumSize(label->size());
    lay->addWidget(label,5,1);
    lay->activate();

    m_pCharset = new QComboBox( false, this );
    charsets = kapp->getCharsets()->available();
    charsets.insert(0, i18n("Use language charset"));
    m_pCharset->insertStrList( &charsets );

    m_pCharset->adjustSize();
    m_pCharset->setMinimumSize(m_pCharset->size());
    lay->addWidget(m_pCharset,5,2);
    connect( m_pCharset, SIGNAL( activated( int ) ),
             SLOT( slotCharset( int ) ) );

    connect( bg, SIGNAL( clicked( int ) ), SLOT( slotFontSize( int ) ) );

    loadSettings();
  
    setMinimumSize( 480, 180 );
}

void KFontOptions::getFontList( QStrList &list, const char *pattern )
{
    // ┌─ [KDE1 Revival 2026] 字体枚举 fontconfig 化（XListFonts 替换）
    // │  What : 以 TQFontDatabase::families() 枚举系统字族替换 1999 年的
    // │        XListFonts(XLFD)；pattern 参数退役——原调用方仅按
    // │        "-p-"(比例)/"-m-"(等宽) 两种模式区分，改为参数为
    // │        "-*-*-m-*" 类等宽模式时用 i/M/W advance 相等法过滤
    // │  Why  : 现代 Debian 的字体（Noto CJK 等）不经 X 核心字体体系注册，
    // │        XListFonts 列不出——kfm 字体设置页"标准/等宽字体"下拉
    // │        只剩遗留 XLFD 字体甚至全空（与控制中心字体页同族缺陷，
    // │        kfontdialog/fontchooser 已先行切换）
    // │  Who  : kfm 设置「字体」页（kcmkfm）标准/等宽字体下拉
    // │  When : KFontOptions 构造时一次性枚举
    // │  How  : fontconfig 全量字族 → strchr(pattern,'m') 判定等宽需求 →
    // │        advance 判定过滤 → 过滤 open look → 去重入表
    // └───────────────────────────────────────────────────────────────────
    bool want_fixed = ( strchr( pattern, 'm' ) != NULL );
    TQFontDatabase db;
    TQStringList fams = db.families();
    for ( TQStringList::Iterator it = fams.begin(); it != fams.end(); ++it ) {
        if ( want_fixed ) {
            TQFont probe( *it, 12 );
            TQFontMetrics fm( probe );
            if ( fm.width( 'i' ) != fm.width( 'M' )
                 || fm.width( 'M' ) != fm.width( 'W' ) )
                continue;
        }
        TQString font = *it;
        if ( font.find( "open look", 0, false ) >= 0 )
            continue;
        if ( list.find( font.latin1() ) != -1 )
            continue;
        list.append( font.latin1() );
    }
}

/* addFont(XLFD parse) removed with the XListFonts channel */

void KFontOptions::slotFontSize( int i )
{
    fSize = i+3;
}

void KFontOptions::slotStandardFont( int index )
{
    stdName = m_pStandard->text( index );
}

void KFontOptions::slotFixedFont( int index )
{
    fixedName = m_pFixed->text( index );
}

void KFontOptions::slotCharset( int index )
{
    charsetName = m_pCharset->text( index );
}

void KFontOptions::loadSettings()
{
    g_pConfig->setGroup( "KFM HTML Defaults" );		
    QString fs = g_pConfig->readEntry( "BaseFontSize" );  
    if ( !fs.isEmpty() )
    {
        fSize = fs.toInt();
        if ( fSize < 3 )
            fSize = 3;
        else if ( fSize > 5 )
            fSize = 5;
    }
    else
        fSize = 3;

    stdName = g_pConfig->readEntry( "StandardFont" );
    fixedName = g_pConfig->readEntry( "FixedFont" );
    charsetName = g_pConfig->readEntry( "DefaultCharset" );

    updateGUI();
}

void KFontOptions::defaultSettings()
{
    g_pConfig->setGroup( "KFM HTML Defaults" );			
    fSize=4;
    stdName = DEFAULT_VIEW_FONT;
    fixedName = DEFAULT_VIEW_FIXED_FONT;
    charsetName = "";

    updateGUI();
}

void KFontOptions::updateGUI()
{
    if ( stdName.isEmpty() )
        stdName = DEFAULT_VIEW_FONT;
    if ( fixedName.isEmpty() )
        fixedName = DEFAULT_VIEW_FIXED_FONT;  
    
    QStrListIterator sit( standardFonts );
    int i;
    for ( i = 0; sit.current(); ++sit, i++ )
    {
        if ( !strcmp( stdName, sit.current() ) )
            m_pStandard->setCurrentItem( i );
    }

    QStrListIterator fit( fixedFonts );
    for ( i = 0; fit.current(); ++fit, i++ )
    {
        if ( !strcmp( fixedName, fit.current() ) )
            m_pFixed->setCurrentItem( i );
    }

    QStrListIterator cit( charsets );
    for ( i = 0; cit.current(); ++cit, i++ )
    {
        if ( !strcmp( charsetName, cit.current() ) )
            m_pCharset->setCurrentItem( i );
    }

    m_pSmall->setChecked( fSize == 3 );
    m_pMedium->setChecked( fSize == 4 );
    m_pLarge->setChecked( fSize == 5 );
}

void KFontOptions::saveSettings()
{
    g_pConfig->setGroup( "KFM HTML Defaults" );			
    g_pConfig->writeEntry( "BaseFontSize", fSize );
    g_pConfig->writeEntry( "StandardFont", stdName );
    g_pConfig->writeEntry( "FixedFont", fixedName );
    // If the user chose "Use language charset", write an empty string
    if (!strcmp(charsetName,i18n("Use language charset")))
        charsetName = "";
    g_pConfig->writeEntry( "DefaultCharset", charsetName );
    g_pConfig->sync();
}

void KFontOptions::applySettings()
{
    saveSettings();
}

//-----------------------------------------------------------------------------

KColorOptions::KColorOptions( QWidget *parent, const char *name )
    : KConfigWidget( parent, name )
{
    QLabel *label;

    //CT 12Nov1998 layout management
    QGridLayout *lay = new QGridLayout(this,12,5,10,5);
    lay->addRowSpacing(0,10);
    lay->addRowSpacing(1,30);
    lay->addRowSpacing(2, 5);
    lay->addRowSpacing(3,30);
    lay->addRowSpacing(4, 5);
    lay->addRowSpacing(5,30);
    lay->addRowSpacing(6, 5);
    lay->addRowSpacing(7,30);
    lay->addRowSpacing(11,10);
    lay->addColSpacing(0,10);
    lay->addColSpacing(2,20);
    lay->addColSpacing(3,80);
    lay->addColSpacing(4,10);

    lay->setRowStretch(0,0);
    lay->setRowStretch(1,0);
    lay->setRowStretch(2,1);
    lay->setRowStretch(3,0);
    lay->setRowStretch(4,1);
    lay->setRowStretch(5,0);
    lay->setRowStretch(6,1);
    lay->setRowStretch(7,0);
    lay->setRowStretch(8,1);
    lay->setRowStretch(9,1);
    lay->setRowStretch(10,1);
    lay->setRowStretch(11,0);

    lay->setColStretch(0,0);
    lay->setColStretch(1,0);
    lay->setColStretch(2,1);
    lay->setColStretch(3,0);
    lay->setColStretch(4,1);
    //CT

    label = new QLabel( i18n("Background Color:"), this );
    label->adjustSize();
    label->setMinimumSize(label->size());
    lay->addWidget(label,1,1);

    m_pBg = new KColorButton( bgColor, this );
    m_pBg->adjustSize();
    m_pBg->setMinimumSize(m_pBg->size());
    lay->addWidget(m_pBg,1,3);
    connect( m_pBg, SIGNAL( changed( const QColor & ) ),
             SLOT( slotBgColorChanged( const QColor & ) ) );

    label = new QLabel( i18n("Normal Text Color:"), this );
    label->adjustSize();
    label->setMinimumSize(label->size());
    lay->addWidget(label,3,1);
  
    m_pText = new KColorButton( textColor, this );
    m_pText->adjustSize();
    m_pText->setMinimumSize(m_pText->size());
    lay->addWidget(m_pText,3,3);
    connect( m_pText, SIGNAL( changed( const QColor & ) ),
             SLOT( slotTextColorChanged( const QColor & ) ) );

    label = new QLabel( i18n("URL Link Color:"), this );
    label->adjustSize();
    label->setMinimumSize(label->size());
    lay->addWidget(label,5,1);

    m_pLink = new KColorButton( linkColor, this );
    m_pLink->adjustSize();
    m_pLink->setMinimumSize(m_pLink->size());
    lay->addWidget(m_pLink,5,3);
    connect( m_pLink, SIGNAL( changed( const QColor & ) ),
             SLOT( slotLinkColorChanged( const QColor & ) ) );

    label = new QLabel( i18n("Followed Link Color:"), this );
    label->adjustSize();
    label->setMinimumSize(label->size());
    lay->addWidget(label,7,1);

    m_pVLink = new KColorButton( vLinkColor, this );
    m_pVLink->adjustSize();
    m_pVLink->setMinimumSize(m_pVLink->size());
    lay->addWidget(m_pVLink,7,3);
    connect( m_pVLink, SIGNAL( changed( const QColor & ) ),
             SLOT( slotVLinkColorChanged( const QColor & ) ) );

    cursorbox = new QCheckBox(i18n("Change cursor over link."),
                              this);
    cursorbox->adjustSize();
    cursorbox->setMinimumSize(cursorbox->size());
    lay->addMultiCellWidget(cursorbox,8,8,1,3);

    underlinebox = new QCheckBox(i18n("Underline links"),
                                 this);

    underlinebox->adjustSize();
    underlinebox->setMinimumSize(underlinebox->size());
    lay->addMultiCellWidget(underlinebox,9,9,1,3);

    forceDefaultsbox = new QCheckBox(i18n("Always use my colors"),
                                 this);

    forceDefaultsbox->adjustSize();
    forceDefaultsbox->setMinimumSize(forceDefaultsbox->size());
    lay->addMultiCellWidget(forceDefaultsbox,10,10,1,3);

    loadSettings();

    setMinimumSize( 400, 180 );
}

void KColorOptions::slotBgColorChanged( const QColor &col )
{
    if ( bgColor != col )
        bgColor = col;
}

void KColorOptions::slotTextColorChanged( const QColor &col )
{
    if ( textColor != col )
        textColor = col;
}

void KColorOptions::slotLinkColorChanged( const QColor &col )
{
    if ( linkColor != col )
        linkColor = col;
}

void KColorOptions::slotVLinkColorChanged( const QColor &col )
{
    if ( vLinkColor != col )
        vLinkColor = col;
}

void KColorOptions::loadSettings()
{
    g_pConfig->setGroup( "KFM HTML Defaults" );	
    bgColor = g_pConfig->readColorEntry( "BgColor", &HTML_DEFAULT_BG_COLOR );
    textColor = g_pConfig->readColorEntry( "TextColor", &HTML_DEFAULT_TXT_COLOR );
    linkColor = g_pConfig->readColorEntry( "LinkColor", &HTML_DEFAULT_LNK_COLOR );
    vLinkColor = g_pConfig->readColorEntry( "VLinkColor", &HTML_DEFAULT_VLNK_COLOR);
    bool changeCursor = g_pConfig->readBoolEntry("ChangeCursor", false);
    bool underlineLinks = g_pConfig->readBoolEntry("UnderlineLinks", true);
    bool forceDefaults = g_pConfig->readBoolEntry("ForceDefaultColors", false);

    m_pBg->setColor( bgColor );
    m_pText->setColor( textColor );
    m_pLink->setColor( linkColor );
    m_pVLink->setColor( vLinkColor );
    cursorbox->setChecked( changeCursor );
    underlinebox->setChecked( underlineLinks );
    forceDefaultsbox->setChecked( forceDefaults );
}

void KColorOptions::defaultSettings()
{
    bgColor = HTML_DEFAULT_BG_COLOR;
    textColor = HTML_DEFAULT_TXT_COLOR;
    linkColor = HTML_DEFAULT_LNK_COLOR;
    vLinkColor = HTML_DEFAULT_VLNK_COLOR;

    m_pBg->setColor( bgColor );
    m_pText->setColor( textColor );
    m_pLink->setColor( linkColor );
    m_pVLink->setColor( vLinkColor );
    cursorbox->setChecked( false );
    underlinebox->setChecked( false );
    forceDefaultsbox->setChecked( false );
}

void KColorOptions::saveSettings()
{
    g_pConfig->setGroup( "KFM HTML Defaults" );			
    g_pConfig->writeEntry( "BgColor", bgColor );
    g_pConfig->writeEntry( "TextColor", textColor );
    g_pConfig->writeEntry( "LinkColor", linkColor);
    g_pConfig->writeEntry( "VLinkColor", vLinkColor );
    g_pConfig->writeEntry( "ChangeCursor", cursorbox->isChecked() );
    g_pConfig->writeEntry( "UnderlineLinks", underlinebox->isChecked() );
    g_pConfig->writeEntry("ForceDefaultColors", forceDefaultsbox->isChecked() );
    g_pConfig->sync();
}

void KColorOptions::applySettings()
{
    saveSettings();
}

#include "htmlopts.moc"
