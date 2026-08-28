/****************************************************************************
**
** Global type declarations and definitions
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of the tools module of the TQt GUI Toolkit.
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

#if defined(Q_TEMPLATEDLL) && ( !defined(QT_MAKEDLL) || defined(TQ_EXPORT_TEMPLATES) )
// MOC_SKIP_BEGIN

#if defined(Q_DEFINED_QASCIIDICT) && defined(Q_DEFINED_QCONNECTION_LIST) && !defined(TQ_EXPORTED_QASCIIDICT_TEMPLATES)
#define TQ_EXPORTED_QASCIIDICT_TEMPLATES
Q_TEMPLATE_EXTERN template class TQ_EXPORT TQAsciiDictIterator<TQConnectionList>;
Q_TEMPLATE_EXTERN template class TQ_EXPORT TQAsciiDict<TQConnectionList>;
#endif

#if defined(Q_DEFINED_QSTYLESHEET) && defined(Q_DEFINED_QDICT) && !defined(TQ_EXPORTED_QSTYLESHEET_TEMPLATES)
#define TQ_EXPORTED_QSTYLESHEET_TEMPLATES
Q_TEMPLATE_EXTERN template class TQ_EXPORT TQDict<TQStyleSheetItem>;
#endif

#if defined(Q_DEFINED_QLIBRARY) && defined(Q_DEFINED_QDICT) && !defined(TQ_EXPORTED_QDICTLIBRARY_TEMPLATES)
#define TQ_EXPORTED_QDICTLIBRARY_TEMPLATES
Q_TEMPLATE_EXTERN template class TQ_EXPORT TQDict<TQLibrary>; // for TQtopia
#endif

#if defined(Q_DEFINED_QGUARDEDPTR) && defined(Q_DEFINED_QOBJECT) && !defined(TQ_EXPORTED_QGUARDEDPTROBJECT_TEMPLATES)
#define TQ_EXPORTED_QGUARDEDPTROBJECT_TEMPLATES
Q_TEMPLATE_EXTERN template class TQ_EXPORT TQGuardedPtr<TQObject>;
#endif

// needed for TQtopia
#if defined(Q_DEFINED_QGUARDEDPTR) && defined(Q_DEFINED_QWIDGET) && !defined(TQ_EXPORTED_QGUARDEDPTRTQWIDGET_TEMPLATES)
#define TQ_EXPORTED_QGUARDEDPTRTQWIDGET_TEMPLATES
Q_TEMPLATE_EXTERN template class TQ_EXPORT TQGuardedPtr<TQWidget>;
#endif

#if defined(Q_DEFINED_QGUARDEDPTR) && defined(Q_DEFINED_QACCESSIBLE_OBJECT) && !defined(TQ_EXPORTED_QACCESSIBLEOBJECT_TEMPLATES)
#define TQ_EXPORTED_QACCESSIBLEOBJECT_TEMPLATES
Q_TEMPLATE_EXTERN template class TQ_EXPORT TQGuardedPtr<TQAccessibleObject>;
#endif

#if defined(Q_DEFINED_QINTDICT) && !defined(TQ_EXPORTED_QINTDICT_TEMPLATES)
#define TQ_EXPORTED_QINTDICT_TEMPLATES
Q_TEMPLATE_EXTERN template class TQ_EXPORT TQIntDict<int>;
#endif

#if defined(Q_DEFINED_QINTDICT) && defined(Q_DEFINED_QWIDGET) && !defined(TQ_EXPORTED__TEMPLATES)
#define TQ_EXPORTED__TEMPLATES
Q_TEMPLATE_EXTERN template class TQ_EXPORT TQIntDictIterator<TQWidget>;
Q_TEMPLATE_EXTERN template class TQ_EXPORT TQIntDict<TQWidget>;
#endif

#if defined(Q_DEFINED_QMAP) && !defined(TQ_EXPORTED_QMAPBASIC_TEMPLATES)
#define TQ_EXPORTED_QMAPBASIC_TEMPLATES
Q_TEMPLATE_EXTERN template class TQ_EXPORT TQMap<int, int>; // for TQtopia
Q_TEMPLATE_EXTERN template class TQ_EXPORT TQMap<int, bool>; // for TQtopia
#endif

#if defined(Q_DEFINED_QMAP) && defined(Q_DEFINED_QSTRING) && !defined(TQ_EXPORTED_QMAPTQSTRING_TEMPLATES)
#define TQ_EXPORTED_QMAPTQSTRING_TEMPLATES
Q_TEMPLATE_EXTERN template class TQ_EXPORT TQMap<TQString, TQString>;
Q_TEMPLATE_EXTERN template class TQ_EXPORT TQMap<TQString, int>; // for TQtopia
Q_TEMPLATE_EXTERN template class TQ_EXPORT TQMap<int, TQString>; // for TQtopia
#endif

#if defined(Q_DEFINED_QMEMARRAY)  && !defined(TQ_EXPORTED_QMEMARRAY_BASIC_TEMPLATES)
#define TQ_EXPORTED_QMEMARRAY_BASIC_TEMPLATES
Q_TEMPLATE_EXTERN template class TQ_EXPORT TQMemArray<int>;
Q_TEMPLATE_EXTERN template class TQ_EXPORT TQMemArray<bool>;
Q_TEMPLATE_EXTERN template class TQ_EXPORT TQMemArray<char>;
#endif

#if defined(Q_DEFINED_QMEMARRAY) && defined(Q_DEFINED_QPOINT)  && !defined(TQ_EXPORTED_QMEMARAYPOINT_TEMPLATES)
#define TQ_EXPORTED_QMEMARAYPOINT_TEMPLATES
Q_TEMPLATE_EXTERN template class TQ_EXPORT TQMemArray<TQPoint>;
#endif

#if defined(Q_DEFINED_QPTRLIST)  && !defined(TQ_EXPORTED_QPTRLIST_BASIC_TEMPLATES)
#define TQ_EXPORTED_QPTRLIST_BASIC_TEMPLATES
Q_TEMPLATE_EXTERN template class TQ_EXPORT TQPtrListIterator<char>;
Q_TEMPLATE_EXTERN template class TQ_EXPORT TQPtrList<char>;
#endif

#if defined(Q_DEFINED_QPTRLIST) && defined(Q_DEFINED_QWIDGET)  && !defined(TQ_EXPORTED_QPTRLISTWIDGET_TEMPLATES)
#define TQ_EXPORTED_QPTRLISTWIDGET_TEMPLATES
Q_TEMPLATE_EXTERN template class TQ_EXPORT TQPtrListIterator<TQWidget>;
Q_TEMPLATE_EXTERN template class TQ_EXPORT TQPtrList<TQWidget>;
#endif

#if defined(Q_DEFINED_QPTRLIST) && defined(Q_DEFINED_QCONNECTION)  && !defined(TQ_EXPORTED_QPTRLISTCONNECTION_TEMPLATES)
#define TQ_EXPORTED_QPTRLISTCONNECTION_TEMPLATES
Q_TEMPLATE_EXTERN template class TQ_EXPORT TQPtrListIterator<TQConnection>;
Q_TEMPLATE_EXTERN template class TQ_EXPORT TQPtrList<TQConnection>;
#endif

#if defined(Q_DEFINED_QPTRLIST) && defined(Q_DEFINED_QOBJECT)  && !defined(TQ_EXPORTED_QPTRLISTOBJECT_TEMPLATES)
#define TQ_EXPORTED_QPTRLISTOBJECT_TEMPLATES
Q_TEMPLATE_EXTERN template class TQ_EXPORT TQPtrListIterator<TQObject>;
Q_TEMPLATE_EXTERN template class TQ_EXPORT TQPtrList<TQObject>;
#endif

#if defined(Q_DEFINED_QPTRLIST) && defined(Q_DEFINED_QDOCKWINDOW)  && !defined(TQ_EXPORTED_QPTRLISTDOCWINDOW_TEMPLATES)
#define TQ_EXPORTED_QPTRLISTDOCWINDOW_TEMPLATES
Q_TEMPLATE_EXTERN template class TQ_EXPORT TQPtrListIterator<TQDockWindow>;
Q_TEMPLATE_EXTERN template class TQ_EXPORT TQPtrList<TQDockWindow>;
#endif

#if defined(Q_DEFINED_QPTRVECTOR)  && !defined(TQ_EXPORTED_QPTRVECTOR_BASIC_TEMPLATES)
#define TQ_EXPORTED_QPTRVECTOR_BASIC_TEMPLATES
Q_TEMPLATE_EXTERN template class TQ_EXPORT TQPtrVector<int>;
#endif

#if defined(Q_DEFINED_QPTRVECTOR) && defined(Q_DEFINED_QSTYLESHEET)  && !defined(TQ_EXPORTED_QPTRVECTORSTYLESHEETITEM_TEMPLATES)
#define TQ_EXPORTED_QPTRVECTORSTYLESHEETITEM_TEMPLATES
Q_TEMPLATE_EXTERN template class TQ_EXPORT TQPtrVector<TQStyleSheetItem>;
#endif

#if defined(Q_DEFINED_QPTRVECTOR) && defined(Q_DEFINED_QWIDGET)  && !defined(TQ_EXPORTED_QPTRVECTORWIDGET_TEMPLATES)
#define TQ_EXPORTED_QPTRVECTORWIDGET_TEMPLATES
Q_TEMPLATE_EXTERN template class TQ_EXPORT TQPtrVector<TQWidget>;
#endif

#if defined(Q_DEFINED_QPTRVECTOR) && defined(Q_DEFINED_QCONNECTION_LIST)  && !defined(TQ_EXPORTED_QPTRVECTORCONNECTTIONLIST_TEMPLATES)
#define TQ_EXPORTED_QPTRVECTORCONNECTTIONLIST_TEMPLATES
Q_TEMPLATE_EXTERN template class TQ_EXPORT TQPtrVector<TQConnectionList>;
#endif

#if defined(Q_DEFINED_QVALUELIST)  && !defined(TQ_EXPORTED_QVALUELIST_BASIC_TEMPLATES)
#define TQ_EXPORTED_QVALUELIST_BASIC_TEMPLATES
Q_TEMPLATE_EXTERN template class TQ_EXPORT TQValueListIterator<bool>;
Q_TEMPLATE_EXTERN template class TQ_EXPORT TQValueList<bool>;
Q_TEMPLATE_EXTERN template class TQ_EXPORT TQValueListIterator<int>;
Q_TEMPLATE_EXTERN template class TQ_EXPORT TQValueList<int>;
#endif

#if defined(Q_DEFINED_QVALUELIST) && defined(Q_DEFINED_QRECT)  && !defined(TQ_EXPORTED_QVALUELISTRECT_TEMPLATES)
#define TQ_EXPORTED_QVALUELISTRECT_TEMPLATES
Q_TEMPLATE_EXTERN template class TQ_EXPORT TQValueListIterator<TQRect>;
Q_TEMPLATE_EXTERN template class TQ_EXPORT TQValueList<TQRect>;
#endif

#if defined(Q_DEFINED_QVALUELIST) && defined(Q_DEFINED_QSTRING)  && !defined(TQ_EXPORTED_QVALUELISTSTRING_TEMPLATES)
#define TQ_EXPORTED_QVALUELISTSTRING_TEMPLATES
Q_TEMPLATE_EXTERN template class TQ_EXPORT TQValueListIterator<TQString>;
Q_TEMPLATE_EXTERN template class TQ_EXPORT TQValueList<TQString>;
#endif

// TQStylesheet template exports
#if defined(Q_DEFINED_QVALUELIST) && defined(Q_DEFINED_QPTRVECTOR) && defined(Q_DEFINED_QSTYLESHEET)  && !defined(TQ_EXPORTED_QSTYLESHEETITEM1_TEMPLATES)
#define TQ_EXPORTED_QSTYLESHEETITEM1_TEMPLATES
Q_TEMPLATE_EXTERN template class TQ_EXPORT TQValueList< TQPtrVector< TQStyleSheetItem> >;
#endif

#if defined(Q_DEFINED_QVALUELIST) && defined(Q_DEFINED_QSTYLESHEET)  && !defined(TQ_EXPORTED_QSTYLESHEETITEM2_TEMPLATES)
#define TQ_EXPORTED_QSTYLESHEETITEM2_TEMPLATES
Q_TEMPLATE_EXTERN template class TQ_EXPORT TQValueList<TQStyleSheetItem::ListStyle>;
#endif

// qcanvas template exports
#if defined(Q_DEFINED_QPTRLIST)  && defined(Q_DEFINED_QCANVAS) && !defined(TQ_EXPORTED_QCANVAS1_TEMPLATES)
#define TQ_EXPORTED_QCANVAS1_TEMPLATES
TQM_TEMPLATE_EXTERN_CANVAS template class TQM_EXPORT_CANVAS TQPtrListIterator< TQCanvasItem >;
TQM_TEMPLATE_EXTERN_CANVAS template class TQM_EXPORT_CANVAS TQPtrList< TQCanvasItem >;
TQM_TEMPLATE_EXTERN_CANVAS template class TQM_EXPORT_CANVAS TQPtrListIterator< TQCanvasView >;
TQM_TEMPLATE_EXTERN_CANVAS template class TQM_EXPORT_CANVAS TQPtrList< TQCanvasView >;
#endif

// qtable template exports
#if defined(Q_DEFINED_QPTRLIST) && defined(Q_DEFINED_QTABLE_SELECTION) && !defined(TQ_EXPORTED_QTABLESELECTION_TEMPLATES)
#define TQ_EXPORTED_QTABLESELECTION_TEMPLATES
TQM_TEMPLATE_EXTERN_TABLE template class TQM_EXPORT_TABLE TQPtrList<TQTableSelection>;
#endif

#if defined(Q_DEFINED_QTABLE_ITEM) && defined(Q_DEFINED_QPTRVECTOR) && !defined(TQ_EXPORTED_QTABLEITEM_TEMPLATES)
#define TQ_EXPORTED_QTABLEITEM_TEMPLATES
TQM_TEMPLATE_EXTERN_TABLE template class TQM_EXPORT_TABLE TQPtrVector<TQTableItem>;
#endif

#if defined(Q_DEFINED_QTABLE) && defined(Q_DEFINED_QPTRVECTOR)
//Q_TEMPLATE_EXTERN template class TQ_EXPORT TQPtrVector<TQTable>;
#endif

// qsqlextension template exports
#if defined(Q_DEFINED_QSQLEXTENSION) && defined(Q_DEFINED_QMAP) && defined(Q_DEFINED_QVALUEVECTOR) && defined(Q_DEFINED_QSTRING) && !defined(TQ_EXPORTED_QSQLEXTENSION_TEMPLATES)
#define TQ_EXPORTED_QSQLEXTENSION_TEMPLATES
TQM_TEMPLATE_EXTERN_SQL template class TQM_EXPORT_SQL TQMap<TQString,TQSqlParam>;
TQM_TEMPLATE_EXTERN_SQL template class TQM_EXPORT_SQL TQValueVector<Holder>;
#endif


// MOC_SKIP_END
#endif // template defined
