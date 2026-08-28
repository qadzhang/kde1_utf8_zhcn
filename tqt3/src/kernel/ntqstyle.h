/****************************************************************************
**
** Definition of TQStyle class
**
** Created : 980616
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of the kernel module of the TQt GUI Toolkit.
**
** This file may be used under the terms of the GNU General
** Public License versions 2.0 or 3.0 as published by the Free
** Software Foundation and appearing in the files LICENSE.GPL2
** and LICENSE.GPL3 included in the packaging of this file.
** Alternatively you may (at your option) use any later version
** of the GNU General Public License if such license has been
** publicly approved by Trolltech ASA (or its successors, if any)
** and the KDE Free TQt Foundation.
**
** Please review the following information to ensure GNU General
** Public Licensing requirements will be met:
** http://trolltech.com/products/qt/licenses/licensing/opensource/.
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://trolltech.com/products/qt/licenses/licensing/licensingoverview
** or contact the sales department at sales@trolltech.com.
**
** This file may be used under the terms of the Q Public License as
** defined by Trolltech ASA and appearing in the file LICENSE.TQPL
** included in the packaging of this file.  Licensees holding valid TQt
** Commercial licenses may use this file in accordance with the TQt
** Commercial License Agreement provided with the Software.
**
** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE. Trolltech reserves all rights not granted
** herein.
**
**********************************************************************/
#ifndef TQSTYLE_H
#define TQSTYLE_H

#ifndef QT_H
#include "ntqobject.h"
#include "ntqpixmap.h"
#include "ntqcolor.h"
#include "ntqiconset.h"
#include "ntqtabbar.h"
#include "ntqtoolbutton.h"
#endif // QT_H


#ifndef TQT_NO_STYLE

class TQPopupMenu;
class TQStylePrivate;
class TQMenuItem;
class TQTab;
class TQListViewItem;
class TQCheckListItem;

class TQ_EXPORT TQStyleOption {
public:
    enum StyleOptionDefault { Default };

    TQStyleOption(StyleOptionDefault=Default);
    TQStyleOption(int in1);
    TQStyleOption(int in1, int in2);
    TQStyleOption(int in1, int in2, int in3, int in4);
    TQStyleOption(TQMenuItem* m);
    TQStyleOption(TQMenuItem* m, int in1);
    TQStyleOption(TQMenuItem* m, int in1, int in2);
    TQStyleOption(const TQColor& c);
    TQStyleOption(TQTab* t);
    TQStyleOption(TQListViewItem* i);
    TQStyleOption(TQCheckListItem* i);
    TQStyleOption(TQt::ArrowType a);
    TQStyleOption(const TQRect& r);
    TQStyleOption(TQWidget *w);

    bool isDefault() const { return def; }

    int day() const { return i1; }

    int lineWidth() const { return i1; }
    int midLineWidth() const { return i2; }
    int frameShape() const { return i3; }
    int frameShadow() const { return i4; }

    int headerSection() const { return i1; }
    TQMenuItem* menuItem() const { return mi; }
    int maxIconWidth() const { return i1; }
    int tabWidth() const { return i2; }

    const TQColor& color() const { return *cl; }

    TQTab* tab() const { return tb; }

    TQCheckListItem* checkListItem() const { return cli; }
    TQListViewItem* listViewItem() const { return li; }

    TQt::ArrowType arrowType() const { return (TQt::ArrowType)i1; }
    TQRect rect() const { return TQRect( i1, i2, i3, i4 ); }
    TQWidget* widget() const { return (TQWidget*)p1; }

    TQStyleOption(TQTab* t, TQTab* h) : def(false), tb(t), cli(NULL), tbh(h) {}
    TQTab* hoverTab() const { return tbh; }

private:
    // NOTE: none of these components have constructors.
    bool def;
    bool b1,b2,b3; // reserved
    TQMenuItem* mi;
    TQTab* tb;
    TQListViewItem* li;
    const TQColor* cl;
    int i1, i2, i3, i4;
    int i5, i6; // reserved
    TQCheckListItem* cli;
    void *p1, *p2, *p3, *p4; // reserved
    TQTab* tbh;
    // (padded to 64 bytes on some architectures)
};

class TQStyleHintReturn; // not defined yet

typedef TQMap<TQ_UINT32, TQSize> DialogButtonSizeMap;
typedef TQMap<TQ_INT32, TQ_INT32> TabIdentifierIndexMap;

class TQStyleControlElementGenericWidgetData;

class TQStyleControlElementPopupMenuData {
	public:
		//
};

class TQStyleControlElementCheckListItemData {
	public:
		bool dataValid;
		bool enabled;
		bool hasParent;
		int height;
};

class TQStyleControlElementListViewData {
	public:
		bool rootDecorated;
		int itemMargin;
};

class TQStyleControlElementSpinWidgetData {
	public:
		TQ_UINT32 buttonSymbols;
		TQRect upRect;
		TQRect downRect;
		bool upEnabled;
		bool downEnabled;
};

class TQStyleControlElementTitleBarData {
	public:
		bool hasWindow;
		bool usesActiveColor;
		int windowState;
		TQString visibleText;
};

class TQStyleControlElementDockWidgetData {
	public:
		bool hasDockArea;
		bool closeEnabled;
		TQt::Orientation areaOrientation;
};

class TQStyleControlElementToolBarWidgetData {
	public:
		TQt::Orientation orientation;
};

class TQStyleControlElementGenericWidgetData {
	public:
		TQStringList widgetObjectTypes;
		bool allDataPopulated;
		TQt::WFlags wflags;
		TQt::WindowState windowState;
		TQPixmap bgPixmap;
		TQBrush bgBrush;
		TQColor bgColor;
		TQPoint bgOffset;
		TQt::BackgroundMode backgroundMode;
		TQColor fgColor;
		TQColorGroup colorGroup;
		TQRect geometry;
		TQRect rect;
		TQPoint pos;
		TQPixmap icon;
		TQPalette palette;
		TQFont font;
		TQColor paletteBgColor;
		TQString name;
		TQString caption;
};

class TQStyleControlElementTabBarData {
	public:
		int tabCount;
		int currentTabIndex;
		TQTabBar::Shape shape;
		TabIdentifierIndexMap identIndexMap;
#ifdef Q_QDOC
#else
		TQStyleControlElementGenericWidgetData cornerWidgets[4];
#endif

		enum CornerWidgetLocation {
			CWL_TopLeft			= 0,
			CWL_TopRight			= 1,
			CWL_BottomLeft			= 2,
			CWL_BottomRight			= 3
		};
};

class TQ_EXPORT TQStyleControlElementData {
	public:
		TQStyleControlElementData();
		~TQStyleControlElementData();

	public:
		bool isNull;
		TQStringList widgetObjectTypes;
		bool allDataPopulated;
		TQt::WFlags wflags;
		TQt::WindowState windowState;
		TQPixmap bgPixmap;
		TQBrush bgBrush;
		TQColor bgColor;
		TQPoint bgOffset;
		TQt::BackgroundMode backgroundMode;
		TQPixmap fgPixmap;
		TQColor fgColor;
		TQColorGroup colorGroup;
		TQRect geometry;
		TQRect rect;
		TQPoint pos;
		TQPixmap icon;
		TQIconSet iconSet;
		TQString text;
		TQt::Orientation orientation;
		TQColor activeItemPaletteBgColor;
		TQPalette palette;
		int totalSteps;
		int currentStep;
		TQ_UINT32 tickMarkSetting;
		int tickInterval;
		int minSteps;
		int maxSteps;
		int startStep;
		int pageStep;
		int lineStep;
		int dlgVisibleButtons;
		DialogButtonSizeMap dlgVisibleSizeHints;
		TQString progressText;
		TQString textLabel;
		TQFont font;
		int percentageVisible;
		TQStyleControlElementDockWidgetData dwData;
		TQToolButton::TextPosition toolButtonTextPosition;
		int popupDelay;
		TQStyleControlElementTitleBarData titleBarData;
		TQStyleControlElementSpinWidgetData spinWidgetData;
		TQStyleControlElementGenericWidgetData parentWidgetData;
		TQStyleControlElementGenericWidgetData viewportData;
		TQStyleControlElementListViewData listViewData;
		TQStyleControlElementTabBarData tabBarData;
		TQStyleControlElementCheckListItemData checkListItemData;
		TQ_UINT32 comboBoxLineEditFlags;
		TQ_UINT32 frameStyle;
		TQRect sliderRect;
		TQPainter* activePainter;
		TQStyleControlElementToolBarWidgetData toolBarData;
		TQ_UINT32 comboBoxListBoxFlags;
		TQColor paletteBgColor;
		TQ_UINT32 parentWidgetFlags;
		TQString name;
		TQString caption;
		TQStyleControlElementGenericWidgetData topLevelWidgetData;
		TQ_UINT32 topLevelWidgetFlags;
		TQPixmap paletteBgPixmap;

#ifdef ENABLE_TQSTYLECONTROLELEMENTDATA_SLOW_COPY
	public:
		TQStyleControlElementData(const TQStyleControlElementData&);
#else
	private:
		// Disable copy constructor
		TQStyleControlElementData(const TQStyleControlElementData&);
#endif

};

class TQ_EXPORT TQStyleWidgetActionRequestData {
	public:
		TQStyleWidgetActionRequestData();
		TQStyleWidgetActionRequestData(int metric1, int metric2=0);
		TQStyleWidgetActionRequestData(TQPalette palette, bool informWidgets = false, const char* className = 0);
		TQStyleWidgetActionRequestData(TQFont font, bool informWidgets = false, const char* className = 0);
		TQStyleWidgetActionRequestData(TQRect rect);
		TQStyleWidgetActionRequestData(TQPaintEvent* paintEvent);
		~TQStyleWidgetActionRequestData();
	public:
		bool bool1;
		bool bool2;
		int metric1;
		int metric2;
		TQPalette palette;
		TQFont font;
		TQRect rect;
		const char * cstr;
		TQString string;
		TQPaintEvent * paintEvent;
};

typedef TQStyleWidgetActionRequestData TQStyleApplicationActionRequestData;

class TQ_EXPORT TQStyle: public TQObject
{
    TQ_OBJECT

public:
    TQStyle();
    virtual ~TQStyle();

    enum ControlElementFlags {
	CEF_None			= 0x00000000,
	CEF_IsDefault			= 0x00000001,
	CEF_AutoDefault			= 0x00000002,
	CEF_IsActive			= 0x00000004,
	CEF_IsDown			= 0x00000008,
	CEF_IsOn			= 0x00000010,
	CEF_IsEnabled			= 0x00000020,
	CEF_BiState			= 0x00000040,
	CEF_HasFocus			= 0x00000080,
	CEF_IsMenuWidget		= 0x00000100,
	CEF_IsContainerEmpty		= 0x00000200,
	CEF_CenterIndicator		= 0x00000400,
	CEF_IndicatorFollowsStyle	= 0x00000800,
	CEF_UsesTextLabel		= 0x00001000,
	CEF_UsesBigPixmap		= 0x00002000,
	CEF_UseGenericParameters	= 0x00004000,
	CEF_HasParentWidget		= 0x00008000,
	CEF_HasPopupMenu		= 0x00010000,
	CEF_IsCheckable			= 0x00020000,
	CEF_HasFocusProxy		= 0x00040000,
	CEF_IsEditable			= 0x00080000,
	CEF_IsFlat			= 0x00100000,
	CEF_IsActiveWindow		= 0x00200000,
	CEF_IsTopLevel			= 0x00400000,
	CEF_IsVisible			= 0x00800000,
	CEF_IsShown			= 0x01000000,
	CEF_HasMouse			= 0x02000000
    };

    // New TQStyle API - most of these should probably be pure virtual

    // Old API
    // DEPRECATED
    virtual void polish( TQWidget * );

    // New API
    virtual void polish( const TQStyleControlElementData &ceData, ControlElementFlags elementFlags, void * );

    // Old API
    // DEPRECATED
    virtual void unPolish( TQWidget * );

    // New API
    virtual void unPolish( const TQStyleControlElementData &ceData, ControlElementFlags elementFlags, void * );

    // Old API
    // DEPRECATED
    virtual void polish( TQApplication * );

    // New API
    virtual void applicationPolish( const TQStyleControlElementData &ceData, ControlElementFlags elementFlags, void * );

    // Old API
    // DEPRECATED
    virtual void unPolish( TQApplication * );

    // New API
    virtual void applicationUnPolish( const TQStyleControlElementData &ceData, ControlElementFlags elementFlags, void * );

    virtual void polish( TQPalette & );

    // Old API
    // DEPRECATED
    virtual void polishPopupMenu( TQPopupMenu* );

    // New API
    virtual void polishPopupMenu( const TQStyleControlElementData &ceData, ControlElementFlags elementFlags, void * ) = 0;

    virtual TQRect itemRect( TQPainter *p, const TQRect &r,
			    int flags, bool enabled,
			    const TQPixmap *pixmap,
			    const TQString &text, int len = -1 ) const;

    virtual void drawItem( TQPainter *p, const TQRect &r,
			   int flags, const TQColorGroup &g, bool enabled,
			   const TQPixmap *pixmap, const TQString &text,
			   int len = -1, const TQColor *penColor = 0 ) const;

    enum PrimitiveElement {
	PE_ButtonCommand,
	PE_ButtonDefault,
	PE_ButtonBevel,
	PE_ButtonTool,
	PE_ButtonDropDown,

	PE_FocusRect,

	PE_ArrowUp,
	PE_ArrowDown,
	PE_ArrowRight,
	PE_ArrowLeft,

	PE_SpinWidgetUp,
	PE_SpinWidgetDown,
	PE_SpinWidgetPlus,
	PE_SpinWidgetMinus,

	PE_Indicator,
	PE_IndicatorMask,
	PE_ExclusiveIndicator,
	PE_ExclusiveIndicatorMask,

	PE_DockWindowHandle,
	PE_DockWindowSeparator,
	PE_DockWindowResizeHandle,

	PE_Splitter,

	PE_Panel,
	PE_PanelPopup,
	PE_PanelMenuBar,
	PE_PanelDockWindow,

	PE_TabBarBase,

	PE_HeaderSection,
	PE_HeaderArrow,
	PE_StatusBarSection,

	PE_GroupBoxFrame,

	PE_Separator,

	PE_SizeGrip,

	PE_CheckMark,

	PE_ScrollBarAddLine,
	PE_ScrollBarSubLine,
	PE_ScrollBarAddPage,
	PE_ScrollBarSubPage,
	PE_ScrollBarSlider,
	PE_ScrollBarFirst,
	PE_ScrollBarLast,

	PE_ProgressBarChunk,

	PE_PanelLineEdit,
	PE_PanelTabWidget,

	PE_WindowFrame,

	PE_CheckListController,
	PE_CheckListIndicator,
	PE_CheckListExclusiveIndicator,

	PE_PanelGroupBox,
	PE_RubberBand,

	PE_HeaderSectionMenu,

	PE_PanelScrollBar,
	PE_MenuItemIndicatorFrame,
	PE_MenuItemIndicatorIconFrame,
	PE_MenuItemIndicatorCheck,

	PE_ScrollBarTrough,

	// do not add any values below/greater this
	PE_CustomBase =			0xf000000
    };

    enum StyleFlags {
	Style_Default = 		0x00000000,
	Style_Enabled = 		0x00000001,
	Style_Raised =			0x00000002,
	Style_Sunken =	 		0x00000004,
	Style_Off =			0x00000008,
	Style_NoChange =		0x00000010,
	Style_On =			0x00000020,
	Style_Down =			0x00000040,
	Style_Horizontal =		0x00000080,
	Style_HasFocus =		0x00000100,
	Style_Top =			0x00000200,
	Style_Bottom =			0x00000400,
	Style_FocusAtBorder =		0x00000800,
	Style_AutoRaise =		0x00001000,
	Style_MouseOver =		0x00002000,
	Style_Up =			0x00004000,
	Style_Selected =	 	0x00008000,
	Style_Active =			0x00010000,
	Style_ButtonDefault =		0x00020000
    };
    typedef uint SFlags;

    // Old API
    // DEPRECATED
    virtual void drawPrimitive( PrimitiveElement pe,
				TQPainter *p,
				const TQRect &r,
				const TQColorGroup &cg,
				SFlags flags = Style_Default,
				const TQStyleOption& = TQStyleOption::Default ) const;

    // New API
    virtual void drawPrimitive( PrimitiveElement pe,
				TQPainter *p,
				const TQStyleControlElementData &ceData,
				ControlElementFlags elementFlags,
				const TQRect &r,
				const TQColorGroup &cg,
				SFlags flags = Style_Default,
				const TQStyleOption& = TQStyleOption::Default ) const = 0;


    enum ControlElement {
	CE_PushButton,
	CE_PushButtonLabel,

	CE_CheckBox,
	CE_CheckBoxLabel,

	CE_RadioButton,
	CE_RadioButtonLabel,

	CE_TabBarTab,
	CE_TabBarLabel,

	CE_ProgressBarGroove,
	CE_ProgressBarContents,
	CE_ProgressBarLabel,

	CE_PopupMenuItem,
	CE_MenuBarItem,

	CE_ToolButtonLabel,
	CE_MenuBarEmptyArea,
	CE_PopupMenuScroller,
	CE_DockWindowEmptyArea,
	CE_PopupMenuVerticalExtra,
	CE_PopupMenuHorizontalExtra,

	CE_ToolBoxTab,
	CE_HeaderLabel,

	// do not add any values below/greater than this
	CE_CustomBase =		0xf0000000
    };

    // Old API
    // DEPRECATED
    virtual void drawControl( ControlElement element,
			      TQPainter *p,
			      const TQWidget *widget,
			      const TQRect &r,
			      const TQColorGroup &cg,
			      SFlags how = Style_Default,
			      const TQStyleOption& = TQStyleOption::Default ) const;

    // New API
    virtual void drawControl( ControlElement element,
			      TQPainter *p,
			      const TQStyleControlElementData &ceData,
			      ControlElementFlags elementFlags,
			      const TQRect &r,
			      const TQColorGroup &cg,
			      SFlags how = Style_Default,
			      const TQStyleOption& = TQStyleOption::Default,
			      const TQWidget *widget = 0 /* compat, will likely go away */ ) const = 0;

    // Old API
    // DEPRECATED
    virtual void drawControlMask( ControlElement element,
				  TQPainter *p,
				  const TQWidget *widget,
				  const TQRect &r,
				  const TQStyleOption& = TQStyleOption::Default ) const;

    // New API
    virtual void drawControlMask( ControlElement element,
				  TQPainter *p,
				  const TQStyleControlElementData &ceData,
				  ControlElementFlags elementFlags,
				  const TQRect &r,
				  const TQStyleOption& = TQStyleOption::Default,
				  const TQWidget *widget = 0 /* compat, will likely go away */ ) const = 0;

    enum SubRect {
	SR_PushButtonContents,
	SR_PushButtonFocusRect,

	SR_CheckBoxIndicator,
	SR_CheckBoxContents,
	SR_CheckBoxFocusRect,

	SR_RadioButtonIndicator,
	SR_RadioButtonContents,
	SR_RadioButtonFocusRect,

	SR_ComboBoxFocusRect,

	SR_SliderFocusRect,

	SR_DockWindowHandleRect,

	SR_ProgressBarGroove,
	SR_ProgressBarContents,
	SR_ProgressBarLabel,

	SR_ToolButtonContents,

	SR_DialogButtonAccept,
	SR_DialogButtonReject,
	SR_DialogButtonApply,
	SR_DialogButtonHelp,
	SR_DialogButtonAll,
	SR_DialogButtonAbort,
	SR_DialogButtonIgnore,
	SR_DialogButtonRetry,
	SR_DialogButtonCustom,

	SR_ToolBoxTabContents,

	// do not add any values below/greater than this
	SR_CustomBase =		0xf0000000
    };

    // Old API
    // DEPRECATED
    virtual TQRect subRect( SubRect r, const TQWidget *widget ) const;

    // New API
    virtual TQRect subRect( SubRect r, const TQStyleControlElementData &ceData, const ControlElementFlags elementFlags, const TQWidget *widget ) const = 0;


    enum ComplexControl{
	CC_SpinWidget,
	CC_ComboBox,
	CC_ScrollBar,
	CC_Slider,
	CC_ToolButton,
	CC_TitleBar,
	CC_ListView,

	// do not add any values below/greater than this
	CC_CustomBase =		0xf0000000
    };

    enum SubControl {
	SC_None =			0x00000000,

	SC_ScrollBarAddLine =		0x00000001,
	SC_ScrollBarSubLine =		0x00000002,
	SC_ScrollBarAddPage =		0x00000004,
	SC_ScrollBarSubPage =		0x00000008,
	SC_ScrollBarFirst =		0x00000010,
	SC_ScrollBarLast =		0x00000020,
	SC_ScrollBarSlider =		0x00000040,
	SC_ScrollBarGroove =		0x00000080,

	SC_SpinWidgetUp =		0x00000001,
	SC_SpinWidgetDown =		0x00000002,
	SC_SpinWidgetFrame =		0x00000004,
	SC_SpinWidgetEditField =	0x00000008,
	SC_SpinWidgetButtonField =	0x00000010,

	SC_ComboBoxFrame =		0x00000001,
	SC_ComboBoxEditField =		0x00000002,
	SC_ComboBoxArrow =		0x00000004,
	SC_ComboBoxListBoxPopup =	0x00000008,

	SC_SliderGroove =		0x00000001,
	SC_SliderHandle = 		0x00000002,
	SC_SliderTickmarks = 		0x00000004,

	SC_ToolButton =			0x00000001,
	SC_ToolButtonMenu =		0x00000002,

	SC_TitleBarLabel =		0x00000001,
	SC_TitleBarSysMenu =		0x00000002,
	SC_TitleBarMinButton =		0x00000004,
	SC_TitleBarMaxButton =		0x00000008,
	SC_TitleBarCloseButton =	0x00000010,
	SC_TitleBarNormalButton =	0x00000020,
	SC_TitleBarShadeButton =	0x00000040,
	SC_TitleBarUnshadeButton =	0x00000080,

	SC_ListView =			0x00000001,
	SC_ListViewBranch =		0x00000002,
	SC_ListViewExpand =		0x00000004,

	SC_All =			0xffffffff
    };
    typedef uint SCFlags;


    // Old API
    // DEPRECATED
    virtual void drawComplexControl( ComplexControl control,
				     TQPainter *p,
				     const TQWidget *widget,
				     const TQRect &r,
				     const TQColorGroup &cg,
				     SFlags how = Style_Default,
#ifdef Q_QDOC
				     SCFlags sub = SC_All,
#else
				     SCFlags sub = (uint)SC_All,
#endif
				     SCFlags subActive = SC_None,
				     const TQStyleOption& = TQStyleOption::Default ) const;

    virtual void drawComplexControl( ComplexControl control,
				     TQPainter *p,
				     const TQStyleControlElementData &ceData,
				     ControlElementFlags elementFlags,
				     const TQRect &r,
				     const TQColorGroup &cg,
				     SFlags how = Style_Default,
#ifdef Q_QDOC
				     SCFlags sub = SC_All,
#else
				     SCFlags sub = (uint)SC_All,
#endif
				     SCFlags subActive = SC_None,
				     const TQStyleOption& = TQStyleOption::Default,
				     const TQWidget *widget = 0 ) const = 0;

    // Old API
    // DEPRECATED
    virtual void drawComplexControlMask( ComplexControl control,
					 TQPainter *p,
					 const TQWidget *widget,
					 const TQRect &r,
					 const TQStyleOption& = TQStyleOption::Default ) const;

    // New API
    virtual void drawComplexControlMask( ComplexControl control,
					 TQPainter *p,
					 const TQStyleControlElementData &ceData,
					 const ControlElementFlags elementFlags,
					 const TQRect &r,
					 const TQStyleOption& = TQStyleOption::Default,
					 const TQWidget *widget = 0 ) const = 0;

    // Old API
    // DEPRECATED
    virtual TQRect querySubControlMetrics( ComplexControl control,
					  const TQWidget *widget,
					  SubControl sc,
					  const TQStyleOption& = TQStyleOption::Default ) const;

    // New API
    virtual TQRect querySubControlMetrics( ComplexControl control,
					  const TQStyleControlElementData &ceData,
					  ControlElementFlags elementFlags,
					  SubControl sc,
					  const TQStyleOption& = TQStyleOption::Default,
					  const TQWidget *widget = 0 ) const = 0;

    // Old API
    // DEPRECATED
    virtual SubControl querySubControl( ComplexControl control,
					const TQWidget *widget,
					const TQPoint &pos,
					const TQStyleOption& = TQStyleOption::Default ) const;

    // New API
    virtual SubControl querySubControl( ComplexControl control,
					const TQStyleControlElementData &ceData,
					ControlElementFlags elementFlags,
					const TQPoint &pos,
					const TQStyleOption& = TQStyleOption::Default,
					const TQWidget *widget = 0 ) const = 0;


    enum PixelMetric {
	PM_ButtonMargin,
	PM_ButtonDefaultIndicator,
	PM_MenuButtonIndicator,
	PM_ButtonShiftHorizontal,
	PM_ButtonShiftVertical,

	PM_DefaultFrameWidth,
	PM_SpinBoxFrameWidth,

	PM_MaximumDragDistance,

	PM_ScrollBarExtent,
	PM_ScrollBarSliderMin,

	PM_SliderThickness,	       	// total slider thickness
	PM_SliderControlThickness,    	// thickness of the business part
	PM_SliderLength,		// total length of slider
	PM_SliderTickmarkOffset,	//
	PM_SliderSpaceAvailable,	// available space for slider to move

	PM_DockWindowSeparatorExtent,
	PM_DockWindowHandleExtent,
	PM_DockWindowFrameWidth,

	PM_MenuBarFrameWidth,

	PM_TabBarTabOverlap,
	PM_TabBarTabHSpace,
	PM_TabBarTabVSpace,
	PM_TabBarBaseHeight,
	PM_TabBarBaseOverlap,

	PM_ProgressBarChunkWidth,

	PM_SplitterWidth,
	PM_TitleBarHeight,

	PM_IndicatorWidth,
	PM_IndicatorHeight,
	PM_ExclusiveIndicatorWidth,
	PM_ExclusiveIndicatorHeight,
	PM_PopupMenuScrollerHeight,
	PM_CheckListButtonSize,
	PM_CheckListControllerSize,
	PM_PopupMenuFrameHorizontalExtra,
	PM_PopupMenuFrameVerticalExtra,

	PM_DialogButtonsSeparator,
	PM_DialogButtonsButtonWidth,
	PM_DialogButtonsButtonHeight,

	PM_MDIFrameWidth,
	PM_MDIMinimizedWidth,
	PM_HeaderMargin,
	PM_HeaderMarkSize,
	PM_HeaderGripMargin,
	PM_TabBarTabShiftHorizontal,
	PM_TabBarTabShiftVertical,
	PM_TabBarScrollButtonWidth,

	PM_MenuBarItemSpacing,
	PM_ToolBarItemSpacing,

	PM_MenuIndicatorFrameHBorder,
	PM_MenuIndicatorFrameVBorder,
	PM_MenuIconIndicatorFrameHBorder,
	PM_MenuIconIndicatorFrameVBorder,

	PM_ArrowSize,

	// do not add any values below/greater than this
	PM_CustomBase =		0xf0000000
    };

    // Old API
    // DEPRECATED
    virtual int pixelMetric( PixelMetric metric,
			     const TQWidget *widget = 0 ) const;

    // New API
    virtual int pixelMetric( PixelMetric metric,
			     const TQStyleControlElementData &ceData,
			     ControlElementFlags elementFlags,
			     const TQWidget *widget = 0 ) const = 0;


    enum ContentsType {
	CT_PushButton,
	CT_CheckBox,
	CT_RadioButton,
	CT_ToolButton,
	CT_ComboBox,
	CT_Splitter,
	CT_DockWindow,
	CT_ProgressBar,
	CT_PopupMenuItem,
	CT_TabBarTab,
	CT_Slider,
	CT_Header,
	CT_LineEdit,
	CT_MenuBar,
	CT_SpinBox,
	CT_SizeGrip,
	CT_TabWidget,
	CT_DialogButtons,

	// do not add any values below/greater than this
	CT_CustomBase =		0xf0000000
    };

    // Old API
    // DEPRECATED
    virtual TQSize sizeFromContents( ContentsType contents,
				    const TQWidget *widget,
				    const TQSize &contentsSize,
				    const TQStyleOption& = TQStyleOption::Default ) const;

    virtual TQSize sizeFromContents( ContentsType contents,
				    const TQStyleControlElementData &ceData,
				    ControlElementFlags elementFlags,
				    const TQSize &contentsSize,
				    const TQStyleOption& = TQStyleOption::Default,
				    const TQWidget *widget = 0 ) const = 0;

    enum StyleHint  {
	// ...
	// the general hints
	// ...
       	// disabled text should be etched, ala Windows
	SH_EtchDisabledText,

	// the GUI style enum, argh!
	SH_GUIStyle,

	// ...
	// widget specific hints
	// ...
	SH_ScrollBar_BackgroundMode,
	SH_ScrollBar_MiddleClickAbsolutePosition,
	SH_ScrollBar_ScrollWhenPointerLeavesControl,

	// TQEvent::Type - which mouse event to select a tab
	SH_TabBar_SelectMouseType,

	SH_TabBar_Alignment,

	SH_Header_ArrowAlignment,

	// bool - sliders snap to values while moving, ala Windows
	SH_Slider_SnapToValue,

	// bool - key presses handled in a sloppy manner - ie. left on a vertical
	// slider subtracts a line
	SH_Slider_SloppyKeyEvents,

	// bool - center button on progress dialogs, ala Motif, else right aligned
	// perhaps this should be a TQt::Alignment value
	SH_ProgressDialog_CenterCancelButton,

	// TQt::AlignmentFlags - text label alignment in progress dialogs
	// Center on windows, Auto|VCenter otherwize
	SH_ProgressDialog_TextLabelAlignment,

	// bool - right align buttons on print dialog, ala Windows
	SH_PrintDialog_RightAlignButtons,

	// bool - 1 or 2 pixel space between the menubar and the dockarea, ala Windows
	// this *REALLY* needs a better name
	SH_MainWindow_SpaceBelowMenuBar,

	// bool - select the text in the line edit about the listbox when selecting
	// an item from the listbox, or when the line edit receives focus, ala Windows
	SH_FontDialog_SelectAssociatedText,

	// bool - allows disabled menu items to be active
	SH_PopupMenu_AllowActiveAndDisabled,

	// bool - pressing space activates item, ala Motif
	SH_PopupMenu_SpaceActivatesItem,

	// int - number of milliseconds to wait before opening a submenu
	// 256 on windows, 96 on motif
	SH_PopupMenu_SubMenuPopupDelay,

	// bool - should scrollviews draw their frame only around contents (ala Motif),
	// or around contents, scrollbars and corner widgets (ala Windows) ?
	SH_ScrollView_FrameOnlyAroundContents,

	// bool - menubars items are navigatable by pressing alt, followed by using
	// the arrow keys to select the desired item
	SH_MenuBar_AltKeyNavigation,

	// bool - mouse tracking in combobox dropdown lists
	SH_ComboBox_ListMouseTracking,

	// bool - mouse tracking in popupmenus
	SH_PopupMenu_MouseTracking,

	// bool - mouse tracking in menubars
	SH_MenuBar_MouseTracking,

	// bool - gray out selected items when loosing focus
	SH_ItemView_ChangeHighlightOnFocus,

	// bool - supports shared activation among modeless widgets
	SH_Widget_ShareActivation,

	// bool - workspace should just maximize the client area
	SH_Workspace_FillSpaceOnMaximize,

	// bool - supports popup menu comboboxes
	SH_ComboBox_Popup,

	// bool - titlebar has no border
	SH_TitleBar_NoBorder,

	// bool - stop scrollbar at mouse
	SH_ScrollBar_StopMouseOverSlider,

	//bool - blink cursort with selected text
	SH_BlinkCursorWhenTextSelected,

	//bool - richtext selections extend the full width of the docuemnt
	SH_RichText_FullWidthSelection,

	//bool - popupmenu supports scrolling instead of multicolumn mode
	SH_PopupMenu_Scrollable,

	// TQt::AlignmentFlags - text label vertical alignment in groupboxes
	// Center on windows, Auto|VCenter otherwize
	SH_GroupBox_TextLabelVerticalAlignment,

	// TQt::TQRgb - text label color in groupboxes
	SH_GroupBox_TextLabelColor,

	// bool - popupmenu supports sloppy submenus
	SH_PopupMenu_SloppySubMenus,

	// TQt::TQRgb - table grid color
	SH_Table_GridLineColor,

	// TQChar - Unicode character for password char
	SH_LineEdit_PasswordCharacter,

	// TQDialogButtons::Button - default button
	SH_DialogButtons_DefaultButton,

	// TQToolBox - Boldness of the selected page title
	SH_ToolBox_SelectedPageTitleBold,

	//bool - if a tabbar prefers not to have scroller arrows
	SH_TabBar_PreferNoArrows,

	//bool - if left button should cause an absolute position
	SH_ScrollBar_LeftClickAbsolutePosition,

	// TQEvent::Type - which mouse event to select a list view expansion
	SH_ListViewExpand_SelectMouseType,

	//bool - if underline for accelerators
	SH_UnderlineAccelerator,

	// bool - TQToolButton - if tool buttons should use a 3D frame
	// when the mouse is over the button
	SH_ToolButton_Uses3D,

	// bool - hide underlined accelerators uless Alt key is currently down
	SH_HideUnderlineAcceleratorWhenAltUp,

	// int - width of menu check column
	SH_MenuIndicatorColumnWidth,

	// bool - whether or not the lower two button drawing areas should be combined into one
	SH_ScrollBar_CombineAddLineRegionDrawingAreas,

	// bool - whether or not the upper two button drawing areas should be combined into one
	SH_ScrollBar_CombineSubLineRegionDrawingAreas,

	// TQt::TQRgb - color of the popup menu arrow (active, menuitem enabled)
	SH_PopupMenu_SubMenuArrowColorActiveEnabled,

	// TQt::TQRgb - color of the popup menu arrow (active, menuitem disabled)
	SH_PopupMenu_SubMenuArrowColorActiveDisabled,

	// TQt::TQRgb - color of the popup menu arrow (inactive, menuitem enabled)
	SH_PopupMenu_SubMenuArrowColorInactiveEnabled,

	// TQt::TQRgb - color of the popup menu arrow (active, menuitem disabled)
	SH_PopupMenu_SubMenuArrowColorInactiveDisabled,

	// do not add any values below/greater than this
	SH_CustomBase =		0xf0000000
    };

    // Old API
    // DEPRECATED
    virtual int styleHint( StyleHint stylehint,
			   const TQWidget *widget = 0,
			   const TQStyleOption& = TQStyleOption::Default,
			   TQStyleHintReturn* returnData = 0
			   ) const;

    // New API
    virtual int styleHint( StyleHint stylehint,
			   const TQStyleControlElementData &ceData,
			   ControlElementFlags elementFlags,
			   const TQStyleOption& = TQStyleOption::Default,
			   TQStyleHintReturn* returnData = 0,
			   const TQWidget *widget = 0
			   ) const = 0;


    enum StylePixmap {
	SP_TitleBarMinButton,
	SP_TitleBarMaxButton,
	SP_TitleBarCloseButton,
	SP_TitleBarNormalButton,
	SP_TitleBarShadeButton,
	SP_TitleBarUnshadeButton,
	SP_DockWindowCloseButton,
	SP_MessageBoxInformation,
	SP_MessageBoxWarning,
	SP_MessageBoxCritical,
	SP_MessageBoxQuestion,

	// do not add any values below/greater than this
	SP_CustomBase =		0xf0000000
    };

    // Old API
    // DEPRECATED
    virtual TQPixmap stylePixmap( StylePixmap stylepixmap,
				 const TQWidget *widget = 0,
				 const TQStyleOption& = TQStyleOption::Default ) const;

    virtual TQPixmap stylePixmap( StylePixmap stylepixmap,
				 const TQStyleControlElementData &ceData,
				 ControlElementFlags elementFlags,
				 const TQStyleOption& = TQStyleOption::Default,
				 const TQWidget *widget = 0 ) const = 0;


    // Old API
    // DEPRECATED
    static TQRect visualRect( const TQRect &logical, const TQWidget *w );

    // New API
    static TQRect visualRect( const TQRect &logical, const TQStyleControlElementData &ceData, const ControlElementFlags elementFlags );

    static TQRect visualRect( const TQRect &logical, const TQRect &bounding );


    // Object event handling API
    typedef TQMap<void*, TQStyle*> ObjectEventSourceToHandlerMap;
    typedef TQMap<void*, TQStyleControlElementData> ObjectEventSourceDataToHandlerMap;
    typedef TQMap<void*, ControlElementFlags> ObjectEventSourceFlagsToHandlerMap;
    typedef bool (*EventHandlerInstallationHook)(const TQStyleControlElementData &ceData, ControlElementFlags elementFlags, void* source, TQStyle* handler);
    typedef bool (*EventHandlerRemovalHook)(const TQStyleControlElementData &ceData, ControlElementFlags elementFlags, void* source, TQStyle* handler);
    void installObjectEventHandler( const TQStyleControlElementData &ceData, ControlElementFlags elementFlags, void* source, TQStyle* handler );
    void removeObjectEventHandler( const TQStyleControlElementData &ceData, ControlElementFlags elementFlags, void* source, TQStyle* handler );
    void setEventHandlerInstallationHook( EventHandlerInstallationHook );
    void setEventHandlerRemovalHook( EventHandlerRemovalHook hook );
    virtual bool objectEventHandler( const TQStyleControlElementData &ceData, ControlElementFlags elementFlags, void* source, TQEvent *e );

    enum WidgetActionRequest {
	WAR_Repaint,
	WAR_RepaintRect,
	WAR_EnableMouseTracking,
	WAR_DisableMouseTracking,
	WAR_SetAutoMask,
	WAR_UnSetAutoMask,
	WAR_SetCheckable,
	WAR_UnSetCheckable,
	WAR_FrameSetStyle,
	WAR_FrameSetLineWidth,
	WAR_SetLayoutMargin,
	WAR_SetPalette,
	WAR_SetBackgroundMode,
	WAR_SetBackgroundOrigin,
	WAR_SetFont,
	WAR_RepaintAllAccelerators,
	WAR_SetDefault,
	WAR_UnSetDefault,
	WAR_SendPaintEvent
    };

    typedef bool (*WidgetActionRequestHook)(const TQStyleControlElementData &ceData, ControlElementFlags elementFlags, void* source, WidgetActionRequest request, TQStyleWidgetActionRequestData requestData);
    void setWidgetActionRequestHook( WidgetActionRequestHook );
    virtual bool widgetActionRequest( const TQStyleControlElementData &ceData, ControlElementFlags elementFlags, void* source, WidgetActionRequest request, TQStyleWidgetActionRequestData requestData = TQStyleWidgetActionRequestData() );

    enum ApplicationActionRequest {
	AAR_SetPalette,
	AAR_SetFont
    };

    typedef bool (*ApplicationActionRequestHook)(const TQStyleControlElementData &ceData, ControlElementFlags elementFlags, void* source, ApplicationActionRequest request, TQStyleApplicationActionRequestData requestData);
    void setApplicationActionRequestHook( ApplicationActionRequestHook );
    virtual bool applicationActionRequest( const TQStyleControlElementData &ceData, ControlElementFlags elementFlags, void* source, ApplicationActionRequest request, TQStyleApplicationActionRequestData requestData = TQStyleApplicationActionRequestData() );

    // Old 2.x TQStyle API

#ifndef TQT_NO_COMPAT
    int defaultFrameWidth() const;
    void tabbarMetrics( const TQWidget* t, int& hf, int& vf, int& ov ) const;
    TQSize scrollBarExtent() const;
#endif

public:
    virtual bool eventFilter( TQObject *, TQEvent * );
    bool acceleratorsShown() const;

protected:
    void acceleratorKeypressEventMonitor( TQObject *, TQEvent * );

private:
    TQStylePrivate * d;

#if defined(TQ_DISABLE_COPY)
    TQStyle( const TQStyle & );
    TQStyle& operator=( const TQStyle & );
#endif

    EventHandlerInstallationHook m_eventHandlerInstallationHook;
    EventHandlerRemovalHook m_eventHandlerRemovalHook;
    WidgetActionRequestHook m_widgetActionRequestHook;
    ApplicationActionRequestHook m_applicationActionRequestHook;
    ObjectEventSourceToHandlerMap m_objectEventSourceToHandlerMap;
    ObjectEventSourceDataToHandlerMap m_objectEventSourceDataToHandlerMap;
    ObjectEventSourceFlagsToHandlerMap m_objectEventSourceFlagsToHandlerMap;
    bool conditionalAcceleratorsEnabled;
};

inline TQStyle::ControlElementFlags operator|(const TQStyle::ControlElementFlags a, const TQStyle::ControlElementFlags b) { return static_cast<TQStyle::ControlElementFlags>(static_cast<int>(a) | static_cast<int>(b)); }
// inline TQStyle::ControlElementFlags operator|=(TQStyle::ControlElementFlags &a, const TQStyle::ControlElementFlags b) { a = static_cast<TQStyle::ControlElementFlags>(static_cast<int>(a) | static_cast<int>(b)); return a; }

TQ_EXPORT const TQStyleControlElementData &populateControlElementDataFromWidget(const TQWidget* widget, const TQStyleOption& opt, bool populateReliantFields=true);
TQ_EXPORT const TQStyleControlElementData &populateControlElementDataFromApplication(const TQApplication* app, const TQStyleOption& opt, bool populateReliantFields=true);
TQ_EXPORT TQStyle::ControlElementFlags getControlElementFlagsForObject(const TQObject* object, const TQStyleOption& opt, bool populateReliantFields=true);
TQ_EXPORT TQStringList getObjectTypeListForObject(const TQObject* object);
TQ_EXPORT const TQStyleControlElementData &populateControlElementDataFromWidget(const TQWidget* widget, const TQStyleOption& opt, bool populateReliantFields, bool populateMinimumNumberOfFields);

#endif // TQT_NO_STYLE
#endif // TQSTYLE_H
