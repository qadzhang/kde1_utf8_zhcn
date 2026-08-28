void OptionsDialog::apply()
{
    TQStringList flagList;
    bool wstyle = false;
    WFlags f = WDestructiveClose | WType_TopLevel | WStyle_Customize;

    if ( bgBorder->isChecked() ) {
	if ( rbBorderNormal->isChecked() ) {
	    f |= WStyle_NormalBorder;
	    flagList += "WStyle_NormalBorder";
	    wstyle = true;
	}
	else if ( rbBorderDialog->isChecked() ) {
	    f |= WStyle_DialogBorder;
	    flagList += "WStyle_DialogBorder";
	    wstyle = true;
	}

	if ( bgTitle->isChecked() ) {
	    f |= WStyle_Title;
	    flagList += "WStyle_Title";
	    wstyle = true;
	    if ( cbTitleSystem->isChecked() ) {
		f |= WStyle_SysMenu;
		flagList += "WStyle_SysMenu";
	    }
	    if ( cbTitleMinimize->isChecked() ) {
		f |= WStyle_Minimize;
		flagList += "WStyle_Minimize";
	    }
	    if ( cbTitleMaximize->isChecked() ) {
		f |= WStyle_Maximize;
		flagList += "WStyle_Maximize";
	    }
	    if ( cbTitleContext->isChecked() ) {
		f |= WStyle_ContextHelp;
		flagList += "WStyle_ContextHelp";
	    }
	}
    } else {
	f |= WStyle_NoBorder;
	flagList += "WStyle_NoBorder";
	wstyle = true;
    }

    TQWidget *parent = this;
    if ( cbBehaviorTaskbar->isChecked() ) {
	parent = 0;
	f |= WGroupLeader;
	flagList += "WGroupLeader";
    }
    if ( cbBehaviorStays->isChecked() ) {
	f |= WStyle_StaysOnTop /*| WX11BypassWM*/;
	flagList += "WStyle_StaysOnTop";
	wstyle = true;
    }
    if ( cbBehaviorPopup->isChecked() ) {
	f |= WType_Popup;
	flagList += "WType_Popup";
    }
    if ( cbBehaviorModal->isChecked() ) {
	f |= WShowModal;
	flagList += "WShowModal";
    }
    if ( cbBehaviorTool->isChecked() ) {
	f |= WStyle_Tool;
	flagList += "WStyle_Tool";
	wstyle = true;
    }

    if (wstyle)
	flagList.push_front("WStyle_Customize");

    if ( !widget ) {
	widget = new TQVBox( parent, 0, f );
	widget->setMargin( 20 );
	TQLabel *label = new TQLabel(flagList.join("&nbsp;| "), widget);
	label->setTextFormat(RichText);
	label->setAlignment(WordBreak);
	TQPushButton *okButton = new TQPushButton( "Close", widget );
	connect( okButton, TQ_SIGNAL(clicked()), widget, TQ_SLOT(close()) );
	widget->move( pos() );
    } else {
	widget->reparent( parent, f, widget->geometry().topLeft(), false);
    }

    widget->setCaption( leCaption->text() );
    widget->setIcon( leIcon->text() );
    widget->setWindowOpacity(double(slTransparency->maxValue() - slTransparency->value()) / 100);

    widget->show();
}

void OptionsDialog::pickIcon()
{
    TQString filename = TQFileDialog::getOpenFileName( TQString::null, TQString::null, this );
    leIcon->setText( filename );
}
