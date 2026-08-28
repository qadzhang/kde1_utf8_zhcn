 /**********************************************************************
** Copyright (C) 2000-2008 Trolltech ASA.  All rights reserved.
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

#ifndef FILTERINTERFACE_H
#define FILTERINTERFACE_H

#include <private/qcom_p.h>

// {ea8cb381-59b5-44a8-bae5-9bea8295762a}
#ifndef IID_ImportFilter
#define IID_ImportFilter TQUuid( 0xea8cb381, 0x59b5, 0x44a8, 0xba, 0xe5, 0x9b, 0xea, 0x82, 0x95, 0x76, 0x2a )
#endif

/*! If you write a filter plugin to import dialogs or other user
  interfaces from a different format than .ui into the TQt Designer,
  implement this interface in that plugin.

  You also have to implement the function featureList() (\sa
  TQFeatureListInterface) and return there all filters (names of it)
  which this interface provides.
*/

struct ImportFilterInterface : public TQFeatureListInterface
{
    /*! This function is called by TQt Designer to open the file \a
      filename using the filter \a filter. TQt Designer expects to get
      back one or more .ui files, which it can open then. In the
      implementation of the interface you have to return these
      filenames, which the filter created, in this function.*/
    virtual TQStringList import( const TQString &filter, const TQString &filename ) = 0;
};

// *************** INTERNAL *************************

// {c32a07e0-b006-471e-afca-d227457a1280}
#ifndef IID_ExportFilterInterface
#define IID_ExportFilterInterface TQUuid( 0xc32a07e0, 0xb006, 0x471e, 0xaf, 0xca, 0xd2, 0x27, 0x45, 0x7a, 0x12, 0x80 )
#endif

struct ExportFilterInterface : public TQFeatureListInterface
{
//    virtual TQStringList export( const TQString& filter, const TQString& filename ) = 0;
};

#endif
