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

#ifndef NEWFORMIMPL_H
#define NEWFORMIMPL_H

#include "newform.h"
#include <ntqiconview.h>

class Project;

class NewItem : public TQIconViewItem
{
public:
    enum Type {
	ProjectType,
	Form,
	CustomForm,
	SourceFileType,
	SourceTemplateType
    };

    NewItem( TQIconView *view, const TQString &text )
	: TQIconViewItem( view, text ) {}
    virtual void insert( Project *pro ) = 0;
    virtual void setProject( Project * ) {}

};

class ProjectItem : public NewItem
{
public:
    ProjectItem( TQIconView *view, const TQString &text );
    void insert( Project *pro );
    int rtti() const { return (int)ProjectType; }

    void setLanguage( const TQString &l ) { lang = l; }
    TQString language() const { return lang; }

private:
    TQString lang;

};

class FormItem : public NewItem
{
public:
    enum FormType {
	Widget,
	Dialog,
	Wizard,
	MainWindow
    };

    FormItem( TQIconView *view, const TQString &text );
    void insert( Project *pro );
    int rtti() const { return (int)Form; }

    void setFormType( FormType ft ) { fType = ft; }
    FormType formType() const { return fType; }

private:
    FormType fType;

};

class CustomFormItem : public NewItem
{
public:
    CustomFormItem( TQIconView *view, const TQString &text );
    void insert( Project *pro );
    int rtti() const { return (int)CustomForm; }

    void setTemplateFile( const TQString &tf ) { templFile = tf; }
    TQString templateFileName() const { return templFile; }

private:
    TQString templFile;

};

class SourceFileItem : public NewItem
{
public:
    SourceFileItem( TQIconView *view, const TQString &text );
    void insert( Project *pro );
    int rtti() const { return (int)SourceFileType; }

    void setExtension( const TQString &e ) { ext = e; }
    TQString extension() const { return ext; }
    void setLanguage( const TQString &l ) { lang = l; }
    void setProject( Project *pro );

private:
    TQString ext;
    TQString lang;
    bool visible;

};

class SourceTemplateItem : public NewItem
{
public:
    SourceTemplateItem( TQIconView *view, const TQString &text );
    void insert( Project *pro );
    int rtti() const { return (int)SourceTemplateType; }

    void setTemplate( const TQString &t ) { templ = t; }
    TQString tenplate() const { return templ; }
    void setLanguage( const TQString &l ) { lang = l; }
    void setProject( Project *pro );

private:
    TQString templ;
    TQString lang;
    bool visible;

};

class NewForm : public NewFormBase
{
    TQ_OBJECT

public:
    NewForm( TQIconView *templateView, const TQString &templatePath );
    NewForm( TQWidget *parent, const TQStringList& projects,
	     const TQString& currentProject, const TQString &templatePath );
    void insertTemplates( TQIconView*, const TQString& );
    void accept();
    TQPtrList<TQIconViewItem> allViewItems();

protected slots:
    void projectChanged( const TQString &project );
    void itemChanged( TQIconViewItem *item );

private:
    TQPtrList<TQIconViewItem> allItems;

};

#endif
