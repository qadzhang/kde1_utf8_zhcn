/****************************************************************************
**
** Definition of something or other
**
** Created : 979899
**
** Copyright (C) 1997-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef WIDGETS_H
#define WIDGETS_H

#include <ntqmainwindow.h>
#include <ntqmovie.h>
#include <ntqlistview.h>
class TQLabel;
class TQCheckBox;
class TQProgressBar;
class TQTabWidget;
class TQGroupBox;
class TQMultiLineEdit;
class TQPopupMenu;

class MyListView : public TQListView
{
    TQ_OBJECT
public:
    MyListView( TQWidget * parent = 0, const char *name = 0 )
	: TQListView( parent, name ), selected(0)
    {}
    ~MyListView()
    {}
protected:

    void contentsMousePressEvent( TQMouseEvent * e )
    {
	selected = selectedItem();
	TQListView::contentsMousePressEvent( e );
    }
    void contentsMouseReleaseEvent( TQMouseEvent * e )
    {
	TQListView::contentsMouseReleaseEvent( e );
	if ( selectedItem() != selected ) {
	    emit mySelectionChanged( selectedItem() );
	    emit mySelectionChanged();
	}
    }

signals:
    void mySelectionChanged();
    void mySelectionChanged( TQListViewItem* );

private:
    TQListViewItem* selected;

};
//
// WidgetView contains lots of TQt widgets.
//

class WidgetView : public TQMainWindow
{
    TQ_OBJECT
public:
    WidgetView( TQWidget *parent=0, const char *name=0 );

public slots:
    void	setStatus(const TQString&);
    void selectionChanged();
    void selectionChanged( TQListViewItem* );
    void clicked( TQListViewItem* );
    void mySelectionChanged( TQListViewItem* );

protected slots:
   virtual void button1Clicked();
private slots:
    void	checkBoxClicked( int );
    void	radioButtonClicked( int );
    void	sliderValueChanged( int );
    void	listBoxItemSelected( int );
    void	comboBoxItemActivated( int );
    void	edComboBoxItemActivated( const TQString& );
    void	lineEditTextChanged( const TQString& );
    void	movieStatus( int );
    void	movieUpdate( const TQRect& );
    void	spinBoxValueChanged( const TQString& );
    void	popupSelected( int );

    void	open();
    void	dummy();
    void	showProperties();

private:
    bool	eventFilter( TQObject *, TQEvent * );
    TQLabel     *msg;
    TQCheckBox  *cb[3];
    TQGroupBox* bg;
    TQLabel     *movielabel;
    TQMovie      movie;
    TQWidget *central;
    TQProgressBar *prog;
    int progress;
    TQTabWidget* tabs;
    TQMultiLineEdit* edit;
    TQPopupMenu *textStylePopup;
    int plainStyleID;
    TQWidget* bla;
};

#endif
