/**********************************************************************
** Copyright (C) 2005-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of TQt Designer.
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
** Licensees holding valid TQt Commercial licenses may use this file in
** accordance with the TQt Commercial License Agreement provided with
** the Software.
**
** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE. Trolltech reserves all rights not granted
** herein.
**
**********************************************************************/

#include "paletteeditorimpl.h"
#include "paletteeditoradvancedimpl.h"
#include "previewframe.h"
#include "styledbutton.h"
#include "mainwindow.h"
#include "formwindow.h"

#include <ntqcheckbox.h>
#include <ntqcombobox.h>
#include <ntqapplication.h>
#include <ntqpushbutton.h>

PaletteEditor::PaletteEditor( FormWindow *fw, TQWidget * parent, const char * name, bool modal, WFlags f )
    : PaletteEditorBase( parent, name, modal, f ), formWindow( fw )
{
    connect( buttonHelp, TQ_SIGNAL( clicked() ), MainWindow::self, TQ_SLOT( showDialogHelp() ) );

    editPalette =  TQApplication::palette();
    setPreviewPalette( editPalette );

    buttonMainColor->setColor( editPalette.active().color( TQColorGroup::Button ) );
    buttonMainColor2->setColor( editPalette.active().color( TQColorGroup::Background ) );
}

PaletteEditor::~PaletteEditor()
{
}

void PaletteEditor::onTune()
{
    bool ok;
    TQPalette pal = PaletteEditorAdvanced::getPalette( &ok, editPalette, backgroundMode, this, "tune_palette", formWindow);
    if (!ok) return;

    editPalette = pal;
    setPreviewPalette( editPalette );
}

void PaletteEditor::onChooseMainColor()
{
    buildPalette();
}

void PaletteEditor::onChoose2ndMainColor()
{
    buildPalette();
}

void PaletteEditor::paletteSelected(int)
{
    setPreviewPalette(editPalette);
}

TQColorGroup::ColorRole PaletteEditor::centralFromItem( int item )
{
    switch( item )
	{
	case 0:
	    return TQColorGroup::Background;
	case 1:
	    return TQColorGroup::Foreground;
	case 2:
	    return TQColorGroup::Button;
	case 3:
	    return TQColorGroup::Base;
	case 4:
	    return TQColorGroup::Text;
	case 5:
	    return TQColorGroup::BrightText;
	case 6:
	    return TQColorGroup::ButtonText;
	case 7:
	    return TQColorGroup::Highlight;
	case 8:
	    return TQColorGroup::HighlightedText;
	default:
	    return TQColorGroup::NColorRoles;
	}
}

TQColorGroup::ColorRole PaletteEditor::effectFromItem( int item )
{
    switch( item )
	{
	case 0:
	    return TQColorGroup::Light;
	case 1:
	    return TQColorGroup::Midlight;
	case 2:
	    return TQColorGroup::Mid;
	case 3:
	    return TQColorGroup::Dark;
	case 4:
	    return TQColorGroup::Shadow;
	default:
	    return TQColorGroup::NColorRoles;
	}
}

void PaletteEditor::buildPalette()
{
    int i;
    TQColorGroup cg;
    TQColor btn = buttonMainColor->color();
    TQColor back = buttonMainColor2->color();
    TQPalette automake( btn, back );

    for (i = 0; i<9; i++)
	cg.setColor( centralFromItem(i), automake.active().color( centralFromItem(i) ) );

    editPalette.setActive( cg );
    buildActiveEffect();

    cg = editPalette.inactive();

    TQPalette temp( editPalette.active().color( TQColorGroup::Button ),
		   editPalette.active().color( TQColorGroup::Background ) );

    for (i = 0; i<9; i++)
	cg.setColor( centralFromItem(i), temp.inactive().color( centralFromItem(i) ) );

    editPalette.setInactive( cg );
    buildInactiveEffect();

    cg = editPalette.disabled();

    for (i = 0; i<9; i++)
	cg.setColor( centralFromItem(i), temp.disabled().color( centralFromItem(i) ) );

    editPalette.setDisabled( cg );
    buildDisabledEffect();

    updateStyledButtons();
}

void PaletteEditor::buildActiveEffect()
{
    TQColorGroup cg = editPalette.active();
    TQColor btn = cg.color( TQColorGroup::Button );

    TQPalette temp( btn, btn );

    for (int i = 0; i<5; i++)
	cg.setColor( effectFromItem(i), temp.active().color( effectFromItem(i) ) );

    editPalette.setActive( cg );
    setPreviewPalette( editPalette );

    updateStyledButtons();
}

void PaletteEditor::buildInactive()
{
    editPalette.setInactive( editPalette.active() );
    buildInactiveEffect();
}

void PaletteEditor::buildInactiveEffect()
{
    TQColorGroup cg = editPalette.inactive();

    TQColor light, midlight, mid, dark, shadow;
    TQColor btn = cg.color( TQColorGroup::Button );

    light = btn.light(150);
    midlight = btn.light(115);
    mid = btn.dark(150);
    dark = btn.dark();
    shadow = black;

    cg.setColor( TQColorGroup::Light, light );
    cg.setColor( TQColorGroup::Midlight, midlight );
    cg.setColor( TQColorGroup::Mid, mid );
    cg.setColor( TQColorGroup::Dark, dark );
    cg.setColor( TQColorGroup::Shadow, shadow );

    editPalette.setInactive( cg );
    setPreviewPalette( editPalette );
    updateStyledButtons();
}

void PaletteEditor::buildDisabled()
{
    TQColorGroup cg = editPalette.active();
    cg.setColor( TQColorGroup::ButtonText, darkGray );
    cg.setColor( TQColorGroup::Foreground, darkGray );
    editPalette.setDisabled( cg );

    buildDisabledEffect();
}

void PaletteEditor::buildDisabledEffect()
{
    TQColorGroup cg = editPalette.disabled();

    TQColor light, midlight, mid, dark, shadow;
    TQColor btn = cg.color( TQColorGroup::Button );

    light = btn.light(150);
    midlight = btn.light(115);
    mid = btn.dark(150);
    dark = btn.dark();
    shadow = black;

    cg.setColor( TQColorGroup::Light, light );
    cg.setColor( TQColorGroup::Midlight, midlight );
    cg.setColor( TQColorGroup::Mid, mid );
    cg.setColor( TQColorGroup::Dark, dark );
    cg.setColor( TQColorGroup::Shadow, shadow );

    editPalette.setDisabled( cg );
    setPreviewPalette( editPalette );
    updateStyledButtons();
}

void PaletteEditor::setPreviewPalette( const TQPalette& pal )
{
    TQColorGroup cg;

    switch (paletteCombo->currentItem()) {
    case 0:
    default:
	cg = pal.active();
	break;
    case 1:
	cg = pal.inactive();
	break;
    case 2:
	cg = pal.disabled();
	break;
    }
    previewPalette.setActive( cg );
    previewPalette.setInactive( cg );
    previewPalette.setDisabled( cg );

    previewFrame->setPreviewPalette(previewPalette);
}

void PaletteEditor::updateStyledButtons()
{
    buttonMainColor->setColor( editPalette.active().color( TQColorGroup::Button ));
    buttonMainColor2->setColor( editPalette.active().color( TQColorGroup::Background ));
}

void PaletteEditor::setPal( const TQPalette& pal )
{
    editPalette = pal;
    setPreviewPalette( pal );
    updateStyledButtons();
}

TQPalette PaletteEditor::pal() const
{
    return editPalette;
}

TQPalette PaletteEditor::getPalette( bool *ok, const TQPalette &init, BackgroundMode mode,
				    TQWidget* parent, const char* name, FormWindow *fw )
{
    PaletteEditor* dlg = new PaletteEditor( fw, parent, name, true );
    dlg->setupBackgroundMode( mode );

    if ( init != TQPalette() )
        dlg->setPal( init );
    int resultCode = dlg->exec();

    TQPalette result = init;
    if ( resultCode == TQDialog::Accepted ) {
	if ( ok )
	    *ok = true;
	result = dlg->pal();
    } else {
	if ( ok )
	    *ok = false;
    }
    delete dlg;
    return result;
}
