/****************************************************************************
**
** Implementation of the TQCommonStyle class
**
** Created : 981231
**
** Copyright (C) 1998-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of the widgets module of the TQt GUI Toolkit.
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

#include "ntqcommonstyle.h"

#ifndef TQT_NO_STYLE

#include "ntqmutex.h"
#include "ntqmenubar.h"
#include "ntqapplication.h"
#include "ntqpainter.h"
#include "ntqdrawutil.h"
#include "ntqpixmap.h"
#include "ntqpushbutton.h"
#include "ntqtabbar.h"
#include "ntqtabwidget.h"
#include "ntqlineedit.h"
#include "ntqscrollbar.h"
#include "ntqtoolbutton.h"
#include "ntqtoolbar.h"
#include "ntqcombobox.h"
#include "ntqdockarea.h"
#include "ntqheader.h"
#include "ntqspinbox.h"
#include "ntqrangecontrol.h"
#include "ntqgroupbox.h"
#include "ntqslider.h"
#include "ntqlistview.h"
#include "ntqcheckbox.h"
#include "ntqradiobutton.h"
#include "ntqbitmap.h"
#include "ntqprogressbar.h"
#include "ntqlistbox.h"
#include "private/qdialogbuttons_p.h"
#include <limits.h>
#include <ntqpixmap.h>
#include "../widgets/qtitlebar_p.h"
#include <ntqtoolbox.h>

/*!
    \class TQCommonStyle ntqcommonstyle.h
    \brief The TQCommonStyle class encapsulates the common Look and Feel of a GUI.

    \ingroup appearance

    This abstract class implements some of the widget's look and feel
    that is common to all GUI styles provided and shipped as part of
    TQt.

    All the functions are documented in \l TQStyle.
*/

/*!
    \enum TQt::ArrowType

    \value UpArrow
    \value DownArrow
    \value LeftArrow
    \value RightArrow

*/

// the active painter, if any... this is used as an optimzation to
// avoid creating a painter if we have an active one (since
// TQStyle::itemRect() needs a painter to operate correctly
static TQPainter *activePainter = 0;

/*!
    Constructs a TQCommonStyle.
*/
TQCommonStyle::TQCommonStyle() : TQStyle()
{
    activePainter = 0;
}

/*! \reimp */
TQCommonStyle::~TQCommonStyle()
{
    activePainter = 0;
}


static const char * const check_list_controller_xpm[] = {
"16 16 4 1",
"	c None",
".	c #000000000000",
"X	c #FFFFFFFF0000",
"o	c #C71BC30BC71B",
"                ",
"                ",
" ..........     ",
" .XXXXXXXX.     ",
" .XXXXXXXX.oo   ",
" .XXXXXXXX.oo   ",
" .XXXXXXXX.oo   ",
" .XXXXXXXX.oo   ",
" .XXXXXXXX.oo   ",
" .XXXXXXXX.oo   ",
" .XXXXXXXX.oo   ",
" ..........oo   ",
"   oooooooooo   ",
"   oooooooooo   ",
"                ",
"                "};

// Increase speed by avoiding constant allocation/deallocation of commonly used strings
static TQString TQPushButton_static_string("TQPushButton");
static TQString TQToolButton_static_string("TQToolButton");
static TQString TQButton_static_string("TQButton");
static TQString TQTabBar_static_string("TQTabBar");
static TQString TQTitleBar_static_string("TQTitleBar");
static TQString TQToolBox_static_string("TQToolBox");
static TQString TQToolBar_static_string("TQToolBar");
static TQString TQProgressBar_static_string("TQProgressBar");
static TQString TQPopupMenu_static_string("TQPopupMenu");
static TQString TQComboBox_static_string("TQComboBox");
static TQString TQCheckBox_static_string("TQCheckBox");
static TQString TQRadioButton_static_string("TQRadioButton");
static TQString TQHeader_static_string("TQHeader");
static TQString TQScrollBar_static_string("TQScrollBar");
static TQString TQSlider_static_string("TQSlider");
static TQString TQDialogButtons_static_string("TQDialogButtons");
static TQString TQSpinWidget_static_string("TQSpinWidget");
static TQString TQListView_static_string("TQListView");
static TQString TQDockWindow_static_string("TQDockWindow");
static TQString TQTabWidget_static_string("TQTabWidget");
static TQString TQFrame_static_string("TQFrame");
static TQString TQWidget_static_string("TQWidget");

static TQStyleControlElementData* TQStyleControlElementData_null = NULL;
static void tqt_style_control_element_data_null_cleanup() {
    delete TQStyleControlElementData_null;
    TQStyleControlElementData_null =0;
}

#include <ntqmetaobject.h>

// Keep in sync with duplicate definition in qobject.cpp
class TQStyleControlElementDataPrivate {
	public:
		bool isTQPushButton;
		bool isTQToolButton;
		bool isTQButton;
		bool isTQTabBar;
		bool isTQTitleBar;
		bool isTQToolBox;
		bool isTQToolBar;
		bool isTQProgressBar;
		bool isTQPopupMenu;
		bool isTQComboBox;
		bool isTQCheckBox;
		bool isTQRadioButton;
		bool isTQHeader;
		bool isTQScrollBar;
		bool isTQSlider;
		bool isTQDialogButtons;
		bool isTQSpinWidget;
		bool isTQListView;
		bool isTQDockWindow;
		bool isTQTabWidget;
		bool isTQFrame;
		bool isTQWidget;
};

TQStringList getObjectTypeListForObject(const TQObject* object) {
	TQStringList objectTypes;

	if (object) {
		TQMetaObject* objectMetaObject = object->metaObject();
		const char* name;

		while (objectMetaObject) {
			name = objectMetaObject->className();
			objectTypes.append(TQString(name));
			objectMetaObject = objectMetaObject->superClass();
		}
	}

	return objectTypes;
}

void determineObjectTypes(const TQObject* object, TQStyleControlElementData &ceData, TQStyleControlElementDataPrivate* ceDataPrivate) {
	ceData.widgetObjectTypes = getObjectTypeListForObject(object);

	// Increase speed on repeated calls by checking object types here and caching them
	ceDataPrivate->isTQPushButton = ceData.widgetObjectTypes.containsYesNo(TQPushButton_static_string);
	ceDataPrivate->isTQToolButton = ceData.widgetObjectTypes.containsYesNo(TQToolButton_static_string);
	ceDataPrivate->isTQButton = ceData.widgetObjectTypes.containsYesNo(TQButton_static_string);
	ceDataPrivate->isTQTabBar = ceData.widgetObjectTypes.containsYesNo(TQTabBar_static_string);
	ceDataPrivate->isTQTitleBar = ceData.widgetObjectTypes.containsYesNo(TQTitleBar_static_string);
	ceDataPrivate->isTQToolBox = ceData.widgetObjectTypes.containsYesNo(TQToolBox_static_string);
	ceDataPrivate->isTQToolBar = ceData.widgetObjectTypes.containsYesNo(TQToolBar_static_string);
	ceDataPrivate->isTQProgressBar = ceData.widgetObjectTypes.containsYesNo(TQProgressBar_static_string);
	ceDataPrivate->isTQPopupMenu = ceData.widgetObjectTypes.containsYesNo(TQPopupMenu_static_string);
	ceDataPrivate->isTQComboBox = ceData.widgetObjectTypes.containsYesNo(TQComboBox_static_string);
	ceDataPrivate->isTQCheckBox = ceData.widgetObjectTypes.containsYesNo(TQCheckBox_static_string);
	ceDataPrivate->isTQRadioButton = ceData.widgetObjectTypes.containsYesNo(TQRadioButton_static_string);
	ceDataPrivate->isTQHeader = ceData.widgetObjectTypes.containsYesNo(TQHeader_static_string);
	ceDataPrivate->isTQScrollBar = ceData.widgetObjectTypes.containsYesNo(TQScrollBar_static_string);
	ceDataPrivate->isTQSlider = ceData.widgetObjectTypes.containsYesNo(TQSlider_static_string);
	ceDataPrivate->isTQDialogButtons = ceData.widgetObjectTypes.containsYesNo(TQDialogButtons_static_string);
	ceDataPrivate->isTQSpinWidget = ceData.widgetObjectTypes.containsYesNo(TQSpinWidget_static_string);
	ceDataPrivate->isTQListView = ceData.widgetObjectTypes.containsYesNo(TQListView_static_string);
	ceDataPrivate->isTQDockWindow = ceData.widgetObjectTypes.containsYesNo(TQDockWindow_static_string);
	ceDataPrivate->isTQTabWidget = ceData.widgetObjectTypes.containsYesNo(TQTabWidget_static_string);
	ceDataPrivate->isTQFrame = ceData.widgetObjectTypes.containsYesNo(TQFrame_static_string);
	ceDataPrivate->isTQWidget = ceData.widgetObjectTypes.containsYesNo(TQWidget_static_string);
}

void updateObjectTypeListIfNeeded(const TQObject* object) {
	TQStyleControlElementData &ceData = *(const_cast<TQObject*>(object)->controlElementDataObject());
	TQStyleControlElementDataPrivate* ceDataPrivate = const_cast<TQObject*>(object)->controlElementDataPrivateObject();

	// This is much faster than (ceData.widgetObjectTypes[0] != TQString(object->metaObject()->className())) due to TQString constructor/destructor overhead
	// Casting to const also increases speed by preventing unneeded calls to detach()
	if ((ceData.widgetObjectTypes.isEmpty()) || (strcmp(const_cast<const TQStringList&>(ceData.widgetObjectTypes)[0].ascii(), object->metaObject()->className()) != 0)) {
		determineObjectTypes(object, ceData, ceDataPrivate);
	}
}

TQStyle::ControlElementFlags getControlElementFlagsForObject(const TQObject* object, const TQStyleOption& opt, bool populateReliantFields) {
	TQStyle::ControlElementFlags cef = TQStyle::CEF_None;

	if (object) {
		TQStyleControlElementDataPrivate* ceDataPrivate = const_cast<TQObject*>(object)->controlElementDataPrivateObject();
		updateObjectTypeListIfNeeded(object);

		if (ceDataPrivate->isTQPushButton) {
			const TQPushButton *button = static_cast<const TQPushButton*>(object);
			if (button) {
				if (button->isDefault())			cef = cef | TQStyle::CEF_IsDefault;
				if (button->autoDefault())			cef = cef | TQStyle::CEF_AutoDefault;
				if (button->isMenuButton())			cef = cef | TQStyle::CEF_IsMenuWidget;
				if (button->isToggleButton())			cef = cef | TQStyle::CEF_BiState;
				if (button->isFlat())				cef = cef | TQStyle::CEF_IsFlat;
			}
		}
		if (ceDataPrivate->isTQToolButton) {
			const TQToolButton *button = static_cast<const TQToolButton*>(object);
			if (button) {
				if (button->isToggleButton())			cef = cef | TQStyle::CEF_BiState;
				if (button->usesTextLabel())			cef = cef | TQStyle::CEF_UsesTextLabel;
				if (button->usesBigPixmap())			cef = cef | TQStyle::CEF_UsesBigPixmap;
				if (button->popup())				cef = cef | TQStyle::CEF_HasPopupMenu;
			}
		}
		if (ceDataPrivate->isTQButton) {
			const TQButton *button = static_cast<const TQButton*>(object);
			if (button) {
				if (button->isDown())				cef = cef | TQStyle::CEF_IsDown;
				if (button->isOn())				cef = cef | TQStyle::CEF_IsOn;
				if (button->isToggleButton())			cef = cef | TQStyle::CEF_BiState;
			}
		}
		if (ceDataPrivate->isTQTabBar) {
			const TQTabBar *tb = static_cast<const TQTabBar*>(object);
			TQTab * t = opt.tab();
			if ((t) && (tb)) {
				if (t->identifier() == tb->currentTab())	cef = cef | TQStyle::CEF_IsActive;
			}
		}
		if (ceDataPrivate->isTQTitleBar) {
			const TQTitleBar *tb = static_cast<const TQTitleBar*>(object);
			if (tb) {
				if (tb->isActive())				cef = cef | TQStyle::CEF_IsActive;
			}
		}
		if (ceDataPrivate->isTQToolBox) {
			const TQToolBox *tb = static_cast<const TQToolBox*>(object);
			if (tb) {
				if (!tb->currentItem())				cef = cef | TQStyle::CEF_IsContainerEmpty;
			}
		}
		if (ceDataPrivate->isTQProgressBar) {
			const TQProgressBar *pb = static_cast<const TQProgressBar*>(object);
			if (pb) {
				if (pb->centerIndicator())			cef = cef | TQStyle::CEF_CenterIndicator;
				if (pb->indicatorFollowsStyle())		cef = cef | TQStyle::CEF_IndicatorFollowsStyle;
			}
		}
		if (ceDataPrivate->isTQPopupMenu) {
			const TQPopupMenu *pm = static_cast<const TQPopupMenu*>(object);
			if (pm) {
				if (pm->isCheckable())				cef = cef | TQStyle::CEF_IsCheckable;
			}
		}
		if (ceDataPrivate->isTQComboBox) {
			const TQComboBox *cb = static_cast<const TQComboBox*>(object);
			if (cb) {
				if (cb->editable())				cef = cef | TQStyle::CEF_IsEditable;
			}
		}
		// Generic flags
		if (ceDataPrivate->isTQWidget) {
			const TQWidget* widget = static_cast<const TQWidget*>(object);
			if (widget) {
				if (widget->isEnabled())				cef = cef | TQStyle::CEF_IsEnabled;
				if (widget->parentWidget())				cef = cef | TQStyle::CEF_HasParentWidget;
				if (widget->focusProxy())				cef = cef | TQStyle::CEF_HasFocusProxy;
				if (widget->hasFocus())					cef = cef | TQStyle::CEF_HasFocus;
				if (widget->hasMouse())					cef = cef | TQStyle::CEF_HasMouse;
				if (populateReliantFields) {
					if (widget->isActiveWindow())			cef = cef | TQStyle::CEF_IsActiveWindow;
					if (widget->isTopLevel())			cef = cef | TQStyle::CEF_IsTopLevel;
					if (widget->isVisible())			cef = cef | TQStyle::CEF_IsVisible;
					if (widget->isShown())				cef = cef | TQStyle::CEF_IsShown;
				}
			}
		}
	}
	else {
		cef = cef | TQStyle::CEF_UseGenericParameters;
	}

	return cef;
}

const TQStyleControlElementData &populateControlElementDataFromWidget(const TQWidget* widget, const TQStyleOption& opt, bool populateReliantFields) {
	return populateControlElementDataFromWidget(widget, opt, populateReliantFields, false);
}

const TQStyleControlElementData &populateControlElementDataFromWidget(const TQWidget* widget, const TQStyleOption& opt, bool populateReliantFields, bool populateMinimumNumberOfFields) {
	if (widget) {
		TQStyleControlElementData &ceData = *(const_cast<TQWidget*>(widget)->controlElementDataObject());
		TQStyleControlElementDataPrivate* ceDataPrivate = const_cast<TQWidget*>(widget)->controlElementDataPrivateObject();

		ceData.isNull = false;
		updateObjectTypeListIfNeeded(widget);

		ceData.allDataPopulated = populateReliantFields & populateMinimumNumberOfFields;
		if (!populateMinimumNumberOfFields) {
			const TQPixmap* erasePixmap = widget->backgroundPixmap();
			if (erasePixmap) {
				ceData.bgPixmap = *erasePixmap;
			}
			if (populateReliantFields) {
				ceData.bgBrush = widget->backgroundBrush();
				ceData.windowState = (TQt::WindowState)(widget->windowState());
			}
		}
		ceData.wflags = widget->getWFlags();
		if (!populateMinimumNumberOfFields) {
			ceData.bgColor = widget->eraseColor();
			ceData.bgOffset = widget->backgroundOffset();
			ceData.backgroundMode = widget->backgroundMode();
			if (populateReliantFields) {
				ceData.fgColor = widget->foregroundColor();
				ceData.colorGroup = widget->colorGroup();
				ceData.paletteBgColor = widget->paletteBackgroundColor();
			}
			ceData.geometry = widget->geometry();
			ceData.rect = widget->rect();
			ceData.pos = widget->pos();
			const TQPixmap* icon = widget->icon();
			if (icon) {
				ceData.icon = *icon;
			}
			ceData.palette = widget->palette();
			ceData.font = widget->font();
		}
		ceData.name = widget->name();
		ceData.caption = widget->caption();
		if (!populateMinimumNumberOfFields) {
			if (ceDataPrivate->isTQPushButton) {
				const TQPushButton *button = static_cast<const TQPushButton*>(widget);
				if (button) {
					TQIconSet* iconSet = 0;
					const TQPixmap* fgPixmap = 0;
					iconSet = button->iconSet();
					fgPixmap = button->pixmap();
					if (iconSet) {
						ceData.iconSet = *iconSet;
					}
					else {
						ceData.iconSet = TQIconSet();
					}
					if (fgPixmap) {
						ceData.fgPixmap = *fgPixmap;
					}
				}
			}
			if (ceDataPrivate->isTQToolButton) {
				const TQToolButton *button = static_cast<const TQToolButton*>(widget);
				if (button) {
					const TQPixmap* fgPixmap = 0;
					ceData.iconSet = button->iconSet();
					fgPixmap = button->pixmap();
					if (fgPixmap) {
						ceData.fgPixmap = *fgPixmap;
					}
					ceData.textLabel = button->textLabel();
					ceData.toolButtonTextPosition = button->textPosition();
					ceData.popupDelay = button->popupDelay();
				}
			}
			if (ceDataPrivate->isTQCheckBox) {
				const TQCheckBox *button = static_cast<const TQCheckBox*>(widget);
				if (button) {
					const TQPixmap* fgPixmap = 0;
					fgPixmap = button->pixmap();
					if (fgPixmap) {
						ceData.fgPixmap = *fgPixmap;
					}
				}
			}
			if (ceDataPrivate->isTQRadioButton) {
				const TQRadioButton *button = static_cast<const TQRadioButton*>(widget);
				if (button) {
					const TQPixmap* fgPixmap = 0;
					fgPixmap = button->pixmap();
					if (fgPixmap) {
						ceData.fgPixmap = *fgPixmap;
					}
				}
			}
			if (ceDataPrivate->isTQButton) {
				const TQButton *button = static_cast<const TQButton*>(widget);
				if (button) {
					ceData.text = button->text();
					const TQPixmap* paletteBgPixmap = 0;
					paletteBgPixmap = button->paletteBackgroundPixmap();
					if (paletteBgPixmap) {
						ceData.paletteBgPixmap = *paletteBgPixmap;
					}
				}
			}
			if (ceDataPrivate->isTQTabBar) {
				const TQTabBar *tb = static_cast<const TQTabBar*>(widget);
				if (tb) {
					ceData.tabBarData.tabCount = tb->count();
					ceData.tabBarData.currentTabIndex = tb->currentTab();
					ceData.tabBarData.shape = tb->shape();
					ceData.tabBarData.identIndexMap.clear();
					const TQTab* currentTab;
					for (int i=0; i<tb->count(); ++i) {
						currentTab = tb->tabAt(i);
						if (currentTab) {
							ceData.tabBarData.identIndexMap[currentTab->identifier()] = tb->indexOf(currentTab->identifier());
						}
					}
					if ( tb->parent() ) {
						updateObjectTypeListIfNeeded(tb->parent());
						if ((const_cast<TQObject*>(tb->parent())->controlElementDataPrivateObject())->isTQTabWidget) {
							const TQTabWidget *tw = static_cast<const TQTabWidget*>(tb->parent());
							TQWidget *cw;
							cw = tw->cornerWidget(TQt::TopLeft);
							if (cw) {
								updateObjectTypeListIfNeeded(cw);
								ceData.tabBarData.cornerWidgets[TQStyleControlElementTabBarData::CWL_TopLeft].widgetObjectTypes = (const_cast<TQWidget*>(cw)->controlElementDataObject())->widgetObjectTypes;
								ceData.tabBarData.cornerWidgets[TQStyleControlElementTabBarData::CWL_TopLeft].geometry = cw->geometry();
								ceData.tabBarData.cornerWidgets[TQStyleControlElementTabBarData::CWL_TopLeft].rect = cw->rect();
								ceData.tabBarData.cornerWidgets[TQStyleControlElementTabBarData::CWL_TopLeft].pos = cw->pos();
							}
							cw = tw->cornerWidget(TQt::TopRight);
							if (cw) {
								updateObjectTypeListIfNeeded(cw);
								ceData.tabBarData.cornerWidgets[TQStyleControlElementTabBarData::CWL_TopRight].widgetObjectTypes = (const_cast<TQWidget*>(cw)->controlElementDataObject())->widgetObjectTypes;
								ceData.tabBarData.cornerWidgets[TQStyleControlElementTabBarData::CWL_TopRight].geometry = cw->geometry();
								ceData.tabBarData.cornerWidgets[TQStyleControlElementTabBarData::CWL_TopRight].rect = cw->rect();
								ceData.tabBarData.cornerWidgets[TQStyleControlElementTabBarData::CWL_TopRight].pos = cw->pos();
							}
							cw = tw->cornerWidget(TQt::BottomLeft);
							if (cw) {
								updateObjectTypeListIfNeeded(cw);
								ceData.tabBarData.cornerWidgets[TQStyleControlElementTabBarData::CWL_BottomLeft].widgetObjectTypes = (const_cast<TQWidget*>(cw)->controlElementDataObject())->widgetObjectTypes;
								ceData.tabBarData.cornerWidgets[TQStyleControlElementTabBarData::CWL_BottomLeft].geometry = cw->geometry();
								ceData.tabBarData.cornerWidgets[TQStyleControlElementTabBarData::CWL_BottomLeft].rect = cw->rect();
								ceData.tabBarData.cornerWidgets[TQStyleControlElementTabBarData::CWL_BottomLeft].pos = cw->pos();
							}
							cw = tw->cornerWidget(TQt::BottomRight);
							if (cw) {
								updateObjectTypeListIfNeeded(cw);
								ceData.tabBarData.cornerWidgets[TQStyleControlElementTabBarData::CWL_BottomRight].widgetObjectTypes = (const_cast<TQWidget*>(cw)->controlElementDataObject())->widgetObjectTypes;
								ceData.tabBarData.cornerWidgets[TQStyleControlElementTabBarData::CWL_BottomRight].geometry = cw->geometry();
								ceData.tabBarData.cornerWidgets[TQStyleControlElementTabBarData::CWL_BottomRight].rect = cw->rect();
								ceData.tabBarData.cornerWidgets[TQStyleControlElementTabBarData::CWL_BottomRight].pos = cw->pos();
							}
						}
					}
				}
			}
			if (ceDataPrivate->isTQToolBox) {
				const TQToolBox *tb = static_cast<const TQToolBox*>(widget);
				if (tb) {
					const TQWidget* currentItem = tb->currentItem();
					if (currentItem) {
						ceData.activeItemPaletteBgColor = currentItem->paletteBackgroundColor();
					}
				}
			}
			if (ceDataPrivate->isTQProgressBar) {
				const TQProgressBar *pb = static_cast<const TQProgressBar*>(widget);
				if (pb) {
					ceData.currentStep = pb->progress();
					ceData.totalSteps = pb->totalSteps();
					ceData.progressText = pb->progressString();
					ceData.percentageVisible = pb->percentageVisible();
					ceData.orientation = pb->orientation();
				}
			}
			if (ceDataPrivate->isTQHeader) {
				const TQHeader *header = static_cast<const TQHeader*>(widget);
				if (header) {
					int section = opt.headerSection();
					TQIconSet* iconSet = 0;
					iconSet = header->iconSet(section);
					ceData.textLabel = header->label(section);
					if (iconSet) {
						ceData.iconSet = *iconSet;
					}
					else {
						ceData.iconSet = TQIconSet();
					}
				}
			}
			// Complex Controls
			if (ceDataPrivate->isTQScrollBar) {
				const TQScrollBar *sb = static_cast<const TQScrollBar*>(widget);
				if (sb) {
					ceData.orientation = sb->orientation();
					ceData.minSteps = sb->minValue();
					ceData.maxSteps = sb->maxValue();
					ceData.currentStep = sb->value();
					ceData.startStep = sb->sliderStart();
					ceData.lineStep = sb->lineStep();
					ceData.pageStep = sb->pageStep();
					if (populateReliantFields) {
						ceData.sliderRect = sb->sliderRect();
					}
				}
			}
			if (ceDataPrivate->isTQSlider) {
				const TQSlider *sl = static_cast<const TQSlider*>(widget);
				if (sl) {
					ceData.orientation = sl->orientation();
					ceData.minSteps = sl->minValue();
					ceData.maxSteps = sl->maxValue();
					ceData.tickMarkSetting = sl->tickmarks();
					ceData.tickInterval = sl->tickInterval();
					ceData.currentStep = sl->value();
					ceData.startStep = sl->sliderStart();
					ceData.lineStep = sl->lineStep();
					ceData.pageStep = sl->pageStep();
					if (populateReliantFields) {
						ceData.sliderRect = sl->sliderRect();
					}
				}
			}
			if (ceDataPrivate->isTQDialogButtons) {
				const TQDialogButtons *dlgbtns = static_cast<const TQDialogButtons*>(widget);
				if (dlgbtns) {
					ceData.orientation = dlgbtns->orientation();
					ceData.dlgVisibleButtons = dlgbtns->visibleButtons();
					TQDialogButtons::Button btns[] = { TQDialogButtons::All, TQDialogButtons::Reject, TQDialogButtons::Accept, TQDialogButtons::Apply, TQDialogButtons::Retry, TQDialogButtons::Ignore, TQDialogButtons::Abort, TQDialogButtons::Help };
					for(unsigned int i = 0; i < (sizeof(btns)/sizeof(btns[0])); i++) {
						if (ceData.dlgVisibleButtons & btns[i]) {
							ceData.dlgVisibleSizeHints[btns[i]] = dlgbtns->sizeHint(btns[i]);
						}
					}

				}
			}
			if (ceDataPrivate->isTQTitleBar) {
				const TQTitleBar *tb = static_cast<const TQTitleBar*>(widget);
				if (tb) {
					ceData.titleBarData.hasWindow = !!(tb->window());
					if (populateReliantFields) {
						if (ceData.titleBarData.hasWindow) {
							ceData.titleBarData.windowState = (TQt::WindowState)(tb->window()->windowState());
						}
						ceData.titleBarData.usesActiveColor = tb->usesActiveColor();
					}
					ceData.titleBarData.visibleText = tb->visibleText();
				}
			}
			if (ceDataPrivate->isTQSpinWidget) {
				const TQSpinWidget *sw = static_cast<const TQSpinWidget*>(widget);
				if (sw) {
					ceData.spinWidgetData.buttonSymbols = sw->buttonSymbols();
					ceData.spinWidgetData.upRect = sw->upRect();
					ceData.spinWidgetData.downRect = sw->downRect();
					ceData.spinWidgetData.upEnabled = sw->isUpEnabled();
					ceData.spinWidgetData.downEnabled = sw->isDownEnabled();
				}
			}
			if (ceDataPrivate->isTQListView) {
				const TQListView *lv = static_cast<const TQListView*>(widget);
				if (lv) {
					ceData.listViewData.rootDecorated = lv->rootIsDecorated();
					ceData.listViewData.itemMargin = lv->itemMargin();
					TQWidget* viewport = lv->viewport();
					if (viewport) {
						updateObjectTypeListIfNeeded(viewport);
						ceData.viewportData.widgetObjectTypes = (const_cast<TQWidget*>(viewport)->controlElementDataObject())->widgetObjectTypes;
						ceData.viewportData.allDataPopulated = populateReliantFields & populateMinimumNumberOfFields;
						const TQPixmap* erasePixmap = viewport->backgroundPixmap();
						if (erasePixmap) {
							ceData.viewportData.bgPixmap = *erasePixmap;
						}
						if (populateReliantFields) {
							ceData.viewportData.bgBrush = viewport->backgroundBrush();
						}
						if (populateReliantFields) {
							ceData.viewportData.windowState = (TQt::WindowState)(widget->windowState());
						}
						ceData.viewportData.wflags = viewport->getWFlags();
						ceData.viewportData.bgColor = viewport->eraseColor();
						ceData.viewportData.bgOffset = viewport->backgroundOffset();
						ceData.viewportData.backgroundMode = viewport->backgroundMode();
						if (populateReliantFields) {
							ceData.viewportData.fgColor = viewport->foregroundColor();
							ceData.viewportData.colorGroup = viewport->colorGroup();
							ceData.viewportData.paletteBgColor = viewport->paletteBackgroundColor();
						}
						ceData.viewportData.geometry = viewport->geometry();
						ceData.viewportData.rect = viewport->rect();
						ceData.viewportData.pos = viewport->pos();
						const TQPixmap* icon = viewport->icon();
						if (icon) {
							ceData.viewportData.icon = *icon;
						}
						ceData.viewportData.palette = viewport->palette();
						ceData.viewportData.font = viewport->font();
						ceData.viewportData.name = viewport->name();
						ceData.viewportData.caption = viewport->caption();
					}
				}
			}
			if (ceDataPrivate->isTQComboBox) {
				const TQComboBox *cb = static_cast<const TQComboBox*>(widget);
				if (cb) {
					const TQLineEdit* lineEdit = cb->lineEdit();
					if (lineEdit) {
						ceData.comboBoxLineEditFlags = getControlElementFlagsForObject(lineEdit, TQStyleOption::Default);
					}
					const TQListBox* listBox = cb->listBox();
					if (listBox) {
						ceData.comboBoxListBoxFlags = getControlElementFlagsForObject(listBox, TQStyleOption::Default);
					}
				}
			}
			if (ceDataPrivate->isTQFrame) {
				const TQFrame *frame = static_cast<const TQFrame*>(widget);
				if (frame) {
					ceData.frameStyle = frame->frameStyle();
				}
			}

			const TQWidget* parentWidget = widget->parentWidget();
			if (parentWidget) {
				updateObjectTypeListIfNeeded(parentWidget);
				ceData.parentWidgetData.widgetObjectTypes = (const_cast<TQWidget*>(parentWidget)->controlElementDataObject())->widgetObjectTypes;
				TQStyleControlElementDataPrivate* parentWidgetCEDataPrivate = const_cast<TQWidget*>(parentWidget)->controlElementDataPrivateObject();
				ceData.parentWidgetData.allDataPopulated = populateReliantFields & populateMinimumNumberOfFields;
				const TQPixmap* erasePixmap = parentWidget->backgroundPixmap();
				if (erasePixmap) {
					ceData.parentWidgetData.bgPixmap = *erasePixmap;
				}
				if (populateReliantFields) {
					ceData.parentWidgetData.bgBrush = parentWidget->backgroundBrush();
				}
				if (populateReliantFields) {
					ceData.parentWidgetData.windowState = (TQt::WindowState)(widget->windowState());
				}
				ceData.parentWidgetData.wflags = parentWidget->getWFlags();
				ceData.parentWidgetData.bgColor = parentWidget->eraseColor();
				ceData.parentWidgetData.bgOffset = parentWidget->backgroundOffset();
				ceData.parentWidgetData.backgroundMode = parentWidget->backgroundMode();
				if (populateReliantFields) {
					ceData.parentWidgetData.fgColor = parentWidget->foregroundColor();
					ceData.parentWidgetData.colorGroup = parentWidget->colorGroup();
					ceData.parentWidgetData.paletteBgColor = parentWidget->paletteBackgroundColor();
				}
				ceData.parentWidgetData.geometry = parentWidget->geometry();
				ceData.parentWidgetData.rect = parentWidget->rect();
				ceData.parentWidgetData.pos = parentWidget->pos();
				const TQPixmap* icon = parentWidget->icon();
				if (icon) {
					ceData.parentWidgetData.icon = *icon;
				}
				ceData.parentWidgetData.palette = parentWidget->palette();
				ceData.parentWidgetData.font = parentWidget->font();
				ceData.parentWidgetData.name = parentWidget->name();
				ceData.parentWidgetData.caption = parentWidget->caption();

				if (parentWidgetCEDataPrivate->isTQDockWindow) {
					const TQDockWindow * dw = static_cast<const TQDockWindow*>(parentWidget);
					if (dw->area()) {
						ceData.dwData.hasDockArea = true;
						ceData.dwData.areaOrientation = dw->area()->orientation();
					}
					else {
						ceData.dwData.hasDockArea = false;
					}
					ceData.dwData.closeEnabled = dw->isCloseEnabled();
				}
				if (parentWidgetCEDataPrivate->isTQToolBar) {
					const TQToolBar * toolbar = static_cast<const TQToolBar*>(parentWidget);
					ceData.toolBarData.orientation = toolbar->orientation();
				}
				ceData.parentWidgetFlags = getControlElementFlagsForObject(parentWidget, TQStyleOption::Default, populateReliantFields);
			}
			const TQWidget* topLevelWidget = widget->topLevelWidget();
			if (topLevelWidget) {
				updateObjectTypeListIfNeeded(topLevelWidget);
				ceData.topLevelWidgetData.widgetObjectTypes = (const_cast<TQWidget*>(topLevelWidget)->controlElementDataObject())->widgetObjectTypes;
				TQStyleControlElementDataPrivate* topLevelWidgetCEDataPrivate = const_cast<TQWidget*>(topLevelWidget)->controlElementDataPrivateObject();
				ceData.topLevelWidgetData.allDataPopulated = populateReliantFields & populateMinimumNumberOfFields;
				const TQPixmap* erasePixmap = topLevelWidget->backgroundPixmap();
				if (erasePixmap) {
					ceData.topLevelWidgetData.bgPixmap = *erasePixmap;
				}
				if (populateReliantFields) {
					ceData.topLevelWidgetData.bgBrush = topLevelWidget->backgroundBrush();
				}
				if (populateReliantFields) {
					ceData.topLevelWidgetData.windowState = (TQt::WindowState)(widget->windowState());
				}
				ceData.topLevelWidgetData.wflags = topLevelWidget->getWFlags();
				ceData.topLevelWidgetData.bgColor = topLevelWidget->eraseColor();
				ceData.topLevelWidgetData.bgOffset = topLevelWidget->backgroundOffset();
				ceData.topLevelWidgetData.backgroundMode = topLevelWidget->backgroundMode();
				if (populateReliantFields) {
					ceData.topLevelWidgetData.fgColor = topLevelWidget->foregroundColor();
					ceData.topLevelWidgetData.colorGroup = topLevelWidget->colorGroup();
					ceData.topLevelWidgetData.paletteBgColor = topLevelWidget->paletteBackgroundColor();
				}
				ceData.topLevelWidgetData.geometry = topLevelWidget->geometry();
				ceData.topLevelWidgetData.rect = topLevelWidget->rect();
				ceData.topLevelWidgetData.pos = topLevelWidget->pos();
				const TQPixmap* icon = topLevelWidget->icon();
				if (icon) {
					ceData.topLevelWidgetData.icon = *icon;
				}
				ceData.topLevelWidgetData.palette = topLevelWidget->palette();
				ceData.topLevelWidgetData.font = topLevelWidget->font();
				ceData.topLevelWidgetData.name = topLevelWidget->name();
				ceData.topLevelWidgetData.caption = topLevelWidget->caption();

				if (topLevelWidgetCEDataPrivate->isTQDockWindow) {
					const TQDockWindow * dw = static_cast<const TQDockWindow*>(topLevelWidget);
					if (dw->area()) {
						ceData.dwData.hasDockArea = true;
						ceData.dwData.areaOrientation = dw->area()->orientation();
					}
					else {
						ceData.dwData.hasDockArea = false;
					}
					ceData.dwData.closeEnabled = dw->isCloseEnabled();
				}
				if (topLevelWidgetCEDataPrivate->isTQToolBar) {
					const TQToolBar * toolbar = static_cast<const TQToolBar*>(topLevelWidget);
					ceData.toolBarData.orientation = toolbar->orientation();
				}
				ceData.topLevelWidgetFlags = getControlElementFlagsForObject(topLevelWidget, TQStyleOption::Default, populateReliantFields);
			}

			TQCheckListItem *item = opt.checkListItem();
			if (item) {
				ceData.checkListItemData.dataValid = true;
				ceData.checkListItemData.enabled = item->isEnabled();
				ceData.checkListItemData.hasParent = ( item->parent() && (item->parent()->rtti() == 1) && (((TQCheckListItem*) item->parent())->type() == TQCheckListItem::Controller) );
				ceData.checkListItemData.height = item->height();
			}
			else {
				ceData.checkListItemData.dataValid = false;
			}
		}

		return ceData;
	}
	else {
		if (!TQStyleControlElementData_null) {
			TQStyleControlElementData_null = new TQStyleControlElementData();
			tqAddPostRoutine(tqt_style_control_element_data_null_cleanup);
		}
		TQStyleControlElementData &ceData = *TQStyleControlElementData_null;
		return ceData;
	}
}

const TQStyleControlElementData &populateControlElementDataFromApplication(const TQApplication* app, const TQStyleOption&, bool) {
	TQStyleControlElementData &ceData = *(const_cast<TQApplication*>(app)->controlElementDataObject());

	if (app) {
		updateObjectTypeListIfNeeded(app);
		ceData.widgetObjectTypes = (const_cast<TQApplication*>(app)->controlElementDataObject())->widgetObjectTypes;
		ceData.palette = app->palette();
		ceData.font = app->font();
	}

	return ceData;
}

/*! \reimp */
void TQCommonStyle::drawPrimitive( PrimitiveElement pe,
				  TQPainter *p,
				  const TQStyleControlElementData &ceData,
				  ControlElementFlags elementFlags,
				  const TQRect &r,
				  const TQColorGroup &cg,
				  SFlags flags,
				  const TQStyleOption& opt ) const
{
    activePainter = p;

    switch (pe) {
#ifndef TQT_NO_LISTVIEW
    case PE_CheckListController: {
	p->drawPixmap(r, TQPixmap((const char **)check_list_controller_xpm));
	break; }
    case PE_CheckListExclusiveIndicator: {
	if(!ceData.checkListItemData.dataValid)
	    return;
	int x = r.x(), y = r.y();
#define TQCOORDARRLEN(x) sizeof(x)/(sizeof(TQCOORD)*2)
	static const TQCOORD pts1[] = {		// dark lines
	    1,9, 1,8, 0,7, 0,4, 1,3, 1,2, 2,1, 3,1, 4,0, 7,0, 8,1, 9,1 };
	static const TQCOORD pts2[] = {		// black lines
	    2,8, 1,7, 1,4, 2,3, 2,2, 3,2, 4,1, 7,1, 8,2, 9,2 };
	static const TQCOORD pts3[] = {		// background lines
	    2,9, 3,9, 4,10, 7,10, 8,9, 9,9, 9,8, 10,7, 10,4, 9,3 };
	static const TQCOORD pts4[] = {		// white lines
	    2,10, 3,10, 4,11, 7,11, 8,10, 9,10, 10,9, 10,8, 11,7,
	    11,4, 10,3, 10,2 };
	// static const TQCOORD pts5[] = {		// inner fill
	//    4,2, 7,2, 9,4, 9,7, 7,9, 4,9, 2,7, 2,4 };
	//TQPointArray a;
	//	p->eraseRect( x, y, w, h );

	if ( flags & Style_Enabled )
	    p->setPen( cg.text() );
	else
	    p->setPen( TQPen( ceData.palette.color( TQPalette::Disabled, TQColorGroup::Text ) ) );
	TQPointArray a( TQCOORDARRLEN(pts1), pts1 );
	a.translate( x, y );
	//p->setPen( cg.dark() );
	p->drawPolyline( a );
	a.setPoints( TQCOORDARRLEN(pts2), pts2 );
	a.translate( x, y );
	p->drawPolyline( a );
	a.setPoints( TQCOORDARRLEN(pts3), pts3 );
	a.translate( x, y );
	//		p->setPen( black );
	p->drawPolyline( a );
	a.setPoints( TQCOORDARRLEN(pts4), pts4 );
	a.translate( x, y );
	//			p->setPen( blue );
	p->drawPolyline( a );
	//		a.setPoints( TQCOORDARRLEN(pts5), pts5 );
	//		a.translate( x, y );
	//	TQColor fillColor = isDown() ? g.background() : g.base();
	//	p->setPen( fillColor );
	//	p->setBrush( fillColor );
	//	p->drawPolygon( a );
	if ( flags & Style_On ) {
	    p->setPen( NoPen );
	    p->setBrush( cg.text() );
	    p->drawRect( x+5, y+4, 2, 4 );
	    p->drawRect( x+4, y+5, 4, 2 );
	}
	break; }
    case PE_CheckListIndicator: {
	if(!ceData.checkListItemData.dataValid)
	    return;
	int x = r.x(), y = r.y(), w = r.width(), h = r.width(), marg = ceData.listViewData.itemMargin;

	if ( flags & Style_Enabled )
	    p->setPen( TQPen( cg.text(), 2 ) );
	else
	    p->setPen( TQPen( ceData.palette.color( TQPalette::Disabled, TQColorGroup::Text ),
			     2 ) );
	if ( flags & Style_Selected && !ceData.listViewData.rootDecorated && !ceData.checkListItemData.hasParent ) {
	    p->fillRect( 0, 0, x + marg + w + 4, ceData.checkListItemData.height,
			 cg.brush( TQColorGroup::Highlight ) );
	    if ( ceData.checkListItemData.enabled )
		p->setPen( TQPen( cg.highlightedText(), 2 ) );
	}

	if ( flags & Style_NoChange )
	    p->setBrush( cg.brush( TQColorGroup::Button ) );
	p->drawRect( x+marg, y+2, w-4, h-4 );
	/////////////////////
	x++;
	y++;
	if ( ( flags & Style_On) || ( flags & Style_NoChange ) ) {
	    TQPointArray a( 7*2 );
	    int i, xx = x+1+marg, yy=y+5;
	    for ( i=0; i<3; i++ ) {
		a.setPoint( 2*i,   xx, yy );
		a.setPoint( 2*i+1, xx, yy+2 );
		xx++; yy++;
	    }
	    yy -= 2;
	    for ( i=3; i<7; i++ ) {
		a.setPoint( 2*i,   xx, yy );
		a.setPoint( 2*i+1, xx, yy+2 );
		xx++; yy--;
	    }
	    p->drawLineSegments( a );
	}
	break; }
#endif
    case PE_HeaderArrow:
	p->save();
	if ( flags & Style_Down ) {
	    TQPointArray pa( 3 );
	    p->setPen( cg.light() );
	    p->drawLine( r.x() + r.width(), r.y(), r.x() + r.width() / 2, r.height() );
	    p->setPen( cg.dark() );
	    pa.setPoint( 0, r.x() + r.width() / 2, r.height() );
	    pa.setPoint( 1, r.x(), r.y() );
	    pa.setPoint( 2, r.x() + r.width(), r.y() );
	    p->drawPolyline( pa );
	} else {
	    TQPointArray pa( 3 );
	    p->setPen( cg.light() );
	    pa.setPoint( 0, r.x(), r.height() );
	    pa.setPoint( 1, r.x() + r.width(), r.height() );
	    pa.setPoint( 2, r.x() + r.width() / 2, r.y() );
	    p->drawPolyline( pa );
	    p->setPen( cg.dark() );
	    p->drawLine( r.x(), r.height(), r.x() + r.width() / 2, r.y() );
	}
	p->restore();
	break;

    case PE_StatusBarSection:
	qDrawShadeRect( p, r, cg, true, 1, 0, 0 );
	break;

    case PE_ButtonCommand:
    case PE_ButtonBevel:
    case PE_ButtonTool:
    case PE_ButtonDropDown:
    case PE_HeaderSection:
    case PE_HeaderSectionMenu:
	qDrawShadePanel(p, r, cg, flags & (Style_Sunken | Style_Down | Style_On) , 1,
			&cg.brush(TQColorGroup::Button));
	break;

    case PE_Separator:
	qDrawShadeLine( p, r.left(), r.top(), r.right(), r.bottom(), cg,
			flags & Style_Sunken, 1, 0);
	break;

    case PE_FocusRect: {
	const TQColor *bg = 0;

	if (!opt.isDefault())
	    bg = &opt.color();

	TQPen oldPen = p->pen();

	if (bg) {
	    int h, s, v;
	    bg->hsv(&h, &s, &v);
	    if (v >= 128)
		p->setPen(TQt::black);
	    else
		p->setPen(TQt::white);
	} else
	    p->setPen(cg.foreground());

	if (flags & Style_FocusAtBorder)
	    p->drawRect(TQRect(r.x() + 1, r.y() + 1, r.width() - 2, r.height() - 2));
	else
	    p->drawRect(r);

	p->setPen(oldPen);
	break; }

    case PE_SpinWidgetPlus:
    case PE_SpinWidgetMinus: {
	p->save();
	int fw = pixelMetric( PM_DefaultFrameWidth, ceData, elementFlags, 0 );
	TQRect br;
	br.setRect( r.x() + fw, r.y() + fw, r.width() - fw*2,
		    r.height() - fw*2 );

	p->fillRect( br, cg.brush( TQColorGroup::Button ) );
	p->setPen( cg.buttonText() );
	p->setBrush( cg.buttonText() );

	int length;
	int x = r.x(), y = r.y(), w = r.width(), h = r.height();
	if ( w <= 8 || h <= 6 )
	    length = TQMIN( w-2, h-2 );
	else
	    length = TQMIN( 2*w / 3, 2*h / 3 );

	if ( !(length & 1) )
	    length -=1;
	int xmarg = ( w - length ) / 2;
	int ymarg = ( h - length ) / 2;

	p->drawLine( x + xmarg, ( y + h / 2 - 1 ),
		     x + xmarg + length - 1, ( y + h / 2 - 1 ) );
	if ( pe == PE_SpinWidgetPlus )
	    p->drawLine( ( x+w / 2 ) - 1, y + ymarg,
			 ( x+w / 2 ) - 1, y + ymarg + length - 1 );
	p->restore();
	break; }

    case PE_SpinWidgetUp:
    case PE_SpinWidgetDown: {
	int fw = pixelMetric( PM_DefaultFrameWidth, ceData, elementFlags, 0 );
	TQRect br;
	br.setRect( r.x() + fw, r.y() + fw, r.width() - fw*2,
		    r.height() - fw*2 );
	p->fillRect( br, cg.brush( TQColorGroup::Button ) );
	int x = r.x(), y = r.y(), w = r.width(), h = r.height();
	int sw = w-4;
	if ( sw < 3 )
	    break;
	else if ( !(sw & 1) )
	    sw--;
	sw -= ( sw / 7 ) * 2;	// Empty border
	int sh = sw/2 + 2;      // Must have empty row at foot of arrow

	int sx = x + w / 2 - sw / 2 - 1;
	int sy = y + h / 2 - sh / 2 - 1;

	TQPointArray a;
	if ( pe == PE_SpinWidgetDown )
	    a.setPoints( 3,  0, 1,  sw-1, 1,  sh-2, sh-1 );
	else
	    a.setPoints( 3,  0, sh-1,  sw-1, sh-1,  sh-2, 1 );
	int bsx = 0;
	int bsy = 0;
	if ( flags & Style_Sunken ) {
	    bsx = pixelMetric(PM_ButtonShiftHorizontal, ceData, elementFlags);
	    bsy = pixelMetric(PM_ButtonShiftVertical, ceData, elementFlags);
	}
	p->save();
	p->translate( sx + bsx, sy + bsy );
	p->setPen( cg.buttonText() );
	p->setBrush( cg.buttonText() );
	p->drawPolygon( a );
	p->restore();
	break; }

    case PE_Indicator: {
	if (flags & Style_NoChange) {
	    p->setPen(cg.foreground());
	    p->fillRect(r, cg.brush(TQColorGroup::Button));
	    p->drawRect(r);
	    p->drawLine(r.topLeft(), r.bottomRight());
	} else
	    qDrawShadePanel(p, r.x(), r.y(), r.width(), r.height(),
			    cg, flags & (Style_Sunken | Style_On), 1,
			    &cg.brush(TQColorGroup::Button));
	break; }

    case PE_IndicatorMask: {
	p->fillRect(r, color1);
	break; }

    case PE_ExclusiveIndicator: {
	TQRect ir = r;
	p->setPen(cg.dark());
	p->drawArc(r, 0, 5760);

	if (flags & (Style_Sunken | Style_On)) {
	    ir.addCoords(2, 2, -2, -2);
	    p->setBrush(cg.foreground());
	    p->drawEllipse(ir);
	}

	break; }

    case PE_ExclusiveIndicatorMask: {
	p->setPen(color1);
	p->setBrush(color1);
	p->drawEllipse(r);
	break; }

    case PE_DockWindowHandle: {
	bool highlight = flags & Style_On;

	p->save();
	p->translate( r.x(), r.y() );
	if ( flags & Style_Horizontal ) {
	    int x = r.width() / 3;
	    if ( r.height() > 4 ) {
		qDrawShadePanel( p, x, 2, 3, r.height() - 4,
				 cg, highlight, 1, 0 );
		qDrawShadePanel( p, x+3, 2, 3, r.height() - 4,
				 cg, highlight, 1, 0 );
	    }
	} else {
	    if ( r.width() > 4 ) {
		int y = r.height() / 3;
		qDrawShadePanel( p, 2, y, r.width() - 4, 3,
				 cg, highlight, 1, 0 );
		qDrawShadePanel( p, 2, y+3, r.width() - 4, 3,
				 cg, highlight, 1, 0 );
	    }
	}
	p->restore();
	break;
    }

    case PE_DockWindowSeparator: {
	TQPoint p1, p2;
	if ( flags & Style_Horizontal ) {
	    p1 = TQPoint( r.width()/2, 0 );
	    p2 = TQPoint( p1.x(), r.height() );
	} else {
	    p1 = TQPoint( 0, r.height()/2 );
	    p2 = TQPoint( r.width(), p1.y() );
	}
	qDrawShadeLine( p, p1, p2, cg, 1, 1, 0 );
	break; }

    case PE_Panel:
    case PE_PanelPopup: {
	int lw = opt.isDefault() ? pixelMetric(PM_DefaultFrameWidth, ceData, elementFlags)
		    : opt.lineWidth();

	qDrawShadePanel(p, r, cg, (flags & Style_Sunken), lw);
	break; }

    case PE_PanelDockWindow: {
	int lw = opt.isDefault() ? pixelMetric(PM_DockWindowFrameWidth, ceData, elementFlags)
		    : opt.lineWidth();

	qDrawShadePanel(p, r, cg, false, lw);
	break; }

    case PE_PanelMenuBar: {
	int lw = opt.isDefault() ? pixelMetric(PM_MenuBarFrameWidth, ceData, elementFlags)
		    : opt.lineWidth();

	qDrawShadePanel(p, r, cg, false, lw, &cg.brush(TQColorGroup::Button));
	break; }

    case PE_SizeGrip: {
	p->save();

	int x, y, w, h;
	r.rect(&x, &y, &w, &h);

	int sw = TQMIN( h,w );
	if ( h > w )
	    p->translate( 0, h - w );
	else
	    p->translate( w - h, 0 );

	int sx = x;
	int sy = y;
	int s = sw / 3;

	if ( TQApplication::reverseLayout() ) {
	    sx = x + sw;
	    for ( int i = 0; i < 4; ++i ) {
		p->setPen( TQPen( cg.light(), 1 ) );
		p->drawLine(  x, sy - 1 , sx + 1,  sw );
		p->setPen( TQPen( cg.dark(), 1 ) );
		p->drawLine(  x, sy, sx,  sw );
		p->setPen( TQPen( cg.dark(), 1 ) );
		p->drawLine(  x, sy + 1, sx - 1,  sw );
		sx -= s;
		sy += s;
	    }
	} else {
	    for ( int i = 0; i < 4; ++i ) {
		p->setPen( TQPen( cg.light(), 1 ) );
		p->drawLine(  sx-1, sw, sw,  sy-1 );
		p->setPen( TQPen( cg.dark(), 1 ) );
		p->drawLine(  sx, sw, sw,  sy );
		p->setPen( TQPen( cg.dark(), 1 ) );
		p->drawLine(  sx+1, sw, sw,  sy+1 );
		sx += s;
		sy += s;
	    }
	}

	p->restore();
	break; }

    case PE_CheckMark: {
	const int markW = r.width() > 7 ? 7 : r.width();
	const int markH = markW;
	int posX = r.x() + ( r.width() - markW )/2 + 1;
	int posY = r.y() + ( r.height() - markH )/2;

	// Could do with some optimizing/caching...
	TQPointArray a( markH*2 );
	int i, xx, yy;
	xx = posX;
	yy = 3 + posY;
	for ( i=0; i<markW/2; i++ ) {
	    a.setPoint( 2*i,   xx, yy );
	    a.setPoint( 2*i+1, xx, yy+2 );
	    xx++; yy++;
	}
	yy -= 2;
	for ( ; i<markH; i++ ) {
	    a.setPoint( 2*i,   xx, yy );
	    a.setPoint( 2*i+1, xx, yy+2 );
	    xx++; yy--;
	}
	if ( !(flags & Style_Enabled) && !(flags & Style_On)) {
	    int pnt;
	    p->setPen( cg.highlightedText() );
	    TQPoint offset(1,1);
	    for ( pnt = 0; pnt < (int)a.size(); pnt++ )
		a[pnt] += offset;
	    p->drawLineSegments( a );
	    for ( pnt = 0; pnt < (int)a.size(); pnt++ )
		a[pnt] -= offset;
	}
	p->setPen( cg.text() );
	p->drawLineSegments( a );
	break; }

    case PE_PanelGroupBox: //We really do not need PE_GroupBoxFrame anymore, nasty holdover ###
	drawPrimitive( PE_GroupBoxFrame, p, ceData, elementFlags, r, cg, flags, opt );
	break;
    case PE_GroupBoxFrame: {
#ifndef TQT_NO_FRAME
	if ( opt.isDefault() )
	    break;
	int lwidth = opt.lineWidth(), mlwidth = opt.midLineWidth();
	if ( flags & (Style_Sunken|Style_Raised))
	    qDrawShadeRect( p, r.x(), r.y(), r.width(), r.height(), cg, flags & Style_Sunken, lwidth, mlwidth );
	else
	    qDrawPlainRect( p, r.x(), r.y(), r.width(), r.height(), cg.foreground(), lwidth );
#endif
	break; }

    case PE_ProgressBarChunk:
	p->fillRect( r.x(), r.y() + 3, r.width() -2, r.height() - 6,
	    cg.brush(TQColorGroup::Highlight));
	break;

    case PE_PanelLineEdit:
    case PE_PanelTabWidget:
    case PE_WindowFrame:
	drawPrimitive( PE_Panel, p, ceData, elementFlags, r, cg, flags, opt );
	break;

    case PE_RubberBand:
	drawPrimitive(PE_FocusRect, p, ceData, elementFlags, r, cg, flags, opt);
	break;
    default:
	break;
    }

    activePainter = 0;
}

void qt_style_erase_region(TQPainter* p, const TQRegion& region, const TQStyleControlElementData &ceData) {
	TQRegion reg = region;

	TQPoint offset = ceData.bgOffset;
	int ox = offset.x();
	int oy = offset.y();

	TQRegion origcr = p->clipRegion();
	p->setClipRegion(region);
	if (!ceData.bgPixmap.isNull()) {
		p->drawTiledPixmap(0, 0, ceData.geometry.width(), ceData.geometry.height(), ceData.bgPixmap, ox, oy);
	}
	else {
		p->fillRect(ceData.rect, ceData.bgColor);
	}
	p->setClipRegion(origcr);
	return;
}

/*! \reimp */
void TQCommonStyle::drawControl( ControlElement element,
				TQPainter *p,
				const TQStyleControlElementData &ceData,
				ControlElementFlags elementFlags,
				const TQRect &r,
				const TQColorGroup &cg,
				SFlags flags,
				const TQStyleOption& opt,
				const TQWidget *widget ) const
{
    activePainter = p;

    switch (element) {
    case CE_MenuBarEmptyArea: {
	TQRegion reg;
	if(p->hasClipping()) //ick
	    reg = p->clipRegion();
	else
	    reg = r;
	qt_style_erase_region(p, reg, ceData);
	break; }
    case CE_PushButton:
	{
#ifndef TQT_NO_PUSHBUTTON
	    TQRect br = r;
	    int dbi = pixelMetric(PM_ButtonDefaultIndicator, ceData, elementFlags, widget);

	    if ((elementFlags & CEF_IsDefault) || (elementFlags & CEF_AutoDefault)) {
		if (elementFlags & CEF_IsDefault) {
		    drawPrimitive(PE_ButtonDefault, p, ceData, elementFlags, br, cg, flags);
		}

		br.setCoords(br.left()   + dbi,
			     br.top()    + dbi,
			     br.right()  - dbi,
			     br.bottom() - dbi);
	    }

	    p->save();
	    p->setBrushOrigin( -ceData.bgOffset.x(),
			       -ceData.bgOffset.y() );
	    drawPrimitive(PE_ButtonCommand, p, ceData, elementFlags, br, cg, flags);
	    p->restore();
#endif
	    break;
	}

    case CE_PushButtonLabel:
	{
#ifndef TQT_NO_PUSHBUTTON
	    TQRect ir = r;

	    if ((elementFlags & CEF_IsDown) || (elementFlags & CEF_IsOn)) {
		flags |= Style_Sunken;
		ir.moveBy(pixelMetric(PM_ButtonShiftHorizontal, ceData, elementFlags, widget),
			  pixelMetric(PM_ButtonShiftVertical, ceData, elementFlags, widget));
	    }

	    if (elementFlags & CEF_IsMenuWidget) {
		int mbi = pixelMetric(PM_MenuButtonIndicator, ceData, elementFlags, widget);
		TQRect ar(ir.right() - mbi, ir.y() + 2, mbi - 4, ir.height() - 4);
		drawPrimitive(PE_ArrowDown, p, ceData, elementFlags, ar, cg, flags, opt);
		ir.setWidth(ir.width() - mbi);
	    }

	    int tf=AlignVCenter | ShowPrefix;
	    if ((!styleHint(SH_UnderlineAccelerator, ceData, elementFlags, TQStyleOption::Default, 0, widget)) || ((styleHint(SH_HideUnderlineAcceleratorWhenAltUp, ceData, elementFlags, TQStyleOption::Default, 0, widget)) && (!acceleratorsShown())))
		tf |= NoAccel;

#ifndef TQT_NO_ICONSET
	    if ( !ceData.iconSet.isNull() ) {
		TQIconSet::Mode mode =
		    (elementFlags & CEF_IsEnabled) ? TQIconSet::Normal : TQIconSet::Disabled;
		if ( mode == TQIconSet::Normal && (elementFlags & CEF_HasFocus) )
		    mode = TQIconSet::Active;

		TQIconSet::State state = TQIconSet::Off;
		if ( (elementFlags & CEF_BiState) && (elementFlags & CEF_IsOn) )
		    state = TQIconSet::On;

		TQPixmap pixmap = ceData.iconSet.pixmap( TQIconSet::Small, mode, state );
		int pixw = pixmap.width();
		int pixh = pixmap.height();

		//Center the icon if there is neither text nor pixmap
		if ( ceData.text.isEmpty() && ceData.fgPixmap.isNull() )
		    p->drawPixmap( ir.x() + ir.width() / 2 - pixw / 2, ir.y() + ir.height() / 2 - pixh / 2, pixmap );
		else
		    p->drawPixmap( ir.x() + 2, ir.y() + ir.height() / 2 - pixh / 2, pixmap );

		ir.moveBy(pixw + 4, 0);
		ir.setWidth(ir.width() - (pixw + 4));
		// left-align text if there is
		if (!ceData.text.isEmpty())
		    tf |= AlignLeft;
		else if (!ceData.fgPixmap.isNull())
		    tf |= AlignHCenter;
	    } else
#endif //TQT_NO_ICONSET
		tf |= AlignHCenter;
	    drawItem(p, ir, tf, cg,
		     flags & Style_Enabled, (ceData.fgPixmap.isNull())?NULL:&ceData.fgPixmap, ceData.text,
		     ceData.text.length(), &(cg.buttonText()) );

	    if (flags & Style_HasFocus)
		drawPrimitive(PE_FocusRect, p, ceData, elementFlags, subRect(SR_PushButtonFocusRect, ceData, elementFlags, widget),
			      cg, flags);
#endif
	    break;
	}

    case CE_CheckBox:
	drawPrimitive(PE_Indicator, p, ceData, elementFlags, r, cg, flags, opt);
	break;

    case CE_CheckBoxLabel:
	{
#ifndef TQT_NO_CHECKBOX
	    int alignment = TQApplication::reverseLayout() ? AlignRight : AlignLeft;
	    if ((!styleHint(SH_UnderlineAccelerator, ceData, elementFlags, TQStyleOption::Default, 0, widget)) || ((styleHint(SH_HideUnderlineAcceleratorWhenAltUp, ceData, elementFlags, TQStyleOption::Default, 0, widget)) && (!acceleratorsShown())))
		alignment |= NoAccel;

	    drawItem(p, r, alignment | AlignVCenter | ShowPrefix, cg,
		     flags & Style_Enabled, (ceData.fgPixmap.isNull())?NULL:&ceData.fgPixmap, ceData.text);

	    if (flags & Style_HasFocus) {
		TQRect fr = visualRect(subRect(SR_CheckBoxFocusRect, ceData, elementFlags, widget), ceData, elementFlags);
		drawPrimitive(PE_FocusRect, p, ceData, elementFlags, fr, cg, flags);
	    }
#endif
	    break;
	}

    case CE_RadioButton:
	drawPrimitive(PE_ExclusiveIndicator, p, ceData, elementFlags, r, cg, flags, opt);
	break;

    case CE_RadioButtonLabel:
	{
#ifndef TQT_NO_RADIOBUTTON
	    int alignment = TQApplication::reverseLayout() ? AlignRight : AlignLeft;
	    if ((!styleHint(SH_UnderlineAccelerator, ceData, elementFlags, TQStyleOption::Default, 0, widget)) || ((styleHint(SH_HideUnderlineAcceleratorWhenAltUp, ceData, elementFlags, TQStyleOption::Default, 0, widget)) && (!acceleratorsShown())))
		alignment |= NoAccel;
	    drawItem(p, r, alignment | AlignVCenter | ShowPrefix, cg,
		     flags & Style_Enabled, (ceData.fgPixmap.isNull())?NULL:&ceData.fgPixmap, ceData.text);

	    if (flags & Style_HasFocus) {
		TQRect fr = visualRect(subRect(SR_RadioButtonFocusRect, ceData, elementFlags, widget), ceData, elementFlags);
		drawPrimitive(PE_FocusRect, p, ceData, elementFlags, fr, cg, flags);
	    }
#endif
	    break;
	}

#ifndef TQT_NO_TABBAR
    case CE_TabBarTab:
	{
	    if ( ceData.tabBarData.shape == TQTabBar::TriangularAbove ||
		 ceData.tabBarData.shape == TQTabBar::TriangularBelow ) {
		// triangular, above or below
		int y;
		int x;
		TQPointArray a( 10 );
		a.setPoint( 0, 0, -1 );
		a.setPoint( 1, 0, 0 );
		y = r.height()-2;
		x = y/3;
		a.setPoint( 2, x++, y-1 );
		a.setPoint( 3, x++, y );
		a.setPoint( 3, x++, y++ );
		a.setPoint( 4, x, y );

		int i;
		int right = r.width() - 1;
		for ( i = 0; i < 5; i++ )
		    a.setPoint( 9-i, right - a.point( i ).x(), a.point( i ).y() );

		if ( ceData.tabBarData.shape == TQTabBar::TriangularAbove )
		    for ( i = 0; i < 10; i++ )
			a.setPoint( i, a.point(i).x(),
				    r.height() - 1 - a.point( i ).y() );

		a.translate( r.left(), r.top() );

		if ( flags & Style_Selected )
		    p->setBrush( cg.base() );
		else
		    p->setBrush( cg.background() );
		p->setPen( cg.foreground() );
		p->drawPolygon( a );
		p->setBrush( NoBrush );
	    }
	    break;
	}

    case CE_TabBarLabel:
	{
	    if ( opt.isDefault() )
		break;

	    TQTab * t = opt.tab();

	    TQRect tr = r;
	    if (elementFlags & CEF_IsActive)
		tr.setBottom( tr.bottom() -
			      pixelMetric( TQStyle::PM_DefaultFrameWidth, ceData, elementFlags, widget ) );

	    int alignment = AlignCenter | ShowPrefix;
	    if ((!styleHint(SH_UnderlineAccelerator, ceData, elementFlags, TQStyleOption::Default, 0, widget)) || ((styleHint(SH_HideUnderlineAcceleratorWhenAltUp, ceData, elementFlags, TQStyleOption::Default, 0, widget)) && (!acceleratorsShown())))
		alignment |= NoAccel;
	    drawItem( p, tr, alignment, cg,
		      flags & Style_Enabled, 0, t->text() );

	    if ( (flags & Style_HasFocus) && !t->text().isEmpty() )
		drawPrimitive( PE_FocusRect, p, ceData, elementFlags, r, cg );
	    break;
	}
#endif // TQT_NO_TABBAR
#ifndef TQT_NO_TOOLBOX
    case CE_ToolBoxTab:
	{
	    int d = 20 + r.height() - 3;
	    TQPointArray a( 7 );
	    a.setPoint( 0, -1, r.height() + 1 );
	    a.setPoint( 1, -1, 1 );
	    a.setPoint( 2, r.width() - d, 1 );
	    a.setPoint( 3, r.width() - 20, r.height() - 2 );
	    a.setPoint( 4, r.width() - 1, r.height() - 2 );
	    a.setPoint( 5, r.width() - 1, r.height() + 1 );
	    a.setPoint( 6, -1, r.height() + 1 );

	    if ( flags & Style_Selected && !(elementFlags & CEF_IsContainerEmpty) )
		p->setBrush( ceData.activeItemPaletteBgColor );
	    else
		p->setBrush( cg.brush(TQColorGroup::Background) );

	    p->setPen( cg.mid().dark( 150 ) );
	    p->drawPolygon( a );
	    p->setPen( cg.light() );
	    p->drawLine( 0, 2, r.width() - d, 2 );
	    p->drawLine( r.width() - d - 1, 2, r.width() - 21, r.height() - 1 );
	    p->drawLine( r.width() - 20, r.height() - 1, r.width(), r.height() - 1 );
	    p->setBrush( NoBrush );
	    break;
	}
#endif // TQT_NO_TOOLBOX
    case CE_ProgressBarGroove:
	qDrawShadePanel(p, r, cg, true, 1, &cg.brush(TQColorGroup::Background));
	break;

#ifndef TQT_NO_PROGRESSBAR
    case CE_ProgressBarContents:
	{
	    // Correct the highlight color if same as background,
	    // or else we cannot see the progress...
	    TQColorGroup cgh = cg;
	    if ( cgh.highlight() == cgh.background() )
		cgh.setColor( TQColorGroup::Highlight, ceData.palette.active().highlight() );
	    bool reverse = TQApplication::reverseLayout();
	    int fw = 2;
	    int w = r.width() - 2*fw;
	    if ( !ceData.totalSteps ) {
		// draw busy indicator
		int x = ceData.currentStep % (w * 2);
		if (x > w)
		    x = 2 * w - x;
		x = reverse ? r.right() - x : x + r.x();
		p->setPen( TQPen(cgh.highlight(), 4) );
		p->drawLine(x, r.y() + 1, x, r.height() - fw);
	    } else {
		const int unit_width = pixelMetric(PM_ProgressBarChunkWidth, ceData, elementFlags, widget);
		int u;
		if ( unit_width > 1 )
		    u = (r.width()+unit_width/3) / unit_width;
		else
		    u = w / unit_width;
		int p_v = ceData.currentStep;
		int t_s = ceData.totalSteps ? ceData.totalSteps : 1;

		if ( u > 0 && p_v >= INT_MAX / u && t_s >= u ) {
		    // scale down to something usable.
		    p_v /= u;
		    t_s /= u;
		}

		// nu < tnu, if last chunk is only a partial chunk
		int tnu, nu;
		tnu = nu = p_v * u / t_s;

		if (nu * unit_width > w)
		    nu--;

		// Draw nu units out of a possible u of unit_width
		// width, each a rectangle bordered by background
		// color, all in a sunken panel with a percentage text
		// display at the end.
		int x = 0;
		int x0 = reverse ? r.right() - ((unit_width > 1) ?
						unit_width : fw) : r.x() + fw;
		for (int i=0; i<nu; i++) {
		    drawPrimitive( PE_ProgressBarChunk, p, ceData, elementFlags,
				   TQRect( x0+x, r.y(), unit_width, r.height() ),
				   cgh, Style_Default, opt );
		    x += reverse ? -unit_width: unit_width;
		}

		// Draw the last partial chunk to fill up the
		// progressbar entirely
		if (nu < tnu) {
		    int pixels_left = w - (nu*unit_width);
		    int offset = reverse ? x0+x+unit_width-pixels_left : x0+x;
		    drawPrimitive( PE_ProgressBarChunk, p, ceData, elementFlags,
				   TQRect( offset, r.y(), pixels_left,
					  r.height() ), cgh, Style_Default,
				   opt );
		}
	    }
	}
	break;

    case CE_ProgressBarLabel:
	{
	    TQColor penColor = cg.highlightedText();
	    TQColor *pcolor = 0;
	    if ( (elementFlags & CEF_CenterIndicator) && !(elementFlags & CEF_IndicatorFollowsStyle) &&
		 ceData.currentStep*2 >= ceData.totalSteps )
		pcolor = &penColor;
	    drawItem(p, r, AlignCenter | SingleLine, cg, flags & Style_Enabled, 0,
		     ceData.progressText, -1, pcolor );
	}
	break;
#endif // TQT_NO_PROGRESSBAR

    case CE_MenuBarItem:
	{
#ifndef TQT_NO_MENUDATA
	    if (opt.isDefault())
		break;

	    TQMenuItem *mi = opt.menuItem();
	    int alignment = AlignCenter|ShowPrefix|DontClip|SingleLine;
	    if ((!styleHint(SH_UnderlineAccelerator, ceData, elementFlags, TQStyleOption::Default, 0, widget)) || ((styleHint(SH_HideUnderlineAcceleratorWhenAltUp, ceData, elementFlags, TQStyleOption::Default, 0, widget)) && (!acceleratorsShown())))
		alignment |= NoAccel;
	    drawItem( p, r, alignment, cg,
		      flags & Style_Enabled, mi->pixmap(), mi->text(), -1,
		      &cg.buttonText() );
#endif
	    break;
	}

#ifndef TQT_NO_TOOLBUTTON
    case CE_ToolButtonLabel:
	{
	    TQRect rect = r;
	    TQt::ArrowType arrowType = opt.isDefault()
			? TQt::DownArrow : opt.arrowType();

	    int shiftX = 0;
	    int shiftY = 0;
	    if (flags & (Style_Down | Style_On)) {
		shiftX = pixelMetric(PM_ButtonShiftHorizontal, ceData, elementFlags, widget);
		shiftY = pixelMetric(PM_ButtonShiftVertical, ceData, elementFlags, widget);
	    }

	    if (!opt.isDefault()) {
		PrimitiveElement pe;
		switch (arrowType) {
		case TQt::LeftArrow:  pe = PE_ArrowLeft;  break;
		case TQt::RightArrow: pe = PE_ArrowRight; break;
		case TQt::UpArrow:    pe = PE_ArrowUp;    break;
		default:
		case TQt::DownArrow:  pe = PE_ArrowDown;  break;
		}

		rect.moveBy(shiftX, shiftY);
		drawPrimitive(pe, p, ceData, elementFlags, rect, cg, flags, opt);
	    } else {
		TQColor btext = ceData.fgColor;

		if (ceData.iconSet.isNull() &&
		    ! ceData.text.isNull() &&
		    ! (elementFlags & CEF_UsesTextLabel)) {
		    int alignment = AlignCenter | ShowPrefix;
		    if ((!styleHint(SH_UnderlineAccelerator, ceData, elementFlags, TQStyleOption::Default, 0, widget)) || ((styleHint(SH_HideUnderlineAcceleratorWhenAltUp, ceData, elementFlags, TQStyleOption::Default, 0, widget)) && (!acceleratorsShown())))
			alignment |= NoAccel;

		    rect.moveBy(shiftX, shiftY);
		    drawItem(p, rect, alignment, cg,
			     flags & Style_Enabled, 0, ceData.text,
			     ceData.text.length(), &btext);
		} else {
		    TQPixmap pm;
		    TQIconSet::Size size =
			(elementFlags & CEF_UsesBigPixmap) ? TQIconSet::Large : TQIconSet::Small;
		    TQIconSet::State state =
			(elementFlags & CEF_IsOn) ? TQIconSet::On : TQIconSet::Off;
		    TQIconSet::Mode mode;
		    if (! (elementFlags & CEF_IsEnabled))
			mode = TQIconSet::Disabled;
		    else if (flags & (Style_Down | Style_On) ||
			     ((flags & Style_Raised) && (flags & Style_AutoRaise)))
			mode = TQIconSet::Active;
		    else
			mode = TQIconSet::Normal;
		    pm = ceData.iconSet.pixmap( size, mode, state );

		    if (elementFlags & CEF_UsesTextLabel) {
			p->setFont( ceData.font );
			TQRect pr = rect, tr = rect;
			int alignment = ShowPrefix;
			if ((!styleHint(SH_UnderlineAccelerator, ceData, elementFlags, TQStyleOption::Default, 0, widget)) || ((styleHint(SH_HideUnderlineAcceleratorWhenAltUp, ceData, elementFlags, TQStyleOption::Default, 0, widget)) && (!acceleratorsShown())))
			    alignment |= NoAccel;

			if ( ceData.toolButtonTextPosition == TQToolButton::Under ) {
			    int fh = p->fontMetrics().height();
			    pr.addCoords( 0, 1, 0, -fh-3 );
			    tr.addCoords( 0, pr.bottom(), 0, -3 );
			    pr.moveBy(shiftX, shiftY);
			    drawItem( p, pr, AlignCenter, cg, true, &pm, TQString::null );
			    alignment |= AlignCenter;
			} else {
			    pr.setWidth( pm.width() + 8 );
			    tr.addCoords( pr.right(), 0, 0, 0 );
			    pr.moveBy(shiftX, shiftY);

			    drawItem( p, pr, AlignCenter, cg, true, &pm, TQString::null );
			    alignment |= AlignLeft | AlignVCenter;
			}

			tr.moveBy(shiftX, shiftY);
			drawItem( p, tr, alignment, cg,
				  flags & Style_Enabled, 0, ceData.textLabel,
				  ceData.textLabel.length(), &btext);
		    } else {
			rect.moveBy(shiftX, shiftY);
			drawItem( p, rect, AlignCenter, cg, true, &pm, TQString::null );
		    }
		}
	    }

	    break;
	}
#endif // TQT_NO_TOOLBUTTON
#ifndef TQT_NO_HEADER
        case CE_HeaderLabel:
	{
	    TQRect rect = r;

	    const TQIconSet* icon = &ceData.iconSet;
	    if ( icon ) {
		TQPixmap pixmap = icon->pixmap( TQIconSet::Small,
					       flags & Style_Enabled ?
					       TQIconSet::Normal : TQIconSet::Disabled );
		int pixw = pixmap.width();
		int pixh = pixmap.height();
		// "pixh - 1" because of tricky integer division

		TQRect pixRect = rect;
		pixRect.setY( rect.center().y() - (pixh - 1) / 2 );
		drawItem ( p, pixRect, AlignVCenter, cg, flags & Style_Enabled,
			   &pixmap, TQString::null );
                if (TQApplication::reverseLayout())
                    rect.setRight(rect.right() - pixw - 2);
                else
                    rect.setLeft(rect.left() + pixw + 2);
	    }

            if (rect.isValid())
                drawItem ( p, rect, AlignVCenter, cg, flags & Style_Enabled,
                           0, ceData.textLabel, -1, &(cg.buttonText()) );
	}
#endif // TQT_NO_HEADER
    default:
	break;
    }

    activePainter = 0;
}

/*! \reimp */
void TQCommonStyle::drawControlMask( ControlElement control,
				TQPainter *p,
				const TQStyleControlElementData &ceData,
				ControlElementFlags elementFlags,
				const TQRect &r,
				const TQStyleOption& opt,
				const TQWidget *widget ) const
{
    Q_UNUSED(ceData);
    Q_UNUSED(elementFlags);
    Q_UNUSED(widget);

    activePainter = p;

    TQColorGroup cg(color1,color1,color1,color1,color1,color1,color1,color1,color0);

    switch (control) {
    case CE_PushButton:
	drawPrimitive(PE_ButtonCommand, p, ceData, elementFlags, r, cg, Style_Default, opt);
	break;

    case CE_CheckBox:
	drawPrimitive(PE_IndicatorMask, p, ceData, elementFlags, r, cg, Style_Default, opt);
	break;

    case CE_RadioButton:
	drawPrimitive(PE_ExclusiveIndicatorMask, p, ceData, elementFlags, r, cg, Style_Default, opt);
	break;

    default:
	p->fillRect(r, color1);
	break;
    }

    activePainter = 0;
}

/*! \reimp */
TQRect TQCommonStyle::subRect(SubRect r, const TQStyleControlElementData &ceData, const ControlElementFlags elementFlags, const TQWidget *widget) const
{
    TQRect rect, wrect(ceData.rect);

    switch (r) {
#ifndef TQT_NO_DIALOGBUTTONS
    case SR_DialogButtonAbort:
    case SR_DialogButtonRetry:
    case SR_DialogButtonIgnore:
    case SR_DialogButtonAccept:
    case SR_DialogButtonReject:
    case SR_DialogButtonApply:
    case SR_DialogButtonHelp:
    case SR_DialogButtonAll:
    case SR_DialogButtonCustom: {
	TQDialogButtons::Button srch = TQDialogButtons::None;
	if(r == SR_DialogButtonAccept)
	    srch = TQDialogButtons::Accept;
	else if(r == SR_DialogButtonReject)
	    srch = TQDialogButtons::Reject;
	else if(r == SR_DialogButtonAll)
	    srch = TQDialogButtons::All;
	else if(r == SR_DialogButtonApply)
	    srch = TQDialogButtons::Apply;
	else if(r == SR_DialogButtonHelp)
	    srch = TQDialogButtons::Help;
	else if(r == SR_DialogButtonRetry)
	    srch = TQDialogButtons::Retry;
	else if(r == SR_DialogButtonIgnore)
	    srch = TQDialogButtons::Ignore;
	else if(r == SR_DialogButtonAbort)
	    srch = TQDialogButtons::Abort;

	const int bwidth = pixelMetric(PM_DialogButtonsButtonWidth, ceData, elementFlags, widget),
		 bheight = pixelMetric(PM_DialogButtonsButtonHeight, ceData, elementFlags, widget),
		  bspace = pixelMetric(PM_DialogButtonsSeparator, ceData, elementFlags, widget),
		      fw = pixelMetric(PM_DefaultFrameWidth, ceData, elementFlags, widget);
	int start = fw;
	if(ceData.orientation == Horizontal)
	    start = wrect.right() - fw;
	TQDialogButtons::Button btns[] = { TQDialogButtons::All, TQDialogButtons::Reject, TQDialogButtons::Accept, //reverse order (right to left)
					  TQDialogButtons::Apply, TQDialogButtons::Retry, TQDialogButtons::Ignore, TQDialogButtons::Abort,
					  TQDialogButtons::Help };
	for(unsigned int i = 0, cnt = 0; i < (sizeof(btns)/sizeof(btns[0])); i++) {
	    if (ceData.dlgVisibleButtons & btns[i]) {
		TQSize szH = ceData.dlgVisibleSizeHints[btns[i]];
		int mwidth = TQMAX(bwidth, szH.width()), mheight = TQMAX(bheight, szH.height());
		if(ceData.orientation == Horizontal) {
		    start -= mwidth;
		    if(cnt)
			start -= bspace;
		} else if(cnt) {
		    start += mheight;
		    start += bspace;
		}
		cnt++;
		if(btns[i] == srch) {
		    if(ceData.orientation == Horizontal)
			return TQRect(start, wrect.bottom() - fw - mheight, mwidth, mheight);
		    else
			return TQRect(fw, start, mwidth, mheight);
		}
	    }
	}
	if(r == SR_DialogButtonCustom) {
	    if(ceData.orientation == Horizontal)
		return TQRect(fw, fw, start - fw - bspace, wrect.height() - (fw*2));
	    else
		return TQRect(fw, start, wrect.width() - (fw*2), wrect.height() - start - (fw*2));
	}
	return TQRect(); }
#endif //TQT_NO_DIALOGBUTTONS
    case SR_PushButtonContents:
	{
#ifndef TQT_NO_PUSHBUTTON
	    int dx1, dx2;

	    dx1 = pixelMetric(PM_DefaultFrameWidth, ceData, elementFlags, widget);
	    if ((elementFlags & CEF_IsDefault) || (elementFlags & CEF_AutoDefault))
		dx1 += pixelMetric(PM_ButtonDefaultIndicator, ceData, elementFlags, widget);
	    dx2 = dx1 * 2;

	    rect.setRect(wrect.x()      + dx1,
			 wrect.y()      + dx1,
			 wrect.width()  - dx2,
			 wrect.height() - dx2);
#endif
	    break;
	}

    case SR_PushButtonFocusRect:
	{
#ifndef TQT_NO_PUSHBUTTON
	    int dbw1 = 0, dbw2 = 0;
	    if ((elementFlags & CEF_IsDefault) || (elementFlags & CEF_AutoDefault)) {
		dbw1 = pixelMetric(PM_ButtonDefaultIndicator, ceData, elementFlags, widget);
		dbw2 = dbw1 * 2;
	    }

	    int dfw1 = pixelMetric(PM_DefaultFrameWidth, ceData, elementFlags, widget) * 2,
		dfw2 = dfw1 * 2;

	    rect.setRect(wrect.x()      + dfw1 + dbw1,
			 wrect.y()      + dfw1 + dbw1,
			 wrect.width()  - dfw2 - dbw2,
			 wrect.height() - dfw2 - dbw2);
#endif
	    break;
	}

    case SR_CheckBoxIndicator:
	{
	    int h = pixelMetric( PM_IndicatorHeight, ceData, elementFlags, widget );
	    rect.setRect(0, ( wrect.height() - h ) / 2,
			 pixelMetric( PM_IndicatorWidth, ceData, elementFlags, widget ), h );
	    break;
	}

    case SR_CheckBoxContents:
	{
#ifndef TQT_NO_CHECKBOX
	    TQRect ir = subRect(SR_CheckBoxIndicator, ceData, elementFlags, widget);
	    rect.setRect(ir.right() + 6, wrect.y(),
			 wrect.width() - ir.width() - 6, wrect.height());
#endif
	    break;
	}

    case SR_CheckBoxFocusRect:
	{
#ifndef TQT_NO_CHECKBOX
	    if ( ceData.fgPixmap.isNull() && ceData.text.isEmpty() ) {
		rect = subRect( SR_CheckBoxIndicator, ceData, elementFlags, widget );
		rect.addCoords( 1, 1, -1, -1 );
		break;
	    }
	    TQRect cr = subRect(SR_CheckBoxContents, ceData, elementFlags, widget);

	    // don't create a painter if we have an active one
	    TQPainter *p = 0;
	    if (! activePainter)
		p = new TQPainter(widget);
	    rect = itemRect((activePainter ? activePainter : p),
			    cr, AlignLeft | AlignVCenter | ShowPrefix,
			    (elementFlags & CEF_IsEnabled),
			    (ceData.fgPixmap.isNull())?NULL:&ceData.fgPixmap,
			    ceData.text);

	    delete p;

	    rect.addCoords( -3, -2, 3, 2 );
	    rect = rect.intersect(wrect);
#endif
	    break;
	}

    case SR_RadioButtonIndicator:
	{
	    int h = pixelMetric( PM_ExclusiveIndicatorHeight, ceData, elementFlags, widget );
	    rect.setRect(0, ( wrect.height() - h ) / 2,
			 pixelMetric( PM_ExclusiveIndicatorWidth, ceData, elementFlags, widget ), h );
	    break;
	}

    case SR_RadioButtonContents:
	{
	    TQRect ir = subRect(SR_RadioButtonIndicator, ceData, elementFlags, widget);
	    rect.setRect(ir.right() + 6, wrect.y(),
			 wrect.width() - ir.width() - 6, wrect.height());
	    break;
	}

    case SR_RadioButtonFocusRect:
	{
#ifndef TQT_NO_RADIOBUTTON
	    if ( ceData.fgPixmap.isNull() && ceData.text.isEmpty() ) {
		rect = subRect( SR_RadioButtonIndicator, ceData, elementFlags, widget );
		rect.addCoords( 1, 1, -1, -1 );
		break;
	    }
	    TQRect cr = subRect(SR_RadioButtonContents, ceData, elementFlags, widget);

	    // don't create a painter if we have an active one
	    TQPainter *p = 0;
	    if (! activePainter)
		p = new TQPainter(widget);
	    rect = itemRect((activePainter ? activePainter : p),
			    cr, AlignLeft | AlignVCenter | ShowPrefix,
			    (elementFlags & CEF_IsEnabled),
			    (ceData.fgPixmap.isNull())?NULL:&ceData.fgPixmap,
			    ceData.text);
	    delete p;

	    rect.addCoords( -3, -2, 3, 2 );
	    rect = rect.intersect(wrect);
#endif
	    break;
	}

    case SR_ComboBoxFocusRect:
	rect.setRect(3, 3, ceData.rect.width()-6-16, ceData.rect.height()-6);
	break;

#ifndef TQT_NO_SLIDER
    case SR_SliderFocusRect:
	{
	    int tickOffset = pixelMetric( PM_SliderTickmarkOffset, ceData, elementFlags, widget );
	    int thickness  = pixelMetric( PM_SliderControlThickness, ceData, elementFlags, widget );

	    if ( ceData.orientation == Horizontal )
		rect.setRect( 0, tickOffset-1, ceData.rect.width(), thickness+2 );
	    else
		rect.setRect( tickOffset-1, 0, thickness+2, ceData.rect.height() );
	    rect = rect.intersect( ceData.rect ); // ## is this really necessary?
	    break;
	}
#endif // TQT_NO_SLIDER

#ifndef TQT_NO_MAINWINDOW
    case SR_DockWindowHandleRect:
	{
	    if (!(elementFlags & CEF_HasParentWidget))
		break;

	    if ( !ceData.dwData.hasDockArea || !ceData.dwData.closeEnabled )
		rect.setRect( 0, 0, ceData.rect.width(), ceData.rect.height() );
	    else {
		if ( ceData.dwData.areaOrientation == Horizontal )
		    rect.setRect(0, 15, ceData.rect.width(), ceData.rect.height() - 15);
		else
		    rect.setRect(0, 1, ceData.rect.width() - 15, ceData.rect.height() - 1);
	    }
	    break;
	}
#endif // TQT_NO_MAINWINDOW

    case SR_ProgressBarGroove:
    case SR_ProgressBarContents:
	{
#ifndef TQT_NO_PROGRESSBAR
	    TQFontMetrics fm( ( (!(elementFlags & CEF_UseGenericParameters)) ? TQFontMetrics(ceData.font) :
			       TQApplication::fontMetrics() ) );
	    int textw = 0;
	    if (ceData.percentageVisible)
		textw = fm.width("100%") + 6;

	    if ((elementFlags & CEF_IndicatorFollowsStyle) ||
		! (elementFlags & CEF_CenterIndicator))
		rect.setCoords(wrect.left(), wrect.top(),
			       wrect.right() - textw, wrect.bottom());
	    else
		rect = wrect;
#endif
	    break;
	}

    case SR_ProgressBarLabel:
	{
#ifndef TQT_NO_PROGRESSBAR
	    TQFontMetrics fm( ( (!(elementFlags & CEF_UseGenericParameters)) ? TQFontMetrics(ceData.font) :
			       TQApplication::fontMetrics() ) );
	    int textw = 0;
	    if (ceData.percentageVisible)
		textw = fm.width("100%") + 6;

	    if ((elementFlags & CEF_IndicatorFollowsStyle) ||
		! (elementFlags & CEF_CenterIndicator))
		rect.setCoords(wrect.right() - textw, wrect.top(),
			       wrect.right(), wrect.bottom());
	    else
		rect = wrect;
#endif
	    break;
	}

    case SR_ToolButtonContents:
	rect = querySubControlMetrics(CC_ToolButton, ceData, elementFlags, SC_ToolButton, TQStyleOption::Default, widget);
	break;

    case SR_ToolBoxTabContents:
	rect = wrect;
	rect.addCoords( 0, 0, -30, 0 );
	break;

    default:
	rect = wrect;
	break;
    }

    return rect;
}

#ifndef TQT_NO_RANGECONTROL
/*
  I really need this and I don't want to expose it in TQRangeControl..
*/
static int qPositionFromValue( const TQStyleControlElementData &ceData, int logical_val,
			       int span )
{
    if ( span <= 0 || logical_val < ceData.minSteps ||
	 ceData.maxSteps <= ceData.minSteps )
	return 0;
    if ( logical_val > ceData.maxSteps )
	return span;

    uint range = ceData.maxSteps - ceData.minSteps;
    uint p = logical_val - ceData.minSteps;

    if ( range > (uint)INT_MAX/4096 ) {
	const int scale = 4096*2;
	return ( (p/scale) * span ) / (range/scale);
	// ### the above line is probably not 100% correct
	// ### but fixing it isn't worth the extreme pain...
    } else if ( range > (uint)span ) {
	return (2*p*span + range) / (2*range);
    } else {
	uint div = span / range;
	uint mod = span % range;
	return p*div + (2*p*mod + range) / (2*range);
    }
    //equiv. to (p*span)/range + 0.5
    // no overflow because of this implicit assumption:
    // span <= 4096
}
#endif // TQT_NO_RANGECONTROL

/*! \reimp */
void TQCommonStyle::drawComplexControl( ComplexControl control,
				       TQPainter *p,
				       const TQStyleControlElementData &ceData,
				       ControlElementFlags elementFlags,
				       const TQRect &r,
				       const TQColorGroup &cg,
				       SFlags flags,
				       SCFlags controls,
				       SCFlags active,
				       const TQStyleOption& opt,
				       const TQWidget *widget ) const
{
    activePainter = p;

    switch (control) {
#ifndef TQT_NO_SCROLLBAR
    case CC_ScrollBar:
	{
	    TQRect addline, subline, addpage, subpage, slider, first, last;
	    bool maxedOut = (ceData.minSteps == ceData.maxSteps);

	    subline = querySubControlMetrics(control, ceData, elementFlags, SC_ScrollBarSubLine, opt, widget);
	    addline = querySubControlMetrics(control, ceData, elementFlags, SC_ScrollBarAddLine, opt, widget);
	    subpage = querySubControlMetrics(control, ceData, elementFlags, SC_ScrollBarSubPage, opt, widget);
	    addpage = querySubControlMetrics(control, ceData, elementFlags, SC_ScrollBarAddPage, opt, widget);
	    slider  = querySubControlMetrics(control, ceData, elementFlags, SC_ScrollBarSlider,  opt, widget);
	    first   = querySubControlMetrics(control, ceData, elementFlags, SC_ScrollBarFirst,   opt, widget);
	    last    = querySubControlMetrics(control, ceData, elementFlags, SC_ScrollBarLast,    opt, widget);

       	    if ((controls & SC_ScrollBarSubLine) && subline.isValid())
		drawPrimitive(PE_ScrollBarSubLine, p, ceData, elementFlags, subline, cg,
			      ((maxedOut) ? Style_Default : Style_Enabled) |
			      ((active == SC_ScrollBarSubLine) ?
			       Style_Down : Style_Default) |
			      ((ceData.orientation == TQt::Horizontal) ?
			       Style_Horizontal : 0));
	    if ((controls & SC_ScrollBarAddLine) && addline.isValid())
		drawPrimitive(PE_ScrollBarAddLine, p, ceData, elementFlags, addline, cg,
			      ((maxedOut) ? Style_Default : Style_Enabled) |
			      ((active == SC_ScrollBarAddLine) ?
			       Style_Down : Style_Default) |
			      ((ceData.orientation == TQt::Horizontal) ?
			       Style_Horizontal : 0));
	    if ((controls & SC_ScrollBarSubPage) && subpage.isValid())
		drawPrimitive(PE_ScrollBarSubPage, p, ceData, elementFlags, subpage, cg,
			      ((maxedOut) ? Style_Default : Style_Enabled) |
			      ((active == SC_ScrollBarSubPage) ?
			       Style_Down : Style_Default) |
			      ((ceData.orientation == TQt::Horizontal) ?
			       Style_Horizontal : 0));
	    if ((controls & SC_ScrollBarAddPage) && addpage.isValid())
		drawPrimitive(PE_ScrollBarAddPage, p, ceData, elementFlags, addpage, cg,
			      ((maxedOut) ? Style_Default : Style_Enabled) |
			      ((active == SC_ScrollBarAddPage) ?
			       Style_Down : Style_Default) |
			      ((ceData.orientation == TQt::Horizontal) ?
			       Style_Horizontal : 0));
       	    if ((controls & SC_ScrollBarFirst) && first.isValid())
		drawPrimitive(PE_ScrollBarFirst, p, ceData, elementFlags, first, cg,
			      ((maxedOut) ? Style_Default : Style_Enabled) |
			      ((active == SC_ScrollBarFirst) ?
			       Style_Down : Style_Default) |
			      ((ceData.orientation == TQt::Horizontal) ?
			       Style_Horizontal : 0));
	    if ((controls & SC_ScrollBarLast) && last.isValid())
		drawPrimitive(PE_ScrollBarLast, p, ceData, elementFlags, last, cg,
			      ((maxedOut) ? Style_Default : Style_Enabled) |
			      ((active == SC_ScrollBarLast) ?
			       Style_Down : Style_Default) |
			      ((ceData.orientation == TQt::Horizontal) ?
			       Style_Horizontal : 0));
	    if ((controls & SC_ScrollBarSlider) && slider.isValid()) {
		drawPrimitive(PE_ScrollBarSlider, p, ceData, elementFlags, slider, cg,
			      ((maxedOut) ? Style_Default : Style_Enabled) |
			      ((active == SC_ScrollBarSlider) ?
			       Style_Down : Style_Default) |
			      ((ceData.orientation == TQt::Horizontal) ?
			       Style_Horizontal : 0));

		// ### perhaps this should not be able to accept focus if maxedOut?
		if (elementFlags & CEF_HasFocus) {
		    TQRect fr(slider.x() + 2, slider.y() + 2,
			     slider.width() - 5, slider.height() - 5);
		    drawPrimitive(PE_FocusRect, p, ceData, elementFlags, fr, cg, Style_Default);
		}
	    }

	    break;
	}
#endif // TQT_NO_SCROLLBAR

#ifndef TQT_NO_TOOLBUTTON
    case CC_ToolButton:
	{
	    TQColorGroup c = cg;
	    if ( ceData.backgroundMode != PaletteButton )
		c.setBrush( TQColorGroup::Button,
			    ceData.bgColor );
	    TQRect button, menuarea;
	    button   = visualRect( querySubControlMetrics(control, ceData, elementFlags, SC_ToolButton, opt, widget), ceData, elementFlags );
	    menuarea = visualRect( querySubControlMetrics(control, ceData, elementFlags, SC_ToolButtonMenu, opt, widget), ceData, elementFlags );

	    SFlags bflags = flags,
		   mflags = flags;

	    if (active & SC_ToolButton)
		bflags |= Style_Down;
	    if (active & SC_ToolButtonMenu)
		mflags |= Style_Down;

	    if (controls & SC_ToolButton) {
		if (bflags & (Style_Down | Style_On | Style_Raised)) {
		    drawPrimitive(PE_ButtonTool, p, ceData, elementFlags, button, c, bflags, opt);
		} else if ( (elementFlags & CEF_HasParentWidget) &&
			  ! ceData.parentWidgetData.bgPixmap.isNull() ) {
		    TQPixmap pixmap =
			ceData.parentWidgetData.bgPixmap;

		    p->drawTiledPixmap( r, pixmap, ceData.pos );
		}
	    }

	    if (controls & SC_ToolButtonMenu) {
		if (mflags & (Style_Down | Style_On | Style_Raised))
		    drawPrimitive(PE_ButtonDropDown, p, ceData, elementFlags, menuarea, c, mflags, opt);
		drawPrimitive(PE_ArrowDown, p, ceData, elementFlags, menuarea, c, mflags, opt);
	    }

	    if ((elementFlags & CEF_HasFocus) && !(elementFlags & CEF_HasFocusProxy)) {
		TQRect fr = ceData.rect;
		fr.addCoords(3, 3, -3, -3);
		drawPrimitive(PE_FocusRect, p, ceData, elementFlags, fr, c);
	    }

	    break;
	}
#endif // TQT_NO_TOOLBUTTON

#ifndef TQT_NO_TITLEBAR
    case CC_TitleBar:
	{
	    if ( controls & SC_TitleBarLabel ) {
		TQColorGroup cgroup = (ceData.titleBarData.usesActiveColor) ?
		    ceData.palette.active() : ceData.palette.inactive();

		TQColor left = cgroup.highlight();
		TQColor right = cgroup.base();

		if ( left != right ) {
		    double rS = left.red();
		    double gS = left.green();
		    double bS = left.blue();

		    const double rD = double(right.red() - rS) / ceData.rect.width();
		    const double gD = double(right.green() - gS) / ceData.rect.width();
		    const double bD = double(right.blue() - bS) / ceData.rect.width();

		    const int w = ceData.rect.width();
		    for ( int sx = 0; sx < w; sx++ ) {
			rS+=rD;
			gS+=gD;
			bS+=bD;
			p->setPen( TQColor( (int)rS, (int)gS, (int)bS ) );
			p->drawLine( sx, 0, sx, ceData.rect.height() );
		    }
		} else {
		    p->fillRect( ceData.rect, left );
		}

		TQRect ir = visualRect( querySubControlMetrics( CC_TitleBar, ceData, elementFlags, SC_TitleBarLabel, TQStyleOption::Default, widget ), ceData, elementFlags );

		p->setPen( cgroup.highlightedText() );
		p->drawText(ir.x()+2, ir.y(), ir.width()-2, ir.height(),
			    AlignAuto | AlignVCenter | SingleLine, ceData.titleBarData.visibleText );
	    }

	    TQRect ir;
	    bool down = false;
	    TQPixmap pm;

	    if ( controls & SC_TitleBarCloseButton ) {
		ir = visualRect( querySubControlMetrics( CC_TitleBar, ceData, elementFlags, SC_TitleBarCloseButton, TQStyleOption::Default, widget ), ceData, elementFlags );
		down = active & SC_TitleBarCloseButton;
		if ( widget->testWFlags( WStyle_Tool )
#ifndef TQT_NO_MAINWINDOW
		     || ::tqt_cast<TQDockWindow*>(widget)
#endif
		    )
		    pm = stylePixmap(SP_DockWindowCloseButton, ceData, elementFlags, TQStyleOption::Default, widget);
		else
		    pm = stylePixmap(SP_TitleBarCloseButton, ceData, elementFlags, TQStyleOption::Default, widget);
    		drawPrimitive(PE_ButtonTool, p, ceData, elementFlags, ir, ceData.colorGroup,
			      down ? Style_Down : Style_Raised);

		p->save();
		if( down )
		    p->translate( pixelMetric(PM_ButtonShiftHorizontal, ceData, elementFlags, widget),
				  pixelMetric(PM_ButtonShiftVertical, ceData, elementFlags, widget) );
		drawItem( p, ir, AlignCenter, ceData.colorGroup, true, &pm, TQString::null );
		p->restore();
	    }

	    if ( ceData.titleBarData.hasWindow ) {
		if ( controls & SC_TitleBarMaxButton ) {
		    ir = visualRect( querySubControlMetrics( CC_TitleBar, ceData, elementFlags, SC_TitleBarMaxButton, TQStyleOption::Default, widget ), ceData, elementFlags );

		    down = active & SC_TitleBarMaxButton;
		    pm = TQPixmap(stylePixmap(SP_TitleBarMaxButton, ceData, elementFlags, TQStyleOption::Default, widget));
		    drawPrimitive(PE_ButtonTool, p, ceData, elementFlags, ir, ceData.colorGroup,
				  down ? Style_Down : Style_Raised);

		    p->save();
		    if( down )
			p->translate( pixelMetric(PM_ButtonShiftHorizontal, ceData, elementFlags, widget),
				      pixelMetric(PM_ButtonShiftVertical, ceData, elementFlags, widget) );
    		    drawItem( p, ir, AlignCenter, ceData.colorGroup, true, &pm, TQString::null );
		    p->restore();
		}

		if ( controls & SC_TitleBarNormalButton || controls & SC_TitleBarMinButton ) {
		    ir = visualRect( querySubControlMetrics( CC_TitleBar, ceData, elementFlags, SC_TitleBarMinButton, TQStyleOption::Default, widget ), ceData, elementFlags );
		    TQStyle::SubControl ctrl = (controls & SC_TitleBarNormalButton ?
					       SC_TitleBarNormalButton :
					       SC_TitleBarMinButton);
		    TQStyle::StylePixmap spixmap = (controls & SC_TitleBarNormalButton ?
						   SP_TitleBarNormalButton :
						   SP_TitleBarMinButton);
		    down = active & ctrl;
		    pm = TQPixmap(stylePixmap(spixmap, ceData, elementFlags, TQStyleOption::Default, widget));
		    drawPrimitive(PE_ButtonTool, p, ceData, elementFlags, ir, ceData.colorGroup,
				  down ? Style_Down : Style_Raised);

		    p->save();
		    if( down )
			p->translate( pixelMetric(PM_ButtonShiftHorizontal, ceData, elementFlags, widget),
				      pixelMetric(PM_ButtonShiftVertical, ceData, elementFlags, widget) );
		    drawItem( p, ir, AlignCenter, ceData.colorGroup, true, &pm, TQString::null );
		    p->restore();
		}

		if ( controls & SC_TitleBarShadeButton ) {
		    ir = visualRect( querySubControlMetrics( CC_TitleBar, ceData, elementFlags, SC_TitleBarShadeButton, TQStyleOption::Default, widget ), ceData, elementFlags );

		    down = active & SC_TitleBarShadeButton;
		    pm = TQPixmap(stylePixmap(SP_TitleBarShadeButton, ceData, elementFlags, TQStyleOption::Default, widget));
		    drawPrimitive(PE_ButtonTool, p, ceData, elementFlags, ir, ceData.colorGroup,
				  down ? Style_Down : Style_Raised);
		    p->save();
		    if( down )
			p->translate( pixelMetric(PM_ButtonShiftHorizontal, ceData, elementFlags, widget),
				      pixelMetric(PM_ButtonShiftVertical, ceData, elementFlags, widget) );
		    drawItem( p, ir, AlignCenter, ceData.colorGroup, true, &pm, TQString::null );
		    p->restore();
		}

		if ( controls & SC_TitleBarUnshadeButton ) {
		    ir = visualRect( querySubControlMetrics( CC_TitleBar, ceData, elementFlags, SC_TitleBarUnshadeButton, TQStyleOption::Default, widget ), ceData, elementFlags );

		    down = active & SC_TitleBarUnshadeButton;
		    pm = TQPixmap(stylePixmap(SP_TitleBarUnshadeButton, ceData, elementFlags, TQStyleOption::Default, widget));
		    drawPrimitive(PE_ButtonTool, p, ceData, elementFlags, ir, ceData.colorGroup,
				  down ? Style_Down : Style_Raised);
		    p->save();
		    if( down )
			p->translate( pixelMetric(PM_ButtonShiftHorizontal, ceData, elementFlags, widget),
				      pixelMetric(PM_ButtonShiftVertical, ceData, elementFlags, widget) );
		    drawItem( p, ir, AlignCenter, ceData.colorGroup, true, &pm, TQString::null );
		    p->restore();
		}
	    }
#ifndef TQT_NO_WIDGET_TOPEXTRA
	    if ( controls & SC_TitleBarSysMenu ) {
		if ( !ceData.icon.isNull() ) {
		    ir = visualRect( querySubControlMetrics( CC_TitleBar, ceData, elementFlags, SC_TitleBarSysMenu, TQStyleOption::Default, widget ), ceData, elementFlags );
		    drawItem( p, ir, AlignCenter, ceData.colorGroup, true, (ceData.icon.isNull())?NULL:&ceData.icon, TQString::null );
		}
	    }
#endif
	    break;
	}
#endif //TQT_NO_TITLEBAR

    case CC_SpinWidget: {
#ifndef TQT_NO_SPINWIDGET
	SFlags flags;
	PrimitiveElement pe;

	if ( controls & SC_SpinWidgetFrame )
	    qDrawWinPanel( p, r, cg, true ); //cstyle == Sunken );

	if ( controls & SC_SpinWidgetUp ) {
	    flags = Style_Default | Style_Enabled;
	    if (active == SC_SpinWidgetUp ) {
		flags |= Style_On;
		flags |= Style_Sunken;
	    } else
		flags |= Style_Raised;
	    if ( ceData.spinWidgetData.buttonSymbols == TQSpinWidget::PlusMinus )
		pe = PE_SpinWidgetPlus;
	    else
		pe = PE_SpinWidgetUp;

	    TQRect re = ceData.spinWidgetData.upRect;
	    TQColorGroup ucg = ceData.spinWidgetData.upEnabled ? cg : ceData.palette.disabled();
	    drawPrimitive(PE_ButtonBevel, p, ceData, elementFlags, re, ucg, flags);
	    drawPrimitive(pe, p, ceData, elementFlags, re, ucg, flags);
	}

	if ( controls & SC_SpinWidgetDown ) {
	    flags = Style_Default | Style_Enabled;
	    if (active == SC_SpinWidgetDown ) {
		flags |= Style_On;
		flags |= Style_Sunken;
	    } else
		flags |= Style_Raised;
	    if ( ceData.spinWidgetData.buttonSymbols == TQSpinWidget::PlusMinus )
		pe = PE_SpinWidgetMinus;
	    else
		pe = PE_SpinWidgetDown;

	    TQRect re = ceData.spinWidgetData.downRect;
	    TQColorGroup dcg = ceData.spinWidgetData.downEnabled ? cg : ceData.palette.disabled();
	    drawPrimitive(PE_ButtonBevel, p, ceData, elementFlags, re, dcg, flags);
	    drawPrimitive(pe, p, ceData, elementFlags, re, dcg, flags);
	}
#endif
	break; }

#ifndef TQT_NO_SLIDER
    case CC_Slider:
	switch ( controls ) {
	case SC_SliderTickmarks: {
	    int tickOffset = pixelMetric( PM_SliderTickmarkOffset, ceData, elementFlags, widget );
	    int ticks = ceData.tickMarkSetting;
	    int thickness = pixelMetric( PM_SliderControlThickness, ceData, elementFlags, widget );
	    int len = pixelMetric( PM_SliderLength, ceData, elementFlags, widget );
	    int available = pixelMetric( PM_SliderSpaceAvailable, ceData, elementFlags, widget );
	    int interval = ceData.tickInterval;

	    if ( interval <= 0 ) {
		interval = ceData.lineStep;
		if ( qPositionFromValue( ceData, interval, available ) -
		     qPositionFromValue( ceData, 0, available ) < 3 )
		    interval = ceData.pageStep;
	    }

	    int fudge = len / 2;
	    int pos;

	    if ( ticks & TQSlider::Above ) {
		p->setPen( cg.foreground() );
		int v = ceData.minSteps;
		if ( !interval )
		    interval = 1;
		while ( v <= ceData.maxSteps + 1 ) {
		    pos = qPositionFromValue( ceData, v, available ) + fudge;
		    if ( ceData.orientation == Horizontal )
			p->drawLine( pos, 0, pos, tickOffset-2 );
		    else
			p->drawLine( 0, pos, tickOffset-2, pos );
		    v += interval;
		}
	    }

	    if ( ticks & TQSlider::Below ) {
		p->setPen( cg.foreground() );
		int v = ceData.minSteps;
		if ( !interval )
		    interval = 1;
		while ( v <= ceData.maxSteps + 1 ) {
		    pos = qPositionFromValue( ceData, v, available ) + fudge;
		    if ( ceData.orientation == Horizontal )
			p->drawLine( pos, tickOffset+thickness+1, pos,
				     tickOffset+thickness+1 + available-2 );
		    else
			p->drawLine( tickOffset+thickness+1, pos,
				     tickOffset+thickness+1 + available-2,
				     pos );
		    v += interval;
		}

	    }

	    break; }
	}
	break;
#endif // TQT_NO_SLIDER
#ifndef TQT_NO_LISTVIEW
    case CC_ListView:
	if ( controls & SC_ListView ) {
	    p->fillRect( r, ceData.viewportData.bgBrush );
	}
	break;
#endif //TQT_NO_LISTVIEW
    default:
	break;
    }

    activePainter = 0;
}

/*! \reimp */
void TQCommonStyle::drawComplexControlMask( ComplexControl control,
					   TQPainter *p,
					   const TQStyleControlElementData &ceData,
					   const ControlElementFlags elementFlags,
					   const TQRect &r,
					   const TQStyleOption& opt,
					   const TQWidget *widget ) const
{
    Q_UNUSED(control);
    Q_UNUSED(ceData);
    Q_UNUSED(elementFlags);
    Q_UNUSED(widget);
    Q_UNUSED(opt);

    p->fillRect(r, color1);
}

/*! \reimp */
TQRect TQCommonStyle::querySubControlMetrics( ComplexControl control,
					    const TQStyleControlElementData &ceData,
					    ControlElementFlags elementFlags,
					    SubControl sc,
					    const TQStyleOption &opt,
					    const TQWidget *widget ) const
{
    switch ( control ) {
    case CC_SpinWidget: {
	int fw = pixelMetric( PM_SpinBoxFrameWidth, ceData, elementFlags, widget);
	TQSize bs;
	bs.setHeight( ceData.rect.height()/2 - fw );
	if ( bs.height() < 8 )
	    bs.setHeight( 8 );
	bs.setWidth( TQMIN( bs.height() * 8 / 5, ceData.rect.width() / 4 ) ); // 1.6 -approximate golden mean
	bs = bs.expandedTo( TQApplication::globalStrut() );
	int y = fw;
	int x, lx, rx;
	x = ceData.rect.width() - y - bs.width();
	lx = fw;
	rx = x - fw;
	switch ( sc ) {
	case SC_SpinWidgetUp:
	    return TQRect(x, y, bs.width(), bs.height());
	case SC_SpinWidgetDown:
	    return TQRect(x, y + bs.height(), bs.width(), bs.height());
	case SC_SpinWidgetButtonField:
	    return TQRect(x, y, bs.width(), ceData.rect.height() - 2*fw);
	case SC_SpinWidgetEditField:
	    return TQRect(lx, fw, rx, ceData.rect.height() - 2*fw);
	case SC_SpinWidgetFrame:
	    return ceData.rect;
	default:
	    break;
	}
	break; }

    case CC_ComboBox: {
	int x = 0, y = 0, wi = ceData.rect.width(), he = ceData.rect.height();
	int xpos = x;
	xpos += wi - 2 - 16;

	switch ( sc ) {
	case SC_ComboBoxFrame:
	    return ceData.rect;
	case SC_ComboBoxArrow:
	    return TQRect(xpos, y+2, 16, he-4);
	case SC_ComboBoxEditField:
	    return TQRect(x+3, y+3, wi-6-16, he-6);
	case SC_ComboBoxListBoxPopup:
	    return opt.rect();
	default:
	    break;
	}
	break; }

#ifndef TQT_NO_SCROLLBAR
    case CC_ScrollBar: {
	int sliderstart = 0;
	int sbextent = pixelMetric(PM_ScrollBarExtent, ceData, elementFlags, widget);
	int maxlen = ((ceData.orientation == TQt::Horizontal) ?
		      ceData.rect.width() : ceData.rect.height()) - (sbextent * 2);
	int sliderlen;

	sliderstart = ceData.startStep;

	// calculate slider length
	if (ceData.maxSteps != ceData.minSteps) {
	    uint range = ceData.maxSteps - ceData.minSteps;
	    sliderlen = (TQ_LLONG(ceData.pageStep) * maxlen) / (range + ceData.pageStep);

	    int slidermin = pixelMetric( PM_ScrollBarSliderMin, ceData, elementFlags, widget );
	    if ( sliderlen < slidermin || range > INT_MAX / 2 )
		sliderlen = slidermin;
	    if ( sliderlen > maxlen )
		sliderlen = maxlen;
	} else
	    sliderlen = maxlen;

	switch (sc) {
	case SC_ScrollBarSubLine:	    // top/left button
	    if (ceData.orientation == TQt::Horizontal) {
		int buttonWidth = TQMIN(ceData.rect.width()/2, sbextent);
		return TQRect( 0, 0, buttonWidth, sbextent );
	    } else {
		int buttonHeight = TQMIN(ceData.rect.height()/2, sbextent);
		return TQRect( 0, 0, sbextent, buttonHeight );
	    }

	case SC_ScrollBarAddLine:	    // bottom/right button
	    if (ceData.orientation == TQt::Horizontal) {
		int buttonWidth = TQMIN(ceData.rect.width()/2, sbextent);
		return TQRect( ceData.rect.width() - buttonWidth, 0, buttonWidth, sbextent );
	    } else {
		int buttonHeight = TQMIN(ceData.rect.height()/2, sbextent);
		return TQRect( 0, ceData.rect.height() - buttonHeight, sbextent, buttonHeight );
	    }

	case SC_ScrollBarSubPage:	    // between top/left button and slider
	    if (ceData.orientation == TQt::Horizontal)
		return TQRect(sbextent, 0, sliderstart - sbextent, sbextent);
	    return TQRect(0, sbextent, sbextent, sliderstart - sbextent);

	case SC_ScrollBarAddPage:	    // between bottom/right button and slider
	    if (ceData.orientation == TQt::Horizontal)
		return TQRect(sliderstart + sliderlen, 0,
			     maxlen - sliderstart - sliderlen + sbextent, sbextent);
	    return TQRect(0, sliderstart + sliderlen,
			 sbextent, maxlen - sliderstart - sliderlen + sbextent);

	case SC_ScrollBarGroove:
	    if (ceData.orientation == TQt::Horizontal)
		return TQRect(sbextent, 0, ceData.rect.width() - sbextent * 2,
			     ceData.rect.height());
	    return TQRect(0, sbextent, ceData.rect.width(),
			 ceData.rect.height() - sbextent * 2);

	case SC_ScrollBarSlider:
	    if (ceData.orientation == TQt::Horizontal)
		return TQRect(sliderstart, 0, sliderlen, sbextent);
	    return TQRect(0, sliderstart, sbextent, sliderlen);

	default: break;
	}

	break; }
#endif // TQT_NO_SCROLLBAR

#ifndef TQT_NO_SLIDER
    case CC_Slider: {
	    int tickOffset = pixelMetric( PM_SliderTickmarkOffset, ceData, elementFlags, widget );
	    int thickness = pixelMetric( PM_SliderControlThickness, ceData, elementFlags, widget );

	    switch ( sc ) {
	    case SC_SliderHandle: {
		    int sliderPos = 0;
		    int len   = pixelMetric( PM_SliderLength, ceData, elementFlags, widget );

		    sliderPos = ceData.startStep;

		    if ( ceData.orientation == Horizontal )
			return TQRect( sliderPos, tickOffset, len, thickness );
		    return TQRect( tickOffset, sliderPos, thickness, len ); }
	    case SC_SliderGroove: {
		if ( ceData.orientation == Horizontal )
		    return TQRect( 0, tickOffset, ceData.rect.width(), thickness );
		return TQRect( tickOffset, 0, thickness, ceData.rect.height() );	}

	    default:
		break;
	    }
	    break; }
#endif // TQT_NO_SLIDER

#if !defined(TQT_NO_TOOLBUTTON) && !defined(TQT_NO_POPUPMENU)
    case CC_ToolButton: {
	    int mbi = pixelMetric(PM_MenuButtonIndicator, ceData, elementFlags, widget);

	    TQRect rect = ceData.rect;
	    switch (sc) {
	    case SC_ToolButton:
		if ((elementFlags & CEF_HasPopupMenu) && ! ceData.popupDelay)
		    rect.addCoords(0, 0, -mbi, 0);
		return rect;

	    case SC_ToolButtonMenu:
		if ((elementFlags & CEF_HasPopupMenu) && ! ceData.popupDelay)
		    rect.addCoords(rect.width() - mbi, 0, 0, 0);
		return rect;

	    default: break;
	    }
	    break;
	}
#endif // TQT_NO_TOOLBUTTON && TQT_NO_POPUPMENU

#ifndef TQT_NO_TITLEBAR
    case CC_TitleBar: {
	    const int controlTop = 2;
	    const int controlHeight = ceData.rect.height() - controlTop * 2;

	    switch (sc) {
	    case SC_TitleBarLabel: {
		TQRect ir( 0, 0, ceData.rect.width(), ceData.rect.height() );
		if ( ceData.wflags & WStyle_Tool ) {
		    if ( ceData.wflags & WStyle_SysMenu )
			ir.addCoords( 0, 0, -controlHeight-3, 0 );
		    if ( ceData.wflags & WStyle_MinMax )
			ir.addCoords( 0, 0, -controlHeight-2, 0 );
		} else {
		    if ( ceData.wflags & WStyle_SysMenu )
			ir.addCoords( controlHeight+3, 0, -controlHeight-3, 0 );
		    if ( ceData.wflags & WStyle_Minimize )
			ir.addCoords( 0, 0, -controlHeight-2, 0 );
		    if ( ceData.wflags & WStyle_Maximize )
			ir.addCoords( 0, 0, -controlHeight-2, 0 );
		}
		return ir; }

	    case SC_TitleBarCloseButton:
		return TQRect( ceData.rect.width() - ( controlHeight + controlTop ),
			      controlTop, controlHeight, controlHeight );

	    case SC_TitleBarMaxButton:
	    case SC_TitleBarShadeButton:
	    case SC_TitleBarUnshadeButton:
		return TQRect( ceData.rect.width() - ( ( controlHeight + controlTop ) * 2 ),
			      controlTop, controlHeight, controlHeight );

	    case SC_TitleBarMinButton:
	    case SC_TitleBarNormalButton: {
		int offset = controlHeight + controlTop;
		if ( !( ceData.wflags & WStyle_Maximize ) )
		    offset *= 2;
		else
		    offset *= 3;
		return TQRect( ceData.rect.width() - offset, controlTop, controlHeight, controlHeight );
	    }

	    case SC_TitleBarSysMenu:
		return TQRect( 3, controlTop, controlHeight, controlHeight);

	    default: break;
	    }
	    break; }
#endif //TQT_NO_TITLEBAR

    default:
	break;
    }
    return TQRect();
}

/*! \reimp */
TQStyle::SubControl TQCommonStyle::querySubControl(ComplexControl control,
						 const TQStyleControlElementData &ceData,
						 ControlElementFlags elementFlags,
						 const TQPoint &pos,
						 const TQStyleOption& opt,
						 const TQWidget *widget ) const
{
    SubControl ret = SC_None;

    switch (control) {
#ifndef TQT_NO_LISTVIEW
    case CC_ListView:
	{
	    if(pos.x() >= 0 && pos.x() <
	       opt.listViewItem()->listView()->treeStepSize())
		ret = SC_ListViewExpand;
	    break;
	}
#endif
#ifndef TQT_NO_SCROLLBAR
    case CC_ScrollBar:
	{
	    TQRect r;
	    uint ctrl = SC_ScrollBarAddLine;

	    // we can do this because subcontrols were designed to be masks as well...
	    while (ret == SC_None && ctrl <= SC_ScrollBarGroove) {
		r = querySubControlMetrics(control, ceData, elementFlags,
					   (TQStyle::SubControl) ctrl, opt, widget);
		if (r.isValid() && r.contains(pos))
		    ret = (TQStyle::SubControl) ctrl;

		ctrl <<= 1;
	    }

	    break;
	}
#endif
    case CC_TitleBar:
	{
#ifndef TQT_NO_TITLEBAR
	    TQRect r;
	    uint ctrl = SC_TitleBarLabel;

	    // we can do this because subcontrols were designed to be masks as well...
	    while (ret == SC_None && ctrl <= SC_TitleBarUnshadeButton) {
		r = visualRect( querySubControlMetrics( control, ceData, elementFlags, (TQStyle::SubControl) ctrl, opt, widget ), ceData, elementFlags );
		if (r.isValid() && r.contains(pos))
		    ret = (TQStyle::SubControl) ctrl;

		ctrl <<= 1;
	    }
	    if ( ceData.titleBarData.hasWindow ) {
		if ( ceData.wflags & WStyle_Tool ) {
		    if ( ret == SC_TitleBarMinButton || ret == SC_TitleBarMaxButton ) {
			if ( ceData.titleBarData.windowState & WindowMinimized )
			    ret = SC_TitleBarUnshadeButton;
			else
			    ret = SC_TitleBarShadeButton;
		    }
		} else if ( ret == SC_TitleBarMinButton && (ceData.titleBarData.windowState & WindowMinimized) ) {
			ret = TQStyle::SC_TitleBarNormalButton;
		}
	    }
#endif
	    break;
	}

    default:
	break;
    }

    return ret;
}

/*! \reimp */
int TQCommonStyle::pixelMetric(PixelMetric m, const TQStyleControlElementData &ceData, ControlElementFlags elementFlags, const TQWidget *widget) const
{
    int ret;

    switch (m) {
    case PM_DialogButtonsSeparator:
	ret = 5;
	break;
    case PM_DialogButtonsButtonWidth:
	ret = 70;
	break;
    case PM_DialogButtonsButtonHeight:
	ret = 30;
	break;
    case PM_CheckListControllerSize:
    case PM_CheckListButtonSize:
	ret = 16;
	break;
    case PM_TitleBarHeight: {
        if ( !(elementFlags & CEF_UseGenericParameters) ) {
	    if ( ceData.wflags & WStyle_Tool ) {
		ret = TQMAX( TQFontMetrics(ceData.font).lineSpacing(), 16 );
#ifndef TQT_NO_MAINWINDOW
	    } else if ( ceData.widgetObjectTypes.containsYesNo(TQDockWindow_static_string) ) {
		ret = TQMAX( TQFontMetrics(ceData.font).lineSpacing(), 13 );
#endif
	    } else {
		ret = TQMAX( TQFontMetrics(ceData.font).lineSpacing(), 18 );
	    }
	}
	else {
		ret = 0;
	}
	break; }
    case PM_ScrollBarSliderMin:
	ret = 9;
	break;

    case PM_ButtonMargin:
	ret = 6;
	break;

    case PM_ButtonDefaultIndicator:
	ret = 0;
	break;

    case PM_MenuButtonIndicator:
	if ( elementFlags & CEF_UseGenericParameters )
	    ret = 12;
	else
	    ret = TQMAX(12, (ceData.rect.height() - 4) / 3);
	break;

    case PM_ButtonShiftHorizontal:
    case PM_ButtonShiftVertical:
	ret = 0;
	break;

    case PM_SpinBoxFrameWidth:
    case PM_DefaultFrameWidth:
	ret = 2;
	break;

    case PM_MDIFrameWidth:
	ret = 2;
	break;

    case PM_MDIMinimizedWidth:
	ret = 196;
	break;

#ifndef TQT_NO_SCROLLBAR
    case PM_ScrollBarExtent:
	if ( elementFlags & CEF_UseGenericParameters ) {
	    ret = 16;
	} else {
	    int s = ceData.orientation == TQt::Horizontal ?
		    TQApplication::globalStrut().height()
		    : TQApplication::globalStrut().width();
	    ret = TQMAX( 16, s );
	}
	break;
#endif
    case PM_MaximumDragDistance:
	ret = -1;
	break;

#ifndef TQT_NO_SLIDER
    case PM_SliderThickness:
	ret = 16;
	break;

    case PM_SliderTickmarkOffset:
	{
	    if ( elementFlags & CEF_UseGenericParameters ) {
		ret = 0;
		break;
	    }

	    int space = (ceData.orientation == Horizontal) ? ceData.rect.height() :
			ceData.rect.width();
	    int thickness = pixelMetric( PM_SliderControlThickness, ceData, elementFlags, widget );
	    int ticks = ceData.tickMarkSetting;

	    if ( ticks == TQSlider::Both ) {
		ret = (space - thickness) / 2;
	    }
	    else if ( ticks == TQSlider::Above ) {
		ret = space - thickness;
	    }
	    else {
		ret = 0;
	    }
	    break;
	}

    case PM_SliderSpaceAvailable:
	{
	    if ( ceData.orientation == Horizontal )
		ret = ceData.rect.width() - pixelMetric( PM_SliderLength, ceData, elementFlags, widget );
	    else
		ret = ceData.rect.height() - pixelMetric( PM_SliderLength, ceData, elementFlags, widget );
	    break;
	}
#endif // TQT_NO_SLIDER

    case PM_DockWindowSeparatorExtent:
	ret = 6;
	break;

    case PM_DockWindowHandleExtent:
	ret = 8;
	break;

    case PM_DockWindowFrameWidth:
	ret = 1;
	break;

    case PM_MenuBarFrameWidth:
	ret = 2;
	break;

    case PM_MenuBarItemSpacing:
    case PM_ToolBarItemSpacing:
	ret = 0;
	break;

    case PM_TabBarTabOverlap:
	ret = 3;
	break;

    case PM_TabBarBaseHeight:
	ret = 0;
	break;

    case PM_TabBarBaseOverlap:
	ret = 0;
	break;

    case PM_TabBarTabHSpace:
	ret = 24;
	break;

    case PM_TabBarTabShiftHorizontal:
    case PM_TabBarTabShiftVertical:
	ret = 2;
	break;

#ifndef TQT_NO_TABBAR
    case PM_TabBarTabVSpace:
	{
	    if ( ceData.widgetObjectTypes.containsYesNo(TQTabBar_static_string) && ( ceData.tabBarData.shape == TQTabBar::RoundedAbove ||
			 ceData.tabBarData.shape == TQTabBar::RoundedBelow ) )
		ret = 10;
	    else
		ret = 0;
	    break;
	}
#endif

    case PM_ProgressBarChunkWidth:
	ret = 9;
	break;

    case PM_IndicatorWidth:
	ret = 13;
	break;

    case PM_IndicatorHeight:
	ret = 13;
	break;

    case PM_ExclusiveIndicatorWidth:
	ret = 12;
	break;

    case PM_ExclusiveIndicatorHeight:
	ret = 12;
	break;

    case PM_PopupMenuFrameHorizontalExtra:
    case PM_PopupMenuFrameVerticalExtra:
	ret = 0;
	break;

    case PM_HeaderMargin:
	ret = 4;
	break;
    case PM_HeaderMarkSize:
	ret = 32;
	break;
    case PM_HeaderGripMargin:
	ret = 4;
	break;
    case PM_TabBarScrollButtonWidth:
	ret = 16;
	break;
    case PM_ArrowSize:
	ret = 7;
	break;
    default:
	ret = 0;
	break;
    }

    return ret;
}

/*! \reimp */
TQSize TQCommonStyle::sizeFromContents(ContentsType contents,
				     const TQStyleControlElementData &ceData,
				     ControlElementFlags elementFlags,
				     const TQSize &contentsSize,
				     const TQStyleOption& opt,
				     const TQWidget *widget ) const
{
    TQSize sz(contentsSize);

    switch (contents) {
#ifndef TQT_NO_DIALOGBUTTONS
    case CT_DialogButtons: {
	int w = contentsSize.width(), h = contentsSize.height();
	const int bwidth = pixelMetric(PM_DialogButtonsButtonWidth, ceData, elementFlags, widget),
		  bspace = pixelMetric(PM_DialogButtonsSeparator, ceData, elementFlags, widget),
		 bheight = pixelMetric(PM_DialogButtonsButtonHeight, ceData, elementFlags, widget);
	if(ceData.orientation == Horizontal) {
	    if(!w)
		w = bwidth;
	} else {
	    if(!h)
		h = bheight;
	}
	TQDialogButtons::Button btns[] = { TQDialogButtons::All, TQDialogButtons::Reject, TQDialogButtons::Accept, //reverse order (right to left)
					  TQDialogButtons::Apply, TQDialogButtons::Retry, TQDialogButtons::Ignore, TQDialogButtons::Abort,
					  TQDialogButtons::Help };
	for(unsigned int i = 0, cnt = 0; i < (sizeof(btns)/sizeof(btns[0])); i++) {
	    if (ceData.dlgVisibleButtons & btns[i]) {
		TQSize szH = ceData.dlgVisibleSizeHints[btns[i]];
		int mwidth = TQMAX(bwidth, szH.width()), mheight = TQMAX(bheight, szH.height());
		if(ceData.orientation == Horizontal)
		    h = TQMAX(h, mheight);
		else
		    w = TQMAX(w, mwidth);

		if(cnt)
		    w += bspace;
		cnt++;
		if(ceData.orientation == Horizontal)
		    w += mwidth;
		else
		    h += mheight;
	    }
	}
	const int fw = pixelMetric(PM_DefaultFrameWidth, ceData, elementFlags, widget) * 2;
	sz = TQSize(w + fw, h + fw);
	break; }
#endif //TQT_NO_DIALOGBUTTONS
    case CT_PushButton:
	{
#ifndef TQT_NO_PUSHBUTTON
	    int w = contentsSize.width(),
		h = contentsSize.height(),
	       bm = pixelMetric(PM_ButtonMargin, ceData, elementFlags, widget),
	       fw = pixelMetric(PM_DefaultFrameWidth, ceData, elementFlags, widget) * 2;

	    w += bm + fw;
	    h += bm + fw;

	    if ((elementFlags & CEF_IsDefault) || (elementFlags & CEF_AutoDefault)) {
		int dbw = pixelMetric(PM_ButtonDefaultIndicator, ceData, elementFlags, widget) * 2;
		w += dbw;
		h += dbw;
	    }

	    sz = TQSize(w, h);
#endif
	    break;
	}

    case CT_CheckBox:
	{
#ifndef TQT_NO_CHECKBOX
	    TQRect irect = subRect(SR_CheckBoxIndicator, ceData, elementFlags, widget);
	    int h = pixelMetric( PM_IndicatorHeight, ceData, elementFlags, widget );
	    int margins = (ceData.fgPixmap.isNull() && ceData.text.isEmpty()) ? 0 : 10;
	    sz += TQSize(irect.right() + margins, 4 );
	    sz.setHeight( TQMAX( sz.height(), h ) );
#endif
	    break;
	}

    case CT_RadioButton:
	{
#ifndef TQT_NO_RADIOBUTTON
	    TQRect irect = subRect(SR_RadioButtonIndicator, ceData, elementFlags, widget);
	    int h = pixelMetric( PM_ExclusiveIndicatorHeight, ceData, elementFlags, widget );
	    int margins = (ceData.fgPixmap.isNull() && ceData.text.isEmpty()) ? 0 : 10;
	    sz += TQSize(irect.right() + margins, 4 );
	    sz.setHeight( TQMAX( sz.height(), h ) );
#endif
	    break;
	}

    case CT_ToolButton:
	{
	    sz = TQSize(sz.width() + 6, sz.height() + 5);
	    break;
	}

    case CT_ComboBox:
	{
	    int dfw = pixelMetric(PM_DefaultFrameWidth, ceData, elementFlags, widget) * 2;
	    sz = TQSize(sz.width() + dfw + 21, sz.height() + dfw );
	    break;
	}

    case CT_PopupMenuItem:
	{
#ifndef TQT_NO_POPUPMENU
	    if (opt.isDefault())
		break;

	    bool checkable = (elementFlags & CEF_IsCheckable);
	    TQMenuItem *mi = opt.menuItem();
	    int maxpmw = opt.maxIconWidth();
	    int w = sz.width(), h = sz.height();

	    if (mi->custom()) {
		w = mi->custom()->sizeHint().width();
		h = mi->custom()->sizeHint().height();
		if (! mi->custom()->fullSpan())
		    h += 8;
	    } else if ( mi->widget() ) {
	    } else if (mi->isSeparator()) {
		w = 10;
		h = 2;
	    } else {
		if (mi->pixmap())
		    h = TQMAX(h, mi->pixmap()->height() + 4);
		else
		    h = TQMAX(h, TQFontMetrics(ceData.font).height() + 8);

		if (mi->iconSet() != 0)
		    h = TQMAX(h, mi->iconSet()->pixmap(TQIconSet::Small,
						      TQIconSet::Normal).height() + 4);
	    }

	    if (! mi->text().isNull()) {
		if (mi->text().find('\t') >= 0)
		    w += 12;
	    }

	    if (maxpmw)
		w += maxpmw + 6;
	    if (checkable && maxpmw < 20)
		w += 20 - maxpmw;
	    if (checkable || maxpmw > 0)
		w += 2;
	    w += 12;

	    sz = TQSize(w, h);
#endif
	    break;
	}

    case CT_LineEdit:
    case CT_Header:
    case CT_Slider:
    case CT_ProgressBar:
	// just return the contentsSize for now
	// fall through intended

    default:
	break;
    }

    return sz;
}

/*! \reimp */
int TQCommonStyle::styleHint(StyleHint sh, const TQStyleControlElementData &ceData, ControlElementFlags elementFlags, const TQStyleOption &, TQStyleHintReturn *, const TQWidget * w) const
{
    Q_UNUSED(w);

    int ret;

    switch (sh) {
#ifndef TQT_NO_DIALOGBUTTONS
    case SH_DialogButtons_DefaultButton:
	ret = TQDialogButtons::Accept;
	break;
#endif
    case SH_GroupBox_TextLabelVerticalAlignment:
	ret = TQt::AlignVCenter;
	break;

    case SH_GroupBox_TextLabelColor:
	ret = (int) ( (!(elementFlags & CEF_UseGenericParameters)) ? ceData.fgColor.rgb() : 0 );
	break;

    case SH_ListViewExpand_SelectMouseType:
    case SH_TabBar_SelectMouseType:
	ret = TQEvent::MouseButtonPress;
	break;

    case SH_GUIStyle:
	ret = WindowsStyle;
	break;

    case SH_ScrollBar_BackgroundMode:
	ret = TQWidget::PaletteBackground;
	break;

    case SH_TabBar_Alignment:
    case SH_Header_ArrowAlignment:
	ret = TQt::AlignLeft;
	break;

    case SH_PopupMenu_SubMenuPopupDelay:
	ret = 256;
	break;

    case SH_ProgressDialog_TextLabelAlignment:
	ret = TQt::AlignCenter;
	break;

    case SH_BlinkCursorWhenTextSelected:
	ret = 1;
	break;

    case SH_Table_GridLineColor:
	ret = -1;
	break;

    case SH_LineEdit_PasswordCharacter:
	ret = '*';
	break;

    case SH_ToolBox_SelectedPageTitleBold:
	ret = 1;
	break;

    case SH_UnderlineAccelerator:
	ret = 1;
	break;

    case SH_ToolButton_Uses3D:
	ret = 1;
	break;

    case SH_HideUnderlineAcceleratorWhenAltUp:
	ret = 0;
	break;

    case SH_PopupMenu_SubMenuArrowColorActiveEnabled:
    case SH_PopupMenu_SubMenuArrowColorActiveDisabled:
    case SH_PopupMenu_SubMenuArrowColorInactiveEnabled:
    case SH_PopupMenu_SubMenuArrowColorInactiveDisabled:
	ret = -1;
	break;

    default:
	ret = 0;
	break;
    }

    return ret;
}

/*! \reimp */
TQPixmap TQCommonStyle::stylePixmap(StylePixmap, const TQStyleControlElementData&, ControlElementFlags, const TQStyleOption&, const TQWidget *) const
{
    return TQPixmap();
}

#endif // TQT_NO_STYLE
