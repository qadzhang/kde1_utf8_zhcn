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

#include <designerinterface.h>
#include <ntqfeatures.h>
#include <ntqwidget.h>
#include <templatewizardiface.h>
#ifndef TQT_NO_SQL
#include "sqlformwizardimpl.h"
#endif
#include "mainwindowwizard.h"
#include <ntqapplication.h>

class StandardTemplateWizardInterface : public TemplateWizardInterface, public TQLibraryInterface
{
public:
    StandardTemplateWizardInterface();
    virtual ~StandardTemplateWizardInterface();

    TQRESULT queryInterface( const TQUuid&, TQUnknownInterface** );
    TQ_REFCOUNT;

    TQStringList featureList() const;

    void setup( const TQString &templ, TQWidget *widget, DesignerFormWindow *fw, TQUnknownInterface *aIface );

    bool init();
    void cleanup();
    bool canUnload() const;

private:
    bool inUse;
};

StandardTemplateWizardInterface::StandardTemplateWizardInterface()
    : inUse( false )
{
}

StandardTemplateWizardInterface::~StandardTemplateWizardInterface()
{
}

bool StandardTemplateWizardInterface::init()
{
    return true;
}

void StandardTemplateWizardInterface::cleanup()
{
}

bool StandardTemplateWizardInterface::canUnload() const
{
    return !inUse;
}

TQStringList StandardTemplateWizardInterface::featureList() const
{
    TQStringList list;
#ifndef TQT_NO_SQL
    list << "TQDataBrowser" << "TQDesignerDataBrowser" << "TQDataView" << \
	"TQDesignerDataView" << "TQDataTable";
#endif
    list << "TQMainWindow";

    return list;
}

void StandardTemplateWizardInterface::setup( const TQString &templ, TQWidget *widget, DesignerFormWindow *fw, TQUnknownInterface *aIface )
{
    inUse = true;
#ifndef TQT_NO_SQL
    if ( templ == "TQDesignerDataView" ||
	 templ == "TQDesignerDataBrowser" ||
	 templ == "TQDataView" ||
	 templ == "TQDataBrowser" ||
	 templ == "TQDataTable" ) {
	SqlFormWizard wizard( aIface, widget, tqApp->mainWidget(), fw, 0, true );
	wizard.exec();
    }
#endif
    if ( templ == "TQMainWindow" ) {
	MainWindowWizardBase wizard( tqApp->mainWidget(), 0, true );
	wizard.setAppInterface( aIface, fw, widget );
	wizard.exec();
    }
    inUse = false;
}

TQRESULT StandardTemplateWizardInterface::queryInterface( const TQUuid& uuid, TQUnknownInterface** iface )
{
    *iface = 0;
    if ( uuid == IID_QUnknown )
	*iface = (TQUnknownInterface*)(TemplateWizardInterface*) this;
    else if ( uuid == IID_QFeatureList )
	*iface = (TQFeatureListInterface*)this;
    else if ( uuid == IID_TemplateWizard )
	*iface = (TemplateWizardInterface*)this;
    else if ( uuid == IID_QLibrary )
	*iface = (TQLibraryInterface*)this;
    else
	return TQE_NOINTERFACE;

    (*iface)->addRef();
    return TQS_OK;
}

TQ_EXPORT_COMPONENT()
{
    Q_CREATE_INSTANCE( StandardTemplateWizardInterface )
}
