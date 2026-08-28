/****************************************************************************
**
** Implementation of TQHostAddress class.
**
** Created : 979899
**
** Copyright (C) 1997-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of the network module of the TQt GUI Toolkit.
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

#include "ntqhostaddress.h"
#include "ntqstringlist.h"

#ifndef TQT_NO_NETWORK
class TQHostAddressPrivate
{
public:
    TQHostAddressPrivate( TQ_UINT32 a_=0 ) : a(a_), isIp4(true)
    {
    }
    TQHostAddressPrivate( TQ_UINT8 *a_ );
    TQHostAddressPrivate(const Q_IPV6ADDR &a_);
    ~TQHostAddressPrivate()
    {
    }

    TQHostAddressPrivate & operator=( const TQHostAddressPrivate &from )
    {
	a = from.a;
	isIp4 = from.isIp4;
	a6 = from.a6;
	return *this;
    }

private:
    TQ_UINT32 a;     // ip 4 address
    Q_IPV6ADDR a6; // ip 6 address
    bool isIp4;

    friend class TQHostAddress;
};

TQHostAddressPrivate::TQHostAddressPrivate(TQ_UINT8 *a_) : a(0), isIp4(false)
{
    for ( int i=0; i<16; i++ ) {
	a6.c[i] = a_[i];
    }
}

TQHostAddressPrivate::TQHostAddressPrivate(const Q_IPV6ADDR &a_) : a(0), isIp4(false)
{
    a6 = a_;
}

/*!
    \class TQHostAddress ntqhostaddress.h
    \brief The TQHostAddress class provides an IP address.
\if defined(commercial)
    It is part of the <a href="commercialeditions.html">TQt Enterprise Edition</a>.
\endif

    \ingroup io
    \module network

    This class contains an IP address in a platform and protocol
    independent manner. It stores both IPv4 and IPv6 addresses in a
    way that you can easily access on any platform.

    TQHostAddress is normally used with the classes TQSocket,
    TQServerSocket and TQSocketDevice to set up a server or to connect
    to a host.

    Host addresses may be set with setAddress() and retrieved with
    ip4Addr() or toString().

    \sa TQSocket, TQServerSocket, TQSocketDevice
*/


/*!
    Creates a host address object with the IP address 0.0.0.0.
*/
TQHostAddress::TQHostAddress()
    : d( new TQHostAddressPrivate )
{
}


/*!
    Creates a host address object for the IPv4 address \a ip4Addr.
*/
TQHostAddress::TQHostAddress( TQ_UINT32 ip4Addr )
    : d( new TQHostAddressPrivate( ip4Addr ) )
{
}


/*!
    Creates a host address object with the specified IPv6 address.

    \a ip6Addr must be a 16 byte array in network byte order
    (high-order byte first).
*/
TQHostAddress::TQHostAddress( TQ_UINT8 *ip6Addr )
    : d( new TQHostAddressPrivate( ip6Addr ) )
{
}

/*!
    Creates a host address object with the IPv6 address, \a ip6Addr.
*/
TQHostAddress::TQHostAddress(const Q_IPV6ADDR &ip6Addr)
    : d(new TQHostAddressPrivate(ip6Addr))
{
}

// ### DOC: Can only make this public if we specify precisely the
// format of the address string.
/*!
    \internal
*/
TQHostAddress::TQHostAddress(const TQString &address)
    : d( new TQHostAddressPrivate )
{
    setAddress( address );
}

/*!
    Creates a copy of \a address.
*/
TQHostAddress::TQHostAddress( const TQHostAddress &address )
    : d( new TQHostAddressPrivate )
{
    *d = *(address.d);
}


/*!
    Destroys the host address object.
*/
TQHostAddress::~TQHostAddress()
{
    delete d;
}


/*!
    Assigns another host address object \a address to this object and
    returns a reference to this object.
*/
TQHostAddress & TQHostAddress::operator=( const TQHostAddress & address )
{
    *d = *(address.d);
    return *this;
}


/*!
    Set the IPv4 address specified by \a ip4Addr.
*/
void TQHostAddress::setAddress( TQ_UINT32 ip4Addr )
{
    delete d;
    d = new TQHostAddressPrivate( ip4Addr );
}


/*!
    \overload

    Set the IPv6 address specified by \a ip6Addr.

    \a ip6Addr must be a 16 byte array in network byte order
    (high-order byte first).
*/
void TQHostAddress::setAddress( TQ_UINT8 *ip6Addr )
{
    delete d;
    d = new TQHostAddressPrivate( ip6Addr );
}

#ifndef TQT_NO_STRINGLIST
static bool parseIp4(const TQString& address, TQ_UINT32 *addr)
{
    TQStringList ipv4 = TQStringList::split(".", address, false);
    if (ipv4.count() == 4) {
	int i = 0;
	bool ok = true;
	while(ok && i < 4) {
	    uint byteValue = ipv4[i].toUInt(&ok);
	    if (byteValue > 255)
		ok = false;
	    if (ok)
		*addr = (*addr << 8) + byteValue;
	    ++i;
	}
	if (ok)
	    return true;
    }
    return false;
}

/*!
    \overload

    Sets the IPv4 or IPv6 address specified by the string
    representation \a address (e.g. "127.0.0.1"). Returns true and
    sets the address if the address was successfully parsed; otherwise
    returns false and leaves the address unchanged.
*/
bool TQHostAddress::setAddress(const TQString& address)
{
    TQString a = address.simplifyWhiteSpace();

    // try ipv4
    TQ_UINT32 maybeIp4 = 0;
    if (parseIp4(address, &maybeIp4)) {
	setAddress(maybeIp4);
	return true;
    }

    // try ipv6
    TQStringList ipv6 = TQStringList::split(":", a, true);
    int count = (int)ipv6.count();
    if (count < 3)
	return false; // there must be at least two ":"
    if (count > 8)
	return false; // maximum of seven ":" exceeded
    TQ_UINT8 maybeIp6[16];
    int mc = 16;
    int fillCount = 9 - count;
    for (int i=count-1; i>=0; --i) {
	if ( mc <= 0 )
	    return false;

	if (ipv6[i].isEmpty()) {
	    if (i==count-1) {
		// special case: ":" is last character
		if (!ipv6[i-1].isEmpty())
		    return false;
		maybeIp6[--mc] = 0;
		maybeIp6[--mc] = 0;
	    } else if (i==0) {
		// special case: ":" is first character
		if (!ipv6[i+1].isEmpty())
		    return false;
		maybeIp6[--mc] = 0;
		maybeIp6[--mc] = 0;
	    } else {
		for (int j=0; j<fillCount; ++j) {
		    if ( mc <= 0 )
			return false;
		    maybeIp6[--mc] = 0;
		    maybeIp6[--mc] = 0;
		}
	    }
	} else {
	    bool ok = false;
	    uint byteValue = ipv6[i].toUInt(&ok, 16);
	    if (ok && byteValue <= 0xffff) {
		maybeIp6[--mc] = byteValue & 0xff;
		maybeIp6[--mc] = (byteValue >> 8) & 0xff;
	    } else {
		if (i == count-1) {
		    // parse the ipv4 part of a mixed type
		    if (!parseIp4(ipv6[i], &maybeIp4))
			return false;
		    maybeIp6[--mc] = maybeIp4 & 0xff;
		    maybeIp6[--mc] = (maybeIp4 >> 8) & 0xff;
		    maybeIp6[--mc] = (maybeIp4 >> 16) & 0xff;
		    maybeIp6[--mc] = (maybeIp4 >> 24) & 0xff;
		    --fillCount;
		} else {
		    return false;
		}
	    }
	}
    }
    if (mc == 0) {
	setAddress(maybeIp6);
	return true;
    }

    return false;
}
#endif

/*!
    \obsolete

    Use isIPv4Address() instead.
*/
bool TQHostAddress::isIp4Addr() const
{
    return isIPv4Address();
}

/*!
    Returns true if the host address represents an IPv4 address;
    otherwise returns false.
*/
bool TQHostAddress::isIPv4Address() const
{
    return d->isIp4;
}

/*!
    \obsolete

    Use toIPv4Address() instead.
*/
TQ_UINT32 TQHostAddress::ip4Addr() const
{
    return toIPv4Address();
}

/*!
    Returns the IPv4 address as a number.

    For example, if the address is 127.0.0.1, the returned value is
    2130706433 (i.e. 0x7f000001).

    This value is only valid when isIp4Addr() returns true.

    \sa toString()
*/
TQ_UINT32 TQHostAddress::toIPv4Address() const
{
    return d->a;
}

/*!
    Returns true if the host address represents an IPv6 address;
    otherwise returns false.
*/
bool TQHostAddress::isIPv6Address() const
{
    return !d->isIp4;
}

/*!
    Returns the IPv6 address as a Q_IPV6ADDR structure. The structure
    consists of 16 unsigned characters.

    \code
        Q_IPV6ADDR addr = hostAddr.ip6Addr();
        // addr.c[] contains 16 unsigned characters

        for (int i = 0; i < 16; ++i) {
            // process addr.c[i]
        }
    \endcode

    This value is only valid when isIPv6Address() returns true.

    \sa toString()
*/
Q_IPV6ADDR TQHostAddress::toIPv6Address() const
{
    return d->a6;
}

#ifndef TQT_NO_SPRINTF
/*!
    Returns the address as a string.

    For example, if the address is the IPv4 address 127.0.0.1, the
    returned string is "127.0.0.1".

    \sa ip4Addr()
*/
TQString TQHostAddress::toString() const
{
    if ( d->isIp4 ) {
	TQ_UINT32 i = ip4Addr();
	TQString s;
	s.sprintf( "%d.%d.%d.%d", (i>>24) & 0xff, (i>>16) & 0xff,
		(i >> 8) & 0xff, i & 0xff );
	return s;
    } else {
	TQ_UINT16 ugle[8];
	for ( int i=0; i<8; i++ ) {
	    ugle[i] = ( (TQ_UINT16)( d->a6.c[2*i] ) << 8 ) |
		( (TQ_UINT16)( d->a6.c[2*i+1] ) );
	}
	TQString s;
	s.sprintf( "%X:%X:%X:%X:%X:%X:%X:%X",
		ugle[0], ugle[1], ugle[2], ugle[3],
		ugle[4], ugle[5], ugle[6], ugle[7] );
	return s;
    }
}
#endif


/*!
    Returns true if this host address is the same as \a other;
    otherwise returns false.
*/
bool TQHostAddress::operator==( const TQHostAddress & other ) const
{
    return d->a == other.d->a;
}


/*!
    Returns true if this host address is null (INADDR_ANY or in6addr_any). The
    default constructor creates a null address, and that address isn't valid
    for any particular host or interface.
*/
bool TQHostAddress::isNull() const
{
    if ( d->isIp4 )
	return d->a == 0;
    int i = 0;
    while( i < 16 ) {
	if ( d->a6.c[i++] != 0 )
	    return false;
    }
    return true;
}

#endif //TQT_NO_NETWORK
