/****************************************************************************
**
** Implementation of TQGPluginManager class
**
** Copyright (C) 2000-2008 Trolltech ASA.  All rights reserved.
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

#include "qgpluginmanager_p.h"
#ifndef TQT_NO_COMPONENT
#include "qcomlibrary_p.h"
#include "ntqmap.h"
#include "ntqdir.h"

/*
  The following co-occurrence code is borrowed from TQt Linguist.

  How similar are two texts? The approach used here relies on
  co-occurrence matrices and is very efficient.

  Let's see with an example: how similar are "here" and "hither"?  The
  co-occurrence matrix M for "here" is M[h,e] = 1, M[e,r] = 1,
  M[r,e] = 1 and 0 elsewhere; the matrix N for "hither" is N[h,i] = 1,
  N[i,t] = 1, ..., N[h,e] = 1, N[e,r] = 1 and 0 elsewhere.  The union
  U of both matrices is the matrix U[i,j] = max { M[i,j], N[i,j] },
  and the intersection V is V[i,j] = min { M[i,j], N[i,j] }. The score
  for a pair of texts is

      score = (sum of V[i,j] over all i, j) / (sum of U[i,j] over all i, j),

  a formula suggested by Arnt Gulbrandsen. Here we have

      score = 2 / 6,

  or one third.

  The implementation differs from this in a few details.  Most
  importantly, repetitions are ignored; for input "xxx", M[x,x] equals
  1, not 2.
*/

/*
  Every character is assigned to one of 20 buckets so that the
  co-occurrence matrix requires only 20 * 20 = 400 bits, not
  256 * 256 = 65536 bits or even more if we want the whole Unicode.
  Which character falls in which bucket is arbitrary.

  The second half of the table is a replica of the first half, because of
  laziness.
*/
static const char indexOf[256] = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
//      !   "   #   $   %   &   '   (   )   *   +   ,   -   .   /
    0,  2,  6,  7,  10, 12, 15, 19, 2,  6,  7,  10, 12, 15, 19, 0,
//  0   1   2   3   4   5   6   7   8   9   :   ;   <   =   >   ?
    1,  3,  4,  5,  8,  9,  11, 13, 14, 16, 2,  6,  7,  10, 12, 15,
//  @   A   B   C   D   E   F   G   H   I   J   K   L   M   N   O
    0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  6,  10, 11, 12, 13, 14,
//  P   Q   R   S   T   U   V   W   X   Y   Z   [   \   ]   ^   _
    15, 12, 16, 17, 18, 19, 2,  10, 15, 7,  19, 2,  6,  7,  10, 0,
//  `   a   b   c   d   e   f   g   h   i   j   k   l   m   n   o
    0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  6,  10, 11, 12, 13, 14,
//  p   q   r   s   t   u   v   w   x   y   z   {   |   }   ~
    15, 12, 16, 17, 18, 19, 2,  10, 15, 7,  19, 2,  6,  7,  10, 0,

    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  2,  6,  7,  10, 12, 15, 19, 2,  6,  7,  10, 12, 15, 19, 0,
    1,  3,  4,  5,  8,  9,  11, 13, 14, 16, 2,  6,  7,  10, 12, 15,
    0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  6,  10, 11, 12, 13, 14,
    15, 12, 16, 17, 18, 19, 2,  10, 15, 7,  19, 2,  6,  7,  10, 0,
    0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  6,  10, 11, 12, 13, 14,
    15, 12, 16, 17, 18, 19, 2,  10, 15, 7,  19, 2,  6,  7,  10, 0
};

/*
  The entry bitCount[i] (for i between 0 and 255) is the number of
  bits used to represent i in binary.
*/
static const char bitCount[256] = {
    0,  1,  1,  2,  1,  2,  2,  3,  1,  2,  2,  3,  2,  3,  3,  4,
    1,  2,  2,  3,  2,  3,  3,  4,  2,  3,  3,  4,  3,  4,  4,  5,
    1,  2,  2,  3,  2,  3,  3,  4,  2,  3,  3,  4,  3,  4,  4,  5,
    2,  3,  3,  4,  3,  4,  4,  5,  3,  4,  4,  5,  4,  5,  5,  6,
    1,  2,  2,  3,  2,  3,  3,  4,  2,  3,  3,  4,  3,  4,  4,  5,
    2,  3,  3,  4,  3,  4,  4,  5,  3,  4,  4,  5,  4,  5,  5,  6,
    2,  3,  3,  4,  3,  4,  4,  5,  3,  4,  4,  5,  4,  5,  5,  6,
    3,  4,  4,  5,  4,  5,  5,  6,  4,  5,  5,  6,  5,  6,  6,  7,
    1,  2,  2,  3,  2,  3,  3,  4,  2,  3,  3,  4,  3,  4,  4,  5,
    2,  3,  3,  4,  3,  4,  4,  5,  3,  4,  4,  5,  4,  5,  5,  6,
    2,  3,  3,  4,  3,  4,  4,  5,  3,  4,  4,  5,  4,  5,  5,  6,
    3,  4,  4,  5,  4,  5,  5,  6,  4,  5,  5,  6,  5,  6,  6,  7,
    2,  3,  3,  4,  3,  4,  4,  5,  3,  4,  4,  5,  4,  5,  5,  6,
    3,  4,  4,  5,  4,  5,  5,  6,  4,  5,  5,  6,  5,  6,  6,  7,
    3,  4,  4,  5,  4,  5,  5,  6,  4,  5,  5,  6,  5,  6,  6,  7,
    4,  5,  5,  6,  5,  6,  6,  7,  5,  6,  6,  7,  6,  7,  7,  8
};

class TQCoMatrix
{
public:
    /*
      The matrix has 20 * 20 = 400 entries. This requires 50 bytes, or
      13 words. Some operations are performed on words for more
      efficiency.
    */
    union {
	TQ_UINT8 b[52];
	TQ_UINT32 w[13];
    };

    TQCoMatrix() { memset( b, 0, 52 ); }
    TQCoMatrix( const char *text ) {
	char c = '\0', d;
	memset( b, 0, 52 );
	while ( (d = *text) != '\0' ) {
	    setCoocc( c, d );
	    if ( (c = *++text) != '\0' ) {
		setCoocc( d, c );
		text++;
	    }
	}
    }

    void setCoocc( char c, char d ) {
	int k = indexOf[(uchar) c] + 20 * indexOf[(uchar) d];
	b[k >> 3] |= k & 0x7;
    }

    int worth() const {
	int result = 0;
	for ( int i = 0; i < 50; i++ )
	    result += bitCount[b[i]];
	return result;
    }

    static TQCoMatrix reunion( const TQCoMatrix& m, const TQCoMatrix& n )
    {
	TQCoMatrix p;
	for ( int i = 0; i < 13; i++ )
	    p.w[i] = m.w[i] | n.w[i];
	return p;
    }
    static TQCoMatrix intersection( const TQCoMatrix& m, const TQCoMatrix& n )
    {
	TQCoMatrix p;
	for ( int i = 0; i < 13; i++ )
	    p.w[i] = m.w[i] & n.w[i];
	return p;
    }
};

/*
  Returns an integer between 0 (dissimilar) and 15 (very similar)
  depending on  how similar the string is to \a target.

  This function is efficient, but its results might change in future
  versions of TQt as the algorithm evolves.

  \code
    TQString s( "color" );
    a = similarity( s, "color" );  // a == 15
    a = similarity( s, "colour" ); // a == 8
    a = similarity( s, "flavor" ); // a == 4
    a = similarity( s, "dahlia" ); // a == 0
  \endcode
*/
static int similarity( const TQString& s1, const TQString& s2 )
{
    TQCoMatrix m1( s1 );
    TQCoMatrix m2( s2 );
    return ( 15 * (TQCoMatrix::intersection(m1, m2).worth() + 1) ) /
	   ( TQCoMatrix::reunion(m1, m2).worth() + 1 );
}

/*!
  \class TQPluginManager qpluginmanager.h
  \reentrant
  \brief The TQPluginManager class provides basic functions to access a certain kind of functionality in libraries.
  \ingroup componentmodel

  \internal

  A common usage of components is to extend the existing functionality in an application using plugins. The application
  defines interfaces that abstract a certain group of functionality, and a plugin provides a specialized implementation
  of one or more of those interfaces.

  The TQPluginManager template has to be instantiated with an interface definition and the IID for this interface.

  \code
  TQPluginManager<MyPluginInterface> *manager = new TQPluginManager<MyPluginInterface>( IID_MyPluginInterface );
  \endcode

  It searches a specified directory for all shared libraries, queries for components that implement the specific interface and
  reads information about the features the plugin wants to add to the application. The component can provide the set of features
  provided by implementing either the TQFeatureListInterface or the TQComponentInformationInterface. The strings returned by the implementations
  of

  \code
  TQStringList TQFeatureListInterface::featureList() const
  \endcode

  or

  \code
  TQString TQComponentInformationInterface::name() const
  \endcode

  respectively, can then be used to access the component that provides the requested feature:

  \code
  MyPluginInterface *iface;
  manager->queryInterface( "feature", &iface );
  if ( iface )
      iface->execute( "feature" );
  \endcode

  The application can use a TQPluginManager instance to create parts of the user interface based on the list of features
  found in plugins:

  \code
  TQPluginManager<MyPluginInterface> *manager = new TQPluginManager<MyPluginInterface>( IID_ImageFilterInterface );
  manager->addLibraryPath(...);

  TQStringList features = manager->featureList();
  for ( TQStringList::Iterator it = features.begin(); it != features.end(); ++it ) {
      MyPluginInterface *iface;
      manager->queryInterface( *it, &iface );

      // use TQAction to provide toolbuttons and menuitems for each feature...
  }
  \endcode
*/

/*!
  \fn TQPluginManager::TQPluginManager( const TQUuid& id, const TQStringList& paths = TQString::null, const TQString &suffix = TQString::null, bool cs = true )

  Creates an TQPluginManager for interfaces \a id that will load all shared library files in the \a paths + \a suffix.
  If \a cs is false the manager will handle feature strings case insensitive.

  \warning
  Setting the cs flag to false requires that components also convert to lower case when comparing with passed strings, so this has
  to be handled with care and documented very well.

  \sa TQApplication::libraryPaths()
*/


/*!
  \fn TQRESULT TQPluginManager::queryInterface(const TQString& feature, Type** iface) const

  Sets \a iface to point to the interface providing \a feature.

  \sa featureList(), library()
*/



#include <ntqptrlist.h>

TQGPluginManager::TQGPluginManager( const TQUuid& id, const TQStringList& paths, const TQString &suffix, bool cs )
    : interfaceId( id ), plugDict( 17, cs ), casesens( cs ), autounload( true )
{
    // Every TQLibrary object is destroyed on destruction of the manager
    libDict.setAutoDelete( true );
    for ( TQStringList::ConstIterator it = paths.begin(); it != paths.end(); ++it ) {
	TQString path = *it;
	addLibraryPath( path + suffix );
    }
}

TQGPluginManager::~TQGPluginManager()
{
    if ( !autounload ) {
	TQDictIterator<TQLibrary> it( libDict );
	while ( it.current() ) {
	    TQLibrary *lib = it.current();
	    ++it;
	    lib->setAutoUnload( false );
	}
    }
}

void TQGPluginManager::addLibraryPath( const TQString& path )
{
    if ( !enabled() || !TQDir( path ).exists( ".", true ) )
	return;

#if defined(Q_OS_WIN32)
    TQString filter = "*.dll";
#elif defined(Q_OS_MACX)
    TQString filter = "*.dylib; *.so; *.bundle";
#elif defined(Q_OS_OPENBSD)
    TQString filter = "*.so; *.so.*";
#elif defined(Q_OS_UNIX)
    TQString filter = "*.so";
#endif

    TQStringList plugins = TQDir(path).entryList( filter );
    for ( TQStringList::Iterator p = plugins.begin(); p != plugins.end(); ++p ) {
	TQString lib = TQDir::cleanDirPath( path + "/" + *p );
	if ( libList.contains( lib ) )
	    continue;
	libList.append( lib );
    }
}

const TQLibrary* TQGPluginManager::library( const TQString& feature ) const
{
    if ( !enabled() || feature.isEmpty() )
	return 0;

    // We already have a TQLibrary object for this feature
    TQLibrary *library = 0;
    if ( ( library = plugDict[feature] ) )
	return library;

    // Find the filename that matches the feature request best
    TQMap<int, TQStringList> map;
    TQStringList::ConstIterator it = libList.begin();
    int best = 0;
    int worst = 15;
    while ( it != libList.end() ) {
	if ( (*it).isEmpty() || libDict[*it] ) {
	    ++it;
	    continue;
	}
	TQString basename = TQFileInfo(*it).baseName();
	int s = similarity( feature, basename );
	if ( s < worst )
	    worst = s;
	if ( s > best )
	    best = s;
	map[s].append( basename + TQChar(0xfffd) + *it );
	++it;
    }

    if ( map.isEmpty() )
	return 0; // no libraries to add

    // Start with the best match to get the library object
    TQGPluginManager *that = (TQGPluginManager*)this;
    for ( int s = best; s >= worst; --s ) {
	TQStringList group = map[s];
	group.sort(); // sort according to the base name
	TQStringList::ConstIterator git = group.begin();
	while ( git != group.end() ) {
	    TQString lib = (*git).mid( (*git).find( TQChar(0xfffd) ) + 1 );
	    TQString basename = (*git).left( (*git).find( TQChar(0xfffd) ) );
	    ++git;

	    TQStringList sameBasename;
	    while( git != group.end() &&
		   basename == (*git).left( (*git).find( TQChar(0xfffd) ) )  ) {
		sameBasename << (*git).mid( (*git).find( TQChar(0xfffd) ) + 1 );
		++git;
	    }

	    if ( sameBasename.isEmpty() ) {
		that->addLibrary( new TQComLibrary( lib ) );
	    } else {
		TQPtrList<TQComLibrary> same;
		same.setAutoDelete( true );
		same.append( new TQComLibrary( lib ) );
		for ( TQStringList::ConstIterator bit = sameBasename.begin();
		      bit != sameBasename.end(); ++bit )
		    same.append( new TQComLibrary( *bit ) );
		TQComLibrary* bestMatch = 0;
		for ( TQComLibrary* candidate = same.first(); candidate; candidate = same.next() )
		    if ( candidate->qtVersion() && candidate->qtVersion() <= TQT_VERSION
			 && ( !bestMatch || candidate->qtVersion() > bestMatch->qtVersion() ) )
			bestMatch = candidate;
		if ( bestMatch ) {
		    same.find( bestMatch );
		    that->addLibrary( same.take() );
		}
	    }

	    if ( ( library = that->plugDict[feature] ) )
		return library;
	}
    }
    return 0;
}

TQStringList TQGPluginManager::featureList() const
{
    TQStringList features;

    if ( !enabled() )
	return features;

    TQGPluginManager *that = (TQGPluginManager*)this;
    TQStringList theLibs = libList;
    TQStringList phase2Libs;
    TQStringList phase2Deny;

    /* In order to get the feature list we need to add all interesting
      libraries. If there are libraries with the same base name, we
      prioritze the one that fits our TQt version number and ignore the
      others  */
    TQStringList::Iterator it;
    for ( it = theLibs.begin(); it != theLibs.end(); ++it  ) {
	if ( (*it).isEmpty() || libDict[*it] )
	    continue;
	TQComLibrary* library = new TQComLibrary( *it );
	if ( library->qtVersion() == TQT_VERSION ) {
	    that->addLibrary( library );
	    phase2Deny << TQFileInfo( *it ).baseName();
	} else {
	    delete library;
	    phase2Libs << *it;
	}
    }
    for ( it = phase2Libs.begin(); it != phase2Libs.end(); ++it  )
	if ( !phase2Deny.contains( TQFileInfo( *it ).baseName() ) )
	    that->addLibrary( new TQComLibrary( *it ) );

    for ( TQDictIterator<TQLibrary> pit( plugDict ); pit.current(); ++pit )
	features << pit.currentKey();

    return features;
}

bool TQGPluginManager::addLibrary( TQLibrary* lib )
{
    if ( !enabled() || !lib )
	return false;

    TQComLibrary* plugin = (TQComLibrary*)lib;
    bool useful = false;

    TQUnknownInterface* iFace = 0;
    plugin->queryInterface( interfaceId, &iFace );
    if ( iFace ) {
	TQFeatureListInterface *fliFace = 0;
	TQComponentInformationInterface *cpiFace = 0;
	iFace->queryInterface( IID_QFeatureList, (TQUnknownInterface**)&fliFace );
	if ( !fliFace )
	    plugin->queryInterface( IID_QFeatureList, (TQUnknownInterface**)&fliFace );
	if ( !fliFace ) {
	    iFace->queryInterface( IID_QComponentInformation, (TQUnknownInterface**)&cpiFace );
	    if ( !cpiFace )
		plugin->queryInterface( IID_QComponentInformation, (TQUnknownInterface**)&cpiFace );
	}
	TQStringList fl;
	if ( fliFace )
	    // Map all found features to the library
	    fl = fliFace->featureList();
	else if ( cpiFace )
	    fl << cpiFace->name();

	for ( TQStringList::Iterator f = fl.begin(); f != fl.end(); ++f ) {
	    TQLibrary *old = plugDict[*f];
	    if ( !old ) {
		useful = true;
		plugDict.replace( *f, plugin );
	    } else {
		// we have old *and* plugin, which one to pick?
		TQComLibrary* first = (TQComLibrary*)old;
		TQComLibrary* second = (TQComLibrary*)plugin;
		bool takeFirst = true;
		if ( first->qtVersion() != TQT_VERSION ) {
		    if ( second->qtVersion() == TQT_VERSION )
			takeFirst = false;
		    else if ( second->qtVersion() < TQT_VERSION &&
			      first->qtVersion() > TQT_VERSION )
			takeFirst = false;
		}
		if ( !takeFirst ) {
		    useful = true;
		    plugDict.replace( *f, plugin );
		    tqWarning("%s: Discarding feature %s in %s!",
			     (const char*) TQFile::encodeName( plugin->library()),
			     (*f).latin1(),
			     (const char*) TQFile::encodeName( old->library() ) );
		} else {
		    tqWarning("%s: Feature %s already defined in %s!",
			     (const char*) TQFile::encodeName( old->library() ),
			     (*f).latin1(),
			     (const char*) TQFile::encodeName( plugin->library() ) );
		}
	    }
	}
	if ( fliFace )
	    fliFace->release();
	if ( cpiFace )
	    cpiFace->release();
	iFace->release();
    }

    if ( useful ) {
	libDict.replace( plugin->library(), plugin );
	if ( !libList.contains( plugin->library() ) )
	    libList.append( plugin->library() );
	return true;
    }
    delete plugin;
    return false;
}


bool TQGPluginManager::enabled() const
{
#ifdef QT_SHARED
    return true;
#else
    return false;
#endif
}

TQRESULT TQGPluginManager::queryUnknownInterface(const TQString& feature, TQUnknownInterface** iface) const
{
    TQComLibrary* plugin = 0;
    plugin = (TQComLibrary*)library( feature );
    return plugin ? plugin->queryInterface( interfaceId, (TQUnknownInterface**)iface ) : TQE_NOINTERFACE;
}

#endif //TQT_NO_COMPONENT
