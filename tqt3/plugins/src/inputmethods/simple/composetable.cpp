/****************************************************************************
** $Id$
**
** Compose table for TQSimpleInputContext class
**
** Copyright (C) 2004 immodule for TQt Project.  All rights reserved.
**
** This file is written to contribute to Trolltech AS under their own
** licence. You may use this file under your TQt license. Following
** description is copied from their original file headers. Contact
** immodule-qt@freedesktop.org if any conditions of this licensing are
** not clear to you.
**
**
** This file is part of the input method module of the TQt GUI Toolkit.
**
** This file may be distributed under the terms of the Q Public License
** as defined by Trolltech AS of Norway and appearing in the file
** LICENSE.TQPL included in the packaging of this file.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Licensees holding valid TQt Enterprise Edition or TQt Professional Edition
** licenses may use this file in accordance with the TQt Commercial License
** Agreement provided with the Software.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/pricing.html or email sales@trolltech.com for
**   information about TQt Commercial License Agreements.
** See http://www.trolltech.com/qpl/ for TQPL licensing information.
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

// The compose table included in file is converted from a Compose file
// of X.org's X11R6.7.0. Original header is follows

// UTF-8 (Unicode) compose sequence
// David.Monniaux@ens.fr
//
// $XFree86: xc/nls/Compose/en_US.UTF-8,v 1.11 2004/01/06 13:14:04 pascal Exp $


#include "qsimpleinputcontext.h"

#include <ntqnamespace.h>

static const TQComposeTableElement defaultTable[] = {
// /* broken */    { {0x0000, 0x0000, 0, 0, 0, 0}, 0x1eda }, // U1EDA # LATIN CAPITAL LETTER O WITH HORN AND ACUTE
// /* broken */    { {0x0000, 0x0000, 0, 0, 0, 0}, 0x1edb }, // U1EDB # LATIN SMALL LETTER O WITH HORN AND ACUTE
// /* broken */    { {0x0000, 0x0000, 0, 0, 0, 0}, 0x1edc }, // U1EDC # LATIN CAPITAL LETTER O WITH HORN AND GRAVE
// /* broken */    { {0x0000, 0x0000, 0, 0, 0, 0}, 0x1edd }, // U1EDD # LATIN SMALL LETTER O WITH HORN AND GRAVE
// /* broken */    { {0x0000, 0x0000, 0, 0, 0, 0}, 0x1ede }, // U1EDE # LATIN CAPITAL LETTER O WITH HORN AND HOOK ABOVE
// /* broken */    { {0x0000, 0x0000, 0, 0, 0, 0}, 0x1edf }, // U1EDF # LATIN SMALL LETTER O WITH HORN AND HOOK ABOVE
// /* broken */    { {0x0000, 0x0000, 0, 0, 0, 0}, 0x1ee3 }, // U1EE3 # LATIN SMALL LETTER O WITH HORN AND DOT BELOW
// /* broken */    { {0x0000, 0x0000, 0, 0, 0, 0}, 0x1ee1 }, // U1EE1 # LATIN SMALL LETTER O WITH HORN AND TILDE
// /* broken */    { {0x0000, 0x0000, 0, 0, 0, 0}, 0x1ee2 }, // U1EE2 # LATIN CAPITAL LETTER O WITH HORN AND DOT BELOW
// /* broken */    { {0x0000, 0x0000, 0, 0, 0, 0}, 0x1ef1 }, // U1EF1 # LATIN SMALL LETTER U WITH HORN AND DOT BELOW
// /* broken */    { {0x0000, 0x0000, 0, 0, 0, 0}, 0x1ef0 }, // U1EF0 # LATIN CAPITAL LETTER U WITH HORN AND DOT BELOW
// /* broken */    { {0x0000, 0x0000, 0, 0, 0, 0}, 0x1eef }, // U1EEF # LATIN SMALL LETTER U WITH HORN AND TILDE
// /* broken */    { {0x0000, 0x0000, 0, 0, 0, 0}, 0x1eee }, // U1EEE # LATIN CAPITAL LETTER U WITH HORN AND TILDE
// /* broken */    { {0x0000, 0x0000, 0, 0, 0, 0}, 0x1eed }, // U1EED # LATIN SMALL LETTER U WITH HORN AND HOOK ABOVE
// /* broken */    { {0x0000, 0x0000, 0, 0, 0, 0}, 0x1eec }, // U1EEC # LATIN CAPITAL LETTER U WITH HORN AND HOOK ABOVE
// /* broken */    { {0x0000, 0x0000, 0, 0, 0, 0}, 0x1eeb }, // U1EEB # LATIN SMALL LETTER U WITH HORN AND GRAVE
// /* broken */    { {0x0000, 0x0000, 0, 0, 0, 0}, 0x1eea }, // U1EEA # LATIN CAPITAL LETTER U WITH HORN AND GRAVE
// /* broken */    { {0x0000, 0x0000, 0, 0, 0, 0}, 0x1ee9 }, // U1EE9 # LATIN SMALL LETTER U WITH HORN AND ACUTE
// /* broken */    { {0x0000, 0x0000, 0, 0, 0, 0}, 0x1ee8 }, // U1EE8 # LATIN CAPITAL LETTER U WITH HORN AND ACUTE
// /* broken */    { {0x0000, 0x0000, 0, 0, 0, 0}, 0x1ee0 }, // U1EE0 # LATIN CAPITAL LETTER O WITH HORN AND TILDE
// /* broken */    { {0x0000, 0x0000, 0x004f, 0, 0, 0}, 0x1e4c }, // U1E4C # LATIN CAPITAL LETTER O WITH TILDE AND ACUTE
// /* broken */    { {0x0000, 0x0000, 0x0055, 0, 0, 0}, 0x1e78 }, // U1E78 # LATIN CAPITAL LETTER U WITH TILDE AND ACUTE
// /* broken */    { {0x0000, 0x0000, 0x006f, 0, 0, 0}, 0x1e4d }, // U1E4D # LATIN SMALL LETTER O WITH TILDE AND ACUTE
// /* broken */    { {0x0000, 0x0000, 0x0075, 0, 0, 0}, 0x1e79 }, // U1E79 # LATIN SMALL LETTER U WITH TILDE AND ACUTE
// /* broken */    { {0x0000, 0x0041, 0, 0, 0, 0}, 0x00c1 }, // U00C1 # LATIN CAPITAL LETTER A WITH ACUTE
// /* broken */    { {0x0000, 0x0041, 0, 0, 0, 0}, 0x00c0 }, // U00C0 # LATIN CAPITAL LETTER A WITH GRAVE
// /* broken */    { {0x0000, 0x0041, 0, 0, 0, 0}, 0x00c3 }, // U00C3 # LATIN CAPITAL LETTER A WITH TILDE
// /* broken */    { {0x0000, 0x0041, 0, 0, 0, 0}, 0x1ea2 }, // U1EA2 # LATIN CAPITAL LETTER A WITH HOOK ABOVE
// /* broken */    { {0x0000, 0x0041, 0, 0, 0, 0}, 0x1ea0 }, // U1EA0 # LATIN CAPITAL LETTER A WITH DOT BELOW
// /* broken */    { {0x0000, 0x0042, 0, 0, 0, 0}, 0x1e04 }, // U1E04 # LATIN CAPITAL LETTER B WITH DOT BELOW
// /* broken */    { {0x0000, 0x0043, 0, 0, 0, 0}, 0x0106 }, // U0106 # LATIN CAPITAL LETTER C WITH ACUTE
// /* broken */    { {0x0000, 0x0044, 0, 0, 0, 0}, 0x1e0c }, // U1E0C # LATIN CAPITAL LETTER D WITH DOT BELOW
// /* broken */    { {0x0000, 0x0045, 0, 0, 0, 0}, 0x1ebc }, // U1EBC # LATIN CAPITAL LETTER E WITH TILDE
// /* broken */    { {0x0000, 0x0045, 0, 0, 0, 0}, 0x1eb8 }, // U1EB8 # LATIN CAPITAL LETTER E WITH DOT BELOW
// /* broken */    { {0x0000, 0x0045, 0, 0, 0, 0}, 0x00c9 }, // U00C9 # LATIN CAPITAL LETTER E WITH ACUTE
// /* broken */    { {0x0000, 0x0045, 0, 0, 0, 0}, 0x00c8 }, // U00C8 # LATIN CAPITAL LETTER E WITH GRAVE
// /* broken */    { {0x0000, 0x0045, 0, 0, 0, 0}, 0x1eba }, // U1EBA # LATIN CAPITAL LETTER E WITH HOOK ABOVE
// /* broken */    { {0x0000, 0x0047, 0, 0, 0, 0}, 0x01f4 }, // U01F4 # LATIN CAPITAL LETTER G WITH ACUTE
// /* broken */    { {0x0000, 0x0048, 0, 0, 0, 0}, 0x1e24 }, // U1E24 # LATIN CAPITAL LETTER H WITH DOT BELOW
// /* broken */    { {0x0000, 0x0049, 0, 0, 0, 0}, 0x00cd }, // U00CD # LATIN CAPITAL LETTER I WITH ACUTE
// /* broken */    { {0x0000, 0x0049, 0, 0, 0, 0}, 0x00cc }, // U00CC # LATIN CAPITAL LETTER I WITH GRAVE
// /* broken */    { {0x0000, 0x0049, 0, 0, 0, 0}, 0x1ec8 }, // U1EC8 # LATIN CAPITAL LETTER I WITH HOOK ABOVE
// /* broken */    { {0x0000, 0x0049, 0, 0, 0, 0}, 0x0128 }, // U0128 # LATIN CAPITAL LETTER I WITH TILDE
// /* broken */    { {0x0000, 0x0049, 0, 0, 0, 0}, 0x1eca }, // U1ECA # LATIN CAPITAL LETTER I WITH DOT BELOW
// /* broken */    { {0x0000, 0x004b, 0, 0, 0, 0}, 0x1e30 }, // U1E30 # LATIN CAPITAL LETTER K WITH ACUTE
// /* broken */    { {0x0000, 0x004b, 0, 0, 0, 0}, 0x1e32 }, // U1E32 # LATIN CAPITAL LETTER K WITH DOT BELOW
// /* broken */    { {0x0000, 0x004c, 0, 0, 0, 0}, 0x0139 }, // U0139 # LATIN CAPITAL LETTER L WITH ACUTE
// /* broken */    { {0x0000, 0x004c, 0, 0, 0, 0}, 0x1e36 }, // U1E36 # LATIN CAPITAL LETTER L WITH DOT BELOW
// /* broken */    { {0x0000, 0x004d, 0, 0, 0, 0}, 0x1e42 }, // U1E42 # LATIN CAPITAL LETTER M WITH DOT BELOW
// /* broken */    { {0x0000, 0x004d, 0, 0, 0, 0}, 0x1e3e }, // U1E3E # LATIN CAPITAL LETTER M WITH ACUTE
// /* broken */    { {0x0000, 0x004e, 0, 0, 0, 0}, 0x1e46 }, // U1E46 # LATIN CAPITAL LETTER N WITH DOT BELOW
// /* broken */    { {0x0000, 0x004e, 0, 0, 0, 0}, 0x0143 }, // U0143 # LATIN CAPITAL LETTER N WITH ACUTE
// /* broken */    { {0x0000, 0x004e, 0, 0, 0, 0}, 0x01f8 }, // U01F8 # LATIN CAPITAL LETTER N WITH GRAVE
// /* broken */    { {0x0000, 0x004e, 0, 0, 0, 0}, 0x00d1 }, // U00D1 # LATIN CAPITAL LETTER N WITH TILDE
// /* broken */    { {0x0000, 0x004f, 0, 0, 0, 0}, 0x00d5 }, // U00D5 # LATIN CAPITAL LETTER O WITH TILDE
// /* broken */    { {0x0000, 0x004f, 0, 0, 0, 0}, 0x00d3 }, // U00D3 # LATIN CAPITAL LETTER O WITH ACUTE
// /* broken */    { {0x0000, 0x004f, 0, 0, 0, 0}, 0x1ecc }, // U1ECC # LATIN CAPITAL LETTER O WITH DOT BELOW
// /* broken */    { {0x0000, 0x004f, 0, 0, 0, 0}, 0x00d2 }, // U00D2 # LATIN CAPITAL LETTER O WITH GRAVE
// /* broken */    { {0x0000, 0x004f, 0, 0, 0, 0}, 0x1ece }, // U1ECE # LATIN CAPITAL LETTER O WITH HOOK ABOVE
// /* broken */    { {0x0000, 0x0050, 0, 0, 0, 0}, 0x1e54 }, // U1E54 # LATIN CAPITAL LETTER P WITH ACUTE
// /* broken */    { {0x0000, 0x0052, 0, 0, 0, 0}, 0x1e5a }, // U1E5A # LATIN CAPITAL LETTER R WITH DOT BELOW
// /* broken */    { {0x0000, 0x0052, 0, 0, 0, 0}, 0x0154 }, // U0154 # LATIN CAPITAL LETTER R WITH ACUTE
// /* broken */    { {0x0000, 0x0053, 0, 0, 0, 0}, 0x015a }, // U015A # LATIN CAPITAL LETTER S WITH ACUTE
// /* broken */    { {0x0000, 0x0053, 0, 0, 0, 0}, 0x1e62 }, // U1E62 # LATIN CAPITAL LETTER S WITH DOT BELOW
// /* broken */    { {0x0000, 0x0054, 0, 0, 0, 0}, 0x1e6c }, // U1E6C # LATIN CAPITAL LETTER T WITH DOT BELOW
// /* broken */    { {0x0000, 0x0055, 0, 0, 0, 0}, 0x00da }, // U00DA # LATIN CAPITAL LETTER U WITH ACUTE
// /* broken */    { {0x0000, 0x0055, 0, 0, 0, 0}, 0x00d9 }, // U00D9 # LATIN CAPITAL LETTER U WITH GRAVE
// /* broken */    { {0x0000, 0x0055, 0, 0, 0, 0}, 0x0168 }, // U0168 # LATIN CAPITAL LETTER U WITH TILDE
// /* broken */    { {0x0000, 0x0055, 0, 0, 0, 0}, 0x1ee4 }, // U1EE4 # LATIN CAPITAL LETTER U WITH DOT BELOW
// /* broken */    { {0x0000, 0x0055, 0, 0, 0, 0}, 0x1ee6 }, // U1EE6 # LATIN CAPITAL LETTER U WITH HOOK ABOVE
// /* broken */    { {0x0000, 0x0056, 0, 0, 0, 0}, 0x1e7e }, // U1E7E # LATIN CAPITAL LETTER V WITH DOT BELOW
// /* broken */    { {0x0000, 0x0056, 0, 0, 0, 0}, 0x1e7c }, // U1E7C # LATIN CAPITAL LETTER V WITH TILDE
// /* broken */    { {0x0000, 0x0057, 0, 0, 0, 0}, 0x1e82 }, // U1E82 # LATIN CAPITAL LETTER W WITH ACUTE
// /* broken */    { {0x0000, 0x0057, 0, 0, 0, 0}, 0x1e88 }, // U1E88 # LATIN CAPITAL LETTER W WITH DOT BELOW
// /* broken */    { {0x0000, 0x0057, 0, 0, 0, 0}, 0x1e80 }, // U1E80 # LATIN CAPITAL LETTER W WITH GRAVE
// /* broken */    { {0x0000, 0x0059, 0, 0, 0, 0}, 0x1ef8 }, // U1EF8 # LATIN CAPITAL LETTER Y WITH TILDE
// /* broken */    { {0x0000, 0x0059, 0, 0, 0, 0}, 0x1ef2 }, // U1EF2 # LATIN CAPITAL LETTER Y WITH GRAVE
// /* broken */    { {0x0000, 0x0059, 0, 0, 0, 0}, 0x00dd }, // U00DD # LATIN CAPITAL LETTER Y WITH ACUTE
// /* broken */    { {0x0000, 0x0059, 0, 0, 0, 0}, 0x1ef6 }, // U1EF6 # LATIN CAPITAL LETTER Y WITH HOOK ABOVE
// /* broken */    { {0x0000, 0x0059, 0, 0, 0, 0}, 0x1ef4 }, // U1EF4 # LATIN CAPITAL LETTER Y WITH DOT BELOW
// /* broken */    { {0x0000, 0x005a, 0, 0, 0, 0}, 0x0179 }, // U0179 # LATIN CAPITAL LETTER Z WITH ACUTE
// /* broken */    { {0x0000, 0x005a, 0, 0, 0, 0}, 0x1e92 }, // U1E92 # LATIN CAPITAL LETTER Z WITH DOT BELOW
// /* broken */    { {0x0000, 0x0061, 0, 0, 0, 0}, 0x1ea1 }, // U1EA1 # LATIN SMALL LETTER A WITH DOT BELOW
// /* broken */    { {0x0000, 0x0061, 0, 0, 0, 0}, 0x1ea3 }, // U1EA3 # LATIN SMALL LETTER A WITH HOOK ABOVE
// /* broken */    { {0x0000, 0x0061, 0, 0, 0, 0}, 0x00e0 }, // U00E0 # LATIN SMALL LETTER A WITH GRAVE
// /* broken */    { {0x0000, 0x0061, 0, 0, 0, 0}, 0x00e3 }, // U00E3 # LATIN SMALL LETTER A WITH TILDE
// /* broken */    { {0x0000, 0x0061, 0, 0, 0, 0}, 0x00e1 }, // U00E1 # LATIN SMALL LETTER A WITH ACUTE
// /* broken */    { {0x0000, 0x0062, 0, 0, 0, 0}, 0x1e05 }, // U1E05 # LATIN SMALL LETTER B WITH DOT BELOW
// /* broken */    { {0x0000, 0x0063, 0, 0, 0, 0}, 0x0107 }, // U0107 # LATIN SMALL LETTER C WITH ACUTE
// /* broken */    { {0x0000, 0x0064, 0, 0, 0, 0}, 0x1e0d }, // U1E0D # LATIN SMALL LETTER D WITH DOT BELOW
// /* broken */    { {0x0000, 0x0065, 0, 0, 0, 0}, 0x00e8 }, // U00E8 # LATIN SMALL LETTER E WITH GRAVE
// /* broken */    { {0x0000, 0x0065, 0, 0, 0, 0}, 0x1ebb }, // U1EBB # LATIN SMALL LETTER E WITH HOOK ABOVE
// /* broken */    { {0x0000, 0x0065, 0, 0, 0, 0}, 0x1eb9 }, // U1EB9 # LATIN SMALL LETTER E WITH DOT BELOW
// /* broken */    { {0x0000, 0x0065, 0, 0, 0, 0}, 0x00e9 }, // U00E9 # LATIN SMALL LETTER E WITH ACUTE
// /* broken */    { {0x0000, 0x0065, 0, 0, 0, 0}, 0x1ebd }, // U1EBD # LATIN SMALL LETTER E WITH TILDE
// /* broken */    { {0x0000, 0x0067, 0, 0, 0, 0}, 0x01f5 }, // U01F5 # LATIN SMALL LETTER G WITH ACUTE
// /* broken */    { {0x0000, 0x0068, 0, 0, 0, 0}, 0x1e25 }, // U1E25 # LATIN SMALL LETTER H WITH DOT BELOW
// /* broken */    { {0x0000, 0x0069, 0, 0, 0, 0}, 0x00ec }, // U00EC # LATIN SMALL LETTER I WITH GRAVE
// /* broken */    { {0x0000, 0x0069, 0, 0, 0, 0}, 0x0129 }, // U0129 # LATIN SMALL LETTER I WITH TILDE
// /* broken */    { {0x0000, 0x0069, 0, 0, 0, 0}, 0x1ecb }, // U1ECB # LATIN SMALL LETTER I WITH DOT BELOW
// /* broken */    { {0x0000, 0x0069, 0, 0, 0, 0}, 0x00ed }, // U00ED # LATIN SMALL LETTER I WITH ACUTE
// /* broken */    { {0x0000, 0x0069, 0, 0, 0, 0}, 0x1ec9 }, // U1EC9 # LATIN SMALL LETTER I WITH HOOK ABOVE
// /* broken */    { {0x0000, 0x006b, 0, 0, 0, 0}, 0x1e33 }, // U1E33 # LATIN SMALL LETTER K WITH DOT BELOW
// /* broken */    { {0x0000, 0x006b, 0, 0, 0, 0}, 0x1e31 }, // U1E31 # LATIN SMALL LETTER K WITH ACUTE
// /* broken */    { {0x0000, 0x006c, 0, 0, 0, 0}, 0x1e37 }, // U1E37 # LATIN SMALL LETTER L WITH DOT BELOW
// /* broken */    { {0x0000, 0x006c, 0, 0, 0, 0}, 0x013a }, // U013A # LATIN SMALL LETTER L WITH ACUTE
// /* broken */    { {0x0000, 0x006d, 0, 0, 0, 0}, 0x1e43 }, // U1E43 # LATIN SMALL LETTER M WITH DOT BELOW
// /* broken */    { {0x0000, 0x006d, 0, 0, 0, 0}, 0x1e3f }, // U1E3F # LATIN SMALL LETTER M WITH ACUTE
// /* broken */    { {0x0000, 0x006e, 0, 0, 0, 0}, 0x00f1 }, // U00F1 # LATIN SMALL LETTER N WITH TILDE
// /* broken */    { {0x0000, 0x006e, 0, 0, 0, 0}, 0x0144 }, // U0144 # LATIN SMALL LETTER N WITH ACUTE
// /* broken */    { {0x0000, 0x006e, 0, 0, 0, 0}, 0x01f9 }, // U01F9 # LATIN SMALL LETTER N WITH GRAVE
// /* broken */    { {0x0000, 0x006e, 0, 0, 0, 0}, 0x1e47 }, // U1E47 # LATIN SMALL LETTER N WITH DOT BELOW
// /* broken */    { {0x0000, 0x006f, 0, 0, 0, 0}, 0x00f5 }, // U00F5 # LATIN SMALL LETTER O WITH TILDE
// /* broken */    { {0x0000, 0x006f, 0, 0, 0, 0}, 0x00f2 }, // U00F2 # LATIN SMALL LETTER O WITH GRAVE
// /* broken */    { {0x0000, 0x006f, 0, 0, 0, 0}, 0x1ecf }, // U1ECF # LATIN SMALL LETTER O WITH HOOK ABOVE
// /* broken */    { {0x0000, 0x006f, 0, 0, 0, 0}, 0x00f3 }, // U00F3 # LATIN SMALL LETTER O WITH ACUTE
// /* broken */    { {0x0000, 0x006f, 0, 0, 0, 0}, 0x1ecd }, // U1ECD # LATIN SMALL LETTER O WITH DOT BELOW
// /* broken */    { {0x0000, 0x0070, 0, 0, 0, 0}, 0x1e55 }, // U1E55 # LATIN SMALL LETTER P WITH ACUTE
// /* broken */    { {0x0000, 0x0072, 0, 0, 0, 0}, 0x0155 }, // U0155 # LATIN SMALL LETTER R WITH ACUTE
// /* broken */    { {0x0000, 0x0072, 0, 0, 0, 0}, 0x1e5b }, // U1E5B # LATIN SMALL LETTER R WITH DOT BELOW
// /* broken */    { {0x0000, 0x0073, 0, 0, 0, 0}, 0x1e63 }, // U1E63 # LATIN SMALL LETTER S WITH DOT BELOW
// /* broken */    { {0x0000, 0x0073, 0, 0, 0, 0}, 0x015b }, // U015B # LATIN SMALL LETTER S WITH ACUTE
// /* broken */    { {0x0000, 0x0074, 0, 0, 0, 0}, 0x1e6d }, // U1E6D # LATIN SMALL LETTER T WITH DOT BELOW
// /* broken */    { {0x0000, 0x0075, 0, 0, 0, 0}, 0x0169 }, // U0169 # LATIN SMALL LETTER U WITH TILDE
// /* broken */    { {0x0000, 0x0075, 0, 0, 0, 0}, 0x1ee7 }, // U1EE7 # LATIN SMALL LETTER U WITH HOOK ABOVE
// /* broken */    { {0x0000, 0x0075, 0, 0, 0, 0}, 0x00f9 }, // U00F9 # LATIN SMALL LETTER U WITH GRAVE
// /* broken */    { {0x0000, 0x0075, 0, 0, 0, 0}, 0x00fa }, // U00FA # LATIN SMALL LETTER U WITH ACUTE
// /* broken */    { {0x0000, 0x0075, 0, 0, 0, 0}, 0x1ee5 }, // U1EE5 # LATIN SMALL LETTER U WITH DOT BELOW
// /* broken */    { {0x0000, 0x0076, 0, 0, 0, 0}, 0x1e7d }, // U1E7D # LATIN SMALL LETTER V WITH TILDE
// /* broken */    { {0x0000, 0x0076, 0, 0, 0, 0}, 0x1e7f }, // U1E7F # LATIN SMALL LETTER V WITH DOT BELOW
// /* broken */    { {0x0000, 0x0077, 0, 0, 0, 0}, 0x1e81 }, // U1E81 # LATIN SMALL LETTER W WITH GRAVE
// /* broken */    { {0x0000, 0x0077, 0, 0, 0, 0}, 0x1e83 }, // U1E83 # LATIN SMALL LETTER W WITH ACUTE
// /* broken */    { {0x0000, 0x0077, 0, 0, 0, 0}, 0x1e89 }, // U1E89 # LATIN SMALL LETTER W WITH DOT BELOW
// /* broken */    { {0x0000, 0x0079, 0, 0, 0, 0}, 0x1ef5 }, // U1EF5 # LATIN SMALL LETTER Y WITH DOT BELOW
// /* broken */    { {0x0000, 0x0079, 0, 0, 0, 0}, 0x1ef9 }, // U1EF9 # LATIN SMALL LETTER Y WITH TILDE
// /* broken */    { {0x0000, 0x0079, 0, 0, 0, 0}, 0x1ef7 }, // U1EF7 # LATIN SMALL LETTER Y WITH HOOK ABOVE
// /* broken */    { {0x0000, 0x0079, 0, 0, 0, 0}, 0x1ef3 }, // U1EF3 # LATIN SMALL LETTER Y WITH GRAVE
// /* broken */    { {0x0000, 0x0079, 0, 0, 0, 0}, 0x00fd }, // U00FD # LATIN SMALL LETTER Y WITH ACUTE
// /* broken */    { {0x0000, 0x007a, 0, 0, 0, 0}, 0x1e93 }, // U1E93 # LATIN SMALL LETTER Z WITH DOT BELOW
// /* broken */    { {0x0000, 0x007a, 0, 0, 0, 0}, 0x017a }, // U017A # LATIN SMALL LETTER Z WITH ACUTE
// /* broken */    { {0x0000, 0x00c2, 0, 0, 0, 0}, 0x1ea6 }, // U1EA6 # LATIN CAPITAL LETTER A WITH CIRCUMFLEX AND GRAVE
// /* broken */    { {0x0000, 0x00c2, 0, 0, 0, 0}, 0x1ea8 }, // U1EA8 # LATIN CAPITAL LETTER A WITH CIRCUMFLEX AND HOOK ABOVE
// /* broken */    { {0x0000, 0x00c2, 0, 0, 0, 0}, 0x1eaa }, // U1EAA # LATIN CAPITAL LETTER A WITH CIRCUMFLEX AND TILDE
// /* broken */    { {0x0000, 0x00c2, 0, 0, 0, 0}, 0x1ea4 }, // U1EA4 # LATIN CAPITAL LETTER A WITH CIRCUMFLEX AND ACUTE
// /* broken */    { {0x0000, 0x00c5, 0, 0, 0, 0}, 0x01fa }, // U01FA # LATIN CAPITAL LETTER A WITH RING ABOVE AND ACUTE
// /* broken */    { {0x0000, 0x00c6, 0, 0, 0, 0}, 0x01fc }, // U01FC # LATIN CAPITAL LETTER AE WITH ACUTE
// /* broken */    { {0x0000, 0x00c7, 0, 0, 0, 0}, 0x1e08 }, // U1E08 # LATIN CAPITAL LETTER C WITH CEDILLA AND ACUTE
// /* broken */    { {0x0000, 0x00ca, 0, 0, 0, 0}, 0x1ec0 }, // U1EC0 # LATIN CAPITAL LETTER E WITH CIRCUMFLEX AND GRAVE
// /* broken */    { {0x0000, 0x00ca, 0, 0, 0, 0}, 0x1ebe }, // U1EBE # LATIN CAPITAL LETTER E WITH CIRCUMFLEX AND ACUTE
// /* broken */    { {0x0000, 0x00ca, 0, 0, 0, 0}, 0x1ec2 }, // U1EC2 # LATIN CAPITAL LETTER E WITH CIRCUMFLEX AND HOOK ABOVE
// /* broken */    { {0x0000, 0x00ca, 0, 0, 0, 0}, 0x1ec4 }, // U1EC4 # LATIN CAPITAL LETTER E WITH CIRCUMFLEX AND TILDE
// /* broken */    { {0x0000, 0x00cf, 0, 0, 0, 0}, 0x1e2e }, // U1E2E # LATIN CAPITAL LETTER I WITH DIAERESIS AND ACUTE
// /* broken */    { {0x0000, 0x00d4, 0, 0, 0, 0}, 0x1ed4 }, // U1ED4 # LATIN CAPITAL LETTER O WITH CIRCUMFLEX AND HOOK ABOVE
// /* broken */    { {0x0000, 0x00d4, 0, 0, 0, 0}, 0x1ed0 }, // U1ED0 # LATIN CAPITAL LETTER O WITH CIRCUMFLEX AND ACUTE
// /* broken */    { {0x0000, 0x00d4, 0, 0, 0, 0}, 0x1ed2 }, // U1ED2 # LATIN CAPITAL LETTER O WITH CIRCUMFLEX AND GRAVE
// /* broken */    { {0x0000, 0x00d4, 0, 0, 0, 0}, 0x1ed6 }, // U1ED6 # LATIN CAPITAL LETTER O WITH CIRCUMFLEX AND TILDE
// /* broken */    { {0x0000, 0x00d5, 0, 0, 0, 0}, 0x1e4c }, // U1E4C # LATIN CAPITAL LETTER O WITH TILDE AND ACUTE
// /* broken */    { {0x0000, 0x00d8, 0, 0, 0, 0}, 0x01fe }, // U01FE # LATIN CAPITAL LETTER O WITH STROKE AND ACUTE
// /* broken */    { {0x0000, 0x00dc, 0, 0, 0, 0}, 0x01d7 }, // U01D7 # LATIN CAPITAL LETTER U WITH DIAERESIS AND ACUTE
// /* broken */    { {0x0000, 0x00dc, 0, 0, 0, 0}, 0x01db }, // U01DB # LATIN CAPITAL LETTER U WITH DIAERESIS AND GRAVE
// /* broken */    { {0x0000, 0x00e2, 0, 0, 0, 0}, 0x1eab }, // U1EAB # LATIN SMALL LETTER A WITH CIRCUMFLEX AND TILDE
// /* broken */    { {0x0000, 0x00e2, 0, 0, 0, 0}, 0x1ea5 }, // U1EA5 # LATIN SMALL LETTER A WITH CIRCUMFLEX AND ACUTE
// /* broken */    { {0x0000, 0x00e2, 0, 0, 0, 0}, 0x1ea9 }, // U1EA9 # LATIN SMALL LETTER A WITH CIRCUMFLEX AND HOOK ABOVE
// /* broken */    { {0x0000, 0x00e2, 0, 0, 0, 0}, 0x1ea7 }, // U1EA7 # LATIN SMALL LETTER A WITH CIRCUMFLEX AND GRAVE
// /* broken */    { {0x0000, 0x00e5, 0, 0, 0, 0}, 0x01fb }, // U01FB # LATIN SMALL LETTER A WITH RING ABOVE AND ACUTE
// /* broken */    { {0x0000, 0x00e6, 0, 0, 0, 0}, 0x01fd }, // U01FD # LATIN SMALL LETTER AE WITH ACUTE
// /* broken */    { {0x0000, 0x00e7, 0, 0, 0, 0}, 0x1e09 }, // U1E09 # LATIN SMALL LETTER C WITH CEDILLA AND ACUTE
// /* broken */    { {0x0000, 0x00ea, 0, 0, 0, 0}, 0x1ec5 }, // U1EC5 # LATIN SMALL LETTER E WITH CIRCUMFLEX AND TILDE
// /* broken */    { {0x0000, 0x00ea, 0, 0, 0, 0}, 0x1ec1 }, // U1EC1 # LATIN SMALL LETTER E WITH CIRCUMFLEX AND GRAVE
// /* broken */    { {0x0000, 0x00ea, 0, 0, 0, 0}, 0x1ec3 }, // U1EC3 # LATIN SMALL LETTER E WITH CIRCUMFLEX AND HOOK ABOVE
// /* broken */    { {0x0000, 0x00ea, 0, 0, 0, 0}, 0x1ebf }, // U1EBF # LATIN SMALL LETTER E WITH CIRCUMFLEX AND ACUTE
// /* broken */    { {0x0000, 0x00ef, 0, 0, 0, 0}, 0x1e2f }, // U1E2F # LATIN SMALL LETTER I WITH DIAERESIS AND ACUTE
// /* broken */    { {0x0000, 0x00f4, 0, 0, 0, 0}, 0x1ed3 }, // U1ED3 # LATIN SMALL LETTER O WITH CIRCUMFLEX AND GRAVE
// /* broken */    { {0x0000, 0x00f4, 0, 0, 0, 0}, 0x1ed1 }, // U1ED1 # LATIN SMALL LETTER O WITH CIRCUMFLEX AND ACUTE
// /* broken */    { {0x0000, 0x00f4, 0, 0, 0, 0}, 0x1ed5 }, // U1ED5 # LATIN SMALL LETTER O WITH CIRCUMFLEX AND HOOK ABOVE
// /* broken */    { {0x0000, 0x00f4, 0, 0, 0, 0}, 0x1ed7 }, // U1ED7 # LATIN SMALL LETTER O WITH CIRCUMFLEX AND TILDE
// /* broken */    { {0x0000, 0x00f5, 0, 0, 0, 0}, 0x1e4d }, // U1E4D # LATIN SMALL LETTER O WITH TILDE AND ACUTE
// /* broken */    { {0x0000, 0x00f8, 0, 0, 0, 0}, 0x01ff }, // U01FF # LATIN SMALL LETTER O WITH STROKE AND ACUTE
// /* broken */    { {0x0000, 0x00fc, 0, 0, 0, 0}, 0x01dc }, // U01DC # LATIN SMALL LETTER U WITH DIAERESIS AND GRAVE
// /* broken */    { {0x0000, 0x00fc, 0, 0, 0, 0}, 0x01d8 }, // U01D8 # LATIN SMALL LETTER U WITH DIAERESIS AND ACUTE
// /* broken */    { {0x0000, 0x0102, 0, 0, 0, 0}, 0x1eb0 }, // U1EB0 # LATIN CAPITAL LETTER A WITH BREVE AND GRAVE
// /* broken */    { {0x0000, 0x0102, 0, 0, 0, 0}, 0x1eb4 }, // U1EB4 # LATIN CAPITAL LETTER A WITH BREVE AND TILDE
// /* broken */    { {0x0000, 0x0102, 0, 0, 0, 0}, 0x1eb2 }, // U1EB2 # LATIN CAPITAL LETTER A WITH BREVE AND HOOK ABOVE
// /* broken */    { {0x0000, 0x0102, 0, 0, 0, 0}, 0x1eae }, // U1EAE # LATIN CAPITAL LETTER A WITH BREVE AND ACUTE
// /* broken */    { {0x0000, 0x0103, 0, 0, 0, 0}, 0x1eb5 }, // U1EB5 # LATIN SMALL LETTER A WITH BREVE AND TILDE
// /* broken */    { {0x0000, 0x0103, 0, 0, 0, 0}, 0x1eaf }, // U1EAF # LATIN SMALL LETTER A WITH BREVE AND ACUTE
// /* broken */    { {0x0000, 0x0103, 0, 0, 0, 0}, 0x1eb3 }, // U1EB3 # LATIN SMALL LETTER A WITH BREVE AND HOOK ABOVE
// /* broken */    { {0x0000, 0x0103, 0, 0, 0, 0}, 0x1eb1 }, // U1EB1 # LATIN SMALL LETTER A WITH BREVE AND GRAVE
// /* broken */    { {0x0000, 0x0112, 0, 0, 0, 0}, 0x1e16 }, // U1E16 # LATIN CAPITAL LETTER E WITH MACRON AND ACUTE
// /* broken */    { {0x0000, 0x0112, 0, 0, 0, 0}, 0x1e14 }, // U1E14 # LATIN CAPITAL LETTER E WITH MACRON AND GRAVE
// /* broken */    { {0x0000, 0x0113, 0, 0, 0, 0}, 0x1e15 }, // U1E15 # LATIN SMALL LETTER E WITH MACRON AND GRAVE
// /* broken */    { {0x0000, 0x0113, 0, 0, 0, 0}, 0x1e17 }, // U1E17 # LATIN SMALL LETTER E WITH MACRON AND ACUTE
// /* broken */    { {0x0000, 0x014c, 0, 0, 0, 0}, 0x1e52 }, // U1E52 # LATIN CAPITAL LETTER O WITH MACRON AND ACUTE
// /* broken */    { {0x0000, 0x014c, 0, 0, 0, 0}, 0x1e50 }, // U1E50 # LATIN CAPITAL LETTER O WITH MACRON AND GRAVE
// /* broken */    { {0x0000, 0x014d, 0, 0, 0, 0}, 0x1e51 }, // U1E51 # LATIN SMALL LETTER O WITH MACRON AND GRAVE
// /* broken */    { {0x0000, 0x014d, 0, 0, 0, 0}, 0x1e53 }, // U1E53 # LATIN SMALL LETTER O WITH MACRON AND ACUTE
// /* broken */    { {0x0000, 0x0168, 0, 0, 0, 0}, 0x1e78 }, // U1E78 # LATIN CAPITAL LETTER U WITH TILDE AND ACUTE
// /* broken */    { {0x0000, 0x0169, 0, 0, 0, 0}, 0x1e79 }, // U1E79 # LATIN SMALL LETTER U WITH TILDE AND ACUTE
// /* broken */    { {0x0000, 0x0313, 0x0391, 0, 0, 0}, 0x1f0a }, // U1F0A # GREEK CAPITAL LETTER ALPHA WITH PSILI AND VARIA
// /* broken */    { {0x0000, 0x0313, 0x0391, 0, 0, 0}, 0x1f0c }, // U1F0C # GREEK CAPITAL LETTER ALPHA WITH PSILI AND OXIA
// /* broken */    { {0x0000, 0x0313, 0x0395, 0, 0, 0}, 0x1f1a }, // U1F1A # GREEK CAPITAL LETTER EPSILON WITH PSILI AND VARIA
// /* broken */    { {0x0000, 0x0313, 0x0395, 0, 0, 0}, 0x1f1c }, // U1F1C # GREEK CAPITAL LETTER EPSILON WITH PSILI AND OXIA
// /* broken */    { {0x0000, 0x0313, 0x0397, 0, 0, 0}, 0x1f2c }, // U1F2C # GREEK CAPITAL LETTER ETA WITH PSILI AND OXIA
// /* broken */    { {0x0000, 0x0313, 0x0397, 0, 0, 0}, 0x1f2a }, // U1F2A # GREEK CAPITAL LETTER ETA WITH PSILI AND VARIA
// /* broken */    { {0x0000, 0x0313, 0x0399, 0, 0, 0}, 0x1f3c }, // U1F3C # GREEK CAPITAL LETTER IOTA WITH PSILI AND OXIA
// /* broken */    { {0x0000, 0x0313, 0x0399, 0, 0, 0}, 0x1f3a }, // U1F3A # GREEK CAPITAL LETTER IOTA WITH PSILI AND VARIA
// /* broken */    { {0x0000, 0x0313, 0x039f, 0, 0, 0}, 0x1f4a }, // U1F4A # GREEK CAPITAL LETTER OMICRON WITH PSILI AND VARIA
// /* broken */    { {0x0000, 0x0313, 0x039f, 0, 0, 0}, 0x1f4c }, // U1F4C # GREEK CAPITAL LETTER OMICRON WITH PSILI AND OXIA
// /* broken */    { {0x0000, 0x0313, 0x03a9, 0, 0, 0}, 0x1f6a }, // U1F6A # GREEK CAPITAL LETTER OMEGA WITH PSILI AND VARIA
// /* broken */    { {0x0000, 0x0313, 0x03a9, 0, 0, 0}, 0x1f6c }, // U1F6C # GREEK CAPITAL LETTER OMEGA WITH PSILI AND OXIA
// /* broken */    { {0x0000, 0x0313, 0x03b1, 0, 0, 0}, 0x1f04 }, // U1F04 # GREEK SMALL LETTER ALPHA WITH PSILI AND OXIA
// /* broken */    { {0x0000, 0x0313, 0x03b1, 0, 0, 0}, 0x1f02 }, // U1F02 # GREEK SMALL LETTER ALPHA WITH PSILI AND VARIA
// /* broken */    { {0x0000, 0x0313, 0x03b5, 0, 0, 0}, 0x1f14 }, // U1F14 # GREEK SMALL LETTER EPSILON WITH PSILI AND OXIA
// /* broken */    { {0x0000, 0x0313, 0x03b5, 0, 0, 0}, 0x1f12 }, // U1F12 # GREEK SMALL LETTER EPSILON WITH PSILI AND VARIA
// /* broken */    { {0x0000, 0x0313, 0x03b7, 0, 0, 0}, 0x1f24 }, // U1F24 # GREEK SMALL LETTER ETA WITH PSILI AND OXIA
// /* broken */    { {0x0000, 0x0313, 0x03b7, 0, 0, 0}, 0x1f22 }, // U1F22 # GREEK SMALL LETTER ETA WITH PSILI AND VARIA
// /* broken */    { {0x0000, 0x0313, 0x03b9, 0, 0, 0}, 0x1f32 }, // U1F32 # GREEK SMALL LETTER IOTA WITH PSILI AND VARIA
// /* broken */    { {0x0000, 0x0313, 0x03b9, 0, 0, 0}, 0x1f34 }, // U1F34 # GREEK SMALL LETTER IOTA WITH PSILI AND OXIA
// /* broken */    { {0x0000, 0x0313, 0x03bf, 0, 0, 0}, 0x1f42 }, // U1F42 # GREEK SMALL LETTER OMICRON WITH PSILI AND VARIA
// /* broken */    { {0x0000, 0x0313, 0x03bf, 0, 0, 0}, 0x1f44 }, // U1F44 # GREEK SMALL LETTER OMICRON WITH PSILI AND OXIA
// /* broken */    { {0x0000, 0x0313, 0x03c5, 0, 0, 0}, 0x1f52 }, // U1F52 # GREEK SMALL LETTER UPSILON WITH PSILI AND VARIA
// /* broken */    { {0x0000, 0x0313, 0x03c5, 0, 0, 0}, 0x1f54 }, // U1F54 # GREEK SMALL LETTER UPSILON WITH PSILI AND OXIA
// /* broken */    { {0x0000, 0x0313, 0x03c9, 0, 0, 0}, 0x1f62 }, // U1F62 # GREEK SMALL LETTER OMEGA WITH PSILI AND VARIA
// /* broken */    { {0x0000, 0x0313, 0x03c9, 0, 0, 0}, 0x1f64 }, // U1F64 # GREEK SMALL LETTER OMEGA WITH PSILI AND OXIA
// /* broken */    { {0x0000, 0x0314, 0x0391, 0, 0, 0}, 0x1f0b }, // U1F0B # GREEK CAPITAL LETTER ALPHA WITH DASIA AND VARIA
// /* broken */    { {0x0000, 0x0314, 0x0391, 0, 0, 0}, 0x1f0d }, // U1F0D # GREEK CAPITAL LETTER ALPHA WITH DASIA AND OXIA
// /* broken */    { {0x0000, 0x0314, 0x0395, 0, 0, 0}, 0x1f1d }, // U1F1D # GREEK CAPITAL LETTER EPSILON WITH DASIA AND OXIA
// /* broken */    { {0x0000, 0x0314, 0x0395, 0, 0, 0}, 0x1f1b }, // U1F1B # GREEK CAPITAL LETTER EPSILON WITH DASIA AND VARIA
// /* broken */    { {0x0000, 0x0314, 0x0397, 0, 0, 0}, 0x1f2d }, // U1F2D # GREEK CAPITAL LETTER ETA WITH DASIA AND OXIA
// /* broken */    { {0x0000, 0x0314, 0x0397, 0, 0, 0}, 0x1f2b }, // U1F2B # GREEK CAPITAL LETTER ETA WITH DASIA AND VARIA
// /* broken */    { {0x0000, 0x0314, 0x0399, 0, 0, 0}, 0x1f3b }, // U1F3B # GREEK CAPITAL LETTER IOTA WITH DASIA AND VARIA
// /* broken */    { {0x0000, 0x0314, 0x0399, 0, 0, 0}, 0x1f3d }, // U1F3D # GREEK CAPITAL LETTER IOTA WITH DASIA AND OXIA
// /* broken */    { {0x0000, 0x0314, 0x039f, 0, 0, 0}, 0x1f4b }, // U1F4B # GREEK CAPITAL LETTER OMICRON WITH DASIA AND VARIA
// /* broken */    { {0x0000, 0x0314, 0x039f, 0, 0, 0}, 0x1f4d }, // U1F4D # GREEK CAPITAL LETTER OMICRON WITH DASIA AND OXIA
// /* broken */    { {0x0000, 0x0314, 0x03a5, 0, 0, 0}, 0x1f5b }, // U1F5B # GREEK CAPITAL LETTER UPSILON WITH DASIA AND VARIA
// /* broken */    { {0x0000, 0x0314, 0x03a5, 0, 0, 0}, 0x1f5d }, // U1F5D # GREEK CAPITAL LETTER UPSILON WITH DASIA AND OXIA
// /* broken */    { {0x0000, 0x0314, 0x03a9, 0, 0, 0}, 0x1f6d }, // U1F6D # GREEK CAPITAL LETTER OMEGA WITH DASIA AND OXIA
// /* broken */    { {0x0000, 0x0314, 0x03a9, 0, 0, 0}, 0x1f6b }, // U1F6B # GREEK CAPITAL LETTER OMEGA WITH DASIA AND VARIA
// /* broken */    { {0x0000, 0x0314, 0x03b1, 0, 0, 0}, 0x1f03 }, // U1F03 # GREEK SMALL LETTER ALPHA WITH DASIA AND VARIA
// /* broken */    { {0x0000, 0x0314, 0x03b1, 0, 0, 0}, 0x1f05 }, // U1F05 # GREEK SMALL LETTER ALPHA WITH DASIA AND OXIA
// /* broken */    { {0x0000, 0x0314, 0x03b5, 0, 0, 0}, 0x1f13 }, // U1F13 # GREEK SMALL LETTER EPSILON WITH DASIA AND VARIA
// /* broken */    { {0x0000, 0x0314, 0x03b5, 0, 0, 0}, 0x1f15 }, // U1F15 # GREEK SMALL LETTER EPSILON WITH DASIA AND OXIA
// /* broken */    { {0x0000, 0x0314, 0x03b7, 0, 0, 0}, 0x1f23 }, // U1F23 # GREEK SMALL LETTER ETA WITH DASIA AND VARIA
// /* broken */    { {0x0000, 0x0314, 0x03b7, 0, 0, 0}, 0x1f25 }, // U1F25 # GREEK SMALL LETTER ETA WITH DASIA AND OXIA
// /* broken */    { {0x0000, 0x0314, 0x03b9, 0, 0, 0}, 0x1f33 }, // U1F33 # GREEK SMALL LETTER IOTA WITH DASIA AND VARIA
// /* broken */    { {0x0000, 0x0314, 0x03b9, 0, 0, 0}, 0x1f35 }, // U1F35 # GREEK SMALL LETTER IOTA WITH DASIA AND OXIA
// /* broken */    { {0x0000, 0x0314, 0x03bf, 0, 0, 0}, 0x1f45 }, // U1F45 # GREEK SMALL LETTER OMICRON WITH DASIA AND OXIA
// /* broken */    { {0x0000, 0x0314, 0x03bf, 0, 0, 0}, 0x1f43 }, // U1F43 # GREEK SMALL LETTER OMICRON WITH DASIA AND VARIA
// /* broken */    { {0x0000, 0x0314, 0x03c5, 0, 0, 0}, 0x1f53 }, // U1F53 # GREEK SMALL LETTER UPSILON WITH DASIA AND VARIA
// /* broken */    { {0x0000, 0x0314, 0x03c5, 0, 0, 0}, 0x1f55 }, // U1F55 # GREEK SMALL LETTER UPSILON WITH DASIA AND OXIA
// /* broken */    { {0x0000, 0x0314, 0x03c9, 0, 0, 0}, 0x1f63 }, // U1F63 # GREEK SMALL LETTER OMEGA WITH DASIA AND VARIA
// /* broken */    { {0x0000, 0x0314, 0x03c9, 0, 0, 0}, 0x1f65 }, // U1F65 # GREEK SMALL LETTER OMEGA WITH DASIA AND OXIA
// /* broken */    { {0x0000, 0x0391, 0, 0, 0, 0}, 0x0386 }, // U0386 # GREEK CAPITAL LETTER ALPHA WITH TONOS
// /* broken */    { {0x0000, 0x0391, 0, 0, 0, 0}, 0x1fba }, // U1FBA # GREEK CAPITAL LETTER ALPHA WITH VARIA
// /* broken */    { {0x0000, 0x0395, 0, 0, 0, 0}, 0x0388 }, // U0388 # GREEK CAPITAL LETTER EPSILON WITH TONOS
// /* broken */    { {0x0000, 0x0395, 0, 0, 0, 0}, 0x1fc8 }, // U1FC8 # GREEK CAPITAL LETTER EPSILON WITH VARIA
// /* broken */    { {0x0000, 0x0397, 0, 0, 0, 0}, 0x1fca }, // U1FCA # GREEK CAPITAL LETTER ETA WITH VARIA
// /* broken */    { {0x0000, 0x0397, 0, 0, 0, 0}, 0x0389 }, // U0389 # GREEK CAPITAL LETTER ETA WITH TONOS
// /* broken */    { {0x0000, 0x0399, 0, 0, 0, 0}, 0x038a }, // U038A # GREEK CAPITAL LETTER IOTA WITH TONOS
// /* broken */    { {0x0000, 0x0399, 0, 0, 0, 0}, 0x1fda }, // U1FDA # GREEK CAPITAL LETTER IOTA WITH VARIA
// /* broken */    { {0x0000, 0x039f, 0, 0, 0, 0}, 0x038c }, // U038C # GREEK CAPITAL LETTER OMICRON WITH TONOS
// /* broken */    { {0x0000, 0x039f, 0, 0, 0, 0}, 0x1ff8 }, // U1FF8 # GREEK CAPITAL LETTER OMICRON WITH VARIA
// /* broken */    { {0x0000, 0x03a5, 0, 0, 0, 0}, 0x038e }, // U038E # GREEK CAPITAL LETTER UPSILON WITH TONOS
// /* broken */    { {0x0000, 0x03a5, 0, 0, 0, 0}, 0x1fea }, // U1FEA # GREEK CAPITAL LETTER UPSILON WITH VARIA
// /* broken */    { {0x0000, 0x03a9, 0, 0, 0, 0}, 0x038f }, // U038F # GREEK CAPITAL LETTER OMEGA WITH TONOS
// /* broken */    { {0x0000, 0x03a9, 0, 0, 0, 0}, 0x1ffa }, // U1FFA # GREEK CAPITAL LETTER OMEGA WITH VARIA
// /* broken */    { {0x0000, 0x03b1, 0, 0, 0, 0}, 0x03ac }, // U03AC # GREEK SMALL LETTER ALPHA WITH TONOS
// /* broken */    { {0x0000, 0x03b1, 0, 0, 0, 0}, 0x1f70 }, // U1F70 # GREEK SMALL LETTER ALPHA WITH VARIA
// /* broken */    { {0x0000, 0x03b5, 0, 0, 0, 0}, 0x03ad }, // U03AD # GREEK SMALL LETTER EPSILON WITH TONOS
// /* broken */    { {0x0000, 0x03b5, 0, 0, 0, 0}, 0x1f72 }, // U1F72 # GREEK SMALL LETTER EPSILON WITH VARIA
// /* broken */    { {0x0000, 0x03b7, 0, 0, 0, 0}, 0x03ae }, // U03AE # GREEK SMALL LETTER ETA WITH TONOS
// /* broken */    { {0x0000, 0x03b7, 0, 0, 0, 0}, 0x1f74 }, // U1F74 # GREEK SMALL LETTER ETA WITH VARIA
// /* broken */    { {0x0000, 0x03b9, 0, 0, 0, 0}, 0x03af }, // U03AF # GREEK SMALL LETTER IOTA WITH TONOS
// /* broken */    { {0x0000, 0x03b9, 0, 0, 0, 0}, 0x1f76 }, // U1F76 # GREEK SMALL LETTER IOTA WITH VARIA
// /* broken */    { {0x0000, 0x03bf, 0, 0, 0, 0}, 0x03cc }, // U03CC # GREEK SMALL LETTER OMICRON WITH TONOS
// /* broken */    { {0x0000, 0x03bf, 0, 0, 0, 0}, 0x1f78 }, // U1F78 # GREEK SMALL LETTER OMICRON WITH VARIA
// /* broken */    { {0x0000, 0x03c5, 0, 0, 0, 0}, 0x03cd }, // U03CD # GREEK SMALL LETTER UPSILON WITH TONOS
// /* broken */    { {0x0000, 0x03c5, 0, 0, 0, 0}, 0x1f7a }, // U1F7A # GREEK SMALL LETTER UPSILON WITH VARIA
// /* broken */    { {0x0000, 0x03c9, 0, 0, 0, 0}, 0x03ce }, // U03CE # GREEK SMALL LETTER OMEGA WITH TONOS
// /* broken */    { {0x0000, 0x03c9, 0, 0, 0, 0}, 0x1f7c }, // U1F7C # GREEK SMALL LETTER OMEGA WITH VARIA
// /* broken */    { {0x0000, 0x03ca, 0, 0, 0, 0}, 0x0390 }, // U0390 # GREEK SMALL LETTER IOTA WITH DIALYTIKA AND TONOS
// /* broken */    { {0x0000, 0x03ca, 0, 0, 0, 0}, 0x1fd2 }, // U1FD2 # GREEK SMALL LETTER IOTA WITH DIALYTIKA AND VARIA
// /* broken */    { {0x0000, 0x03cb, 0, 0, 0, 0}, 0x1fe2 }, // U1FE2 # GREEK SMALL LETTER UPSILON WITH DIALYTIKA AND VARIA
// /* broken */    { {0x0000, 0x03cb, 0, 0, 0, 0}, 0x03b0 }, // U03B0 # GREEK SMALL LETTER UPSILON WITH DIALYTIKA AND TONOS
// /* broken */    { {0x0000, 0x0413, 0, 0, 0, 0}, 0x0403 }, // U0403 # CYRILLIC CAPITAL LETTER GJE
// /* broken */    { {0x0000, 0x0415, 0, 0, 0, 0}, 0x0400 }, // U0400 # CYRILLIC CAPITAL LETTER IE WITH GRAVE
// /* broken */    { {0x0000, 0x0418, 0, 0, 0, 0}, 0x040d }, // U040D # CYRILLIC CAPITAL LETTER I WITH GRAVE
// /* broken */    { {0x0000, 0x041a, 0, 0, 0, 0}, 0x040c }, // U040C # CYRILLIC CAPITAL LETTER KJE
// /* broken */    { {0x0000, 0x0433, 0, 0, 0, 0}, 0x0453 }, // U0453 # CYRILLIC SMALL LETTER GJE
// /* broken */    { {0x0000, 0x0435, 0, 0, 0, 0}, 0x0450 }, // U0450 # CYRILLIC SMALL LETTER IE WITH GRAVE
// /* broken */    { {0x0000, 0x0438, 0, 0, 0, 0}, 0x045d }, // U045D # CYRILLIC SMALL LETTER I WITH GRAVE
// /* broken */    { {0x0000, 0x043a, 0, 0, 0, 0}, 0x045c }, // U045C # CYRILLIC SMALL LETTER KJE
// /* broken */    { {0x0000, 0x1f00, 0, 0, 0, 0}, 0x1f02 }, // U1F02 # GREEK SMALL LETTER ALPHA WITH PSILI AND VARIA
// /* broken */    { {0x0000, 0x1f00, 0, 0, 0, 0}, 0x1f04 }, // U1F04 # GREEK SMALL LETTER ALPHA WITH PSILI AND OXIA
// /* broken */    { {0x0000, 0x1f01, 0, 0, 0, 0}, 0x1f03 }, // U1F03 # GREEK SMALL LETTER ALPHA WITH DASIA AND VARIA
// /* broken */    { {0x0000, 0x1f01, 0, 0, 0, 0}, 0x1f05 }, // U1F05 # GREEK SMALL LETTER ALPHA WITH DASIA AND OXIA
// /* broken */    { {0x0000, 0x1f08, 0, 0, 0, 0}, 0x1f0c }, // U1F0C # GREEK CAPITAL LETTER ALPHA WITH PSILI AND OXIA
// /* broken */    { {0x0000, 0x1f08, 0, 0, 0, 0}, 0x1f0a }, // U1F0A # GREEK CAPITAL LETTER ALPHA WITH PSILI AND VARIA
// /* broken */    { {0x0000, 0x1f09, 0, 0, 0, 0}, 0x1f0b }, // U1F0B # GREEK CAPITAL LETTER ALPHA WITH DASIA AND VARIA
// /* broken */    { {0x0000, 0x1f09, 0, 0, 0, 0}, 0x1f0d }, // U1F0D # GREEK CAPITAL LETTER ALPHA WITH DASIA AND OXIA
// /* broken */    { {0x0000, 0x1f10, 0, 0, 0, 0}, 0x1f12 }, // U1F12 # GREEK SMALL LETTER EPSILON WITH PSILI AND VARIA
// /* broken */    { {0x0000, 0x1f10, 0, 0, 0, 0}, 0x1f14 }, // U1F14 # GREEK SMALL LETTER EPSILON WITH PSILI AND OXIA
// /* broken */    { {0x0000, 0x1f11, 0, 0, 0, 0}, 0x1f13 }, // U1F13 # GREEK SMALL LETTER EPSILON WITH DASIA AND VARIA
// /* broken */    { {0x0000, 0x1f11, 0, 0, 0, 0}, 0x1f15 }, // U1F15 # GREEK SMALL LETTER EPSILON WITH DASIA AND OXIA
// /* broken */    { {0x0000, 0x1f18, 0, 0, 0, 0}, 0x1f1c }, // U1F1C # GREEK CAPITAL LETTER EPSILON WITH PSILI AND OXIA
// /* broken */    { {0x0000, 0x1f18, 0, 0, 0, 0}, 0x1f1a }, // U1F1A # GREEK CAPITAL LETTER EPSILON WITH PSILI AND VARIA
// /* broken */    { {0x0000, 0x1f19, 0, 0, 0, 0}, 0x1f1b }, // U1F1B # GREEK CAPITAL LETTER EPSILON WITH DASIA AND VARIA
// /* broken */    { {0x0000, 0x1f19, 0, 0, 0, 0}, 0x1f1d }, // U1F1D # GREEK CAPITAL LETTER EPSILON WITH DASIA AND OXIA
// /* broken */    { {0x0000, 0x1f20, 0, 0, 0, 0}, 0x1f22 }, // U1F22 # GREEK SMALL LETTER ETA WITH PSILI AND VARIA
// /* broken */    { {0x0000, 0x1f20, 0, 0, 0, 0}, 0x1f24 }, // U1F24 # GREEK SMALL LETTER ETA WITH PSILI AND OXIA
// /* broken */    { {0x0000, 0x1f21, 0, 0, 0, 0}, 0x1f23 }, // U1F23 # GREEK SMALL LETTER ETA WITH DASIA AND VARIA
// /* broken */    { {0x0000, 0x1f21, 0, 0, 0, 0}, 0x1f25 }, // U1F25 # GREEK SMALL LETTER ETA WITH DASIA AND OXIA
// /* broken */    { {0x0000, 0x1f28, 0, 0, 0, 0}, 0x1f2a }, // U1F2A # GREEK CAPITAL LETTER ETA WITH PSILI AND VARIA
// /* broken */    { {0x0000, 0x1f28, 0, 0, 0, 0}, 0x1f2c }, // U1F2C # GREEK CAPITAL LETTER ETA WITH PSILI AND OXIA
// /* broken */    { {0x0000, 0x1f29, 0, 0, 0, 0}, 0x1f2d }, // U1F2D # GREEK CAPITAL LETTER ETA WITH DASIA AND OXIA
// /* broken */    { {0x0000, 0x1f29, 0, 0, 0, 0}, 0x1f2b }, // U1F2B # GREEK CAPITAL LETTER ETA WITH DASIA AND VARIA
// /* broken */    { {0x0000, 0x1f30, 0, 0, 0, 0}, 0x1f32 }, // U1F32 # GREEK SMALL LETTER IOTA WITH PSILI AND VARIA
// /* broken */    { {0x0000, 0x1f30, 0, 0, 0, 0}, 0x1f34 }, // U1F34 # GREEK SMALL LETTER IOTA WITH PSILI AND OXIA
// /* broken */    { {0x0000, 0x1f31, 0, 0, 0, 0}, 0x1f33 }, // U1F33 # GREEK SMALL LETTER IOTA WITH DASIA AND VARIA
// /* broken */    { {0x0000, 0x1f31, 0, 0, 0, 0}, 0x1f35 }, // U1F35 # GREEK SMALL LETTER IOTA WITH DASIA AND OXIA
// /* broken */    { {0x0000, 0x1f38, 0, 0, 0, 0}, 0x1f3a }, // U1F3A # GREEK CAPITAL LETTER IOTA WITH PSILI AND VARIA
// /* broken */    { {0x0000, 0x1f38, 0, 0, 0, 0}, 0x1f3c }, // U1F3C # GREEK CAPITAL LETTER IOTA WITH PSILI AND OXIA
// /* broken */    { {0x0000, 0x1f39, 0, 0, 0, 0}, 0x1f3b }, // U1F3B # GREEK CAPITAL LETTER IOTA WITH DASIA AND VARIA
// /* broken */    { {0x0000, 0x1f39, 0, 0, 0, 0}, 0x1f3d }, // U1F3D # GREEK CAPITAL LETTER IOTA WITH DASIA AND OXIA
// /* broken */    { {0x0000, 0x1f40, 0, 0, 0, 0}, 0x1f44 }, // U1F44 # GREEK SMALL LETTER OMICRON WITH PSILI AND OXIA
// /* broken */    { {0x0000, 0x1f40, 0, 0, 0, 0}, 0x1f42 }, // U1F42 # GREEK SMALL LETTER OMICRON WITH PSILI AND VARIA
// /* broken */    { {0x0000, 0x1f41, 0, 0, 0, 0}, 0x1f43 }, // U1F43 # GREEK SMALL LETTER OMICRON WITH DASIA AND VARIA
// /* broken */    { {0x0000, 0x1f41, 0, 0, 0, 0}, 0x1f45 }, // U1F45 # GREEK SMALL LETTER OMICRON WITH DASIA AND OXIA
// /* broken */    { {0x0000, 0x1f48, 0, 0, 0, 0}, 0x1f4a }, // U1F4A # GREEK CAPITAL LETTER OMICRON WITH PSILI AND VARIA
// /* broken */    { {0x0000, 0x1f48, 0, 0, 0, 0}, 0x1f4c }, // U1F4C # GREEK CAPITAL LETTER OMICRON WITH PSILI AND OXIA
// /* broken */    { {0x0000, 0x1f49, 0, 0, 0, 0}, 0x1f4d }, // U1F4D # GREEK CAPITAL LETTER OMICRON WITH DASIA AND OXIA
// /* broken */    { {0x0000, 0x1f49, 0, 0, 0, 0}, 0x1f4b }, // U1F4B # GREEK CAPITAL LETTER OMICRON WITH DASIA AND VARIA
// /* broken */    { {0x0000, 0x1f50, 0, 0, 0, 0}, 0x1f52 }, // U1F52 # GREEK SMALL LETTER UPSILON WITH PSILI AND VARIA
// /* broken */    { {0x0000, 0x1f50, 0, 0, 0, 0}, 0x1f54 }, // U1F54 # GREEK SMALL LETTER UPSILON WITH PSILI AND OXIA
// /* broken */    { {0x0000, 0x1f51, 0, 0, 0, 0}, 0x1f53 }, // U1F53 # GREEK SMALL LETTER UPSILON WITH DASIA AND VARIA
// /* broken */    { {0x0000, 0x1f51, 0, 0, 0, 0}, 0x1f55 }, // U1F55 # GREEK SMALL LETTER UPSILON WITH DASIA AND OXIA
// /* broken */    { {0x0000, 0x1f59, 0, 0, 0, 0}, 0x1f5b }, // U1F5B # GREEK CAPITAL LETTER UPSILON WITH DASIA AND VARIA
// /* broken */    { {0x0000, 0x1f59, 0, 0, 0, 0}, 0x1f5d }, // U1F5D # GREEK CAPITAL LETTER UPSILON WITH DASIA AND OXIA
// /* broken */    { {0x0000, 0x1f60, 0, 0, 0, 0}, 0x1f64 }, // U1F64 # GREEK SMALL LETTER OMEGA WITH PSILI AND OXIA
// /* broken */    { {0x0000, 0x1f60, 0, 0, 0, 0}, 0x1f62 }, // U1F62 # GREEK SMALL LETTER OMEGA WITH PSILI AND VARIA
// /* broken */    { {0x0000, 0x1f61, 0, 0, 0, 0}, 0x1f63 }, // U1F63 # GREEK SMALL LETTER OMEGA WITH DASIA AND VARIA
// /* broken */    { {0x0000, 0x1f61, 0, 0, 0, 0}, 0x1f65 }, // U1F65 # GREEK SMALL LETTER OMEGA WITH DASIA AND OXIA
// /* broken */    { {0x0000, 0x1f68, 0, 0, 0, 0}, 0x1f6a }, // U1F6A # GREEK CAPITAL LETTER OMEGA WITH PSILI AND VARIA
// /* broken */    { {0x0000, 0x1f68, 0, 0, 0, 0}, 0x1f6c }, // U1F6C # GREEK CAPITAL LETTER OMEGA WITH PSILI AND OXIA
// /* broken */    { {0x0000, 0x1f69, 0, 0, 0, 0}, 0x1f6b }, // U1F6B # GREEK CAPITAL LETTER OMEGA WITH DASIA AND VARIA
// /* broken */    { {0x0000, 0x1f69, 0, 0, 0, 0}, 0x1f6d }, // U1F6D # GREEK CAPITAL LETTER OMEGA WITH DASIA AND OXIA
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x0000, 0x004f, 0, 0}, 0x01fe }, // U01FE # LATIN CAPITAL LETTER O WITH STROKE AND ACUTE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x0000, 0x006f, 0, 0}, 0x01ff }, // U01FF # LATIN SMALL LETTER O WITH STROKE AND ACUTE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x0022, 0x0049, 0, 0}, 0x1e2e }, // U1E2E # LATIN CAPITAL LETTER I WITH DIAERESIS AND ACUTE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x0022, 0x0055, 0, 0}, 0x01d7 }, // U01D7 # LATIN CAPITAL LETTER U WITH DIAERESIS AND ACUTE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x0022, 0x0055, 0, 0}, 0x01db }, // U01DB # LATIN CAPITAL LETTER U WITH DIAERESIS AND GRAVE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x0022, 0x0069, 0, 0}, 0x1e2f }, // U1E2F # LATIN SMALL LETTER I WITH DIAERESIS AND ACUTE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x0022, 0x0075, 0, 0}, 0x01dc }, // U01DC # LATIN SMALL LETTER U WITH DIAERESIS AND GRAVE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x0022, 0x0075, 0, 0}, 0x01d8 }, // U01D8 # LATIN SMALL LETTER U WITH DIAERESIS AND ACUTE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x0022, 0x03b9, 0, 0}, 0x1fd2 }, // U1FD2 # GREEK SMALL LETTER IOTA WITH DIALYTIKA AND VARIA
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x0022, 0x03b9, 0, 0}, 0x0390 }, // U0390 # GREEK SMALL LETTER IOTA WITH DIALYTIKA AND TONOS
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x0022, 0x03c5, 0, 0}, 0x1fe2 }, // U1FE2 # GREEK SMALL LETTER UPSILON WITH DIALYTIKA AND VARIA
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x0022, 0x03c5, 0, 0}, 0x03b0 }, // U03B0 # GREEK SMALL LETTER UPSILON WITH DIALYTIKA AND TONOS
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0391, 0, 0}, 0x1f0b }, // U1F0B # GREEK CAPITAL LETTER ALPHA WITH DASIA AND VARIA
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0391, 0, 0}, 0x1f0d }, // U1F0D # GREEK CAPITAL LETTER ALPHA WITH DASIA AND OXIA
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0395, 0, 0}, 0x1f1d }, // U1F1D # GREEK CAPITAL LETTER EPSILON WITH DASIA AND OXIA
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0395, 0, 0}, 0x1f1b }, // U1F1B # GREEK CAPITAL LETTER EPSILON WITH DASIA AND VARIA
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0397, 0, 0}, 0x1f2b }, // U1F2B # GREEK CAPITAL LETTER ETA WITH DASIA AND VARIA
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0397, 0, 0}, 0x1f2d }, // U1F2D # GREEK CAPITAL LETTER ETA WITH DASIA AND OXIA
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0399, 0, 0}, 0x1f3b }, // U1F3B # GREEK CAPITAL LETTER IOTA WITH DASIA AND VARIA
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0399, 0, 0}, 0x1f3d }, // U1F3D # GREEK CAPITAL LETTER IOTA WITH DASIA AND OXIA
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x0028, 0x039f, 0, 0}, 0x1f4b }, // U1F4B # GREEK CAPITAL LETTER OMICRON WITH DASIA AND VARIA
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x0028, 0x039f, 0, 0}, 0x1f4d }, // U1F4D # GREEK CAPITAL LETTER OMICRON WITH DASIA AND OXIA
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x0028, 0x03a5, 0, 0}, 0x1f5b }, // U1F5B # GREEK CAPITAL LETTER UPSILON WITH DASIA AND VARIA
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x0028, 0x03a5, 0, 0}, 0x1f5d }, // U1F5D # GREEK CAPITAL LETTER UPSILON WITH DASIA AND OXIA
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x0028, 0x03a9, 0, 0}, 0x1f6d }, // U1F6D # GREEK CAPITAL LETTER OMEGA WITH DASIA AND OXIA
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x0028, 0x03a9, 0, 0}, 0x1f6b }, // U1F6B # GREEK CAPITAL LETTER OMEGA WITH DASIA AND VARIA
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x0028, 0x03b1, 0, 0}, 0x1f03 }, // U1F03 # GREEK SMALL LETTER ALPHA WITH DASIA AND VARIA
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x0028, 0x03b1, 0, 0}, 0x1f05 }, // U1F05 # GREEK SMALL LETTER ALPHA WITH DASIA AND OXIA
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x0028, 0x03b5, 0, 0}, 0x1f15 }, // U1F15 # GREEK SMALL LETTER EPSILON WITH DASIA AND OXIA
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x0028, 0x03b5, 0, 0}, 0x1f13 }, // U1F13 # GREEK SMALL LETTER EPSILON WITH DASIA AND VARIA
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x0028, 0x03b7, 0, 0}, 0x1f23 }, // U1F23 # GREEK SMALL LETTER ETA WITH DASIA AND VARIA
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x0028, 0x03b7, 0, 0}, 0x1f25 }, // U1F25 # GREEK SMALL LETTER ETA WITH DASIA AND OXIA
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x0028, 0x03b9, 0, 0}, 0x1f35 }, // U1F35 # GREEK SMALL LETTER IOTA WITH DASIA AND OXIA
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x0028, 0x03b9, 0, 0}, 0x1f33 }, // U1F33 # GREEK SMALL LETTER IOTA WITH DASIA AND VARIA
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x0028, 0x03bf, 0, 0}, 0x1f45 }, // U1F45 # GREEK SMALL LETTER OMICRON WITH DASIA AND OXIA
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x0028, 0x03bf, 0, 0}, 0x1f43 }, // U1F43 # GREEK SMALL LETTER OMICRON WITH DASIA AND VARIA
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x0028, 0x03c5, 0, 0}, 0x1f55 }, // U1F55 # GREEK SMALL LETTER UPSILON WITH DASIA AND OXIA
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x0028, 0x03c5, 0, 0}, 0x1f53 }, // U1F53 # GREEK SMALL LETTER UPSILON WITH DASIA AND VARIA
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x0028, 0x03c9, 0, 0}, 0x1f65 }, // U1F65 # GREEK SMALL LETTER OMEGA WITH DASIA AND OXIA
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x0028, 0x03c9, 0, 0}, 0x1f63 }, // U1F63 # GREEK SMALL LETTER OMEGA WITH DASIA AND VARIA
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x0029, 0x0391, 0, 0}, 0x1f0c }, // U1F0C # GREEK CAPITAL LETTER ALPHA WITH PSILI AND OXIA
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x0029, 0x0391, 0, 0}, 0x1f0a }, // U1F0A # GREEK CAPITAL LETTER ALPHA WITH PSILI AND VARIA
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x0029, 0x0395, 0, 0}, 0x1f1c }, // U1F1C # GREEK CAPITAL LETTER EPSILON WITH PSILI AND OXIA
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x0029, 0x0395, 0, 0}, 0x1f1a }, // U1F1A # GREEK CAPITAL LETTER EPSILON WITH PSILI AND VARIA
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x0029, 0x0397, 0, 0}, 0x1f2c }, // U1F2C # GREEK CAPITAL LETTER ETA WITH PSILI AND OXIA
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x0029, 0x0397, 0, 0}, 0x1f2a }, // U1F2A # GREEK CAPITAL LETTER ETA WITH PSILI AND VARIA
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x0029, 0x0399, 0, 0}, 0x1f3c }, // U1F3C # GREEK CAPITAL LETTER IOTA WITH PSILI AND OXIA
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x0029, 0x0399, 0, 0}, 0x1f3a }, // U1F3A # GREEK CAPITAL LETTER IOTA WITH PSILI AND VARIA
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x0029, 0x039f, 0, 0}, 0x1f4c }, // U1F4C # GREEK CAPITAL LETTER OMICRON WITH PSILI AND OXIA
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x0029, 0x039f, 0, 0}, 0x1f4a }, // U1F4A # GREEK CAPITAL LETTER OMICRON WITH PSILI AND VARIA
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x0029, 0x03a9, 0, 0}, 0x1f6c }, // U1F6C # GREEK CAPITAL LETTER OMEGA WITH PSILI AND OXIA
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x0029, 0x03a9, 0, 0}, 0x1f6a }, // U1F6A # GREEK CAPITAL LETTER OMEGA WITH PSILI AND VARIA
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x0029, 0x03b1, 0, 0}, 0x1f04 }, // U1F04 # GREEK SMALL LETTER ALPHA WITH PSILI AND OXIA
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x0029, 0x03b1, 0, 0}, 0x1f02 }, // U1F02 # GREEK SMALL LETTER ALPHA WITH PSILI AND VARIA
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x0029, 0x03b5, 0, 0}, 0x1f12 }, // U1F12 # GREEK SMALL LETTER EPSILON WITH PSILI AND VARIA
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x0029, 0x03b5, 0, 0}, 0x1f14 }, // U1F14 # GREEK SMALL LETTER EPSILON WITH PSILI AND OXIA
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x0029, 0x03b7, 0, 0}, 0x1f22 }, // U1F22 # GREEK SMALL LETTER ETA WITH PSILI AND VARIA
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x0029, 0x03b7, 0, 0}, 0x1f24 }, // U1F24 # GREEK SMALL LETTER ETA WITH PSILI AND OXIA
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x0029, 0x03b9, 0, 0}, 0x1f32 }, // U1F32 # GREEK SMALL LETTER IOTA WITH PSILI AND VARIA
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x0029, 0x03b9, 0, 0}, 0x1f34 }, // U1F34 # GREEK SMALL LETTER IOTA WITH PSILI AND OXIA
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x0029, 0x03bf, 0, 0}, 0x1f44 }, // U1F44 # GREEK SMALL LETTER OMICRON WITH PSILI AND OXIA
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x0029, 0x03bf, 0, 0}, 0x1f42 }, // U1F42 # GREEK SMALL LETTER OMICRON WITH PSILI AND VARIA
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x0029, 0x03c5, 0, 0}, 0x1f54 }, // U1F54 # GREEK SMALL LETTER UPSILON WITH PSILI AND OXIA
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x0029, 0x03c5, 0, 0}, 0x1f52 }, // U1F52 # GREEK SMALL LETTER UPSILON WITH PSILI AND VARIA
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x0029, 0x03c9, 0, 0}, 0x1f64 }, // U1F64 # GREEK SMALL LETTER OMEGA WITH PSILI AND OXIA
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x0029, 0x03c9, 0, 0}, 0x1f62 }, // U1F62 # GREEK SMALL LETTER OMEGA WITH PSILI AND VARIA
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x002b, 0x004f, 0, 0}, 0x1ee0 }, // U1EE0 # LATIN CAPITAL LETTER O WITH HORN AND TILDE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x002b, 0x004f, 0, 0}, 0x1ede }, // U1EDE # LATIN CAPITAL LETTER O WITH HORN AND HOOK ABOVE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x002b, 0x004f, 0, 0}, 0x1ee2 }, // U1EE2 # LATIN CAPITAL LETTER O WITH HORN AND DOT BELOW
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x002b, 0x004f, 0, 0}, 0x1eda }, // U1EDA # LATIN CAPITAL LETTER O WITH HORN AND ACUTE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x002b, 0x004f, 0, 0}, 0x1edc }, // U1EDC # LATIN CAPITAL LETTER O WITH HORN AND GRAVE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x002b, 0x0055, 0, 0}, 0x1eea }, // U1EEA # LATIN CAPITAL LETTER U WITH HORN AND GRAVE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x002b, 0x0055, 0, 0}, 0x1ef0 }, // U1EF0 # LATIN CAPITAL LETTER U WITH HORN AND DOT BELOW
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x002b, 0x0055, 0, 0}, 0x1eee }, // U1EEE # LATIN CAPITAL LETTER U WITH HORN AND TILDE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x002b, 0x0055, 0, 0}, 0x1ee8 }, // U1EE8 # LATIN CAPITAL LETTER U WITH HORN AND ACUTE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x002b, 0x0055, 0, 0}, 0x1eec }, // U1EEC # LATIN CAPITAL LETTER U WITH HORN AND HOOK ABOVE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x002b, 0x006f, 0, 0}, 0x1edb }, // U1EDB # LATIN SMALL LETTER O WITH HORN AND ACUTE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x002b, 0x006f, 0, 0}, 0x1ee1 }, // U1EE1 # LATIN SMALL LETTER O WITH HORN AND TILDE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x002b, 0x006f, 0, 0}, 0x1edf }, // U1EDF # LATIN SMALL LETTER O WITH HORN AND HOOK ABOVE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x002b, 0x006f, 0, 0}, 0x1ee3 }, // U1EE3 # LATIN SMALL LETTER O WITH HORN AND DOT BELOW
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x002b, 0x006f, 0, 0}, 0x1edd }, // U1EDD # LATIN SMALL LETTER O WITH HORN AND GRAVE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x002b, 0x0075, 0, 0}, 0x1eeb }, // U1EEB # LATIN SMALL LETTER U WITH HORN AND GRAVE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x002b, 0x0075, 0, 0}, 0x1ef1 }, // U1EF1 # LATIN SMALL LETTER U WITH HORN AND DOT BELOW
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x002b, 0x0075, 0, 0}, 0x1eed }, // U1EED # LATIN SMALL LETTER U WITH HORN AND HOOK ABOVE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x002b, 0x0075, 0, 0}, 0x1ee9 }, // U1EE9 # LATIN SMALL LETTER U WITH HORN AND ACUTE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x002b, 0x0075, 0, 0}, 0x1eef }, // U1EEF # LATIN SMALL LETTER U WITH HORN AND TILDE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x002c, 0x0043, 0, 0}, 0x1e08 }, // U1E08 # LATIN CAPITAL LETTER C WITH CEDILLA AND ACUTE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x002c, 0x0063, 0, 0}, 0x1e09 }, // U1E09 # LATIN SMALL LETTER C WITH CEDILLA AND ACUTE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x002f, 0x004f, 0, 0}, 0x01fe }, // U01FE # LATIN CAPITAL LETTER O WITH STROKE AND ACUTE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x002f, 0x006f, 0, 0}, 0x01ff }, // U01FF # LATIN SMALL LETTER O WITH STROKE AND ACUTE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x0055, 0x0041, 0, 0}, 0x1eb2 }, // U1EB2 # LATIN CAPITAL LETTER A WITH BREVE AND HOOK ABOVE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x0055, 0x0041, 0, 0}, 0x1eb4 }, // U1EB4 # LATIN CAPITAL LETTER A WITH BREVE AND TILDE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x0055, 0x0041, 0, 0}, 0x1eae }, // U1EAE # LATIN CAPITAL LETTER A WITH BREVE AND ACUTE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x0055, 0x0041, 0, 0}, 0x1eb0 }, // U1EB0 # LATIN CAPITAL LETTER A WITH BREVE AND GRAVE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x0055, 0x0061, 0, 0}, 0x1eb5 }, // U1EB5 # LATIN SMALL LETTER A WITH BREVE AND TILDE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x0055, 0x0061, 0, 0}, 0x1eaf }, // U1EAF # LATIN SMALL LETTER A WITH BREVE AND ACUTE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x0055, 0x0061, 0, 0}, 0x1eb3 }, // U1EB3 # LATIN SMALL LETTER A WITH BREVE AND HOOK ABOVE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x0055, 0x0061, 0, 0}, 0x1eb1 }, // U1EB1 # LATIN SMALL LETTER A WITH BREVE AND GRAVE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x005e, 0x0041, 0, 0}, 0x1ea8 }, // U1EA8 # LATIN CAPITAL LETTER A WITH CIRCUMFLEX AND HOOK ABOVE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x005e, 0x0041, 0, 0}, 0x1ea6 }, // U1EA6 # LATIN CAPITAL LETTER A WITH CIRCUMFLEX AND GRAVE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x005e, 0x0041, 0, 0}, 0x1eaa }, // U1EAA # LATIN CAPITAL LETTER A WITH CIRCUMFLEX AND TILDE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x005e, 0x0041, 0, 0}, 0x1ea4 }, // U1EA4 # LATIN CAPITAL LETTER A WITH CIRCUMFLEX AND ACUTE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x005e, 0x0045, 0, 0}, 0x1ec4 }, // U1EC4 # LATIN CAPITAL LETTER E WITH CIRCUMFLEX AND TILDE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x005e, 0x0045, 0, 0}, 0x1ec2 }, // U1EC2 # LATIN CAPITAL LETTER E WITH CIRCUMFLEX AND HOOK ABOVE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x005e, 0x0045, 0, 0}, 0x1ec0 }, // U1EC0 # LATIN CAPITAL LETTER E WITH CIRCUMFLEX AND GRAVE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x005e, 0x0045, 0, 0}, 0x1ebe }, // U1EBE # LATIN CAPITAL LETTER E WITH CIRCUMFLEX AND ACUTE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x005e, 0x004f, 0, 0}, 0x1ed6 }, // U1ED6 # LATIN CAPITAL LETTER O WITH CIRCUMFLEX AND TILDE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x005e, 0x004f, 0, 0}, 0x1ed4 }, // U1ED4 # LATIN CAPITAL LETTER O WITH CIRCUMFLEX AND HOOK ABOVE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x005e, 0x004f, 0, 0}, 0x1ed0 }, // U1ED0 # LATIN CAPITAL LETTER O WITH CIRCUMFLEX AND ACUTE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x005e, 0x004f, 0, 0}, 0x1ed2 }, // U1ED2 # LATIN CAPITAL LETTER O WITH CIRCUMFLEX AND GRAVE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x005e, 0x0061, 0, 0}, 0x1ea9 }, // U1EA9 # LATIN SMALL LETTER A WITH CIRCUMFLEX AND HOOK ABOVE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x005e, 0x0061, 0, 0}, 0x1ea7 }, // U1EA7 # LATIN SMALL LETTER A WITH CIRCUMFLEX AND GRAVE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x005e, 0x0061, 0, 0}, 0x1ea5 }, // U1EA5 # LATIN SMALL LETTER A WITH CIRCUMFLEX AND ACUTE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x005e, 0x0061, 0, 0}, 0x1eab }, // U1EAB # LATIN SMALL LETTER A WITH CIRCUMFLEX AND TILDE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x005e, 0x0065, 0, 0}, 0x1ec5 }, // U1EC5 # LATIN SMALL LETTER E WITH CIRCUMFLEX AND TILDE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x005e, 0x0065, 0, 0}, 0x1ec1 }, // U1EC1 # LATIN SMALL LETTER E WITH CIRCUMFLEX AND GRAVE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x005e, 0x0065, 0, 0}, 0x1ebf }, // U1EBF # LATIN SMALL LETTER E WITH CIRCUMFLEX AND ACUTE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x005e, 0x0065, 0, 0}, 0x1ec3 }, // U1EC3 # LATIN SMALL LETTER E WITH CIRCUMFLEX AND HOOK ABOVE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x005e, 0x006f, 0, 0}, 0x1ed7 }, // U1ED7 # LATIN SMALL LETTER O WITH CIRCUMFLEX AND TILDE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x005e, 0x006f, 0, 0}, 0x1ed5 }, // U1ED5 # LATIN SMALL LETTER O WITH CIRCUMFLEX AND HOOK ABOVE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x005e, 0x006f, 0, 0}, 0x1ed1 }, // U1ED1 # LATIN SMALL LETTER O WITH CIRCUMFLEX AND ACUTE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x005e, 0x006f, 0, 0}, 0x1ed3 }, // U1ED3 # LATIN SMALL LETTER O WITH CIRCUMFLEX AND GRAVE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0045, 0, 0}, 0x1e16 }, // U1E16 # LATIN CAPITAL LETTER E WITH MACRON AND ACUTE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0045, 0, 0}, 0x1e14 }, // U1E14 # LATIN CAPITAL LETTER E WITH MACRON AND GRAVE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x005f, 0x004f, 0, 0}, 0x1e52 }, // U1E52 # LATIN CAPITAL LETTER O WITH MACRON AND ACUTE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x005f, 0x004f, 0, 0}, 0x1e50 }, // U1E50 # LATIN CAPITAL LETTER O WITH MACRON AND GRAVE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0065, 0, 0}, 0x1e15 }, // U1E15 # LATIN SMALL LETTER E WITH MACRON AND GRAVE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0065, 0, 0}, 0x1e17 }, // U1E17 # LATIN SMALL LETTER E WITH MACRON AND ACUTE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x005f, 0x006f, 0, 0}, 0x1e51 }, // U1E51 # LATIN SMALL LETTER O WITH MACRON AND GRAVE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x005f, 0x006f, 0, 0}, 0x1e53 }, // U1E53 # LATIN SMALL LETTER O WITH MACRON AND ACUTE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x0062, 0x0041, 0, 0}, 0x1eae }, // U1EAE # LATIN CAPITAL LETTER A WITH BREVE AND ACUTE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x0062, 0x0041, 0, 0}, 0x1eb4 }, // U1EB4 # LATIN CAPITAL LETTER A WITH BREVE AND TILDE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x0062, 0x0041, 0, 0}, 0x1eb0 }, // U1EB0 # LATIN CAPITAL LETTER A WITH BREVE AND GRAVE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x0062, 0x0041, 0, 0}, 0x1eb2 }, // U1EB2 # LATIN CAPITAL LETTER A WITH BREVE AND HOOK ABOVE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x0062, 0x0061, 0, 0}, 0x1eb1 }, // U1EB1 # LATIN SMALL LETTER A WITH BREVE AND GRAVE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x0062, 0x0061, 0, 0}, 0x1eaf }, // U1EAF # LATIN SMALL LETTER A WITH BREVE AND ACUTE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x0062, 0x0061, 0, 0}, 0x1eb5 }, // U1EB5 # LATIN SMALL LETTER A WITH BREVE AND TILDE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x0062, 0x0061, 0, 0}, 0x1eb3 }, // U1EB3 # LATIN SMALL LETTER A WITH BREVE AND HOOK ABOVE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x006f, 0x0041, 0, 0}, 0x01fa }, // U01FA # LATIN CAPITAL LETTER A WITH RING ABOVE AND ACUTE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x006f, 0x0061, 0, 0}, 0x01fb }, // U01FB # LATIN SMALL LETTER A WITH RING ABOVE AND ACUTE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x007e, 0x004f, 0, 0}, 0x1e4c }, // U1E4C # LATIN CAPITAL LETTER O WITH TILDE AND ACUTE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x007e, 0x0055, 0, 0}, 0x1e78 }, // U1E78 # LATIN CAPITAL LETTER U WITH TILDE AND ACUTE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x007e, 0x006f, 0, 0}, 0x1e4d }, // U1E4D # LATIN SMALL LETTER O WITH TILDE AND ACUTE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x007e, 0x0075, 0, 0}, 0x1e79 }, // U1E79 # LATIN SMALL LETTER U WITH TILDE AND ACUTE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x00af, 0x0045, 0, 0}, 0x1e16 }, // U1E16 # LATIN CAPITAL LETTER E WITH MACRON AND ACUTE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x00af, 0x0045, 0, 0}, 0x1e14 }, // U1E14 # LATIN CAPITAL LETTER E WITH MACRON AND GRAVE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x00af, 0x004f, 0, 0}, 0x1e52 }, // U1E52 # LATIN CAPITAL LETTER O WITH MACRON AND ACUTE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x00af, 0x004f, 0, 0}, 0x1e50 }, // U1E50 # LATIN CAPITAL LETTER O WITH MACRON AND GRAVE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x00af, 0x0065, 0, 0}, 0x1e15 }, // U1E15 # LATIN SMALL LETTER E WITH MACRON AND GRAVE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x00af, 0x0065, 0, 0}, 0x1e17 }, // U1E17 # LATIN SMALL LETTER E WITH MACRON AND ACUTE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x00af, 0x006f, 0, 0}, 0x1e53 }, // U1E53 # LATIN SMALL LETTER O WITH MACRON AND ACUTE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Multi_key), 0x00af, 0x006f, 0, 0}, 0x1e51 }, // U1E51 # LATIN SMALL LETTER O WITH MACRON AND GRAVE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Dead_Circumflex), 0x0041, 0, 0, 0}, 0x1eaa }, // U1EAA # LATIN CAPITAL LETTER A WITH CIRCUMFLEX AND TILDE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Dead_Circumflex), 0x0041, 0, 0, 0}, 0x1ea6 }, // U1EA6 # LATIN CAPITAL LETTER A WITH CIRCUMFLEX AND GRAVE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Dead_Circumflex), 0x0041, 0, 0, 0}, 0x1ea4 }, // U1EA4 # LATIN CAPITAL LETTER A WITH CIRCUMFLEX AND ACUTE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Dead_Circumflex), 0x0041, 0, 0, 0}, 0x1ea8 }, // U1EA8 # LATIN CAPITAL LETTER A WITH CIRCUMFLEX AND HOOK ABOVE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Dead_Circumflex), 0x0045, 0, 0, 0}, 0x1ec4 }, // U1EC4 # LATIN CAPITAL LETTER E WITH CIRCUMFLEX AND TILDE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Dead_Circumflex), 0x0045, 0, 0, 0}, 0x1ec0 }, // U1EC0 # LATIN CAPITAL LETTER E WITH CIRCUMFLEX AND GRAVE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Dead_Circumflex), 0x0045, 0, 0, 0}, 0x1ec2 }, // U1EC2 # LATIN CAPITAL LETTER E WITH CIRCUMFLEX AND HOOK ABOVE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Dead_Circumflex), 0x0045, 0, 0, 0}, 0x1ebe }, // U1EBE # LATIN CAPITAL LETTER E WITH CIRCUMFLEX AND ACUTE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Dead_Circumflex), 0x004f, 0, 0, 0}, 0x1ed0 }, // U1ED0 # LATIN CAPITAL LETTER O WITH CIRCUMFLEX AND ACUTE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Dead_Circumflex), 0x004f, 0, 0, 0}, 0x1ed2 }, // U1ED2 # LATIN CAPITAL LETTER O WITH CIRCUMFLEX AND GRAVE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Dead_Circumflex), 0x004f, 0, 0, 0}, 0x1ed4 }, // U1ED4 # LATIN CAPITAL LETTER O WITH CIRCUMFLEX AND HOOK ABOVE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Dead_Circumflex), 0x004f, 0, 0, 0}, 0x1ed6 }, // U1ED6 # LATIN CAPITAL LETTER O WITH CIRCUMFLEX AND TILDE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Dead_Circumflex), 0x0061, 0, 0, 0}, 0x1eab }, // U1EAB # LATIN SMALL LETTER A WITH CIRCUMFLEX AND TILDE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Dead_Circumflex), 0x0061, 0, 0, 0}, 0x1ea5 }, // U1EA5 # LATIN SMALL LETTER A WITH CIRCUMFLEX AND ACUTE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Dead_Circumflex), 0x0061, 0, 0, 0}, 0x1ea9 }, // U1EA9 # LATIN SMALL LETTER A WITH CIRCUMFLEX AND HOOK ABOVE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Dead_Circumflex), 0x0061, 0, 0, 0}, 0x1ea7 }, // U1EA7 # LATIN SMALL LETTER A WITH CIRCUMFLEX AND GRAVE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Dead_Circumflex), 0x0065, 0, 0, 0}, 0x1ebf }, // U1EBF # LATIN SMALL LETTER E WITH CIRCUMFLEX AND ACUTE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Dead_Circumflex), 0x0065, 0, 0, 0}, 0x1ec3 }, // U1EC3 # LATIN SMALL LETTER E WITH CIRCUMFLEX AND HOOK ABOVE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Dead_Circumflex), 0x0065, 0, 0, 0}, 0x1ec1 }, // U1EC1 # LATIN SMALL LETTER E WITH CIRCUMFLEX AND GRAVE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Dead_Circumflex), 0x0065, 0, 0, 0}, 0x1ec5 }, // U1EC5 # LATIN SMALL LETTER E WITH CIRCUMFLEX AND TILDE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Dead_Circumflex), 0x006f, 0, 0, 0}, 0x1ed3 }, // U1ED3 # LATIN SMALL LETTER O WITH CIRCUMFLEX AND GRAVE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Dead_Circumflex), 0x006f, 0, 0, 0}, 0x1ed7 }, // U1ED7 # LATIN SMALL LETTER O WITH CIRCUMFLEX AND TILDE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Dead_Circumflex), 0x006f, 0, 0, 0}, 0x1ed1 }, // U1ED1 # LATIN SMALL LETTER O WITH CIRCUMFLEX AND ACUTE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Dead_Circumflex), 0x006f, 0, 0, 0}, 0x1ed5 }, // U1ED5 # LATIN SMALL LETTER O WITH CIRCUMFLEX AND HOOK ABOVE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Dead_Tilde), 0x004f, 0, 0, 0}, 0x1e4c }, // U1E4C # LATIN CAPITAL LETTER O WITH TILDE AND ACUTE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Dead_Tilde), 0x0055, 0, 0, 0}, 0x1e78 }, // U1E78 # LATIN CAPITAL LETTER U WITH TILDE AND ACUTE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Dead_Tilde), 0x006f, 0, 0, 0}, 0x1e4d }, // U1E4D # LATIN SMALL LETTER O WITH TILDE AND ACUTE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Dead_Tilde), 0x0075, 0, 0, 0}, 0x1e79 }, // U1E79 # LATIN SMALL LETTER U WITH TILDE AND ACUTE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Dead_Macron), 0x0045, 0, 0, 0}, 0x1e16 }, // U1E16 # LATIN CAPITAL LETTER E WITH MACRON AND ACUTE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Dead_Macron), 0x0045, 0, 0, 0}, 0x1e14 }, // U1E14 # LATIN CAPITAL LETTER E WITH MACRON AND GRAVE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Dead_Macron), 0x004f, 0, 0, 0}, 0x1e52 }, // U1E52 # LATIN CAPITAL LETTER O WITH MACRON AND ACUTE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Dead_Macron), 0x004f, 0, 0, 0}, 0x1e50 }, // U1E50 # LATIN CAPITAL LETTER O WITH MACRON AND GRAVE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Dead_Macron), 0x0065, 0, 0, 0}, 0x1e17 }, // U1E17 # LATIN SMALL LETTER E WITH MACRON AND ACUTE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Dead_Macron), 0x0065, 0, 0, 0}, 0x1e15 }, // U1E15 # LATIN SMALL LETTER E WITH MACRON AND GRAVE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Dead_Macron), 0x006f, 0, 0, 0}, 0x1e53 }, // U1E53 # LATIN SMALL LETTER O WITH MACRON AND ACUTE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Dead_Macron), 0x006f, 0, 0, 0}, 0x1e51 }, // U1E51 # LATIN SMALL LETTER O WITH MACRON AND GRAVE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Dead_Breve), 0x0041, 0, 0, 0}, 0x1eb4 }, // U1EB4 # LATIN CAPITAL LETTER A WITH BREVE AND TILDE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Dead_Breve), 0x0041, 0, 0, 0}, 0x1eb0 }, // U1EB0 # LATIN CAPITAL LETTER A WITH BREVE AND GRAVE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Dead_Breve), 0x0041, 0, 0, 0}, 0x1eae }, // U1EAE # LATIN CAPITAL LETTER A WITH BREVE AND ACUTE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Dead_Breve), 0x0041, 0, 0, 0}, 0x1eb2 }, // U1EB2 # LATIN CAPITAL LETTER A WITH BREVE AND HOOK ABOVE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Dead_Breve), 0x0061, 0, 0, 0}, 0x1eb1 }, // U1EB1 # LATIN SMALL LETTER A WITH BREVE AND GRAVE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Dead_Breve), 0x0061, 0, 0, 0}, 0x1eb3 }, // U1EB3 # LATIN SMALL LETTER A WITH BREVE AND HOOK ABOVE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Dead_Breve), 0x0061, 0, 0, 0}, 0x1eaf }, // U1EAF # LATIN SMALL LETTER A WITH BREVE AND ACUTE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Dead_Breve), 0x0061, 0, 0, 0}, 0x1eb5 }, // U1EB5 # LATIN SMALL LETTER A WITH BREVE AND TILDE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Dead_Diaeresis), 0x0049, 0, 0, 0}, 0x1e2e }, // U1E2E # LATIN CAPITAL LETTER I WITH DIAERESIS AND ACUTE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Dead_Diaeresis), 0x0055, 0, 0, 0}, 0x01db }, // U01DB # LATIN CAPITAL LETTER U WITH DIAERESIS AND GRAVE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Dead_Diaeresis), 0x0055, 0, 0, 0}, 0x01d7 }, // U01D7 # LATIN CAPITAL LETTER U WITH DIAERESIS AND ACUTE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Dead_Diaeresis), 0x0069, 0, 0, 0}, 0x1e2f }, // U1E2F # LATIN SMALL LETTER I WITH DIAERESIS AND ACUTE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Dead_Diaeresis), 0x0075, 0, 0, 0}, 0x01dc }, // U01DC # LATIN SMALL LETTER U WITH DIAERESIS AND GRAVE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Dead_Diaeresis), 0x0075, 0, 0, 0}, 0x01d8 }, // U01D8 # LATIN SMALL LETTER U WITH DIAERESIS AND ACUTE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Dead_Diaeresis), 0x03b9, 0, 0, 0}, 0x0390 }, // U0390 # GREEK SMALL LETTER IOTA WITH DIALYTIKA AND TONOS
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Dead_Diaeresis), 0x03b9, 0, 0, 0}, 0x1fd2 }, // U1FD2 # GREEK SMALL LETTER IOTA WITH DIALYTIKA AND VARIA
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Dead_Diaeresis), 0x03c5, 0, 0, 0}, 0x1fe2 }, // U1FE2 # GREEK SMALL LETTER UPSILON WITH DIALYTIKA AND VARIA
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Dead_Diaeresis), 0x03c5, 0, 0, 0}, 0x03b0 }, // U03B0 # GREEK SMALL LETTER UPSILON WITH DIALYTIKA AND TONOS
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Dead_Abovering), 0x0041, 0, 0, 0}, 0x01fa }, // U01FA # LATIN CAPITAL LETTER A WITH RING ABOVE AND ACUTE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Dead_Abovering), 0x0061, 0, 0, 0}, 0x01fb }, // U01FB # LATIN SMALL LETTER A WITH RING ABOVE AND ACUTE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Dead_Cedilla), 0x0043, 0, 0, 0}, 0x1e08 }, // U1E08 # LATIN CAPITAL LETTER C WITH CEDILLA AND ACUTE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Dead_Cedilla), 0x0063, 0, 0, 0}, 0x1e09 }, // U1E09 # LATIN SMALL LETTER C WITH CEDILLA AND ACUTE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Dead_Horn), 0x004f, 0, 0, 0}, 0x1ede }, // U1EDE # LATIN CAPITAL LETTER O WITH HORN AND HOOK ABOVE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Dead_Horn), 0x004f, 0, 0, 0}, 0x1ee2 }, // U1EE2 # LATIN CAPITAL LETTER O WITH HORN AND DOT BELOW
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Dead_Horn), 0x004f, 0, 0, 0}, 0x1edc }, // U1EDC # LATIN CAPITAL LETTER O WITH HORN AND GRAVE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Dead_Horn), 0x004f, 0, 0, 0}, 0x1ee0 }, // U1EE0 # LATIN CAPITAL LETTER O WITH HORN AND TILDE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Dead_Horn), 0x004f, 0, 0, 0}, 0x1eda }, // U1EDA # LATIN CAPITAL LETTER O WITH HORN AND ACUTE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Dead_Horn), 0x0055, 0, 0, 0}, 0x1eea }, // U1EEA # LATIN CAPITAL LETTER U WITH HORN AND GRAVE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Dead_Horn), 0x0055, 0, 0, 0}, 0x1ee8 }, // U1EE8 # LATIN CAPITAL LETTER U WITH HORN AND ACUTE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Dead_Horn), 0x0055, 0, 0, 0}, 0x1ef0 }, // U1EF0 # LATIN CAPITAL LETTER U WITH HORN AND DOT BELOW
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Dead_Horn), 0x0055, 0, 0, 0}, 0x1eec }, // U1EEC # LATIN CAPITAL LETTER U WITH HORN AND HOOK ABOVE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Dead_Horn), 0x0055, 0, 0, 0}, 0x1eee }, // U1EEE # LATIN CAPITAL LETTER U WITH HORN AND TILDE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Dead_Horn), 0x006f, 0, 0, 0}, 0x1edf }, // U1EDF # LATIN SMALL LETTER O WITH HORN AND HOOK ABOVE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Dead_Horn), 0x006f, 0, 0, 0}, 0x1ee3 }, // U1EE3 # LATIN SMALL LETTER O WITH HORN AND DOT BELOW
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Dead_Horn), 0x006f, 0, 0, 0}, 0x1edd }, // U1EDD # LATIN SMALL LETTER O WITH HORN AND GRAVE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Dead_Horn), 0x006f, 0, 0, 0}, 0x1edb }, // U1EDB # LATIN SMALL LETTER O WITH HORN AND ACUTE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Dead_Horn), 0x006f, 0, 0, 0}, 0x1ee1 }, // U1EE1 # LATIN SMALL LETTER O WITH HORN AND TILDE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Dead_Horn), 0x0075, 0, 0, 0}, 0x1eed }, // U1EED # LATIN SMALL LETTER U WITH HORN AND HOOK ABOVE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Dead_Horn), 0x0075, 0, 0, 0}, 0x1eef }, // U1EEF # LATIN SMALL LETTER U WITH HORN AND TILDE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Dead_Horn), 0x0075, 0, 0, 0}, 0x1ef1 }, // U1EF1 # LATIN SMALL LETTER U WITH HORN AND DOT BELOW
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Dead_Horn), 0x0075, 0, 0, 0}, 0x1eeb }, // U1EEB # LATIN SMALL LETTER U WITH HORN AND GRAVE
// /* broken */    { {0x0000, UNITIZE(TQt::Key_Dead_Horn), 0x0075, 0, 0, 0}, 0x1ee9 }, // U1EE9 # LATIN SMALL LETTER U WITH HORN AND ACUTE
    { {0x030f, 0x0041, 0, 0, 0, 0}, 0x0200 }, // U0200 # LATIN CAPITAL LETTER A WITH DOUBLE GRAVE
    { {0x030f, 0x0045, 0, 0, 0, 0}, 0x0204 }, // U0204 # LATIN CAPITAL LETTER E WITH DOUBLE GRAVE
    { {0x030f, 0x0049, 0, 0, 0, 0}, 0x0208 }, // U0208 # LATIN CAPITAL LETTER I WITH DOUBLE GRAVE
    { {0x030f, 0x004f, 0, 0, 0, 0}, 0x020c }, // U020C # LATIN CAPITAL LETTER O WITH DOUBLE GRAVE
    { {0x030f, 0x0052, 0, 0, 0, 0}, 0x0210 }, // U0210 # LATIN CAPITAL LETTER R WITH DOUBLE GRAVE
    { {0x030f, 0x0055, 0, 0, 0, 0}, 0x0214 }, // U0214 # LATIN CAPITAL LETTER U WITH DOUBLE GRAVE
    { {0x030f, 0x0061, 0, 0, 0, 0}, 0x0201 }, // U0201 # LATIN SMALL LETTER A WITH DOUBLE GRAVE
    { {0x030f, 0x0065, 0, 0, 0, 0}, 0x0205 }, // U0205 # LATIN SMALL LETTER E WITH DOUBLE GRAVE
    { {0x030f, 0x0069, 0, 0, 0, 0}, 0x0209 }, // U0209 # LATIN SMALL LETTER I WITH DOUBLE GRAVE
    { {0x030f, 0x006f, 0, 0, 0, 0}, 0x020d }, // U020D # LATIN SMALL LETTER O WITH DOUBLE GRAVE
    { {0x030f, 0x0072, 0, 0, 0, 0}, 0x0211 }, // U0211 # LATIN SMALL LETTER R WITH DOUBLE GRAVE
    { {0x030f, 0x0075, 0, 0, 0, 0}, 0x0215 }, // U0215 # LATIN SMALL LETTER U WITH DOUBLE GRAVE
    { {0x030f, 0x0474, 0, 0, 0, 0}, 0x0476 }, // U0476 # CYRILLIC CAPITAL LETTER IZHITSA WITH DOUBLE GRAVE ACCENT
    { {0x030f, 0x0475, 0, 0, 0, 0}, 0x0477 }, // U0477 # CYRILLIC SMALL LETTER IZHITSA WITH DOUBLE GRAVE ACCENT
    { {0x0311, 0x0041, 0, 0, 0, 0}, 0x0202 }, // U0202 # LATIN CAPITAL LETTER A WITH INVERTED BREVE
    { {0x0311, 0x0045, 0, 0, 0, 0}, 0x0206 }, // U0206 # LATIN CAPITAL LETTER E WITH INVERTED BREVE
    { {0x0311, 0x0049, 0, 0, 0, 0}, 0x020a }, // U020A # LATIN CAPITAL LETTER I WITH INVERTED BREVE
    { {0x0311, 0x004f, 0, 0, 0, 0}, 0x020e }, // U020E # LATIN CAPITAL LETTER O WITH INVERTED BREVE
    { {0x0311, 0x0052, 0, 0, 0, 0}, 0x0212 }, // U0212 # LATIN CAPITAL LETTER R WITH INVERTED BREVE
    { {0x0311, 0x0055, 0, 0, 0, 0}, 0x0216 }, // U0216 # LATIN CAPITAL LETTER U WITH INVERTED BREVE
    { {0x0311, 0x0061, 0, 0, 0, 0}, 0x0203 }, // U0203 # LATIN SMALL LETTER A WITH INVERTED BREVE
    { {0x0311, 0x0065, 0, 0, 0, 0}, 0x0207 }, // U0207 # LATIN SMALL LETTER E WITH INVERTED BREVE
    { {0x0311, 0x0069, 0, 0, 0, 0}, 0x020b }, // U020B # LATIN SMALL LETTER I WITH INVERTED BREVE
    { {0x0311, 0x006f, 0, 0, 0, 0}, 0x020f }, // U020F # LATIN SMALL LETTER O WITH INVERTED BREVE
    { {0x0311, 0x0072, 0, 0, 0, 0}, 0x0213 }, // U0213 # LATIN SMALL LETTER R WITH INVERTED BREVE
    { {0x0311, 0x0075, 0, 0, 0, 0}, 0x0217 }, // U0217 # LATIN SMALL LETTER U WITH INVERTED BREVE
    { {0x0313, 0x0391, 0, 0, 0, 0}, 0x1f08 }, // U1F08 # GREEK CAPITAL LETTER ALPHA WITH PSILI
    { {0x0313, 0x0395, 0, 0, 0, 0}, 0x1f18 }, // U1F18 # GREEK CAPITAL LETTER EPSILON WITH PSILI
    { {0x0313, 0x0397, 0, 0, 0, 0}, 0x1f28 }, // U1F28 # GREEK CAPITAL LETTER ETA WITH PSILI
    { {0x0313, 0x0399, 0, 0, 0, 0}, 0x1f38 }, // U1F38 # GREEK CAPITAL LETTER IOTA WITH PSILI
    { {0x0313, 0x039f, 0, 0, 0, 0}, 0x1f48 }, // U1F48 # GREEK CAPITAL LETTER OMICRON WITH PSILI
    { {0x0313, 0x03a9, 0, 0, 0, 0}, 0x1f68 }, // U1F68 # GREEK CAPITAL LETTER OMEGA WITH PSILI
    { {0x0313, 0x03b1, 0, 0, 0, 0}, 0x1f00 }, // U1F00 # GREEK SMALL LETTER ALPHA WITH PSILI
    { {0x0313, 0x03b5, 0, 0, 0, 0}, 0x1f10 }, // U1F10 # GREEK SMALL LETTER EPSILON WITH PSILI
    { {0x0313, 0x03b7, 0, 0, 0, 0}, 0x1f20 }, // U1F20 # GREEK SMALL LETTER ETA WITH PSILI
    { {0x0313, 0x03b9, 0, 0, 0, 0}, 0x1f30 }, // U1F30 # GREEK SMALL LETTER IOTA WITH PSILI
    { {0x0313, 0x03bf, 0, 0, 0, 0}, 0x1f40 }, // U1F40 # GREEK SMALL LETTER OMICRON WITH PSILI
    { {0x0313, 0x03c1, 0, 0, 0, 0}, 0x1fe4 }, // U1FE4 # GREEK SMALL LETTER RHO WITH PSILI
    { {0x0313, 0x03c5, 0, 0, 0, 0}, 0x1f50 }, // U1F50 # GREEK SMALL LETTER UPSILON WITH PSILI
    { {0x0313, 0x03c9, 0, 0, 0, 0}, 0x1f60 }, // U1F60 # GREEK SMALL LETTER OMEGA WITH PSILI
    { {0x0314, 0x0391, 0, 0, 0, 0}, 0x1f09 }, // U1F09 # GREEK CAPITAL LETTER ALPHA WITH DASIA
    { {0x0314, 0x0395, 0, 0, 0, 0}, 0x1f19 }, // U1F19 # GREEK CAPITAL LETTER EPSILON WITH DASIA
    { {0x0314, 0x0397, 0, 0, 0, 0}, 0x1f29 }, // U1F29 # GREEK CAPITAL LETTER ETA WITH DASIA
    { {0x0314, 0x0399, 0, 0, 0, 0}, 0x1f39 }, // U1F39 # GREEK CAPITAL LETTER IOTA WITH DASIA
    { {0x0314, 0x039f, 0, 0, 0, 0}, 0x1f49 }, // U1F49 # GREEK CAPITAL LETTER OMICRON WITH DASIA
    { {0x0314, 0x03a1, 0, 0, 0, 0}, 0x1fec }, // U1FEC # GREEK CAPITAL LETTER RHO WITH DASIA
    { {0x0314, 0x03a5, 0, 0, 0, 0}, 0x1f59 }, // U1F59 # GREEK CAPITAL LETTER UPSILON WITH DASIA
    { {0x0314, 0x03a9, 0, 0, 0, 0}, 0x1f69 }, // U1F69 # GREEK CAPITAL LETTER OMEGA WITH DASIA
    { {0x0314, 0x03b1, 0, 0, 0, 0}, 0x1f01 }, // U1F01 # GREEK SMALL LETTER ALPHA WITH DASIA
    { {0x0314, 0x03b5, 0, 0, 0, 0}, 0x1f11 }, // U1F11 # GREEK SMALL LETTER EPSILON WITH DASIA
    { {0x0314, 0x03b7, 0, 0, 0, 0}, 0x1f21 }, // U1F21 # GREEK SMALL LETTER ETA WITH DASIA
    { {0x0314, 0x03b9, 0, 0, 0, 0}, 0x1f31 }, // U1F31 # GREEK SMALL LETTER IOTA WITH DASIA
    { {0x0314, 0x03bf, 0, 0, 0, 0}, 0x1f41 }, // U1F41 # GREEK SMALL LETTER OMICRON WITH DASIA
    { {0x0314, 0x03c1, 0, 0, 0, 0}, 0x1fe5 }, // U1FE5 # GREEK SMALL LETTER RHO WITH DASIA
    { {0x0314, 0x03c5, 0, 0, 0, 0}, 0x1f51 }, // U1F51 # GREEK SMALL LETTER UPSILON WITH DASIA
    { {0x0314, 0x03c9, 0, 0, 0, 0}, 0x1f61 }, // U1F61 # GREEK SMALL LETTER OMEGA WITH DASIA
    { {0x0324, 0x0055, 0, 0, 0, 0}, 0x1e72 }, // U1E72 # LATIN CAPITAL LETTER U WITH DIAERESIS BELOW
    { {0x0324, 0x0075, 0, 0, 0, 0}, 0x1e73 }, // U1E73 # LATIN SMALL LETTER U WITH DIAERESIS BELOW
    { {0x0325, 0x0041, 0, 0, 0, 0}, 0x1e00 }, // U1E00 # LATIN CAPITAL LETTER A WITH RING BELOW
    { {0x0325, 0x0061, 0, 0, 0, 0}, 0x1e01 }, // U1E01 # LATIN SMALL LETTER A WITH RING BELOW
    { {0x0326, 0x0053, 0, 0, 0, 0}, 0x0218 }, // U0218 # LATIN CAPITAL LETTER S WITH COMMA BELOW
    { {0x0326, 0x0054, 0, 0, 0, 0}, 0x021a }, // U021A # LATIN CAPITAL LETTER T WITH COMMA BELOW
    { {0x0326, 0x0073, 0, 0, 0, 0}, 0x0219 }, // U0219 # LATIN SMALL LETTER S WITH COMMA BELOW
    { {0x0326, 0x0074, 0, 0, 0, 0}, 0x021b }, // U021B # LATIN SMALL LETTER T WITH COMMA BELOW
    { {0x032d, 0x0044, 0, 0, 0, 0}, 0x1e12 }, // U1E12 # LATIN CAPITAL LETTER D WITH CIRCUMFLEX BELOW
    { {0x032d, 0x0045, 0, 0, 0, 0}, 0x1e18 }, // U1E18 # LATIN CAPITAL LETTER E WITH CIRCUMFLEX BELOW
    { {0x032d, 0x004c, 0, 0, 0, 0}, 0x1e3c }, // U1E3C # LATIN CAPITAL LETTER L WITH CIRCUMFLEX BELOW
    { {0x032d, 0x004e, 0, 0, 0, 0}, 0x1e4a }, // U1E4A # LATIN CAPITAL LETTER N WITH CIRCUMFLEX BELOW
    { {0x032d, 0x0054, 0, 0, 0, 0}, 0x1e70 }, // U1E70 # LATIN CAPITAL LETTER T WITH CIRCUMFLEX BELOW
    { {0x032d, 0x0055, 0, 0, 0, 0}, 0x1e76 }, // U1E76 # LATIN CAPITAL LETTER U WITH CIRCUMFLEX BELOW
    { {0x032d, 0x0064, 0, 0, 0, 0}, 0x1e13 }, // U1E13 # LATIN SMALL LETTER D WITH CIRCUMFLEX BELOW
    { {0x032d, 0x0065, 0, 0, 0, 0}, 0x1e19 }, // U1E19 # LATIN SMALL LETTER E WITH CIRCUMFLEX BELOW
    { {0x032d, 0x006c, 0, 0, 0, 0}, 0x1e3d }, // U1E3D # LATIN SMALL LETTER L WITH CIRCUMFLEX BELOW
    { {0x032d, 0x006e, 0, 0, 0, 0}, 0x1e4b }, // U1E4B # LATIN SMALL LETTER N WITH CIRCUMFLEX BELOW
    { {0x032d, 0x0074, 0, 0, 0, 0}, 0x1e71 }, // U1E71 # LATIN SMALL LETTER T WITH CIRCUMFLEX BELOW
    { {0x032d, 0x0075, 0, 0, 0, 0}, 0x1e77 }, // U1E77 # LATIN SMALL LETTER U WITH CIRCUMFLEX BELOW
    { {0x032e, 0x0048, 0, 0, 0, 0}, 0x1e2a }, // U1E2A # LATIN CAPITAL LETTER H WITH BREVE BELOW
    { {0x032e, 0x0068, 0, 0, 0, 0}, 0x1e2b }, // U1E2B # LATIN SMALL LETTER H WITH BREVE BELOW
    { {0x0330, 0x0045, 0, 0, 0, 0}, 0x1e1a }, // U1E1A # LATIN CAPITAL LETTER E WITH TILDE BELOW
    { {0x0330, 0x0049, 0, 0, 0, 0}, 0x1e2c }, // U1E2C # LATIN CAPITAL LETTER I WITH TILDE BELOW
    { {0x0330, 0x0055, 0, 0, 0, 0}, 0x1e74 }, // U1E74 # LATIN CAPITAL LETTER U WITH TILDE BELOW
    { {0x0330, 0x0065, 0, 0, 0, 0}, 0x1e1b }, // U1E1B # LATIN SMALL LETTER E WITH TILDE BELOW
    { {0x0330, 0x0069, 0, 0, 0, 0}, 0x1e2d }, // U1E2D # LATIN SMALL LETTER I WITH TILDE BELOW
    { {0x0330, 0x0075, 0, 0, 0, 0}, 0x1e75 }, // U1E75 # LATIN SMALL LETTER U WITH TILDE BELOW
    { {0x0331, 0x0042, 0, 0, 0, 0}, 0x1e06 }, // U1E06 # LATIN CAPITAL LETTER B WITH LINE BELOW
    { {0x0331, 0x0044, 0, 0, 0, 0}, 0x1e0e }, // U1E0E # LATIN CAPITAL LETTER D WITH LINE BELOW
    { {0x0331, 0x004b, 0, 0, 0, 0}, 0x1e34 }, // U1E34 # LATIN CAPITAL LETTER K WITH LINE BELOW
    { {0x0331, 0x004c, 0, 0, 0, 0}, 0x1e3a }, // U1E3A # LATIN CAPITAL LETTER L WITH LINE BELOW
    { {0x0331, 0x004e, 0, 0, 0, 0}, 0x1e48 }, // U1E48 # LATIN CAPITAL LETTER N WITH LINE BELOW
    { {0x0331, 0x0052, 0, 0, 0, 0}, 0x1e5e }, // U1E5E # LATIN CAPITAL LETTER R WITH LINE BELOW
    { {0x0331, 0x0054, 0, 0, 0, 0}, 0x1e6e }, // U1E6E # LATIN CAPITAL LETTER T WITH LINE BELOW
    { {0x0331, 0x005a, 0, 0, 0, 0}, 0x1e94 }, // U1E94 # LATIN CAPITAL LETTER Z WITH LINE BELOW
    { {0x0331, 0x0062, 0, 0, 0, 0}, 0x1e07 }, // U1E07 # LATIN SMALL LETTER B WITH LINE BELOW
    { {0x0331, 0x0064, 0, 0, 0, 0}, 0x1e0f }, // U1E0F # LATIN SMALL LETTER D WITH LINE BELOW
    { {0x0331, 0x0068, 0, 0, 0, 0}, 0x1e96 }, // U1E96 # LATIN SMALL LETTER H WITH LINE BELOW
    { {0x0331, 0x006b, 0, 0, 0, 0}, 0x1e35 }, // U1E35 # LATIN SMALL LETTER K WITH LINE BELOW
    { {0x0331, 0x006c, 0, 0, 0, 0}, 0x1e3b }, // U1E3B # LATIN SMALL LETTER L WITH LINE BELOW
    { {0x0331, 0x006e, 0, 0, 0, 0}, 0x1e49 }, // U1E49 # LATIN SMALL LETTER N WITH LINE BELOW
    { {0x0331, 0x0072, 0, 0, 0, 0}, 0x1e5f }, // U1E5F # LATIN SMALL LETTER R WITH LINE BELOW
    { {0x0331, 0x0074, 0, 0, 0, 0}, 0x1e6f }, // U1E6F # LATIN SMALL LETTER T WITH LINE BELOW
    { {0x0331, 0x007a, 0, 0, 0, 0}, 0x1e95 }, // U1E95 # LATIN SMALL LETTER Z WITH LINE BELOW
    { {0x0342, 0x0313, 0x0391, 0, 0, 0}, 0x1f0e }, // U1F0E # GREEK CAPITAL LETTER ALPHA WITH PSILI AND PERISPOMENI
    { {0x0342, 0x0313, 0x0397, 0, 0, 0}, 0x1f2e }, // U1F2E # GREEK CAPITAL LETTER ETA WITH PSILI AND PERISPOMENI
    { {0x0342, 0x0313, 0x0399, 0, 0, 0}, 0x1f3e }, // U1F3E # GREEK CAPITAL LETTER IOTA WITH PSILI AND PERISPOMENI
    { {0x0342, 0x0313, 0x03a9, 0, 0, 0}, 0x1f6e }, // U1F6E # GREEK CAPITAL LETTER OMEGA WITH PSILI AND PERISPOMENI
    { {0x0342, 0x0313, 0x03b1, 0, 0, 0}, 0x1f06 }, // U1F06 # GREEK SMALL LETTER ALPHA WITH PSILI AND PERISPOMENI
    { {0x0342, 0x0313, 0x03b7, 0, 0, 0}, 0x1f26 }, // U1F26 # GREEK SMALL LETTER ETA WITH PSILI AND PERISPOMENI
    { {0x0342, 0x0313, 0x03b9, 0, 0, 0}, 0x1f36 }, // U1F36 # GREEK SMALL LETTER IOTA WITH PSILI AND PERISPOMENI
    { {0x0342, 0x0313, 0x03c5, 0, 0, 0}, 0x1f56 }, // U1F56 # GREEK SMALL LETTER UPSILON WITH PSILI AND PERISPOMENI
    { {0x0342, 0x0313, 0x03c9, 0, 0, 0}, 0x1f66 }, // U1F66 # GREEK SMALL LETTER OMEGA WITH PSILI AND PERISPOMENI
    { {0x0342, 0x0314, 0x0391, 0, 0, 0}, 0x1f0f }, // U1F0F # GREEK CAPITAL LETTER ALPHA WITH DASIA AND PERISPOMENI
    { {0x0342, 0x0314, 0x0397, 0, 0, 0}, 0x1f2f }, // U1F2F # GREEK CAPITAL LETTER ETA WITH DASIA AND PERISPOMENI
    { {0x0342, 0x0314, 0x0399, 0, 0, 0}, 0x1f3f }, // U1F3F # GREEK CAPITAL LETTER IOTA WITH DASIA AND PERISPOMENI
    { {0x0342, 0x0314, 0x03a5, 0, 0, 0}, 0x1f5f }, // U1F5F # GREEK CAPITAL LETTER UPSILON WITH DASIA AND PERISPOMENI
    { {0x0342, 0x0314, 0x03a9, 0, 0, 0}, 0x1f6f }, // U1F6F # GREEK CAPITAL LETTER OMEGA WITH DASIA AND PERISPOMENI
    { {0x0342, 0x0314, 0x03b1, 0, 0, 0}, 0x1f07 }, // U1F07 # GREEK SMALL LETTER ALPHA WITH DASIA AND PERISPOMENI
    { {0x0342, 0x0314, 0x03b7, 0, 0, 0}, 0x1f27 }, // U1F27 # GREEK SMALL LETTER ETA WITH DASIA AND PERISPOMENI
    { {0x0342, 0x0314, 0x03b9, 0, 0, 0}, 0x1f37 }, // U1F37 # GREEK SMALL LETTER IOTA WITH DASIA AND PERISPOMENI
    { {0x0342, 0x0314, 0x03c5, 0, 0, 0}, 0x1f57 }, // U1F57 # GREEK SMALL LETTER UPSILON WITH DASIA AND PERISPOMENI
    { {0x0342, 0x0314, 0x03c9, 0, 0, 0}, 0x1f67 }, // U1F67 # GREEK SMALL LETTER OMEGA WITH DASIA AND PERISPOMENI
    { {0x0342, 0x03b1, 0, 0, 0, 0}, 0x1fb6 }, // U1FB6 # GREEK SMALL LETTER ALPHA WITH PERISPOMENI
    { {0x0342, 0x03b7, 0, 0, 0, 0}, 0x1fc6 }, // U1FC6 # GREEK SMALL LETTER ETA WITH PERISPOMENI
    { {0x0342, 0x03b9, 0, 0, 0, 0}, 0x1fd6 }, // U1FD6 # GREEK SMALL LETTER IOTA WITH PERISPOMENI
    { {0x0342, 0x03c5, 0, 0, 0, 0}, 0x1fe6 }, // U1FE6 # GREEK SMALL LETTER UPSILON WITH PERISPOMENI
    { {0x0342, 0x03c9, 0, 0, 0, 0}, 0x1ff6 }, // U1FF6 # GREEK SMALL LETTER OMEGA WITH PERISPOMENI
    { {0x0342, 0x03ca, 0, 0, 0, 0}, 0x1fd7 }, // U1FD7 # GREEK SMALL LETTER IOTA WITH DIALYTIKA AND PERISPOMENI
    { {0x0342, 0x03cb, 0, 0, 0, 0}, 0x1fe7 }, // U1FE7 # GREEK SMALL LETTER UPSILON WITH DIALYTIKA AND PERISPOMENI
    { {0x0342, 0x1f00, 0, 0, 0, 0}, 0x1f06 }, // U1F06 # GREEK SMALL LETTER ALPHA WITH PSILI AND PERISPOMENI
    { {0x0342, 0x1f01, 0, 0, 0, 0}, 0x1f07 }, // U1F07 # GREEK SMALL LETTER ALPHA WITH DASIA AND PERISPOMENI
    { {0x0342, 0x1f08, 0, 0, 0, 0}, 0x1f0e }, // U1F0E # GREEK CAPITAL LETTER ALPHA WITH PSILI AND PERISPOMENI
    { {0x0342, 0x1f09, 0, 0, 0, 0}, 0x1f0f }, // U1F0F # GREEK CAPITAL LETTER ALPHA WITH DASIA AND PERISPOMENI
    { {0x0342, 0x1f20, 0, 0, 0, 0}, 0x1f26 }, // U1F26 # GREEK SMALL LETTER ETA WITH PSILI AND PERISPOMENI
    { {0x0342, 0x1f21, 0, 0, 0, 0}, 0x1f27 }, // U1F27 # GREEK SMALL LETTER ETA WITH DASIA AND PERISPOMENI
    { {0x0342, 0x1f28, 0, 0, 0, 0}, 0x1f2e }, // U1F2E # GREEK CAPITAL LETTER ETA WITH PSILI AND PERISPOMENI
    { {0x0342, 0x1f29, 0, 0, 0, 0}, 0x1f2f }, // U1F2F # GREEK CAPITAL LETTER ETA WITH DASIA AND PERISPOMENI
    { {0x0342, 0x1f30, 0, 0, 0, 0}, 0x1f36 }, // U1F36 # GREEK SMALL LETTER IOTA WITH PSILI AND PERISPOMENI
    { {0x0342, 0x1f31, 0, 0, 0, 0}, 0x1f37 }, // U1F37 # GREEK SMALL LETTER IOTA WITH DASIA AND PERISPOMENI
    { {0x0342, 0x1f38, 0, 0, 0, 0}, 0x1f3e }, // U1F3E # GREEK CAPITAL LETTER IOTA WITH PSILI AND PERISPOMENI
    { {0x0342, 0x1f39, 0, 0, 0, 0}, 0x1f3f }, // U1F3F # GREEK CAPITAL LETTER IOTA WITH DASIA AND PERISPOMENI
    { {0x0342, 0x1f50, 0, 0, 0, 0}, 0x1f56 }, // U1F56 # GREEK SMALL LETTER UPSILON WITH PSILI AND PERISPOMENI
    { {0x0342, 0x1f51, 0, 0, 0, 0}, 0x1f57 }, // U1F57 # GREEK SMALL LETTER UPSILON WITH DASIA AND PERISPOMENI
    { {0x0342, 0x1f59, 0, 0, 0, 0}, 0x1f5f }, // U1F5F # GREEK CAPITAL LETTER UPSILON WITH DASIA AND PERISPOMENI
    { {0x0342, 0x1f60, 0, 0, 0, 0}, 0x1f66 }, // U1F66 # GREEK SMALL LETTER OMEGA WITH PSILI AND PERISPOMENI
    { {0x0342, 0x1f61, 0, 0, 0, 0}, 0x1f67 }, // U1F67 # GREEK SMALL LETTER OMEGA WITH DASIA AND PERISPOMENI
    { {0x0342, 0x1f68, 0, 0, 0, 0}, 0x1f6e }, // U1F6E # GREEK CAPITAL LETTER OMEGA WITH PSILI AND PERISPOMENI
    { {0x0342, 0x1f69, 0, 0, 0, 0}, 0x1f6f }, // U1F6F # GREEK CAPITAL LETTER OMEGA WITH DASIA AND PERISPOMENI
    { {0x0342, UNITIZE(TQt::Key_Multi_key), 0x0022, 0x03b9, 0, 0}, 0x1fd7 }, // U1FD7 # GREEK SMALL LETTER IOTA WITH DIALYTIKA AND PERISPOMENI
    { {0x0342, UNITIZE(TQt::Key_Multi_key), 0x0022, 0x03c5, 0, 0}, 0x1fe7 }, // U1FE7 # GREEK SMALL LETTER UPSILON WITH DIALYTIKA AND PERISPOMENI
    { {0x0342, UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0391, 0, 0}, 0x1f0f }, // U1F0F # GREEK CAPITAL LETTER ALPHA WITH DASIA AND PERISPOMENI
    { {0x0342, UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0397, 0, 0}, 0x1f2f }, // U1F2F # GREEK CAPITAL LETTER ETA WITH DASIA AND PERISPOMENI
    { {0x0342, UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0399, 0, 0}, 0x1f3f }, // U1F3F # GREEK CAPITAL LETTER IOTA WITH DASIA AND PERISPOMENI
    { {0x0342, UNITIZE(TQt::Key_Multi_key), 0x0028, 0x03a5, 0, 0}, 0x1f5f }, // U1F5F # GREEK CAPITAL LETTER UPSILON WITH DASIA AND PERISPOMENI
    { {0x0342, UNITIZE(TQt::Key_Multi_key), 0x0028, 0x03a9, 0, 0}, 0x1f6f }, // U1F6F # GREEK CAPITAL LETTER OMEGA WITH DASIA AND PERISPOMENI
    { {0x0342, UNITIZE(TQt::Key_Multi_key), 0x0028, 0x03b1, 0, 0}, 0x1f07 }, // U1F07 # GREEK SMALL LETTER ALPHA WITH DASIA AND PERISPOMENI
    { {0x0342, UNITIZE(TQt::Key_Multi_key), 0x0028, 0x03b7, 0, 0}, 0x1f27 }, // U1F27 # GREEK SMALL LETTER ETA WITH DASIA AND PERISPOMENI
    { {0x0342, UNITIZE(TQt::Key_Multi_key), 0x0028, 0x03b9, 0, 0}, 0x1f37 }, // U1F37 # GREEK SMALL LETTER IOTA WITH DASIA AND PERISPOMENI
    { {0x0342, UNITIZE(TQt::Key_Multi_key), 0x0028, 0x03c5, 0, 0}, 0x1f57 }, // U1F57 # GREEK SMALL LETTER UPSILON WITH DASIA AND PERISPOMENI
    { {0x0342, UNITIZE(TQt::Key_Multi_key), 0x0028, 0x03c9, 0, 0}, 0x1f67 }, // U1F67 # GREEK SMALL LETTER OMEGA WITH DASIA AND PERISPOMENI
    { {0x0342, UNITIZE(TQt::Key_Multi_key), 0x0029, 0x0391, 0, 0}, 0x1f0e }, // U1F0E # GREEK CAPITAL LETTER ALPHA WITH PSILI AND PERISPOMENI
    { {0x0342, UNITIZE(TQt::Key_Multi_key), 0x0029, 0x0397, 0, 0}, 0x1f2e }, // U1F2E # GREEK CAPITAL LETTER ETA WITH PSILI AND PERISPOMENI
    { {0x0342, UNITIZE(TQt::Key_Multi_key), 0x0029, 0x0399, 0, 0}, 0x1f3e }, // U1F3E # GREEK CAPITAL LETTER IOTA WITH PSILI AND PERISPOMENI
    { {0x0342, UNITIZE(TQt::Key_Multi_key), 0x0029, 0x03a9, 0, 0}, 0x1f6e }, // U1F6E # GREEK CAPITAL LETTER OMEGA WITH PSILI AND PERISPOMENI
    { {0x0342, UNITIZE(TQt::Key_Multi_key), 0x0029, 0x03b1, 0, 0}, 0x1f06 }, // U1F06 # GREEK SMALL LETTER ALPHA WITH PSILI AND PERISPOMENI
    { {0x0342, UNITIZE(TQt::Key_Multi_key), 0x0029, 0x03b7, 0, 0}, 0x1f26 }, // U1F26 # GREEK SMALL LETTER ETA WITH PSILI AND PERISPOMENI
    { {0x0342, UNITIZE(TQt::Key_Multi_key), 0x0029, 0x03b9, 0, 0}, 0x1f36 }, // U1F36 # GREEK SMALL LETTER IOTA WITH PSILI AND PERISPOMENI
    { {0x0342, UNITIZE(TQt::Key_Multi_key), 0x0029, 0x03c5, 0, 0}, 0x1f56 }, // U1F56 # GREEK SMALL LETTER UPSILON WITH PSILI AND PERISPOMENI
    { {0x0342, UNITIZE(TQt::Key_Multi_key), 0x0029, 0x03c9, 0, 0}, 0x1f66 }, // U1F66 # GREEK SMALL LETTER OMEGA WITH PSILI AND PERISPOMENI
    { {0x0342, UNITIZE(TQt::Key_Dead_Diaeresis), 0x03b9, 0, 0, 0}, 0x1fd7 }, // U1FD7 # GREEK SMALL LETTER IOTA WITH DIALYTIKA AND PERISPOMENI
    { {0x0342, UNITIZE(TQt::Key_Dead_Diaeresis), 0x03c5, 0, 0, 0}, 0x1fe7 }, // U1FE7 # GREEK SMALL LETTER UPSILON WITH DIALYTIKA AND PERISPOMENI
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0000, 0x0044, 0, 0, 0}, 0x0110 }, // U0110 # LATIN CAPITAL LETTER D WITH STROKE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0000, 0x0047, 0, 0, 0}, 0x01e4 }, // U01E4 # LATIN CAPITAL LETTER G WITH STROKE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0000, 0x0048, 0, 0, 0}, 0x0126 }, // U0126 # LATIN CAPITAL LETTER H WITH STROKE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0000, 0x0049, 0, 0, 0}, 0x0197 }, // U0197 # LATIN CAPITAL LETTER I WITH STROKE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0000, 0x004c, 0, 0, 0}, 0x0141 }, // U0141 # LATIN CAPITAL LETTER L WITH STROKE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0000, 0x004f, 0, 0, 0}, 0x00d8 }, // U00D8 # LATIN CAPITAL LETTER O WITH STROKE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0000, 0x0054, 0, 0, 0}, 0x0166 }, // U0166 # LATIN CAPITAL LETTER T WITH STROKE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0000, 0x005a, 0, 0, 0}, 0x01b5 }, // U01B5 # LATIN CAPITAL LETTER Z WITH STROKE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0000, 0x0062, 0, 0, 0}, 0x0180 }, // U0180 # LATIN SMALL LETTER B WITH STROKE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0000, 0x0064, 0, 0, 0}, 0x0111 }, // U0111 # LATIN SMALL LETTER D WITH STROKE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0000, 0x0067, 0, 0, 0}, 0x01e5 }, // U01E5 # LATIN SMALL LETTER G WITH STROKE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0000, 0x0068, 0, 0, 0}, 0x0127 }, // U0127 # LATIN SMALL LETTER H WITH STROKE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0000, 0x0069, 0, 0, 0}, 0x0268 }, // U0268 # LATIN SMALL LETTER I WITH STROKE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0000, 0x006c, 0, 0, 0}, 0x0142 }, // U0142 # LATIN SMALL LETTER L WITH STROKE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0000, 0x006f, 0, 0, 0}, 0x00f8 }, // U00F8 # LATIN SMALL LETTER O WITH STROKE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0000, 0x0074, 0, 0, 0}, 0x0167 }, // U0167 # LATIN SMALL LETTER T WITH STROKE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0000, 0x007a, 0, 0, 0}, 0x01b6 }, // U01B6 # LATIN SMALL LETTER Z WITH STROKE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0000, 0x0294, 0, 0, 0}, 0x02a1 }, // U02A1 # LATIN LETTER GLOTTAL STOP WITH STROKE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0000, 0x0338, 0, 0, 0}, 0x2260 }, // U2260 # NOT EQUAL TO
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0000, 0x0413, 0, 0, 0}, 0x0492 }, // U0492 # CYRILLIC CAPITAL LETTER GHE WITH STROKE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0000, 0x041a, 0, 0, 0}, 0x049e }, // U049E # CYRILLIC CAPITAL LETTER KA WITH STROKE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0000, 0x0433, 0, 0, 0}, 0x0493 }, // U0493 # CYRILLIC SMALL LETTER GHE WITH STROKE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0000, 0x043a, 0, 0, 0}, 0x049f }, // U049F # CYRILLIC SMALL LETTER KA WITH STROKE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0000, 0x04ae, 0, 0, 0}, 0x04b0 }, // U04B0 # CYRILLIC CAPITAL LETTER STRAIGHT U WITH STROKE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0000, 0x04af, 0, 0, 0}, 0x04b1 }, // U04B1 # CYRILLIC SMALL LETTER STRAIGHT U WITH STROKE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0000, 0x2190, 0, 0, 0}, 0x219a }, // U219A # LEFTWARDS ARROW WITH STROKE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0000, 0x2192, 0, 0, 0}, 0x219b }, // U219B # RIGHTWARDS ARROW WITH STROKE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0000, 0x2194, 0, 0, 0}, 0x21ae }, // U21AE # LEFT RIGHT ARROW WITH STROKE
    { {UNITIZE(TQt::Key_Multi_key), 0x0020, 0x0020, 0, 0, 0}, 0x00a0 }, // nobreakspace # NO-BREAK SPACE
    { {UNITIZE(TQt::Key_Multi_key), 0x0020, 0x0027, 0, 0, 0}, 0x0027 }, // apostrophe
    { {UNITIZE(TQt::Key_Multi_key), 0x0020, 0x002c, 0, 0, 0}, 0x00b8 }, // cedilla
    { {UNITIZE(TQt::Key_Multi_key), 0x0020, 0x002d, 0, 0, 0}, 0x007e }, // asciitilde
    { {UNITIZE(TQt::Key_Multi_key), 0x0020, 0x002e, 0, 0, 0}, 0x2008 }, // U2008 # PUNCTUATION SPACE
    { {UNITIZE(TQt::Key_Multi_key), 0x0020, 0x003e, 0, 0, 0}, 0x005e }, // asciicircum
    { {UNITIZE(TQt::Key_Multi_key), 0x0020, 0x005e, 0, 0, 0}, 0x005e }, // asciicircum
    { {UNITIZE(TQt::Key_Multi_key), 0x0020, 0x0060, 0, 0, 0}, 0x0060 }, // grave
    { {UNITIZE(TQt::Key_Multi_key), 0x0020, 0x007e, 0, 0, 0}, 0x007e }, // asciitilde
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0021, 0x0000, 0, 0, 0}, 0x1ef0 }, // U1EF0 # LATIN CAPITAL LETTER U WITH HORN AND DOT BELOW
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0021, 0x0000, 0, 0, 0}, 0x1ee3 }, // U1EE3 # LATIN SMALL LETTER O WITH HORN AND DOT BELOW
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0021, 0x0000, 0, 0, 0}, 0x1ee2 }, // U1EE2 # LATIN CAPITAL LETTER O WITH HORN AND DOT BELOW
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0021, 0x0000, 0, 0, 0}, 0x1ef1 }, // U1EF1 # LATIN SMALL LETTER U WITH HORN AND DOT BELOW
    { {UNITIZE(TQt::Key_Multi_key), 0x0021, 0x0021, 0, 0, 0}, 0x00a1 }, // exclamdown
    { {UNITIZE(TQt::Key_Multi_key), 0x0021, 0x002b, 0x004f, 0, 0}, 0x1ee2 }, // U1EE2 # LATIN CAPITAL LETTER O WITH HORN AND DOT BELOW
    { {UNITIZE(TQt::Key_Multi_key), 0x0021, 0x002b, 0x0055, 0, 0}, 0x1ef0 }, // U1EF0 # LATIN CAPITAL LETTER U WITH HORN AND DOT BELOW
    { {UNITIZE(TQt::Key_Multi_key), 0x0021, 0x002b, 0x006f, 0, 0}, 0x1ee3 }, // U1EE3 # LATIN SMALL LETTER O WITH HORN AND DOT BELOW
    { {UNITIZE(TQt::Key_Multi_key), 0x0021, 0x002b, 0x0075, 0, 0}, 0x1ef1 }, // U1EF1 # LATIN SMALL LETTER U WITH HORN AND DOT BELOW
    { {UNITIZE(TQt::Key_Multi_key), 0x0021, 0x0041, 0, 0, 0}, 0x1ea0 }, // U1EA0 # LATIN CAPITAL LETTER A WITH DOT BELOW
    { {UNITIZE(TQt::Key_Multi_key), 0x0021, 0x0042, 0, 0, 0}, 0x1e04 }, // U1E04 # LATIN CAPITAL LETTER B WITH DOT BELOW
    { {UNITIZE(TQt::Key_Multi_key), 0x0021, 0x0044, 0, 0, 0}, 0x1e0c }, // U1E0C # LATIN CAPITAL LETTER D WITH DOT BELOW
    { {UNITIZE(TQt::Key_Multi_key), 0x0021, 0x0045, 0, 0, 0}, 0x1eb8 }, // U1EB8 # LATIN CAPITAL LETTER E WITH DOT BELOW
    { {UNITIZE(TQt::Key_Multi_key), 0x0021, 0x0048, 0, 0, 0}, 0x1e24 }, // U1E24 # LATIN CAPITAL LETTER H WITH DOT BELOW
    { {UNITIZE(TQt::Key_Multi_key), 0x0021, 0x0049, 0, 0, 0}, 0x1eca }, // U1ECA # LATIN CAPITAL LETTER I WITH DOT BELOW
    { {UNITIZE(TQt::Key_Multi_key), 0x0021, 0x004b, 0, 0, 0}, 0x1e32 }, // U1E32 # LATIN CAPITAL LETTER K WITH DOT BELOW
    { {UNITIZE(TQt::Key_Multi_key), 0x0021, 0x004c, 0, 0, 0}, 0x1e36 }, // U1E36 # LATIN CAPITAL LETTER L WITH DOT BELOW
    { {UNITIZE(TQt::Key_Multi_key), 0x0021, 0x004d, 0, 0, 0}, 0x1e42 }, // U1E42 # LATIN CAPITAL LETTER M WITH DOT BELOW
    { {UNITIZE(TQt::Key_Multi_key), 0x0021, 0x004e, 0, 0, 0}, 0x1e46 }, // U1E46 # LATIN CAPITAL LETTER N WITH DOT BELOW
    { {UNITIZE(TQt::Key_Multi_key), 0x0021, 0x004f, 0, 0, 0}, 0x1ecc }, // U1ECC # LATIN CAPITAL LETTER O WITH DOT BELOW
    { {UNITIZE(TQt::Key_Multi_key), 0x0021, 0x0052, 0, 0, 0}, 0x1e5a }, // U1E5A # LATIN CAPITAL LETTER R WITH DOT BELOW
    { {UNITIZE(TQt::Key_Multi_key), 0x0021, 0x0053, 0, 0, 0}, 0x1e62 }, // U1E62 # LATIN CAPITAL LETTER S WITH DOT BELOW
    { {UNITIZE(TQt::Key_Multi_key), 0x0021, 0x0054, 0, 0, 0}, 0x1e6c }, // U1E6C # LATIN CAPITAL LETTER T WITH DOT BELOW
    { {UNITIZE(TQt::Key_Multi_key), 0x0021, 0x0055, 0, 0, 0}, 0x1ee4 }, // U1EE4 # LATIN CAPITAL LETTER U WITH DOT BELOW
    { {UNITIZE(TQt::Key_Multi_key), 0x0021, 0x0056, 0, 0, 0}, 0x1e7e }, // U1E7E # LATIN CAPITAL LETTER V WITH DOT BELOW
    { {UNITIZE(TQt::Key_Multi_key), 0x0021, 0x0057, 0, 0, 0}, 0x1e88 }, // U1E88 # LATIN CAPITAL LETTER W WITH DOT BELOW
    { {UNITIZE(TQt::Key_Multi_key), 0x0021, 0x0059, 0, 0, 0}, 0x1ef4 }, // U1EF4 # LATIN CAPITAL LETTER Y WITH DOT BELOW
    { {UNITIZE(TQt::Key_Multi_key), 0x0021, 0x005a, 0, 0, 0}, 0x1e92 }, // U1E92 # LATIN CAPITAL LETTER Z WITH DOT BELOW
    { {UNITIZE(TQt::Key_Multi_key), 0x0021, 0x005e, 0, 0, 0}, 0x00a6 }, // brokenbar
    { {UNITIZE(TQt::Key_Multi_key), 0x0021, 0x0061, 0, 0, 0}, 0x1ea1 }, // U1EA1 # LATIN SMALL LETTER A WITH DOT BELOW
    { {UNITIZE(TQt::Key_Multi_key), 0x0021, 0x0062, 0, 0, 0}, 0x1e05 }, // U1E05 # LATIN SMALL LETTER B WITH DOT BELOW
    { {UNITIZE(TQt::Key_Multi_key), 0x0021, 0x0064, 0, 0, 0}, 0x1e0d }, // U1E0D # LATIN SMALL LETTER D WITH DOT BELOW
    { {UNITIZE(TQt::Key_Multi_key), 0x0021, 0x0065, 0, 0, 0}, 0x1eb9 }, // U1EB9 # LATIN SMALL LETTER E WITH DOT BELOW
    { {UNITIZE(TQt::Key_Multi_key), 0x0021, 0x0068, 0, 0, 0}, 0x1e25 }, // U1E25 # LATIN SMALL LETTER H WITH DOT BELOW
    { {UNITIZE(TQt::Key_Multi_key), 0x0021, 0x0069, 0, 0, 0}, 0x1ecb }, // U1ECB # LATIN SMALL LETTER I WITH DOT BELOW
    { {UNITIZE(TQt::Key_Multi_key), 0x0021, 0x006b, 0, 0, 0}, 0x1e33 }, // U1E33 # LATIN SMALL LETTER K WITH DOT BELOW
    { {UNITIZE(TQt::Key_Multi_key), 0x0021, 0x006c, 0, 0, 0}, 0x1e37 }, // U1E37 # LATIN SMALL LETTER L WITH DOT BELOW
    { {UNITIZE(TQt::Key_Multi_key), 0x0021, 0x006d, 0, 0, 0}, 0x1e43 }, // U1E43 # LATIN SMALL LETTER M WITH DOT BELOW
    { {UNITIZE(TQt::Key_Multi_key), 0x0021, 0x006e, 0, 0, 0}, 0x1e47 }, // U1E47 # LATIN SMALL LETTER N WITH DOT BELOW
    { {UNITIZE(TQt::Key_Multi_key), 0x0021, 0x006f, 0, 0, 0}, 0x1ecd }, // U1ECD # LATIN SMALL LETTER O WITH DOT BELOW
    { {UNITIZE(TQt::Key_Multi_key), 0x0021, 0x0072, 0, 0, 0}, 0x1e5b }, // U1E5B # LATIN SMALL LETTER R WITH DOT BELOW
    { {UNITIZE(TQt::Key_Multi_key), 0x0021, 0x0073, 0, 0, 0}, 0x1e63 }, // U1E63 # LATIN SMALL LETTER S WITH DOT BELOW
    { {UNITIZE(TQt::Key_Multi_key), 0x0021, 0x0074, 0, 0, 0}, 0x1e6d }, // U1E6D # LATIN SMALL LETTER T WITH DOT BELOW
    { {UNITIZE(TQt::Key_Multi_key), 0x0021, 0x0075, 0, 0, 0}, 0x1ee5 }, // U1EE5 # LATIN SMALL LETTER U WITH DOT BELOW
    { {UNITIZE(TQt::Key_Multi_key), 0x0021, 0x0076, 0, 0, 0}, 0x1e7f }, // U1E7F # LATIN SMALL LETTER V WITH DOT BELOW
    { {UNITIZE(TQt::Key_Multi_key), 0x0021, 0x0077, 0, 0, 0}, 0x1e89 }, // U1E89 # LATIN SMALL LETTER W WITH DOT BELOW
    { {UNITIZE(TQt::Key_Multi_key), 0x0021, 0x0079, 0, 0, 0}, 0x1ef5 }, // U1EF5 # LATIN SMALL LETTER Y WITH DOT BELOW
    { {UNITIZE(TQt::Key_Multi_key), 0x0021, 0x007a, 0, 0, 0}, 0x1e93 }, // U1E93 # LATIN SMALL LETTER Z WITH DOT BELOW
    { {UNITIZE(TQt::Key_Multi_key), 0x0021, UNITIZE(TQt::Key_Dead_Horn), 0x004f, 0, 0}, 0x1ee2 }, // U1EE2 # LATIN CAPITAL LETTER O WITH HORN AND DOT BELOW
    { {UNITIZE(TQt::Key_Multi_key), 0x0021, UNITIZE(TQt::Key_Dead_Horn), 0x0055, 0, 0}, 0x1ef0 }, // U1EF0 # LATIN CAPITAL LETTER U WITH HORN AND DOT BELOW
    { {UNITIZE(TQt::Key_Multi_key), 0x0021, UNITIZE(TQt::Key_Dead_Horn), 0x006f, 0, 0}, 0x1ee3 }, // U1EE3 # LATIN SMALL LETTER O WITH HORN AND DOT BELOW
    { {UNITIZE(TQt::Key_Multi_key), 0x0021, UNITIZE(TQt::Key_Dead_Horn), 0x0075, 0, 0}, 0x1ef1 }, // U1EF1 # LATIN SMALL LETTER U WITH HORN AND DOT BELOW
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0022, 0x0000, 0, 0, 0}, 0x0344 }, // U0344 # COMBINING GREEK DIALYTIKA TONOS
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0022, 0x0000, 0x004f, 0, 0}, 0x1e4e }, // U1E4E # LATIN CAPITAL LETTER O WITH TILDE AND DIAERESIS
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0022, 0x0000, 0x006f, 0, 0}, 0x1e4f }, // U1E4F # LATIN SMALL LETTER O WITH TILDE AND DIAERESIS
    { {UNITIZE(TQt::Key_Multi_key), 0x0022, 0x0027, 0, 0, 0}, 0x0344 }, // U0344 # COMBINING GREEK DIALYTIKA TONOS
    { {UNITIZE(TQt::Key_Multi_key), 0x0022, 0x002c, 0, 0, 0}, 0x201e }, // U201e # DOUBLE LOW-9 QUOTATION MARK
    { {UNITIZE(TQt::Key_Multi_key), 0x0022, 0x002f, 0, 0, 0}, 0x301e }, // U301e # DOUBLE PRIME QUOTATION MARK
    { {UNITIZE(TQt::Key_Multi_key), 0x0022, 0x003c, 0, 0, 0}, 0x201c }, // U201c # LEFT DOUBLE QUOTATION MARK
    { {UNITIZE(TQt::Key_Multi_key), 0x0022, 0x003e, 0, 0, 0}, 0x201d }, // U201d # RIGHT DOUBLE QUOTATION MARK
    { {UNITIZE(TQt::Key_Multi_key), 0x0022, 0x0041, 0, 0, 0}, 0x00c4 }, // U00C4 # LATIN CAPITAL LETTER A WITH DIAERESIS
    { {UNITIZE(TQt::Key_Multi_key), 0x0022, 0x0045, 0, 0, 0}, 0x00cb }, // U00CB # LATIN CAPITAL LETTER E WITH DIAERESIS
    { {UNITIZE(TQt::Key_Multi_key), 0x0022, 0x0048, 0, 0, 0}, 0x1e26 }, // U1E26 # LATIN CAPITAL LETTER H WITH DIAERESIS
    { {UNITIZE(TQt::Key_Multi_key), 0x0022, 0x0049, 0, 0, 0}, 0x00cf }, // U00CF # LATIN CAPITAL LETTER I WITH DIAERESIS
    { {UNITIZE(TQt::Key_Multi_key), 0x0022, 0x004f, 0, 0, 0}, 0x00d6 }, // U00D6 # LATIN CAPITAL LETTER O WITH DIAERESIS
    { {UNITIZE(TQt::Key_Multi_key), 0x0022, 0x0055, 0, 0, 0}, 0x00dc }, // U00DC # LATIN CAPITAL LETTER U WITH DIAERESIS
    { {UNITIZE(TQt::Key_Multi_key), 0x0022, 0x0057, 0, 0, 0}, 0x1e84 }, // U1E84 # LATIN CAPITAL LETTER W WITH DIAERESIS
    { {UNITIZE(TQt::Key_Multi_key), 0x0022, 0x0058, 0, 0, 0}, 0x1e8c }, // U1E8C # LATIN CAPITAL LETTER X WITH DIAERESIS
    { {UNITIZE(TQt::Key_Multi_key), 0x0022, 0x0059, 0, 0, 0}, 0x0178 }, // U0178 # LATIN CAPITAL LETTER Y WITH DIAERESIS
    { {UNITIZE(TQt::Key_Multi_key), 0x0022, 0x005c, 0, 0, 0}, 0x301d }, // U301d # REVERSED DOUBLE PRIME QUOTATION MARK
    { {UNITIZE(TQt::Key_Multi_key), 0x0022, 0x005f, 0x0055, 0, 0}, 0x1e7a }, // U1E7A # LATIN CAPITAL LETTER U WITH MACRON AND DIAERESIS
    { {UNITIZE(TQt::Key_Multi_key), 0x0022, 0x005f, 0x0075, 0, 0}, 0x1e7b }, // U1E7B # LATIN SMALL LETTER U WITH MACRON AND DIAERESIS
    { {UNITIZE(TQt::Key_Multi_key), 0x0022, 0x0061, 0, 0, 0}, 0x00e4 }, // U00E4 # LATIN SMALL LETTER A WITH DIAERESIS
    { {UNITIZE(TQt::Key_Multi_key), 0x0022, 0x0065, 0, 0, 0}, 0x00eb }, // U00EB # LATIN SMALL LETTER E WITH DIAERESIS
    { {UNITIZE(TQt::Key_Multi_key), 0x0022, 0x0068, 0, 0, 0}, 0x1e27 }, // U1E27 # LATIN SMALL LETTER H WITH DIAERESIS
    { {UNITIZE(TQt::Key_Multi_key), 0x0022, 0x0069, 0, 0, 0}, 0x00ef }, // U00EF # LATIN SMALL LETTER I WITH DIAERESIS
    { {UNITIZE(TQt::Key_Multi_key), 0x0022, 0x006f, 0, 0, 0}, 0x00f6 }, // U00F6 # LATIN SMALL LETTER O WITH DIAERESIS
    { {UNITIZE(TQt::Key_Multi_key), 0x0022, 0x0074, 0, 0, 0}, 0x1e97 }, // U1E97 # LATIN SMALL LETTER T WITH DIAERESIS
    { {UNITIZE(TQt::Key_Multi_key), 0x0022, 0x0075, 0, 0, 0}, 0x00fc }, // U00FC # LATIN SMALL LETTER U WITH DIAERESIS
    { {UNITIZE(TQt::Key_Multi_key), 0x0022, 0x0077, 0, 0, 0}, 0x1e85 }, // U1E85 # LATIN SMALL LETTER W WITH DIAERESIS
    { {UNITIZE(TQt::Key_Multi_key), 0x0022, 0x0078, 0, 0, 0}, 0x1e8d }, // U1E8D # LATIN SMALL LETTER X WITH DIAERESIS
    { {UNITIZE(TQt::Key_Multi_key), 0x0022, 0x0079, 0, 0, 0}, 0x00ff }, // U00FF # LATIN SMALL LETTER Y WITH DIAERESIS
    { {UNITIZE(TQt::Key_Multi_key), 0x0022, 0x007e, 0x004f, 0, 0}, 0x1e4e }, // U1E4E # LATIN CAPITAL LETTER O WITH TILDE AND DIAERESIS
    { {UNITIZE(TQt::Key_Multi_key), 0x0022, 0x007e, 0x006f, 0, 0}, 0x1e4f }, // U1E4F # LATIN SMALL LETTER O WITH TILDE AND DIAERESIS
    { {UNITIZE(TQt::Key_Multi_key), 0x0022, 0x00af, 0x0055, 0, 0}, 0x1e7a }, // U1E7A # LATIN CAPITAL LETTER U WITH MACRON AND DIAERESIS
    { {UNITIZE(TQt::Key_Multi_key), 0x0022, 0x00af, 0x0075, 0, 0}, 0x1e7b }, // U1E7B # LATIN SMALL LETTER U WITH MACRON AND DIAERESIS
    { {UNITIZE(TQt::Key_Multi_key), 0x0022, 0x00b4, 0, 0, 0}, 0x0344 }, // U0344 # COMBINING GREEK DIALYTIKA TONOS
    { {UNITIZE(TQt::Key_Multi_key), 0x0022, 0x00d5, 0, 0, 0}, 0x1e4e }, // U1E4E # LATIN CAPITAL LETTER O WITH TILDE AND DIAERESIS
    { {UNITIZE(TQt::Key_Multi_key), 0x0022, 0x00f5, 0, 0, 0}, 0x1e4f }, // U1E4F # LATIN SMALL LETTER O WITH TILDE AND DIAERESIS
    { {UNITIZE(TQt::Key_Multi_key), 0x0022, 0x016a, 0, 0, 0}, 0x1e7a }, // U1E7A # LATIN CAPITAL LETTER U WITH MACRON AND DIAERESIS
    { {UNITIZE(TQt::Key_Multi_key), 0x0022, 0x016b, 0, 0, 0}, 0x1e7b }, // U1E7B # LATIN SMALL LETTER U WITH MACRON AND DIAERESIS
    { {UNITIZE(TQt::Key_Multi_key), 0x0022, 0x0399, 0, 0, 0}, 0x03aa }, // U03AA # GREEK CAPITAL LETTER IOTA WITH DIALYTIKA
    { {UNITIZE(TQt::Key_Multi_key), 0x0022, 0x03a5, 0, 0, 0}, 0x03ab }, // U03AB # GREEK CAPITAL LETTER UPSILON WITH DIALYTIKA
    { {UNITIZE(TQt::Key_Multi_key), 0x0022, 0x03b9, 0, 0, 0}, 0x03ca }, // U03CA # GREEK SMALL LETTER IOTA WITH DIALYTIKA
    { {UNITIZE(TQt::Key_Multi_key), 0x0022, 0x03c5, 0, 0, 0}, 0x03cb }, // U03CB # GREEK SMALL LETTER UPSILON WITH DIALYTIKA
    { {UNITIZE(TQt::Key_Multi_key), 0x0022, 0x03d2, 0, 0, 0}, 0x03d4 }, // U03D4 # GREEK UPSILON WITH DIAERESIS AND HOOK SYMBOL
    { {UNITIZE(TQt::Key_Multi_key), 0x0022, 0x0406, 0, 0, 0}, 0x0407 }, // U0407 # CYRILLIC CAPITAL LETTER YI
    { {UNITIZE(TQt::Key_Multi_key), 0x0022, 0x0410, 0, 0, 0}, 0x04d2 }, // U04D2 # CYRILLIC CAPITAL LETTER A WITH DIAERESIS
    { {UNITIZE(TQt::Key_Multi_key), 0x0022, 0x0415, 0, 0, 0}, 0x0401 }, // U0401 # CYRILLIC CAPITAL LETTER IO
    { {UNITIZE(TQt::Key_Multi_key), 0x0022, 0x0416, 0, 0, 0}, 0x04dc }, // U04DC # CYRILLIC CAPITAL LETTER ZHE WITH DIAERESIS
    { {UNITIZE(TQt::Key_Multi_key), 0x0022, 0x0417, 0, 0, 0}, 0x04de }, // U04DE # CYRILLIC CAPITAL LETTER ZE WITH DIAERESIS
    { {UNITIZE(TQt::Key_Multi_key), 0x0022, 0x0418, 0, 0, 0}, 0x04e4 }, // U04E4 # CYRILLIC CAPITAL LETTER I WITH DIAERESIS
    { {UNITIZE(TQt::Key_Multi_key), 0x0022, 0x041e, 0, 0, 0}, 0x04e6 }, // U04E6 # CYRILLIC CAPITAL LETTER O WITH DIAERESIS
    { {UNITIZE(TQt::Key_Multi_key), 0x0022, 0x0423, 0, 0, 0}, 0x04f0 }, // U04F0 # CYRILLIC CAPITAL LETTER U WITH DIAERESIS
    { {UNITIZE(TQt::Key_Multi_key), 0x0022, 0x0427, 0, 0, 0}, 0x04f4 }, // U04F4 # CYRILLIC CAPITAL LETTER CHE WITH DIAERESIS
    { {UNITIZE(TQt::Key_Multi_key), 0x0022, 0x042b, 0, 0, 0}, 0x04f8 }, // U04F8 # CYRILLIC CAPITAL LETTER YERU WITH DIAERESIS
    { {UNITIZE(TQt::Key_Multi_key), 0x0022, 0x042d, 0, 0, 0}, 0x04ec }, // U04EC # CYRILLIC CAPITAL LETTER E WITH DIAERESIS
    { {UNITIZE(TQt::Key_Multi_key), 0x0022, 0x0430, 0, 0, 0}, 0x04d3 }, // U04D3 # CYRILLIC SMALL LETTER A WITH DIAERESIS
    { {UNITIZE(TQt::Key_Multi_key), 0x0022, 0x0435, 0, 0, 0}, 0x0451 }, // U0451 # CYRILLIC SMALL LETTER IO
    { {UNITIZE(TQt::Key_Multi_key), 0x0022, 0x0436, 0, 0, 0}, 0x04dd }, // U04DD # CYRILLIC SMALL LETTER ZHE WITH DIAERESIS
    { {UNITIZE(TQt::Key_Multi_key), 0x0022, 0x0437, 0, 0, 0}, 0x04df }, // U04DF # CYRILLIC SMALL LETTER ZE WITH DIAERESIS
    { {UNITIZE(TQt::Key_Multi_key), 0x0022, 0x0438, 0, 0, 0}, 0x04e5 }, // U04E5 # CYRILLIC SMALL LETTER I WITH DIAERESIS
    { {UNITIZE(TQt::Key_Multi_key), 0x0022, 0x043e, 0, 0, 0}, 0x04e7 }, // U04E7 # CYRILLIC SMALL LETTER O WITH DIAERESIS
    { {UNITIZE(TQt::Key_Multi_key), 0x0022, 0x0443, 0, 0, 0}, 0x04f1 }, // U04F1 # CYRILLIC SMALL LETTER U WITH DIAERESIS
    { {UNITIZE(TQt::Key_Multi_key), 0x0022, 0x0447, 0, 0, 0}, 0x04f5 }, // U04F5 # CYRILLIC SMALL LETTER CHE WITH DIAERESIS
    { {UNITIZE(TQt::Key_Multi_key), 0x0022, 0x044b, 0, 0, 0}, 0x04f9 }, // U04F9 # CYRILLIC SMALL LETTER YERU WITH DIAERESIS
    { {UNITIZE(TQt::Key_Multi_key), 0x0022, 0x044d, 0, 0, 0}, 0x04ed }, // U04ED # CYRILLIC SMALL LETTER E WITH DIAERESIS
    { {UNITIZE(TQt::Key_Multi_key), 0x0022, 0x0456, 0, 0, 0}, 0x0457 }, // U0457 # CYRILLIC SMALL LETTER YI
    { {UNITIZE(TQt::Key_Multi_key), 0x0022, 0x04d8, 0, 0, 0}, 0x04da }, // U04DA # CYRILLIC CAPITAL LETTER SCHWA WITH DIAERESIS
    { {UNITIZE(TQt::Key_Multi_key), 0x0022, 0x04d9, 0, 0, 0}, 0x04db }, // U04DB # CYRILLIC SMALL LETTER SCHWA WITH DIAERESIS
    { {UNITIZE(TQt::Key_Multi_key), 0x0022, 0x04e8, 0, 0, 0}, 0x04ea }, // U04EA # CYRILLIC CAPITAL LETTER BARRED O WITH DIAERESIS
    { {UNITIZE(TQt::Key_Multi_key), 0x0022, 0x04e9, 0, 0, 0}, 0x04eb }, // U04EB # CYRILLIC SMALL LETTER BARRED O WITH DIAERESIS
    { {UNITIZE(TQt::Key_Multi_key), 0x0022, UNITIZE(TQt::Key_Dead_Acute), 0, 0, 0}, 0x0344 }, // U0344 # COMBINING GREEK DIALYTIKA TONOS
    { {UNITIZE(TQt::Key_Multi_key), 0x0022, UNITIZE(TQt::Key_Dead_Tilde), 0x004f, 0, 0}, 0x1e4e }, // U1E4E # LATIN CAPITAL LETTER O WITH TILDE AND DIAERESIS
    { {UNITIZE(TQt::Key_Multi_key), 0x0022, UNITIZE(TQt::Key_Dead_Tilde), 0x006f, 0, 0}, 0x1e4f }, // U1E4F # LATIN SMALL LETTER O WITH TILDE AND DIAERESIS
    { {UNITIZE(TQt::Key_Multi_key), 0x0022, UNITIZE(TQt::Key_Dead_Macron), 0x0055, 0, 0}, 0x1e7a }, // U1E7A # LATIN CAPITAL LETTER U WITH MACRON AND DIAERESIS
    { {UNITIZE(TQt::Key_Multi_key), 0x0022, UNITIZE(TQt::Key_Dead_Macron), 0x0075, 0, 0}, 0x1e7b }, // U1E7B # LATIN SMALL LETTER U WITH MACRON AND DIAERESIS
    { {UNITIZE(TQt::Key_Multi_key), 0x0023, 0x0023, 0, 0, 0}, 0x266f }, // U266f # MUSIC SHARP SIGN
    { {UNITIZE(TQt::Key_Multi_key), 0x0023, 0x0062, 0, 0, 0}, 0x266d }, // U266d # MUSIC FLAT SIGN
    { {UNITIZE(TQt::Key_Multi_key), 0x0023, 0x0066, 0, 0, 0}, 0x266e }, // U266e # MUSIC NATURAL SIGN
    { {UNITIZE(TQt::Key_Multi_key), 0x0025, 0x006f, 0, 0, 0}, 0x2030 }, // U2030 # PER MILLE SIGN
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0000, 0, 0, 0}, 0x1eda }, // U1EDA # LATIN CAPITAL LETTER O WITH HORN AND ACUTE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0000, 0, 0, 0}, 0x1ee9 }, // U1EE9 # LATIN SMALL LETTER U WITH HORN AND ACUTE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0000, 0, 0, 0}, 0x1ee8 }, // U1EE8 # LATIN CAPITAL LETTER U WITH HORN AND ACUTE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0000, 0, 0, 0}, 0x1edb }, // U1EDB # LATIN SMALL LETTER O WITH HORN AND ACUTE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0000, 0x004f, 0, 0}, 0x1e4c }, // U1E4C # LATIN CAPITAL LETTER O WITH TILDE AND ACUTE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0000, 0x004f, 0, 0}, 0x01fe }, // U01FE # LATIN CAPITAL LETTER O WITH STROKE AND ACUTE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0000, 0x0055, 0, 0}, 0x1e78 }, // U1E78 # LATIN CAPITAL LETTER U WITH TILDE AND ACUTE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0000, 0x006f, 0, 0}, 0x1e4d }, // U1E4D # LATIN SMALL LETTER O WITH TILDE AND ACUTE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0000, 0x006f, 0, 0}, 0x01ff }, // U01FF # LATIN SMALL LETTER O WITH STROKE AND ACUTE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0000, 0x0075, 0, 0}, 0x1e79 }, // U1E79 # LATIN SMALL LETTER U WITH TILDE AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0020, 0, 0, 0}, 0x0027 }, // apostrophe
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0022, 0x0049, 0, 0}, 0x1e2e }, // U1E2E # LATIN CAPITAL LETTER I WITH DIAERESIS AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0022, 0x0055, 0, 0}, 0x01d7 }, // U01D7 # LATIN CAPITAL LETTER U WITH DIAERESIS AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0022, 0x0069, 0, 0}, 0x1e2f }, // U1E2F # LATIN SMALL LETTER I WITH DIAERESIS AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0022, 0x0075, 0, 0}, 0x01d8 }, // U01D8 # LATIN SMALL LETTER U WITH DIAERESIS AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0022, 0x03b9, 0, 0}, 0x0390 }, // U0390 # GREEK SMALL LETTER IOTA WITH DIALYTIKA AND TONOS
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0022, 0x03c5, 0, 0}, 0x03b0 }, // U03B0 # GREEK SMALL LETTER UPSILON WITH DIALYTIKA AND TONOS
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0028, 0x0391, 0, 0}, 0x1f0d }, // U1F0D # GREEK CAPITAL LETTER ALPHA WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0028, 0x0395, 0, 0}, 0x1f1d }, // U1F1D # GREEK CAPITAL LETTER EPSILON WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0028, 0x0397, 0, 0}, 0x1f2d }, // U1F2D # GREEK CAPITAL LETTER ETA WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0028, 0x0399, 0, 0}, 0x1f3d }, // U1F3D # GREEK CAPITAL LETTER IOTA WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0028, 0x039f, 0, 0}, 0x1f4d }, // U1F4D # GREEK CAPITAL LETTER OMICRON WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0028, 0x03a5, 0, 0}, 0x1f5d }, // U1F5D # GREEK CAPITAL LETTER UPSILON WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0028, 0x03a9, 0, 0}, 0x1f6d }, // U1F6D # GREEK CAPITAL LETTER OMEGA WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0028, 0x03b1, 0, 0}, 0x1f05 }, // U1F05 # GREEK SMALL LETTER ALPHA WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0028, 0x03b5, 0, 0}, 0x1f15 }, // U1F15 # GREEK SMALL LETTER EPSILON WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0028, 0x03b7, 0, 0}, 0x1f25 }, // U1F25 # GREEK SMALL LETTER ETA WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0028, 0x03b9, 0, 0}, 0x1f35 }, // U1F35 # GREEK SMALL LETTER IOTA WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0028, 0x03bf, 0, 0}, 0x1f45 }, // U1F45 # GREEK SMALL LETTER OMICRON WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0028, 0x03c5, 0, 0}, 0x1f55 }, // U1F55 # GREEK SMALL LETTER UPSILON WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0028, 0x03c9, 0, 0}, 0x1f65 }, // U1F65 # GREEK SMALL LETTER OMEGA WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0029, 0x0391, 0, 0}, 0x1f0c }, // U1F0C # GREEK CAPITAL LETTER ALPHA WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0029, 0x0395, 0, 0}, 0x1f1c }, // U1F1C # GREEK CAPITAL LETTER EPSILON WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0029, 0x0397, 0, 0}, 0x1f2c }, // U1F2C # GREEK CAPITAL LETTER ETA WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0029, 0x0399, 0, 0}, 0x1f3c }, // U1F3C # GREEK CAPITAL LETTER IOTA WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0029, 0x039f, 0, 0}, 0x1f4c }, // U1F4C # GREEK CAPITAL LETTER OMICRON WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0029, 0x03a9, 0, 0}, 0x1f6c }, // U1F6C # GREEK CAPITAL LETTER OMEGA WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0029, 0x03b1, 0, 0}, 0x1f04 }, // U1F04 # GREEK SMALL LETTER ALPHA WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0029, 0x03b5, 0, 0}, 0x1f14 }, // U1F14 # GREEK SMALL LETTER EPSILON WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0029, 0x03b7, 0, 0}, 0x1f24 }, // U1F24 # GREEK SMALL LETTER ETA WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0029, 0x03b9, 0, 0}, 0x1f34 }, // U1F34 # GREEK SMALL LETTER IOTA WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0029, 0x03bf, 0, 0}, 0x1f44 }, // U1F44 # GREEK SMALL LETTER OMICRON WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0029, 0x03c5, 0, 0}, 0x1f54 }, // U1F54 # GREEK SMALL LETTER UPSILON WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0029, 0x03c9, 0, 0}, 0x1f64 }, // U1F64 # GREEK SMALL LETTER OMEGA WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x002b, 0x004f, 0, 0}, 0x1eda }, // U1EDA # LATIN CAPITAL LETTER O WITH HORN AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x002b, 0x0055, 0, 0}, 0x1ee8 }, // U1EE8 # LATIN CAPITAL LETTER U WITH HORN AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x002b, 0x006f, 0, 0}, 0x1edb }, // U1EDB # LATIN SMALL LETTER O WITH HORN AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x002b, 0x0075, 0, 0}, 0x1ee9 }, // U1EE9 # LATIN SMALL LETTER U WITH HORN AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x002c, 0, 0, 0}, 0x201a }, // U201a # SINGLE LOW-9 QUOTATION MARK
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x002c, 0x0043, 0, 0}, 0x1e08 }, // U1E08 # LATIN CAPITAL LETTER C WITH CEDILLA AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x002c, 0x0063, 0, 0}, 0x1e09 }, // U1E09 # LATIN SMALL LETTER C WITH CEDILLA AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x002f, 0x004f, 0, 0}, 0x01fe }, // U01FE # LATIN CAPITAL LETTER O WITH STROKE AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x002f, 0x006f, 0, 0}, 0x01ff }, // U01FF # LATIN SMALL LETTER O WITH STROKE AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x003c, 0, 0, 0}, 0x2018 }, // U2018 # LEFT SINGLE QUOTATION MARK
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x003e, 0, 0, 0}, 0x2019 }, // U2019 # RIGHT SINGLE QUOTATION MARK
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0041, 0, 0, 0}, 0x00c1 }, // U00C1 # LATIN CAPITAL LETTER A WITH ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0043, 0, 0, 0}, 0x0106 }, // U0106 # LATIN CAPITAL LETTER C WITH ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0045, 0, 0, 0}, 0x00c9 }, // U00C9 # LATIN CAPITAL LETTER E WITH ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0047, 0, 0, 0}, 0x01f4 }, // U01F4 # LATIN CAPITAL LETTER G WITH ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0049, 0, 0, 0}, 0x00cd }, // U00CD # LATIN CAPITAL LETTER I WITH ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x004b, 0, 0, 0}, 0x1e30 }, // U1E30 # LATIN CAPITAL LETTER K WITH ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x004c, 0, 0, 0}, 0x0139 }, // U0139 # LATIN CAPITAL LETTER L WITH ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x004d, 0, 0, 0}, 0x1e3e }, // U1E3E # LATIN CAPITAL LETTER M WITH ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x004e, 0, 0, 0}, 0x0143 }, // U0143 # LATIN CAPITAL LETTER N WITH ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x004f, 0, 0, 0}, 0x00d3 }, // U00D3 # LATIN CAPITAL LETTER O WITH ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0050, 0, 0, 0}, 0x1e54 }, // U1E54 # LATIN CAPITAL LETTER P WITH ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0052, 0, 0, 0}, 0x0154 }, // U0154 # LATIN CAPITAL LETTER R WITH ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0053, 0, 0, 0}, 0x015a }, // U015A # LATIN CAPITAL LETTER S WITH ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0055, 0, 0, 0}, 0x00da }, // U00DA # LATIN CAPITAL LETTER U WITH ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0055, 0x0041, 0, 0}, 0x1eae }, // U1EAE # LATIN CAPITAL LETTER A WITH BREVE AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0055, 0x0061, 0, 0}, 0x1eaf }, // U1EAF # LATIN SMALL LETTER A WITH BREVE AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0057, 0, 0, 0}, 0x1e82 }, // U1E82 # LATIN CAPITAL LETTER W WITH ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0059, 0, 0, 0}, 0x00dd }, // U00DD # LATIN CAPITAL LETTER Y WITH ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x005a, 0, 0, 0}, 0x0179 }, // U0179 # LATIN CAPITAL LETTER Z WITH ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x005e, 0x0041, 0, 0}, 0x1ea4 }, // U1EA4 # LATIN CAPITAL LETTER A WITH CIRCUMFLEX AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x005e, 0x0045, 0, 0}, 0x1ebe }, // U1EBE # LATIN CAPITAL LETTER E WITH CIRCUMFLEX AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x005e, 0x004f, 0, 0}, 0x1ed0 }, // U1ED0 # LATIN CAPITAL LETTER O WITH CIRCUMFLEX AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x005e, 0x0061, 0, 0}, 0x1ea5 }, // U1EA5 # LATIN SMALL LETTER A WITH CIRCUMFLEX AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x005e, 0x0065, 0, 0}, 0x1ebf }, // U1EBF # LATIN SMALL LETTER E WITH CIRCUMFLEX AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x005e, 0x006f, 0, 0}, 0x1ed1 }, // U1ED1 # LATIN SMALL LETTER O WITH CIRCUMFLEX AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x005f, 0x0045, 0, 0}, 0x1e16 }, // U1E16 # LATIN CAPITAL LETTER E WITH MACRON AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x005f, 0x004f, 0, 0}, 0x1e52 }, // U1E52 # LATIN CAPITAL LETTER O WITH MACRON AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x005f, 0x0065, 0, 0}, 0x1e17 }, // U1E17 # LATIN SMALL LETTER E WITH MACRON AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x005f, 0x006f, 0, 0}, 0x1e53 }, // U1E53 # LATIN SMALL LETTER O WITH MACRON AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0061, 0, 0, 0}, 0x00e1 }, // U00E1 # LATIN SMALL LETTER A WITH ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0062, 0x0041, 0, 0}, 0x1eae }, // U1EAE # LATIN CAPITAL LETTER A WITH BREVE AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0062, 0x0061, 0, 0}, 0x1eaf }, // U1EAF # LATIN SMALL LETTER A WITH BREVE AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0063, 0, 0, 0}, 0x0107 }, // U0107 # LATIN SMALL LETTER C WITH ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0065, 0, 0, 0}, 0x00e9 }, // U00E9 # LATIN SMALL LETTER E WITH ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0067, 0, 0, 0}, 0x01f5 }, // U01F5 # LATIN SMALL LETTER G WITH ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0069, 0, 0, 0}, 0x00ed }, // U00ED # LATIN SMALL LETTER I WITH ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x006b, 0, 0, 0}, 0x1e31 }, // U1E31 # LATIN SMALL LETTER K WITH ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x006c, 0, 0, 0}, 0x013a }, // U013A # LATIN SMALL LETTER L WITH ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x006d, 0, 0, 0}, 0x1e3f }, // U1E3F # LATIN SMALL LETTER M WITH ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x006e, 0, 0, 0}, 0x0144 }, // U0144 # LATIN SMALL LETTER N WITH ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x006f, 0, 0, 0}, 0x00f3 }, // U00F3 # LATIN SMALL LETTER O WITH ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0070, 0, 0, 0}, 0x1e55 }, // U1E55 # LATIN SMALL LETTER P WITH ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0072, 0, 0, 0}, 0x0155 }, // U0155 # LATIN SMALL LETTER R WITH ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0073, 0, 0, 0}, 0x015b }, // U015B # LATIN SMALL LETTER S WITH ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0075, 0, 0, 0}, 0x00fa }, // U00FA # LATIN SMALL LETTER U WITH ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0077, 0, 0, 0}, 0x1e83 }, // U1E83 # LATIN SMALL LETTER W WITH ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0079, 0, 0, 0}, 0x00fd }, // U00FD # LATIN SMALL LETTER Y WITH ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x007a, 0, 0, 0}, 0x017a }, // U017A # LATIN SMALL LETTER Z WITH ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x007e, 0x004f, 0, 0}, 0x1e4c }, // U1E4C # LATIN CAPITAL LETTER O WITH TILDE AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x007e, 0x0055, 0, 0}, 0x1e78 }, // U1E78 # LATIN CAPITAL LETTER U WITH TILDE AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x007e, 0x006f, 0, 0}, 0x1e4d }, // U1E4D # LATIN SMALL LETTER O WITH TILDE AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x007e, 0x0075, 0, 0}, 0x1e79 }, // U1E79 # LATIN SMALL LETTER U WITH TILDE AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x00af, 0x0045, 0, 0}, 0x1e16 }, // U1E16 # LATIN CAPITAL LETTER E WITH MACRON AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x00af, 0x004f, 0, 0}, 0x1e52 }, // U1E52 # LATIN CAPITAL LETTER O WITH MACRON AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x00af, 0x0065, 0, 0}, 0x1e17 }, // U1E17 # LATIN SMALL LETTER E WITH MACRON AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x00af, 0x006f, 0, 0}, 0x1e53 }, // U1E53 # LATIN SMALL LETTER O WITH MACRON AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x00c2, 0, 0, 0}, 0x1ea4 }, // U1EA4 # LATIN CAPITAL LETTER A WITH CIRCUMFLEX AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x00c5, 0, 0, 0}, 0x01fa }, // U01FA # LATIN CAPITAL LETTER A WITH RING ABOVE AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x00c6, 0, 0, 0}, 0x01fc }, // U01FC # LATIN CAPITAL LETTER AE WITH ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x00c7, 0, 0, 0}, 0x1e08 }, // U1E08 # LATIN CAPITAL LETTER C WITH CEDILLA AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x00ca, 0, 0, 0}, 0x1ebe }, // U1EBE # LATIN CAPITAL LETTER E WITH CIRCUMFLEX AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x00cf, 0, 0, 0}, 0x1e2e }, // U1E2E # LATIN CAPITAL LETTER I WITH DIAERESIS AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x00d4, 0, 0, 0}, 0x1ed0 }, // U1ED0 # LATIN CAPITAL LETTER O WITH CIRCUMFLEX AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x00d5, 0, 0, 0}, 0x1e4c }, // U1E4C # LATIN CAPITAL LETTER O WITH TILDE AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x00d8, 0, 0, 0}, 0x01fe }, // U01FE # LATIN CAPITAL LETTER O WITH STROKE AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x00dc, 0, 0, 0}, 0x01d7 }, // U01D7 # LATIN CAPITAL LETTER U WITH DIAERESIS AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x00e2, 0, 0, 0}, 0x1ea5 }, // U1EA5 # LATIN SMALL LETTER A WITH CIRCUMFLEX AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x00e5, 0, 0, 0}, 0x01fb }, // U01FB # LATIN SMALL LETTER A WITH RING ABOVE AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x00e6, 0, 0, 0}, 0x01fd }, // U01FD # LATIN SMALL LETTER AE WITH ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x00e7, 0, 0, 0}, 0x1e09 }, // U1E09 # LATIN SMALL LETTER C WITH CEDILLA AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x00ea, 0, 0, 0}, 0x1ebf }, // U1EBF # LATIN SMALL LETTER E WITH CIRCUMFLEX AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x00ef, 0, 0, 0}, 0x1e2f }, // U1E2F # LATIN SMALL LETTER I WITH DIAERESIS AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x00f4, 0, 0, 0}, 0x1ed1 }, // U1ED1 # LATIN SMALL LETTER O WITH CIRCUMFLEX AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x00f5, 0, 0, 0}, 0x1e4d }, // U1E4D # LATIN SMALL LETTER O WITH TILDE AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x00f8, 0, 0, 0}, 0x01ff }, // U01FF # LATIN SMALL LETTER O WITH STROKE AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x00fc, 0, 0, 0}, 0x01d8 }, // U01D8 # LATIN SMALL LETTER U WITH DIAERESIS AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0102, 0, 0, 0}, 0x1eae }, // U1EAE # LATIN CAPITAL LETTER A WITH BREVE AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0103, 0, 0, 0}, 0x1eaf }, // U1EAF # LATIN SMALL LETTER A WITH BREVE AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0112, 0, 0, 0}, 0x1e16 }, // U1E16 # LATIN CAPITAL LETTER E WITH MACRON AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0113, 0, 0, 0}, 0x1e17 }, // U1E17 # LATIN SMALL LETTER E WITH MACRON AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x014c, 0, 0, 0}, 0x1e52 }, // U1E52 # LATIN CAPITAL LETTER O WITH MACRON AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x014d, 0, 0, 0}, 0x1e53 }, // U1E53 # LATIN SMALL LETTER O WITH MACRON AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0168, 0, 0, 0}, 0x1e78 }, // U1E78 # LATIN CAPITAL LETTER U WITH TILDE AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0169, 0, 0, 0}, 0x1e79 }, // U1E79 # LATIN SMALL LETTER U WITH TILDE AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0313, 0x0391, 0, 0}, 0x1f0c }, // U1F0C # GREEK CAPITAL LETTER ALPHA WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0313, 0x0395, 0, 0}, 0x1f1c }, // U1F1C # GREEK CAPITAL LETTER EPSILON WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0313, 0x0397, 0, 0}, 0x1f2c }, // U1F2C # GREEK CAPITAL LETTER ETA WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0313, 0x0399, 0, 0}, 0x1f3c }, // U1F3C # GREEK CAPITAL LETTER IOTA WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0313, 0x039f, 0, 0}, 0x1f4c }, // U1F4C # GREEK CAPITAL LETTER OMICRON WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0313, 0x03a9, 0, 0}, 0x1f6c }, // U1F6C # GREEK CAPITAL LETTER OMEGA WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0313, 0x03b1, 0, 0}, 0x1f04 }, // U1F04 # GREEK SMALL LETTER ALPHA WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0313, 0x03b5, 0, 0}, 0x1f14 }, // U1F14 # GREEK SMALL LETTER EPSILON WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0313, 0x03b7, 0, 0}, 0x1f24 }, // U1F24 # GREEK SMALL LETTER ETA WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0313, 0x03b9, 0, 0}, 0x1f34 }, // U1F34 # GREEK SMALL LETTER IOTA WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0313, 0x03bf, 0, 0}, 0x1f44 }, // U1F44 # GREEK SMALL LETTER OMICRON WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0313, 0x03c5, 0, 0}, 0x1f54 }, // U1F54 # GREEK SMALL LETTER UPSILON WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0313, 0x03c9, 0, 0}, 0x1f64 }, // U1F64 # GREEK SMALL LETTER OMEGA WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0314, 0x0391, 0, 0}, 0x1f0d }, // U1F0D # GREEK CAPITAL LETTER ALPHA WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0314, 0x0395, 0, 0}, 0x1f1d }, // U1F1D # GREEK CAPITAL LETTER EPSILON WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0314, 0x0397, 0, 0}, 0x1f2d }, // U1F2D # GREEK CAPITAL LETTER ETA WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0314, 0x0399, 0, 0}, 0x1f3d }, // U1F3D # GREEK CAPITAL LETTER IOTA WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0314, 0x039f, 0, 0}, 0x1f4d }, // U1F4D # GREEK CAPITAL LETTER OMICRON WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0314, 0x03a5, 0, 0}, 0x1f5d }, // U1F5D # GREEK CAPITAL LETTER UPSILON WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0314, 0x03a9, 0, 0}, 0x1f6d }, // U1F6D # GREEK CAPITAL LETTER OMEGA WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0314, 0x03b1, 0, 0}, 0x1f05 }, // U1F05 # GREEK SMALL LETTER ALPHA WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0314, 0x03b5, 0, 0}, 0x1f15 }, // U1F15 # GREEK SMALL LETTER EPSILON WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0314, 0x03b7, 0, 0}, 0x1f25 }, // U1F25 # GREEK SMALL LETTER ETA WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0314, 0x03b9, 0, 0}, 0x1f35 }, // U1F35 # GREEK SMALL LETTER IOTA WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0314, 0x03bf, 0, 0}, 0x1f45 }, // U1F45 # GREEK SMALL LETTER OMICRON WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0314, 0x03c5, 0, 0}, 0x1f55 }, // U1F55 # GREEK SMALL LETTER UPSILON WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0314, 0x03c9, 0, 0}, 0x1f65 }, // U1F65 # GREEK SMALL LETTER OMEGA WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0391, 0, 0, 0}, 0x0386 }, // U0386 # GREEK CAPITAL LETTER ALPHA WITH TONOS
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0395, 0, 0, 0}, 0x0388 }, // U0388 # GREEK CAPITAL LETTER EPSILON WITH TONOS
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0397, 0, 0, 0}, 0x0389 }, // U0389 # GREEK CAPITAL LETTER ETA WITH TONOS
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0399, 0, 0, 0}, 0x038a }, // U038A # GREEK CAPITAL LETTER IOTA WITH TONOS
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x039f, 0, 0, 0}, 0x038c }, // U038C # GREEK CAPITAL LETTER OMICRON WITH TONOS
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x03a5, 0, 0, 0}, 0x038e }, // U038E # GREEK CAPITAL LETTER UPSILON WITH TONOS
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x03a9, 0, 0, 0}, 0x038f }, // U038F # GREEK CAPITAL LETTER OMEGA WITH TONOS
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x03b1, 0, 0, 0}, 0x03ac }, // U03AC # GREEK SMALL LETTER ALPHA WITH TONOS
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x03b5, 0, 0, 0}, 0x03ad }, // U03AD # GREEK SMALL LETTER EPSILON WITH TONOS
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x03b7, 0, 0, 0}, 0x03ae }, // U03AE # GREEK SMALL LETTER ETA WITH TONOS
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x03b9, 0, 0, 0}, 0x03af }, // U03AF # GREEK SMALL LETTER IOTA WITH TONOS
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x03bf, 0, 0, 0}, 0x03cc }, // U03CC # GREEK SMALL LETTER OMICRON WITH TONOS
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x03c5, 0, 0, 0}, 0x03cd }, // U03CD # GREEK SMALL LETTER UPSILON WITH TONOS
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x03c9, 0, 0, 0}, 0x03ce }, // U03CE # GREEK SMALL LETTER OMEGA WITH TONOS
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x03ca, 0, 0, 0}, 0x0390 }, // U0390 # GREEK SMALL LETTER IOTA WITH DIALYTIKA AND TONOS
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x03cb, 0, 0, 0}, 0x03b0 }, // U03B0 # GREEK SMALL LETTER UPSILON WITH DIALYTIKA AND TONOS
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x03d2, 0, 0, 0}, 0x03d3 }, // U03D3 # GREEK UPSILON WITH ACUTE AND HOOK SYMBOL
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0413, 0, 0, 0}, 0x0403 }, // U0403 # CYRILLIC CAPITAL LETTER GJE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x041a, 0, 0, 0}, 0x040c }, // U040C # CYRILLIC CAPITAL LETTER KJE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0433, 0, 0, 0}, 0x0453 }, // U0453 # CYRILLIC SMALL LETTER GJE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x043a, 0, 0, 0}, 0x045c }, // U045C # CYRILLIC SMALL LETTER KJE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x1f00, 0, 0, 0}, 0x1f04 }, // U1F04 # GREEK SMALL LETTER ALPHA WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x1f01, 0, 0, 0}, 0x1f05 }, // U1F05 # GREEK SMALL LETTER ALPHA WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x1f08, 0, 0, 0}, 0x1f0c }, // U1F0C # GREEK CAPITAL LETTER ALPHA WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x1f09, 0, 0, 0}, 0x1f0d }, // U1F0D # GREEK CAPITAL LETTER ALPHA WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x1f10, 0, 0, 0}, 0x1f14 }, // U1F14 # GREEK SMALL LETTER EPSILON WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x1f11, 0, 0, 0}, 0x1f15 }, // U1F15 # GREEK SMALL LETTER EPSILON WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x1f18, 0, 0, 0}, 0x1f1c }, // U1F1C # GREEK CAPITAL LETTER EPSILON WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x1f19, 0, 0, 0}, 0x1f1d }, // U1F1D # GREEK CAPITAL LETTER EPSILON WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x1f20, 0, 0, 0}, 0x1f24 }, // U1F24 # GREEK SMALL LETTER ETA WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x1f21, 0, 0, 0}, 0x1f25 }, // U1F25 # GREEK SMALL LETTER ETA WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x1f28, 0, 0, 0}, 0x1f2c }, // U1F2C # GREEK CAPITAL LETTER ETA WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x1f29, 0, 0, 0}, 0x1f2d }, // U1F2D # GREEK CAPITAL LETTER ETA WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x1f30, 0, 0, 0}, 0x1f34 }, // U1F34 # GREEK SMALL LETTER IOTA WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x1f31, 0, 0, 0}, 0x1f35 }, // U1F35 # GREEK SMALL LETTER IOTA WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x1f38, 0, 0, 0}, 0x1f3c }, // U1F3C # GREEK CAPITAL LETTER IOTA WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x1f39, 0, 0, 0}, 0x1f3d }, // U1F3D # GREEK CAPITAL LETTER IOTA WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x1f40, 0, 0, 0}, 0x1f44 }, // U1F44 # GREEK SMALL LETTER OMICRON WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x1f41, 0, 0, 0}, 0x1f45 }, // U1F45 # GREEK SMALL LETTER OMICRON WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x1f48, 0, 0, 0}, 0x1f4c }, // U1F4C # GREEK CAPITAL LETTER OMICRON WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x1f49, 0, 0, 0}, 0x1f4d }, // U1F4D # GREEK CAPITAL LETTER OMICRON WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x1f50, 0, 0, 0}, 0x1f54 }, // U1F54 # GREEK SMALL LETTER UPSILON WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x1f51, 0, 0, 0}, 0x1f55 }, // U1F55 # GREEK SMALL LETTER UPSILON WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x1f59, 0, 0, 0}, 0x1f5d }, // U1F5D # GREEK CAPITAL LETTER UPSILON WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x1f60, 0, 0, 0}, 0x1f64 }, // U1F64 # GREEK SMALL LETTER OMEGA WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x1f61, 0, 0, 0}, 0x1f65 }, // U1F65 # GREEK SMALL LETTER OMEGA WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x1f68, 0, 0, 0}, 0x1f6c }, // U1F6C # GREEK CAPITAL LETTER OMEGA WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, 0x1f69, 0, 0, 0}, 0x1f6d }, // U1F6D # GREEK CAPITAL LETTER OMEGA WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, UNITIZE(TQt::Key_Dead_Circumflex), 0x0041, 0, 0}, 0x1ea4 }, // U1EA4 # LATIN CAPITAL LETTER A WITH CIRCUMFLEX AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, UNITIZE(TQt::Key_Dead_Circumflex), 0x0045, 0, 0}, 0x1ebe }, // U1EBE # LATIN CAPITAL LETTER E WITH CIRCUMFLEX AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, UNITIZE(TQt::Key_Dead_Circumflex), 0x004f, 0, 0}, 0x1ed0 }, // U1ED0 # LATIN CAPITAL LETTER O WITH CIRCUMFLEX AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, UNITIZE(TQt::Key_Dead_Circumflex), 0x0061, 0, 0}, 0x1ea5 }, // U1EA5 # LATIN SMALL LETTER A WITH CIRCUMFLEX AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, UNITIZE(TQt::Key_Dead_Circumflex), 0x0065, 0, 0}, 0x1ebf }, // U1EBF # LATIN SMALL LETTER E WITH CIRCUMFLEX AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, UNITIZE(TQt::Key_Dead_Circumflex), 0x006f, 0, 0}, 0x1ed1 }, // U1ED1 # LATIN SMALL LETTER O WITH CIRCUMFLEX AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, UNITIZE(TQt::Key_Dead_Tilde), 0x004f, 0, 0}, 0x1e4c }, // U1E4C # LATIN CAPITAL LETTER O WITH TILDE AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, UNITIZE(TQt::Key_Dead_Tilde), 0x0055, 0, 0}, 0x1e78 }, // U1E78 # LATIN CAPITAL LETTER U WITH TILDE AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, UNITIZE(TQt::Key_Dead_Tilde), 0x006f, 0, 0}, 0x1e4d }, // U1E4D # LATIN SMALL LETTER O WITH TILDE AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, UNITIZE(TQt::Key_Dead_Tilde), 0x0075, 0, 0}, 0x1e79 }, // U1E79 # LATIN SMALL LETTER U WITH TILDE AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, UNITIZE(TQt::Key_Dead_Macron), 0x0045, 0, 0}, 0x1e16 }, // U1E16 # LATIN CAPITAL LETTER E WITH MACRON AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, UNITIZE(TQt::Key_Dead_Macron), 0x004f, 0, 0}, 0x1e52 }, // U1E52 # LATIN CAPITAL LETTER O WITH MACRON AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, UNITIZE(TQt::Key_Dead_Macron), 0x0065, 0, 0}, 0x1e17 }, // U1E17 # LATIN SMALL LETTER E WITH MACRON AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, UNITIZE(TQt::Key_Dead_Macron), 0x006f, 0, 0}, 0x1e53 }, // U1E53 # LATIN SMALL LETTER O WITH MACRON AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, UNITIZE(TQt::Key_Dead_Breve), 0x0041, 0, 0}, 0x1eae }, // U1EAE # LATIN CAPITAL LETTER A WITH BREVE AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, UNITIZE(TQt::Key_Dead_Breve), 0x0061, 0, 0}, 0x1eaf }, // U1EAF # LATIN SMALL LETTER A WITH BREVE AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, UNITIZE(TQt::Key_Dead_Diaeresis), 0x0049, 0, 0}, 0x1e2e }, // U1E2E # LATIN CAPITAL LETTER I WITH DIAERESIS AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, UNITIZE(TQt::Key_Dead_Diaeresis), 0x0055, 0, 0}, 0x01d7 }, // U01D7 # LATIN CAPITAL LETTER U WITH DIAERESIS AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, UNITIZE(TQt::Key_Dead_Diaeresis), 0x0069, 0, 0}, 0x1e2f }, // U1E2F # LATIN SMALL LETTER I WITH DIAERESIS AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, UNITIZE(TQt::Key_Dead_Diaeresis), 0x0075, 0, 0}, 0x01d8 }, // U01D8 # LATIN SMALL LETTER U WITH DIAERESIS AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, UNITIZE(TQt::Key_Dead_Diaeresis), 0x03b9, 0, 0}, 0x0390 }, // U0390 # GREEK SMALL LETTER IOTA WITH DIALYTIKA AND TONOS
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, UNITIZE(TQt::Key_Dead_Diaeresis), 0x03c5, 0, 0}, 0x03b0 }, // U03B0 # GREEK SMALL LETTER UPSILON WITH DIALYTIKA AND TONOS
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, UNITIZE(TQt::Key_Dead_Abovering), 0x0041, 0, 0}, 0x01fa }, // U01FA # LATIN CAPITAL LETTER A WITH RING ABOVE AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, UNITIZE(TQt::Key_Dead_Abovering), 0x0061, 0, 0}, 0x01fb }, // U01FB # LATIN SMALL LETTER A WITH RING ABOVE AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, UNITIZE(TQt::Key_Dead_Cedilla), 0x0043, 0, 0}, 0x1e08 }, // U1E08 # LATIN CAPITAL LETTER C WITH CEDILLA AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, UNITIZE(TQt::Key_Dead_Cedilla), 0x0063, 0, 0}, 0x1e09 }, // U1E09 # LATIN SMALL LETTER C WITH CEDILLA AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, UNITIZE(TQt::Key_Dead_Horn), 0x004f, 0, 0}, 0x1eda }, // U1EDA # LATIN CAPITAL LETTER O WITH HORN AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, UNITIZE(TQt::Key_Dead_Horn), 0x0055, 0, 0}, 0x1ee8 }, // U1EE8 # LATIN CAPITAL LETTER U WITH HORN AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, UNITIZE(TQt::Key_Dead_Horn), 0x006f, 0, 0}, 0x1edb }, // U1EDB # LATIN SMALL LETTER O WITH HORN AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x0027, UNITIZE(TQt::Key_Dead_Horn), 0x0075, 0, 0}, 0x1ee9 }, // U1EE9 # LATIN SMALL LETTER U WITH HORN AND ACUTE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0000, 0x0029, 0}, 0x246b }, // U246B # CIRCLED NUMBER TWELVE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0000, 0x0029, 0}, 0x32b7 }, // U32B7 # CIRCLED NUMBER FORTY TWO
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0000, 0x0029, 0}, 0x3253 }, // U3253 # CIRCLED NUMBER TWENTY THREE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0000, 0x0029, 0}, 0x3252 }, // U3252 # CIRCLED NUMBER TWENTY TWO
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0000, 0x0029, 0}, 0x3253 }, // U3253 # CIRCLED NUMBER TWENTY THREE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0000, 0x0029, 0}, 0x2473 }, // U2473 # CIRCLED NUMBER TWENTY
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0000, 0x0029, 0}, 0x32b2 }, // U32B2 # CIRCLED NUMBER THIRTY SEVEN
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0000, 0x0029, 0}, 0x32b7 }, // U32B7 # CIRCLED NUMBER FORTY TWO
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0000, 0x0029, 0}, 0x3252 }, // U3252 # CIRCLED NUMBER TWENTY TWO
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0000, 0x0029, 0}, 0x246d }, // U246D # CIRCLED NUMBER FOURTEEN
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0000, 0x0029, 0}, 0x32be }, // U32BE # CIRCLED NUMBER FORTY NINE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0000, 0x0029, 0}, 0x3254 }, // U3254 # CIRCLED NUMBER TWENTY FOUR
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0000, 0x0029, 0}, 0x2470 }, // U2470 # CIRCLED NUMBER SEVENTEEN
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0000, 0x0029, 0}, 0x3252 }, // U3252 # CIRCLED NUMBER TWENTY TWO
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0000, 0x0029, 0}, 0x32bc }, // U32BC # CIRCLED NUMBER FORTY SEVEN
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0000, 0x0029, 0}, 0x3254 }, // U3254 # CIRCLED NUMBER TWENTY FOUR
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0000, 0x0029, 0}, 0x246f }, // U246F # CIRCLED NUMBER SIXTEEN
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0000, 0x0029, 0}, 0x3252 }, // U3252 # CIRCLED NUMBER TWENTY TWO
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0000, 0x0029, 0}, 0x32bb }, // U32BB # CIRCLED NUMBER FORTY SIX
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0000, 0x0029, 0}, 0x32b3 }, // U32B3 # CIRCLED NUMBER THIRTY EIGHT
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0000, 0x0029, 0}, 0x32b6 }, // U32B6 # CIRCLED NUMBER FORTY ONE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0000, 0x0029, 0}, 0x2469 }, // U2469 # CIRCLED NUMBER TEN
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0000, 0x0029, 0}, 0x3255 }, // U3255 # CIRCLED NUMBER TWENTY FIVE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0000, 0x0029, 0}, 0x3251 }, // U3251 # CIRCLED NUMBER TWENTY ONE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0000, 0x0029, 0}, 0x3257 }, // U3257 # CIRCLED NUMBER TWENTY SEVEN
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0000, 0x0029, 0}, 0x3255 }, // U3255 # CIRCLED NUMBER TWENTY FIVE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0000, 0x0029, 0}, 0x325f }, // U325F # CIRCLED NUMBER THIRTY FIVE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0000, 0x0029, 0}, 0x325e }, // U325E # CIRCLED NUMBER THIRTY FOUR
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0000, 0x0029, 0}, 0x3251 }, // U3251 # CIRCLED NUMBER TWENTY ONE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0000, 0x0029, 0}, 0x32b8 }, // U32B8 # CIRCLED NUMBER FORTY THREE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0000, 0x0029, 0}, 0x32b1 }, // U32B1 # CIRCLED NUMBER THIRTY SIX
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0000, 0x0029, 0}, 0x325d }, // U325D # CIRCLED NUMBER THIRTY THREE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0000, 0x0029, 0}, 0x2471 }, // U2471 # CIRCLED NUMBER EIGHTEEN
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0000, 0x0029, 0}, 0x325c }, // U325C # CIRCLED NUMBER THIRTY TWO
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0000, 0x0029, 0}, 0x3256 }, // U3256 # CIRCLED NUMBER TWENTY SIX
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0000, 0x0029, 0}, 0x325c }, // U325C # CIRCLED NUMBER THIRTY TWO
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0000, 0x0029, 0}, 0x3256 }, // U3256 # CIRCLED NUMBER TWENTY SIX
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0000, 0x0029, 0}, 0x325b }, // U325B # CIRCLED NUMBER THIRTY ONE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0000, 0x0029, 0}, 0x2473 }, // U2473 # CIRCLED NUMBER TWENTY
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0000, 0x0029, 0}, 0x2472 }, // U2472 # CIRCLED NUMBER NINETEEN
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0000, 0x0029, 0}, 0x246c }, // U246C # CIRCLED NUMBER THIRTEEN
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0000, 0x0029, 0}, 0x32b9 }, // U32B9 # CIRCLED NUMBER FORTY FOUR
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0000, 0x0029, 0}, 0x32ba }, // U32BA # CIRCLED NUMBER FORTY FIVE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0000, 0x0029, 0}, 0x325a }, // U325A # CIRCLED NUMBER THIRTY
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0000, 0x0029, 0}, 0x32b5 }, // U32B5 # CIRCLED NUMBER FORTY
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0000, 0x0029, 0}, 0x3257 }, // U3257 # CIRCLED NUMBER TWENTY SEVEN
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0000, 0x0029, 0}, 0x3259 }, // U3259 # CIRCLED NUMBER TWENTY NINE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0000, 0x0029, 0}, 0x3259 }, // U3259 # CIRCLED NUMBER TWENTY NINE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0000, 0x0029, 0}, 0x32b4 }, // U32B4 # CIRCLED NUMBER THIRTY NINE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0000, 0x0029, 0}, 0x3258 }, // U3258 # CIRCLED NUMBER TWENTY EIGHT
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0000, 0x0029, 0}, 0x3258 }, // U3258 # CIRCLED NUMBER TWENTY EIGHT
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0000, 0x0029, 0}, 0x246b }, // U246B # CIRCLED NUMBER TWELVE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0000, 0x0029, 0}, 0x246a }, // U246A # CIRCLED NUMBER ELEVEN
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0000, 0x0029, 0}, 0x246e }, // U246E # CIRCLED NUMBER FIFTEEN
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0000, 0x0029, 0}, 0x32bd }, // U32BD # CIRCLED NUMBER FORTY EIGHT
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0000, 0x0029, 0}, 0x32bf }, // U32BF # CIRCLED NUMBER FIFTY
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0029, 0, 0}, 0x2461 }, // U2461 # CIRCLED DIGIT TWO
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0029, 0, 0}, 0x2461 }, // U2461 # CIRCLED DIGIT TWO
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0029, 0, 0}, 0x2462 }, // U2462 # CIRCLED DIGIT THREE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0029, 0, 0}, 0x2468 }, // U2468 # CIRCLED DIGIT NINE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0029, 0, 0}, 0x2463 }, // U2463 # CIRCLED DIGIT FOUR
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0029, 0, 0}, 0x2460 }, // U2460 # CIRCLED DIGIT ONE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0029, 0, 0}, 0x2467 }, // U2467 # CIRCLED DIGIT EIGHT
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0029, 0, 0}, 0x24ea }, // U24EA # CIRCLED DIGIT ZERO
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0029, 0, 0}, 0x2466 }, // U2466 # CIRCLED DIGIT SEVEN
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0029, 0, 0}, 0x2465 }, // U2465 # CIRCLED DIGIT SIX
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0029, 0, 0}, 0x2464 }, // U2464 # CIRCLED DIGIT FIVE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0030, 0x0029, 0}, 0x32b5 }, // U32B5 # CIRCLED NUMBER FORTY
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0030, 0x0029, 0}, 0x2473 }, // U2473 # CIRCLED NUMBER TWENTY
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0030, 0x0029, 0}, 0x2469 }, // U2469 # CIRCLED NUMBER TEN
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0030, 0x0029, 0}, 0x32bf }, // U32BF # CIRCLED NUMBER FIFTY
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0030, 0x0029, 0}, 0x325a }, // U325A # CIRCLED NUMBER THIRTY
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0030, 0x0029, 0}, 0x2473 }, // U2473 # CIRCLED NUMBER TWENTY
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0031, 0x0029, 0}, 0x32b6 }, // U32B6 # CIRCLED NUMBER FORTY ONE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0031, 0x0029, 0}, 0x246a }, // U246A # CIRCLED NUMBER ELEVEN
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0031, 0x0029, 0}, 0x3251 }, // U3251 # CIRCLED NUMBER TWENTY ONE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0031, 0x0029, 0}, 0x325b }, // U325B # CIRCLED NUMBER THIRTY ONE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0031, 0x0029, 0}, 0x3251 }, // U3251 # CIRCLED NUMBER TWENTY ONE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0032, 0x0029, 0}, 0x3252 }, // U3252 # CIRCLED NUMBER TWENTY TWO
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0032, 0x0029, 0}, 0x32b7 }, // U32B7 # CIRCLED NUMBER FORTY TWO
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0032, 0x0029, 0}, 0x246b }, // U246B # CIRCLED NUMBER TWELVE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0032, 0x0029, 0}, 0x325c }, // U325C # CIRCLED NUMBER THIRTY TWO
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0032, 0x0029, 0}, 0x3252 }, // U3252 # CIRCLED NUMBER TWENTY TWO
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0033, 0x0029, 0}, 0x246c }, // U246C # CIRCLED NUMBER THIRTEEN
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0033, 0x0029, 0}, 0x3253 }, // U3253 # CIRCLED NUMBER TWENTY THREE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0033, 0x0029, 0}, 0x325d }, // U325D # CIRCLED NUMBER THIRTY THREE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0033, 0x0029, 0}, 0x32b8 }, // U32B8 # CIRCLED NUMBER FORTY THREE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0033, 0x0029, 0}, 0x3253 }, // U3253 # CIRCLED NUMBER TWENTY THREE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0034, 0x0029, 0}, 0x246d }, // U246D # CIRCLED NUMBER FOURTEEN
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0034, 0x0029, 0}, 0x3254 }, // U3254 # CIRCLED NUMBER TWENTY FOUR
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0034, 0x0029, 0}, 0x32b9 }, // U32B9 # CIRCLED NUMBER FORTY FOUR
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0034, 0x0029, 0}, 0x3254 }, // U3254 # CIRCLED NUMBER TWENTY FOUR
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0034, 0x0029, 0}, 0x325e }, // U325E # CIRCLED NUMBER THIRTY FOUR
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0035, 0x0029, 0}, 0x3255 }, // U3255 # CIRCLED NUMBER TWENTY FIVE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0035, 0x0029, 0}, 0x325f }, // U325F # CIRCLED NUMBER THIRTY FIVE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0035, 0x0029, 0}, 0x32ba }, // U32BA # CIRCLED NUMBER FORTY FIVE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0035, 0x0029, 0}, 0x3255 }, // U3255 # CIRCLED NUMBER TWENTY FIVE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0035, 0x0029, 0}, 0x246e }, // U246E # CIRCLED NUMBER FIFTEEN
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0036, 0x0029, 0}, 0x3256 }, // U3256 # CIRCLED NUMBER TWENTY SIX
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0036, 0x0029, 0}, 0x32bb }, // U32BB # CIRCLED NUMBER FORTY SIX
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0036, 0x0029, 0}, 0x32b1 }, // U32B1 # CIRCLED NUMBER THIRTY SIX
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0036, 0x0029, 0}, 0x246f }, // U246F # CIRCLED NUMBER SIXTEEN
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0036, 0x0029, 0}, 0x3256 }, // U3256 # CIRCLED NUMBER TWENTY SIX
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0037, 0x0029, 0}, 0x32bc }, // U32BC # CIRCLED NUMBER FORTY SEVEN
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0037, 0x0029, 0}, 0x3257 }, // U3257 # CIRCLED NUMBER TWENTY SEVEN
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0037, 0x0029, 0}, 0x32b2 }, // U32B2 # CIRCLED NUMBER THIRTY SEVEN
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0037, 0x0029, 0}, 0x2470 }, // U2470 # CIRCLED NUMBER SEVENTEEN
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0037, 0x0029, 0}, 0x3257 }, // U3257 # CIRCLED NUMBER TWENTY SEVEN
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0038, 0x0029, 0}, 0x2471 }, // U2471 # CIRCLED NUMBER EIGHTEEN
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0038, 0x0029, 0}, 0x32b3 }, // U32B3 # CIRCLED NUMBER THIRTY EIGHT
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0038, 0x0029, 0}, 0x32bd }, // U32BD # CIRCLED NUMBER FORTY EIGHT
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0038, 0x0029, 0}, 0x3258 }, // U3258 # CIRCLED NUMBER TWENTY EIGHT
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0038, 0x0029, 0}, 0x3258 }, // U3258 # CIRCLED NUMBER TWENTY EIGHT
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0039, 0x0029, 0}, 0x32b4 }, // U32B4 # CIRCLED NUMBER THIRTY NINE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0039, 0x0029, 0}, 0x3259 }, // U3259 # CIRCLED NUMBER TWENTY NINE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0039, 0x0029, 0}, 0x32be }, // U32BE # CIRCLED NUMBER FORTY NINE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0039, 0x0029, 0}, 0x3259 }, // U3259 # CIRCLED NUMBER TWENTY NINE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0000, 0x0039, 0x0029, 0}, 0x2472 }, // U2472 # CIRCLED NUMBER NINETEEN
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0028, 0, 0, 0}, 0x005b }, // bracketleft
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x002d, 0, 0, 0}, 0x007b }, // braceleft
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0030, 0x0029, 0, 0}, 0x24ea }, // U24EA # CIRCLED DIGIT ZERO
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0031, 0x0000, 0x0029, 0}, 0x2472 }, // U2472 # CIRCLED NUMBER NINETEEN
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0031, 0x0000, 0x0029, 0}, 0x246e }, // U246E # CIRCLED NUMBER FIFTEEN
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0031, 0x0000, 0x0029, 0}, 0x2469 }, // U2469 # CIRCLED NUMBER TEN
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0031, 0x0000, 0x0029, 0}, 0x246c }, // U246C # CIRCLED NUMBER THIRTEEN
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0031, 0x0000, 0x0029, 0}, 0x246b }, // U246B # CIRCLED NUMBER TWELVE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0031, 0x0000, 0x0029, 0}, 0x2470 }, // U2470 # CIRCLED NUMBER SEVENTEEN
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0031, 0x0000, 0x0029, 0}, 0x246a }, // U246A # CIRCLED NUMBER ELEVEN
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0031, 0x0000, 0x0029, 0}, 0x246d }, // U246D # CIRCLED NUMBER FOURTEEN
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0031, 0x0000, 0x0029, 0}, 0x2471 }, // U2471 # CIRCLED NUMBER EIGHTEEN
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0031, 0x0000, 0x0029, 0}, 0x246f }, // U246F # CIRCLED NUMBER SIXTEEN
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0031, 0x0000, 0x0029, 0}, 0x246b }, // U246B # CIRCLED NUMBER TWELVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0031, 0x0029, 0, 0}, 0x2460 }, // U2460 # CIRCLED DIGIT ONE
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0031, 0x0030, 0x0029, 0}, 0x2469 }, // U2469 # CIRCLED NUMBER TEN
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0031, 0x0031, 0x0029, 0}, 0x246a }, // U246A # CIRCLED NUMBER ELEVEN
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0031, 0x0032, 0x0029, 0}, 0x246b }, // U246B # CIRCLED NUMBER TWELVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0031, 0x0033, 0x0029, 0}, 0x246c }, // U246C # CIRCLED NUMBER THIRTEEN
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0031, 0x0034, 0x0029, 0}, 0x246d }, // U246D # CIRCLED NUMBER FOURTEEN
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0031, 0x0035, 0x0029, 0}, 0x246e }, // U246E # CIRCLED NUMBER FIFTEEN
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0031, 0x0036, 0x0029, 0}, 0x246f }, // U246F # CIRCLED NUMBER SIXTEEN
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0031, 0x0037, 0x0029, 0}, 0x2470 }, // U2470 # CIRCLED NUMBER SEVENTEEN
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0031, 0x0038, 0x0029, 0}, 0x2471 }, // U2471 # CIRCLED NUMBER EIGHTEEN
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0031, 0x0039, 0x0029, 0}, 0x2472 }, // U2472 # CIRCLED NUMBER NINETEEN
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0032, 0x0000, 0x0029, 0}, 0x2473 }, // U2473 # CIRCLED NUMBER TWENTY
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0032, 0x0000, 0x0029, 0}, 0x3257 }, // U3257 # CIRCLED NUMBER TWENTY SEVEN
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0032, 0x0000, 0x0029, 0}, 0x3259 }, // U3259 # CIRCLED NUMBER TWENTY NINE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0032, 0x0000, 0x0029, 0}, 0x3258 }, // U3258 # CIRCLED NUMBER TWENTY EIGHT
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0032, 0x0000, 0x0029, 0}, 0x3253 }, // U3253 # CIRCLED NUMBER TWENTY THREE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0032, 0x0000, 0x0029, 0}, 0x3255 }, // U3255 # CIRCLED NUMBER TWENTY FIVE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0032, 0x0000, 0x0029, 0}, 0x3252 }, // U3252 # CIRCLED NUMBER TWENTY TWO
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0032, 0x0000, 0x0029, 0}, 0x3256 }, // U3256 # CIRCLED NUMBER TWENTY SIX
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0032, 0x0000, 0x0029, 0}, 0x3251 }, // U3251 # CIRCLED NUMBER TWENTY ONE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0032, 0x0000, 0x0029, 0}, 0x3252 }, // U3252 # CIRCLED NUMBER TWENTY TWO
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0032, 0x0000, 0x0029, 0}, 0x3254 }, // U3254 # CIRCLED NUMBER TWENTY FOUR
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0032, 0x0029, 0, 0}, 0x2461 }, // U2461 # CIRCLED DIGIT TWO
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0032, 0x0030, 0x0029, 0}, 0x2473 }, // U2473 # CIRCLED NUMBER TWENTY
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0032, 0x0031, 0x0029, 0}, 0x3251 }, // U3251 # CIRCLED NUMBER TWENTY ONE
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0032, 0x0032, 0x0029, 0}, 0x3252 }, // U3252 # CIRCLED NUMBER TWENTY TWO
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0032, 0x0033, 0x0029, 0}, 0x3253 }, // U3253 # CIRCLED NUMBER TWENTY THREE
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0032, 0x0034, 0x0029, 0}, 0x3254 }, // U3254 # CIRCLED NUMBER TWENTY FOUR
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0032, 0x0035, 0x0029, 0}, 0x3255 }, // U3255 # CIRCLED NUMBER TWENTY FIVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0032, 0x0036, 0x0029, 0}, 0x3256 }, // U3256 # CIRCLED NUMBER TWENTY SIX
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0032, 0x0037, 0x0029, 0}, 0x3257 }, // U3257 # CIRCLED NUMBER TWENTY SEVEN
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0032, 0x0038, 0x0029, 0}, 0x3258 }, // U3258 # CIRCLED NUMBER TWENTY EIGHT
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0032, 0x0039, 0x0029, 0}, 0x3259 }, // U3259 # CIRCLED NUMBER TWENTY NINE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0033, 0x0000, 0x0029, 0}, 0x32b4 }, // U32B4 # CIRCLED NUMBER THIRTY NINE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0033, 0x0000, 0x0029, 0}, 0x325a }, // U325A # CIRCLED NUMBER THIRTY
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0033, 0x0000, 0x0029, 0}, 0x325b }, // U325B # CIRCLED NUMBER THIRTY ONE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0033, 0x0000, 0x0029, 0}, 0x32b3 }, // U32B3 # CIRCLED NUMBER THIRTY EIGHT
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0033, 0x0000, 0x0029, 0}, 0x32b2 }, // U32B2 # CIRCLED NUMBER THIRTY SEVEN
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0033, 0x0000, 0x0029, 0}, 0x325c }, // U325C # CIRCLED NUMBER THIRTY TWO
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0033, 0x0000, 0x0029, 0}, 0x325c }, // U325C # CIRCLED NUMBER THIRTY TWO
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0033, 0x0000, 0x0029, 0}, 0x325f }, // U325F # CIRCLED NUMBER THIRTY FIVE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0033, 0x0000, 0x0029, 0}, 0x325e }, // U325E # CIRCLED NUMBER THIRTY FOUR
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0033, 0x0000, 0x0029, 0}, 0x32b1 }, // U32B1 # CIRCLED NUMBER THIRTY SIX
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0033, 0x0000, 0x0029, 0}, 0x325d }, // U325D # CIRCLED NUMBER THIRTY THREE
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0033, 0x0029, 0, 0}, 0x2462 }, // U2462 # CIRCLED DIGIT THREE
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0033, 0x0030, 0x0029, 0}, 0x325a }, // U325A # CIRCLED NUMBER THIRTY
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0033, 0x0031, 0x0029, 0}, 0x325b }, // U325B # CIRCLED NUMBER THIRTY ONE
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0033, 0x0032, 0x0029, 0}, 0x325c }, // U325C # CIRCLED NUMBER THIRTY TWO
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0033, 0x0033, 0x0029, 0}, 0x325d }, // U325D # CIRCLED NUMBER THIRTY THREE
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0033, 0x0034, 0x0029, 0}, 0x325e }, // U325E # CIRCLED NUMBER THIRTY FOUR
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0033, 0x0035, 0x0029, 0}, 0x325f }, // U325F # CIRCLED NUMBER THIRTY FIVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0033, 0x0036, 0x0029, 0}, 0x32b1 }, // U32B1 # CIRCLED NUMBER THIRTY SIX
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0033, 0x0037, 0x0029, 0}, 0x32b2 }, // U32B2 # CIRCLED NUMBER THIRTY SEVEN
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0033, 0x0038, 0x0029, 0}, 0x32b3 }, // U32B3 # CIRCLED NUMBER THIRTY EIGHT
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0033, 0x0039, 0x0029, 0}, 0x32b4 }, // U32B4 # CIRCLED NUMBER THIRTY NINE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0034, 0x0000, 0x0029, 0}, 0x32b7 }, // U32B7 # CIRCLED NUMBER FORTY TWO
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0034, 0x0000, 0x0029, 0}, 0x32b5 }, // U32B5 # CIRCLED NUMBER FORTY
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0034, 0x0000, 0x0029, 0}, 0x32b6 }, // U32B6 # CIRCLED NUMBER FORTY ONE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0034, 0x0000, 0x0029, 0}, 0x32bc }, // U32BC # CIRCLED NUMBER FORTY SEVEN
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0034, 0x0000, 0x0029, 0}, 0x32b7 }, // U32B7 # CIRCLED NUMBER FORTY TWO
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0034, 0x0000, 0x0029, 0}, 0x32b9 }, // U32B9 # CIRCLED NUMBER FORTY FOUR
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0034, 0x0000, 0x0029, 0}, 0x32b8 }, // U32B8 # CIRCLED NUMBER FORTY THREE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0034, 0x0000, 0x0029, 0}, 0x32bb }, // U32BB # CIRCLED NUMBER FORTY SIX
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0034, 0x0000, 0x0029, 0}, 0x32be }, // U32BE # CIRCLED NUMBER FORTY NINE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0034, 0x0000, 0x0029, 0}, 0x32ba }, // U32BA # CIRCLED NUMBER FORTY FIVE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0034, 0x0000, 0x0029, 0}, 0x32bd }, // U32BD # CIRCLED NUMBER FORTY EIGHT
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0034, 0x0029, 0, 0}, 0x2463 }, // U2463 # CIRCLED DIGIT FOUR
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0034, 0x0030, 0x0029, 0}, 0x32b5 }, // U32B5 # CIRCLED NUMBER FORTY
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0034, 0x0031, 0x0029, 0}, 0x32b6 }, // U32B6 # CIRCLED NUMBER FORTY ONE
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0034, 0x0032, 0x0029, 0}, 0x32b7 }, // U32B7 # CIRCLED NUMBER FORTY TWO
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0034, 0x0033, 0x0029, 0}, 0x32b8 }, // U32B8 # CIRCLED NUMBER FORTY THREE
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0034, 0x0034, 0x0029, 0}, 0x32b9 }, // U32B9 # CIRCLED NUMBER FORTY FOUR
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0034, 0x0035, 0x0029, 0}, 0x32ba }, // U32BA # CIRCLED NUMBER FORTY FIVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0034, 0x0036, 0x0029, 0}, 0x32bb }, // U32BB # CIRCLED NUMBER FORTY SIX
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0034, 0x0037, 0x0029, 0}, 0x32bc }, // U32BC # CIRCLED NUMBER FORTY SEVEN
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0034, 0x0038, 0x0029, 0}, 0x32bd }, // U32BD # CIRCLED NUMBER FORTY EIGHT
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0034, 0x0039, 0x0029, 0}, 0x32be }, // U32BE # CIRCLED NUMBER FORTY NINE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0035, 0x0000, 0x0029, 0}, 0x32bf }, // U32BF # CIRCLED NUMBER FIFTY
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0035, 0x0029, 0, 0}, 0x2464 }, // U2464 # CIRCLED DIGIT FIVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0035, 0x0030, 0x0029, 0}, 0x32bf }, // U32BF # CIRCLED NUMBER FIFTY
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0036, 0x0029, 0, 0}, 0x2465 }, // U2465 # CIRCLED DIGIT SIX
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0037, 0x0029, 0, 0}, 0x2466 }, // U2466 # CIRCLED DIGIT SEVEN
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0038, 0x0029, 0, 0}, 0x2467 }, // U2467 # CIRCLED DIGIT EIGHT
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0039, 0x0029, 0, 0}, 0x2468 }, // U2468 # CIRCLED DIGIT NINE
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0041, 0x0029, 0, 0}, 0x24b6 }, // U24B6 # CIRCLED LATIN CAPITAL LETTER A
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0042, 0x0029, 0, 0}, 0x24b7 }, // U24B7 # CIRCLED LATIN CAPITAL LETTER B
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0043, 0x0029, 0, 0}, 0x24b8 }, // U24B8 # CIRCLED LATIN CAPITAL LETTER C
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0044, 0x0029, 0, 0}, 0x24b9 }, // U24B9 # CIRCLED LATIN CAPITAL LETTER D
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0045, 0x0029, 0, 0}, 0x24ba }, // U24BA # CIRCLED LATIN CAPITAL LETTER E
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0046, 0x0029, 0, 0}, 0x24bb }, // U24BB # CIRCLED LATIN CAPITAL LETTER F
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0047, 0x0029, 0, 0}, 0x24bc }, // U24BC # CIRCLED LATIN CAPITAL LETTER G
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0048, 0x0029, 0, 0}, 0x24bd }, // U24BD # CIRCLED LATIN CAPITAL LETTER H
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0049, 0x0029, 0, 0}, 0x24be }, // U24BE # CIRCLED LATIN CAPITAL LETTER I
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x004a, 0x0029, 0, 0}, 0x24bf }, // U24BF # CIRCLED LATIN CAPITAL LETTER J
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x004b, 0x0029, 0, 0}, 0x24c0 }, // U24C0 # CIRCLED LATIN CAPITAL LETTER K
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x004c, 0x0029, 0, 0}, 0x24c1 }, // U24C1 # CIRCLED LATIN CAPITAL LETTER L
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x004d, 0x0029, 0, 0}, 0x24c2 }, // U24C2 # CIRCLED LATIN CAPITAL LETTER M
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x004e, 0x0029, 0, 0}, 0x24c3 }, // U24C3 # CIRCLED LATIN CAPITAL LETTER N
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x004f, 0x0029, 0, 0}, 0x24c4 }, // U24C4 # CIRCLED LATIN CAPITAL LETTER O
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0050, 0x0029, 0, 0}, 0x24c5 }, // U24C5 # CIRCLED LATIN CAPITAL LETTER P
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0051, 0x0029, 0, 0}, 0x24c6 }, // U24C6 # CIRCLED LATIN CAPITAL LETTER Q
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0052, 0x0029, 0, 0}, 0x24c7 }, // U24C7 # CIRCLED LATIN CAPITAL LETTER R
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0053, 0x0029, 0, 0}, 0x24c8 }, // U24C8 # CIRCLED LATIN CAPITAL LETTER S
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0054, 0x0029, 0, 0}, 0x24c9 }, // U24C9 # CIRCLED LATIN CAPITAL LETTER T
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0055, 0x0029, 0, 0}, 0x24ca }, // U24CA # CIRCLED LATIN CAPITAL LETTER U
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0056, 0x0029, 0, 0}, 0x24cb }, // U24CB # CIRCLED LATIN CAPITAL LETTER V
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0057, 0x0029, 0, 0}, 0x24cc }, // U24CC # CIRCLED LATIN CAPITAL LETTER W
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0058, 0x0029, 0, 0}, 0x24cd }, // U24CD # CIRCLED LATIN CAPITAL LETTER X
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0059, 0x0029, 0, 0}, 0x24ce }, // U24CE # CIRCLED LATIN CAPITAL LETTER Y
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x005a, 0x0029, 0, 0}, 0x24cf }, // U24CF # CIRCLED LATIN CAPITAL LETTER Z
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0061, 0x0029, 0, 0}, 0x24d0 }, // U24D0 # CIRCLED LATIN SMALL LETTER A
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0062, 0x0029, 0, 0}, 0x24d1 }, // U24D1 # CIRCLED LATIN SMALL LETTER B
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0063, 0x0029, 0, 0}, 0x24d2 }, // U24D2 # CIRCLED LATIN SMALL LETTER C
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0064, 0x0029, 0, 0}, 0x24d3 }, // U24D3 # CIRCLED LATIN SMALL LETTER D
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0065, 0x0029, 0, 0}, 0x24d4 }, // U24D4 # CIRCLED LATIN SMALL LETTER E
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0066, 0x0029, 0, 0}, 0x24d5 }, // U24D5 # CIRCLED LATIN SMALL LETTER F
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0067, 0x0029, 0, 0}, 0x24d6 }, // U24D6 # CIRCLED LATIN SMALL LETTER G
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0068, 0x0029, 0, 0}, 0x24d7 }, // U24D7 # CIRCLED LATIN SMALL LETTER H
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0069, 0x0029, 0, 0}, 0x24d8 }, // U24D8 # CIRCLED LATIN SMALL LETTER I
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x006a, 0x0029, 0, 0}, 0x24d9 }, // U24D9 # CIRCLED LATIN SMALL LETTER J
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x006b, 0x0029, 0, 0}, 0x24da }, // U24DA # CIRCLED LATIN SMALL LETTER K
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x006c, 0x0029, 0, 0}, 0x24db }, // U24DB # CIRCLED LATIN SMALL LETTER L
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x006d, 0x0029, 0, 0}, 0x24dc }, // U24DC # CIRCLED LATIN SMALL LETTER M
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x006e, 0x0029, 0, 0}, 0x24dd }, // U24DD # CIRCLED LATIN SMALL LETTER N
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x006f, 0x0029, 0, 0}, 0x24de }, // U24DE # CIRCLED LATIN SMALL LETTER O
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0070, 0x0029, 0, 0}, 0x24df }, // U24DF # CIRCLED LATIN SMALL LETTER P
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0071, 0x0029, 0, 0}, 0x24e0 }, // U24E0 # CIRCLED LATIN SMALL LETTER Q
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0072, 0x0029, 0, 0}, 0x24e1 }, // U24E1 # CIRCLED LATIN SMALL LETTER R
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0073, 0x0029, 0, 0}, 0x24e2 }, // U24E2 # CIRCLED LATIN SMALL LETTER S
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0074, 0x0029, 0, 0}, 0x24e3 }, // U24E3 # CIRCLED LATIN SMALL LETTER T
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0075, 0x0029, 0, 0}, 0x24e4 }, // U24E4 # CIRCLED LATIN SMALL LETTER U
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0076, 0x0029, 0, 0}, 0x24e5 }, // U24E5 # CIRCLED LATIN SMALL LETTER V
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0077, 0x0029, 0, 0}, 0x24e6 }, // U24E6 # CIRCLED LATIN SMALL LETTER W
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0078, 0x0029, 0, 0}, 0x24e7 }, // U24E7 # CIRCLED LATIN SMALL LETTER X
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0079, 0x0029, 0, 0}, 0x24e8 }, // U24E8 # CIRCLED LATIN SMALL LETTER Y
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x007a, 0x0029, 0, 0}, 0x24e9 }, // U24E9 # CIRCLED LATIN SMALL LETTER Z
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0391, 0, 0, 0}, 0x1f09 }, // U1F09 # GREEK CAPITAL LETTER ALPHA WITH DASIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0395, 0, 0, 0}, 0x1f19 }, // U1F19 # GREEK CAPITAL LETTER EPSILON WITH DASIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0397, 0, 0, 0}, 0x1f29 }, // U1F29 # GREEK CAPITAL LETTER ETA WITH DASIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0399, 0, 0, 0}, 0x1f39 }, // U1F39 # GREEK CAPITAL LETTER IOTA WITH DASIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x039f, 0, 0, 0}, 0x1f49 }, // U1F49 # GREEK CAPITAL LETTER OMICRON WITH DASIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x03a1, 0, 0, 0}, 0x1fec }, // U1FEC # GREEK CAPITAL LETTER RHO WITH DASIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x03a5, 0, 0, 0}, 0x1f59 }, // U1F59 # GREEK CAPITAL LETTER UPSILON WITH DASIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x03a9, 0, 0, 0}, 0x1f69 }, // U1F69 # GREEK CAPITAL LETTER OMEGA WITH DASIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x03b1, 0, 0, 0}, 0x1f01 }, // U1F01 # GREEK SMALL LETTER ALPHA WITH DASIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x03b5, 0, 0, 0}, 0x1f11 }, // U1F11 # GREEK SMALL LETTER EPSILON WITH DASIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x03b7, 0, 0, 0}, 0x1f21 }, // U1F21 # GREEK SMALL LETTER ETA WITH DASIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x03b9, 0, 0, 0}, 0x1f31 }, // U1F31 # GREEK SMALL LETTER IOTA WITH DASIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x03bf, 0, 0, 0}, 0x1f41 }, // U1F41 # GREEK SMALL LETTER OMICRON WITH DASIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x03c1, 0, 0, 0}, 0x1fe5 }, // U1FE5 # GREEK SMALL LETTER RHO WITH DASIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x03c5, 0, 0, 0}, 0x1f51 }, // U1F51 # GREEK SMALL LETTER UPSILON WITH DASIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x03c9, 0, 0, 0}, 0x1f61 }, // U1F61 # GREEK SMALL LETTER OMEGA WITH DASIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x1100, 0x0029, 0, 0}, 0x3260 }, // U3260 # CIRCLED HANGUL KIYEOK
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x1100, 0x1161, 0x0029, 0}, 0x326e }, // U326E # CIRCLED HANGUL KIYEOK A
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x1102, 0x0029, 0, 0}, 0x3261 }, // U3261 # CIRCLED HANGUL NIEUN
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x1102, 0x1161, 0x0029, 0}, 0x326f }, // U326F # CIRCLED HANGUL NIEUN A
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x1103, 0x0029, 0, 0}, 0x3262 }, // U3262 # CIRCLED HANGUL TIKEUT
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x1103, 0x1161, 0x0029, 0}, 0x3270 }, // U3270 # CIRCLED HANGUL TIKEUT A
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x1105, 0x0029, 0, 0}, 0x3263 }, // U3263 # CIRCLED HANGUL RIEUL
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x1105, 0x1161, 0x0029, 0}, 0x3271 }, // U3271 # CIRCLED HANGUL RIEUL A
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x1106, 0x0029, 0, 0}, 0x3264 }, // U3264 # CIRCLED HANGUL MIEUM
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x1106, 0x1161, 0x0029, 0}, 0x3272 }, // U3272 # CIRCLED HANGUL MIEUM A
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x1107, 0x0029, 0, 0}, 0x3265 }, // U3265 # CIRCLED HANGUL PIEUP
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x1107, 0x1161, 0x0029, 0}, 0x3273 }, // U3273 # CIRCLED HANGUL PIEUP A
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x1109, 0x0029, 0, 0}, 0x3266 }, // U3266 # CIRCLED HANGUL SIOS
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x1109, 0x1161, 0x0029, 0}, 0x3274 }, // U3274 # CIRCLED HANGUL SIOS A
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x110b, 0x0029, 0, 0}, 0x3267 }, // U3267 # CIRCLED HANGUL IEUNG
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x110b, 0x1161, 0x0029, 0}, 0x3275 }, // U3275 # CIRCLED HANGUL IEUNG A
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x110c, 0x0029, 0, 0}, 0x3268 }, // U3268 # CIRCLED HANGUL CIEUC
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x110c, 0x1161, 0x0029, 0}, 0x3276 }, // U3276 # CIRCLED HANGUL CIEUC A
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x110e, 0x0029, 0, 0}, 0x3269 }, // U3269 # CIRCLED HANGUL CHIEUCH
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x110e, 0x1161, 0x0029, 0}, 0x3277 }, // U3277 # CIRCLED HANGUL CHIEUCH A
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x110f, 0x0029, 0, 0}, 0x326a }, // U326A # CIRCLED HANGUL KHIEUKH
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x110f, 0x1161, 0x0029, 0}, 0x3278 }, // U3278 # CIRCLED HANGUL KHIEUKH A
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x1110, 0x0029, 0, 0}, 0x326b }, // U326B # CIRCLED HANGUL THIEUTH
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x1110, 0x1161, 0x0029, 0}, 0x3279 }, // U3279 # CIRCLED HANGUL THIEUTH A
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x1111, 0x0029, 0, 0}, 0x326c }, // U326C # CIRCLED HANGUL PHIEUPH
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x1111, 0x1161, 0x0029, 0}, 0x327a }, // U327A # CIRCLED HANGUL PHIEUPH A
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x1112, 0x0029, 0, 0}, 0x326d }, // U326D # CIRCLED HANGUL HIEUH
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x1112, 0x1161, 0x0029, 0}, 0x327b }, // U327B # CIRCLED HANGUL HIEUH A
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x30a2, 0x0029, 0, 0}, 0x32d0 }, // U32D0 # CIRCLED KATAKANA A
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x30a4, 0x0029, 0, 0}, 0x32d1 }, // U32D1 # CIRCLED KATAKANA I
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x30a6, 0x0029, 0, 0}, 0x32d2 }, // U32D2 # CIRCLED KATAKANA U
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x30a8, 0x0029, 0, 0}, 0x32d3 }, // U32D3 # CIRCLED KATAKANA E
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x30aa, 0x0029, 0, 0}, 0x32d4 }, // U32D4 # CIRCLED KATAKANA O
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x30ab, 0x0029, 0, 0}, 0x32d5 }, // U32D5 # CIRCLED KATAKANA KA
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x30ad, 0x0029, 0, 0}, 0x32d6 }, // U32D6 # CIRCLED KATAKANA KI
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x30af, 0x0029, 0, 0}, 0x32d7 }, // U32D7 # CIRCLED KATAKANA KU
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x30b1, 0x0029, 0, 0}, 0x32d8 }, // U32D8 # CIRCLED KATAKANA KE
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x30b3, 0x0029, 0, 0}, 0x32d9 }, // U32D9 # CIRCLED KATAKANA KO
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x30b5, 0x0029, 0, 0}, 0x32da }, // U32DA # CIRCLED KATAKANA SA
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x30b7, 0x0029, 0, 0}, 0x32db }, // U32DB # CIRCLED KATAKANA SI
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x30b9, 0x0029, 0, 0}, 0x32dc }, // U32DC # CIRCLED KATAKANA SU
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x30bb, 0x0029, 0, 0}, 0x32dd }, // U32DD # CIRCLED KATAKANA SE
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x30bd, 0x0029, 0, 0}, 0x32de }, // U32DE # CIRCLED KATAKANA SO
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x30bf, 0x0029, 0, 0}, 0x32df }, // U32DF # CIRCLED KATAKANA TA
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x30c1, 0x0029, 0, 0}, 0x32e0 }, // U32E0 # CIRCLED KATAKANA TI
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x30c4, 0x0029, 0, 0}, 0x32e1 }, // U32E1 # CIRCLED KATAKANA TU
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x30c6, 0x0029, 0, 0}, 0x32e2 }, // U32E2 # CIRCLED KATAKANA TE
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x30c8, 0x0029, 0, 0}, 0x32e3 }, // U32E3 # CIRCLED KATAKANA TO
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x30ca, 0x0029, 0, 0}, 0x32e4 }, // U32E4 # CIRCLED KATAKANA NA
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x30cb, 0x0029, 0, 0}, 0x32e5 }, // U32E5 # CIRCLED KATAKANA NI
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x30cc, 0x0029, 0, 0}, 0x32e6 }, // U32E6 # CIRCLED KATAKANA NU
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x30cd, 0x0029, 0, 0}, 0x32e7 }, // U32E7 # CIRCLED KATAKANA NE
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x30ce, 0x0029, 0, 0}, 0x32e8 }, // U32E8 # CIRCLED KATAKANA NO
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x30cf, 0x0029, 0, 0}, 0x32e9 }, // U32E9 # CIRCLED KATAKANA HA
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x30d2, 0x0029, 0, 0}, 0x32ea }, // U32EA # CIRCLED KATAKANA HI
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x30d5, 0x0029, 0, 0}, 0x32eb }, // U32EB # CIRCLED KATAKANA HU
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x30d8, 0x0029, 0, 0}, 0x32ec }, // U32EC # CIRCLED KATAKANA HE
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x30db, 0x0029, 0, 0}, 0x32ed }, // U32ED # CIRCLED KATAKANA HO
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x30de, 0x0029, 0, 0}, 0x32ee }, // U32EE # CIRCLED KATAKANA MA
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x30df, 0x0029, 0, 0}, 0x32ef }, // U32EF # CIRCLED KATAKANA MI
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x30e0, 0x0029, 0, 0}, 0x32f0 }, // U32F0 # CIRCLED KATAKANA MU
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x30e1, 0x0029, 0, 0}, 0x32f1 }, // U32F1 # CIRCLED KATAKANA ME
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x30e2, 0x0029, 0, 0}, 0x32f2 }, // U32F2 # CIRCLED KATAKANA MO
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x30e4, 0x0029, 0, 0}, 0x32f3 }, // U32F3 # CIRCLED KATAKANA YA
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x30e6, 0x0029, 0, 0}, 0x32f4 }, // U32F4 # CIRCLED KATAKANA YU
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x30e8, 0x0029, 0, 0}, 0x32f5 }, // U32F5 # CIRCLED KATAKANA YO
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x30e9, 0x0029, 0, 0}, 0x32f6 }, // U32F6 # CIRCLED KATAKANA RA
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x30ea, 0x0029, 0, 0}, 0x32f7 }, // U32F7 # CIRCLED KATAKANA RI
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x30eb, 0x0029, 0, 0}, 0x32f8 }, // U32F8 # CIRCLED KATAKANA RU
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x30ec, 0x0029, 0, 0}, 0x32f9 }, // U32F9 # CIRCLED KATAKANA RE
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x30ed, 0x0029, 0, 0}, 0x32fa }, // U32FA # CIRCLED KATAKANA RO
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x30ef, 0x0029, 0, 0}, 0x32fb }, // U32FB # CIRCLED KATAKANA WA
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x30f0, 0x0029, 0, 0}, 0x32fc }, // U32FC # CIRCLED KATAKANA WI
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x30f1, 0x0029, 0, 0}, 0x32fd }, // U32FD # CIRCLED KATAKANA WE
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x30f2, 0x0029, 0, 0}, 0x32fe }, // U32FE # CIRCLED KATAKANA WO
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x4e00, 0x0029, 0, 0}, 0x3280 }, // U3280 # CIRCLED IDEOGRAPH ONE
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x4e03, 0x0029, 0, 0}, 0x3286 }, // U3286 # CIRCLED IDEOGRAPH SEVEN
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x4e09, 0x0029, 0, 0}, 0x3282 }, // U3282 # CIRCLED IDEOGRAPH THREE
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x4e0a, 0x0029, 0, 0}, 0x32a4 }, // U32A4 # CIRCLED IDEOGRAPH HIGH
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x4e0b, 0x0029, 0, 0}, 0x32a6 }, // U32A6 # CIRCLED IDEOGRAPH LOW
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x4e2d, 0x0029, 0, 0}, 0x32a5 }, // U32A5 # CIRCLED IDEOGRAPH CENTRE
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x4e5d, 0x0029, 0, 0}, 0x3288 }, // U3288 # CIRCLED IDEOGRAPH NINE
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x4e8c, 0x0029, 0, 0}, 0x3281 }, // U3281 # CIRCLED IDEOGRAPH TWO
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x4e94, 0x0029, 0, 0}, 0x3284 }, // U3284 # CIRCLED IDEOGRAPH FIVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x4f01, 0x0029, 0, 0}, 0x32ad }, // U32AD # CIRCLED IDEOGRAPH ENTERPRISE
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x4f11, 0x0029, 0, 0}, 0x32a1 }, // U32A1 # CIRCLED IDEOGRAPH REST
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x512a, 0x0029, 0, 0}, 0x329d }, // U329D # CIRCLED IDEOGRAPH EXCELLENT
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x516b, 0x0029, 0, 0}, 0x3287 }, // U3287 # CIRCLED IDEOGRAPH EIGHT
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x516d, 0x0029, 0, 0}, 0x3285 }, // U3285 # CIRCLED IDEOGRAPH SIX
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x5199, 0x0029, 0, 0}, 0x32a2 }, // U32A2 # CIRCLED IDEOGRAPH COPY
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x52b4, 0x0029, 0, 0}, 0x3298 }, // U3298 # CIRCLED IDEOGRAPH LABOR
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x533b, 0x0029, 0, 0}, 0x32a9 }, // U32A9 # CIRCLED IDEOGRAPH MEDICINE
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x5341, 0x0029, 0, 0}, 0x3289 }, // U3289 # CIRCLED IDEOGRAPH TEN
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x5354, 0x0029, 0, 0}, 0x32af }, // U32AF # CIRCLED IDEOGRAPH ALLIANCE
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x5370, 0x0029, 0, 0}, 0x329e }, // U329E # CIRCLED IDEOGRAPH PRINT
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x53f3, 0x0029, 0, 0}, 0x32a8 }, // U32A8 # CIRCLED IDEOGRAPH RIGHT
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x540d, 0x0029, 0, 0}, 0x3294 }, // U3294 # CIRCLED IDEOGRAPH NAME
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x56db, 0x0029, 0, 0}, 0x3283 }, // U3283 # CIRCLED IDEOGRAPH FOUR
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x571f, 0x0029, 0, 0}, 0x328f }, // U328F # CIRCLED IDEOGRAPH EARTH
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x591c, 0x0029, 0, 0}, 0x32b0 }, // U32B0 # CIRCLED IDEOGRAPH NIGHT
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x5973, 0x0029, 0, 0}, 0x329b }, // U329B # CIRCLED IDEOGRAPH FEMALE
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x5b66, 0x0029, 0, 0}, 0x32ab }, // U32AB # CIRCLED IDEOGRAPH STUDY
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x5b97, 0x0029, 0, 0}, 0x32aa }, // U32AA # CIRCLED IDEOGRAPH RELIGION
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x5de6, 0x0029, 0, 0}, 0x32a7 }, // U32A7 # CIRCLED IDEOGRAPH LEFT
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x65e5, 0x0029, 0, 0}, 0x3290 }, // U3290 # CIRCLED IDEOGRAPH SUN
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x6708, 0x0029, 0, 0}, 0x328a }, // U328A # CIRCLED IDEOGRAPH MOON
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x6709, 0x0029, 0, 0}, 0x3292 }, // U3292 # CIRCLED IDEOGRAPH HAVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x6728, 0x0029, 0, 0}, 0x328d }, // U328D # CIRCLED IDEOGRAPH WOOD
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x682a, 0x0029, 0, 0}, 0x3291 }, // U3291 # CIRCLED IDEOGRAPH STOCK
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x6b63, 0x0029, 0, 0}, 0x32a3 }, // U32A3 # CIRCLED IDEOGRAPH CORRECT
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x6c34, 0x0029, 0, 0}, 0x328c }, // U328C # CIRCLED IDEOGRAPH WATER
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x6ce8, 0x0029, 0, 0}, 0x329f }, // U329F # CIRCLED IDEOGRAPH ATTENTION
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x706b, 0x0029, 0, 0}, 0x328b }, // U328B # CIRCLED IDEOGRAPH FIRE
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x7279, 0x0029, 0, 0}, 0x3295 }, // U3295 # CIRCLED IDEOGRAPH SPECIAL
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x7537, 0x0029, 0, 0}, 0x329a }, // U329A # CIRCLED IDEOGRAPH MALE
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x76e3, 0x0029, 0, 0}, 0x32ac }, // U32AC # CIRCLED IDEOGRAPH SUPERVISE
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x793e, 0x0029, 0, 0}, 0x3293 }, // U3293 # CIRCLED IDEOGRAPH SOCIETY
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x795d, 0x0029, 0, 0}, 0x3297 }, // U3297 # CIRCLED IDEOGRAPH CONGRATULATION
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x79d8, 0x0029, 0, 0}, 0x3299 }, // U3299 # CIRCLED IDEOGRAPH SECRET
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x8ca1, 0x0029, 0, 0}, 0x3296 }, // U3296 # CIRCLED IDEOGRAPH FINANCIAL
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x8cc7, 0x0029, 0, 0}, 0x32ae }, // U32AE # CIRCLED IDEOGRAPH RESOURCE
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x9069, 0x0029, 0, 0}, 0x329c }, // U329C # CIRCLED IDEOGRAPH SUITABLE
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x91d1, 0x0029, 0, 0}, 0x328e }, // U328E # CIRCLED IDEOGRAPH METAL
    { {UNITIZE(TQt::Key_Multi_key), 0x0028, 0x9805, 0x0029, 0, 0}, 0x32a0 }, // U32A0 # CIRCLED IDEOGRAPH ITEM
    { {UNITIZE(TQt::Key_Multi_key), 0x0029, 0x0029, 0, 0, 0}, 0x005d }, // bracketright
    { {UNITIZE(TQt::Key_Multi_key), 0x0029, 0x002d, 0, 0, 0}, 0x007d }, // braceright
    { {UNITIZE(TQt::Key_Multi_key), 0x0029, 0x0391, 0, 0, 0}, 0x1f08 }, // U1F08 # GREEK CAPITAL LETTER ALPHA WITH PSILI
    { {UNITIZE(TQt::Key_Multi_key), 0x0029, 0x0395, 0, 0, 0}, 0x1f18 }, // U1F18 # GREEK CAPITAL LETTER EPSILON WITH PSILI
    { {UNITIZE(TQt::Key_Multi_key), 0x0029, 0x0397, 0, 0, 0}, 0x1f28 }, // U1F28 # GREEK CAPITAL LETTER ETA WITH PSILI
    { {UNITIZE(TQt::Key_Multi_key), 0x0029, 0x0399, 0, 0, 0}, 0x1f38 }, // U1F38 # GREEK CAPITAL LETTER IOTA WITH PSILI
    { {UNITIZE(TQt::Key_Multi_key), 0x0029, 0x039f, 0, 0, 0}, 0x1f48 }, // U1F48 # GREEK CAPITAL LETTER OMICRON WITH PSILI
    { {UNITIZE(TQt::Key_Multi_key), 0x0029, 0x03a9, 0, 0, 0}, 0x1f68 }, // U1F68 # GREEK CAPITAL LETTER OMEGA WITH PSILI
    { {UNITIZE(TQt::Key_Multi_key), 0x0029, 0x03b1, 0, 0, 0}, 0x1f00 }, // U1F00 # GREEK SMALL LETTER ALPHA WITH PSILI
    { {UNITIZE(TQt::Key_Multi_key), 0x0029, 0x03b5, 0, 0, 0}, 0x1f10 }, // U1F10 # GREEK SMALL LETTER EPSILON WITH PSILI
    { {UNITIZE(TQt::Key_Multi_key), 0x0029, 0x03b7, 0, 0, 0}, 0x1f20 }, // U1F20 # GREEK SMALL LETTER ETA WITH PSILI
    { {UNITIZE(TQt::Key_Multi_key), 0x0029, 0x03b9, 0, 0, 0}, 0x1f30 }, // U1F30 # GREEK SMALL LETTER IOTA WITH PSILI
    { {UNITIZE(TQt::Key_Multi_key), 0x0029, 0x03bf, 0, 0, 0}, 0x1f40 }, // U1F40 # GREEK SMALL LETTER OMICRON WITH PSILI
    { {UNITIZE(TQt::Key_Multi_key), 0x0029, 0x03c1, 0, 0, 0}, 0x1fe4 }, // U1FE4 # GREEK SMALL LETTER RHO WITH PSILI
    { {UNITIZE(TQt::Key_Multi_key), 0x0029, 0x03c5, 0, 0, 0}, 0x1f50 }, // U1F50 # GREEK SMALL LETTER UPSILON WITH PSILI
    { {UNITIZE(TQt::Key_Multi_key), 0x0029, 0x03c9, 0, 0, 0}, 0x1f60 }, // U1F60 # GREEK SMALL LETTER OMEGA WITH PSILI
    { {UNITIZE(TQt::Key_Multi_key), 0x002b, 0x002b, 0, 0, 0}, 0x0023 }, // numbersign
    { {UNITIZE(TQt::Key_Multi_key), 0x002b, 0x002d, 0, 0, 0}, 0x00b1 }, // plusminus
    { {UNITIZE(TQt::Key_Multi_key), 0x002b, 0x004f, 0, 0, 0}, 0x01a0 }, // U01A0 # LATIN CAPITAL LETTER O WITH HORN
    { {UNITIZE(TQt::Key_Multi_key), 0x002b, 0x0055, 0, 0, 0}, 0x01af }, // U01AF # LATIN CAPITAL LETTER U WITH HORN
    { {UNITIZE(TQt::Key_Multi_key), 0x002b, 0x006f, 0, 0, 0}, 0x01a1 }, // U01A1 # LATIN SMALL LETTER O WITH HORN
    { {UNITIZE(TQt::Key_Multi_key), 0x002b, 0x0075, 0, 0, 0}, 0x01b0 }, // U01B0 # LATIN SMALL LETTER U WITH HORN
    { {UNITIZE(TQt::Key_Multi_key), 0x002c, 0x0020, 0, 0, 0}, 0x00b8 }, // cedilla
    { {UNITIZE(TQt::Key_Multi_key), 0x002c, 0x0022, 0, 0, 0}, 0x201e }, // U201e # DOUBLE LOW-9 QUOTATION MARK
    { {UNITIZE(TQt::Key_Multi_key), 0x002c, 0x0027, 0, 0, 0}, 0x201a }, // U201a # SINGLE LOW-9 QUOTATION MARK
    { {UNITIZE(TQt::Key_Multi_key), 0x002c, 0x002d, 0, 0, 0}, 0x00ac }, // U00AC # NOT SIGN
    { {UNITIZE(TQt::Key_Multi_key), 0x002c, 0x0043, 0, 0, 0}, 0x00c7 }, // U00C7 # LATIN CAPITAL LETTER C WITH CEDILLA
    { {UNITIZE(TQt::Key_Multi_key), 0x002c, 0x0044, 0, 0, 0}, 0x1e10 }, // U1E10 # LATIN CAPITAL LETTER D WITH CEDILLA
    { {UNITIZE(TQt::Key_Multi_key), 0x002c, 0x0045, 0, 0, 0}, 0x0228 }, // U0228 # LATIN CAPITAL LETTER E WITH CEDILLA
    { {UNITIZE(TQt::Key_Multi_key), 0x002c, 0x0047, 0, 0, 0}, 0x0122 }, // U0122 # LATIN CAPITAL LETTER G WITH CEDILLA
    { {UNITIZE(TQt::Key_Multi_key), 0x002c, 0x0048, 0, 0, 0}, 0x1e28 }, // U1E28 # LATIN CAPITAL LETTER H WITH CEDILLA
    { {UNITIZE(TQt::Key_Multi_key), 0x002c, 0x004b, 0, 0, 0}, 0x0136 }, // U0136 # LATIN CAPITAL LETTER K WITH CEDILLA
    { {UNITIZE(TQt::Key_Multi_key), 0x002c, 0x004c, 0, 0, 0}, 0x013b }, // U013B # LATIN CAPITAL LETTER L WITH CEDILLA
    { {UNITIZE(TQt::Key_Multi_key), 0x002c, 0x004e, 0, 0, 0}, 0x0145 }, // U0145 # LATIN CAPITAL LETTER N WITH CEDILLA
    { {UNITIZE(TQt::Key_Multi_key), 0x002c, 0x0052, 0, 0, 0}, 0x0156 }, // U0156 # LATIN CAPITAL LETTER R WITH CEDILLA
    { {UNITIZE(TQt::Key_Multi_key), 0x002c, 0x0053, 0, 0, 0}, 0x015e }, // U015E # LATIN CAPITAL LETTER S WITH CEDILLA
    { {UNITIZE(TQt::Key_Multi_key), 0x002c, 0x0054, 0, 0, 0}, 0x0162 }, // U0162 # LATIN CAPITAL LETTER T WITH CEDILLA
    { {UNITIZE(TQt::Key_Multi_key), 0x002c, 0x0063, 0, 0, 0}, 0x00e7 }, // U00E7 # LATIN SMALL LETTER C WITH CEDILLA
    { {UNITIZE(TQt::Key_Multi_key), 0x002c, 0x0064, 0, 0, 0}, 0x1e11 }, // U1E11 # LATIN SMALL LETTER D WITH CEDILLA
    { {UNITIZE(TQt::Key_Multi_key), 0x002c, 0x0065, 0, 0, 0}, 0x0229 }, // U0229 # LATIN SMALL LETTER E WITH CEDILLA
    { {UNITIZE(TQt::Key_Multi_key), 0x002c, 0x0067, 0, 0, 0}, 0x0123 }, // U0123 # LATIN SMALL LETTER G WITH CEDILLA
    { {UNITIZE(TQt::Key_Multi_key), 0x002c, 0x0068, 0, 0, 0}, 0x1e29 }, // U1E29 # LATIN SMALL LETTER H WITH CEDILLA
    { {UNITIZE(TQt::Key_Multi_key), 0x002c, 0x006b, 0, 0, 0}, 0x0137 }, // U0137 # LATIN SMALL LETTER K WITH CEDILLA
    { {UNITIZE(TQt::Key_Multi_key), 0x002c, 0x006c, 0, 0, 0}, 0x013c }, // U013C # LATIN SMALL LETTER L WITH CEDILLA
    { {UNITIZE(TQt::Key_Multi_key), 0x002c, 0x006e, 0, 0, 0}, 0x0146 }, // U0146 # LATIN SMALL LETTER N WITH CEDILLA
    { {UNITIZE(TQt::Key_Multi_key), 0x002c, 0x0072, 0, 0, 0}, 0x0157 }, // U0157 # LATIN SMALL LETTER R WITH CEDILLA
    { {UNITIZE(TQt::Key_Multi_key), 0x002c, 0x0073, 0, 0, 0}, 0x015f }, // U015F # LATIN SMALL LETTER S WITH CEDILLA
    { {UNITIZE(TQt::Key_Multi_key), 0x002c, 0x0074, 0, 0, 0}, 0x0163 }, // U0163 # LATIN SMALL LETTER T WITH CEDILLA
    { {UNITIZE(TQt::Key_Multi_key), 0x002d, 0x0020, 0, 0, 0}, 0x007e }, // asciitilde
    { {UNITIZE(TQt::Key_Multi_key), 0x002d, 0x0028, 0, 0, 0}, 0x007b }, // braceleft
    { {UNITIZE(TQt::Key_Multi_key), 0x002d, 0x0029, 0, 0, 0}, 0x007d }, // braceright
    { {UNITIZE(TQt::Key_Multi_key), 0x002d, 0x002c, 0, 0, 0}, 0x00ac }, // U00AC # NOT SIGN
    { {UNITIZE(TQt::Key_Multi_key), 0x002d, 0x002d, 0x002d, 0, 0}, 0x2014 }, // U2014 # EM DASH
    { {UNITIZE(TQt::Key_Multi_key), 0x002d, 0x002d, 0x002e, 0, 0}, 0x2013 }, // U2013 # EN DASH
    { {UNITIZE(TQt::Key_Multi_key), 0x002d, 0x003a, 0, 0, 0}, 0x00f7 }, // U00F7 # DIVISION SIGN
    { {UNITIZE(TQt::Key_Multi_key), 0x002d, 0x0044, 0, 0, 0}, 0x0110 }, // Dstroke
    { {UNITIZE(TQt::Key_Multi_key), 0x002d, 0x004c, 0, 0, 0}, 0x00a3 }, // U00a3 # POUND SIGN
    { {UNITIZE(TQt::Key_Multi_key), 0x002d, 0x0064, 0, 0, 0}, 0x0111 }, // dstroke
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x002e, 0x0000, 0x0053, 0, 0}, 0x1e68 }, // U1E68 # LATIN CAPITAL LETTER S WITH DOT BELOW AND DOT ABOVE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x002e, 0x0000, 0x0053, 0, 0}, 0x1e64 }, // U1E64 # LATIN CAPITAL LETTER S WITH ACUTE AND DOT ABOVE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x002e, 0x0000, 0x0073, 0, 0}, 0x1e69 }, // U1E69 # LATIN SMALL LETTER S WITH DOT BELOW AND DOT ABOVE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x002e, 0x0000, 0x0073, 0, 0}, 0x1e65 }, // U1E65 # LATIN SMALL LETTER S WITH ACUTE AND DOT ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x002e, 0x0021, 0x0053, 0, 0}, 0x1e68 }, // U1E68 # LATIN CAPITAL LETTER S WITH DOT BELOW AND DOT ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x002e, 0x0021, 0x0073, 0, 0}, 0x1e69 }, // U1E69 # LATIN SMALL LETTER S WITH DOT BELOW AND DOT ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x002e, 0x0027, 0x0053, 0, 0}, 0x1e64 }, // U1E64 # LATIN CAPITAL LETTER S WITH ACUTE AND DOT ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x002e, 0x0027, 0x0073, 0, 0}, 0x1e65 }, // U1E65 # LATIN SMALL LETTER S WITH ACUTE AND DOT ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x002e, 0x002e, 0, 0, 0}, 0x00b7 }, // U00B7
    { {UNITIZE(TQt::Key_Multi_key), 0x002e, 0x003c, 0, 0, 0}, 0x2039 }, // U2039
    { {UNITIZE(TQt::Key_Multi_key), 0x002e, 0x003e, 0, 0, 0}, 0x203a }, // U203a
    { {UNITIZE(TQt::Key_Multi_key), 0x002e, 0x0041, 0, 0, 0}, 0x0226 }, // U0226 # LATIN CAPITAL LETTER A WITH DOT ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x002e, 0x0042, 0, 0, 0}, 0x1e02 }, // U1E02 # LATIN CAPITAL LETTER B WITH DOT ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x002e, 0x0043, 0, 0, 0}, 0x010a }, // U010A # LATIN CAPITAL LETTER C WITH DOT ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x002e, 0x0044, 0, 0, 0}, 0x1e0a }, // U1E0A # LATIN CAPITAL LETTER D WITH DOT ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x002e, 0x0045, 0, 0, 0}, 0x0116 }, // U0116 # LATIN CAPITAL LETTER E WITH DOT ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x002e, 0x0046, 0, 0, 0}, 0x1e1e }, // U1E1E # LATIN CAPITAL LETTER F WITH DOT ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x002e, 0x0047, 0, 0, 0}, 0x0120 }, // U0120 # LATIN CAPITAL LETTER G WITH DOT ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x002e, 0x0048, 0, 0, 0}, 0x1e22 }, // U1E22 # LATIN CAPITAL LETTER H WITH DOT ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x002e, 0x0049, 0, 0, 0}, 0x0130 }, // U0130 # LATIN CAPITAL LETTER I WITH DOT ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x002e, 0x004d, 0, 0, 0}, 0x1e40 }, // U1E40 # LATIN CAPITAL LETTER M WITH DOT ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x002e, 0x004e, 0, 0, 0}, 0x1e44 }, // U1E44 # LATIN CAPITAL LETTER N WITH DOT ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x002e, 0x004f, 0, 0, 0}, 0x022e }, // U022E # LATIN CAPITAL LETTER O WITH DOT ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x002e, 0x0050, 0, 0, 0}, 0x1e56 }, // U1E56 # LATIN CAPITAL LETTER P WITH DOT ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x002e, 0x0052, 0, 0, 0}, 0x1e58 }, // U1E58 # LATIN CAPITAL LETTER R WITH DOT ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x002e, 0x0053, 0, 0, 0}, 0x1e60 }, // U1E60 # LATIN CAPITAL LETTER S WITH DOT ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x002e, 0x0054, 0, 0, 0}, 0x1e6a }, // U1E6A # LATIN CAPITAL LETTER T WITH DOT ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x002e, 0x0057, 0, 0, 0}, 0x1e86 }, // U1E86 # LATIN CAPITAL LETTER W WITH DOT ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x002e, 0x0058, 0, 0, 0}, 0x1e8a }, // U1E8A # LATIN CAPITAL LETTER X WITH DOT ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x002e, 0x0059, 0, 0, 0}, 0x1e8e }, // U1E8E # LATIN CAPITAL LETTER Y WITH DOT ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x002e, 0x005a, 0, 0, 0}, 0x017b }, // U017B # LATIN CAPITAL LETTER Z WITH DOT ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x002e, 0x0061, 0, 0, 0}, 0x0227 }, // U0227 # LATIN SMALL LETTER A WITH DOT ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x002e, 0x0062, 0, 0, 0}, 0x1e03 }, // U1E03 # LATIN SMALL LETTER B WITH DOT ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x002e, 0x0063, 0, 0, 0}, 0x010b }, // U010B # LATIN SMALL LETTER C WITH DOT ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x002e, 0x0063, 0x0053, 0, 0}, 0x1e66 }, // U1E66 # LATIN CAPITAL LETTER S WITH CARON AND DOT ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x002e, 0x0063, 0x0073, 0, 0}, 0x1e67 }, // U1E67 # LATIN SMALL LETTER S WITH CARON AND DOT ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x002e, 0x0064, 0, 0, 0}, 0x1e0b }, // U1E0B # LATIN SMALL LETTER D WITH DOT ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x002e, 0x0065, 0, 0, 0}, 0x0117 }, // U0117 # LATIN SMALL LETTER E WITH DOT ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x002e, 0x0066, 0, 0, 0}, 0x1e1f }, // U1E1F # LATIN SMALL LETTER F WITH DOT ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x002e, 0x0067, 0, 0, 0}, 0x0121 }, // U0121 # LATIN SMALL LETTER G WITH DOT ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x002e, 0x0068, 0, 0, 0}, 0x1e23 }, // U1E23 # LATIN SMALL LETTER H WITH DOT ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x002e, 0x006d, 0, 0, 0}, 0x1e41 }, // U1E41 # LATIN SMALL LETTER M WITH DOT ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x002e, 0x006e, 0, 0, 0}, 0x1e45 }, // U1E45 # LATIN SMALL LETTER N WITH DOT ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x002e, 0x006f, 0, 0, 0}, 0x022f }, // U022F # LATIN SMALL LETTER O WITH DOT ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x002e, 0x0070, 0, 0, 0}, 0x1e57 }, // U1E57 # LATIN SMALL LETTER P WITH DOT ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x002e, 0x0072, 0, 0, 0}, 0x1e59 }, // U1E59 # LATIN SMALL LETTER R WITH DOT ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x002e, 0x0073, 0, 0, 0}, 0x1e61 }, // U1E61 # LATIN SMALL LETTER S WITH DOT ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x002e, 0x0074, 0, 0, 0}, 0x1e6b }, // U1E6B # LATIN SMALL LETTER T WITH DOT ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x002e, 0x0077, 0, 0, 0}, 0x1e87 }, // U1E87 # LATIN SMALL LETTER W WITH DOT ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x002e, 0x0078, 0, 0, 0}, 0x1e8b }, // U1E8B # LATIN SMALL LETTER X WITH DOT ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x002e, 0x0079, 0, 0, 0}, 0x1e8f }, // U1E8F # LATIN SMALL LETTER Y WITH DOT ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x002e, 0x007a, 0, 0, 0}, 0x017c }, // U017C # LATIN SMALL LETTER Z WITH DOT ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x002e, 0x00b4, 0x0053, 0, 0}, 0x1e64 }, // U1E64 # LATIN CAPITAL LETTER S WITH ACUTE AND DOT ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x002e, 0x00b4, 0x0073, 0, 0}, 0x1e65 }, // U1E65 # LATIN SMALL LETTER S WITH ACUTE AND DOT ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x002e, 0x015a, 0, 0, 0}, 0x1e64 }, // U1E64 # LATIN CAPITAL LETTER S WITH ACUTE AND DOT ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x002e, 0x015b, 0, 0, 0}, 0x1e65 }, // U1E65 # LATIN SMALL LETTER S WITH ACUTE AND DOT ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x002e, 0x0160, 0, 0, 0}, 0x1e66 }, // U1E66 # LATIN CAPITAL LETTER S WITH CARON AND DOT ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x002e, 0x0161, 0, 0, 0}, 0x1e67 }, // U1E67 # LATIN SMALL LETTER S WITH CARON AND DOT ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x002e, 0x017f, 0, 0, 0}, 0x1e9b }, // U1E9B # LATIN SMALL LETTER LONG S WITH DOT ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x002e, 0x1e62, 0, 0, 0}, 0x1e68 }, // U1E68 # LATIN CAPITAL LETTER S WITH DOT BELOW AND DOT ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x002e, 0x1e63, 0, 0, 0}, 0x1e69 }, // U1E69 # LATIN SMALL LETTER S WITH DOT BELOW AND DOT ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x002e, UNITIZE(TQt::Key_Dead_Acute), 0x0053, 0, 0}, 0x1e64 }, // U1E64 # LATIN CAPITAL LETTER S WITH ACUTE AND DOT ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x002e, UNITIZE(TQt::Key_Dead_Acute), 0x0073, 0, 0}, 0x1e65 }, // U1E65 # LATIN SMALL LETTER S WITH ACUTE AND DOT ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x002e, UNITIZE(TQt::Key_Dead_Caron), 0x0053, 0, 0}, 0x1e66 }, // U1E66 # LATIN CAPITAL LETTER S WITH CARON AND DOT ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x002e, UNITIZE(TQt::Key_Dead_Caron), 0x0073, 0, 0}, 0x1e67 }, // U1E67 # LATIN SMALL LETTER S WITH CARON AND DOT ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x002e, UNITIZE(TQt::Key_Dead_Belowdot), 0x0053, 0, 0}, 0x1e68 }, // U1E68 # LATIN CAPITAL LETTER S WITH DOT BELOW AND DOT ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x002e, UNITIZE(TQt::Key_Dead_Belowdot), 0x0073, 0, 0}, 0x1e69 }, // U1E69 # LATIN SMALL LETTER S WITH DOT BELOW AND DOT ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x002f, 0x002f, 0, 0, 0}, 0x005c }, // backslash
    { {UNITIZE(TQt::Key_Multi_key), 0x002f, 0x003c, 0, 0, 0}, 0x005c }, // backslash
    { {UNITIZE(TQt::Key_Multi_key), 0x002f, 0x0043, 0, 0, 0}, 0x20a1 }, // U20a1 # COLON SIGN
    { {UNITIZE(TQt::Key_Multi_key), 0x002f, 0x0043, 0, 0, 0}, 0x00a2 }, // U00A2 # CENT SIGN
    { {UNITIZE(TQt::Key_Multi_key), 0x002f, 0x0044, 0, 0, 0}, 0x0110 }, // U0110 # LATIN CAPITAL LETTER D WITH STROKE
    { {UNITIZE(TQt::Key_Multi_key), 0x002f, 0x0047, 0, 0, 0}, 0x01e4 }, // U01E4 # LATIN CAPITAL LETTER G WITH STROKE
    { {UNITIZE(TQt::Key_Multi_key), 0x002f, 0x0048, 0, 0, 0}, 0x0126 }, // U0126 # LATIN CAPITAL LETTER H WITH STROKE
    { {UNITIZE(TQt::Key_Multi_key), 0x002f, 0x0049, 0, 0, 0}, 0x0197 }, // U0197 # LATIN CAPITAL LETTER I WITH STROKE
    { {UNITIZE(TQt::Key_Multi_key), 0x002f, 0x004c, 0, 0, 0}, 0x0141 }, // U0141 # LATIN CAPITAL LETTER L WITH STROKE
    { {UNITIZE(TQt::Key_Multi_key), 0x002f, 0x004f, 0, 0, 0}, 0x00d8 }, // U00D8 # LATIN CAPITAL LETTER O WITH STROKE
    { {UNITIZE(TQt::Key_Multi_key), 0x002f, 0x0054, 0, 0, 0}, 0x0166 }, // U0166 # LATIN CAPITAL LETTER T WITH STROKE
    { {UNITIZE(TQt::Key_Multi_key), 0x002f, 0x005a, 0, 0, 0}, 0x01b5 }, // U01B5 # LATIN CAPITAL LETTER Z WITH STROKE
    { {UNITIZE(TQt::Key_Multi_key), 0x002f, 0x005e, 0, 0, 0}, 0x007c }, // bar
    { {UNITIZE(TQt::Key_Multi_key), 0x002f, 0x0062, 0, 0, 0}, 0x0180 }, // U0180 # LATIN SMALL LETTER B WITH STROKE
    { {UNITIZE(TQt::Key_Multi_key), 0x002f, 0x0063, 0, 0, 0}, 0x00a2 }, // U00A2 # CENT SIGN
    { {UNITIZE(TQt::Key_Multi_key), 0x002f, 0x0064, 0, 0, 0}, 0x0111 }, // U0111 # LATIN SMALL LETTER D WITH STROKE
    { {UNITIZE(TQt::Key_Multi_key), 0x002f, 0x0067, 0, 0, 0}, 0x01e5 }, // U01E5 # LATIN SMALL LETTER G WITH STROKE
    { {UNITIZE(TQt::Key_Multi_key), 0x002f, 0x0068, 0, 0, 0}, 0x0127 }, // U0127 # LATIN SMALL LETTER H WITH STROKE
    { {UNITIZE(TQt::Key_Multi_key), 0x002f, 0x0069, 0, 0, 0}, 0x0268 }, // U0268 # LATIN SMALL LETTER I WITH STROKE
    { {UNITIZE(TQt::Key_Multi_key), 0x002f, 0x006c, 0, 0, 0}, 0x0142 }, // U0142 # LATIN SMALL LETTER L WITH STROKE
    { {UNITIZE(TQt::Key_Multi_key), 0x002f, 0x006d, 0, 0, 0}, 0x20a5 }, // U20a5 # MILL SIGN
    { {UNITIZE(TQt::Key_Multi_key), 0x002f, 0x006f, 0, 0, 0}, 0x00f8 }, // U00F8 # LATIN SMALL LETTER O WITH STROKE
    { {UNITIZE(TQt::Key_Multi_key), 0x002f, 0x0074, 0, 0, 0}, 0x0167 }, // U0167 # LATIN SMALL LETTER T WITH STROKE
    { {UNITIZE(TQt::Key_Multi_key), 0x002f, 0x007a, 0, 0, 0}, 0x01b6 }, // U01B6 # LATIN SMALL LETTER Z WITH STROKE
    { {UNITIZE(TQt::Key_Multi_key), 0x002f, 0x0294, 0, 0, 0}, 0x02a1 }, // U02A1 # LATIN LETTER GLOTTAL STOP WITH STROKE
    { {UNITIZE(TQt::Key_Multi_key), 0x002f, 0x0413, 0, 0, 0}, 0x0492 }, // U0492 # CYRILLIC CAPITAL LETTER GHE WITH STROKE
    { {UNITIZE(TQt::Key_Multi_key), 0x002f, 0x041a, 0, 0, 0}, 0x049e }, // U049E # CYRILLIC CAPITAL LETTER KA WITH STROKE
    { {UNITIZE(TQt::Key_Multi_key), 0x002f, 0x0433, 0, 0, 0}, 0x0493 }, // U0493 # CYRILLIC SMALL LETTER GHE WITH STROKE
    { {UNITIZE(TQt::Key_Multi_key), 0x002f, 0x043a, 0, 0, 0}, 0x049f }, // U049F # CYRILLIC SMALL LETTER KA WITH STROKE
    { {UNITIZE(TQt::Key_Multi_key), 0x002f, 0x04ae, 0, 0, 0}, 0x04b0 }, // U04B0 # CYRILLIC CAPITAL LETTER STRAIGHT U WITH STROKE
    { {UNITIZE(TQt::Key_Multi_key), 0x002f, 0x04af, 0, 0, 0}, 0x04b1 }, // U04B1 # CYRILLIC SMALL LETTER STRAIGHT U WITH STROKE
    { {UNITIZE(TQt::Key_Multi_key), 0x002f, 0x2190, 0, 0, 0}, 0x219a }, // U219A # LEFTWARDS ARROW WITH STROKE
    { {UNITIZE(TQt::Key_Multi_key), 0x002f, 0x2192, 0, 0, 0}, 0x219b }, // U219B # RIGHTWARDS ARROW WITH STROKE
    { {UNITIZE(TQt::Key_Multi_key), 0x002f, 0x2194, 0, 0, 0}, 0x21ae }, // U21AE # LEFT RIGHT ARROW WITH STROKE
    { {UNITIZE(TQt::Key_Multi_key), 0x0031, 0x0032, 0, 0, 0}, 0x00bd }, // U00BD # VULGAR FRACTION ONE HALF
    { {UNITIZE(TQt::Key_Multi_key), 0x0031, 0x0034, 0, 0, 0}, 0x00bc }, // U00BC # VULGAR FRACTION ONE QUARTER
    { {UNITIZE(TQt::Key_Multi_key), 0x0033, 0x0034, 0, 0, 0}, 0x00be }, // U00BE # VULGAR FRACTION THREE QUARTERS
    { {UNITIZE(TQt::Key_Multi_key), 0x003a, 0x002d, 0, 0, 0}, 0x00f7 }, // U00F7 # DIVISION SIGN
    { {UNITIZE(TQt::Key_Multi_key), 0x003b, 0x0041, 0, 0, 0}, 0x0104 }, // U0104 # LATIN CAPITAL LETTER A WITH OGONEK
    { {UNITIZE(TQt::Key_Multi_key), 0x003b, 0x0045, 0, 0, 0}, 0x0118 }, // U0118 # LATIN CAPITAL LETTER E WITH OGONEK
    { {UNITIZE(TQt::Key_Multi_key), 0x003b, 0x0049, 0, 0, 0}, 0x012e }, // U012E # LATIN CAPITAL LETTER I WITH OGONEK
    { {UNITIZE(TQt::Key_Multi_key), 0x003b, 0x004f, 0, 0, 0}, 0x01ea }, // U01EA # LATIN CAPITAL LETTER O WITH OGONEK
    { {UNITIZE(TQt::Key_Multi_key), 0x003b, 0x0055, 0, 0, 0}, 0x0172 }, // U0172 # LATIN CAPITAL LETTER U WITH OGONEK
    { {UNITIZE(TQt::Key_Multi_key), 0x003b, 0x0061, 0, 0, 0}, 0x0105 }, // U0105 # LATIN SMALL LETTER A WITH OGONEK
    { {UNITIZE(TQt::Key_Multi_key), 0x003b, 0x0065, 0, 0, 0}, 0x0119 }, // U0119 # LATIN SMALL LETTER E WITH OGONEK
    { {UNITIZE(TQt::Key_Multi_key), 0x003b, 0x0069, 0, 0, 0}, 0x012f }, // U012F # LATIN SMALL LETTER I WITH OGONEK
    { {UNITIZE(TQt::Key_Multi_key), 0x003b, 0x006f, 0, 0, 0}, 0x01eb }, // U01EB # LATIN SMALL LETTER O WITH OGONEK
    { {UNITIZE(TQt::Key_Multi_key), 0x003b, 0x0075, 0, 0, 0}, 0x0173 }, // U0173 # LATIN SMALL LETTER U WITH OGONEK
    { {UNITIZE(TQt::Key_Multi_key), 0x003c, 0x0022, 0, 0, 0}, 0x201c }, // U201c # LEFT DOUBLE QUOTATION MARK
    { {UNITIZE(TQt::Key_Multi_key), 0x003c, 0x0027, 0, 0, 0}, 0x2018 }, // U2018 # LEFT SINGLE QUOTATION MARK
    { {UNITIZE(TQt::Key_Multi_key), 0x003c, 0x002f, 0, 0, 0}, 0x005c }, // backslash
    { {UNITIZE(TQt::Key_Multi_key), 0x003c, 0x003c, 0, 0, 0}, 0x00ab }, // guillemotleft # LEFT-POINTING DOUBLE ANGLE QUOTATION MARK
    { {UNITIZE(TQt::Key_Multi_key), 0x003c, 0x0338, 0, 0, 0}, 0x226e }, // U226E # NOT LESS-THAN
    { {UNITIZE(TQt::Key_Multi_key), 0x003c, 0x0338, 0, 0, 0}, 0x226e }, // U226E # NOT LESS-THAN
    { {UNITIZE(TQt::Key_Multi_key), 0x003d, 0x0043, 0, 0, 0}, 0x20ac }, // EuroSign # EURO SIGN
    { {UNITIZE(TQt::Key_Multi_key), 0x003d, 0x0045, 0, 0, 0}, 0x20ac }, // EuroSign # EURO SIGN
    { {UNITIZE(TQt::Key_Multi_key), 0x003d, 0x004c, 0, 0, 0}, 0x20a4 }, // U20a4 # LIRA SIGN
    { {UNITIZE(TQt::Key_Multi_key), 0x003d, 0x004e, 0, 0, 0}, 0x20a6 }, // U20a6 # NAIRA SIGN
    { {UNITIZE(TQt::Key_Multi_key), 0x003d, 0x004f, 0, 0, 0}, 0x0150 }, // U0150 # LATIN CAPITAL LETTER O WITH DOUBLE ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x003d, 0x0055, 0, 0, 0}, 0x0170 }, // U0170 # LATIN CAPITAL LETTER U WITH DOUBLE ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x003d, 0x0057, 0, 0, 0}, 0x20a9 }, // U20a9 # WON SIGN
    { {UNITIZE(TQt::Key_Multi_key), 0x003d, 0x0059, 0, 0, 0}, 0x00a5 }, // yen
    { {UNITIZE(TQt::Key_Multi_key), 0x003d, 0x006f, 0, 0, 0}, 0x0151 }, // U0151 # LATIN SMALL LETTER O WITH DOUBLE ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x003d, 0x0075, 0, 0, 0}, 0x0171 }, // U0171 # LATIN SMALL LETTER U WITH DOUBLE ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x003d, 0x0338, 0, 0, 0}, 0x2260 }, // U2260 # NOT EQUAL TO
    { {UNITIZE(TQt::Key_Multi_key), 0x003d, 0x0423, 0, 0, 0}, 0x04f2 }, // U04F2 # CYRILLIC CAPITAL LETTER U WITH DOUBLE ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x003d, 0x0443, 0, 0, 0}, 0x04f3 }, // U04F3 # CYRILLIC SMALL LETTER U WITH DOUBLE ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x003e, 0x0020, 0, 0, 0}, 0x005e }, // asciicircum
    { {UNITIZE(TQt::Key_Multi_key), 0x003e, 0x0022, 0, 0, 0}, 0x201d }, // U201d # RIGHT DOUBLE QUOTATION MARK
    { {UNITIZE(TQt::Key_Multi_key), 0x003e, 0x0027, 0, 0, 0}, 0x2019 }, // U2019 # RIGHT SINGLE QUOTATION MARK
    { {UNITIZE(TQt::Key_Multi_key), 0x003e, 0x003e, 0, 0, 0}, 0x00bb }, // guillemotright # RIGHT-POINTING DOUBLE ANGLE QUOTATION MARK
    { {UNITIZE(TQt::Key_Multi_key), 0x003e, 0x0338, 0, 0, 0}, 0x226f }, // U226F # NOT GREATER-THAN
    { {UNITIZE(TQt::Key_Multi_key), 0x003e, 0x0338, 0, 0, 0}, 0x226f }, // U226F # NOT GREATER-THAN
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x003f, 0x0000, 0, 0, 0}, 0x1eed }, // U1EED # LATIN SMALL LETTER U WITH HORN AND HOOK ABOVE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x003f, 0x0000, 0, 0, 0}, 0x1eec }, // U1EEC # LATIN CAPITAL LETTER U WITH HORN AND HOOK ABOVE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x003f, 0x0000, 0, 0, 0}, 0x1edf }, // U1EDF # LATIN SMALL LETTER O WITH HORN AND HOOK ABOVE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x003f, 0x0000, 0, 0, 0}, 0x1ede }, // U1EDE # LATIN CAPITAL LETTER O WITH HORN AND HOOK ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x003f, 0x002b, 0x004f, 0, 0}, 0x1ede }, // U1EDE # LATIN CAPITAL LETTER O WITH HORN AND HOOK ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x003f, 0x002b, 0x0055, 0, 0}, 0x1eec }, // U1EEC # LATIN CAPITAL LETTER U WITH HORN AND HOOK ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x003f, 0x002b, 0x006f, 0, 0}, 0x1edf }, // U1EDF # LATIN SMALL LETTER O WITH HORN AND HOOK ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x003f, 0x002b, 0x0075, 0, 0}, 0x1eed }, // U1EED # LATIN SMALL LETTER U WITH HORN AND HOOK ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x003f, 0x003f, 0, 0, 0}, 0x00bf }, // questiondown
    { {UNITIZE(TQt::Key_Multi_key), 0x003f, 0x0041, 0, 0, 0}, 0x1ea2 }, // U1EA2 # LATIN CAPITAL LETTER A WITH HOOK ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x003f, 0x0045, 0, 0, 0}, 0x1eba }, // U1EBA # LATIN CAPITAL LETTER E WITH HOOK ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x003f, 0x0049, 0, 0, 0}, 0x1ec8 }, // U1EC8 # LATIN CAPITAL LETTER I WITH HOOK ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x003f, 0x004f, 0, 0, 0}, 0x1ece }, // U1ECE # LATIN CAPITAL LETTER O WITH HOOK ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x003f, 0x0055, 0, 0, 0}, 0x1ee6 }, // U1EE6 # LATIN CAPITAL LETTER U WITH HOOK ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x003f, 0x0055, 0x0041, 0, 0}, 0x1eb2 }, // U1EB2 # LATIN CAPITAL LETTER A WITH BREVE AND HOOK ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x003f, 0x0055, 0x0061, 0, 0}, 0x1eb3 }, // U1EB3 # LATIN SMALL LETTER A WITH BREVE AND HOOK ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x003f, 0x0059, 0, 0, 0}, 0x1ef6 }, // U1EF6 # LATIN CAPITAL LETTER Y WITH HOOK ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x003f, 0x005e, 0x0041, 0, 0}, 0x1ea8 }, // U1EA8 # LATIN CAPITAL LETTER A WITH CIRCUMFLEX AND HOOK ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x003f, 0x005e, 0x0045, 0, 0}, 0x1ec2 }, // U1EC2 # LATIN CAPITAL LETTER E WITH CIRCUMFLEX AND HOOK ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x003f, 0x005e, 0x004f, 0, 0}, 0x1ed4 }, // U1ED4 # LATIN CAPITAL LETTER O WITH CIRCUMFLEX AND HOOK ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x003f, 0x005e, 0x0061, 0, 0}, 0x1ea9 }, // U1EA9 # LATIN SMALL LETTER A WITH CIRCUMFLEX AND HOOK ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x003f, 0x005e, 0x0065, 0, 0}, 0x1ec3 }, // U1EC3 # LATIN SMALL LETTER E WITH CIRCUMFLEX AND HOOK ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x003f, 0x005e, 0x006f, 0, 0}, 0x1ed5 }, // U1ED5 # LATIN SMALL LETTER O WITH CIRCUMFLEX AND HOOK ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x003f, 0x0061, 0, 0, 0}, 0x1ea3 }, // U1EA3 # LATIN SMALL LETTER A WITH HOOK ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x003f, 0x0062, 0x0041, 0, 0}, 0x1eb2 }, // U1EB2 # LATIN CAPITAL LETTER A WITH BREVE AND HOOK ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x003f, 0x0062, 0x0061, 0, 0}, 0x1eb3 }, // U1EB3 # LATIN SMALL LETTER A WITH BREVE AND HOOK ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x003f, 0x0065, 0, 0, 0}, 0x1ebb }, // U1EBB # LATIN SMALL LETTER E WITH HOOK ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x003f, 0x0069, 0, 0, 0}, 0x1ec9 }, // U1EC9 # LATIN SMALL LETTER I WITH HOOK ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x003f, 0x006f, 0, 0, 0}, 0x1ecf }, // U1ECF # LATIN SMALL LETTER O WITH HOOK ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x003f, 0x0075, 0, 0, 0}, 0x1ee7 }, // U1EE7 # LATIN SMALL LETTER U WITH HOOK ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x003f, 0x0079, 0, 0, 0}, 0x1ef7 }, // U1EF7 # LATIN SMALL LETTER Y WITH HOOK ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x003f, 0x00c2, 0, 0, 0}, 0x1ea8 }, // U1EA8 # LATIN CAPITAL LETTER A WITH CIRCUMFLEX AND HOOK ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x003f, 0x00ca, 0, 0, 0}, 0x1ec2 }, // U1EC2 # LATIN CAPITAL LETTER E WITH CIRCUMFLEX AND HOOK ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x003f, 0x00d4, 0, 0, 0}, 0x1ed4 }, // U1ED4 # LATIN CAPITAL LETTER O WITH CIRCUMFLEX AND HOOK ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x003f, 0x00e2, 0, 0, 0}, 0x1ea9 }, // U1EA9 # LATIN SMALL LETTER A WITH CIRCUMFLEX AND HOOK ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x003f, 0x00ea, 0, 0, 0}, 0x1ec3 }, // U1EC3 # LATIN SMALL LETTER E WITH CIRCUMFLEX AND HOOK ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x003f, 0x00f4, 0, 0, 0}, 0x1ed5 }, // U1ED5 # LATIN SMALL LETTER O WITH CIRCUMFLEX AND HOOK ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x003f, 0x0102, 0, 0, 0}, 0x1eb2 }, // U1EB2 # LATIN CAPITAL LETTER A WITH BREVE AND HOOK ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x003f, 0x0103, 0, 0, 0}, 0x1eb3 }, // U1EB3 # LATIN SMALL LETTER A WITH BREVE AND HOOK ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x003f, UNITIZE(TQt::Key_Dead_Circumflex), 0x0041, 0, 0}, 0x1ea8 }, // U1EA8 # LATIN CAPITAL LETTER A WITH CIRCUMFLEX AND HOOK ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x003f, UNITIZE(TQt::Key_Dead_Circumflex), 0x0045, 0, 0}, 0x1ec2 }, // U1EC2 # LATIN CAPITAL LETTER E WITH CIRCUMFLEX AND HOOK ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x003f, UNITIZE(TQt::Key_Dead_Circumflex), 0x004f, 0, 0}, 0x1ed4 }, // U1ED4 # LATIN CAPITAL LETTER O WITH CIRCUMFLEX AND HOOK ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x003f, UNITIZE(TQt::Key_Dead_Circumflex), 0x0061, 0, 0}, 0x1ea9 }, // U1EA9 # LATIN SMALL LETTER A WITH CIRCUMFLEX AND HOOK ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x003f, UNITIZE(TQt::Key_Dead_Circumflex), 0x0065, 0, 0}, 0x1ec3 }, // U1EC3 # LATIN SMALL LETTER E WITH CIRCUMFLEX AND HOOK ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x003f, UNITIZE(TQt::Key_Dead_Circumflex), 0x006f, 0, 0}, 0x1ed5 }, // U1ED5 # LATIN SMALL LETTER O WITH CIRCUMFLEX AND HOOK ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x003f, UNITIZE(TQt::Key_Dead_Breve), 0x0041, 0, 0}, 0x1eb2 }, // U1EB2 # LATIN CAPITAL LETTER A WITH BREVE AND HOOK ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x003f, UNITIZE(TQt::Key_Dead_Breve), 0x0061, 0, 0}, 0x1eb3 }, // U1EB3 # LATIN SMALL LETTER A WITH BREVE AND HOOK ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x003f, UNITIZE(TQt::Key_Dead_Horn), 0x004f, 0, 0}, 0x1ede }, // U1EDE # LATIN CAPITAL LETTER O WITH HORN AND HOOK ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x003f, UNITIZE(TQt::Key_Dead_Horn), 0x0055, 0, 0}, 0x1eec }, // U1EEC # LATIN CAPITAL LETTER U WITH HORN AND HOOK ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x003f, UNITIZE(TQt::Key_Dead_Horn), 0x006f, 0, 0}, 0x1edf }, // U1EDF # LATIN SMALL LETTER O WITH HORN AND HOOK ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x003f, UNITIZE(TQt::Key_Dead_Horn), 0x0075, 0, 0}, 0x1eed }, // U1EED # LATIN SMALL LETTER U WITH HORN AND HOOK ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0041, 0x0045, 0, 0, 0}, 0x00c6 }, // AE
    { {UNITIZE(TQt::Key_Multi_key), 0x0041, 0x0054, 0, 0, 0}, 0x0040 }, // at
    { {UNITIZE(TQt::Key_Multi_key), 0x0043, 0x002f, 0, 0, 0}, 0x20a1 }, // U20a1 # COLON SIGN
    { {UNITIZE(TQt::Key_Multi_key), 0x0043, 0x002f, 0, 0, 0}, 0x00a2 }, // U00A2 # CENT SIGN
    { {UNITIZE(TQt::Key_Multi_key), 0x0043, 0x003d, 0, 0, 0}, 0x20ac }, // EuroSign # EURO SIGN
    { {UNITIZE(TQt::Key_Multi_key), 0x0043, 0x0045, 0, 0, 0}, 0x20a0 }, // U20a0 # EURO-CURRENCY SIGN
    { {UNITIZE(TQt::Key_Multi_key), 0x0043, 0x0072, 0, 0, 0}, 0x20a2 }, // U20a2 # CRUZEIRO SIGN
    { {UNITIZE(TQt::Key_Multi_key), 0x0043, 0x007c, 0, 0, 0}, 0x00a2 }, // U00A2 # CENT SIGN
    { {UNITIZE(TQt::Key_Multi_key), 0x0044, 0x0048, 0, 0, 0}, 0x00d0 }, // U00D0 # LATIN CAPITAL LETTER ETH
    { {UNITIZE(TQt::Key_Multi_key), 0x0045, 0x003d, 0, 0, 0}, 0x20ac }, // EuroSign # EURO SIGN
    { {UNITIZE(TQt::Key_Multi_key), 0x0046, 0x0072, 0, 0, 0}, 0x20a3 }, // U20a3 # FRENCH FRANC SIGN
    { {UNITIZE(TQt::Key_Multi_key), 0x004c, 0x002d, 0, 0, 0}, 0x00a3 }, // U00a3 # POUND SIGN
    { {UNITIZE(TQt::Key_Multi_key), 0x004c, 0x003d, 0, 0, 0}, 0x20a4 }, // U20a4 # LIRA SIGN
    { {UNITIZE(TQt::Key_Multi_key), 0x004c, 0x0056, 0, 0, 0}, 0x007c }, // bar
    { {UNITIZE(TQt::Key_Multi_key), 0x004e, 0x003d, 0, 0, 0}, 0x20a6 }, // U20a6 # NAIRA SIGN
    { {UNITIZE(TQt::Key_Multi_key), 0x004e, 0x0047, 0, 0, 0}, 0x014a }, // U014A # LATIN CAPITAL LETTER ENG
    { {UNITIZE(TQt::Key_Multi_key), 0x004f, 0x0043, 0, 0, 0}, 0x00a9 }, // copyright
    { {UNITIZE(TQt::Key_Multi_key), 0x004f, 0x0045, 0, 0, 0}, 0x0152 }, // OE
    { {UNITIZE(TQt::Key_Multi_key), 0x004f, 0x0052, 0, 0, 0}, 0x00ae }, // registered
    { {UNITIZE(TQt::Key_Multi_key), 0x004f, 0x0063, 0, 0, 0}, 0x00a9 }, // copyright
    { {UNITIZE(TQt::Key_Multi_key), 0x004f, 0x0072, 0, 0, 0}, 0x00ae }, // registered
    { {UNITIZE(TQt::Key_Multi_key), 0x0050, 0x0021, 0, 0, 0}, 0x00b6 }, // paragraph
    { {UNITIZE(TQt::Key_Multi_key), 0x0050, 0x0050, 0, 0, 0}, 0x00b6 }, // paragraph # PILCROW SIGN (PARAGRAPH SIGN)
    { {UNITIZE(TQt::Key_Multi_key), 0x0050, 0x0074, 0, 0, 0}, 0x20a7 }, // U20a7 # PESETA SIGN
    { {UNITIZE(TQt::Key_Multi_key), 0x0052, 0x0073, 0, 0, 0}, 0x20a8 }, // U20a8 # RUPEE SIGN
    { {UNITIZE(TQt::Key_Multi_key), 0x0054, 0x0048, 0, 0, 0}, 0x00de }, // U00DE # LATIN CAPITAL LETTER THORN
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0055, 0x0000, 0x0041, 0, 0}, 0x1eb6 }, // U1EB6 # LATIN CAPITAL LETTER A WITH BREVE AND DOT BELOW
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0055, 0x0000, 0x0061, 0, 0}, 0x1eb7 }, // U1EB7 # LATIN SMALL LETTER A WITH BREVE AND DOT BELOW
    { {UNITIZE(TQt::Key_Multi_key), 0x0055, 0x0021, 0x0041, 0, 0}, 0x1eb6 }, // U1EB6 # LATIN CAPITAL LETTER A WITH BREVE AND DOT BELOW
    { {UNITIZE(TQt::Key_Multi_key), 0x0055, 0x0021, 0x0061, 0, 0}, 0x1eb7 }, // U1EB7 # LATIN SMALL LETTER A WITH BREVE AND DOT BELOW
    { {UNITIZE(TQt::Key_Multi_key), 0x0055, 0x002c, 0x0045, 0, 0}, 0x1e1c }, // U1E1C # LATIN CAPITAL LETTER E WITH CEDILLA AND BREVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0055, 0x002c, 0x0065, 0, 0}, 0x1e1d }, // U1E1D # LATIN SMALL LETTER E WITH CEDILLA AND BREVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0055, 0x0041, 0, 0, 0}, 0x0102 }, // U0102 # LATIN CAPITAL LETTER A WITH BREVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0055, 0x0045, 0, 0, 0}, 0x0114 }, // U0114 # LATIN CAPITAL LETTER E WITH BREVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0055, 0x0047, 0, 0, 0}, 0x011e }, // U011E # LATIN CAPITAL LETTER G WITH BREVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0055, 0x0049, 0, 0, 0}, 0x012c }, // U012C # LATIN CAPITAL LETTER I WITH BREVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0055, 0x004f, 0, 0, 0}, 0x014e }, // U014E # LATIN CAPITAL LETTER O WITH BREVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0055, 0x0055, 0, 0, 0}, 0x016c }, // U016C # LATIN CAPITAL LETTER U WITH BREVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0055, 0x0061, 0, 0, 0}, 0x0103 }, // U0103 # LATIN SMALL LETTER A WITH BREVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0055, 0x0065, 0, 0, 0}, 0x0115 }, // U0115 # LATIN SMALL LETTER E WITH BREVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0055, 0x0067, 0, 0, 0}, 0x011f }, // U011F # LATIN SMALL LETTER G WITH BREVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0055, 0x0069, 0, 0, 0}, 0x012d }, // U012D # LATIN SMALL LETTER I WITH BREVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0055, 0x006f, 0, 0, 0}, 0x014f }, // U014F # LATIN SMALL LETTER O WITH BREVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0055, 0x0075, 0, 0, 0}, 0x016d }, // U016D # LATIN SMALL LETTER U WITH BREVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0055, 0x0228, 0, 0, 0}, 0x1e1c }, // U1E1C # LATIN CAPITAL LETTER E WITH CEDILLA AND BREVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0055, 0x0229, 0, 0, 0}, 0x1e1d }, // U1E1D # LATIN SMALL LETTER E WITH CEDILLA AND BREVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0055, 0x0391, 0, 0, 0}, 0x1fb8 }, // U1FB8 # GREEK CAPITAL LETTER ALPHA WITH VRACHY
    { {UNITIZE(TQt::Key_Multi_key), 0x0055, 0x0399, 0, 0, 0}, 0x1fd8 }, // U1FD8 # GREEK CAPITAL LETTER IOTA WITH VRACHY
    { {UNITIZE(TQt::Key_Multi_key), 0x0055, 0x03a5, 0, 0, 0}, 0x1fe8 }, // U1FE8 # GREEK CAPITAL LETTER UPSILON WITH VRACHY
    { {UNITIZE(TQt::Key_Multi_key), 0x0055, 0x03b1, 0, 0, 0}, 0x1fb0 }, // U1FB0 # GREEK SMALL LETTER ALPHA WITH VRACHY
    { {UNITIZE(TQt::Key_Multi_key), 0x0055, 0x03b9, 0, 0, 0}, 0x1fd0 }, // U1FD0 # GREEK SMALL LETTER IOTA WITH VRACHY
    { {UNITIZE(TQt::Key_Multi_key), 0x0055, 0x03c5, 0, 0, 0}, 0x1fe0 }, // U1FE0 # GREEK SMALL LETTER UPSILON WITH VRACHY
    { {UNITIZE(TQt::Key_Multi_key), 0x0055, 0x0410, 0, 0, 0}, 0x04d0 }, // U04D0 # CYRILLIC CAPITAL LETTER A WITH BREVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0055, 0x0415, 0, 0, 0}, 0x04d6 }, // U04D6 # CYRILLIC CAPITAL LETTER IE WITH BREVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0055, 0x0416, 0, 0, 0}, 0x04c1 }, // U04C1 # CYRILLIC CAPITAL LETTER ZHE WITH BREVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0055, 0x0418, 0, 0, 0}, 0x0419 }, // U0419 # CYRILLIC CAPITAL LETTER SHORT I
    { {UNITIZE(TQt::Key_Multi_key), 0x0055, 0x0423, 0, 0, 0}, 0x040e }, // U040E # CYRILLIC CAPITAL LETTER SHORT U
    { {UNITIZE(TQt::Key_Multi_key), 0x0055, 0x0430, 0, 0, 0}, 0x04d1 }, // U04D1 # CYRILLIC SMALL LETTER A WITH BREVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0055, 0x0435, 0, 0, 0}, 0x04d7 }, // U04D7 # CYRILLIC SMALL LETTER IE WITH BREVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0055, 0x0436, 0, 0, 0}, 0x04c2 }, // U04C2 # CYRILLIC SMALL LETTER ZHE WITH BREVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0055, 0x0438, 0, 0, 0}, 0x0439 }, // U0439 # CYRILLIC SMALL LETTER SHORT I
    { {UNITIZE(TQt::Key_Multi_key), 0x0055, 0x0443, 0, 0, 0}, 0x045e }, // U045E # CYRILLIC SMALL LETTER SHORT U
    { {UNITIZE(TQt::Key_Multi_key), 0x0055, 0x1ea0, 0, 0, 0}, 0x1eb6 }, // U1EB6 # LATIN CAPITAL LETTER A WITH BREVE AND DOT BELOW
    { {UNITIZE(TQt::Key_Multi_key), 0x0055, 0x1ea1, 0, 0, 0}, 0x1eb7 }, // U1EB7 # LATIN SMALL LETTER A WITH BREVE AND DOT BELOW
    { {UNITIZE(TQt::Key_Multi_key), 0x0055, UNITIZE(TQt::Key_Dead_Cedilla), 0x0045, 0, 0}, 0x1e1c }, // U1E1C # LATIN CAPITAL LETTER E WITH CEDILLA AND BREVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0055, UNITIZE(TQt::Key_Dead_Cedilla), 0x0065, 0, 0}, 0x1e1d }, // U1E1D # LATIN SMALL LETTER E WITH CEDILLA AND BREVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0055, UNITIZE(TQt::Key_Dead_Belowdot), 0x0041, 0, 0}, 0x1eb6 }, // U1EB6 # LATIN CAPITAL LETTER A WITH BREVE AND DOT BELOW
    { {UNITIZE(TQt::Key_Multi_key), 0x0055, UNITIZE(TQt::Key_Dead_Belowdot), 0x0061, 0, 0}, 0x1eb7 }, // U1EB7 # LATIN SMALL LETTER A WITH BREVE AND DOT BELOW
    { {UNITIZE(TQt::Key_Multi_key), 0x0056, 0x004c, 0, 0, 0}, 0x007c }, // bar
    { {UNITIZE(TQt::Key_Multi_key), 0x0057, 0x003d, 0, 0, 0}, 0x20a9 }, // U20a9 # WON SIGN
    { {UNITIZE(TQt::Key_Multi_key), 0x0059, 0x003d, 0, 0, 0}, 0x00a5 }, // yen
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x0000, 0, 0, 0}, 0x00b2 }, // U00B2 # SUPERSCRIPT TWO
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x0000, 0, 0, 0}, 0x2076 }, // U2076 # SUPERSCRIPT SIX
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x0000, 0, 0, 0}, 0x2070 }, // U2070 # SUPERSCRIPT ZERO
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x0000, 0, 0, 0}, 0x00b9 }, // U00B9 # SUPERSCRIPT ONE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x0000, 0, 0, 0}, 0x2078 }, // U2078 # SUPERSCRIPT EIGHT
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x0000, 0, 0, 0}, 0x2074 }, // U2074 # SUPERSCRIPT FOUR
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x0000, 0, 0, 0}, 0x00b3 }, // U00B3 # SUPERSCRIPT THREE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x0000, 0, 0, 0}, 0x2077 }, // U2077 # SUPERSCRIPT SEVEN
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x0000, 0, 0, 0}, 0x207c }, // U207C # SUPERSCRIPT EQUALS SIGN
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x0000, 0, 0, 0}, 0x207a }, // U207A # SUPERSCRIPT PLUS SIGN
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x0000, 0, 0, 0}, 0x00b2 }, // U00B2 # SUPERSCRIPT TWO
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x0000, 0, 0, 0}, 0x2079 }, // U2079 # SUPERSCRIPT NINE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x0000, 0, 0, 0}, 0x2075 }, // U2075 # SUPERSCRIPT FIVE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x0000, 0x0041, 0, 0}, 0x1eac }, // U1EAC # LATIN CAPITAL LETTER A WITH CIRCUMFLEX AND DOT BELOW
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x0000, 0x0045, 0, 0}, 0x1ec6 }, // U1EC6 # LATIN CAPITAL LETTER E WITH CIRCUMFLEX AND DOT BELOW
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x0000, 0x004f, 0, 0}, 0x1ed8 }, // U1ED8 # LATIN CAPITAL LETTER O WITH CIRCUMFLEX AND DOT BELOW
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x0000, 0x0061, 0, 0}, 0x1ead }, // U1EAD # LATIN SMALL LETTER A WITH CIRCUMFLEX AND DOT BELOW
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x0000, 0x0065, 0, 0}, 0x1ec7 }, // U1EC7 # LATIN SMALL LETTER E WITH CIRCUMFLEX AND DOT BELOW
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x0000, 0x006f, 0, 0}, 0x1ed9 }, // U1ED9 # LATIN SMALL LETTER O WITH CIRCUMFLEX AND DOT BELOW
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x0020, 0, 0, 0}, 0x005e }, // asciicircum
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x0021, 0x0041, 0, 0}, 0x1eac }, // U1EAC # LATIN CAPITAL LETTER A WITH CIRCUMFLEX AND DOT BELOW
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x0021, 0x0045, 0, 0}, 0x1ec6 }, // U1EC6 # LATIN CAPITAL LETTER E WITH CIRCUMFLEX AND DOT BELOW
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x0021, 0x004f, 0, 0}, 0x1ed8 }, // U1ED8 # LATIN CAPITAL LETTER O WITH CIRCUMFLEX AND DOT BELOW
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x0021, 0x0061, 0, 0}, 0x1ead }, // U1EAD # LATIN SMALL LETTER A WITH CIRCUMFLEX AND DOT BELOW
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x0021, 0x0065, 0, 0}, 0x1ec7 }, // U1EC7 # LATIN SMALL LETTER E WITH CIRCUMFLEX AND DOT BELOW
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x0021, 0x006f, 0, 0}, 0x1ed9 }, // U1ED9 # LATIN SMALL LETTER O WITH CIRCUMFLEX AND DOT BELOW
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x0028, 0, 0, 0}, 0x207d }, // U207D # SUPERSCRIPT LEFT PARENTHESIS
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x0029, 0, 0, 0}, 0x207e }, // U207E # SUPERSCRIPT RIGHT PARENTHESIS
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x002b, 0, 0, 0}, 0x207a }, // U207A # SUPERSCRIPT PLUS SIGN
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x002f, 0, 0, 0}, 0x007c }, // bar
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x0030, 0, 0, 0}, 0x2070 }, // U2070 # SUPERSCRIPT ZERO
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x0031, 0, 0, 0}, 0x00b9 }, // U00B9 # SUPERSCRIPT ONE
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x0032, 0, 0, 0}, 0x00b2 }, // U00B2 # SUPERSCRIPT TWO
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x0033, 0, 0, 0}, 0x00b3 }, // U00B3 # SUPERSCRIPT THREE
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x0034, 0, 0, 0}, 0x2074 }, // U2074 # SUPERSCRIPT FOUR
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x0035, 0, 0, 0}, 0x2075 }, // U2075 # SUPERSCRIPT FIVE
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x0036, 0, 0, 0}, 0x2076 }, // U2076 # SUPERSCRIPT SIX
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x0037, 0, 0, 0}, 0x2077 }, // U2077 # SUPERSCRIPT SEVEN
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x0038, 0, 0, 0}, 0x2078 }, // U2078 # SUPERSCRIPT EIGHT
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x0039, 0, 0, 0}, 0x2079 }, // U2079 # SUPERSCRIPT NINE
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x003d, 0, 0, 0}, 0x207c }, // U207C # SUPERSCRIPT EQUALS SIGN
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x0041, 0, 0, 0}, 0x00c2 }, // U00C2 # LATIN CAPITAL LETTER A WITH CIRCUMFLEX
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x0043, 0, 0, 0}, 0x0108 }, // U0108 # LATIN CAPITAL LETTER C WITH CIRCUMFLEX
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x0045, 0, 0, 0}, 0x00ca }, // U00CA # LATIN CAPITAL LETTER E WITH CIRCUMFLEX
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x0047, 0, 0, 0}, 0x011c }, // U011C # LATIN CAPITAL LETTER G WITH CIRCUMFLEX
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x0048, 0, 0, 0}, 0x0124 }, // U0124 # LATIN CAPITAL LETTER H WITH CIRCUMFLEX
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x0049, 0, 0, 0}, 0x00ce }, // U00CE # LATIN CAPITAL LETTER I WITH CIRCUMFLEX
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x004a, 0, 0, 0}, 0x0134 }, // U0134 # LATIN CAPITAL LETTER J WITH CIRCUMFLEX
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x004f, 0, 0, 0}, 0x00d4 }, // U00D4 # LATIN CAPITAL LETTER O WITH CIRCUMFLEX
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x0053, 0, 0, 0}, 0x015c }, // U015C # LATIN CAPITAL LETTER S WITH CIRCUMFLEX
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x0053, 0x004d, 0, 0}, 0x2120 }, // U2120 # SERVICE MARK
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x0054, 0x004d, 0, 0}, 0x2122 }, // U2122 # TRADE MARK SIGN
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x0055, 0, 0, 0}, 0x00db }, // U00DB # LATIN CAPITAL LETTER U WITH CIRCUMFLEX
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x0057, 0, 0, 0}, 0x0174 }, // U0174 # LATIN CAPITAL LETTER W WITH CIRCUMFLEX
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x0059, 0, 0, 0}, 0x0176 }, // U0176 # LATIN CAPITAL LETTER Y WITH CIRCUMFLEX
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x005a, 0, 0, 0}, 0x1e90 }, // U1E90 # LATIN CAPITAL LETTER Z WITH CIRCUMFLEX
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x005f, 0x0061, 0, 0}, 0x00aa }, // U00AA # FEMININE ORDINAL INDICATOR
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x005f, 0x0061, 0, 0}, 0x00aa }, // U00AA # FEMININE ORDINAL INDICATOR
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x005f, 0x0068, 0, 0}, 0x02b0 }, // U02B0 # MODIFIER LETTER SMALL H
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x005f, 0x0068, 0, 0}, 0x02b0 }, // U02B0 # MODIFIER LETTER SMALL H
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x005f, 0x0069, 0, 0}, 0x2071 }, // U2071 # SUPERSCRIPT LATIN SMALL LETTER I
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x005f, 0x0069, 0, 0}, 0x2071 }, // U2071 # SUPERSCRIPT LATIN SMALL LETTER I
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x005f, 0x006a, 0, 0}, 0x02b2 }, // U02B2 # MODIFIER LETTER SMALL J
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x005f, 0x006a, 0, 0}, 0x02b2 }, // U02B2 # MODIFIER LETTER SMALL J
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x005f, 0x006c, 0, 0}, 0x02e1 }, // U02E1 # MODIFIER LETTER SMALL L
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x005f, 0x006c, 0, 0}, 0x02e1 }, // U02E1 # MODIFIER LETTER SMALL L
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x005f, 0x006e, 0, 0}, 0x207f }, // U207F # SUPERSCRIPT LATIN SMALL LETTER N
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x005f, 0x006e, 0, 0}, 0x207f }, // U207F # SUPERSCRIPT LATIN SMALL LETTER N
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x005f, 0x006f, 0, 0}, 0x00ba }, // U00BA # MASCULINE ORDINAL INDICATOR
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x005f, 0x006f, 0, 0}, 0x00ba }, // U00BA # MASCULINE ORDINAL INDICATOR
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x005f, 0x0072, 0, 0}, 0x02b3 }, // U02B3 # MODIFIER LETTER SMALL R
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x005f, 0x0072, 0, 0}, 0x02b3 }, // U02B3 # MODIFIER LETTER SMALL R
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x005f, 0x0073, 0, 0}, 0x02e2 }, // U02E2 # MODIFIER LETTER SMALL S
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x005f, 0x0073, 0, 0}, 0x02e2 }, // U02E2 # MODIFIER LETTER SMALL S
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x005f, 0x0077, 0, 0}, 0x02b7 }, // U02B7 # MODIFIER LETTER SMALL W
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x005f, 0x0077, 0, 0}, 0x02b7 }, // U02B7 # MODIFIER LETTER SMALL W
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x005f, 0x0078, 0, 0}, 0x02e3 }, // U02E3 # MODIFIER LETTER SMALL X
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x005f, 0x0078, 0, 0}, 0x02e3 }, // U02E3 # MODIFIER LETTER SMALL X
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x005f, 0x0079, 0, 0}, 0x02b8 }, // U02B8 # MODIFIER LETTER SMALL Y
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x005f, 0x0079, 0, 0}, 0x02b8 }, // U02B8 # MODIFIER LETTER SMALL Y
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x005f, 0x0263, 0, 0}, 0x02e0 }, // U02E0 # MODIFIER LETTER SMALL GAMMA
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x005f, 0x0263, 0, 0}, 0x02e0 }, // U02E0 # MODIFIER LETTER SMALL GAMMA
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x005f, 0x0266, 0, 0}, 0x02b1 }, // U02B1 # MODIFIER LETTER SMALL H WITH HOOK
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x005f, 0x0266, 0, 0}, 0x02b1 }, // U02B1 # MODIFIER LETTER SMALL H WITH HOOK
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x005f, 0x0279, 0, 0}, 0x02b4 }, // U02B4 # MODIFIER LETTER SMALL TURNED R
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x005f, 0x0279, 0, 0}, 0x02b4 }, // U02B4 # MODIFIER LETTER SMALL TURNED R
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x005f, 0x027b, 0, 0}, 0x02b5 }, // U02B5 # MODIFIER LETTER SMALL TURNED R WITH HOOK
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x005f, 0x027b, 0, 0}, 0x02b5 }, // U02B5 # MODIFIER LETTER SMALL TURNED R WITH HOOK
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x005f, 0x0281, 0, 0}, 0x02b6 }, // U02B6 # MODIFIER LETTER SMALL CAPITAL INVERTED R
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x005f, 0x0281, 0, 0}, 0x02b6 }, // U02B6 # MODIFIER LETTER SMALL CAPITAL INVERTED R
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x005f, 0x0295, 0, 0}, 0x02e4 }, // U02E4 # MODIFIER LETTER SMALL REVERSED GLOTTAL STOP
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x005f, 0x0295, 0, 0}, 0x02e4 }, // U02E4 # MODIFIER LETTER SMALL REVERSED GLOTTAL STOP
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x0061, 0, 0, 0}, 0x00e2 }, // U00E2 # LATIN SMALL LETTER A WITH CIRCUMFLEX
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x0063, 0, 0, 0}, 0x0109 }, // U0109 # LATIN SMALL LETTER C WITH CIRCUMFLEX
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x0065, 0, 0, 0}, 0x00ea }, // U00EA # LATIN SMALL LETTER E WITH CIRCUMFLEX
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x0067, 0, 0, 0}, 0x011d }, // U011D # LATIN SMALL LETTER G WITH CIRCUMFLEX
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x0068, 0, 0, 0}, 0x0125 }, // U0125 # LATIN SMALL LETTER H WITH CIRCUMFLEX
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x0069, 0, 0, 0}, 0x00ee }, // U00EE # LATIN SMALL LETTER I WITH CIRCUMFLEX
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x006a, 0, 0, 0}, 0x0135 }, // U0135 # LATIN SMALL LETTER J WITH CIRCUMFLEX
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x006f, 0, 0, 0}, 0x00f4 }, // U00F4 # LATIN SMALL LETTER O WITH CIRCUMFLEX
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x0073, 0, 0, 0}, 0x015d }, // U015D # LATIN SMALL LETTER S WITH CIRCUMFLEX
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x0075, 0, 0, 0}, 0x00fb }, // U00FB # LATIN SMALL LETTER U WITH CIRCUMFLEX
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x0077, 0, 0, 0}, 0x0175 }, // U0175 # LATIN SMALL LETTER W WITH CIRCUMFLEX
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x0079, 0, 0, 0}, 0x0177 }, // U0177 # LATIN SMALL LETTER Y WITH CIRCUMFLEX
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x007a, 0, 0, 0}, 0x1e91 }, // U1E91 # LATIN SMALL LETTER Z WITH CIRCUMFLEX
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x1ea0, 0, 0, 0}, 0x1eac }, // U1EAC # LATIN CAPITAL LETTER A WITH CIRCUMFLEX AND DOT BELOW
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x1ea1, 0, 0, 0}, 0x1ead }, // U1EAD # LATIN SMALL LETTER A WITH CIRCUMFLEX AND DOT BELOW
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x1eb8, 0, 0, 0}, 0x1ec6 }, // U1EC6 # LATIN CAPITAL LETTER E WITH CIRCUMFLEX AND DOT BELOW
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x1eb9, 0, 0, 0}, 0x1ec7 }, // U1EC7 # LATIN SMALL LETTER E WITH CIRCUMFLEX AND DOT BELOW
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x1ecc, 0, 0, 0}, 0x1ed8 }, // U1ED8 # LATIN CAPITAL LETTER O WITH CIRCUMFLEX AND DOT BELOW
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x1ecd, 0, 0, 0}, 0x1ed9 }, // U1ED9 # LATIN SMALL LETTER O WITH CIRCUMFLEX AND DOT BELOW
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x2212, 0, 0, 0}, 0x207b }, // U207B # SUPERSCRIPT MINUS
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x4e00, 0, 0, 0}, 0x3192 }, // U3192 # IDEOGRAPHIC ANNOTATION ONE MARK
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x4e01, 0, 0, 0}, 0x319c }, // U319C # IDEOGRAPHIC ANNOTATION FOURTH MARK
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x4e09, 0, 0, 0}, 0x3194 }, // U3194 # IDEOGRAPHIC ANNOTATION THREE MARK
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x4e0a, 0, 0, 0}, 0x3196 }, // U3196 # IDEOGRAPHIC ANNOTATION TOP MARK
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x4e0b, 0, 0, 0}, 0x3198 }, // U3198 # IDEOGRAPHIC ANNOTATION BOTTOM MARK
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x4e19, 0, 0, 0}, 0x319b }, // U319B # IDEOGRAPHIC ANNOTATION THIRD MARK
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x4e2d, 0, 0, 0}, 0x3197 }, // U3197 # IDEOGRAPHIC ANNOTATION MIDDLE MARK
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x4e59, 0, 0, 0}, 0x319a }, // U319A # IDEOGRAPHIC ANNOTATION SECOND MARK
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x4e8c, 0, 0, 0}, 0x3193 }, // U3193 # IDEOGRAPHIC ANNOTATION TWO MARK
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x4eba, 0, 0, 0}, 0x319f }, // U319F # IDEOGRAPHIC ANNOTATION MAN MARK
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x56db, 0, 0, 0}, 0x3195 }, // U3195 # IDEOGRAPHIC ANNOTATION FOUR MARK
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x5730, 0, 0, 0}, 0x319e }, // U319E # IDEOGRAPHIC ANNOTATION EARTH MARK
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x5929, 0, 0, 0}, 0x319d }, // U319D # IDEOGRAPHIC ANNOTATION HEAVEN MARK
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, 0x7532, 0, 0, 0}, 0x3199 }, // U3199 # IDEOGRAPHIC ANNOTATION FIRST MARK
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, UNITIZE(TQt::Key_Dead_Belowdot), 0x0041, 0, 0}, 0x1eac }, // U1EAC # LATIN CAPITAL LETTER A WITH CIRCUMFLEX AND DOT BELOW
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, UNITIZE(TQt::Key_Dead_Belowdot), 0x0045, 0, 0}, 0x1ec6 }, // U1EC6 # LATIN CAPITAL LETTER E WITH CIRCUMFLEX AND DOT BELOW
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, UNITIZE(TQt::Key_Dead_Belowdot), 0x004f, 0, 0}, 0x1ed8 }, // U1ED8 # LATIN CAPITAL LETTER O WITH CIRCUMFLEX AND DOT BELOW
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, UNITIZE(TQt::Key_Dead_Belowdot), 0x0061, 0, 0}, 0x1ead }, // U1EAD # LATIN SMALL LETTER A WITH CIRCUMFLEX AND DOT BELOW
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, UNITIZE(TQt::Key_Dead_Belowdot), 0x0065, 0, 0}, 0x1ec7 }, // U1EC7 # LATIN SMALL LETTER E WITH CIRCUMFLEX AND DOT BELOW
    { {UNITIZE(TQt::Key_Multi_key), 0x005e, UNITIZE(TQt::Key_Dead_Belowdot), 0x006f, 0, 0}, 0x1ed9 }, // U1ED9 # LATIN SMALL LETTER O WITH CIRCUMFLEX AND DOT BELOW
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0000, 0, 0, 0}, 0x2085 }, // U2085 # SUBSCRIPT FIVE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0000, 0, 0, 0}, 0x2087 }, // U2087 # SUBSCRIPT SEVEN
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0000, 0, 0, 0}, 0x2082 }, // U2082 # SUBSCRIPT TWO
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0000, 0, 0, 0}, 0x2085 }, // U2085 # SUBSCRIPT FIVE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0000, 0, 0, 0}, 0x2084 }, // U2084 # SUBSCRIPT FOUR
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0000, 0, 0, 0}, 0x2084 }, // U2084 # SUBSCRIPT FOUR
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0000, 0, 0, 0}, 0x2086 }, // U2086 # SUBSCRIPT SIX
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0000, 0, 0, 0}, 0x2081 }, // U2081 # SUBSCRIPT ONE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0000, 0, 0, 0}, 0x208a }, // U208A # SUBSCRIPT PLUS SIGN
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0000, 0, 0, 0}, 0x2088 }, // U2088 # SUBSCRIPT EIGHT
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0000, 0, 0, 0}, 0x2087 }, // U2087 # SUBSCRIPT SEVEN
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0000, 0, 0, 0}, 0x208a }, // U208A # SUBSCRIPT PLUS SIGN
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0000, 0, 0, 0}, 0x2080 }, // U2080 # SUBSCRIPT ZERO
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0000, 0, 0, 0}, 0x2083 }, // U2083 # SUBSCRIPT THREE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0000, 0, 0, 0}, 0x2082 }, // U2082 # SUBSCRIPT TWO
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0000, 0, 0, 0}, 0x2083 }, // U2083 # SUBSCRIPT THREE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0000, 0, 0, 0}, 0x2086 }, // U2086 # SUBSCRIPT SIX
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0000, 0, 0, 0}, 0x2088 }, // U2088 # SUBSCRIPT EIGHT
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0000, 0, 0, 0}, 0x2089 }, // U2089 # SUBSCRIPT NINE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0000, 0, 0, 0}, 0x2089 }, // U2089 # SUBSCRIPT NINE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0000, 0, 0, 0}, 0x2080 }, // U2080 # SUBSCRIPT ZERO
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0000, 0, 0, 0}, 0x2082 }, // U2082 # SUBSCRIPT TWO
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0000, 0, 0, 0}, 0x2081 }, // U2081 # SUBSCRIPT ONE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0000, 0, 0, 0}, 0x2082 }, // U2082 # SUBSCRIPT TWO
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0000, 0, 0, 0}, 0x208c }, // U208C # SUBSCRIPT EQUALS SIGN
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0000, 0, 0, 0}, 0x208c }, // U208C # SUBSCRIPT EQUALS SIGN
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0000, 0x004c, 0, 0}, 0x1e38 }, // U1E38 # LATIN CAPITAL LETTER L WITH DOT BELOW AND MACRON
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0000, 0x004f, 0, 0}, 0x022c }, // U022C # LATIN CAPITAL LETTER O WITH TILDE AND MACRON
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0000, 0x0052, 0, 0}, 0x1e5c }, // U1E5C # LATIN CAPITAL LETTER R WITH DOT BELOW AND MACRON
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0000, 0x006c, 0, 0}, 0x1e39 }, // U1E39 # LATIN SMALL LETTER L WITH DOT BELOW AND MACRON
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0000, 0x006f, 0, 0}, 0x022d }, // U022D # LATIN SMALL LETTER O WITH TILDE AND MACRON
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0000, 0x0072, 0, 0}, 0x1e5d }, // U1E5D # LATIN SMALL LETTER R WITH DOT BELOW AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0021, 0x004c, 0, 0}, 0x1e38 }, // U1E38 # LATIN CAPITAL LETTER L WITH DOT BELOW AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0021, 0x0052, 0, 0}, 0x1e5c }, // U1E5C # LATIN CAPITAL LETTER R WITH DOT BELOW AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0021, 0x006c, 0, 0}, 0x1e39 }, // U1E39 # LATIN SMALL LETTER L WITH DOT BELOW AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0021, 0x0072, 0, 0}, 0x1e5d }, // U1E5D # LATIN SMALL LETTER R WITH DOT BELOW AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0022, 0x0041, 0, 0}, 0x01de }, // U01DE # LATIN CAPITAL LETTER A WITH DIAERESIS AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0022, 0x004f, 0, 0}, 0x022a }, // U022A # LATIN CAPITAL LETTER O WITH DIAERESIS AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0022, 0x0055, 0, 0}, 0x01d5 }, // U01D5 # LATIN CAPITAL LETTER U WITH DIAERESIS AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0022, 0x0061, 0, 0}, 0x01df }, // U01DF # LATIN SMALL LETTER A WITH DIAERESIS AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0022, 0x006f, 0, 0}, 0x022b }, // U022B # LATIN SMALL LETTER O WITH DIAERESIS AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0022, 0x0075, 0, 0}, 0x01d6 }, // U01D6 # LATIN SMALL LETTER U WITH DIAERESIS AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0028, 0, 0, 0}, 0x208d }, // U208D # SUBSCRIPT LEFT PARENTHESIS
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0028, 0, 0, 0}, 0x208d }, // U208D # SUBSCRIPT LEFT PARENTHESIS
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0029, 0, 0, 0}, 0x208e }, // U208E # SUBSCRIPT RIGHT PARENTHESIS
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0029, 0, 0, 0}, 0x208e }, // U208E # SUBSCRIPT RIGHT PARENTHESIS
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x002b, 0, 0, 0}, 0x208a }, // U208A # SUBSCRIPT PLUS SIGN
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x002b, 0, 0, 0}, 0x208a }, // U208A # SUBSCRIPT PLUS SIGN
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x002e, 0x0041, 0, 0}, 0x01e0 }, // U01E0 # LATIN CAPITAL LETTER A WITH DOT ABOVE AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x002e, 0x004f, 0, 0}, 0x0230 }, // U0230 # LATIN CAPITAL LETTER O WITH DOT ABOVE AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x002e, 0x0061, 0, 0}, 0x01e1 }, // U01E1 # LATIN SMALL LETTER A WITH DOT ABOVE AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x002e, 0x006f, 0, 0}, 0x0231 }, // U0231 # LATIN SMALL LETTER O WITH DOT ABOVE AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0030, 0, 0, 0}, 0x2080 }, // U2080 # SUBSCRIPT ZERO
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0030, 0, 0, 0}, 0x2080 }, // U2080 # SUBSCRIPT ZERO
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0031, 0, 0, 0}, 0x2081 }, // U2081 # SUBSCRIPT ONE
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0031, 0, 0, 0}, 0x2081 }, // U2081 # SUBSCRIPT ONE
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0032, 0, 0, 0}, 0x2082 }, // U2082 # SUBSCRIPT TWO
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0032, 0, 0, 0}, 0x2082 }, // U2082 # SUBSCRIPT TWO
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0033, 0, 0, 0}, 0x2083 }, // U2083 # SUBSCRIPT THREE
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0033, 0, 0, 0}, 0x2083 }, // U2083 # SUBSCRIPT THREE
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0034, 0, 0, 0}, 0x2084 }, // U2084 # SUBSCRIPT FOUR
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0034, 0, 0, 0}, 0x2084 }, // U2084 # SUBSCRIPT FOUR
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0035, 0, 0, 0}, 0x2085 }, // U2085 # SUBSCRIPT FIVE
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0035, 0, 0, 0}, 0x2085 }, // U2085 # SUBSCRIPT FIVE
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0036, 0, 0, 0}, 0x2086 }, // U2086 # SUBSCRIPT SIX
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0036, 0, 0, 0}, 0x2086 }, // U2086 # SUBSCRIPT SIX
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0037, 0, 0, 0}, 0x2087 }, // U2087 # SUBSCRIPT SEVEN
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0037, 0, 0, 0}, 0x2087 }, // U2087 # SUBSCRIPT SEVEN
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0038, 0, 0, 0}, 0x2088 }, // U2088 # SUBSCRIPT EIGHT
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0038, 0, 0, 0}, 0x2088 }, // U2088 # SUBSCRIPT EIGHT
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0039, 0, 0, 0}, 0x2089 }, // U2089 # SUBSCRIPT NINE
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0039, 0, 0, 0}, 0x2089 }, // U2089 # SUBSCRIPT NINE
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x003b, 0x004f, 0, 0}, 0x01ec }, // U01EC # LATIN CAPITAL LETTER O WITH OGONEK AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x003b, 0x006f, 0, 0}, 0x01ed }, // U01ED # LATIN SMALL LETTER O WITH OGONEK AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x003d, 0, 0, 0}, 0x208c }, // U208C # SUBSCRIPT EQUALS SIGN
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x003d, 0, 0, 0}, 0x208c }, // U208C # SUBSCRIPT EQUALS SIGN
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0041, 0, 0, 0}, 0x0100 }, // U0100 # LATIN CAPITAL LETTER A WITH MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0045, 0, 0, 0}, 0x0112 }, // U0112 # LATIN CAPITAL LETTER E WITH MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0047, 0, 0, 0}, 0x1e20 }, // U1E20 # LATIN CAPITAL LETTER G WITH MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0049, 0, 0, 0}, 0x012a }, // U012A # LATIN CAPITAL LETTER I WITH MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x004f, 0, 0, 0}, 0x014c }, // U014C # LATIN CAPITAL LETTER O WITH MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0055, 0, 0, 0}, 0x016a }, // U016A # LATIN CAPITAL LETTER U WITH MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0059, 0, 0, 0}, 0x0232 }, // U0232 # LATIN CAPITAL LETTER Y WITH MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0061, 0, 0, 0}, 0x0101 }, // U0101 # LATIN SMALL LETTER A WITH MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0065, 0, 0, 0}, 0x0113 }, // U0113 # LATIN SMALL LETTER E WITH MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0067, 0, 0, 0}, 0x1e21 }, // U1E21 # LATIN SMALL LETTER G WITH MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0069, 0, 0, 0}, 0x012b }, // U012B # LATIN SMALL LETTER I WITH MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x006f, 0, 0, 0}, 0x014d }, // U014D # LATIN SMALL LETTER O WITH MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0075, 0, 0, 0}, 0x016b }, // U016B # LATIN SMALL LETTER U WITH MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0079, 0, 0, 0}, 0x0233 }, // U0233 # LATIN SMALL LETTER Y WITH MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x007e, 0x004f, 0, 0}, 0x022c }, // U022C # LATIN CAPITAL LETTER O WITH TILDE AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x007e, 0x006f, 0, 0}, 0x022d }, // U022D # LATIN SMALL LETTER O WITH TILDE AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x00c4, 0, 0, 0}, 0x01de }, // U01DE # LATIN CAPITAL LETTER A WITH DIAERESIS AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x00c6, 0, 0, 0}, 0x01e2 }, // U01E2 # LATIN CAPITAL LETTER AE WITH MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x00d5, 0, 0, 0}, 0x022c }, // U022C # LATIN CAPITAL LETTER O WITH TILDE AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x00d6, 0, 0, 0}, 0x022a }, // U022A # LATIN CAPITAL LETTER O WITH DIAERESIS AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x00dc, 0, 0, 0}, 0x01d5 }, // U01D5 # LATIN CAPITAL LETTER U WITH DIAERESIS AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x00e4, 0, 0, 0}, 0x01df }, // U01DF # LATIN SMALL LETTER A WITH DIAERESIS AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x00e6, 0, 0, 0}, 0x01e3 }, // U01E3 # LATIN SMALL LETTER AE WITH MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x00f5, 0, 0, 0}, 0x022d }, // U022D # LATIN SMALL LETTER O WITH TILDE AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x00f6, 0, 0, 0}, 0x022b }, // U022B # LATIN SMALL LETTER O WITH DIAERESIS AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x00fc, 0, 0, 0}, 0x01d6 }, // U01D6 # LATIN SMALL LETTER U WITH DIAERESIS AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x01ea, 0, 0, 0}, 0x01ec }, // U01EC # LATIN CAPITAL LETTER O WITH OGONEK AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x01eb, 0, 0, 0}, 0x01ed }, // U01ED # LATIN SMALL LETTER O WITH OGONEK AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0226, 0, 0, 0}, 0x01e0 }, // U01E0 # LATIN CAPITAL LETTER A WITH DOT ABOVE AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0227, 0, 0, 0}, 0x01e1 }, // U01E1 # LATIN SMALL LETTER A WITH DOT ABOVE AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x022e, 0, 0, 0}, 0x0230 }, // U0230 # LATIN CAPITAL LETTER O WITH DOT ABOVE AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x022f, 0, 0, 0}, 0x0231 }, // U0231 # LATIN SMALL LETTER O WITH DOT ABOVE AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0391, 0, 0, 0}, 0x1fb9 }, // U1FB9 # GREEK CAPITAL LETTER ALPHA WITH MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0399, 0, 0, 0}, 0x1fd9 }, // U1FD9 # GREEK CAPITAL LETTER IOTA WITH MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x03a5, 0, 0, 0}, 0x1fe9 }, // U1FE9 # GREEK CAPITAL LETTER UPSILON WITH MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x03b1, 0, 0, 0}, 0x1fb1 }, // U1FB1 # GREEK SMALL LETTER ALPHA WITH MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x03b9, 0, 0, 0}, 0x1fd1 }, // U1FD1 # GREEK SMALL LETTER IOTA WITH MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x03c5, 0, 0, 0}, 0x1fe1 }, // U1FE1 # GREEK SMALL LETTER UPSILON WITH MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0418, 0, 0, 0}, 0x04e2 }, // U04E2 # CYRILLIC CAPITAL LETTER I WITH MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0423, 0, 0, 0}, 0x04ee }, // U04EE # CYRILLIC CAPITAL LETTER U WITH MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0438, 0, 0, 0}, 0x04e3 }, // U04E3 # CYRILLIC SMALL LETTER I WITH MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0443, 0, 0, 0}, 0x04ef }, // U04EF # CYRILLIC SMALL LETTER U WITH MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x1e36, 0, 0, 0}, 0x1e38 }, // U1E38 # LATIN CAPITAL LETTER L WITH DOT BELOW AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x1e37, 0, 0, 0}, 0x1e39 }, // U1E39 # LATIN SMALL LETTER L WITH DOT BELOW AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x1e5a, 0, 0, 0}, 0x1e5c }, // U1E5C # LATIN CAPITAL LETTER R WITH DOT BELOW AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x1e5b, 0, 0, 0}, 0x1e5d }, // U1E5D # LATIN SMALL LETTER R WITH DOT BELOW AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x2212, 0, 0, 0}, 0x208b }, // U208B # SUBSCRIPT MINUS
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, 0x2212, 0, 0, 0}, 0x208b }, // U208B # SUBSCRIPT MINUS
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, UNITIZE(TQt::Key_Dead_Tilde), 0x004f, 0, 0}, 0x022c }, // U022C # LATIN CAPITAL LETTER O WITH TILDE AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, UNITIZE(TQt::Key_Dead_Tilde), 0x006f, 0, 0}, 0x022d }, // U022D # LATIN SMALL LETTER O WITH TILDE AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, UNITIZE(TQt::Key_Dead_Abovedot), 0x0041, 0, 0}, 0x01e0 }, // U01E0 # LATIN CAPITAL LETTER A WITH DOT ABOVE AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, UNITIZE(TQt::Key_Dead_Abovedot), 0x004f, 0, 0}, 0x0230 }, // U0230 # LATIN CAPITAL LETTER O WITH DOT ABOVE AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, UNITIZE(TQt::Key_Dead_Abovedot), 0x0061, 0, 0}, 0x01e1 }, // U01E1 # LATIN SMALL LETTER A WITH DOT ABOVE AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, UNITIZE(TQt::Key_Dead_Abovedot), 0x006f, 0, 0}, 0x0231 }, // U0231 # LATIN SMALL LETTER O WITH DOT ABOVE AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, UNITIZE(TQt::Key_Dead_Diaeresis), 0x0041, 0, 0}, 0x01de }, // U01DE # LATIN CAPITAL LETTER A WITH DIAERESIS AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, UNITIZE(TQt::Key_Dead_Diaeresis), 0x004f, 0, 0}, 0x022a }, // U022A # LATIN CAPITAL LETTER O WITH DIAERESIS AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, UNITIZE(TQt::Key_Dead_Diaeresis), 0x0055, 0, 0}, 0x01d5 }, // U01D5 # LATIN CAPITAL LETTER U WITH DIAERESIS AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, UNITIZE(TQt::Key_Dead_Diaeresis), 0x0061, 0, 0}, 0x01df }, // U01DF # LATIN SMALL LETTER A WITH DIAERESIS AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, UNITIZE(TQt::Key_Dead_Diaeresis), 0x006f, 0, 0}, 0x022b }, // U022B # LATIN SMALL LETTER O WITH DIAERESIS AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, UNITIZE(TQt::Key_Dead_Diaeresis), 0x0075, 0, 0}, 0x01d6 }, // U01D6 # LATIN SMALL LETTER U WITH DIAERESIS AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, UNITIZE(TQt::Key_Dead_Ogonek), 0x004f, 0, 0}, 0x01ec }, // U01EC # LATIN CAPITAL LETTER O WITH OGONEK AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, UNITIZE(TQt::Key_Dead_Ogonek), 0x006f, 0, 0}, 0x01ed }, // U01ED # LATIN SMALL LETTER O WITH OGONEK AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, UNITIZE(TQt::Key_Dead_Belowdot), 0x004c, 0, 0}, 0x1e38 }, // U1E38 # LATIN CAPITAL LETTER L WITH DOT BELOW AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, UNITIZE(TQt::Key_Dead_Belowdot), 0x0052, 0, 0}, 0x1e5c }, // U1E5C # LATIN CAPITAL LETTER R WITH DOT BELOW AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, UNITIZE(TQt::Key_Dead_Belowdot), 0x006c, 0, 0}, 0x1e39 }, // U1E39 # LATIN SMALL LETTER L WITH DOT BELOW AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x005f, UNITIZE(TQt::Key_Dead_Belowdot), 0x0072, 0, 0}, 0x1e5d }, // U1E5D # LATIN SMALL LETTER R WITH DOT BELOW AND MACRON
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0000, 0, 0, 0}, 0x1edd }, // U1EDD # LATIN SMALL LETTER O WITH HORN AND GRAVE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0000, 0, 0, 0}, 0x1eea }, // U1EEA # LATIN CAPITAL LETTER U WITH HORN AND GRAVE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0000, 0, 0, 0}, 0x1edc }, // U1EDC # LATIN CAPITAL LETTER O WITH HORN AND GRAVE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0000, 0, 0, 0}, 0x1eeb }, // U1EEB # LATIN SMALL LETTER U WITH HORN AND GRAVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0020, 0, 0, 0}, 0x0060 }, // grave
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0022, 0x0055, 0, 0}, 0x01db }, // U01DB # LATIN CAPITAL LETTER U WITH DIAERESIS AND GRAVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0022, 0x0075, 0, 0}, 0x01dc }, // U01DC # LATIN SMALL LETTER U WITH DIAERESIS AND GRAVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0022, 0x03b9, 0, 0}, 0x1fd2 }, // U1FD2 # GREEK SMALL LETTER IOTA WITH DIALYTIKA AND VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0022, 0x03c5, 0, 0}, 0x1fe2 }, // U1FE2 # GREEK SMALL LETTER UPSILON WITH DIALYTIKA AND VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0028, 0x0391, 0, 0}, 0x1f0b }, // U1F0B # GREEK CAPITAL LETTER ALPHA WITH DASIA AND VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0028, 0x0395, 0, 0}, 0x1f1b }, // U1F1B # GREEK CAPITAL LETTER EPSILON WITH DASIA AND VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0028, 0x0397, 0, 0}, 0x1f2b }, // U1F2B # GREEK CAPITAL LETTER ETA WITH DASIA AND VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0028, 0x0399, 0, 0}, 0x1f3b }, // U1F3B # GREEK CAPITAL LETTER IOTA WITH DASIA AND VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0028, 0x039f, 0, 0}, 0x1f4b }, // U1F4B # GREEK CAPITAL LETTER OMICRON WITH DASIA AND VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0028, 0x03a5, 0, 0}, 0x1f5b }, // U1F5B # GREEK CAPITAL LETTER UPSILON WITH DASIA AND VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0028, 0x03a9, 0, 0}, 0x1f6b }, // U1F6B # GREEK CAPITAL LETTER OMEGA WITH DASIA AND VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0028, 0x03b1, 0, 0}, 0x1f03 }, // U1F03 # GREEK SMALL LETTER ALPHA WITH DASIA AND VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0028, 0x03b5, 0, 0}, 0x1f13 }, // U1F13 # GREEK SMALL LETTER EPSILON WITH DASIA AND VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0028, 0x03b7, 0, 0}, 0x1f23 }, // U1F23 # GREEK SMALL LETTER ETA WITH DASIA AND VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0028, 0x03b9, 0, 0}, 0x1f33 }, // U1F33 # GREEK SMALL LETTER IOTA WITH DASIA AND VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0028, 0x03bf, 0, 0}, 0x1f43 }, // U1F43 # GREEK SMALL LETTER OMICRON WITH DASIA AND VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0028, 0x03c5, 0, 0}, 0x1f53 }, // U1F53 # GREEK SMALL LETTER UPSILON WITH DASIA AND VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0028, 0x03c9, 0, 0}, 0x1f63 }, // U1F63 # GREEK SMALL LETTER OMEGA WITH DASIA AND VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0029, 0x0391, 0, 0}, 0x1f0a }, // U1F0A # GREEK CAPITAL LETTER ALPHA WITH PSILI AND VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0029, 0x0395, 0, 0}, 0x1f1a }, // U1F1A # GREEK CAPITAL LETTER EPSILON WITH PSILI AND VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0029, 0x0397, 0, 0}, 0x1f2a }, // U1F2A # GREEK CAPITAL LETTER ETA WITH PSILI AND VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0029, 0x0399, 0, 0}, 0x1f3a }, // U1F3A # GREEK CAPITAL LETTER IOTA WITH PSILI AND VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0029, 0x039f, 0, 0}, 0x1f4a }, // U1F4A # GREEK CAPITAL LETTER OMICRON WITH PSILI AND VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0029, 0x03a9, 0, 0}, 0x1f6a }, // U1F6A # GREEK CAPITAL LETTER OMEGA WITH PSILI AND VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0029, 0x03b1, 0, 0}, 0x1f02 }, // U1F02 # GREEK SMALL LETTER ALPHA WITH PSILI AND VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0029, 0x03b5, 0, 0}, 0x1f12 }, // U1F12 # GREEK SMALL LETTER EPSILON WITH PSILI AND VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0029, 0x03b7, 0, 0}, 0x1f22 }, // U1F22 # GREEK SMALL LETTER ETA WITH PSILI AND VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0029, 0x03b9, 0, 0}, 0x1f32 }, // U1F32 # GREEK SMALL LETTER IOTA WITH PSILI AND VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0029, 0x03bf, 0, 0}, 0x1f42 }, // U1F42 # GREEK SMALL LETTER OMICRON WITH PSILI AND VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0029, 0x03c5, 0, 0}, 0x1f52 }, // U1F52 # GREEK SMALL LETTER UPSILON WITH PSILI AND VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0029, 0x03c9, 0, 0}, 0x1f62 }, // U1F62 # GREEK SMALL LETTER OMEGA WITH PSILI AND VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x002b, 0x004f, 0, 0}, 0x1edc }, // U1EDC # LATIN CAPITAL LETTER O WITH HORN AND GRAVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x002b, 0x0055, 0, 0}, 0x1eea }, // U1EEA # LATIN CAPITAL LETTER U WITH HORN AND GRAVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x002b, 0x006f, 0, 0}, 0x1edd }, // U1EDD # LATIN SMALL LETTER O WITH HORN AND GRAVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x002b, 0x0075, 0, 0}, 0x1eeb }, // U1EEB # LATIN SMALL LETTER U WITH HORN AND GRAVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0041, 0, 0, 0}, 0x00c0 }, // U00C0 # LATIN CAPITAL LETTER A WITH GRAVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0045, 0, 0, 0}, 0x00c8 }, // U00C8 # LATIN CAPITAL LETTER E WITH GRAVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0049, 0, 0, 0}, 0x00cc }, // U00CC # LATIN CAPITAL LETTER I WITH GRAVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x004e, 0, 0, 0}, 0x01f8 }, // U01F8 # LATIN CAPITAL LETTER N WITH GRAVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x004f, 0, 0, 0}, 0x00d2 }, // U00D2 # LATIN CAPITAL LETTER O WITH GRAVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0055, 0, 0, 0}, 0x00d9 }, // U00D9 # LATIN CAPITAL LETTER U WITH GRAVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0055, 0x0041, 0, 0}, 0x1eb0 }, // U1EB0 # LATIN CAPITAL LETTER A WITH BREVE AND GRAVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0055, 0x0061, 0, 0}, 0x1eb1 }, // U1EB1 # LATIN SMALL LETTER A WITH BREVE AND GRAVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0057, 0, 0, 0}, 0x1e80 }, // U1E80 # LATIN CAPITAL LETTER W WITH GRAVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0059, 0, 0, 0}, 0x1ef2 }, // U1EF2 # LATIN CAPITAL LETTER Y WITH GRAVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x005e, 0x0041, 0, 0}, 0x1ea6 }, // U1EA6 # LATIN CAPITAL LETTER A WITH CIRCUMFLEX AND GRAVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x005e, 0x0045, 0, 0}, 0x1ec0 }, // U1EC0 # LATIN CAPITAL LETTER E WITH CIRCUMFLEX AND GRAVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x005e, 0x004f, 0, 0}, 0x1ed2 }, // U1ED2 # LATIN CAPITAL LETTER O WITH CIRCUMFLEX AND GRAVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x005e, 0x0061, 0, 0}, 0x1ea7 }, // U1EA7 # LATIN SMALL LETTER A WITH CIRCUMFLEX AND GRAVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x005e, 0x0065, 0, 0}, 0x1ec1 }, // U1EC1 # LATIN SMALL LETTER E WITH CIRCUMFLEX AND GRAVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x005e, 0x006f, 0, 0}, 0x1ed3 }, // U1ED3 # LATIN SMALL LETTER O WITH CIRCUMFLEX AND GRAVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x005f, 0x0045, 0, 0}, 0x1e14 }, // U1E14 # LATIN CAPITAL LETTER E WITH MACRON AND GRAVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x005f, 0x004f, 0, 0}, 0x1e50 }, // U1E50 # LATIN CAPITAL LETTER O WITH MACRON AND GRAVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x005f, 0x0065, 0, 0}, 0x1e15 }, // U1E15 # LATIN SMALL LETTER E WITH MACRON AND GRAVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x005f, 0x006f, 0, 0}, 0x1e51 }, // U1E51 # LATIN SMALL LETTER O WITH MACRON AND GRAVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0061, 0, 0, 0}, 0x00e0 }, // U00E0 # LATIN SMALL LETTER A WITH GRAVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0062, 0x0041, 0, 0}, 0x1eb0 }, // U1EB0 # LATIN CAPITAL LETTER A WITH BREVE AND GRAVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0062, 0x0061, 0, 0}, 0x1eb1 }, // U1EB1 # LATIN SMALL LETTER A WITH BREVE AND GRAVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0065, 0, 0, 0}, 0x00e8 }, // U00E8 # LATIN SMALL LETTER E WITH GRAVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0069, 0, 0, 0}, 0x00ec }, // U00EC # LATIN SMALL LETTER I WITH GRAVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x006e, 0, 0, 0}, 0x01f9 }, // U01F9 # LATIN SMALL LETTER N WITH GRAVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x006f, 0, 0, 0}, 0x00f2 }, // U00F2 # LATIN SMALL LETTER O WITH GRAVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0075, 0, 0, 0}, 0x00f9 }, // U00F9 # LATIN SMALL LETTER U WITH GRAVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0077, 0, 0, 0}, 0x1e81 }, // U1E81 # LATIN SMALL LETTER W WITH GRAVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0079, 0, 0, 0}, 0x1ef3 }, // U1EF3 # LATIN SMALL LETTER Y WITH GRAVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x00af, 0x0045, 0, 0}, 0x1e14 }, // U1E14 # LATIN CAPITAL LETTER E WITH MACRON AND GRAVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x00af, 0x004f, 0, 0}, 0x1e50 }, // U1E50 # LATIN CAPITAL LETTER O WITH MACRON AND GRAVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x00af, 0x0065, 0, 0}, 0x1e15 }, // U1E15 # LATIN SMALL LETTER E WITH MACRON AND GRAVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x00af, 0x006f, 0, 0}, 0x1e51 }, // U1E51 # LATIN SMALL LETTER O WITH MACRON AND GRAVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x00c2, 0, 0, 0}, 0x1ea6 }, // U1EA6 # LATIN CAPITAL LETTER A WITH CIRCUMFLEX AND GRAVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x00ca, 0, 0, 0}, 0x1ec0 }, // U1EC0 # LATIN CAPITAL LETTER E WITH CIRCUMFLEX AND GRAVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x00d4, 0, 0, 0}, 0x1ed2 }, // U1ED2 # LATIN CAPITAL LETTER O WITH CIRCUMFLEX AND GRAVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x00dc, 0, 0, 0}, 0x01db }, // U01DB # LATIN CAPITAL LETTER U WITH DIAERESIS AND GRAVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x00e2, 0, 0, 0}, 0x1ea7 }, // U1EA7 # LATIN SMALL LETTER A WITH CIRCUMFLEX AND GRAVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x00ea, 0, 0, 0}, 0x1ec1 }, // U1EC1 # LATIN SMALL LETTER E WITH CIRCUMFLEX AND GRAVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x00f4, 0, 0, 0}, 0x1ed3 }, // U1ED3 # LATIN SMALL LETTER O WITH CIRCUMFLEX AND GRAVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x00fc, 0, 0, 0}, 0x01dc }, // U01DC # LATIN SMALL LETTER U WITH DIAERESIS AND GRAVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0102, 0, 0, 0}, 0x1eb0 }, // U1EB0 # LATIN CAPITAL LETTER A WITH BREVE AND GRAVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0103, 0, 0, 0}, 0x1eb1 }, // U1EB1 # LATIN SMALL LETTER A WITH BREVE AND GRAVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0112, 0, 0, 0}, 0x1e14 }, // U1E14 # LATIN CAPITAL LETTER E WITH MACRON AND GRAVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0113, 0, 0, 0}, 0x1e15 }, // U1E15 # LATIN SMALL LETTER E WITH MACRON AND GRAVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x014c, 0, 0, 0}, 0x1e50 }, // U1E50 # LATIN CAPITAL LETTER O WITH MACRON AND GRAVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x014d, 0, 0, 0}, 0x1e51 }, // U1E51 # LATIN SMALL LETTER O WITH MACRON AND GRAVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0313, 0x0391, 0, 0}, 0x1f0a }, // U1F0A # GREEK CAPITAL LETTER ALPHA WITH PSILI AND VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0313, 0x0395, 0, 0}, 0x1f1a }, // U1F1A # GREEK CAPITAL LETTER EPSILON WITH PSILI AND VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0313, 0x0397, 0, 0}, 0x1f2a }, // U1F2A # GREEK CAPITAL LETTER ETA WITH PSILI AND VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0313, 0x0399, 0, 0}, 0x1f3a }, // U1F3A # GREEK CAPITAL LETTER IOTA WITH PSILI AND VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0313, 0x039f, 0, 0}, 0x1f4a }, // U1F4A # GREEK CAPITAL LETTER OMICRON WITH PSILI AND VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0313, 0x03a9, 0, 0}, 0x1f6a }, // U1F6A # GREEK CAPITAL LETTER OMEGA WITH PSILI AND VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0313, 0x03b1, 0, 0}, 0x1f02 }, // U1F02 # GREEK SMALL LETTER ALPHA WITH PSILI AND VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0313, 0x03b5, 0, 0}, 0x1f12 }, // U1F12 # GREEK SMALL LETTER EPSILON WITH PSILI AND VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0313, 0x03b7, 0, 0}, 0x1f22 }, // U1F22 # GREEK SMALL LETTER ETA WITH PSILI AND VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0313, 0x03b9, 0, 0}, 0x1f32 }, // U1F32 # GREEK SMALL LETTER IOTA WITH PSILI AND VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0313, 0x03bf, 0, 0}, 0x1f42 }, // U1F42 # GREEK SMALL LETTER OMICRON WITH PSILI AND VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0313, 0x03c5, 0, 0}, 0x1f52 }, // U1F52 # GREEK SMALL LETTER UPSILON WITH PSILI AND VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0313, 0x03c9, 0, 0}, 0x1f62 }, // U1F62 # GREEK SMALL LETTER OMEGA WITH PSILI AND VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0314, 0x0391, 0, 0}, 0x1f0b }, // U1F0B # GREEK CAPITAL LETTER ALPHA WITH DASIA AND VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0314, 0x0395, 0, 0}, 0x1f1b }, // U1F1B # GREEK CAPITAL LETTER EPSILON WITH DASIA AND VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0314, 0x0397, 0, 0}, 0x1f2b }, // U1F2B # GREEK CAPITAL LETTER ETA WITH DASIA AND VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0314, 0x0399, 0, 0}, 0x1f3b }, // U1F3B # GREEK CAPITAL LETTER IOTA WITH DASIA AND VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0314, 0x039f, 0, 0}, 0x1f4b }, // U1F4B # GREEK CAPITAL LETTER OMICRON WITH DASIA AND VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0314, 0x03a5, 0, 0}, 0x1f5b }, // U1F5B # GREEK CAPITAL LETTER UPSILON WITH DASIA AND VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0314, 0x03a9, 0, 0}, 0x1f6b }, // U1F6B # GREEK CAPITAL LETTER OMEGA WITH DASIA AND VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0314, 0x03b1, 0, 0}, 0x1f03 }, // U1F03 # GREEK SMALL LETTER ALPHA WITH DASIA AND VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0314, 0x03b5, 0, 0}, 0x1f13 }, // U1F13 # GREEK SMALL LETTER EPSILON WITH DASIA AND VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0314, 0x03b7, 0, 0}, 0x1f23 }, // U1F23 # GREEK SMALL LETTER ETA WITH DASIA AND VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0314, 0x03b9, 0, 0}, 0x1f33 }, // U1F33 # GREEK SMALL LETTER IOTA WITH DASIA AND VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0314, 0x03bf, 0, 0}, 0x1f43 }, // U1F43 # GREEK SMALL LETTER OMICRON WITH DASIA AND VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0314, 0x03c5, 0, 0}, 0x1f53 }, // U1F53 # GREEK SMALL LETTER UPSILON WITH DASIA AND VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0314, 0x03c9, 0, 0}, 0x1f63 }, // U1F63 # GREEK SMALL LETTER OMEGA WITH DASIA AND VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0391, 0, 0, 0}, 0x1fba }, // U1FBA # GREEK CAPITAL LETTER ALPHA WITH VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0395, 0, 0, 0}, 0x1fc8 }, // U1FC8 # GREEK CAPITAL LETTER EPSILON WITH VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0397, 0, 0, 0}, 0x1fca }, // U1FCA # GREEK CAPITAL LETTER ETA WITH VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0399, 0, 0, 0}, 0x1fda }, // U1FDA # GREEK CAPITAL LETTER IOTA WITH VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x039f, 0, 0, 0}, 0x1ff8 }, // U1FF8 # GREEK CAPITAL LETTER OMICRON WITH VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x03a5, 0, 0, 0}, 0x1fea }, // U1FEA # GREEK CAPITAL LETTER UPSILON WITH VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x03a9, 0, 0, 0}, 0x1ffa }, // U1FFA # GREEK CAPITAL LETTER OMEGA WITH VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x03b1, 0, 0, 0}, 0x1f70 }, // U1F70 # GREEK SMALL LETTER ALPHA WITH VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x03b5, 0, 0, 0}, 0x1f72 }, // U1F72 # GREEK SMALL LETTER EPSILON WITH VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x03b7, 0, 0, 0}, 0x1f74 }, // U1F74 # GREEK SMALL LETTER ETA WITH VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x03b9, 0, 0, 0}, 0x1f76 }, // U1F76 # GREEK SMALL LETTER IOTA WITH VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x03bf, 0, 0, 0}, 0x1f78 }, // U1F78 # GREEK SMALL LETTER OMICRON WITH VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x03c5, 0, 0, 0}, 0x1f7a }, // U1F7A # GREEK SMALL LETTER UPSILON WITH VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x03c9, 0, 0, 0}, 0x1f7c }, // U1F7C # GREEK SMALL LETTER OMEGA WITH VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x03ca, 0, 0, 0}, 0x1fd2 }, // U1FD2 # GREEK SMALL LETTER IOTA WITH DIALYTIKA AND VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x03cb, 0, 0, 0}, 0x1fe2 }, // U1FE2 # GREEK SMALL LETTER UPSILON WITH DIALYTIKA AND VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0415, 0, 0, 0}, 0x0400 }, // U0400 # CYRILLIC CAPITAL LETTER IE WITH GRAVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0418, 0, 0, 0}, 0x040d }, // U040D # CYRILLIC CAPITAL LETTER I WITH GRAVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0435, 0, 0, 0}, 0x0450 }, // U0450 # CYRILLIC SMALL LETTER IE WITH GRAVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0438, 0, 0, 0}, 0x045d }, // U045D # CYRILLIC SMALL LETTER I WITH GRAVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x1f00, 0, 0, 0}, 0x1f02 }, // U1F02 # GREEK SMALL LETTER ALPHA WITH PSILI AND VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x1f01, 0, 0, 0}, 0x1f03 }, // U1F03 # GREEK SMALL LETTER ALPHA WITH DASIA AND VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x1f08, 0, 0, 0}, 0x1f0a }, // U1F0A # GREEK CAPITAL LETTER ALPHA WITH PSILI AND VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x1f09, 0, 0, 0}, 0x1f0b }, // U1F0B # GREEK CAPITAL LETTER ALPHA WITH DASIA AND VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x1f10, 0, 0, 0}, 0x1f12 }, // U1F12 # GREEK SMALL LETTER EPSILON WITH PSILI AND VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x1f11, 0, 0, 0}, 0x1f13 }, // U1F13 # GREEK SMALL LETTER EPSILON WITH DASIA AND VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x1f18, 0, 0, 0}, 0x1f1a }, // U1F1A # GREEK CAPITAL LETTER EPSILON WITH PSILI AND VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x1f19, 0, 0, 0}, 0x1f1b }, // U1F1B # GREEK CAPITAL LETTER EPSILON WITH DASIA AND VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x1f20, 0, 0, 0}, 0x1f22 }, // U1F22 # GREEK SMALL LETTER ETA WITH PSILI AND VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x1f21, 0, 0, 0}, 0x1f23 }, // U1F23 # GREEK SMALL LETTER ETA WITH DASIA AND VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x1f28, 0, 0, 0}, 0x1f2a }, // U1F2A # GREEK CAPITAL LETTER ETA WITH PSILI AND VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x1f29, 0, 0, 0}, 0x1f2b }, // U1F2B # GREEK CAPITAL LETTER ETA WITH DASIA AND VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x1f30, 0, 0, 0}, 0x1f32 }, // U1F32 # GREEK SMALL LETTER IOTA WITH PSILI AND VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x1f31, 0, 0, 0}, 0x1f33 }, // U1F33 # GREEK SMALL LETTER IOTA WITH DASIA AND VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x1f38, 0, 0, 0}, 0x1f3a }, // U1F3A # GREEK CAPITAL LETTER IOTA WITH PSILI AND VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x1f39, 0, 0, 0}, 0x1f3b }, // U1F3B # GREEK CAPITAL LETTER IOTA WITH DASIA AND VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x1f40, 0, 0, 0}, 0x1f42 }, // U1F42 # GREEK SMALL LETTER OMICRON WITH PSILI AND VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x1f41, 0, 0, 0}, 0x1f43 }, // U1F43 # GREEK SMALL LETTER OMICRON WITH DASIA AND VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x1f48, 0, 0, 0}, 0x1f4a }, // U1F4A # GREEK CAPITAL LETTER OMICRON WITH PSILI AND VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x1f49, 0, 0, 0}, 0x1f4b }, // U1F4B # GREEK CAPITAL LETTER OMICRON WITH DASIA AND VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x1f50, 0, 0, 0}, 0x1f52 }, // U1F52 # GREEK SMALL LETTER UPSILON WITH PSILI AND VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x1f51, 0, 0, 0}, 0x1f53 }, // U1F53 # GREEK SMALL LETTER UPSILON WITH DASIA AND VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x1f59, 0, 0, 0}, 0x1f5b }, // U1F5B # GREEK CAPITAL LETTER UPSILON WITH DASIA AND VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x1f60, 0, 0, 0}, 0x1f62 }, // U1F62 # GREEK SMALL LETTER OMEGA WITH PSILI AND VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x1f61, 0, 0, 0}, 0x1f63 }, // U1F63 # GREEK SMALL LETTER OMEGA WITH DASIA AND VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x1f68, 0, 0, 0}, 0x1f6a }, // U1F6A # GREEK CAPITAL LETTER OMEGA WITH PSILI AND VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, 0x1f69, 0, 0, 0}, 0x1f6b }, // U1F6B # GREEK CAPITAL LETTER OMEGA WITH DASIA AND VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, UNITIZE(TQt::Key_Dead_Circumflex), 0x0041, 0, 0}, 0x1ea6 }, // U1EA6 # LATIN CAPITAL LETTER A WITH CIRCUMFLEX AND GRAVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, UNITIZE(TQt::Key_Dead_Circumflex), 0x0045, 0, 0}, 0x1ec0 }, // U1EC0 # LATIN CAPITAL LETTER E WITH CIRCUMFLEX AND GRAVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, UNITIZE(TQt::Key_Dead_Circumflex), 0x004f, 0, 0}, 0x1ed2 }, // U1ED2 # LATIN CAPITAL LETTER O WITH CIRCUMFLEX AND GRAVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, UNITIZE(TQt::Key_Dead_Circumflex), 0x0061, 0, 0}, 0x1ea7 }, // U1EA7 # LATIN SMALL LETTER A WITH CIRCUMFLEX AND GRAVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, UNITIZE(TQt::Key_Dead_Circumflex), 0x0065, 0, 0}, 0x1ec1 }, // U1EC1 # LATIN SMALL LETTER E WITH CIRCUMFLEX AND GRAVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, UNITIZE(TQt::Key_Dead_Circumflex), 0x006f, 0, 0}, 0x1ed3 }, // U1ED3 # LATIN SMALL LETTER O WITH CIRCUMFLEX AND GRAVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, UNITIZE(TQt::Key_Dead_Macron), 0x0045, 0, 0}, 0x1e14 }, // U1E14 # LATIN CAPITAL LETTER E WITH MACRON AND GRAVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, UNITIZE(TQt::Key_Dead_Macron), 0x004f, 0, 0}, 0x1e50 }, // U1E50 # LATIN CAPITAL LETTER O WITH MACRON AND GRAVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, UNITIZE(TQt::Key_Dead_Macron), 0x0065, 0, 0}, 0x1e15 }, // U1E15 # LATIN SMALL LETTER E WITH MACRON AND GRAVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, UNITIZE(TQt::Key_Dead_Macron), 0x006f, 0, 0}, 0x1e51 }, // U1E51 # LATIN SMALL LETTER O WITH MACRON AND GRAVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, UNITIZE(TQt::Key_Dead_Breve), 0x0041, 0, 0}, 0x1eb0 }, // U1EB0 # LATIN CAPITAL LETTER A WITH BREVE AND GRAVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, UNITIZE(TQt::Key_Dead_Breve), 0x0061, 0, 0}, 0x1eb1 }, // U1EB1 # LATIN SMALL LETTER A WITH BREVE AND GRAVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, UNITIZE(TQt::Key_Dead_Diaeresis), 0x0055, 0, 0}, 0x01db }, // U01DB # LATIN CAPITAL LETTER U WITH DIAERESIS AND GRAVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, UNITIZE(TQt::Key_Dead_Diaeresis), 0x0075, 0, 0}, 0x01dc }, // U01DC # LATIN SMALL LETTER U WITH DIAERESIS AND GRAVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, UNITIZE(TQt::Key_Dead_Diaeresis), 0x03b9, 0, 0}, 0x1fd2 }, // U1FD2 # GREEK SMALL LETTER IOTA WITH DIALYTIKA AND VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, UNITIZE(TQt::Key_Dead_Diaeresis), 0x03c5, 0, 0}, 0x1fe2 }, // U1FE2 # GREEK SMALL LETTER UPSILON WITH DIALYTIKA AND VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, UNITIZE(TQt::Key_Dead_Horn), 0x004f, 0, 0}, 0x1edc }, // U1EDC # LATIN CAPITAL LETTER O WITH HORN AND GRAVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, UNITIZE(TQt::Key_Dead_Horn), 0x0055, 0, 0}, 0x1eea }, // U1EEA # LATIN CAPITAL LETTER U WITH HORN AND GRAVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, UNITIZE(TQt::Key_Dead_Horn), 0x006f, 0, 0}, 0x1edd }, // U1EDD # LATIN SMALL LETTER O WITH HORN AND GRAVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0060, UNITIZE(TQt::Key_Dead_Horn), 0x0075, 0, 0}, 0x1eeb }, // U1EEB # LATIN SMALL LETTER U WITH HORN AND GRAVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0061, 0x0065, 0, 0, 0}, 0x00e6 }, // ae
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0062, 0x0000, 0x0041, 0, 0}, 0x1eb6 }, // U1EB6 # LATIN CAPITAL LETTER A WITH BREVE AND DOT BELOW
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x0062, 0x0000, 0x0061, 0, 0}, 0x1eb7 }, // U1EB7 # LATIN SMALL LETTER A WITH BREVE AND DOT BELOW
    { {UNITIZE(TQt::Key_Multi_key), 0x0062, 0x0021, 0x0041, 0, 0}, 0x1eb6 }, // U1EB6 # LATIN CAPITAL LETTER A WITH BREVE AND DOT BELOW
    { {UNITIZE(TQt::Key_Multi_key), 0x0062, 0x0021, 0x0061, 0, 0}, 0x1eb7 }, // U1EB7 # LATIN SMALL LETTER A WITH BREVE AND DOT BELOW
    { {UNITIZE(TQt::Key_Multi_key), 0x0062, 0x002c, 0x0045, 0, 0}, 0x1e1c }, // U1E1C # LATIN CAPITAL LETTER E WITH CEDILLA AND BREVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0062, 0x002c, 0x0065, 0, 0}, 0x1e1d }, // U1E1D # LATIN SMALL LETTER E WITH CEDILLA AND BREVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0062, 0x0041, 0, 0, 0}, 0x0102 }, // U0102 # LATIN CAPITAL LETTER A WITH BREVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0062, 0x0045, 0, 0, 0}, 0x0114 }, // U0114 # LATIN CAPITAL LETTER E WITH BREVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0062, 0x0047, 0, 0, 0}, 0x011e }, // U011E # LATIN CAPITAL LETTER G WITH BREVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0062, 0x0049, 0, 0, 0}, 0x012c }, // U012C # LATIN CAPITAL LETTER I WITH BREVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0062, 0x004f, 0, 0, 0}, 0x014e }, // U014E # LATIN CAPITAL LETTER O WITH BREVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0062, 0x0055, 0, 0, 0}, 0x016c }, // U016C # LATIN CAPITAL LETTER U WITH BREVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0062, 0x0061, 0, 0, 0}, 0x0103 }, // U0103 # LATIN SMALL LETTER A WITH BREVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0062, 0x0065, 0, 0, 0}, 0x0115 }, // U0115 # LATIN SMALL LETTER E WITH BREVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0062, 0x0067, 0, 0, 0}, 0x011f }, // U011F # LATIN SMALL LETTER G WITH BREVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0062, 0x0069, 0, 0, 0}, 0x012d }, // U012D # LATIN SMALL LETTER I WITH BREVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0062, 0x006f, 0, 0, 0}, 0x014f }, // U014F # LATIN SMALL LETTER O WITH BREVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0062, 0x0075, 0, 0, 0}, 0x016d }, // U016D # LATIN SMALL LETTER U WITH BREVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0062, 0x0228, 0, 0, 0}, 0x1e1c }, // U1E1C # LATIN CAPITAL LETTER E WITH CEDILLA AND BREVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0062, 0x0229, 0, 0, 0}, 0x1e1d }, // U1E1D # LATIN SMALL LETTER E WITH CEDILLA AND BREVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0062, 0x0391, 0, 0, 0}, 0x1fb8 }, // U1FB8 # GREEK CAPITAL LETTER ALPHA WITH VRACHY
    { {UNITIZE(TQt::Key_Multi_key), 0x0062, 0x0399, 0, 0, 0}, 0x1fd8 }, // U1FD8 # GREEK CAPITAL LETTER IOTA WITH VRACHY
    { {UNITIZE(TQt::Key_Multi_key), 0x0062, 0x03a5, 0, 0, 0}, 0x1fe8 }, // U1FE8 # GREEK CAPITAL LETTER UPSILON WITH VRACHY
    { {UNITIZE(TQt::Key_Multi_key), 0x0062, 0x03b1, 0, 0, 0}, 0x1fb0 }, // U1FB0 # GREEK SMALL LETTER ALPHA WITH VRACHY
    { {UNITIZE(TQt::Key_Multi_key), 0x0062, 0x03b9, 0, 0, 0}, 0x1fd0 }, // U1FD0 # GREEK SMALL LETTER IOTA WITH VRACHY
    { {UNITIZE(TQt::Key_Multi_key), 0x0062, 0x03c5, 0, 0, 0}, 0x1fe0 }, // U1FE0 # GREEK SMALL LETTER UPSILON WITH VRACHY
    { {UNITIZE(TQt::Key_Multi_key), 0x0062, 0x0410, 0, 0, 0}, 0x04d0 }, // U04D0 # CYRILLIC CAPITAL LETTER A WITH BREVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0062, 0x0415, 0, 0, 0}, 0x04d6 }, // U04D6 # CYRILLIC CAPITAL LETTER IE WITH BREVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0062, 0x0416, 0, 0, 0}, 0x04c1 }, // U04C1 # CYRILLIC CAPITAL LETTER ZHE WITH BREVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0062, 0x0418, 0, 0, 0}, 0x0419 }, // U0419 # CYRILLIC CAPITAL LETTER SHORT I
    { {UNITIZE(TQt::Key_Multi_key), 0x0062, 0x0423, 0, 0, 0}, 0x040e }, // U040E # CYRILLIC CAPITAL LETTER SHORT U
    { {UNITIZE(TQt::Key_Multi_key), 0x0062, 0x0430, 0, 0, 0}, 0x04d1 }, // U04D1 # CYRILLIC SMALL LETTER A WITH BREVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0062, 0x0435, 0, 0, 0}, 0x04d7 }, // U04D7 # CYRILLIC SMALL LETTER IE WITH BREVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0062, 0x0436, 0, 0, 0}, 0x04c2 }, // U04C2 # CYRILLIC SMALL LETTER ZHE WITH BREVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0062, 0x0438, 0, 0, 0}, 0x0439 }, // U0439 # CYRILLIC SMALL LETTER SHORT I
    { {UNITIZE(TQt::Key_Multi_key), 0x0062, 0x0443, 0, 0, 0}, 0x045e }, // U045E # CYRILLIC SMALL LETTER SHORT U
    { {UNITIZE(TQt::Key_Multi_key), 0x0062, 0x1ea0, 0, 0, 0}, 0x1eb6 }, // U1EB6 # LATIN CAPITAL LETTER A WITH BREVE AND DOT BELOW
    { {UNITIZE(TQt::Key_Multi_key), 0x0062, 0x1ea1, 0, 0, 0}, 0x1eb7 }, // U1EB7 # LATIN SMALL LETTER A WITH BREVE AND DOT BELOW
    { {UNITIZE(TQt::Key_Multi_key), 0x0062, UNITIZE(TQt::Key_Dead_Cedilla), 0x0045, 0, 0}, 0x1e1c }, // U1E1C # LATIN CAPITAL LETTER E WITH CEDILLA AND BREVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0062, UNITIZE(TQt::Key_Dead_Cedilla), 0x0065, 0, 0}, 0x1e1d }, // U1E1D # LATIN SMALL LETTER E WITH CEDILLA AND BREVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0062, UNITIZE(TQt::Key_Dead_Belowdot), 0x0041, 0, 0}, 0x1eb6 }, // U1EB6 # LATIN CAPITAL LETTER A WITH BREVE AND DOT BELOW
    { {UNITIZE(TQt::Key_Multi_key), 0x0062, UNITIZE(TQt::Key_Dead_Belowdot), 0x0061, 0, 0}, 0x1eb7 }, // U1EB7 # LATIN SMALL LETTER A WITH BREVE AND DOT BELOW
    { {UNITIZE(TQt::Key_Multi_key), 0x0063, 0x0022, 0x0055, 0, 0}, 0x01d9 }, // U01D9 # LATIN CAPITAL LETTER U WITH DIAERESIS AND CARON
    { {UNITIZE(TQt::Key_Multi_key), 0x0063, 0x0022, 0x0075, 0, 0}, 0x01da }, // U01DA # LATIN SMALL LETTER U WITH DIAERESIS AND CARON
    { {UNITIZE(TQt::Key_Multi_key), 0x0063, 0x002f, 0, 0, 0}, 0x00a2 }, // U00A2 # CENT SIGN
    { {UNITIZE(TQt::Key_Multi_key), 0x0063, 0x0041, 0, 0, 0}, 0x01cd }, // U01CD # LATIN CAPITAL LETTER A WITH CARON
    { {UNITIZE(TQt::Key_Multi_key), 0x0063, 0x0043, 0, 0, 0}, 0x010c }, // U010C # LATIN CAPITAL LETTER C WITH CARON
    { {UNITIZE(TQt::Key_Multi_key), 0x0063, 0x0044, 0, 0, 0}, 0x010e }, // U010E # LATIN CAPITAL LETTER D WITH CARON
    { {UNITIZE(TQt::Key_Multi_key), 0x0063, 0x0045, 0, 0, 0}, 0x011a }, // U011A # LATIN CAPITAL LETTER E WITH CARON
    { {UNITIZE(TQt::Key_Multi_key), 0x0063, 0x0047, 0, 0, 0}, 0x01e6 }, // U01E6 # LATIN CAPITAL LETTER G WITH CARON
    { {UNITIZE(TQt::Key_Multi_key), 0x0063, 0x0048, 0, 0, 0}, 0x021e }, // U021E # LATIN CAPITAL LETTER H WITH CARON
    { {UNITIZE(TQt::Key_Multi_key), 0x0063, 0x0049, 0, 0, 0}, 0x01cf }, // U01CF # LATIN CAPITAL LETTER I WITH CARON
    { {UNITIZE(TQt::Key_Multi_key), 0x0063, 0x004b, 0, 0, 0}, 0x01e8 }, // U01E8 # LATIN CAPITAL LETTER K WITH CARON
    { {UNITIZE(TQt::Key_Multi_key), 0x0063, 0x004c, 0, 0, 0}, 0x013d }, // U013D # LATIN CAPITAL LETTER L WITH CARON
    { {UNITIZE(TQt::Key_Multi_key), 0x0063, 0x004e, 0, 0, 0}, 0x0147 }, // U0147 # LATIN CAPITAL LETTER N WITH CARON
    { {UNITIZE(TQt::Key_Multi_key), 0x0063, 0x004f, 0, 0, 0}, 0x01d1 }, // U01D1 # LATIN CAPITAL LETTER O WITH CARON
    { {UNITIZE(TQt::Key_Multi_key), 0x0063, 0x0052, 0, 0, 0}, 0x0158 }, // U0158 # LATIN CAPITAL LETTER R WITH CARON
    { {UNITIZE(TQt::Key_Multi_key), 0x0063, 0x0053, 0, 0, 0}, 0x0160 }, // U0160 # LATIN CAPITAL LETTER S WITH CARON
    { {UNITIZE(TQt::Key_Multi_key), 0x0063, 0x0054, 0, 0, 0}, 0x0164 }, // U0164 # LATIN CAPITAL LETTER T WITH CARON
    { {UNITIZE(TQt::Key_Multi_key), 0x0063, 0x0055, 0, 0, 0}, 0x01d3 }, // U01D3 # LATIN CAPITAL LETTER U WITH CARON
    { {UNITIZE(TQt::Key_Multi_key), 0x0063, 0x005a, 0, 0, 0}, 0x017d }, // U017D # LATIN CAPITAL LETTER Z WITH CARON
    { {UNITIZE(TQt::Key_Multi_key), 0x0063, 0x0061, 0, 0, 0}, 0x01ce }, // U01CE # LATIN SMALL LETTER A WITH CARON
    { {UNITIZE(TQt::Key_Multi_key), 0x0063, 0x0063, 0, 0, 0}, 0x010d }, // U010D # LATIN SMALL LETTER C WITH CARON
    { {UNITIZE(TQt::Key_Multi_key), 0x0063, 0x0064, 0, 0, 0}, 0x010f }, // U010F # LATIN SMALL LETTER D WITH CARON
    { {UNITIZE(TQt::Key_Multi_key), 0x0063, 0x0065, 0, 0, 0}, 0x011b }, // U011B # LATIN SMALL LETTER E WITH CARON
    { {UNITIZE(TQt::Key_Multi_key), 0x0063, 0x0067, 0, 0, 0}, 0x01e7 }, // U01E7 # LATIN SMALL LETTER G WITH CARON
    { {UNITIZE(TQt::Key_Multi_key), 0x0063, 0x0068, 0, 0, 0}, 0x021f }, // U021F # LATIN SMALL LETTER H WITH CARON
    { {UNITIZE(TQt::Key_Multi_key), 0x0063, 0x0069, 0, 0, 0}, 0x01d0 }, // U01D0 # LATIN SMALL LETTER I WITH CARON
    { {UNITIZE(TQt::Key_Multi_key), 0x0063, 0x006a, 0, 0, 0}, 0x01f0 }, // U01F0 # LATIN SMALL LETTER J WITH CARON
    { {UNITIZE(TQt::Key_Multi_key), 0x0063, 0x006b, 0, 0, 0}, 0x01e9 }, // U01E9 # LATIN SMALL LETTER K WITH CARON
    { {UNITIZE(TQt::Key_Multi_key), 0x0063, 0x006c, 0, 0, 0}, 0x013e }, // U013E # LATIN SMALL LETTER L WITH CARON
    { {UNITIZE(TQt::Key_Multi_key), 0x0063, 0x006e, 0, 0, 0}, 0x0148 }, // U0148 # LATIN SMALL LETTER N WITH CARON
    { {UNITIZE(TQt::Key_Multi_key), 0x0063, 0x006f, 0, 0, 0}, 0x01d2 }, // U01D2 # LATIN SMALL LETTER O WITH CARON
    { {UNITIZE(TQt::Key_Multi_key), 0x0063, 0x0072, 0, 0, 0}, 0x0159 }, // U0159 # LATIN SMALL LETTER R WITH CARON
    { {UNITIZE(TQt::Key_Multi_key), 0x0063, 0x0073, 0, 0, 0}, 0x0161 }, // U0161 # LATIN SMALL LETTER S WITH CARON
    { {UNITIZE(TQt::Key_Multi_key), 0x0063, 0x0074, 0, 0, 0}, 0x0165 }, // U0165 # LATIN SMALL LETTER T WITH CARON
    { {UNITIZE(TQt::Key_Multi_key), 0x0063, 0x0075, 0, 0, 0}, 0x01d4 }, // U01D4 # LATIN SMALL LETTER U WITH CARON
    { {UNITIZE(TQt::Key_Multi_key), 0x0063, 0x007a, 0, 0, 0}, 0x017e }, // U017E # LATIN SMALL LETTER Z WITH CARON
    { {UNITIZE(TQt::Key_Multi_key), 0x0063, 0x007c, 0, 0, 0}, 0x00a2 }, // U00A2 # CENT SIGN
    { {UNITIZE(TQt::Key_Multi_key), 0x0063, 0x00dc, 0, 0, 0}, 0x01d9 }, // U01D9 # LATIN CAPITAL LETTER U WITH DIAERESIS AND CARON
    { {UNITIZE(TQt::Key_Multi_key), 0x0063, 0x00fc, 0, 0, 0}, 0x01da }, // U01DA # LATIN SMALL LETTER U WITH DIAERESIS AND CARON
    { {UNITIZE(TQt::Key_Multi_key), 0x0063, 0x01b7, 0, 0, 0}, 0x01ee }, // U01EE # LATIN CAPITAL LETTER EZH WITH CARON
    { {UNITIZE(TQt::Key_Multi_key), 0x0063, 0x0292, 0, 0, 0}, 0x01ef }, // U01EF # LATIN SMALL LETTER EZH WITH CARON
    { {UNITIZE(TQt::Key_Multi_key), 0x0063, UNITIZE(TQt::Key_Dead_Diaeresis), 0x0055, 0, 0}, 0x01d9 }, // U01D9 # LATIN CAPITAL LETTER U WITH DIAERESIS AND CARON
    { {UNITIZE(TQt::Key_Multi_key), 0x0063, UNITIZE(TQt::Key_Dead_Diaeresis), 0x0075, 0, 0}, 0x01da }, // U01DA # LATIN SMALL LETTER U WITH DIAERESIS AND CARON
    { {UNITIZE(TQt::Key_Multi_key), 0x0064, 0x002d, 0, 0, 0}, 0x20ab }, // U20ab # DONG SIGN
    { {UNITIZE(TQt::Key_Multi_key), 0x0064, 0x0068, 0, 0, 0}, 0x00f0 }, // U00F0 # LATIN SMALL LETTER ETH
    { {UNITIZE(TQt::Key_Multi_key), 0x0065, 0x0065, 0, 0, 0}, 0x0259 }, // U0259 # LATIN SMALL LETTER SCHWA
    { {UNITIZE(TQt::Key_Multi_key), 0x0066, 0x0053, 0, 0, 0}, 0x017f }, // U017f # LONG S
    { {UNITIZE(TQt::Key_Multi_key), 0x0066, 0x0073, 0, 0, 0}, 0x017f }, // U017f # LONG S
    { {UNITIZE(TQt::Key_Multi_key), 0x0069, 0x002e, 0, 0, 0}, 0x0131 }, // U0131 # LATIN SMALL LETTER DOTLESS I
    { {UNITIZE(TQt::Key_Multi_key), 0x006b, 0x006b, 0, 0, 0}, 0x0138 }, // U0138 # LATIN SMALL LETTER KRA
    { {UNITIZE(TQt::Key_Multi_key), 0x006c, 0x0076, 0, 0, 0}, 0x007c }, // bar
    { {UNITIZE(TQt::Key_Multi_key), 0x006d, 0x002f, 0, 0, 0}, 0x20a5 }, // U20a5 # MILL SIGN
    { {UNITIZE(TQt::Key_Multi_key), 0x006d, 0x0075, 0, 0, 0}, 0x00b5 }, // U00B5 # MICRO SIGN
    { {UNITIZE(TQt::Key_Multi_key), 0x006e, 0x0067, 0, 0, 0}, 0x014b }, // U014B # LATIN SMALL LETTER ENG
    { {UNITIZE(TQt::Key_Multi_key), 0x006f, 0x0027, 0x0041, 0, 0}, 0x01fa }, // U01FA # LATIN CAPITAL LETTER A WITH RING ABOVE AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x006f, 0x0027, 0x0061, 0, 0}, 0x01fb }, // U01FB # LATIN SMALL LETTER A WITH RING ABOVE AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x006f, 0x0041, 0, 0, 0}, 0x00c5 }, // U00C5 # LATIN CAPITAL LETTER A WITH RING ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x006f, 0x0043, 0, 0, 0}, 0x00a9 }, // copyright
    { {UNITIZE(TQt::Key_Multi_key), 0x006f, 0x0052, 0, 0, 0}, 0x00ae }, // registered
    { {UNITIZE(TQt::Key_Multi_key), 0x006f, 0x0055, 0, 0, 0}, 0x016e }, // U016E # LATIN CAPITAL LETTER U WITH RING ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x006f, 0x0061, 0, 0, 0}, 0x00e5 }, // U00E5 # LATIN SMALL LETTER A WITH RING ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x006f, 0x0063, 0, 0, 0}, 0x00a9 }, // copyright
    { {UNITIZE(TQt::Key_Multi_key), 0x006f, 0x0065, 0, 0, 0}, 0x0153 }, // oe
    { {UNITIZE(TQt::Key_Multi_key), 0x006f, 0x006f, 0, 0, 0}, 0x00b0 }, // degree # ^0 already taken
    { {UNITIZE(TQt::Key_Multi_key), 0x006f, 0x0072, 0, 0, 0}, 0x00ae }, // registered
    { {UNITIZE(TQt::Key_Multi_key), 0x006f, 0x0073, 0, 0, 0}, 0x00a7 }, // section # SECTION SIGN
    { {UNITIZE(TQt::Key_Multi_key), 0x006f, 0x0075, 0, 0, 0}, 0x016f }, // U016F # LATIN SMALL LETTER U WITH RING ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x006f, 0x0077, 0, 0, 0}, 0x1e98 }, // U1E98 # LATIN SMALL LETTER W WITH RING ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x006f, 0x0078, 0, 0, 0}, 0x00a4 }, // currency
    { {UNITIZE(TQt::Key_Multi_key), 0x006f, 0x0079, 0, 0, 0}, 0x1e99 }, // U1E99 # LATIN SMALL LETTER Y WITH RING ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0070, 0x0021, 0, 0, 0}, 0x00b6 }, // paragraph
    { {UNITIZE(TQt::Key_Multi_key), 0x0073, 0x006f, 0, 0, 0}, 0x00a7 }, // section # SECTION SIGN
    { {UNITIZE(TQt::Key_Multi_key), 0x0073, 0x0073, 0, 0, 0}, 0x00df }, // ssharp
    { {UNITIZE(TQt::Key_Multi_key), 0x0074, 0x0068, 0, 0, 0}, 0x00fe }, // U00FE # LATIN SMALL LETTER THORN
    { {UNITIZE(TQt::Key_Multi_key), 0x0076, 0x006c, 0, 0, 0}, 0x007c }, // bar
    { {UNITIZE(TQt::Key_Multi_key), 0x0078, 0x006f, 0, 0, 0}, 0x00a4 }, // currency
    { {UNITIZE(TQt::Key_Multi_key), 0x0078, 0x0078, 0, 0, 0}, 0x00d7 }, // U00D7 # MULTIPLICATION SIGN
    { {UNITIZE(TQt::Key_Multi_key), 0x007c, 0x0043, 0, 0, 0}, 0x00a2 }, // U00A2 # CENT SIGN
    { {UNITIZE(TQt::Key_Multi_key), 0x007c, 0x0063, 0, 0, 0}, 0x00a2 }, // U00A2 # CENT SIGN
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x0000, 0, 0, 0}, 0x1eee }, // U1EEE # LATIN CAPITAL LETTER U WITH HORN AND TILDE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x0000, 0, 0, 0}, 0x1ee1 }, // U1EE1 # LATIN SMALL LETTER O WITH HORN AND TILDE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x0000, 0, 0, 0}, 0x1eef }, // U1EEF # LATIN SMALL LETTER U WITH HORN AND TILDE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x0000, 0, 0, 0}, 0x1ee0 }, // U1EE0 # LATIN CAPITAL LETTER O WITH HORN AND TILDE
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x0020, 0, 0, 0}, 0x007e }, // asciitilde
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x0022, 0x03b9, 0, 0}, 0x1fd7 }, // U1FD7 # GREEK SMALL LETTER IOTA WITH DIALYTIKA AND PERISPOMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x0022, 0x03c5, 0, 0}, 0x1fe7 }, // U1FE7 # GREEK SMALL LETTER UPSILON WITH DIALYTIKA AND PERISPOMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x0028, 0x0391, 0, 0}, 0x1f0f }, // U1F0F # GREEK CAPITAL LETTER ALPHA WITH DASIA AND PERISPOMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x0028, 0x0397, 0, 0}, 0x1f2f }, // U1F2F # GREEK CAPITAL LETTER ETA WITH DASIA AND PERISPOMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x0028, 0x0399, 0, 0}, 0x1f3f }, // U1F3F # GREEK CAPITAL LETTER IOTA WITH DASIA AND PERISPOMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x0028, 0x03a5, 0, 0}, 0x1f5f }, // U1F5F # GREEK CAPITAL LETTER UPSILON WITH DASIA AND PERISPOMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x0028, 0x03a9, 0, 0}, 0x1f6f }, // U1F6F # GREEK CAPITAL LETTER OMEGA WITH DASIA AND PERISPOMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x0028, 0x03b1, 0, 0}, 0x1f07 }, // U1F07 # GREEK SMALL LETTER ALPHA WITH DASIA AND PERISPOMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x0028, 0x03b7, 0, 0}, 0x1f27 }, // U1F27 # GREEK SMALL LETTER ETA WITH DASIA AND PERISPOMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x0028, 0x03b9, 0, 0}, 0x1f37 }, // U1F37 # GREEK SMALL LETTER IOTA WITH DASIA AND PERISPOMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x0028, 0x03c5, 0, 0}, 0x1f57 }, // U1F57 # GREEK SMALL LETTER UPSILON WITH DASIA AND PERISPOMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x0028, 0x03c9, 0, 0}, 0x1f67 }, // U1F67 # GREEK SMALL LETTER OMEGA WITH DASIA AND PERISPOMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x0029, 0x0391, 0, 0}, 0x1f0e }, // U1F0E # GREEK CAPITAL LETTER ALPHA WITH PSILI AND PERISPOMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x0029, 0x0397, 0, 0}, 0x1f2e }, // U1F2E # GREEK CAPITAL LETTER ETA WITH PSILI AND PERISPOMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x0029, 0x0399, 0, 0}, 0x1f3e }, // U1F3E # GREEK CAPITAL LETTER IOTA WITH PSILI AND PERISPOMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x0029, 0x03a9, 0, 0}, 0x1f6e }, // U1F6E # GREEK CAPITAL LETTER OMEGA WITH PSILI AND PERISPOMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x0029, 0x03b1, 0, 0}, 0x1f06 }, // U1F06 # GREEK SMALL LETTER ALPHA WITH PSILI AND PERISPOMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x0029, 0x03b7, 0, 0}, 0x1f26 }, // U1F26 # GREEK SMALL LETTER ETA WITH PSILI AND PERISPOMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x0029, 0x03b9, 0, 0}, 0x1f36 }, // U1F36 # GREEK SMALL LETTER IOTA WITH PSILI AND PERISPOMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x0029, 0x03c5, 0, 0}, 0x1f56 }, // U1F56 # GREEK SMALL LETTER UPSILON WITH PSILI AND PERISPOMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x0029, 0x03c9, 0, 0}, 0x1f66 }, // U1F66 # GREEK SMALL LETTER OMEGA WITH PSILI AND PERISPOMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x002b, 0x004f, 0, 0}, 0x1ee0 }, // U1EE0 # LATIN CAPITAL LETTER O WITH HORN AND TILDE
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x002b, 0x0055, 0, 0}, 0x1eee }, // U1EEE # LATIN CAPITAL LETTER U WITH HORN AND TILDE
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x002b, 0x006f, 0, 0}, 0x1ee1 }, // U1EE1 # LATIN SMALL LETTER O WITH HORN AND TILDE
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x002b, 0x0075, 0, 0}, 0x1eef }, // U1EEF # LATIN SMALL LETTER U WITH HORN AND TILDE
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x0041, 0, 0, 0}, 0x00c3 }, // U00C3 # LATIN CAPITAL LETTER A WITH TILDE
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x0045, 0, 0, 0}, 0x1ebc }, // U1EBC # LATIN CAPITAL LETTER E WITH TILDE
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x0049, 0, 0, 0}, 0x0128 }, // U0128 # LATIN CAPITAL LETTER I WITH TILDE
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x004e, 0, 0, 0}, 0x00d1 }, // U00D1 # LATIN CAPITAL LETTER N WITH TILDE
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x004f, 0, 0, 0}, 0x00d5 }, // U00D5 # LATIN CAPITAL LETTER O WITH TILDE
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x0055, 0, 0, 0}, 0x0168 }, // U0168 # LATIN CAPITAL LETTER U WITH TILDE
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x0055, 0x0041, 0, 0}, 0x1eb4 }, // U1EB4 # LATIN CAPITAL LETTER A WITH BREVE AND TILDE
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x0055, 0x0061, 0, 0}, 0x1eb5 }, // U1EB5 # LATIN SMALL LETTER A WITH BREVE AND TILDE
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x0056, 0, 0, 0}, 0x1e7c }, // U1E7C # LATIN CAPITAL LETTER V WITH TILDE
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x0059, 0, 0, 0}, 0x1ef8 }, // U1EF8 # LATIN CAPITAL LETTER Y WITH TILDE
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x005e, 0x0041, 0, 0}, 0x1eaa }, // U1EAA # LATIN CAPITAL LETTER A WITH CIRCUMFLEX AND TILDE
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x005e, 0x0045, 0, 0}, 0x1ec4 }, // U1EC4 # LATIN CAPITAL LETTER E WITH CIRCUMFLEX AND TILDE
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x005e, 0x004f, 0, 0}, 0x1ed6 }, // U1ED6 # LATIN CAPITAL LETTER O WITH CIRCUMFLEX AND TILDE
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x005e, 0x0061, 0, 0}, 0x1eab }, // U1EAB # LATIN SMALL LETTER A WITH CIRCUMFLEX AND TILDE
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x005e, 0x0065, 0, 0}, 0x1ec5 }, // U1EC5 # LATIN SMALL LETTER E WITH CIRCUMFLEX AND TILDE
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x005e, 0x006f, 0, 0}, 0x1ed7 }, // U1ED7 # LATIN SMALL LETTER O WITH CIRCUMFLEX AND TILDE
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x0061, 0, 0, 0}, 0x00e3 }, // U00E3 # LATIN SMALL LETTER A WITH TILDE
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x0062, 0x0041, 0, 0}, 0x1eb4 }, // U1EB4 # LATIN CAPITAL LETTER A WITH BREVE AND TILDE
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x0062, 0x0061, 0, 0}, 0x1eb5 }, // U1EB5 # LATIN SMALL LETTER A WITH BREVE AND TILDE
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x0065, 0, 0, 0}, 0x1ebd }, // U1EBD # LATIN SMALL LETTER E WITH TILDE
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x0069, 0, 0, 0}, 0x0129 }, // U0129 # LATIN SMALL LETTER I WITH TILDE
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x006e, 0, 0, 0}, 0x00f1 }, // U00F1 # LATIN SMALL LETTER N WITH TILDE
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x006f, 0, 0, 0}, 0x00f5 }, // U00F5 # LATIN SMALL LETTER O WITH TILDE
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x0075, 0, 0, 0}, 0x0169 }, // U0169 # LATIN SMALL LETTER U WITH TILDE
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x0076, 0, 0, 0}, 0x1e7d }, // U1E7D # LATIN SMALL LETTER V WITH TILDE
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x0079, 0, 0, 0}, 0x1ef9 }, // U1EF9 # LATIN SMALL LETTER Y WITH TILDE
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x00c2, 0, 0, 0}, 0x1eaa }, // U1EAA # LATIN CAPITAL LETTER A WITH CIRCUMFLEX AND TILDE
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x00ca, 0, 0, 0}, 0x1ec4 }, // U1EC4 # LATIN CAPITAL LETTER E WITH CIRCUMFLEX AND TILDE
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x00d4, 0, 0, 0}, 0x1ed6 }, // U1ED6 # LATIN CAPITAL LETTER O WITH CIRCUMFLEX AND TILDE
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x00e2, 0, 0, 0}, 0x1eab }, // U1EAB # LATIN SMALL LETTER A WITH CIRCUMFLEX AND TILDE
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x00ea, 0, 0, 0}, 0x1ec5 }, // U1EC5 # LATIN SMALL LETTER E WITH CIRCUMFLEX AND TILDE
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x00f4, 0, 0, 0}, 0x1ed7 }, // U1ED7 # LATIN SMALL LETTER O WITH CIRCUMFLEX AND TILDE
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x0102, 0, 0, 0}, 0x1eb4 }, // U1EB4 # LATIN CAPITAL LETTER A WITH BREVE AND TILDE
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x0103, 0, 0, 0}, 0x1eb5 }, // U1EB5 # LATIN SMALL LETTER A WITH BREVE AND TILDE
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x0313, 0x0391, 0, 0}, 0x1f0e }, // U1F0E # GREEK CAPITAL LETTER ALPHA WITH PSILI AND PERISPOMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x0313, 0x0397, 0, 0}, 0x1f2e }, // U1F2E # GREEK CAPITAL LETTER ETA WITH PSILI AND PERISPOMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x0313, 0x0399, 0, 0}, 0x1f3e }, // U1F3E # GREEK CAPITAL LETTER IOTA WITH PSILI AND PERISPOMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x0313, 0x03a9, 0, 0}, 0x1f6e }, // U1F6E # GREEK CAPITAL LETTER OMEGA WITH PSILI AND PERISPOMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x0313, 0x03b1, 0, 0}, 0x1f06 }, // U1F06 # GREEK SMALL LETTER ALPHA WITH PSILI AND PERISPOMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x0313, 0x03b7, 0, 0}, 0x1f26 }, // U1F26 # GREEK SMALL LETTER ETA WITH PSILI AND PERISPOMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x0313, 0x03b9, 0, 0}, 0x1f36 }, // U1F36 # GREEK SMALL LETTER IOTA WITH PSILI AND PERISPOMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x0313, 0x03c5, 0, 0}, 0x1f56 }, // U1F56 # GREEK SMALL LETTER UPSILON WITH PSILI AND PERISPOMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x0313, 0x03c9, 0, 0}, 0x1f66 }, // U1F66 # GREEK SMALL LETTER OMEGA WITH PSILI AND PERISPOMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x0314, 0x0391, 0, 0}, 0x1f0f }, // U1F0F # GREEK CAPITAL LETTER ALPHA WITH DASIA AND PERISPOMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x0314, 0x0397, 0, 0}, 0x1f2f }, // U1F2F # GREEK CAPITAL LETTER ETA WITH DASIA AND PERISPOMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x0314, 0x0399, 0, 0}, 0x1f3f }, // U1F3F # GREEK CAPITAL LETTER IOTA WITH DASIA AND PERISPOMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x0314, 0x03a5, 0, 0}, 0x1f5f }, // U1F5F # GREEK CAPITAL LETTER UPSILON WITH DASIA AND PERISPOMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x0314, 0x03a9, 0, 0}, 0x1f6f }, // U1F6F # GREEK CAPITAL LETTER OMEGA WITH DASIA AND PERISPOMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x0314, 0x03b1, 0, 0}, 0x1f07 }, // U1F07 # GREEK SMALL LETTER ALPHA WITH DASIA AND PERISPOMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x0314, 0x03b7, 0, 0}, 0x1f27 }, // U1F27 # GREEK SMALL LETTER ETA WITH DASIA AND PERISPOMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x0314, 0x03b9, 0, 0}, 0x1f37 }, // U1F37 # GREEK SMALL LETTER IOTA WITH DASIA AND PERISPOMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x0314, 0x03c5, 0, 0}, 0x1f57 }, // U1F57 # GREEK SMALL LETTER UPSILON WITH DASIA AND PERISPOMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x0314, 0x03c9, 0, 0}, 0x1f67 }, // U1F67 # GREEK SMALL LETTER OMEGA WITH DASIA AND PERISPOMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x03b1, 0, 0, 0}, 0x1fb6 }, // U1FB6 # GREEK SMALL LETTER ALPHA WITH PERISPOMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x03b7, 0, 0, 0}, 0x1fc6 }, // U1FC6 # GREEK SMALL LETTER ETA WITH PERISPOMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x03b9, 0, 0, 0}, 0x1fd6 }, // U1FD6 # GREEK SMALL LETTER IOTA WITH PERISPOMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x03c5, 0, 0, 0}, 0x1fe6 }, // U1FE6 # GREEK SMALL LETTER UPSILON WITH PERISPOMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x03c9, 0, 0, 0}, 0x1ff6 }, // U1FF6 # GREEK SMALL LETTER OMEGA WITH PERISPOMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x03ca, 0, 0, 0}, 0x1fd7 }, // U1FD7 # GREEK SMALL LETTER IOTA WITH DIALYTIKA AND PERISPOMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x03cb, 0, 0, 0}, 0x1fe7 }, // U1FE7 # GREEK SMALL LETTER UPSILON WITH DIALYTIKA AND PERISPOMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x1f00, 0, 0, 0}, 0x1f06 }, // U1F06 # GREEK SMALL LETTER ALPHA WITH PSILI AND PERISPOMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x1f01, 0, 0, 0}, 0x1f07 }, // U1F07 # GREEK SMALL LETTER ALPHA WITH DASIA AND PERISPOMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x1f08, 0, 0, 0}, 0x1f0e }, // U1F0E # GREEK CAPITAL LETTER ALPHA WITH PSILI AND PERISPOMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x1f09, 0, 0, 0}, 0x1f0f }, // U1F0F # GREEK CAPITAL LETTER ALPHA WITH DASIA AND PERISPOMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x1f20, 0, 0, 0}, 0x1f26 }, // U1F26 # GREEK SMALL LETTER ETA WITH PSILI AND PERISPOMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x1f21, 0, 0, 0}, 0x1f27 }, // U1F27 # GREEK SMALL LETTER ETA WITH DASIA AND PERISPOMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x1f28, 0, 0, 0}, 0x1f2e }, // U1F2E # GREEK CAPITAL LETTER ETA WITH PSILI AND PERISPOMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x1f29, 0, 0, 0}, 0x1f2f }, // U1F2F # GREEK CAPITAL LETTER ETA WITH DASIA AND PERISPOMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x1f30, 0, 0, 0}, 0x1f36 }, // U1F36 # GREEK SMALL LETTER IOTA WITH PSILI AND PERISPOMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x1f31, 0, 0, 0}, 0x1f37 }, // U1F37 # GREEK SMALL LETTER IOTA WITH DASIA AND PERISPOMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x1f38, 0, 0, 0}, 0x1f3e }, // U1F3E # GREEK CAPITAL LETTER IOTA WITH PSILI AND PERISPOMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x1f39, 0, 0, 0}, 0x1f3f }, // U1F3F # GREEK CAPITAL LETTER IOTA WITH DASIA AND PERISPOMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x1f50, 0, 0, 0}, 0x1f56 }, // U1F56 # GREEK SMALL LETTER UPSILON WITH PSILI AND PERISPOMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x1f51, 0, 0, 0}, 0x1f57 }, // U1F57 # GREEK SMALL LETTER UPSILON WITH DASIA AND PERISPOMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x1f59, 0, 0, 0}, 0x1f5f }, // U1F5F # GREEK CAPITAL LETTER UPSILON WITH DASIA AND PERISPOMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x1f60, 0, 0, 0}, 0x1f66 }, // U1F66 # GREEK SMALL LETTER OMEGA WITH PSILI AND PERISPOMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x1f61, 0, 0, 0}, 0x1f67 }, // U1F67 # GREEK SMALL LETTER OMEGA WITH DASIA AND PERISPOMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x1f68, 0, 0, 0}, 0x1f6e }, // U1F6E # GREEK CAPITAL LETTER OMEGA WITH PSILI AND PERISPOMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, 0x1f69, 0, 0, 0}, 0x1f6f }, // U1F6F # GREEK CAPITAL LETTER OMEGA WITH DASIA AND PERISPOMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, UNITIZE(TQt::Key_Dead_Circumflex), 0x0041, 0, 0}, 0x1eaa }, // U1EAA # LATIN CAPITAL LETTER A WITH CIRCUMFLEX AND TILDE
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, UNITIZE(TQt::Key_Dead_Circumflex), 0x0045, 0, 0}, 0x1ec4 }, // U1EC4 # LATIN CAPITAL LETTER E WITH CIRCUMFLEX AND TILDE
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, UNITIZE(TQt::Key_Dead_Circumflex), 0x004f, 0, 0}, 0x1ed6 }, // U1ED6 # LATIN CAPITAL LETTER O WITH CIRCUMFLEX AND TILDE
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, UNITIZE(TQt::Key_Dead_Circumflex), 0x0061, 0, 0}, 0x1eab }, // U1EAB # LATIN SMALL LETTER A WITH CIRCUMFLEX AND TILDE
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, UNITIZE(TQt::Key_Dead_Circumflex), 0x0065, 0, 0}, 0x1ec5 }, // U1EC5 # LATIN SMALL LETTER E WITH CIRCUMFLEX AND TILDE
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, UNITIZE(TQt::Key_Dead_Circumflex), 0x006f, 0, 0}, 0x1ed7 }, // U1ED7 # LATIN SMALL LETTER O WITH CIRCUMFLEX AND TILDE
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, UNITIZE(TQt::Key_Dead_Breve), 0x0041, 0, 0}, 0x1eb4 }, // U1EB4 # LATIN CAPITAL LETTER A WITH BREVE AND TILDE
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, UNITIZE(TQt::Key_Dead_Breve), 0x0061, 0, 0}, 0x1eb5 }, // U1EB5 # LATIN SMALL LETTER A WITH BREVE AND TILDE
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, UNITIZE(TQt::Key_Dead_Diaeresis), 0x03b9, 0, 0}, 0x1fd7 }, // U1FD7 # GREEK SMALL LETTER IOTA WITH DIALYTIKA AND PERISPOMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, UNITIZE(TQt::Key_Dead_Diaeresis), 0x03c5, 0, 0}, 0x1fe7 }, // U1FE7 # GREEK SMALL LETTER UPSILON WITH DIALYTIKA AND PERISPOMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, UNITIZE(TQt::Key_Dead_Horn), 0x004f, 0, 0}, 0x1ee0 }, // U1EE0 # LATIN CAPITAL LETTER O WITH HORN AND TILDE
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, UNITIZE(TQt::Key_Dead_Horn), 0x0055, 0, 0}, 0x1eee }, // U1EEE # LATIN CAPITAL LETTER U WITH HORN AND TILDE
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, UNITIZE(TQt::Key_Dead_Horn), 0x006f, 0, 0}, 0x1ee1 }, // U1EE1 # LATIN SMALL LETTER O WITH HORN AND TILDE
    { {UNITIZE(TQt::Key_Multi_key), 0x007e, UNITIZE(TQt::Key_Dead_Horn), 0x0075, 0, 0}, 0x1eef }, // U1EEF # LATIN SMALL LETTER U WITH HORN AND TILDE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x00a8, 0x0000, 0, 0, 0}, 0x0385 }, // U0385 # GREEK DIALYTIKA TONOS
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x00a8, 0x0000, 0, 0, 0}, 0x1fed }, // U1FED # GREEK DIALYTIKA AND VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x00a8, 0x0027, 0, 0, 0}, 0x0385 }, // U0385 # GREEK DIALYTIKA TONOS
    { {UNITIZE(TQt::Key_Multi_key), 0x00a8, 0x0060, 0, 0, 0}, 0x1fed }, // U1FED # GREEK DIALYTIKA AND VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x00a8, 0x007e, 0, 0, 0}, 0x1fc1 }, // U1FC1 # GREEK DIALYTIKA AND PERISPOMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x00a8, 0x00b4, 0, 0, 0}, 0x0385 }, // U0385 # GREEK DIALYTIKA TONOS
    { {UNITIZE(TQt::Key_Multi_key), 0x00a8, 0x0342, 0, 0, 0}, 0x1fc1 }, // U1FC1 # GREEK DIALYTIKA AND PERISPOMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x00a8, UNITIZE(TQt::Key_Dead_Grave), 0, 0, 0}, 0x1fed }, // U1FED # GREEK DIALYTIKA AND VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x00a8, UNITIZE(TQt::Key_Dead_Acute), 0, 0, 0}, 0x0385 }, // U0385 # GREEK DIALYTIKA TONOS
    { {UNITIZE(TQt::Key_Multi_key), 0x00a8, UNITIZE(TQt::Key_Dead_Tilde), 0, 0, 0}, 0x1fc1 }, // U1FC1 # GREEK DIALYTIKA AND PERISPOMENI
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x00af, 0x0000, 0x004c, 0, 0}, 0x1e38 }, // U1E38 # LATIN CAPITAL LETTER L WITH DOT BELOW AND MACRON
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x00af, 0x0000, 0x004f, 0, 0}, 0x022c }, // U022C # LATIN CAPITAL LETTER O WITH TILDE AND MACRON
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x00af, 0x0000, 0x0052, 0, 0}, 0x1e5c }, // U1E5C # LATIN CAPITAL LETTER R WITH DOT BELOW AND MACRON
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x00af, 0x0000, 0x006c, 0, 0}, 0x1e39 }, // U1E39 # LATIN SMALL LETTER L WITH DOT BELOW AND MACRON
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x00af, 0x0000, 0x006f, 0, 0}, 0x022d }, // U022D # LATIN SMALL LETTER O WITH TILDE AND MACRON
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x00af, 0x0000, 0x0072, 0, 0}, 0x1e5d }, // U1E5D # LATIN SMALL LETTER R WITH DOT BELOW AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x00af, 0x0021, 0x004c, 0, 0}, 0x1e38 }, // U1E38 # LATIN CAPITAL LETTER L WITH DOT BELOW AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x00af, 0x0021, 0x0052, 0, 0}, 0x1e5c }, // U1E5C # LATIN CAPITAL LETTER R WITH DOT BELOW AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x00af, 0x0021, 0x006c, 0, 0}, 0x1e39 }, // U1E39 # LATIN SMALL LETTER L WITH DOT BELOW AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x00af, 0x0021, 0x0072, 0, 0}, 0x1e5d }, // U1E5D # LATIN SMALL LETTER R WITH DOT BELOW AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x00af, 0x0022, 0x0041, 0, 0}, 0x01de }, // U01DE # LATIN CAPITAL LETTER A WITH DIAERESIS AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x00af, 0x0022, 0x004f, 0, 0}, 0x022a }, // U022A # LATIN CAPITAL LETTER O WITH DIAERESIS AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x00af, 0x0022, 0x0055, 0, 0}, 0x01d5 }, // U01D5 # LATIN CAPITAL LETTER U WITH DIAERESIS AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x00af, 0x0022, 0x0061, 0, 0}, 0x01df }, // U01DF # LATIN SMALL LETTER A WITH DIAERESIS AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x00af, 0x0022, 0x006f, 0, 0}, 0x022b }, // U022B # LATIN SMALL LETTER O WITH DIAERESIS AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x00af, 0x0022, 0x0075, 0, 0}, 0x01d6 }, // U01D6 # LATIN SMALL LETTER U WITH DIAERESIS AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x00af, 0x002e, 0x0041, 0, 0}, 0x01e0 }, // U01E0 # LATIN CAPITAL LETTER A WITH DOT ABOVE AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x00af, 0x002e, 0x004f, 0, 0}, 0x0230 }, // U0230 # LATIN CAPITAL LETTER O WITH DOT ABOVE AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x00af, 0x002e, 0x0061, 0, 0}, 0x01e1 }, // U01E1 # LATIN SMALL LETTER A WITH DOT ABOVE AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x00af, 0x002e, 0x006f, 0, 0}, 0x0231 }, // U0231 # LATIN SMALL LETTER O WITH DOT ABOVE AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x00af, 0x003b, 0x004f, 0, 0}, 0x01ec }, // U01EC # LATIN CAPITAL LETTER O WITH OGONEK AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x00af, 0x003b, 0x006f, 0, 0}, 0x01ed }, // U01ED # LATIN SMALL LETTER O WITH OGONEK AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x00af, 0x0041, 0, 0, 0}, 0x0100 }, // U0100 # LATIN CAPITAL LETTER A WITH MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x00af, 0x0045, 0, 0, 0}, 0x0112 }, // U0112 # LATIN CAPITAL LETTER E WITH MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x00af, 0x0047, 0, 0, 0}, 0x1e20 }, // U1E20 # LATIN CAPITAL LETTER G WITH MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x00af, 0x0049, 0, 0, 0}, 0x012a }, // U012A # LATIN CAPITAL LETTER I WITH MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x00af, 0x004f, 0, 0, 0}, 0x014c }, // U014C # LATIN CAPITAL LETTER O WITH MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x00af, 0x0055, 0, 0, 0}, 0x016a }, // U016A # LATIN CAPITAL LETTER U WITH MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x00af, 0x0059, 0, 0, 0}, 0x0232 }, // U0232 # LATIN CAPITAL LETTER Y WITH MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x00af, 0x0061, 0, 0, 0}, 0x0101 }, // U0101 # LATIN SMALL LETTER A WITH MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x00af, 0x0065, 0, 0, 0}, 0x0113 }, // U0113 # LATIN SMALL LETTER E WITH MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x00af, 0x0067, 0, 0, 0}, 0x1e21 }, // U1E21 # LATIN SMALL LETTER G WITH MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x00af, 0x0069, 0, 0, 0}, 0x012b }, // U012B # LATIN SMALL LETTER I WITH MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x00af, 0x006f, 0, 0, 0}, 0x014d }, // U014D # LATIN SMALL LETTER O WITH MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x00af, 0x0075, 0, 0, 0}, 0x016b }, // U016B # LATIN SMALL LETTER U WITH MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x00af, 0x0079, 0, 0, 0}, 0x0233 }, // U0233 # LATIN SMALL LETTER Y WITH MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x00af, 0x007e, 0x004f, 0, 0}, 0x022c }, // U022C # LATIN CAPITAL LETTER O WITH TILDE AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x00af, 0x007e, 0x006f, 0, 0}, 0x022d }, // U022D # LATIN SMALL LETTER O WITH TILDE AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x00af, 0x00c4, 0, 0, 0}, 0x01de }, // U01DE # LATIN CAPITAL LETTER A WITH DIAERESIS AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x00af, 0x00c6, 0, 0, 0}, 0x01e2 }, // U01E2 # LATIN CAPITAL LETTER AE WITH MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x00af, 0x00d5, 0, 0, 0}, 0x022c }, // U022C # LATIN CAPITAL LETTER O WITH TILDE AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x00af, 0x00d6, 0, 0, 0}, 0x022a }, // U022A # LATIN CAPITAL LETTER O WITH DIAERESIS AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x00af, 0x00dc, 0, 0, 0}, 0x01d5 }, // U01D5 # LATIN CAPITAL LETTER U WITH DIAERESIS AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x00af, 0x00e4, 0, 0, 0}, 0x01df }, // U01DF # LATIN SMALL LETTER A WITH DIAERESIS AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x00af, 0x00e6, 0, 0, 0}, 0x01e3 }, // U01E3 # LATIN SMALL LETTER AE WITH MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x00af, 0x00f5, 0, 0, 0}, 0x022d }, // U022D # LATIN SMALL LETTER O WITH TILDE AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x00af, 0x00f6, 0, 0, 0}, 0x022b }, // U022B # LATIN SMALL LETTER O WITH DIAERESIS AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x00af, 0x00fc, 0, 0, 0}, 0x01d6 }, // U01D6 # LATIN SMALL LETTER U WITH DIAERESIS AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x00af, 0x01ea, 0, 0, 0}, 0x01ec }, // U01EC # LATIN CAPITAL LETTER O WITH OGONEK AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x00af, 0x01eb, 0, 0, 0}, 0x01ed }, // U01ED # LATIN SMALL LETTER O WITH OGONEK AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x00af, 0x0226, 0, 0, 0}, 0x01e0 }, // U01E0 # LATIN CAPITAL LETTER A WITH DOT ABOVE AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x00af, 0x0227, 0, 0, 0}, 0x01e1 }, // U01E1 # LATIN SMALL LETTER A WITH DOT ABOVE AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x00af, 0x022e, 0, 0, 0}, 0x0230 }, // U0230 # LATIN CAPITAL LETTER O WITH DOT ABOVE AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x00af, 0x022f, 0, 0, 0}, 0x0231 }, // U0231 # LATIN SMALL LETTER O WITH DOT ABOVE AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x00af, 0x0391, 0, 0, 0}, 0x1fb9 }, // U1FB9 # GREEK CAPITAL LETTER ALPHA WITH MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x00af, 0x0399, 0, 0, 0}, 0x1fd9 }, // U1FD9 # GREEK CAPITAL LETTER IOTA WITH MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x00af, 0x03a5, 0, 0, 0}, 0x1fe9 }, // U1FE9 # GREEK CAPITAL LETTER UPSILON WITH MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x00af, 0x03b1, 0, 0, 0}, 0x1fb1 }, // U1FB1 # GREEK SMALL LETTER ALPHA WITH MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x00af, 0x03b9, 0, 0, 0}, 0x1fd1 }, // U1FD1 # GREEK SMALL LETTER IOTA WITH MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x00af, 0x03c5, 0, 0, 0}, 0x1fe1 }, // U1FE1 # GREEK SMALL LETTER UPSILON WITH MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x00af, 0x0418, 0, 0, 0}, 0x04e2 }, // U04E2 # CYRILLIC CAPITAL LETTER I WITH MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x00af, 0x0423, 0, 0, 0}, 0x04ee }, // U04EE # CYRILLIC CAPITAL LETTER U WITH MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x00af, 0x0438, 0, 0, 0}, 0x04e3 }, // U04E3 # CYRILLIC SMALL LETTER I WITH MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x00af, 0x0443, 0, 0, 0}, 0x04ef }, // U04EF # CYRILLIC SMALL LETTER U WITH MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x00af, 0x1e36, 0, 0, 0}, 0x1e38 }, // U1E38 # LATIN CAPITAL LETTER L WITH DOT BELOW AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x00af, 0x1e37, 0, 0, 0}, 0x1e39 }, // U1E39 # LATIN SMALL LETTER L WITH DOT BELOW AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x00af, 0x1e5a, 0, 0, 0}, 0x1e5c }, // U1E5C # LATIN CAPITAL LETTER R WITH DOT BELOW AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x00af, 0x1e5b, 0, 0, 0}, 0x1e5d }, // U1E5D # LATIN SMALL LETTER R WITH DOT BELOW AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x00af, UNITIZE(TQt::Key_Dead_Tilde), 0x004f, 0, 0}, 0x022c }, // U022C # LATIN CAPITAL LETTER O WITH TILDE AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x00af, UNITIZE(TQt::Key_Dead_Tilde), 0x006f, 0, 0}, 0x022d }, // U022D # LATIN SMALL LETTER O WITH TILDE AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x00af, UNITIZE(TQt::Key_Dead_Abovedot), 0x0041, 0, 0}, 0x01e0 }, // U01E0 # LATIN CAPITAL LETTER A WITH DOT ABOVE AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x00af, UNITIZE(TQt::Key_Dead_Abovedot), 0x004f, 0, 0}, 0x0230 }, // U0230 # LATIN CAPITAL LETTER O WITH DOT ABOVE AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x00af, UNITIZE(TQt::Key_Dead_Abovedot), 0x0061, 0, 0}, 0x01e1 }, // U01E1 # LATIN SMALL LETTER A WITH DOT ABOVE AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x00af, UNITIZE(TQt::Key_Dead_Abovedot), 0x006f, 0, 0}, 0x0231 }, // U0231 # LATIN SMALL LETTER O WITH DOT ABOVE AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x00af, UNITIZE(TQt::Key_Dead_Diaeresis), 0x0041, 0, 0}, 0x01de }, // U01DE # LATIN CAPITAL LETTER A WITH DIAERESIS AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x00af, UNITIZE(TQt::Key_Dead_Diaeresis), 0x004f, 0, 0}, 0x022a }, // U022A # LATIN CAPITAL LETTER O WITH DIAERESIS AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x00af, UNITIZE(TQt::Key_Dead_Diaeresis), 0x0055, 0, 0}, 0x01d5 }, // U01D5 # LATIN CAPITAL LETTER U WITH DIAERESIS AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x00af, UNITIZE(TQt::Key_Dead_Diaeresis), 0x0061, 0, 0}, 0x01df }, // U01DF # LATIN SMALL LETTER A WITH DIAERESIS AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x00af, UNITIZE(TQt::Key_Dead_Diaeresis), 0x006f, 0, 0}, 0x022b }, // U022B # LATIN SMALL LETTER O WITH DIAERESIS AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x00af, UNITIZE(TQt::Key_Dead_Diaeresis), 0x0075, 0, 0}, 0x01d6 }, // U01D6 # LATIN SMALL LETTER U WITH DIAERESIS AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x00af, UNITIZE(TQt::Key_Dead_Ogonek), 0x004f, 0, 0}, 0x01ec }, // U01EC # LATIN CAPITAL LETTER O WITH OGONEK AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x00af, UNITIZE(TQt::Key_Dead_Ogonek), 0x006f, 0, 0}, 0x01ed }, // U01ED # LATIN SMALL LETTER O WITH OGONEK AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x00af, UNITIZE(TQt::Key_Dead_Belowdot), 0x004c, 0, 0}, 0x1e38 }, // U1E38 # LATIN CAPITAL LETTER L WITH DOT BELOW AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x00af, UNITIZE(TQt::Key_Dead_Belowdot), 0x0052, 0, 0}, 0x1e5c }, // U1E5C # LATIN CAPITAL LETTER R WITH DOT BELOW AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x00af, UNITIZE(TQt::Key_Dead_Belowdot), 0x006c, 0, 0}, 0x1e39 }, // U1E39 # LATIN SMALL LETTER L WITH DOT BELOW AND MACRON
    { {UNITIZE(TQt::Key_Multi_key), 0x00af, UNITIZE(TQt::Key_Dead_Belowdot), 0x0072, 0, 0}, 0x1e5d }, // U1E5D # LATIN SMALL LETTER R WITH DOT BELOW AND MACRON
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0000, 0, 0, 0}, 0x1edb }, // U1EDB # LATIN SMALL LETTER O WITH HORN AND ACUTE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0000, 0, 0, 0}, 0x1ee9 }, // U1EE9 # LATIN SMALL LETTER U WITH HORN AND ACUTE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0000, 0, 0, 0}, 0x1ee8 }, // U1EE8 # LATIN CAPITAL LETTER U WITH HORN AND ACUTE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0000, 0, 0, 0}, 0x1eda }, // U1EDA # LATIN CAPITAL LETTER O WITH HORN AND ACUTE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0000, 0x004f, 0, 0}, 0x01fe }, // U01FE # LATIN CAPITAL LETTER O WITH STROKE AND ACUTE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0000, 0x004f, 0, 0}, 0x1e4c }, // U1E4C # LATIN CAPITAL LETTER O WITH TILDE AND ACUTE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0000, 0x0055, 0, 0}, 0x1e78 }, // U1E78 # LATIN CAPITAL LETTER U WITH TILDE AND ACUTE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0000, 0x006f, 0, 0}, 0x01ff }, // U01FF # LATIN SMALL LETTER O WITH STROKE AND ACUTE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0000, 0x006f, 0, 0}, 0x1e4d }, // U1E4D # LATIN SMALL LETTER O WITH TILDE AND ACUTE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0000, 0x0075, 0, 0}, 0x1e79 }, // U1E79 # LATIN SMALL LETTER U WITH TILDE AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0022, 0x0049, 0, 0}, 0x1e2e }, // U1E2E # LATIN CAPITAL LETTER I WITH DIAERESIS AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0022, 0x0055, 0, 0}, 0x01d7 }, // U01D7 # LATIN CAPITAL LETTER U WITH DIAERESIS AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0022, 0x0069, 0, 0}, 0x1e2f }, // U1E2F # LATIN SMALL LETTER I WITH DIAERESIS AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0022, 0x0075, 0, 0}, 0x01d8 }, // U01D8 # LATIN SMALL LETTER U WITH DIAERESIS AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0022, 0x03b9, 0, 0}, 0x0390 }, // U0390 # GREEK SMALL LETTER IOTA WITH DIALYTIKA AND TONOS
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0022, 0x03c5, 0, 0}, 0x03b0 }, // U03B0 # GREEK SMALL LETTER UPSILON WITH DIALYTIKA AND TONOS
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0028, 0x0391, 0, 0}, 0x1f0d }, // U1F0D # GREEK CAPITAL LETTER ALPHA WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0028, 0x0395, 0, 0}, 0x1f1d }, // U1F1D # GREEK CAPITAL LETTER EPSILON WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0028, 0x0397, 0, 0}, 0x1f2d }, // U1F2D # GREEK CAPITAL LETTER ETA WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0028, 0x0399, 0, 0}, 0x1f3d }, // U1F3D # GREEK CAPITAL LETTER IOTA WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0028, 0x039f, 0, 0}, 0x1f4d }, // U1F4D # GREEK CAPITAL LETTER OMICRON WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0028, 0x03a5, 0, 0}, 0x1f5d }, // U1F5D # GREEK CAPITAL LETTER UPSILON WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0028, 0x03a9, 0, 0}, 0x1f6d }, // U1F6D # GREEK CAPITAL LETTER OMEGA WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0028, 0x03b1, 0, 0}, 0x1f05 }, // U1F05 # GREEK SMALL LETTER ALPHA WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0028, 0x03b5, 0, 0}, 0x1f15 }, // U1F15 # GREEK SMALL LETTER EPSILON WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0028, 0x03b7, 0, 0}, 0x1f25 }, // U1F25 # GREEK SMALL LETTER ETA WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0028, 0x03b9, 0, 0}, 0x1f35 }, // U1F35 # GREEK SMALL LETTER IOTA WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0028, 0x03bf, 0, 0}, 0x1f45 }, // U1F45 # GREEK SMALL LETTER OMICRON WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0028, 0x03c5, 0, 0}, 0x1f55 }, // U1F55 # GREEK SMALL LETTER UPSILON WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0028, 0x03c9, 0, 0}, 0x1f65 }, // U1F65 # GREEK SMALL LETTER OMEGA WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0029, 0x0391, 0, 0}, 0x1f0c }, // U1F0C # GREEK CAPITAL LETTER ALPHA WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0029, 0x0395, 0, 0}, 0x1f1c }, // U1F1C # GREEK CAPITAL LETTER EPSILON WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0029, 0x0397, 0, 0}, 0x1f2c }, // U1F2C # GREEK CAPITAL LETTER ETA WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0029, 0x0399, 0, 0}, 0x1f3c }, // U1F3C # GREEK CAPITAL LETTER IOTA WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0029, 0x039f, 0, 0}, 0x1f4c }, // U1F4C # GREEK CAPITAL LETTER OMICRON WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0029, 0x03a9, 0, 0}, 0x1f6c }, // U1F6C # GREEK CAPITAL LETTER OMEGA WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0029, 0x03b1, 0, 0}, 0x1f04 }, // U1F04 # GREEK SMALL LETTER ALPHA WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0029, 0x03b5, 0, 0}, 0x1f14 }, // U1F14 # GREEK SMALL LETTER EPSILON WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0029, 0x03b7, 0, 0}, 0x1f24 }, // U1F24 # GREEK SMALL LETTER ETA WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0029, 0x03b9, 0, 0}, 0x1f34 }, // U1F34 # GREEK SMALL LETTER IOTA WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0029, 0x03bf, 0, 0}, 0x1f44 }, // U1F44 # GREEK SMALL LETTER OMICRON WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0029, 0x03c5, 0, 0}, 0x1f54 }, // U1F54 # GREEK SMALL LETTER UPSILON WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0029, 0x03c9, 0, 0}, 0x1f64 }, // U1F64 # GREEK SMALL LETTER OMEGA WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x002b, 0x004f, 0, 0}, 0x1eda }, // U1EDA # LATIN CAPITAL LETTER O WITH HORN AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x002b, 0x0055, 0, 0}, 0x1ee8 }, // U1EE8 # LATIN CAPITAL LETTER U WITH HORN AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x002b, 0x006f, 0, 0}, 0x1edb }, // U1EDB # LATIN SMALL LETTER O WITH HORN AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x002b, 0x0075, 0, 0}, 0x1ee9 }, // U1EE9 # LATIN SMALL LETTER U WITH HORN AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x002c, 0x0043, 0, 0}, 0x1e08 }, // U1E08 # LATIN CAPITAL LETTER C WITH CEDILLA AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x002c, 0x0063, 0, 0}, 0x1e09 }, // U1E09 # LATIN SMALL LETTER C WITH CEDILLA AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x002f, 0x004f, 0, 0}, 0x01fe }, // U01FE # LATIN CAPITAL LETTER O WITH STROKE AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x002f, 0x006f, 0, 0}, 0x01ff }, // U01FF # LATIN SMALL LETTER O WITH STROKE AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0041, 0, 0, 0}, 0x00c1 }, // U00C1 # LATIN CAPITAL LETTER A WITH ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0043, 0, 0, 0}, 0x0106 }, // U0106 # LATIN CAPITAL LETTER C WITH ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0045, 0, 0, 0}, 0x00c9 }, // U00C9 # LATIN CAPITAL LETTER E WITH ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0047, 0, 0, 0}, 0x01f4 }, // U01F4 # LATIN CAPITAL LETTER G WITH ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0049, 0, 0, 0}, 0x00cd }, // U00CD # LATIN CAPITAL LETTER I WITH ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x004b, 0, 0, 0}, 0x1e30 }, // U1E30 # LATIN CAPITAL LETTER K WITH ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x004c, 0, 0, 0}, 0x0139 }, // U0139 # LATIN CAPITAL LETTER L WITH ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x004d, 0, 0, 0}, 0x1e3e }, // U1E3E # LATIN CAPITAL LETTER M WITH ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x004e, 0, 0, 0}, 0x0143 }, // U0143 # LATIN CAPITAL LETTER N WITH ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x004f, 0, 0, 0}, 0x00d3 }, // U00D3 # LATIN CAPITAL LETTER O WITH ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0050, 0, 0, 0}, 0x1e54 }, // U1E54 # LATIN CAPITAL LETTER P WITH ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0052, 0, 0, 0}, 0x0154 }, // U0154 # LATIN CAPITAL LETTER R WITH ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0053, 0, 0, 0}, 0x015a }, // U015A # LATIN CAPITAL LETTER S WITH ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0055, 0, 0, 0}, 0x00da }, // U00DA # LATIN CAPITAL LETTER U WITH ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0055, 0x0041, 0, 0}, 0x1eae }, // U1EAE # LATIN CAPITAL LETTER A WITH BREVE AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0055, 0x0061, 0, 0}, 0x1eaf }, // U1EAF # LATIN SMALL LETTER A WITH BREVE AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0057, 0, 0, 0}, 0x1e82 }, // U1E82 # LATIN CAPITAL LETTER W WITH ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0059, 0, 0, 0}, 0x00dd }, // U00DD # LATIN CAPITAL LETTER Y WITH ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x005a, 0, 0, 0}, 0x0179 }, // U0179 # LATIN CAPITAL LETTER Z WITH ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x005e, 0x0041, 0, 0}, 0x1ea4 }, // U1EA4 # LATIN CAPITAL LETTER A WITH CIRCUMFLEX AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x005e, 0x0045, 0, 0}, 0x1ebe }, // U1EBE # LATIN CAPITAL LETTER E WITH CIRCUMFLEX AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x005e, 0x004f, 0, 0}, 0x1ed0 }, // U1ED0 # LATIN CAPITAL LETTER O WITH CIRCUMFLEX AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x005e, 0x0061, 0, 0}, 0x1ea5 }, // U1EA5 # LATIN SMALL LETTER A WITH CIRCUMFLEX AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x005e, 0x0065, 0, 0}, 0x1ebf }, // U1EBF # LATIN SMALL LETTER E WITH CIRCUMFLEX AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x005e, 0x006f, 0, 0}, 0x1ed1 }, // U1ED1 # LATIN SMALL LETTER O WITH CIRCUMFLEX AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x005f, 0x0045, 0, 0}, 0x1e16 }, // U1E16 # LATIN CAPITAL LETTER E WITH MACRON AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x005f, 0x004f, 0, 0}, 0x1e52 }, // U1E52 # LATIN CAPITAL LETTER O WITH MACRON AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x005f, 0x0065, 0, 0}, 0x1e17 }, // U1E17 # LATIN SMALL LETTER E WITH MACRON AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x005f, 0x006f, 0, 0}, 0x1e53 }, // U1E53 # LATIN SMALL LETTER O WITH MACRON AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0061, 0, 0, 0}, 0x00e1 }, // U00E1 # LATIN SMALL LETTER A WITH ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0062, 0x0041, 0, 0}, 0x1eae }, // U1EAE # LATIN CAPITAL LETTER A WITH BREVE AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0062, 0x0061, 0, 0}, 0x1eaf }, // U1EAF # LATIN SMALL LETTER A WITH BREVE AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0063, 0, 0, 0}, 0x0107 }, // U0107 # LATIN SMALL LETTER C WITH ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0065, 0, 0, 0}, 0x00e9 }, // U00E9 # LATIN SMALL LETTER E WITH ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0067, 0, 0, 0}, 0x01f5 }, // U01F5 # LATIN SMALL LETTER G WITH ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0069, 0, 0, 0}, 0x00ed }, // U00ED # LATIN SMALL LETTER I WITH ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x006b, 0, 0, 0}, 0x1e31 }, // U1E31 # LATIN SMALL LETTER K WITH ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x006c, 0, 0, 0}, 0x013a }, // U013A # LATIN SMALL LETTER L WITH ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x006d, 0, 0, 0}, 0x1e3f }, // U1E3F # LATIN SMALL LETTER M WITH ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x006e, 0, 0, 0}, 0x0144 }, // U0144 # LATIN SMALL LETTER N WITH ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x006f, 0, 0, 0}, 0x00f3 }, // U00F3 # LATIN SMALL LETTER O WITH ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x006f, 0x0041, 0, 0}, 0x01fa }, // U01FA # LATIN CAPITAL LETTER A WITH RING ABOVE AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x006f, 0x0061, 0, 0}, 0x01fb }, // U01FB # LATIN SMALL LETTER A WITH RING ABOVE AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0070, 0, 0, 0}, 0x1e55 }, // U1E55 # LATIN SMALL LETTER P WITH ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0072, 0, 0, 0}, 0x0155 }, // U0155 # LATIN SMALL LETTER R WITH ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0073, 0, 0, 0}, 0x015b }, // U015B # LATIN SMALL LETTER S WITH ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0075, 0, 0, 0}, 0x00fa }, // U00FA # LATIN SMALL LETTER U WITH ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0077, 0, 0, 0}, 0x1e83 }, // U1E83 # LATIN SMALL LETTER W WITH ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0079, 0, 0, 0}, 0x00fd }, // U00FD # LATIN SMALL LETTER Y WITH ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x007a, 0, 0, 0}, 0x017a }, // U017A # LATIN SMALL LETTER Z WITH ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x007e, 0x004f, 0, 0}, 0x1e4c }, // U1E4C # LATIN CAPITAL LETTER O WITH TILDE AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x007e, 0x0055, 0, 0}, 0x1e78 }, // U1E78 # LATIN CAPITAL LETTER U WITH TILDE AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x007e, 0x006f, 0, 0}, 0x1e4d }, // U1E4D # LATIN SMALL LETTER O WITH TILDE AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x007e, 0x0075, 0, 0}, 0x1e79 }, // U1E79 # LATIN SMALL LETTER U WITH TILDE AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x00af, 0x0045, 0, 0}, 0x1e16 }, // U1E16 # LATIN CAPITAL LETTER E WITH MACRON AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x00af, 0x004f, 0, 0}, 0x1e52 }, // U1E52 # LATIN CAPITAL LETTER O WITH MACRON AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x00af, 0x0065, 0, 0}, 0x1e17 }, // U1E17 # LATIN SMALL LETTER E WITH MACRON AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x00af, 0x006f, 0, 0}, 0x1e53 }, // U1E53 # LATIN SMALL LETTER O WITH MACRON AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x00c2, 0, 0, 0}, 0x1ea4 }, // U1EA4 # LATIN CAPITAL LETTER A WITH CIRCUMFLEX AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x00c5, 0, 0, 0}, 0x01fa }, // U01FA # LATIN CAPITAL LETTER A WITH RING ABOVE AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x00c6, 0, 0, 0}, 0x01fc }, // U01FC # LATIN CAPITAL LETTER AE WITH ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x00c7, 0, 0, 0}, 0x1e08 }, // U1E08 # LATIN CAPITAL LETTER C WITH CEDILLA AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x00ca, 0, 0, 0}, 0x1ebe }, // U1EBE # LATIN CAPITAL LETTER E WITH CIRCUMFLEX AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x00cf, 0, 0, 0}, 0x1e2e }, // U1E2E # LATIN CAPITAL LETTER I WITH DIAERESIS AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x00d4, 0, 0, 0}, 0x1ed0 }, // U1ED0 # LATIN CAPITAL LETTER O WITH CIRCUMFLEX AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x00d5, 0, 0, 0}, 0x1e4c }, // U1E4C # LATIN CAPITAL LETTER O WITH TILDE AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x00d8, 0, 0, 0}, 0x01fe }, // U01FE # LATIN CAPITAL LETTER O WITH STROKE AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x00dc, 0, 0, 0}, 0x01d7 }, // U01D7 # LATIN CAPITAL LETTER U WITH DIAERESIS AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x00e2, 0, 0, 0}, 0x1ea5 }, // U1EA5 # LATIN SMALL LETTER A WITH CIRCUMFLEX AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x00e5, 0, 0, 0}, 0x01fb }, // U01FB # LATIN SMALL LETTER A WITH RING ABOVE AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x00e6, 0, 0, 0}, 0x01fd }, // U01FD # LATIN SMALL LETTER AE WITH ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x00e7, 0, 0, 0}, 0x1e09 }, // U1E09 # LATIN SMALL LETTER C WITH CEDILLA AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x00ea, 0, 0, 0}, 0x1ebf }, // U1EBF # LATIN SMALL LETTER E WITH CIRCUMFLEX AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x00ef, 0, 0, 0}, 0x1e2f }, // U1E2F # LATIN SMALL LETTER I WITH DIAERESIS AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x00f4, 0, 0, 0}, 0x1ed1 }, // U1ED1 # LATIN SMALL LETTER O WITH CIRCUMFLEX AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x00f5, 0, 0, 0}, 0x1e4d }, // U1E4D # LATIN SMALL LETTER O WITH TILDE AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x00f8, 0, 0, 0}, 0x01ff }, // U01FF # LATIN SMALL LETTER O WITH STROKE AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x00fc, 0, 0, 0}, 0x01d8 }, // U01D8 # LATIN SMALL LETTER U WITH DIAERESIS AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0102, 0, 0, 0}, 0x1eae }, // U1EAE # LATIN CAPITAL LETTER A WITH BREVE AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0103, 0, 0, 0}, 0x1eaf }, // U1EAF # LATIN SMALL LETTER A WITH BREVE AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0112, 0, 0, 0}, 0x1e16 }, // U1E16 # LATIN CAPITAL LETTER E WITH MACRON AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0113, 0, 0, 0}, 0x1e17 }, // U1E17 # LATIN SMALL LETTER E WITH MACRON AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x014c, 0, 0, 0}, 0x1e52 }, // U1E52 # LATIN CAPITAL LETTER O WITH MACRON AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x014d, 0, 0, 0}, 0x1e53 }, // U1E53 # LATIN SMALL LETTER O WITH MACRON AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0168, 0, 0, 0}, 0x1e78 }, // U1E78 # LATIN CAPITAL LETTER U WITH TILDE AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0169, 0, 0, 0}, 0x1e79 }, // U1E79 # LATIN SMALL LETTER U WITH TILDE AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0313, 0x0391, 0, 0}, 0x1f0c }, // U1F0C # GREEK CAPITAL LETTER ALPHA WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0313, 0x0395, 0, 0}, 0x1f1c }, // U1F1C # GREEK CAPITAL LETTER EPSILON WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0313, 0x0397, 0, 0}, 0x1f2c }, // U1F2C # GREEK CAPITAL LETTER ETA WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0313, 0x0399, 0, 0}, 0x1f3c }, // U1F3C # GREEK CAPITAL LETTER IOTA WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0313, 0x039f, 0, 0}, 0x1f4c }, // U1F4C # GREEK CAPITAL LETTER OMICRON WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0313, 0x03a9, 0, 0}, 0x1f6c }, // U1F6C # GREEK CAPITAL LETTER OMEGA WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0313, 0x03b1, 0, 0}, 0x1f04 }, // U1F04 # GREEK SMALL LETTER ALPHA WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0313, 0x03b5, 0, 0}, 0x1f14 }, // U1F14 # GREEK SMALL LETTER EPSILON WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0313, 0x03b7, 0, 0}, 0x1f24 }, // U1F24 # GREEK SMALL LETTER ETA WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0313, 0x03b9, 0, 0}, 0x1f34 }, // U1F34 # GREEK SMALL LETTER IOTA WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0313, 0x03bf, 0, 0}, 0x1f44 }, // U1F44 # GREEK SMALL LETTER OMICRON WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0313, 0x03c5, 0, 0}, 0x1f54 }, // U1F54 # GREEK SMALL LETTER UPSILON WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0313, 0x03c9, 0, 0}, 0x1f64 }, // U1F64 # GREEK SMALL LETTER OMEGA WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0314, 0x0391, 0, 0}, 0x1f0d }, // U1F0D # GREEK CAPITAL LETTER ALPHA WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0314, 0x0395, 0, 0}, 0x1f1d }, // U1F1D # GREEK CAPITAL LETTER EPSILON WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0314, 0x0397, 0, 0}, 0x1f2d }, // U1F2D # GREEK CAPITAL LETTER ETA WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0314, 0x0399, 0, 0}, 0x1f3d }, // U1F3D # GREEK CAPITAL LETTER IOTA WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0314, 0x039f, 0, 0}, 0x1f4d }, // U1F4D # GREEK CAPITAL LETTER OMICRON WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0314, 0x03a5, 0, 0}, 0x1f5d }, // U1F5D # GREEK CAPITAL LETTER UPSILON WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0314, 0x03a9, 0, 0}, 0x1f6d }, // U1F6D # GREEK CAPITAL LETTER OMEGA WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0314, 0x03b1, 0, 0}, 0x1f05 }, // U1F05 # GREEK SMALL LETTER ALPHA WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0314, 0x03b5, 0, 0}, 0x1f15 }, // U1F15 # GREEK SMALL LETTER EPSILON WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0314, 0x03b7, 0, 0}, 0x1f25 }, // U1F25 # GREEK SMALL LETTER ETA WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0314, 0x03b9, 0, 0}, 0x1f35 }, // U1F35 # GREEK SMALL LETTER IOTA WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0314, 0x03bf, 0, 0}, 0x1f45 }, // U1F45 # GREEK SMALL LETTER OMICRON WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0314, 0x03c5, 0, 0}, 0x1f55 }, // U1F55 # GREEK SMALL LETTER UPSILON WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0314, 0x03c9, 0, 0}, 0x1f65 }, // U1F65 # GREEK SMALL LETTER OMEGA WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0391, 0, 0, 0}, 0x0386 }, // U0386 # GREEK CAPITAL LETTER ALPHA WITH TONOS
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0395, 0, 0, 0}, 0x0388 }, // U0388 # GREEK CAPITAL LETTER EPSILON WITH TONOS
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0397, 0, 0, 0}, 0x0389 }, // U0389 # GREEK CAPITAL LETTER ETA WITH TONOS
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0399, 0, 0, 0}, 0x038a }, // U038A # GREEK CAPITAL LETTER IOTA WITH TONOS
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x039f, 0, 0, 0}, 0x038c }, // U038C # GREEK CAPITAL LETTER OMICRON WITH TONOS
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x03a5, 0, 0, 0}, 0x038e }, // U038E # GREEK CAPITAL LETTER UPSILON WITH TONOS
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x03a9, 0, 0, 0}, 0x038f }, // U038F # GREEK CAPITAL LETTER OMEGA WITH TONOS
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x03b1, 0, 0, 0}, 0x03ac }, // U03AC # GREEK SMALL LETTER ALPHA WITH TONOS
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x03b5, 0, 0, 0}, 0x03ad }, // U03AD # GREEK SMALL LETTER EPSILON WITH TONOS
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x03b7, 0, 0, 0}, 0x03ae }, // U03AE # GREEK SMALL LETTER ETA WITH TONOS
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x03b9, 0, 0, 0}, 0x03af }, // U03AF # GREEK SMALL LETTER IOTA WITH TONOS
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x03bf, 0, 0, 0}, 0x03cc }, // U03CC # GREEK SMALL LETTER OMICRON WITH TONOS
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x03c5, 0, 0, 0}, 0x03cd }, // U03CD # GREEK SMALL LETTER UPSILON WITH TONOS
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x03c9, 0, 0, 0}, 0x03ce }, // U03CE # GREEK SMALL LETTER OMEGA WITH TONOS
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x03ca, 0, 0, 0}, 0x0390 }, // U0390 # GREEK SMALL LETTER IOTA WITH DIALYTIKA AND TONOS
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x03cb, 0, 0, 0}, 0x03b0 }, // U03B0 # GREEK SMALL LETTER UPSILON WITH DIALYTIKA AND TONOS
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x03d2, 0, 0, 0}, 0x03d3 }, // U03D3 # GREEK UPSILON WITH ACUTE AND HOOK SYMBOL
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0413, 0, 0, 0}, 0x0403 }, // U0403 # CYRILLIC CAPITAL LETTER GJE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x041a, 0, 0, 0}, 0x040c }, // U040C # CYRILLIC CAPITAL LETTER KJE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0433, 0, 0, 0}, 0x0453 }, // U0453 # CYRILLIC SMALL LETTER GJE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x043a, 0, 0, 0}, 0x045c }, // U045C # CYRILLIC SMALL LETTER KJE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x1f00, 0, 0, 0}, 0x1f04 }, // U1F04 # GREEK SMALL LETTER ALPHA WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x1f01, 0, 0, 0}, 0x1f05 }, // U1F05 # GREEK SMALL LETTER ALPHA WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x1f08, 0, 0, 0}, 0x1f0c }, // U1F0C # GREEK CAPITAL LETTER ALPHA WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x1f09, 0, 0, 0}, 0x1f0d }, // U1F0D # GREEK CAPITAL LETTER ALPHA WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x1f10, 0, 0, 0}, 0x1f14 }, // U1F14 # GREEK SMALL LETTER EPSILON WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x1f11, 0, 0, 0}, 0x1f15 }, // U1F15 # GREEK SMALL LETTER EPSILON WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x1f18, 0, 0, 0}, 0x1f1c }, // U1F1C # GREEK CAPITAL LETTER EPSILON WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x1f19, 0, 0, 0}, 0x1f1d }, // U1F1D # GREEK CAPITAL LETTER EPSILON WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x1f20, 0, 0, 0}, 0x1f24 }, // U1F24 # GREEK SMALL LETTER ETA WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x1f21, 0, 0, 0}, 0x1f25 }, // U1F25 # GREEK SMALL LETTER ETA WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x1f28, 0, 0, 0}, 0x1f2c }, // U1F2C # GREEK CAPITAL LETTER ETA WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x1f29, 0, 0, 0}, 0x1f2d }, // U1F2D # GREEK CAPITAL LETTER ETA WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x1f30, 0, 0, 0}, 0x1f34 }, // U1F34 # GREEK SMALL LETTER IOTA WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x1f31, 0, 0, 0}, 0x1f35 }, // U1F35 # GREEK SMALL LETTER IOTA WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x1f38, 0, 0, 0}, 0x1f3c }, // U1F3C # GREEK CAPITAL LETTER IOTA WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x1f39, 0, 0, 0}, 0x1f3d }, // U1F3D # GREEK CAPITAL LETTER IOTA WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x1f40, 0, 0, 0}, 0x1f44 }, // U1F44 # GREEK SMALL LETTER OMICRON WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x1f41, 0, 0, 0}, 0x1f45 }, // U1F45 # GREEK SMALL LETTER OMICRON WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x1f48, 0, 0, 0}, 0x1f4c }, // U1F4C # GREEK CAPITAL LETTER OMICRON WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x1f49, 0, 0, 0}, 0x1f4d }, // U1F4D # GREEK CAPITAL LETTER OMICRON WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x1f50, 0, 0, 0}, 0x1f54 }, // U1F54 # GREEK SMALL LETTER UPSILON WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x1f51, 0, 0, 0}, 0x1f55 }, // U1F55 # GREEK SMALL LETTER UPSILON WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x1f59, 0, 0, 0}, 0x1f5d }, // U1F5D # GREEK CAPITAL LETTER UPSILON WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x1f60, 0, 0, 0}, 0x1f64 }, // U1F64 # GREEK SMALL LETTER OMEGA WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x1f61, 0, 0, 0}, 0x1f65 }, // U1F65 # GREEK SMALL LETTER OMEGA WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x1f68, 0, 0, 0}, 0x1f6c }, // U1F6C # GREEK CAPITAL LETTER OMEGA WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x1f69, 0, 0, 0}, 0x1f6d }, // U1F6D # GREEK CAPITAL LETTER OMEGA WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, UNITIZE(TQt::Key_Dead_Circumflex), 0x0041, 0, 0}, 0x1ea4 }, // U1EA4 # LATIN CAPITAL LETTER A WITH CIRCUMFLEX AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, UNITIZE(TQt::Key_Dead_Circumflex), 0x0045, 0, 0}, 0x1ebe }, // U1EBE # LATIN CAPITAL LETTER E WITH CIRCUMFLEX AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, UNITIZE(TQt::Key_Dead_Circumflex), 0x004f, 0, 0}, 0x1ed0 }, // U1ED0 # LATIN CAPITAL LETTER O WITH CIRCUMFLEX AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, UNITIZE(TQt::Key_Dead_Circumflex), 0x0061, 0, 0}, 0x1ea5 }, // U1EA5 # LATIN SMALL LETTER A WITH CIRCUMFLEX AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, UNITIZE(TQt::Key_Dead_Circumflex), 0x0065, 0, 0}, 0x1ebf }, // U1EBF # LATIN SMALL LETTER E WITH CIRCUMFLEX AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, UNITIZE(TQt::Key_Dead_Circumflex), 0x006f, 0, 0}, 0x1ed1 }, // U1ED1 # LATIN SMALL LETTER O WITH CIRCUMFLEX AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, UNITIZE(TQt::Key_Dead_Tilde), 0x004f, 0, 0}, 0x1e4c }, // U1E4C # LATIN CAPITAL LETTER O WITH TILDE AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, UNITIZE(TQt::Key_Dead_Tilde), 0x0055, 0, 0}, 0x1e78 }, // U1E78 # LATIN CAPITAL LETTER U WITH TILDE AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, UNITIZE(TQt::Key_Dead_Tilde), 0x006f, 0, 0}, 0x1e4d }, // U1E4D # LATIN SMALL LETTER O WITH TILDE AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, UNITIZE(TQt::Key_Dead_Tilde), 0x0075, 0, 0}, 0x1e79 }, // U1E79 # LATIN SMALL LETTER U WITH TILDE AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, UNITIZE(TQt::Key_Dead_Macron), 0x0045, 0, 0}, 0x1e16 }, // U1E16 # LATIN CAPITAL LETTER E WITH MACRON AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, UNITIZE(TQt::Key_Dead_Macron), 0x004f, 0, 0}, 0x1e52 }, // U1E52 # LATIN CAPITAL LETTER O WITH MACRON AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, UNITIZE(TQt::Key_Dead_Macron), 0x0065, 0, 0}, 0x1e17 }, // U1E17 # LATIN SMALL LETTER E WITH MACRON AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, UNITIZE(TQt::Key_Dead_Macron), 0x006f, 0, 0}, 0x1e53 }, // U1E53 # LATIN SMALL LETTER O WITH MACRON AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, UNITIZE(TQt::Key_Dead_Breve), 0x0041, 0, 0}, 0x1eae }, // U1EAE # LATIN CAPITAL LETTER A WITH BREVE AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, UNITIZE(TQt::Key_Dead_Breve), 0x0061, 0, 0}, 0x1eaf }, // U1EAF # LATIN SMALL LETTER A WITH BREVE AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, UNITIZE(TQt::Key_Dead_Diaeresis), 0x0049, 0, 0}, 0x1e2e }, // U1E2E # LATIN CAPITAL LETTER I WITH DIAERESIS AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, UNITIZE(TQt::Key_Dead_Diaeresis), 0x0055, 0, 0}, 0x01d7 }, // U01D7 # LATIN CAPITAL LETTER U WITH DIAERESIS AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, UNITIZE(TQt::Key_Dead_Diaeresis), 0x0069, 0, 0}, 0x1e2f }, // U1E2F # LATIN SMALL LETTER I WITH DIAERESIS AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, UNITIZE(TQt::Key_Dead_Diaeresis), 0x0075, 0, 0}, 0x01d8 }, // U01D8 # LATIN SMALL LETTER U WITH DIAERESIS AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, UNITIZE(TQt::Key_Dead_Diaeresis), 0x03b9, 0, 0}, 0x0390 }, // U0390 # GREEK SMALL LETTER IOTA WITH DIALYTIKA AND TONOS
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, UNITIZE(TQt::Key_Dead_Diaeresis), 0x03c5, 0, 0}, 0x03b0 }, // U03B0 # GREEK SMALL LETTER UPSILON WITH DIALYTIKA AND TONOS
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, UNITIZE(TQt::Key_Dead_Abovering), 0x0041, 0, 0}, 0x01fa }, // U01FA # LATIN CAPITAL LETTER A WITH RING ABOVE AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, UNITIZE(TQt::Key_Dead_Abovering), 0x0061, 0, 0}, 0x01fb }, // U01FB # LATIN SMALL LETTER A WITH RING ABOVE AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, UNITIZE(TQt::Key_Dead_Cedilla), 0x0043, 0, 0}, 0x1e08 }, // U1E08 # LATIN CAPITAL LETTER C WITH CEDILLA AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, UNITIZE(TQt::Key_Dead_Cedilla), 0x0063, 0, 0}, 0x1e09 }, // U1E09 # LATIN SMALL LETTER C WITH CEDILLA AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, UNITIZE(TQt::Key_Dead_Horn), 0x004f, 0, 0}, 0x1eda }, // U1EDA # LATIN CAPITAL LETTER O WITH HORN AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, UNITIZE(TQt::Key_Dead_Horn), 0x0055, 0, 0}, 0x1ee8 }, // U1EE8 # LATIN CAPITAL LETTER U WITH HORN AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, UNITIZE(TQt::Key_Dead_Horn), 0x006f, 0, 0}, 0x1edb }, // U1EDB # LATIN SMALL LETTER O WITH HORN AND ACUTE
    { {UNITIZE(TQt::Key_Multi_key), 0x00b4, UNITIZE(TQt::Key_Dead_Horn), 0x0075, 0, 0}, 0x1ee9 }, // U1EE9 # LATIN SMALL LETTER U WITH HORN AND ACUTE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0000, 0x0028, 0x0391, 0}, 0x1f8d }, // U1F8D # GREEK CAPITAL LETTER ALPHA WITH DASIA AND OXIA AND PROSGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0000, 0x0028, 0x0391, 0}, 0x1f8b }, // U1F8B # GREEK CAPITAL LETTER ALPHA WITH DASIA AND VARIA AND PROSGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0000, 0x0028, 0x0397, 0}, 0x1f9b }, // U1F9B # GREEK CAPITAL LETTER ETA WITH DASIA AND VARIA AND PROSGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0000, 0x0028, 0x0397, 0}, 0x1f9d }, // U1F9D # GREEK CAPITAL LETTER ETA WITH DASIA AND OXIA AND PROSGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0000, 0x0028, 0x03a9, 0}, 0x1fab }, // U1FAB # GREEK CAPITAL LETTER OMEGA WITH DASIA AND VARIA AND PROSGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0000, 0x0028, 0x03a9, 0}, 0x1fad }, // U1FAD # GREEK CAPITAL LETTER OMEGA WITH DASIA AND OXIA AND PROSGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0000, 0x0028, 0x03b1, 0}, 0x1f85 }, // U1F85 # GREEK SMALL LETTER ALPHA WITH DASIA AND OXIA AND YPOGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0000, 0x0028, 0x03b1, 0}, 0x1f83 }, // U1F83 # GREEK SMALL LETTER ALPHA WITH DASIA AND VARIA AND YPOGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0000, 0x0028, 0x03b7, 0}, 0x1f93 }, // U1F93 # GREEK SMALL LETTER ETA WITH DASIA AND VARIA AND YPOGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0000, 0x0028, 0x03b7, 0}, 0x1f95 }, // U1F95 # GREEK SMALL LETTER ETA WITH DASIA AND OXIA AND YPOGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0000, 0x0028, 0x03c9, 0}, 0x1fa3 }, // U1FA3 # GREEK SMALL LETTER OMEGA WITH DASIA AND VARIA AND YPOGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0000, 0x0028, 0x03c9, 0}, 0x1fa5 }, // U1FA5 # GREEK SMALL LETTER OMEGA WITH DASIA AND OXIA AND YPOGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0000, 0x0029, 0x0391, 0}, 0x1f8c }, // U1F8C # GREEK CAPITAL LETTER ALPHA WITH PSILI AND OXIA AND PROSGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0000, 0x0029, 0x0391, 0}, 0x1f8a }, // U1F8A # GREEK CAPITAL LETTER ALPHA WITH PSILI AND VARIA AND PROSGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0000, 0x0029, 0x0397, 0}, 0x1f9c }, // U1F9C # GREEK CAPITAL LETTER ETA WITH PSILI AND OXIA AND PROSGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0000, 0x0029, 0x0397, 0}, 0x1f9a }, // U1F9A # GREEK CAPITAL LETTER ETA WITH PSILI AND VARIA AND PROSGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0000, 0x0029, 0x03a9, 0}, 0x1faa }, // U1FAA # GREEK CAPITAL LETTER OMEGA WITH PSILI AND VARIA AND PROSGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0000, 0x0029, 0x03a9, 0}, 0x1fac }, // U1FAC # GREEK CAPITAL LETTER OMEGA WITH PSILI AND OXIA AND PROSGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0000, 0x0029, 0x03b1, 0}, 0x1f84 }, // U1F84 # GREEK SMALL LETTER ALPHA WITH PSILI AND OXIA AND YPOGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0000, 0x0029, 0x03b1, 0}, 0x1f82 }, // U1F82 # GREEK SMALL LETTER ALPHA WITH PSILI AND VARIA AND YPOGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0000, 0x0029, 0x03b7, 0}, 0x1f92 }, // U1F92 # GREEK SMALL LETTER ETA WITH PSILI AND VARIA AND YPOGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0000, 0x0029, 0x03b7, 0}, 0x1f94 }, // U1F94 # GREEK SMALL LETTER ETA WITH PSILI AND OXIA AND YPOGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0000, 0x0029, 0x03c9, 0}, 0x1fa4 }, // U1FA4 # GREEK SMALL LETTER OMEGA WITH PSILI AND OXIA AND YPOGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0000, 0x0029, 0x03c9, 0}, 0x1fa2 }, // U1FA2 # GREEK SMALL LETTER OMEGA WITH PSILI AND VARIA AND YPOGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0000, 0x0313, 0x0391, 0}, 0x1f8c }, // U1F8C # GREEK CAPITAL LETTER ALPHA WITH PSILI AND OXIA AND PROSGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0000, 0x0313, 0x0391, 0}, 0x1f8a }, // U1F8A # GREEK CAPITAL LETTER ALPHA WITH PSILI AND VARIA AND PROSGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0000, 0x0313, 0x0397, 0}, 0x1f9a }, // U1F9A # GREEK CAPITAL LETTER ETA WITH PSILI AND VARIA AND PROSGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0000, 0x0313, 0x0397, 0}, 0x1f9c }, // U1F9C # GREEK CAPITAL LETTER ETA WITH PSILI AND OXIA AND PROSGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0000, 0x0313, 0x03a9, 0}, 0x1faa }, // U1FAA # GREEK CAPITAL LETTER OMEGA WITH PSILI AND VARIA AND PROSGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0000, 0x0313, 0x03a9, 0}, 0x1fac }, // U1FAC # GREEK CAPITAL LETTER OMEGA WITH PSILI AND OXIA AND PROSGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0000, 0x0313, 0x03b1, 0}, 0x1f82 }, // U1F82 # GREEK SMALL LETTER ALPHA WITH PSILI AND VARIA AND YPOGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0000, 0x0313, 0x03b1, 0}, 0x1f84 }, // U1F84 # GREEK SMALL LETTER ALPHA WITH PSILI AND OXIA AND YPOGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0000, 0x0313, 0x03b7, 0}, 0x1f92 }, // U1F92 # GREEK SMALL LETTER ETA WITH PSILI AND VARIA AND YPOGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0000, 0x0313, 0x03b7, 0}, 0x1f94 }, // U1F94 # GREEK SMALL LETTER ETA WITH PSILI AND OXIA AND YPOGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0000, 0x0313, 0x03c9, 0}, 0x1fa4 }, // U1FA4 # GREEK SMALL LETTER OMEGA WITH PSILI AND OXIA AND YPOGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0000, 0x0313, 0x03c9, 0}, 0x1fa2 }, // U1FA2 # GREEK SMALL LETTER OMEGA WITH PSILI AND VARIA AND YPOGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0000, 0x0314, 0x0391, 0}, 0x1f8b }, // U1F8B # GREEK CAPITAL LETTER ALPHA WITH DASIA AND VARIA AND PROSGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0000, 0x0314, 0x0391, 0}, 0x1f8d }, // U1F8D # GREEK CAPITAL LETTER ALPHA WITH DASIA AND OXIA AND PROSGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0000, 0x0314, 0x0397, 0}, 0x1f9b }, // U1F9B # GREEK CAPITAL LETTER ETA WITH DASIA AND VARIA AND PROSGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0000, 0x0314, 0x0397, 0}, 0x1f9d }, // U1F9D # GREEK CAPITAL LETTER ETA WITH DASIA AND OXIA AND PROSGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0000, 0x0314, 0x03a9, 0}, 0x1fab }, // U1FAB # GREEK CAPITAL LETTER OMEGA WITH DASIA AND VARIA AND PROSGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0000, 0x0314, 0x03a9, 0}, 0x1fad }, // U1FAD # GREEK CAPITAL LETTER OMEGA WITH DASIA AND OXIA AND PROSGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0000, 0x0314, 0x03b1, 0}, 0x1f85 }, // U1F85 # GREEK SMALL LETTER ALPHA WITH DASIA AND OXIA AND YPOGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0000, 0x0314, 0x03b1, 0}, 0x1f83 }, // U1F83 # GREEK SMALL LETTER ALPHA WITH DASIA AND VARIA AND YPOGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0000, 0x0314, 0x03b7, 0}, 0x1f95 }, // U1F95 # GREEK SMALL LETTER ETA WITH DASIA AND OXIA AND YPOGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0000, 0x0314, 0x03b7, 0}, 0x1f93 }, // U1F93 # GREEK SMALL LETTER ETA WITH DASIA AND VARIA AND YPOGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0000, 0x0314, 0x03c9, 0}, 0x1fa5 }, // U1FA5 # GREEK SMALL LETTER OMEGA WITH DASIA AND OXIA AND YPOGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0000, 0x0314, 0x03c9, 0}, 0x1fa3 }, // U1FA3 # GREEK SMALL LETTER OMEGA WITH DASIA AND VARIA AND YPOGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0000, 0x03b1, 0, 0}, 0x1fb4 }, // U1FB4 # GREEK SMALL LETTER ALPHA WITH OXIA AND YPOGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0000, 0x03b1, 0, 0}, 0x1fb2 }, // U1FB2 # GREEK SMALL LETTER ALPHA WITH VARIA AND YPOGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0000, 0x03b7, 0, 0}, 0x1fc4 }, // U1FC4 # GREEK SMALL LETTER ETA WITH OXIA AND YPOGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0000, 0x03b7, 0, 0}, 0x1fc2 }, // U1FC2 # GREEK SMALL LETTER ETA WITH VARIA AND YPOGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0000, 0x03c9, 0, 0}, 0x1ff4 }, // U1FF4 # GREEK SMALL LETTER OMEGA WITH OXIA AND YPOGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0000, 0x03c9, 0, 0}, 0x1ff2 }, // U1FF2 # GREEK SMALL LETTER OMEGA WITH VARIA AND YPOGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0000, 0x1f00, 0, 0}, 0x1f84 }, // U1F84 # GREEK SMALL LETTER ALPHA WITH PSILI AND OXIA AND YPOGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0000, 0x1f00, 0, 0}, 0x1f82 }, // U1F82 # GREEK SMALL LETTER ALPHA WITH PSILI AND VARIA AND YPOGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0000, 0x1f01, 0, 0}, 0x1f83 }, // U1F83 # GREEK SMALL LETTER ALPHA WITH DASIA AND VARIA AND YPOGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0000, 0x1f01, 0, 0}, 0x1f85 }, // U1F85 # GREEK SMALL LETTER ALPHA WITH DASIA AND OXIA AND YPOGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0000, 0x1f08, 0, 0}, 0x1f8c }, // U1F8C # GREEK CAPITAL LETTER ALPHA WITH PSILI AND OXIA AND PROSGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0000, 0x1f08, 0, 0}, 0x1f8a }, // U1F8A # GREEK CAPITAL LETTER ALPHA WITH PSILI AND VARIA AND PROSGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0000, 0x1f09, 0, 0}, 0x1f8b }, // U1F8B # GREEK CAPITAL LETTER ALPHA WITH DASIA AND VARIA AND PROSGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0000, 0x1f09, 0, 0}, 0x1f8d }, // U1F8D # GREEK CAPITAL LETTER ALPHA WITH DASIA AND OXIA AND PROSGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0000, 0x1f20, 0, 0}, 0x1f92 }, // U1F92 # GREEK SMALL LETTER ETA WITH PSILI AND VARIA AND YPOGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0000, 0x1f20, 0, 0}, 0x1f94 }, // U1F94 # GREEK SMALL LETTER ETA WITH PSILI AND OXIA AND YPOGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0000, 0x1f21, 0, 0}, 0x1f93 }, // U1F93 # GREEK SMALL LETTER ETA WITH DASIA AND VARIA AND YPOGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0000, 0x1f21, 0, 0}, 0x1f95 }, // U1F95 # GREEK SMALL LETTER ETA WITH DASIA AND OXIA AND YPOGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0000, 0x1f28, 0, 0}, 0x1f9c }, // U1F9C # GREEK CAPITAL LETTER ETA WITH PSILI AND OXIA AND PROSGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0000, 0x1f28, 0, 0}, 0x1f9a }, // U1F9A # GREEK CAPITAL LETTER ETA WITH PSILI AND VARIA AND PROSGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0000, 0x1f29, 0, 0}, 0x1f9b }, // U1F9B # GREEK CAPITAL LETTER ETA WITH DASIA AND VARIA AND PROSGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0000, 0x1f29, 0, 0}, 0x1f9d }, // U1F9D # GREEK CAPITAL LETTER ETA WITH DASIA AND OXIA AND PROSGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0000, 0x1f60, 0, 0}, 0x1fa2 }, // U1FA2 # GREEK SMALL LETTER OMEGA WITH PSILI AND VARIA AND YPOGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0000, 0x1f60, 0, 0}, 0x1fa4 }, // U1FA4 # GREEK SMALL LETTER OMEGA WITH PSILI AND OXIA AND YPOGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0000, 0x1f61, 0, 0}, 0x1fa5 }, // U1FA5 # GREEK SMALL LETTER OMEGA WITH DASIA AND OXIA AND YPOGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0000, 0x1f61, 0, 0}, 0x1fa3 }, // U1FA3 # GREEK SMALL LETTER OMEGA WITH DASIA AND VARIA AND YPOGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0000, 0x1f68, 0, 0}, 0x1fac }, // U1FAC # GREEK CAPITAL LETTER OMEGA WITH PSILI AND OXIA AND PROSGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0000, 0x1f68, 0, 0}, 0x1faa }, // U1FAA # GREEK CAPITAL LETTER OMEGA WITH PSILI AND VARIA AND PROSGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0000, 0x1f69, 0, 0}, 0x1fad }, // U1FAD # GREEK CAPITAL LETTER OMEGA WITH DASIA AND OXIA AND PROSGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0000, 0x1f69, 0, 0}, 0x1fab }, // U1FAB # GREEK CAPITAL LETTER OMEGA WITH DASIA AND VARIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0027, 0x0028, 0x0391, 0}, 0x1f8d }, // U1F8D # GREEK CAPITAL LETTER ALPHA WITH DASIA AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0027, 0x0028, 0x0397, 0}, 0x1f9d }, // U1F9D # GREEK CAPITAL LETTER ETA WITH DASIA AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0027, 0x0028, 0x03a9, 0}, 0x1fad }, // U1FAD # GREEK CAPITAL LETTER OMEGA WITH DASIA AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0027, 0x0028, 0x03b1, 0}, 0x1f85 }, // U1F85 # GREEK SMALL LETTER ALPHA WITH DASIA AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0027, 0x0028, 0x03b7, 0}, 0x1f95 }, // U1F95 # GREEK SMALL LETTER ETA WITH DASIA AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0027, 0x0028, 0x03c9, 0}, 0x1fa5 }, // U1FA5 # GREEK SMALL LETTER OMEGA WITH DASIA AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0027, 0x0029, 0x0391, 0}, 0x1f8c }, // U1F8C # GREEK CAPITAL LETTER ALPHA WITH PSILI AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0027, 0x0029, 0x0397, 0}, 0x1f9c }, // U1F9C # GREEK CAPITAL LETTER ETA WITH PSILI AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0027, 0x0029, 0x03a9, 0}, 0x1fac }, // U1FAC # GREEK CAPITAL LETTER OMEGA WITH PSILI AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0027, 0x0029, 0x03b1, 0}, 0x1f84 }, // U1F84 # GREEK SMALL LETTER ALPHA WITH PSILI AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0027, 0x0029, 0x03b7, 0}, 0x1f94 }, // U1F94 # GREEK SMALL LETTER ETA WITH PSILI AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0027, 0x0029, 0x03c9, 0}, 0x1fa4 }, // U1FA4 # GREEK SMALL LETTER OMEGA WITH PSILI AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0027, 0x0313, 0x0391, 0}, 0x1f8c }, // U1F8C # GREEK CAPITAL LETTER ALPHA WITH PSILI AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0027, 0x0313, 0x0397, 0}, 0x1f9c }, // U1F9C # GREEK CAPITAL LETTER ETA WITH PSILI AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0027, 0x0313, 0x03a9, 0}, 0x1fac }, // U1FAC # GREEK CAPITAL LETTER OMEGA WITH PSILI AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0027, 0x0313, 0x03b1, 0}, 0x1f84 }, // U1F84 # GREEK SMALL LETTER ALPHA WITH PSILI AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0027, 0x0313, 0x03b7, 0}, 0x1f94 }, // U1F94 # GREEK SMALL LETTER ETA WITH PSILI AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0027, 0x0313, 0x03c9, 0}, 0x1fa4 }, // U1FA4 # GREEK SMALL LETTER OMEGA WITH PSILI AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0027, 0x0314, 0x0391, 0}, 0x1f8d }, // U1F8D # GREEK CAPITAL LETTER ALPHA WITH DASIA AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0027, 0x0314, 0x0397, 0}, 0x1f9d }, // U1F9D # GREEK CAPITAL LETTER ETA WITH DASIA AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0027, 0x0314, 0x03a9, 0}, 0x1fad }, // U1FAD # GREEK CAPITAL LETTER OMEGA WITH DASIA AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0027, 0x0314, 0x03b1, 0}, 0x1f85 }, // U1F85 # GREEK SMALL LETTER ALPHA WITH DASIA AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0027, 0x0314, 0x03b7, 0}, 0x1f95 }, // U1F95 # GREEK SMALL LETTER ETA WITH DASIA AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0027, 0x0314, 0x03c9, 0}, 0x1fa5 }, // U1FA5 # GREEK SMALL LETTER OMEGA WITH DASIA AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0027, 0x03b1, 0, 0}, 0x1fb4 }, // U1FB4 # GREEK SMALL LETTER ALPHA WITH OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0027, 0x03b7, 0, 0}, 0x1fc4 }, // U1FC4 # GREEK SMALL LETTER ETA WITH OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0027, 0x03c9, 0, 0}, 0x1ff4 }, // U1FF4 # GREEK SMALL LETTER OMEGA WITH OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0027, 0x1f00, 0, 0}, 0x1f84 }, // U1F84 # GREEK SMALL LETTER ALPHA WITH PSILI AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0027, 0x1f01, 0, 0}, 0x1f85 }, // U1F85 # GREEK SMALL LETTER ALPHA WITH DASIA AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0027, 0x1f08, 0, 0}, 0x1f8c }, // U1F8C # GREEK CAPITAL LETTER ALPHA WITH PSILI AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0027, 0x1f09, 0, 0}, 0x1f8d }, // U1F8D # GREEK CAPITAL LETTER ALPHA WITH DASIA AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0027, 0x1f20, 0, 0}, 0x1f94 }, // U1F94 # GREEK SMALL LETTER ETA WITH PSILI AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0027, 0x1f21, 0, 0}, 0x1f95 }, // U1F95 # GREEK SMALL LETTER ETA WITH DASIA AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0027, 0x1f28, 0, 0}, 0x1f9c }, // U1F9C # GREEK CAPITAL LETTER ETA WITH PSILI AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0027, 0x1f29, 0, 0}, 0x1f9d }, // U1F9D # GREEK CAPITAL LETTER ETA WITH DASIA AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0027, 0x1f60, 0, 0}, 0x1fa4 }, // U1FA4 # GREEK SMALL LETTER OMEGA WITH PSILI AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0027, 0x1f61, 0, 0}, 0x1fa5 }, // U1FA5 # GREEK SMALL LETTER OMEGA WITH DASIA AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0027, 0x1f68, 0, 0}, 0x1fac }, // U1FAC # GREEK CAPITAL LETTER OMEGA WITH PSILI AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0027, 0x1f69, 0, 0}, 0x1fad }, // U1FAD # GREEK CAPITAL LETTER OMEGA WITH DASIA AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0028, 0x0391, 0, 0}, 0x1f89 }, // U1F89 # GREEK CAPITAL LETTER ALPHA WITH DASIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0028, 0x0397, 0, 0}, 0x1f99 }, // U1F99 # GREEK CAPITAL LETTER ETA WITH DASIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0028, 0x03a9, 0, 0}, 0x1fa9 }, // U1FA9 # GREEK CAPITAL LETTER OMEGA WITH DASIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0028, 0x03b1, 0, 0}, 0x1f81 }, // U1F81 # GREEK SMALL LETTER ALPHA WITH DASIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0028, 0x03b7, 0, 0}, 0x1f91 }, // U1F91 # GREEK SMALL LETTER ETA WITH DASIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0028, 0x03c9, 0, 0}, 0x1fa1 }, // U1FA1 # GREEK SMALL LETTER OMEGA WITH DASIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0029, 0x0391, 0, 0}, 0x1f88 }, // U1F88 # GREEK CAPITAL LETTER ALPHA WITH PSILI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0029, 0x0397, 0, 0}, 0x1f98 }, // U1F98 # GREEK CAPITAL LETTER ETA WITH PSILI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0029, 0x03a9, 0, 0}, 0x1fa8 }, // U1FA8 # GREEK CAPITAL LETTER OMEGA WITH PSILI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0029, 0x03b1, 0, 0}, 0x1f80 }, // U1F80 # GREEK SMALL LETTER ALPHA WITH PSILI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0029, 0x03b7, 0, 0}, 0x1f90 }, // U1F90 # GREEK SMALL LETTER ETA WITH PSILI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0029, 0x03c9, 0, 0}, 0x1fa0 }, // U1FA0 # GREEK SMALL LETTER OMEGA WITH PSILI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0060, 0x0028, 0x0391, 0}, 0x1f8b }, // U1F8B # GREEK CAPITAL LETTER ALPHA WITH DASIA AND VARIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0060, 0x0028, 0x0397, 0}, 0x1f9b }, // U1F9B # GREEK CAPITAL LETTER ETA WITH DASIA AND VARIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0060, 0x0028, 0x03a9, 0}, 0x1fab }, // U1FAB # GREEK CAPITAL LETTER OMEGA WITH DASIA AND VARIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0060, 0x0028, 0x03b1, 0}, 0x1f83 }, // U1F83 # GREEK SMALL LETTER ALPHA WITH DASIA AND VARIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0060, 0x0028, 0x03b7, 0}, 0x1f93 }, // U1F93 # GREEK SMALL LETTER ETA WITH DASIA AND VARIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0060, 0x0028, 0x03c9, 0}, 0x1fa3 }, // U1FA3 # GREEK SMALL LETTER OMEGA WITH DASIA AND VARIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0060, 0x0029, 0x0391, 0}, 0x1f8a }, // U1F8A # GREEK CAPITAL LETTER ALPHA WITH PSILI AND VARIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0060, 0x0029, 0x0397, 0}, 0x1f9a }, // U1F9A # GREEK CAPITAL LETTER ETA WITH PSILI AND VARIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0060, 0x0029, 0x03a9, 0}, 0x1faa }, // U1FAA # GREEK CAPITAL LETTER OMEGA WITH PSILI AND VARIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0060, 0x0029, 0x03b1, 0}, 0x1f82 }, // U1F82 # GREEK SMALL LETTER ALPHA WITH PSILI AND VARIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0060, 0x0029, 0x03b7, 0}, 0x1f92 }, // U1F92 # GREEK SMALL LETTER ETA WITH PSILI AND VARIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0060, 0x0029, 0x03c9, 0}, 0x1fa2 }, // U1FA2 # GREEK SMALL LETTER OMEGA WITH PSILI AND VARIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0060, 0x0313, 0x0391, 0}, 0x1f8a }, // U1F8A # GREEK CAPITAL LETTER ALPHA WITH PSILI AND VARIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0060, 0x0313, 0x0397, 0}, 0x1f9a }, // U1F9A # GREEK CAPITAL LETTER ETA WITH PSILI AND VARIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0060, 0x0313, 0x03a9, 0}, 0x1faa }, // U1FAA # GREEK CAPITAL LETTER OMEGA WITH PSILI AND VARIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0060, 0x0313, 0x03b1, 0}, 0x1f82 }, // U1F82 # GREEK SMALL LETTER ALPHA WITH PSILI AND VARIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0060, 0x0313, 0x03b7, 0}, 0x1f92 }, // U1F92 # GREEK SMALL LETTER ETA WITH PSILI AND VARIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0060, 0x0313, 0x03c9, 0}, 0x1fa2 }, // U1FA2 # GREEK SMALL LETTER OMEGA WITH PSILI AND VARIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0060, 0x0314, 0x0391, 0}, 0x1f8b }, // U1F8B # GREEK CAPITAL LETTER ALPHA WITH DASIA AND VARIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0060, 0x0314, 0x0397, 0}, 0x1f9b }, // U1F9B # GREEK CAPITAL LETTER ETA WITH DASIA AND VARIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0060, 0x0314, 0x03a9, 0}, 0x1fab }, // U1FAB # GREEK CAPITAL LETTER OMEGA WITH DASIA AND VARIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0060, 0x0314, 0x03b1, 0}, 0x1f83 }, // U1F83 # GREEK SMALL LETTER ALPHA WITH DASIA AND VARIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0060, 0x0314, 0x03b7, 0}, 0x1f93 }, // U1F93 # GREEK SMALL LETTER ETA WITH DASIA AND VARIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0060, 0x0314, 0x03c9, 0}, 0x1fa3 }, // U1FA3 # GREEK SMALL LETTER OMEGA WITH DASIA AND VARIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0060, 0x03b1, 0, 0}, 0x1fb2 }, // U1FB2 # GREEK SMALL LETTER ALPHA WITH VARIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0060, 0x03b7, 0, 0}, 0x1fc2 }, // U1FC2 # GREEK SMALL LETTER ETA WITH VARIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0060, 0x03c9, 0, 0}, 0x1ff2 }, // U1FF2 # GREEK SMALL LETTER OMEGA WITH VARIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0060, 0x1f00, 0, 0}, 0x1f82 }, // U1F82 # GREEK SMALL LETTER ALPHA WITH PSILI AND VARIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0060, 0x1f01, 0, 0}, 0x1f83 }, // U1F83 # GREEK SMALL LETTER ALPHA WITH DASIA AND VARIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0060, 0x1f08, 0, 0}, 0x1f8a }, // U1F8A # GREEK CAPITAL LETTER ALPHA WITH PSILI AND VARIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0060, 0x1f09, 0, 0}, 0x1f8b }, // U1F8B # GREEK CAPITAL LETTER ALPHA WITH DASIA AND VARIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0060, 0x1f20, 0, 0}, 0x1f92 }, // U1F92 # GREEK SMALL LETTER ETA WITH PSILI AND VARIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0060, 0x1f21, 0, 0}, 0x1f93 }, // U1F93 # GREEK SMALL LETTER ETA WITH DASIA AND VARIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0060, 0x1f28, 0, 0}, 0x1f9a }, // U1F9A # GREEK CAPITAL LETTER ETA WITH PSILI AND VARIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0060, 0x1f29, 0, 0}, 0x1f9b }, // U1F9B # GREEK CAPITAL LETTER ETA WITH DASIA AND VARIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0060, 0x1f60, 0, 0}, 0x1fa2 }, // U1FA2 # GREEK SMALL LETTER OMEGA WITH PSILI AND VARIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0060, 0x1f61, 0, 0}, 0x1fa3 }, // U1FA3 # GREEK SMALL LETTER OMEGA WITH DASIA AND VARIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0060, 0x1f68, 0, 0}, 0x1faa }, // U1FAA # GREEK CAPITAL LETTER OMEGA WITH PSILI AND VARIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0060, 0x1f69, 0, 0}, 0x1fab }, // U1FAB # GREEK CAPITAL LETTER OMEGA WITH DASIA AND VARIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x007e, 0x0028, 0x0391, 0}, 0x1f8f }, // U1F8F # GREEK CAPITAL LETTER ALPHA WITH DASIA AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x007e, 0x0028, 0x0397, 0}, 0x1f9f }, // U1F9F # GREEK CAPITAL LETTER ETA WITH DASIA AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x007e, 0x0028, 0x03a9, 0}, 0x1faf }, // U1FAF # GREEK CAPITAL LETTER OMEGA WITH DASIA AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x007e, 0x0028, 0x03b1, 0}, 0x1f87 }, // U1F87 # GREEK SMALL LETTER ALPHA WITH DASIA AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x007e, 0x0028, 0x03b7, 0}, 0x1f97 }, // U1F97 # GREEK SMALL LETTER ETA WITH DASIA AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x007e, 0x0028, 0x03c9, 0}, 0x1fa7 }, // U1FA7 # GREEK SMALL LETTER OMEGA WITH DASIA AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x007e, 0x0029, 0x0391, 0}, 0x1f8e }, // U1F8E # GREEK CAPITAL LETTER ALPHA WITH PSILI AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x007e, 0x0029, 0x0397, 0}, 0x1f9e }, // U1F9E # GREEK CAPITAL LETTER ETA WITH PSILI AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x007e, 0x0029, 0x03a9, 0}, 0x1fae }, // U1FAE # GREEK CAPITAL LETTER OMEGA WITH PSILI AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x007e, 0x0029, 0x03b1, 0}, 0x1f86 }, // U1F86 # GREEK SMALL LETTER ALPHA WITH PSILI AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x007e, 0x0029, 0x03b7, 0}, 0x1f96 }, // U1F96 # GREEK SMALL LETTER ETA WITH PSILI AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x007e, 0x0029, 0x03c9, 0}, 0x1fa6 }, // U1FA6 # GREEK SMALL LETTER OMEGA WITH PSILI AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x007e, 0x0313, 0x0391, 0}, 0x1f8e }, // U1F8E # GREEK CAPITAL LETTER ALPHA WITH PSILI AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x007e, 0x0313, 0x0397, 0}, 0x1f9e }, // U1F9E # GREEK CAPITAL LETTER ETA WITH PSILI AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x007e, 0x0313, 0x03a9, 0}, 0x1fae }, // U1FAE # GREEK CAPITAL LETTER OMEGA WITH PSILI AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x007e, 0x0313, 0x03b1, 0}, 0x1f86 }, // U1F86 # GREEK SMALL LETTER ALPHA WITH PSILI AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x007e, 0x0313, 0x03b7, 0}, 0x1f96 }, // U1F96 # GREEK SMALL LETTER ETA WITH PSILI AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x007e, 0x0313, 0x03c9, 0}, 0x1fa6 }, // U1FA6 # GREEK SMALL LETTER OMEGA WITH PSILI AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x007e, 0x0314, 0x0391, 0}, 0x1f8f }, // U1F8F # GREEK CAPITAL LETTER ALPHA WITH DASIA AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x007e, 0x0314, 0x0397, 0}, 0x1f9f }, // U1F9F # GREEK CAPITAL LETTER ETA WITH DASIA AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x007e, 0x0314, 0x03a9, 0}, 0x1faf }, // U1FAF # GREEK CAPITAL LETTER OMEGA WITH DASIA AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x007e, 0x0314, 0x03b1, 0}, 0x1f87 }, // U1F87 # GREEK SMALL LETTER ALPHA WITH DASIA AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x007e, 0x0314, 0x03b7, 0}, 0x1f97 }, // U1F97 # GREEK SMALL LETTER ETA WITH DASIA AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x007e, 0x0314, 0x03c9, 0}, 0x1fa7 }, // U1FA7 # GREEK SMALL LETTER OMEGA WITH DASIA AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x007e, 0x03b1, 0, 0}, 0x1fb7 }, // U1FB7 # GREEK SMALL LETTER ALPHA WITH PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x007e, 0x03b7, 0, 0}, 0x1fc7 }, // U1FC7 # GREEK SMALL LETTER ETA WITH PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x007e, 0x03c9, 0, 0}, 0x1ff7 }, // U1FF7 # GREEK SMALL LETTER OMEGA WITH PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x007e, 0x1f00, 0, 0}, 0x1f86 }, // U1F86 # GREEK SMALL LETTER ALPHA WITH PSILI AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x007e, 0x1f01, 0, 0}, 0x1f87 }, // U1F87 # GREEK SMALL LETTER ALPHA WITH DASIA AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x007e, 0x1f08, 0, 0}, 0x1f8e }, // U1F8E # GREEK CAPITAL LETTER ALPHA WITH PSILI AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x007e, 0x1f09, 0, 0}, 0x1f8f }, // U1F8F # GREEK CAPITAL LETTER ALPHA WITH DASIA AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x007e, 0x1f20, 0, 0}, 0x1f96 }, // U1F96 # GREEK SMALL LETTER ETA WITH PSILI AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x007e, 0x1f21, 0, 0}, 0x1f97 }, // U1F97 # GREEK SMALL LETTER ETA WITH DASIA AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x007e, 0x1f28, 0, 0}, 0x1f9e }, // U1F9E # GREEK CAPITAL LETTER ETA WITH PSILI AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x007e, 0x1f29, 0, 0}, 0x1f9f }, // U1F9F # GREEK CAPITAL LETTER ETA WITH DASIA AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x007e, 0x1f60, 0, 0}, 0x1fa6 }, // U1FA6 # GREEK SMALL LETTER OMEGA WITH PSILI AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x007e, 0x1f61, 0, 0}, 0x1fa7 }, // U1FA7 # GREEK SMALL LETTER OMEGA WITH DASIA AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x007e, 0x1f68, 0, 0}, 0x1fae }, // U1FAE # GREEK CAPITAL LETTER OMEGA WITH PSILI AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x007e, 0x1f69, 0, 0}, 0x1faf }, // U1FAF # GREEK CAPITAL LETTER OMEGA WITH DASIA AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x00b4, 0x0028, 0x0391, 0}, 0x1f8d }, // U1F8D # GREEK CAPITAL LETTER ALPHA WITH DASIA AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x00b4, 0x0028, 0x0397, 0}, 0x1f9d }, // U1F9D # GREEK CAPITAL LETTER ETA WITH DASIA AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x00b4, 0x0028, 0x03a9, 0}, 0x1fad }, // U1FAD # GREEK CAPITAL LETTER OMEGA WITH DASIA AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x00b4, 0x0028, 0x03b1, 0}, 0x1f85 }, // U1F85 # GREEK SMALL LETTER ALPHA WITH DASIA AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x00b4, 0x0028, 0x03b7, 0}, 0x1f95 }, // U1F95 # GREEK SMALL LETTER ETA WITH DASIA AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x00b4, 0x0028, 0x03c9, 0}, 0x1fa5 }, // U1FA5 # GREEK SMALL LETTER OMEGA WITH DASIA AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x00b4, 0x0029, 0x0391, 0}, 0x1f8c }, // U1F8C # GREEK CAPITAL LETTER ALPHA WITH PSILI AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x00b4, 0x0029, 0x0397, 0}, 0x1f9c }, // U1F9C # GREEK CAPITAL LETTER ETA WITH PSILI AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x00b4, 0x0029, 0x03a9, 0}, 0x1fac }, // U1FAC # GREEK CAPITAL LETTER OMEGA WITH PSILI AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x00b4, 0x0029, 0x03b1, 0}, 0x1f84 }, // U1F84 # GREEK SMALL LETTER ALPHA WITH PSILI AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x00b4, 0x0029, 0x03b7, 0}, 0x1f94 }, // U1F94 # GREEK SMALL LETTER ETA WITH PSILI AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x00b4, 0x0029, 0x03c9, 0}, 0x1fa4 }, // U1FA4 # GREEK SMALL LETTER OMEGA WITH PSILI AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x00b4, 0x0313, 0x0391, 0}, 0x1f8c }, // U1F8C # GREEK CAPITAL LETTER ALPHA WITH PSILI AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x00b4, 0x0313, 0x0397, 0}, 0x1f9c }, // U1F9C # GREEK CAPITAL LETTER ETA WITH PSILI AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x00b4, 0x0313, 0x03a9, 0}, 0x1fac }, // U1FAC # GREEK CAPITAL LETTER OMEGA WITH PSILI AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x00b4, 0x0313, 0x03b1, 0}, 0x1f84 }, // U1F84 # GREEK SMALL LETTER ALPHA WITH PSILI AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x00b4, 0x0313, 0x03b7, 0}, 0x1f94 }, // U1F94 # GREEK SMALL LETTER ETA WITH PSILI AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x00b4, 0x0313, 0x03c9, 0}, 0x1fa4 }, // U1FA4 # GREEK SMALL LETTER OMEGA WITH PSILI AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x00b4, 0x0314, 0x0391, 0}, 0x1f8d }, // U1F8D # GREEK CAPITAL LETTER ALPHA WITH DASIA AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x00b4, 0x0314, 0x0397, 0}, 0x1f9d }, // U1F9D # GREEK CAPITAL LETTER ETA WITH DASIA AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x00b4, 0x0314, 0x03a9, 0}, 0x1fad }, // U1FAD # GREEK CAPITAL LETTER OMEGA WITH DASIA AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x00b4, 0x0314, 0x03b1, 0}, 0x1f85 }, // U1F85 # GREEK SMALL LETTER ALPHA WITH DASIA AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x00b4, 0x0314, 0x03b7, 0}, 0x1f95 }, // U1F95 # GREEK SMALL LETTER ETA WITH DASIA AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x00b4, 0x0314, 0x03c9, 0}, 0x1fa5 }, // U1FA5 # GREEK SMALL LETTER OMEGA WITH DASIA AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x00b4, 0x03b1, 0, 0}, 0x1fb4 }, // U1FB4 # GREEK SMALL LETTER ALPHA WITH OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x00b4, 0x03b7, 0, 0}, 0x1fc4 }, // U1FC4 # GREEK SMALL LETTER ETA WITH OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x00b4, 0x03c9, 0, 0}, 0x1ff4 }, // U1FF4 # GREEK SMALL LETTER OMEGA WITH OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x00b4, 0x1f00, 0, 0}, 0x1f84 }, // U1F84 # GREEK SMALL LETTER ALPHA WITH PSILI AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x00b4, 0x1f01, 0, 0}, 0x1f85 }, // U1F85 # GREEK SMALL LETTER ALPHA WITH DASIA AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x00b4, 0x1f08, 0, 0}, 0x1f8c }, // U1F8C # GREEK CAPITAL LETTER ALPHA WITH PSILI AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x00b4, 0x1f09, 0, 0}, 0x1f8d }, // U1F8D # GREEK CAPITAL LETTER ALPHA WITH DASIA AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x00b4, 0x1f20, 0, 0}, 0x1f94 }, // U1F94 # GREEK SMALL LETTER ETA WITH PSILI AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x00b4, 0x1f21, 0, 0}, 0x1f95 }, // U1F95 # GREEK SMALL LETTER ETA WITH DASIA AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x00b4, 0x1f28, 0, 0}, 0x1f9c }, // U1F9C # GREEK CAPITAL LETTER ETA WITH PSILI AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x00b4, 0x1f29, 0, 0}, 0x1f9d }, // U1F9D # GREEK CAPITAL LETTER ETA WITH DASIA AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x00b4, 0x1f60, 0, 0}, 0x1fa4 }, // U1FA4 # GREEK SMALL LETTER OMEGA WITH PSILI AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x00b4, 0x1f61, 0, 0}, 0x1fa5 }, // U1FA5 # GREEK SMALL LETTER OMEGA WITH DASIA AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x00b4, 0x1f68, 0, 0}, 0x1fac }, // U1FAC # GREEK CAPITAL LETTER OMEGA WITH PSILI AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x00b4, 0x1f69, 0, 0}, 0x1fad }, // U1FAD # GREEK CAPITAL LETTER OMEGA WITH DASIA AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0313, 0x0391, 0, 0}, 0x1f88 }, // U1F88 # GREEK CAPITAL LETTER ALPHA WITH PSILI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0313, 0x0397, 0, 0}, 0x1f98 }, // U1F98 # GREEK CAPITAL LETTER ETA WITH PSILI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0313, 0x03a9, 0, 0}, 0x1fa8 }, // U1FA8 # GREEK CAPITAL LETTER OMEGA WITH PSILI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0313, 0x03b1, 0, 0}, 0x1f80 }, // U1F80 # GREEK SMALL LETTER ALPHA WITH PSILI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0313, 0x03b7, 0, 0}, 0x1f90 }, // U1F90 # GREEK SMALL LETTER ETA WITH PSILI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0313, 0x03c9, 0, 0}, 0x1fa0 }, // U1FA0 # GREEK SMALL LETTER OMEGA WITH PSILI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0314, 0x0391, 0, 0}, 0x1f89 }, // U1F89 # GREEK CAPITAL LETTER ALPHA WITH DASIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0314, 0x0397, 0, 0}, 0x1f99 }, // U1F99 # GREEK CAPITAL LETTER ETA WITH DASIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0314, 0x03a9, 0, 0}, 0x1fa9 }, // U1FA9 # GREEK CAPITAL LETTER OMEGA WITH DASIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0314, 0x03b1, 0, 0}, 0x1f81 }, // U1F81 # GREEK SMALL LETTER ALPHA WITH DASIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0314, 0x03b7, 0, 0}, 0x1f91 }, // U1F91 # GREEK SMALL LETTER ETA WITH DASIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0314, 0x03c9, 0, 0}, 0x1fa1 }, // U1FA1 # GREEK SMALL LETTER OMEGA WITH DASIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0342, 0x0028, 0x0391, 0}, 0x1f8f }, // U1F8F # GREEK CAPITAL LETTER ALPHA WITH DASIA AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0342, 0x0028, 0x0397, 0}, 0x1f9f }, // U1F9F # GREEK CAPITAL LETTER ETA WITH DASIA AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0342, 0x0028, 0x03a9, 0}, 0x1faf }, // U1FAF # GREEK CAPITAL LETTER OMEGA WITH DASIA AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0342, 0x0028, 0x03b1, 0}, 0x1f87 }, // U1F87 # GREEK SMALL LETTER ALPHA WITH DASIA AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0342, 0x0028, 0x03b7, 0}, 0x1f97 }, // U1F97 # GREEK SMALL LETTER ETA WITH DASIA AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0342, 0x0028, 0x03c9, 0}, 0x1fa7 }, // U1FA7 # GREEK SMALL LETTER OMEGA WITH DASIA AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0342, 0x0029, 0x0391, 0}, 0x1f8e }, // U1F8E # GREEK CAPITAL LETTER ALPHA WITH PSILI AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0342, 0x0029, 0x0397, 0}, 0x1f9e }, // U1F9E # GREEK CAPITAL LETTER ETA WITH PSILI AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0342, 0x0029, 0x03a9, 0}, 0x1fae }, // U1FAE # GREEK CAPITAL LETTER OMEGA WITH PSILI AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0342, 0x0029, 0x03b1, 0}, 0x1f86 }, // U1F86 # GREEK SMALL LETTER ALPHA WITH PSILI AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0342, 0x0029, 0x03b7, 0}, 0x1f96 }, // U1F96 # GREEK SMALL LETTER ETA WITH PSILI AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0342, 0x0029, 0x03c9, 0}, 0x1fa6 }, // U1FA6 # GREEK SMALL LETTER OMEGA WITH PSILI AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0342, 0x0313, 0x0391, 0}, 0x1f8e }, // U1F8E # GREEK CAPITAL LETTER ALPHA WITH PSILI AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0342, 0x0313, 0x0397, 0}, 0x1f9e }, // U1F9E # GREEK CAPITAL LETTER ETA WITH PSILI AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0342, 0x0313, 0x03a9, 0}, 0x1fae }, // U1FAE # GREEK CAPITAL LETTER OMEGA WITH PSILI AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0342, 0x0313, 0x03b1, 0}, 0x1f86 }, // U1F86 # GREEK SMALL LETTER ALPHA WITH PSILI AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0342, 0x0313, 0x03b7, 0}, 0x1f96 }, // U1F96 # GREEK SMALL LETTER ETA WITH PSILI AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0342, 0x0313, 0x03c9, 0}, 0x1fa6 }, // U1FA6 # GREEK SMALL LETTER OMEGA WITH PSILI AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0342, 0x0314, 0x0391, 0}, 0x1f8f }, // U1F8F # GREEK CAPITAL LETTER ALPHA WITH DASIA AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0342, 0x0314, 0x0397, 0}, 0x1f9f }, // U1F9F # GREEK CAPITAL LETTER ETA WITH DASIA AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0342, 0x0314, 0x03a9, 0}, 0x1faf }, // U1FAF # GREEK CAPITAL LETTER OMEGA WITH DASIA AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0342, 0x0314, 0x03b1, 0}, 0x1f87 }, // U1F87 # GREEK SMALL LETTER ALPHA WITH DASIA AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0342, 0x0314, 0x03b7, 0}, 0x1f97 }, // U1F97 # GREEK SMALL LETTER ETA WITH DASIA AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0342, 0x0314, 0x03c9, 0}, 0x1fa7 }, // U1FA7 # GREEK SMALL LETTER OMEGA WITH DASIA AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0342, 0x03b1, 0, 0}, 0x1fb7 }, // U1FB7 # GREEK SMALL LETTER ALPHA WITH PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0342, 0x03b7, 0, 0}, 0x1fc7 }, // U1FC7 # GREEK SMALL LETTER ETA WITH PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0342, 0x03c9, 0, 0}, 0x1ff7 }, // U1FF7 # GREEK SMALL LETTER OMEGA WITH PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0342, 0x1f00, 0, 0}, 0x1f86 }, // U1F86 # GREEK SMALL LETTER ALPHA WITH PSILI AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0342, 0x1f01, 0, 0}, 0x1f87 }, // U1F87 # GREEK SMALL LETTER ALPHA WITH DASIA AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0342, 0x1f08, 0, 0}, 0x1f8e }, // U1F8E # GREEK CAPITAL LETTER ALPHA WITH PSILI AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0342, 0x1f09, 0, 0}, 0x1f8f }, // U1F8F # GREEK CAPITAL LETTER ALPHA WITH DASIA AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0342, 0x1f20, 0, 0}, 0x1f96 }, // U1F96 # GREEK SMALL LETTER ETA WITH PSILI AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0342, 0x1f21, 0, 0}, 0x1f97 }, // U1F97 # GREEK SMALL LETTER ETA WITH DASIA AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0342, 0x1f28, 0, 0}, 0x1f9e }, // U1F9E # GREEK CAPITAL LETTER ETA WITH PSILI AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0342, 0x1f29, 0, 0}, 0x1f9f }, // U1F9F # GREEK CAPITAL LETTER ETA WITH DASIA AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0342, 0x1f60, 0, 0}, 0x1fa6 }, // U1FA6 # GREEK SMALL LETTER OMEGA WITH PSILI AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0342, 0x1f61, 0, 0}, 0x1fa7 }, // U1FA7 # GREEK SMALL LETTER OMEGA WITH DASIA AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0342, 0x1f68, 0, 0}, 0x1fae }, // U1FAE # GREEK CAPITAL LETTER OMEGA WITH PSILI AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0342, 0x1f69, 0, 0}, 0x1faf }, // U1FAF # GREEK CAPITAL LETTER OMEGA WITH DASIA AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0391, 0, 0, 0}, 0x1fbc }, // U1FBC # GREEK CAPITAL LETTER ALPHA WITH PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x0397, 0, 0, 0}, 0x1fcc }, // U1FCC # GREEK CAPITAL LETTER ETA WITH PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x03a9, 0, 0, 0}, 0x1ffc }, // U1FFC # GREEK CAPITAL LETTER OMEGA WITH PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x03ac, 0, 0, 0}, 0x1fb4 }, // U1FB4 # GREEK SMALL LETTER ALPHA WITH OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x03ae, 0, 0, 0}, 0x1fc4 }, // U1FC4 # GREEK SMALL LETTER ETA WITH OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x03b1, 0, 0, 0}, 0x1fb3 }, // U1FB3 # GREEK SMALL LETTER ALPHA WITH YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x03b7, 0, 0, 0}, 0x1fc3 }, // U1FC3 # GREEK SMALL LETTER ETA WITH YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x03c9, 0, 0, 0}, 0x1ff3 }, // U1FF3 # GREEK SMALL LETTER OMEGA WITH YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x03ce, 0, 0, 0}, 0x1ff4 }, // U1FF4 # GREEK SMALL LETTER OMEGA WITH OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x1f00, 0, 0, 0}, 0x1f80 }, // U1F80 # GREEK SMALL LETTER ALPHA WITH PSILI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x1f01, 0, 0, 0}, 0x1f81 }, // U1F81 # GREEK SMALL LETTER ALPHA WITH DASIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x1f02, 0, 0, 0}, 0x1f82 }, // U1F82 # GREEK SMALL LETTER ALPHA WITH PSILI AND VARIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x1f03, 0, 0, 0}, 0x1f83 }, // U1F83 # GREEK SMALL LETTER ALPHA WITH DASIA AND VARIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x1f04, 0, 0, 0}, 0x1f84 }, // U1F84 # GREEK SMALL LETTER ALPHA WITH PSILI AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x1f05, 0, 0, 0}, 0x1f85 }, // U1F85 # GREEK SMALL LETTER ALPHA WITH DASIA AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x1f06, 0, 0, 0}, 0x1f86 }, // U1F86 # GREEK SMALL LETTER ALPHA WITH PSILI AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x1f07, 0, 0, 0}, 0x1f87 }, // U1F87 # GREEK SMALL LETTER ALPHA WITH DASIA AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x1f08, 0, 0, 0}, 0x1f88 }, // U1F88 # GREEK CAPITAL LETTER ALPHA WITH PSILI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x1f09, 0, 0, 0}, 0x1f89 }, // U1F89 # GREEK CAPITAL LETTER ALPHA WITH DASIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x1f0a, 0, 0, 0}, 0x1f8a }, // U1F8A # GREEK CAPITAL LETTER ALPHA WITH PSILI AND VARIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x1f0b, 0, 0, 0}, 0x1f8b }, // U1F8B # GREEK CAPITAL LETTER ALPHA WITH DASIA AND VARIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x1f0c, 0, 0, 0}, 0x1f8c }, // U1F8C # GREEK CAPITAL LETTER ALPHA WITH PSILI AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x1f0d, 0, 0, 0}, 0x1f8d }, // U1F8D # GREEK CAPITAL LETTER ALPHA WITH DASIA AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x1f0e, 0, 0, 0}, 0x1f8e }, // U1F8E # GREEK CAPITAL LETTER ALPHA WITH PSILI AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x1f0f, 0, 0, 0}, 0x1f8f }, // U1F8F # GREEK CAPITAL LETTER ALPHA WITH DASIA AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x1f20, 0, 0, 0}, 0x1f90 }, // U1F90 # GREEK SMALL LETTER ETA WITH PSILI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x1f21, 0, 0, 0}, 0x1f91 }, // U1F91 # GREEK SMALL LETTER ETA WITH DASIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x1f22, 0, 0, 0}, 0x1f92 }, // U1F92 # GREEK SMALL LETTER ETA WITH PSILI AND VARIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x1f23, 0, 0, 0}, 0x1f93 }, // U1F93 # GREEK SMALL LETTER ETA WITH DASIA AND VARIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x1f24, 0, 0, 0}, 0x1f94 }, // U1F94 # GREEK SMALL LETTER ETA WITH PSILI AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x1f25, 0, 0, 0}, 0x1f95 }, // U1F95 # GREEK SMALL LETTER ETA WITH DASIA AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x1f26, 0, 0, 0}, 0x1f96 }, // U1F96 # GREEK SMALL LETTER ETA WITH PSILI AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x1f27, 0, 0, 0}, 0x1f97 }, // U1F97 # GREEK SMALL LETTER ETA WITH DASIA AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x1f28, 0, 0, 0}, 0x1f98 }, // U1F98 # GREEK CAPITAL LETTER ETA WITH PSILI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x1f29, 0, 0, 0}, 0x1f99 }, // U1F99 # GREEK CAPITAL LETTER ETA WITH DASIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x1f2a, 0, 0, 0}, 0x1f9a }, // U1F9A # GREEK CAPITAL LETTER ETA WITH PSILI AND VARIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x1f2b, 0, 0, 0}, 0x1f9b }, // U1F9B # GREEK CAPITAL LETTER ETA WITH DASIA AND VARIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x1f2c, 0, 0, 0}, 0x1f9c }, // U1F9C # GREEK CAPITAL LETTER ETA WITH PSILI AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x1f2d, 0, 0, 0}, 0x1f9d }, // U1F9D # GREEK CAPITAL LETTER ETA WITH DASIA AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x1f2e, 0, 0, 0}, 0x1f9e }, // U1F9E # GREEK CAPITAL LETTER ETA WITH PSILI AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x1f2f, 0, 0, 0}, 0x1f9f }, // U1F9F # GREEK CAPITAL LETTER ETA WITH DASIA AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x1f60, 0, 0, 0}, 0x1fa0 }, // U1FA0 # GREEK SMALL LETTER OMEGA WITH PSILI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x1f61, 0, 0, 0}, 0x1fa1 }, // U1FA1 # GREEK SMALL LETTER OMEGA WITH DASIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x1f62, 0, 0, 0}, 0x1fa2 }, // U1FA2 # GREEK SMALL LETTER OMEGA WITH PSILI AND VARIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x1f63, 0, 0, 0}, 0x1fa3 }, // U1FA3 # GREEK SMALL LETTER OMEGA WITH DASIA AND VARIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x1f64, 0, 0, 0}, 0x1fa4 }, // U1FA4 # GREEK SMALL LETTER OMEGA WITH PSILI AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x1f65, 0, 0, 0}, 0x1fa5 }, // U1FA5 # GREEK SMALL LETTER OMEGA WITH DASIA AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x1f66, 0, 0, 0}, 0x1fa6 }, // U1FA6 # GREEK SMALL LETTER OMEGA WITH PSILI AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x1f67, 0, 0, 0}, 0x1fa7 }, // U1FA7 # GREEK SMALL LETTER OMEGA WITH DASIA AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x1f68, 0, 0, 0}, 0x1fa8 }, // U1FA8 # GREEK CAPITAL LETTER OMEGA WITH PSILI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x1f69, 0, 0, 0}, 0x1fa9 }, // U1FA9 # GREEK CAPITAL LETTER OMEGA WITH DASIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x1f6a, 0, 0, 0}, 0x1faa }, // U1FAA # GREEK CAPITAL LETTER OMEGA WITH PSILI AND VARIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x1f6b, 0, 0, 0}, 0x1fab }, // U1FAB # GREEK CAPITAL LETTER OMEGA WITH DASIA AND VARIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x1f6c, 0, 0, 0}, 0x1fac }, // U1FAC # GREEK CAPITAL LETTER OMEGA WITH PSILI AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x1f6d, 0, 0, 0}, 0x1fad }, // U1FAD # GREEK CAPITAL LETTER OMEGA WITH DASIA AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x1f6e, 0, 0, 0}, 0x1fae }, // U1FAE # GREEK CAPITAL LETTER OMEGA WITH PSILI AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x1f6f, 0, 0, 0}, 0x1faf }, // U1FAF # GREEK CAPITAL LETTER OMEGA WITH DASIA AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x1f70, 0, 0, 0}, 0x1fb2 }, // U1FB2 # GREEK SMALL LETTER ALPHA WITH VARIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x1f74, 0, 0, 0}, 0x1fc2 }, // U1FC2 # GREEK SMALL LETTER ETA WITH VARIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x1f7c, 0, 0, 0}, 0x1ff2 }, // U1FF2 # GREEK SMALL LETTER OMEGA WITH VARIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x1fb6, 0, 0, 0}, 0x1fb7 }, // U1FB7 # GREEK SMALL LETTER ALPHA WITH PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x1fc6, 0, 0, 0}, 0x1fc7 }, // U1FC7 # GREEK SMALL LETTER ETA WITH PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, 0x1ff6, 0, 0, 0}, 0x1ff7 }, // U1FF7 # GREEK SMALL LETTER OMEGA WITH PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Grave), 0x0028, 0x0391, 0}, 0x1f8b }, // U1F8B # GREEK CAPITAL LETTER ALPHA WITH DASIA AND VARIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Grave), 0x0028, 0x0397, 0}, 0x1f9b }, // U1F9B # GREEK CAPITAL LETTER ETA WITH DASIA AND VARIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Grave), 0x0028, 0x03a9, 0}, 0x1fab }, // U1FAB # GREEK CAPITAL LETTER OMEGA WITH DASIA AND VARIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Grave), 0x0028, 0x03b1, 0}, 0x1f83 }, // U1F83 # GREEK SMALL LETTER ALPHA WITH DASIA AND VARIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Grave), 0x0028, 0x03b7, 0}, 0x1f93 }, // U1F93 # GREEK SMALL LETTER ETA WITH DASIA AND VARIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Grave), 0x0028, 0x03c9, 0}, 0x1fa3 }, // U1FA3 # GREEK SMALL LETTER OMEGA WITH DASIA AND VARIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Grave), 0x0029, 0x0391, 0}, 0x1f8a }, // U1F8A # GREEK CAPITAL LETTER ALPHA WITH PSILI AND VARIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Grave), 0x0029, 0x0397, 0}, 0x1f9a }, // U1F9A # GREEK CAPITAL LETTER ETA WITH PSILI AND VARIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Grave), 0x0029, 0x03a9, 0}, 0x1faa }, // U1FAA # GREEK CAPITAL LETTER OMEGA WITH PSILI AND VARIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Grave), 0x0029, 0x03b1, 0}, 0x1f82 }, // U1F82 # GREEK SMALL LETTER ALPHA WITH PSILI AND VARIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Grave), 0x0029, 0x03b7, 0}, 0x1f92 }, // U1F92 # GREEK SMALL LETTER ETA WITH PSILI AND VARIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Grave), 0x0029, 0x03c9, 0}, 0x1fa2 }, // U1FA2 # GREEK SMALL LETTER OMEGA WITH PSILI AND VARIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Grave), 0x0313, 0x0391, 0}, 0x1f8a }, // U1F8A # GREEK CAPITAL LETTER ALPHA WITH PSILI AND VARIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Grave), 0x0313, 0x0397, 0}, 0x1f9a }, // U1F9A # GREEK CAPITAL LETTER ETA WITH PSILI AND VARIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Grave), 0x0313, 0x03a9, 0}, 0x1faa }, // U1FAA # GREEK CAPITAL LETTER OMEGA WITH PSILI AND VARIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Grave), 0x0313, 0x03b1, 0}, 0x1f82 }, // U1F82 # GREEK SMALL LETTER ALPHA WITH PSILI AND VARIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Grave), 0x0313, 0x03b7, 0}, 0x1f92 }, // U1F92 # GREEK SMALL LETTER ETA WITH PSILI AND VARIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Grave), 0x0313, 0x03c9, 0}, 0x1fa2 }, // U1FA2 # GREEK SMALL LETTER OMEGA WITH PSILI AND VARIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Grave), 0x0314, 0x0391, 0}, 0x1f8b }, // U1F8B # GREEK CAPITAL LETTER ALPHA WITH DASIA AND VARIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Grave), 0x0314, 0x0397, 0}, 0x1f9b }, // U1F9B # GREEK CAPITAL LETTER ETA WITH DASIA AND VARIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Grave), 0x0314, 0x03a9, 0}, 0x1fab }, // U1FAB # GREEK CAPITAL LETTER OMEGA WITH DASIA AND VARIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Grave), 0x0314, 0x03b1, 0}, 0x1f83 }, // U1F83 # GREEK SMALL LETTER ALPHA WITH DASIA AND VARIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Grave), 0x0314, 0x03b7, 0}, 0x1f93 }, // U1F93 # GREEK SMALL LETTER ETA WITH DASIA AND VARIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Grave), 0x0314, 0x03c9, 0}, 0x1fa3 }, // U1FA3 # GREEK SMALL LETTER OMEGA WITH DASIA AND VARIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Grave), 0x03b1, 0, 0}, 0x1fb2 }, // U1FB2 # GREEK SMALL LETTER ALPHA WITH VARIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Grave), 0x03b7, 0, 0}, 0x1fc2 }, // U1FC2 # GREEK SMALL LETTER ETA WITH VARIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Grave), 0x03c9, 0, 0}, 0x1ff2 }, // U1FF2 # GREEK SMALL LETTER OMEGA WITH VARIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Grave), 0x1f00, 0, 0}, 0x1f82 }, // U1F82 # GREEK SMALL LETTER ALPHA WITH PSILI AND VARIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Grave), 0x1f01, 0, 0}, 0x1f83 }, // U1F83 # GREEK SMALL LETTER ALPHA WITH DASIA AND VARIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Grave), 0x1f08, 0, 0}, 0x1f8a }, // U1F8A # GREEK CAPITAL LETTER ALPHA WITH PSILI AND VARIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Grave), 0x1f09, 0, 0}, 0x1f8b }, // U1F8B # GREEK CAPITAL LETTER ALPHA WITH DASIA AND VARIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Grave), 0x1f20, 0, 0}, 0x1f92 }, // U1F92 # GREEK SMALL LETTER ETA WITH PSILI AND VARIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Grave), 0x1f21, 0, 0}, 0x1f93 }, // U1F93 # GREEK SMALL LETTER ETA WITH DASIA AND VARIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Grave), 0x1f28, 0, 0}, 0x1f9a }, // U1F9A # GREEK CAPITAL LETTER ETA WITH PSILI AND VARIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Grave), 0x1f29, 0, 0}, 0x1f9b }, // U1F9B # GREEK CAPITAL LETTER ETA WITH DASIA AND VARIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Grave), 0x1f60, 0, 0}, 0x1fa2 }, // U1FA2 # GREEK SMALL LETTER OMEGA WITH PSILI AND VARIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Grave), 0x1f61, 0, 0}, 0x1fa3 }, // U1FA3 # GREEK SMALL LETTER OMEGA WITH DASIA AND VARIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Grave), 0x1f68, 0, 0}, 0x1faa }, // U1FAA # GREEK CAPITAL LETTER OMEGA WITH PSILI AND VARIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Grave), 0x1f69, 0, 0}, 0x1fab }, // U1FAB # GREEK CAPITAL LETTER OMEGA WITH DASIA AND VARIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Acute), 0x0028, 0x0391, 0}, 0x1f8d }, // U1F8D # GREEK CAPITAL LETTER ALPHA WITH DASIA AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Acute), 0x0028, 0x0397, 0}, 0x1f9d }, // U1F9D # GREEK CAPITAL LETTER ETA WITH DASIA AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Acute), 0x0028, 0x03a9, 0}, 0x1fad }, // U1FAD # GREEK CAPITAL LETTER OMEGA WITH DASIA AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Acute), 0x0028, 0x03b1, 0}, 0x1f85 }, // U1F85 # GREEK SMALL LETTER ALPHA WITH DASIA AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Acute), 0x0028, 0x03b7, 0}, 0x1f95 }, // U1F95 # GREEK SMALL LETTER ETA WITH DASIA AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Acute), 0x0028, 0x03c9, 0}, 0x1fa5 }, // U1FA5 # GREEK SMALL LETTER OMEGA WITH DASIA AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Acute), 0x0029, 0x0391, 0}, 0x1f8c }, // U1F8C # GREEK CAPITAL LETTER ALPHA WITH PSILI AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Acute), 0x0029, 0x0397, 0}, 0x1f9c }, // U1F9C # GREEK CAPITAL LETTER ETA WITH PSILI AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Acute), 0x0029, 0x03a9, 0}, 0x1fac }, // U1FAC # GREEK CAPITAL LETTER OMEGA WITH PSILI AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Acute), 0x0029, 0x03b1, 0}, 0x1f84 }, // U1F84 # GREEK SMALL LETTER ALPHA WITH PSILI AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Acute), 0x0029, 0x03b7, 0}, 0x1f94 }, // U1F94 # GREEK SMALL LETTER ETA WITH PSILI AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Acute), 0x0029, 0x03c9, 0}, 0x1fa4 }, // U1FA4 # GREEK SMALL LETTER OMEGA WITH PSILI AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Acute), 0x0313, 0x0391, 0}, 0x1f8c }, // U1F8C # GREEK CAPITAL LETTER ALPHA WITH PSILI AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Acute), 0x0313, 0x0397, 0}, 0x1f9c }, // U1F9C # GREEK CAPITAL LETTER ETA WITH PSILI AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Acute), 0x0313, 0x03a9, 0}, 0x1fac }, // U1FAC # GREEK CAPITAL LETTER OMEGA WITH PSILI AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Acute), 0x0313, 0x03b1, 0}, 0x1f84 }, // U1F84 # GREEK SMALL LETTER ALPHA WITH PSILI AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Acute), 0x0313, 0x03b7, 0}, 0x1f94 }, // U1F94 # GREEK SMALL LETTER ETA WITH PSILI AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Acute), 0x0313, 0x03c9, 0}, 0x1fa4 }, // U1FA4 # GREEK SMALL LETTER OMEGA WITH PSILI AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Acute), 0x0314, 0x0391, 0}, 0x1f8d }, // U1F8D # GREEK CAPITAL LETTER ALPHA WITH DASIA AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Acute), 0x0314, 0x0397, 0}, 0x1f9d }, // U1F9D # GREEK CAPITAL LETTER ETA WITH DASIA AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Acute), 0x0314, 0x03a9, 0}, 0x1fad }, // U1FAD # GREEK CAPITAL LETTER OMEGA WITH DASIA AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Acute), 0x0314, 0x03b1, 0}, 0x1f85 }, // U1F85 # GREEK SMALL LETTER ALPHA WITH DASIA AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Acute), 0x0314, 0x03b7, 0}, 0x1f95 }, // U1F95 # GREEK SMALL LETTER ETA WITH DASIA AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Acute), 0x0314, 0x03c9, 0}, 0x1fa5 }, // U1FA5 # GREEK SMALL LETTER OMEGA WITH DASIA AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Acute), 0x03b1, 0, 0}, 0x1fb4 }, // U1FB4 # GREEK SMALL LETTER ALPHA WITH OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Acute), 0x03b7, 0, 0}, 0x1fc4 }, // U1FC4 # GREEK SMALL LETTER ETA WITH OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Acute), 0x03c9, 0, 0}, 0x1ff4 }, // U1FF4 # GREEK SMALL LETTER OMEGA WITH OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Acute), 0x1f00, 0, 0}, 0x1f84 }, // U1F84 # GREEK SMALL LETTER ALPHA WITH PSILI AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Acute), 0x1f01, 0, 0}, 0x1f85 }, // U1F85 # GREEK SMALL LETTER ALPHA WITH DASIA AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Acute), 0x1f08, 0, 0}, 0x1f8c }, // U1F8C # GREEK CAPITAL LETTER ALPHA WITH PSILI AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Acute), 0x1f09, 0, 0}, 0x1f8d }, // U1F8D # GREEK CAPITAL LETTER ALPHA WITH DASIA AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Acute), 0x1f20, 0, 0}, 0x1f94 }, // U1F94 # GREEK SMALL LETTER ETA WITH PSILI AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Acute), 0x1f21, 0, 0}, 0x1f95 }, // U1F95 # GREEK SMALL LETTER ETA WITH DASIA AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Acute), 0x1f28, 0, 0}, 0x1f9c }, // U1F9C # GREEK CAPITAL LETTER ETA WITH PSILI AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Acute), 0x1f29, 0, 0}, 0x1f9d }, // U1F9D # GREEK CAPITAL LETTER ETA WITH DASIA AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Acute), 0x1f60, 0, 0}, 0x1fa4 }, // U1FA4 # GREEK SMALL LETTER OMEGA WITH PSILI AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Acute), 0x1f61, 0, 0}, 0x1fa5 }, // U1FA5 # GREEK SMALL LETTER OMEGA WITH DASIA AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Acute), 0x1f68, 0, 0}, 0x1fac }, // U1FAC # GREEK CAPITAL LETTER OMEGA WITH PSILI AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Acute), 0x1f69, 0, 0}, 0x1fad }, // U1FAD # GREEK CAPITAL LETTER OMEGA WITH DASIA AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Tilde), 0x0028, 0x0391, 0}, 0x1f8f }, // U1F8F # GREEK CAPITAL LETTER ALPHA WITH DASIA AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Tilde), 0x0028, 0x0397, 0}, 0x1f9f }, // U1F9F # GREEK CAPITAL LETTER ETA WITH DASIA AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Tilde), 0x0028, 0x03a9, 0}, 0x1faf }, // U1FAF # GREEK CAPITAL LETTER OMEGA WITH DASIA AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Tilde), 0x0028, 0x03b1, 0}, 0x1f87 }, // U1F87 # GREEK SMALL LETTER ALPHA WITH DASIA AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Tilde), 0x0028, 0x03b7, 0}, 0x1f97 }, // U1F97 # GREEK SMALL LETTER ETA WITH DASIA AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Tilde), 0x0028, 0x03c9, 0}, 0x1fa7 }, // U1FA7 # GREEK SMALL LETTER OMEGA WITH DASIA AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Tilde), 0x0029, 0x0391, 0}, 0x1f8e }, // U1F8E # GREEK CAPITAL LETTER ALPHA WITH PSILI AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Tilde), 0x0029, 0x0397, 0}, 0x1f9e }, // U1F9E # GREEK CAPITAL LETTER ETA WITH PSILI AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Tilde), 0x0029, 0x03a9, 0}, 0x1fae }, // U1FAE # GREEK CAPITAL LETTER OMEGA WITH PSILI AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Tilde), 0x0029, 0x03b1, 0}, 0x1f86 }, // U1F86 # GREEK SMALL LETTER ALPHA WITH PSILI AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Tilde), 0x0029, 0x03b7, 0}, 0x1f96 }, // U1F96 # GREEK SMALL LETTER ETA WITH PSILI AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Tilde), 0x0029, 0x03c9, 0}, 0x1fa6 }, // U1FA6 # GREEK SMALL LETTER OMEGA WITH PSILI AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Tilde), 0x0313, 0x0391, 0}, 0x1f8e }, // U1F8E # GREEK CAPITAL LETTER ALPHA WITH PSILI AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Tilde), 0x0313, 0x0397, 0}, 0x1f9e }, // U1F9E # GREEK CAPITAL LETTER ETA WITH PSILI AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Tilde), 0x0313, 0x03a9, 0}, 0x1fae }, // U1FAE # GREEK CAPITAL LETTER OMEGA WITH PSILI AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Tilde), 0x0313, 0x03b1, 0}, 0x1f86 }, // U1F86 # GREEK SMALL LETTER ALPHA WITH PSILI AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Tilde), 0x0313, 0x03b7, 0}, 0x1f96 }, // U1F96 # GREEK SMALL LETTER ETA WITH PSILI AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Tilde), 0x0313, 0x03c9, 0}, 0x1fa6 }, // U1FA6 # GREEK SMALL LETTER OMEGA WITH PSILI AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Tilde), 0x0314, 0x0391, 0}, 0x1f8f }, // U1F8F # GREEK CAPITAL LETTER ALPHA WITH DASIA AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Tilde), 0x0314, 0x0397, 0}, 0x1f9f }, // U1F9F # GREEK CAPITAL LETTER ETA WITH DASIA AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Tilde), 0x0314, 0x03a9, 0}, 0x1faf }, // U1FAF # GREEK CAPITAL LETTER OMEGA WITH DASIA AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Tilde), 0x0314, 0x03b1, 0}, 0x1f87 }, // U1F87 # GREEK SMALL LETTER ALPHA WITH DASIA AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Tilde), 0x0314, 0x03b7, 0}, 0x1f97 }, // U1F97 # GREEK SMALL LETTER ETA WITH DASIA AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Tilde), 0x0314, 0x03c9, 0}, 0x1fa7 }, // U1FA7 # GREEK SMALL LETTER OMEGA WITH DASIA AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Tilde), 0x03b1, 0, 0}, 0x1fb7 }, // U1FB7 # GREEK SMALL LETTER ALPHA WITH PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Tilde), 0x03b7, 0, 0}, 0x1fc7 }, // U1FC7 # GREEK SMALL LETTER ETA WITH PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Tilde), 0x03c9, 0, 0}, 0x1ff7 }, // U1FF7 # GREEK SMALL LETTER OMEGA WITH PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Tilde), 0x1f00, 0, 0}, 0x1f86 }, // U1F86 # GREEK SMALL LETTER ALPHA WITH PSILI AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Tilde), 0x1f01, 0, 0}, 0x1f87 }, // U1F87 # GREEK SMALL LETTER ALPHA WITH DASIA AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Tilde), 0x1f08, 0, 0}, 0x1f8e }, // U1F8E # GREEK CAPITAL LETTER ALPHA WITH PSILI AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Tilde), 0x1f09, 0, 0}, 0x1f8f }, // U1F8F # GREEK CAPITAL LETTER ALPHA WITH DASIA AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Tilde), 0x1f20, 0, 0}, 0x1f96 }, // U1F96 # GREEK SMALL LETTER ETA WITH PSILI AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Tilde), 0x1f21, 0, 0}, 0x1f97 }, // U1F97 # GREEK SMALL LETTER ETA WITH DASIA AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Tilde), 0x1f28, 0, 0}, 0x1f9e }, // U1F9E # GREEK CAPITAL LETTER ETA WITH PSILI AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Tilde), 0x1f29, 0, 0}, 0x1f9f }, // U1F9F # GREEK CAPITAL LETTER ETA WITH DASIA AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Tilde), 0x1f60, 0, 0}, 0x1fa6 }, // U1FA6 # GREEK SMALL LETTER OMEGA WITH PSILI AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Tilde), 0x1f61, 0, 0}, 0x1fa7 }, // U1FA7 # GREEK SMALL LETTER OMEGA WITH DASIA AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Tilde), 0x1f68, 0, 0}, 0x1fae }, // U1FAE # GREEK CAPITAL LETTER OMEGA WITH PSILI AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x03b9, UNITIZE(TQt::Key_Dead_Tilde), 0x1f69, 0, 0}, 0x1faf }, // U1FAF # GREEK CAPITAL LETTER OMEGA WITH DASIA AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x05b4, 0x05d9, 0, 0, 0}, 0xfb1d }, // UFB1D # HEBREW LETTER YOD WITH HIRIQ
    { {UNITIZE(TQt::Key_Multi_key), 0x05b7, 0x05d0, 0, 0, 0}, 0xfb2e }, // UFB2E # HEBREW LETTER ALEF WITH PATAH
    { {UNITIZE(TQt::Key_Multi_key), 0x05b7, 0x05f2, 0, 0, 0}, 0xfb1f }, // UFB1F # HEBREW LIGATURE YIDDISH YOD YOD PATAH
    { {UNITIZE(TQt::Key_Multi_key), 0x05b8, 0x05d0, 0, 0, 0}, 0xfb2f }, // UFB2F # HEBREW LETTER ALEF WITH TQAMATS
    { {UNITIZE(TQt::Key_Multi_key), 0x05b9, 0x05d5, 0, 0, 0}, 0xfb4b }, // UFB4B # HEBREW LETTER VAV WITH HOLAM
    { {UNITIZE(TQt::Key_Multi_key), 0x05bc, 0x05d0, 0, 0, 0}, 0xfb30 }, // UFB30 # HEBREW LETTER ALEF WITH MAPIQ
    { {UNITIZE(TQt::Key_Multi_key), 0x05bc, 0x05d1, 0, 0, 0}, 0xfb31 }, // UFB31 # HEBREW LETTER BET WITH DAGESH
    { {UNITIZE(TQt::Key_Multi_key), 0x05bc, 0x05d1, 0, 0, 0}, 0xfb31 }, // UFB31 # HEBREW LETTER BET WITH DAGESH
    { {UNITIZE(TQt::Key_Multi_key), 0x05bc, 0x05d2, 0, 0, 0}, 0xfb32 }, // UFB32 # HEBREW LETTER GIMEL WITH DAGESH
    { {UNITIZE(TQt::Key_Multi_key), 0x05bc, 0x05d2, 0, 0, 0}, 0xfb32 }, // UFB32 # HEBREW LETTER GIMEL WITH DAGESH
    { {UNITIZE(TQt::Key_Multi_key), 0x05bc, 0x05d3, 0, 0, 0}, 0xfb33 }, // UFB33 # HEBREW LETTER DALET WITH DAGESH
    { {UNITIZE(TQt::Key_Multi_key), 0x05bc, 0x05d3, 0, 0, 0}, 0xfb33 }, // UFB33 # HEBREW LETTER DALET WITH DAGESH
    { {UNITIZE(TQt::Key_Multi_key), 0x05bc, 0x05d4, 0, 0, 0}, 0xfb34 }, // UFB34 # HEBREW LETTER HE WITH MAPIQ
    { {UNITIZE(TQt::Key_Multi_key), 0x05bc, 0x05d5, 0, 0, 0}, 0xfb35 }, // UFB35 # HEBREW LETTER VAV WITH DAGESH
    { {UNITIZE(TQt::Key_Multi_key), 0x05bc, 0x05d6, 0, 0, 0}, 0xfb36 }, // UFB36 # HEBREW LETTER ZAYIN WITH DAGESH
    { {UNITIZE(TQt::Key_Multi_key), 0x05bc, 0x05d6, 0, 0, 0}, 0xfb36 }, // UFB36 # HEBREW LETTER ZAYIN WITH DAGESH
    { {UNITIZE(TQt::Key_Multi_key), 0x05bc, 0x05d8, 0, 0, 0}, 0xfb38 }, // UFB38 # HEBREW LETTER TET WITH DAGESH
    { {UNITIZE(TQt::Key_Multi_key), 0x05bc, 0x05d8, 0, 0, 0}, 0xfb38 }, // UFB38 # HEBREW LETTER TET WITH DAGESH
    { {UNITIZE(TQt::Key_Multi_key), 0x05bc, 0x05d9, 0, 0, 0}, 0xfb39 }, // UFB39 # HEBREW LETTER YOD WITH DAGESH
    { {UNITIZE(TQt::Key_Multi_key), 0x05bc, 0x05da, 0, 0, 0}, 0xfb3a }, // UFB3A # HEBREW LETTER FINAL KAF WITH DAGESH
    { {UNITIZE(TQt::Key_Multi_key), 0x05bc, 0x05db, 0, 0, 0}, 0xfb3b }, // UFB3B # HEBREW LETTER KAF WITH DAGESH
    { {UNITIZE(TQt::Key_Multi_key), 0x05bc, 0x05dc, 0, 0, 0}, 0xfb3c }, // UFB3C # HEBREW LETTER LAMED WITH DAGESH
    { {UNITIZE(TQt::Key_Multi_key), 0x05bc, 0x05de, 0, 0, 0}, 0xfb3e }, // UFB3E # HEBREW LETTER MEM WITH DAGESH
    { {UNITIZE(TQt::Key_Multi_key), 0x05bc, 0x05e0, 0, 0, 0}, 0xfb40 }, // UFB40 # HEBREW LETTER NUN WITH DAGESH
    { {UNITIZE(TQt::Key_Multi_key), 0x05bc, 0x05e1, 0, 0, 0}, 0xfb41 }, // UFB41 # HEBREW LETTER SAMEKH WITH DAGESH
    { {UNITIZE(TQt::Key_Multi_key), 0x05bc, 0x05e1, 0, 0, 0}, 0xfb41 }, // UFB41 # HEBREW LETTER SAMEKH WITH DAGESH
    { {UNITIZE(TQt::Key_Multi_key), 0x05bc, 0x05e3, 0, 0, 0}, 0xfb43 }, // UFB43 # HEBREW LETTER FINAL PE WITH DAGESH
    { {UNITIZE(TQt::Key_Multi_key), 0x05bc, 0x05e4, 0, 0, 0}, 0xfb44 }, // UFB44 # HEBREW LETTER PE WITH DAGESH
    { {UNITIZE(TQt::Key_Multi_key), 0x05bc, 0x05e6, 0, 0, 0}, 0xfb46 }, // UFB46 # HEBREW LETTER TSADI WITH DAGESH
    { {UNITIZE(TQt::Key_Multi_key), 0x05bc, 0x05e6, 0, 0, 0}, 0xfb46 }, // UFB46 # HEBREW LETTER TSADI WITH DAGESH
    { {UNITIZE(TQt::Key_Multi_key), 0x05bc, 0x05e7, 0, 0, 0}, 0xfb47 }, // UFB47 # HEBREW LETTER TQOF WITH DAGESH
    { {UNITIZE(TQt::Key_Multi_key), 0x05bc, 0x05e7, 0, 0, 0}, 0xfb47 }, // UFB47 # HEBREW LETTER TQOF WITH DAGESH
    { {UNITIZE(TQt::Key_Multi_key), 0x05bc, 0x05e8, 0, 0, 0}, 0xfb48 }, // UFB48 # HEBREW LETTER RESH WITH DAGESH
    { {UNITIZE(TQt::Key_Multi_key), 0x05bc, 0x05e9, 0, 0, 0}, 0xfb49 }, // UFB49 # HEBREW LETTER SHIN WITH DAGESH
    { {UNITIZE(TQt::Key_Multi_key), 0x05bc, 0x05ea, 0, 0, 0}, 0xfb4a }, // UFB4A # HEBREW LETTER TAV WITH DAGESH
    { {UNITIZE(TQt::Key_Multi_key), 0x05bc, 0x05ea, 0, 0, 0}, 0xfb4a }, // UFB4A # HEBREW LETTER TAV WITH DAGESH
    { {UNITIZE(TQt::Key_Multi_key), 0x05bf, 0x05d1, 0, 0, 0}, 0xfb4c }, // UFB4C # HEBREW LETTER BET WITH RAFE
    { {UNITIZE(TQt::Key_Multi_key), 0x05bf, 0x05d1, 0, 0, 0}, 0xfb4c }, // UFB4C # HEBREW LETTER BET WITH RAFE
    { {UNITIZE(TQt::Key_Multi_key), 0x05bf, 0x05db, 0, 0, 0}, 0xfb4d }, // UFB4D # HEBREW LETTER KAF WITH RAFE
    { {UNITIZE(TQt::Key_Multi_key), 0x05bf, 0x05e4, 0, 0, 0}, 0xfb4e }, // UFB4E # HEBREW LETTER PE WITH RAFE
    { {UNITIZE(TQt::Key_Multi_key), 0x05c1, 0x05bc, 0x05e9, 0, 0}, 0xfb2c }, // UFB2C # HEBREW LETTER SHIN WITH DAGESH AND SHIN DOT
    { {UNITIZE(TQt::Key_Multi_key), 0x05c1, 0x05e9, 0, 0, 0}, 0xfb2a }, // UFB2A # HEBREW LETTER SHIN WITH SHIN DOT
    { {UNITIZE(TQt::Key_Multi_key), 0x05c1, 0xfb49, 0, 0, 0}, 0xfb2c }, // UFB2C # HEBREW LETTER SHIN WITH DAGESH AND SHIN DOT
    { {UNITIZE(TQt::Key_Multi_key), 0x05c2, 0x05bc, 0x05e9, 0, 0}, 0xfb2d }, // UFB2D # HEBREW LETTER SHIN WITH DAGESH AND SIN DOT
    { {UNITIZE(TQt::Key_Multi_key), 0x05c2, 0x05e9, 0, 0, 0}, 0xfb2b }, // UFB2B # HEBREW LETTER SHIN WITH SIN DOT
    { {UNITIZE(TQt::Key_Multi_key), 0x05c2, 0xfb49, 0, 0, 0}, 0xfb2d }, // UFB2D # HEBREW LETTER SHIN WITH DAGESH AND SIN DOT
    { {UNITIZE(TQt::Key_Multi_key), 0x0653, 0x0627, 0, 0, 0}, 0x0622 }, // U0622 # ARABIC LETTER ALEF WITH MADDA ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0654, 0x0627, 0, 0, 0}, 0x0623 }, // U0623 # ARABIC LETTER ALEF WITH HAMZA ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0654, 0x0648, 0, 0, 0}, 0x0624 }, // U0624 # ARABIC LETTER WAW WITH HAMZA ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0654, 0x064a, 0, 0, 0}, 0x0626 }, // U0626 # ARABIC LETTER YEH WITH HAMZA ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0654, 0x06c1, 0, 0, 0}, 0x06c2 }, // U06C2 # ARABIC LETTER HEH GOAL WITH HAMZA ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0654, 0x06d2, 0, 0, 0}, 0x06d3 }, // U06D3 # ARABIC LETTER YEH BARREE WITH HAMZA ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0654, 0x06d5, 0, 0, 0}, 0x06c0 }, // U06C0 # ARABIC LETTER HEH WITH YEH ABOVE
    { {UNITIZE(TQt::Key_Multi_key), 0x0655, 0x0627, 0, 0, 0}, 0x0625 }, // U0625 # ARABIC LETTER ALEF WITH HAMZA BELOW
    { {UNITIZE(TQt::Key_Multi_key), 0x093c, 0x0915, 0, 0, 0}, 0x0958 }, // U0958 # DEVANAGARI LETTER QA
    { {UNITIZE(TQt::Key_Multi_key), 0x093c, 0x0916, 0, 0, 0}, 0x0959 }, // U0959 # DEVANAGARI LETTER KHHA
    { {UNITIZE(TQt::Key_Multi_key), 0x093c, 0x0917, 0, 0, 0}, 0x095a }, // U095A # DEVANAGARI LETTER GHHA
    { {UNITIZE(TQt::Key_Multi_key), 0x093c, 0x091c, 0, 0, 0}, 0x095b }, // U095B # DEVANAGARI LETTER ZA
    { {UNITIZE(TQt::Key_Multi_key), 0x093c, 0x0921, 0, 0, 0}, 0x095c }, // U095C # DEVANAGARI LETTER DDDHA
    { {UNITIZE(TQt::Key_Multi_key), 0x093c, 0x0922, 0, 0, 0}, 0x095d }, // U095D # DEVANAGARI LETTER RHA
    { {UNITIZE(TQt::Key_Multi_key), 0x093c, 0x0928, 0, 0, 0}, 0x0929 }, // U0929 # DEVANAGARI LETTER NNNA
    { {UNITIZE(TQt::Key_Multi_key), 0x093c, 0x092b, 0, 0, 0}, 0x095e }, // U095E # DEVANAGARI LETTER FA
    { {UNITIZE(TQt::Key_Multi_key), 0x093c, 0x092f, 0, 0, 0}, 0x095f }, // U095F # DEVANAGARI LETTER YYA
    { {UNITIZE(TQt::Key_Multi_key), 0x093c, 0x0930, 0, 0, 0}, 0x0931 }, // U0931 # DEVANAGARI LETTER RRA
    { {UNITIZE(TQt::Key_Multi_key), 0x093c, 0x0933, 0, 0, 0}, 0x0934 }, // U0934 # DEVANAGARI LETTER LLLA
    { {UNITIZE(TQt::Key_Multi_key), 0x09bc, 0x09a1, 0, 0, 0}, 0x09dc }, // U09DC # BENGALI LETTER RRA
    { {UNITIZE(TQt::Key_Multi_key), 0x09bc, 0x09a2, 0, 0, 0}, 0x09dd }, // U09DD # BENGALI LETTER RHA
    { {UNITIZE(TQt::Key_Multi_key), 0x09bc, 0x09af, 0, 0, 0}, 0x09df }, // U09DF # BENGALI LETTER YYA
    { {UNITIZE(TQt::Key_Multi_key), 0x09c7, 0x09be, 0, 0, 0}, 0x09cb }, // U09CB # BENGALI VOWEL SIGN O
    { {UNITIZE(TQt::Key_Multi_key), 0x09c7, 0x09d7, 0, 0, 0}, 0x09cc }, // U09CC # BENGALI VOWEL SIGN AU
    { {UNITIZE(TQt::Key_Multi_key), 0x0a3c, 0x0a16, 0, 0, 0}, 0x0a59 }, // U0A59 # GURMUKHI LETTER KHHA
    { {UNITIZE(TQt::Key_Multi_key), 0x0a3c, 0x0a17, 0, 0, 0}, 0x0a5a }, // U0A5A # GURMUKHI LETTER GHHA
    { {UNITIZE(TQt::Key_Multi_key), 0x0a3c, 0x0a1c, 0, 0, 0}, 0x0a5b }, // U0A5B # GURMUKHI LETTER ZA
    { {UNITIZE(TQt::Key_Multi_key), 0x0a3c, 0x0a2b, 0, 0, 0}, 0x0a5e }, // U0A5E # GURMUKHI LETTER FA
    { {UNITIZE(TQt::Key_Multi_key), 0x0a3c, 0x0a32, 0, 0, 0}, 0x0a33 }, // U0A33 # GURMUKHI LETTER LLA
    { {UNITIZE(TQt::Key_Multi_key), 0x0a3c, 0x0a38, 0, 0, 0}, 0x0a36 }, // U0A36 # GURMUKHI LETTER SHA
    { {UNITIZE(TQt::Key_Multi_key), 0x0b3c, 0x0b21, 0, 0, 0}, 0x0b5c }, // U0B5C # ORIYA LETTER RRA
    { {UNITIZE(TQt::Key_Multi_key), 0x0b3c, 0x0b22, 0, 0, 0}, 0x0b5d }, // U0B5D # ORIYA LETTER RHA
    { {UNITIZE(TQt::Key_Multi_key), 0x0b47, 0x0b3e, 0, 0, 0}, 0x0b4b }, // U0B4B # ORIYA VOWEL SIGN O
    { {UNITIZE(TQt::Key_Multi_key), 0x0b47, 0x0b56, 0, 0, 0}, 0x0b48 }, // U0B48 # ORIYA VOWEL SIGN AI
    { {UNITIZE(TQt::Key_Multi_key), 0x0b47, 0x0b57, 0, 0, 0}, 0x0b4c }, // U0B4C # ORIYA VOWEL SIGN AU
    { {UNITIZE(TQt::Key_Multi_key), 0x0bc6, 0x0bbe, 0, 0, 0}, 0x0bca }, // U0BCA # TAMIL VOWEL SIGN O
    { {UNITIZE(TQt::Key_Multi_key), 0x0bc6, 0x0bd7, 0, 0, 0}, 0x0bcc }, // U0BCC # TAMIL VOWEL SIGN AU
    { {UNITIZE(TQt::Key_Multi_key), 0x0bc7, 0x0bbe, 0, 0, 0}, 0x0bcb }, // U0BCB # TAMIL VOWEL SIGN OO
    { {UNITIZE(TQt::Key_Multi_key), 0x0bd7, 0x0b92, 0, 0, 0}, 0x0b94 }, // U0B94 # TAMIL LETTER AU
    { {UNITIZE(TQt::Key_Multi_key), 0x0c46, 0x0c56, 0, 0, 0}, 0x0c48 }, // U0C48 # TELUGU VOWEL SIGN AI
    { {UNITIZE(TQt::Key_Multi_key), 0x0cbf, 0x0cd5, 0, 0, 0}, 0x0cc0 }, // U0CC0 # KANNADA VOWEL SIGN II
    { {UNITIZE(TQt::Key_Multi_key), 0x0cc6, 0x0cc2, 0, 0, 0}, 0x0cca }, // U0CCA # KANNADA VOWEL SIGN O
    { {UNITIZE(TQt::Key_Multi_key), 0x0cc6, 0x0cc2, 0x0cd5, 0, 0}, 0x0ccb }, // U0CCB # KANNADA VOWEL SIGN OO
    { {UNITIZE(TQt::Key_Multi_key), 0x0cc6, 0x0cd5, 0, 0, 0}, 0x0cc7 }, // U0CC7 # KANNADA VOWEL SIGN EE
    { {UNITIZE(TQt::Key_Multi_key), 0x0cc6, 0x0cd6, 0, 0, 0}, 0x0cc8 }, // U0CC8 # KANNADA VOWEL SIGN AI
    { {UNITIZE(TQt::Key_Multi_key), 0x0cca, 0x0cd5, 0, 0, 0}, 0x0ccb }, // U0CCB # KANNADA VOWEL SIGN OO
    { {UNITIZE(TQt::Key_Multi_key), 0x0d46, 0x0d3e, 0, 0, 0}, 0x0d4a }, // U0D4A # MALAYALAM VOWEL SIGN O
    { {UNITIZE(TQt::Key_Multi_key), 0x0d46, 0x0d57, 0, 0, 0}, 0x0d4c }, // U0D4C # MALAYALAM VOWEL SIGN AU
    { {UNITIZE(TQt::Key_Multi_key), 0x0d47, 0x0d3e, 0, 0, 0}, 0x0d4b }, // U0D4B # MALAYALAM VOWEL SIGN OO
    { {UNITIZE(TQt::Key_Multi_key), 0x0dd9, 0x0dca, 0, 0, 0}, 0x0dda }, // U0DDA # SINHALA VOWEL SIGN DIGA KOMBUVA
    { {UNITIZE(TQt::Key_Multi_key), 0x0dd9, 0x0dcf, 0, 0, 0}, 0x0ddc }, // U0DDC # SINHALA VOWEL SIGN KOMBUVA HAA AELA-PILLA
    { {UNITIZE(TQt::Key_Multi_key), 0x0dd9, 0x0dcf, 0x0dca, 0, 0}, 0x0ddd }, // U0DDD # SINHALA VOWEL SIGN KOMBUVA HAA DIGA AELA-PILLA
    { {UNITIZE(TQt::Key_Multi_key), 0x0dd9, 0x0ddf, 0, 0, 0}, 0x0dde }, // U0DDE # SINHALA VOWEL SIGN KOMBUVA HAA GAYANUKITTA
    { {UNITIZE(TQt::Key_Multi_key), 0x0ddc, 0x0dca, 0, 0, 0}, 0x0ddd }, // U0DDD # SINHALA VOWEL SIGN KOMBUVA HAA DIGA AELA-PILLA
    { {UNITIZE(TQt::Key_Multi_key), 0x0f71, 0x0f72, 0, 0, 0}, 0x0f73 }, // U0F73 # TIBETAN VOWEL SIGN II
    { {UNITIZE(TQt::Key_Multi_key), 0x0f71, 0x0f74, 0, 0, 0}, 0x0f75 }, // U0F75 # TIBETAN VOWEL SIGN UU
    { {UNITIZE(TQt::Key_Multi_key), 0x0f71, 0x0f80, 0, 0, 0}, 0x0f81 }, // U0F81 # TIBETAN VOWEL SIGN REVERSED II
    { {UNITIZE(TQt::Key_Multi_key), 0x0f90, 0x0fb5, 0, 0, 0}, 0x0fb9 }, // U0FB9 # TIBETAN SUBJOINED LETTER KSSA
    { {UNITIZE(TQt::Key_Multi_key), 0x0f92, 0x0fb7, 0, 0, 0}, 0x0f93 }, // U0F93 # TIBETAN SUBJOINED LETTER GHA
    { {UNITIZE(TQt::Key_Multi_key), 0x0f9c, 0x0fb7, 0, 0, 0}, 0x0f9d }, // U0F9D # TIBETAN SUBJOINED LETTER DDHA
    { {UNITIZE(TQt::Key_Multi_key), 0x0fa1, 0x0fb7, 0, 0, 0}, 0x0fa2 }, // U0FA2 # TIBETAN SUBJOINED LETTER DHA
    { {UNITIZE(TQt::Key_Multi_key), 0x0fa6, 0x0fb7, 0, 0, 0}, 0x0fa7 }, // U0FA7 # TIBETAN SUBJOINED LETTER BHA
    { {UNITIZE(TQt::Key_Multi_key), 0x0fab, 0x0fb7, 0, 0, 0}, 0x0fac }, // U0FAC # TIBETAN SUBJOINED LETTER DZHA
    { {UNITIZE(TQt::Key_Multi_key), 0x0fb2, 0x0f80, 0, 0, 0}, 0x0f76 }, // U0F76 # TIBETAN VOWEL SIGN VOCALIC R
    { {UNITIZE(TQt::Key_Multi_key), 0x0fb3, 0x0f80, 0, 0, 0}, 0x0f78 }, // U0F78 # TIBETAN VOWEL SIGN VOCALIC L
    { {UNITIZE(TQt::Key_Multi_key), 0x0fb5, 0x0f40, 0, 0, 0}, 0x0f69 }, // U0F69 # TIBETAN LETTER KSSA
    { {UNITIZE(TQt::Key_Multi_key), 0x0fb7, 0x0f42, 0, 0, 0}, 0x0f43 }, // U0F43 # TIBETAN LETTER GHA
    { {UNITIZE(TQt::Key_Multi_key), 0x0fb7, 0x0f4c, 0, 0, 0}, 0x0f4d }, // U0F4D # TIBETAN LETTER DDHA
    { {UNITIZE(TQt::Key_Multi_key), 0x0fb7, 0x0f51, 0, 0, 0}, 0x0f52 }, // U0F52 # TIBETAN LETTER DHA
    { {UNITIZE(TQt::Key_Multi_key), 0x0fb7, 0x0f56, 0, 0, 0}, 0x0f57 }, // U0F57 # TIBETAN LETTER BHA
    { {UNITIZE(TQt::Key_Multi_key), 0x0fb7, 0x0f5b, 0, 0, 0}, 0x0f5c }, // U0F5C # TIBETAN LETTER DZHA
    { {UNITIZE(TQt::Key_Multi_key), 0x102e, 0x1025, 0, 0, 0}, 0x1026 }, // U1026 # MYANMAR LETTER UU
    { {UNITIZE(TQt::Key_Multi_key), 0x1100, 0x1100, 0, 0, 0}, 0x1101 }, // U1101	 # �� ��   = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x1102, 0x1100, 0, 0, 0}, 0x1113 }, // U1113	 # �� ��   = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x1102, 0x1102, 0, 0, 0}, 0x1114 }, // U1114	 # �� ��   = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x1102, 0x1103, 0, 0, 0}, 0x1115 }, // U1115	 # �� ��   = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x1102, 0x1107, 0, 0, 0}, 0x1116 }, // U1116	 # �� ��   = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x1103, 0x1100, 0, 0, 0}, 0x1117 }, // U1117	 # �� ��   = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x1103, 0x1103, 0, 0, 0}, 0x1104 }, // U1104	 # �� ��   = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x1105, 0x1102, 0, 0, 0}, 0x1118 }, // U1118	 # �� ��   = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x1105, 0x1105, 0, 0, 0}, 0x1119 }, // U1119	 # �� ��   = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x1105, 0x110b, 0, 0, 0}, 0x111b }, // U111b	 # �� ��   = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x1105, 0x1112, 0, 0, 0}, 0x111a }, // U111a	 # �� ��   = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x1106, 0x1107, 0, 0, 0}, 0x111c }, // U111c	 # �� ��   = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x1106, 0x110b, 0, 0, 0}, 0x111d }, // U111d	 # �� ��   = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x1107, 0x1100, 0, 0, 0}, 0x111e }, // U111e	 # �� ��   = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x1107, 0x1102, 0, 0, 0}, 0x111f }, // U111f	 # �� ��   = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x1107, 0x1103, 0, 0, 0}, 0x1120 }, // U1120	 # �� ��   = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x1107, 0x1107, 0, 0, 0}, 0x1108 }, // U1108	 # �� ��   = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x1107, 0x1107, 0x110b, 0, 0}, 0x112c }, // U112c	 # �� �� ��  = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x1107, 0x1109, 0, 0, 0}, 0x1121 }, // U1121	 # �� ��   = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x1107, 0x1109, 0x1100, 0, 0}, 0x1122 }, // U1122	 # �� �� ��  = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x1107, 0x1109, 0x1103, 0, 0}, 0x1123 }, // U1123	 # �� �� ��  = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x1107, 0x1109, 0x1107, 0, 0}, 0x1124 }, // U1124	 # �� �� ��  = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x1107, 0x1109, 0x1109, 0, 0}, 0x1125 }, // U1125	 # �� �� ��  = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x1107, 0x1109, 0x110c, 0, 0}, 0x1126 }, // U1126	 # �� �� ��  = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x1107, 0x110a, 0, 0, 0}, 0x1125 }, // U1125	 # �� ��   = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x1107, 0x110b, 0, 0, 0}, 0x112b }, // U112b	 # �� ��   = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x1107, 0x110c, 0, 0, 0}, 0x1127 }, // U1127	 # �� ��   = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x1107, 0x110e, 0, 0, 0}, 0x1128 }, // U1128	 # �� ��   = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x1107, 0x1110, 0, 0, 0}, 0x1129 }, // U1129	 # �� ��   = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x1107, 0x1111, 0, 0, 0}, 0x112a }, // U112a	 # �� ��   = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x1107, 0x112b, 0, 0, 0}, 0x112c }, // U112c	 # �� ��   = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x1107, 0x112d, 0, 0, 0}, 0x1122 }, // U1122	 # �� ��   = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x1107, 0x112f, 0, 0, 0}, 0x1123 }, // U1123	 # �� ��   = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x1107, 0x1132, 0, 0, 0}, 0x1124 }, // U1124	 # �� ��   = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x1107, 0x1136, 0, 0, 0}, 0x1126 }, // U1126	 # �� ��   = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x1108, 0x110b, 0, 0, 0}, 0x112c }, // U112c	 # �� ��   = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x1109, 0x1100, 0, 0, 0}, 0x112d }, // U112d	 # �� ��   = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x1109, 0x1102, 0, 0, 0}, 0x112e }, // U112e	 # �� ��   = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x1109, 0x1103, 0, 0, 0}, 0x112f }, // U112f	 # �� ��   = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x1109, 0x1105, 0, 0, 0}, 0x1130 }, // U1130	 # �� ��   = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x1109, 0x1106, 0, 0, 0}, 0x1131 }, // U1131	 # �� ��   = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x1109, 0x1107, 0, 0, 0}, 0x1132 }, // U1132	 # �� ��   = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x1109, 0x1107, 0x1100, 0, 0}, 0x1133 }, // U1133	 # �� �� ��  = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x1109, 0x1109, 0, 0, 0}, 0x110a }, // U110a	 # �� ��   = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x1109, 0x1109, 0x1109, 0, 0}, 0x1134 }, // U1134	 # �� �� ��  = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x1109, 0x110a, 0, 0, 0}, 0x1134 }, // U1134	 # �� ��   = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x1109, 0x110b, 0, 0, 0}, 0x1135 }, // U1135	 # �� ��   = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x1109, 0x110c, 0, 0, 0}, 0x1136 }, // U1136	 # �� ��   = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x1109, 0x110e, 0, 0, 0}, 0x1137 }, // U1137	 # �� ��   = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x1109, 0x110f, 0, 0, 0}, 0x1138 }, // U1138	 # �� ��   = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x1109, 0x1110, 0, 0, 0}, 0x1139 }, // U1139	 # �� ��   = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x1109, 0x1111, 0, 0, 0}, 0x113a }, // U113a	 # �� ��   = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x1109, 0x1112, 0, 0, 0}, 0x113b }, // U113b	 # �� ��   = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x1109, 0x111e, 0, 0, 0}, 0x1133 }, // U1133	 # �� ��   = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x110a, 0x1109, 0, 0, 0}, 0x1134 }, // U1134	 # �� ��   = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x110b, 0x1100, 0, 0, 0}, 0x1141 }, // U1141	 # �� ��   = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x110b, 0x1103, 0, 0, 0}, 0x1142 }, // U1142	 # �� ��   = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x110b, 0x1106, 0, 0, 0}, 0x1143 }, // U1143	 # �� ��   = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x110b, 0x1107, 0, 0, 0}, 0x1144 }, // U1144	 # �� ��   = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x110b, 0x1109, 0, 0, 0}, 0x1145 }, // U1145	 # �� ��   = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x110b, 0x110b, 0, 0, 0}, 0x1147 }, // U1147	 # �� ��   = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x110b, 0x110c, 0, 0, 0}, 0x1148 }, // U1148	 # �� ��   = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x110b, 0x110e, 0, 0, 0}, 0x1149 }, // U1149	 # �� ��   = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x110b, 0x1110, 0, 0, 0}, 0x114a }, // U114a	 # �� ��   = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x110b, 0x1111, 0, 0, 0}, 0x114b }, // U114b	 # �� ��   = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x110b, 0x1140, 0, 0, 0}, 0x1146 }, // U1146	 # �� ��   = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x110c, 0x110b, 0, 0, 0}, 0x114d }, // U114d	 # �� ��   = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x110c, 0x110c, 0, 0, 0}, 0x110d }, // U110d	 # �� ��   = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x110e, 0x110f, 0, 0, 0}, 0x1152 }, // U1152	 # �� ��   = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x110e, 0x1112, 0, 0, 0}, 0x1153 }, // U1153	 # �� ��   = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x1111, 0x1107, 0, 0, 0}, 0x1156 }, // U1156	 # �� ��   = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x1111, 0x110b, 0, 0, 0}, 0x1157 }, // U1157	 # �� ��   = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x1112, 0x1112, 0, 0, 0}, 0x1158 }, // U1158	 # �� ��   = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x1121, 0x1100, 0, 0, 0}, 0x1122 }, // U1122	 # �� ��   = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x1121, 0x1103, 0, 0, 0}, 0x1123 }, // U1123	 # �� ��   = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x1121, 0x1107, 0, 0, 0}, 0x1124 }, // U1124	 # �� ��   = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x1121, 0x1109, 0, 0, 0}, 0x1125 }, // U1125	 # �� ��   = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x1121, 0x110c, 0, 0, 0}, 0x1126 }, // U1126	 # �� ��   = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x1132, 0x1100, 0, 0, 0}, 0x1133 }, // U1133	 # �� ��   = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x113c, 0x113c, 0, 0, 0}, 0x113d }, // U113d	 # �� ��   = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x113e, 0x113e, 0, 0, 0}, 0x113f }, // U113f	 # �� ��   = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x114e, 0x114e, 0, 0, 0}, 0x114f }, // U114f	 # �� ��   = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x1150, 0x1150, 0, 0, 0}, 0x1151 }, // U1151	 # �� ��   = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x1161, 0x1169, 0, 0, 0}, 0x1176 }, // U1176	 # �� ��   = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x1161, 0x116e, 0, 0, 0}, 0x1177 }, // U1177	 # �� ��   = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x1161, 0x1175, 0, 0, 0}, 0x1162 }, // U1162	 # �� ��   = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x1163, 0x1169, 0, 0, 0}, 0x1178 }, // U1178	 # �� ��   = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x1163, 0x116d, 0, 0, 0}, 0x1179 }, // U1179	 # �� ��   = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x1163, 0x1175, 0, 0, 0}, 0x1164 }, // U1164	 # �� ��   = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x1165, 0x1169, 0, 0, 0}, 0x117a }, // U117a	 # �� ��   = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x1165, 0x116e, 0, 0, 0}, 0x117b }, // U117b	 # �� ��   = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x1165, 0x1173, 0, 0, 0}, 0x117c }, // U117c	 # �� ��   = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x1165, 0x1175, 0, 0, 0}, 0x1166 }, // U1166	 # �� ��   = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x1167, 0x1169, 0, 0, 0}, 0x117d }, // U117d	 # �� ��   = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x1167, 0x116e, 0, 0, 0}, 0x117e }, // U117e	 # �� ��   = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x1167, 0x1175, 0, 0, 0}, 0x1168 }, // U1168	 # �� ��   = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x1169, 0x1161, 0, 0, 0}, 0x116a }, // U116a	 # �� ��   = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x1169, 0x1161, 0x1175, 0, 0}, 0x116b }, // U116b	 # �� �� ��  = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x1169, 0x1162, 0, 0, 0}, 0x116b }, // U116b	 # �� ��   = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x1169, 0x1165, 0, 0, 0}, 0x117f }, // U117f	 # �� ��   = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x1169, 0x1166, 0, 0, 0}, 0x1180 }, // U1180	 # �� ��   = ߠ
    { {UNITIZE(TQt::Key_Multi_key), 0x1169, 0x1168, 0, 0, 0}, 0x1181 }, // U1181	 # �� ��   = ߡ
    { {UNITIZE(TQt::Key_Multi_key), 0x1169, 0x1169, 0, 0, 0}, 0x1182 }, // U1182	 # �� ��   = ߢ
    { {UNITIZE(TQt::Key_Multi_key), 0x1169, 0x116e, 0, 0, 0}, 0x1183 }, // U1183	 # �� ��   = ߣ
    { {UNITIZE(TQt::Key_Multi_key), 0x1169, 0x1175, 0, 0, 0}, 0x116c }, // U116c	 # �� ��   = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x116a, 0x1175, 0, 0, 0}, 0x116b }, // U116b	 # �� ��   = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x116d, 0x1163, 0, 0, 0}, 0x1184 }, // U1184	 # �� ��   = ߤ
    { {UNITIZE(TQt::Key_Multi_key), 0x116d, 0x1164, 0, 0, 0}, 0x1185 }, // U1185	 # �� ��   = ߥ
    { {UNITIZE(TQt::Key_Multi_key), 0x116d, 0x1167, 0, 0, 0}, 0x1186 }, // U1186	 # �� ��   = ߦ
    { {UNITIZE(TQt::Key_Multi_key), 0x116d, 0x1169, 0, 0, 0}, 0x1187 }, // U1187	 # �� ��   = ߧ
    { {UNITIZE(TQt::Key_Multi_key), 0x116d, 0x1175, 0, 0, 0}, 0x1188 }, // U1188	 # �� ��   = ߨ
    { {UNITIZE(TQt::Key_Multi_key), 0x116e, 0x1161, 0, 0, 0}, 0x1189 }, // U1189	 # �� ��   = ߩ
    { {UNITIZE(TQt::Key_Multi_key), 0x116e, 0x1162, 0, 0, 0}, 0x118a }, // U118a	 # �� ��   = ߪ
    { {UNITIZE(TQt::Key_Multi_key), 0x116e, 0x1165, 0, 0, 0}, 0x116f }, // U116f	 # �� ��   = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x116e, 0x1165, 0x1173, 0, 0}, 0x118b }, // U118b	 # �� �� ��  = ߫
    { {UNITIZE(TQt::Key_Multi_key), 0x116e, 0x1165, 0x1175, 0, 0}, 0x1170 }, // U1170	 # �� �� ��  = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x116e, 0x1166, 0, 0, 0}, 0x1170 }, // U1170	 # �� ��   = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x116e, 0x1168, 0, 0, 0}, 0x118c }, // U118c	 # �� ��   = ߬
    { {UNITIZE(TQt::Key_Multi_key), 0x116e, 0x116e, 0, 0, 0}, 0x118d }, // U118d	 # �� ��   = ߭
    { {UNITIZE(TQt::Key_Multi_key), 0x116e, 0x1175, 0, 0, 0}, 0x1171 }, // U1171	 # �� ��   = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x116e, 0x117c, 0, 0, 0}, 0x118b }, // U118b	 # �� ��   = ߫
    { {UNITIZE(TQt::Key_Multi_key), 0x116f, 0x1173, 0, 0, 0}, 0x118b }, // U118b	 # �� ��   = ߫
    { {UNITIZE(TQt::Key_Multi_key), 0x116f, 0x1175, 0, 0, 0}, 0x1170 }, // U1170	 # �� ��   = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x1172, 0x1161, 0, 0, 0}, 0x118e }, // U118e	 # �� ��   = ߮
    { {UNITIZE(TQt::Key_Multi_key), 0x1172, 0x1165, 0, 0, 0}, 0x118f }, // U118f	 # �� ��   = ߯
    { {UNITIZE(TQt::Key_Multi_key), 0x1172, 0x1166, 0, 0, 0}, 0x1190 }, // U1190	 # �� ��   = ߰
    { {UNITIZE(TQt::Key_Multi_key), 0x1172, 0x1167, 0, 0, 0}, 0x1191 }, // U1191	 # �� ��   = ߱
    { {UNITIZE(TQt::Key_Multi_key), 0x1172, 0x1168, 0, 0, 0}, 0x1192 }, // U1192	 # �� ��   = ߲
    { {UNITIZE(TQt::Key_Multi_key), 0x1172, 0x116e, 0, 0, 0}, 0x1193 }, // U1193	 # �� ��   = ߳
    { {UNITIZE(TQt::Key_Multi_key), 0x1172, 0x1175, 0, 0, 0}, 0x1194 }, // U1194	 # �� ��   = ߴ
    { {UNITIZE(TQt::Key_Multi_key), 0x1173, 0x116e, 0, 0, 0}, 0x1195 }, // U1195	 # �� ��   = ߵ
    { {UNITIZE(TQt::Key_Multi_key), 0x1173, 0x1173, 0, 0, 0}, 0x1196 }, // U1196	 # �� ��   = ߶
    { {UNITIZE(TQt::Key_Multi_key), 0x1173, 0x1175, 0, 0, 0}, 0x1174 }, // U1174	 # �� ��   = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x1174, 0x116e, 0, 0, 0}, 0x1197 }, // U1197	 # �� ��   = ߷
    { {UNITIZE(TQt::Key_Multi_key), 0x1175, 0x1161, 0, 0, 0}, 0x1198 }, // U1198	 # �� ��   = ߸
    { {UNITIZE(TQt::Key_Multi_key), 0x1175, 0x1163, 0, 0, 0}, 0x1199 }, // U1199	 # �� ��   = ߹
    { {UNITIZE(TQt::Key_Multi_key), 0x1175, 0x1169, 0, 0, 0}, 0x119a }, // U119a	 # �� ��   = ߺ
    { {UNITIZE(TQt::Key_Multi_key), 0x1175, 0x116e, 0, 0, 0}, 0x119b }, // U119b	 # �� ��   = ߻
    { {UNITIZE(TQt::Key_Multi_key), 0x1175, 0x1173, 0, 0, 0}, 0x119c }, // U119c	 # �� ��   = ߼
    { {UNITIZE(TQt::Key_Multi_key), 0x1175, 0x119e, 0, 0, 0}, 0x119d }, // U119d	 # �� ߾   = ߽
    { {UNITIZE(TQt::Key_Multi_key), 0x119e, 0x1165, 0, 0, 0}, 0x119f }, // U119f	 # ߾ ��   = ߿
    { {UNITIZE(TQt::Key_Multi_key), 0x119e, 0x116e, 0, 0, 0}, 0x11a0 }, // U11a0	 # ߾ ��   = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x119e, 0x1175, 0, 0, 0}, 0x11a1 }, // U11a1	 # ߾ ��   = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x119e, 0x119e, 0, 0, 0}, 0x11a2 }, // U11a2	 # ߾ ߾   = ��
    { {UNITIZE(TQt::Key_Multi_key), 0x11a8, 0x11a8, 0, 0, 0}, 0x11a9 }, // U11a9	 # ��� ���   = ���
    { {UNITIZE(TQt::Key_Multi_key), 0x11a8, 0x11af, 0, 0, 0}, 0x11c3 }, // U11c3	 # ��� ���   = ���
    { {UNITIZE(TQt::Key_Multi_key), 0x11a8, 0x11ba, 0, 0, 0}, 0x11aa }, // U11aa	 # ��� ���   = ���
    { {UNITIZE(TQt::Key_Multi_key), 0x11a8, 0x11ba, 0x11a8, 0, 0}, 0x11c4 }, // U11c4	 # ��� ��� ���  = ���
    { {UNITIZE(TQt::Key_Multi_key), 0x11a8, 0x11e7, 0, 0, 0}, 0x11c4 }, // U11c4	 # ��� ���   = ���
    { {UNITIZE(TQt::Key_Multi_key), 0x11aa, 0x11a8, 0, 0, 0}, 0x11c4 }, // U11c4	 # ��� ���   = ���
    { {UNITIZE(TQt::Key_Multi_key), 0x11ab, 0x11a8, 0, 0, 0}, 0x11c5 }, // U11c5	 # ��� ���   = ���
    { {UNITIZE(TQt::Key_Multi_key), 0x11ab, 0x11ae, 0, 0, 0}, 0x11c6 }, // U11c6	 # ��� ���   = ���
    { {UNITIZE(TQt::Key_Multi_key), 0x11ab, 0x11ba, 0, 0, 0}, 0x11c7 }, // U11c7	 # ��� ���   = ���
    { {UNITIZE(TQt::Key_Multi_key), 0x11ab, 0x11bd, 0, 0, 0}, 0x11ac }, // U11ac	 # ��� ���   = ���
    { {UNITIZE(TQt::Key_Multi_key), 0x11ab, 0x11c0, 0, 0, 0}, 0x11c9 }, // U11c9	 # ��� ���   = ���
    { {UNITIZE(TQt::Key_Multi_key), 0x11ab, 0x11c2, 0, 0, 0}, 0x11ad }, // U11ad	 # ��� ���   = ���
    { {UNITIZE(TQt::Key_Multi_key), 0x11ab, 0x11eb, 0, 0, 0}, 0x11c8 }, // U11c8	 # ��� ���   = ���
    { {UNITIZE(TQt::Key_Multi_key), 0x11ae, 0x11a8, 0, 0, 0}, 0x11ca }, // U11ca	 # ��� ���   = ���
    { {UNITIZE(TQt::Key_Multi_key), 0x11ae, 0x11af, 0, 0, 0}, 0x11cb }, // U11cb	 # ��� ���   = ���
    { {UNITIZE(TQt::Key_Multi_key), 0x11af, 0x11a8, 0, 0, 0}, 0x11b0 }, // U11b0	 # ��� ���   = ���
    { {UNITIZE(TQt::Key_Multi_key), 0x11af, 0x11a8, 0x11ba, 0, 0}, 0x11cc }, // U11cc	 # ��� ��� ���  = ���
    { {UNITIZE(TQt::Key_Multi_key), 0x11af, 0x11aa, 0, 0, 0}, 0x11cc }, // U11cc	 # ��� ���   = ���
    { {UNITIZE(TQt::Key_Multi_key), 0x11af, 0x11ab, 0, 0, 0}, 0x11cd }, // U11cd	 # ��� ���   = ���
    { {UNITIZE(TQt::Key_Multi_key), 0x11af, 0x11ae, 0, 0, 0}, 0x11ce }, // U11ce	 # ��� ���   = ���
    { {UNITIZE(TQt::Key_Multi_key), 0x11af, 0x11ae, 0x11c2, 0, 0}, 0x11cf }, // U11cf	 # ��� ��� ���  = ���
    { {UNITIZE(TQt::Key_Multi_key), 0x11af, 0x11af, 0, 0, 0}, 0x11d0 }, // U11d0	 # ��� ���   = ���
    { {UNITIZE(TQt::Key_Multi_key), 0x11af, 0x11b7, 0, 0, 0}, 0x11b1 }, // U11b1	 # ��� ���   = ���
    { {UNITIZE(TQt::Key_Multi_key), 0x11af, 0x11b7, 0x11a8, 0, 0}, 0x11d1 }, // U11d1	 # ��� ��� ���  = ���
    { {UNITIZE(TQt::Key_Multi_key), 0x11af, 0x11b7, 0x11ba, 0, 0}, 0x11d2 }, // U11d2	 # ��� ��� ���  = ���
    { {UNITIZE(TQt::Key_Multi_key), 0x11af, 0x11b8, 0, 0, 0}, 0x11b2 }, // U11b2	 # ��� ���   = ���
    { {UNITIZE(TQt::Key_Multi_key), 0x11af, 0x11b8, 0x11ba, 0, 0}, 0x11d3 }, // U11d3	 # ��� ��� ���  = ���
    { {UNITIZE(TQt::Key_Multi_key), 0x11af, 0x11b8, 0x11bc, 0, 0}, 0x11d5 }, // U11d5	 # ��� ��� ���  = ���
    { {UNITIZE(TQt::Key_Multi_key), 0x11af, 0x11b8, 0x11c2, 0, 0}, 0x11d4 }, // U11d4	 # ��� ��� ���  = ���
    { {UNITIZE(TQt::Key_Multi_key), 0x11af, 0x11b9, 0, 0, 0}, 0x11d3 }, // U11d3	 # ��� ���   = ���
    { {UNITIZE(TQt::Key_Multi_key), 0x11af, 0x11ba, 0, 0, 0}, 0x11b3 }, // U11b3	 # ��� ���   = ���
    { {UNITIZE(TQt::Key_Multi_key), 0x11af, 0x11ba, 0x11ba, 0, 0}, 0x11d6 }, // U11d6	 # ��� ��� ���  = ���
    { {UNITIZE(TQt::Key_Multi_key), 0x11af, 0x11bb, 0, 0, 0}, 0x11d6 }, // U11d6	 # ��� ���   = ���
    { {UNITIZE(TQt::Key_Multi_key), 0x11af, 0x11bf, 0, 0, 0}, 0x11d8 }, // U11d8	 # ��� ���   = ���
    { {UNITIZE(TQt::Key_Multi_key), 0x11af, 0x11c0, 0, 0, 0}, 0x11b4 }, // U11b4	 # ��� ���   = ���
    { {UNITIZE(TQt::Key_Multi_key), 0x11af, 0x11c1, 0, 0, 0}, 0x11b5 }, // U11b5	 # ��� ���   = ���
    { {UNITIZE(TQt::Key_Multi_key), 0x11af, 0x11c2, 0, 0, 0}, 0x11b6 }, // U11b6	 # ��� ���   = ���
    { {UNITIZE(TQt::Key_Multi_key), 0x11af, 0x11da, 0, 0, 0}, 0x11d1 }, // U11d1	 # ��� ���   = ���
    { {UNITIZE(TQt::Key_Multi_key), 0x11af, 0x11dd, 0, 0, 0}, 0x11d2 }, // U11d2	 # ��� ���   = ���
    { {UNITIZE(TQt::Key_Multi_key), 0x11af, 0x11e5, 0, 0, 0}, 0x11d4 }, // U11d4	 # ��� ���   = ���
    { {UNITIZE(TQt::Key_Multi_key), 0x11af, 0x11e6, 0, 0, 0}, 0x11d5 }, // U11d5	 # ��� ���   = ���
    { {UNITIZE(TQt::Key_Multi_key), 0x11af, 0x11eb, 0, 0, 0}, 0x11d7 }, // U11d7	 # ��� ���   = ���
    { {UNITIZE(TQt::Key_Multi_key), 0x11af, 0x11f9, 0, 0, 0}, 0x11d9 }, // U11d9	 # ��� ���   = ���
    { {UNITIZE(TQt::Key_Multi_key), 0x11b0, 0x11ba, 0, 0, 0}, 0x11cc }, // U11cc	 # ��� ���   = ���
    { {UNITIZE(TQt::Key_Multi_key), 0x11b1, 0x11a8, 0, 0, 0}, 0x11d1 }, // U11d1	 # ��� ���   = ���
    { {UNITIZE(TQt::Key_Multi_key), 0x11b1, 0x11ba, 0, 0, 0}, 0x11d2 }, // U11d2	 # ��� ���   = ���
    { {UNITIZE(TQt::Key_Multi_key), 0x11b2, 0x11ba, 0, 0, 0}, 0x11d3 }, // U11d3	 # ��� ���   = ���
    { {UNITIZE(TQt::Key_Multi_key), 0x11b2, 0x11bc, 0, 0, 0}, 0x11d5 }, // U11d5	 # ��� ���   = ���
    { {UNITIZE(TQt::Key_Multi_key), 0x11b2, 0x11c2, 0, 0, 0}, 0x11d4 }, // U11d4	 # ��� ���   = ���
    { {UNITIZE(TQt::Key_Multi_key), 0x11b3, 0x11ba, 0, 0, 0}, 0x11d6 }, // U11d6	 # ��� ���   = ���
    { {UNITIZE(TQt::Key_Multi_key), 0x11b7, 0x11a8, 0, 0, 0}, 0x11da }, // U11da	 # ��� ���   = ���
    { {UNITIZE(TQt::Key_Multi_key), 0x11b7, 0x11af, 0, 0, 0}, 0x11db }, // U11db	 # ��� ���   = ���
    { {UNITIZE(TQt::Key_Multi_key), 0x11b7, 0x11b8, 0, 0, 0}, 0x11dc }, // U11dc	 # ��� ���   = ���
    { {UNITIZE(TQt::Key_Multi_key), 0x11b7, 0x11ba, 0, 0, 0}, 0x11dd }, // U11dd	 # ��� ���   = ���
    { {UNITIZE(TQt::Key_Multi_key), 0x11b7, 0x11ba, 0x11ba, 0, 0}, 0x11de }, // U11de	 # ��� ��� ���  = ���
    { {UNITIZE(TQt::Key_Multi_key), 0x11b7, 0x11bb, 0, 0, 0}, 0x11de }, // U11de	 # ��� ���   = ���
    { {UNITIZE(TQt::Key_Multi_key), 0x11b7, 0x11bc, 0, 0, 0}, 0x11e2 }, // U11e2	 # ��� ���   = ���
    { {UNITIZE(TQt::Key_Multi_key), 0x11b7, 0x11be, 0, 0, 0}, 0x11e0 }, // U11e0	 # ��� ���   = ���
    { {UNITIZE(TQt::Key_Multi_key), 0x11b7, 0x11c2, 0, 0, 0}, 0x11e1 }, // U11e1	 # ��� ���   = ���
    { {UNITIZE(TQt::Key_Multi_key), 0x11b7, 0x11eb, 0, 0, 0}, 0x11df }, // U11df	 # ��� ���   = ���
    { {UNITIZE(TQt::Key_Multi_key), 0x11b8, 0x11af, 0, 0, 0}, 0x11e3 }, // U11e3	 # ��� ���   = ���
    { {UNITIZE(TQt::Key_Multi_key), 0x11b8, 0x11ba, 0, 0, 0}, 0x11b9 }, // U11b9	 # ��� ���   = ���
    { {UNITIZE(TQt::Key_Multi_key), 0x11b8, 0x11bc, 0, 0, 0}, 0x11e6 }, // U11e6	 # ��� ���   = ���
    { {UNITIZE(TQt::Key_Multi_key), 0x11b8, 0x11c1, 0, 0, 0}, 0x11e4 }, // U11e4	 # ��� ���   = ���
    { {UNITIZE(TQt::Key_Multi_key), 0x11b8, 0x11c2, 0, 0, 0}, 0x11e5 }, // U11e5	 # ��� ���   = ���
    { {UNITIZE(TQt::Key_Multi_key), 0x11ba, 0x11a8, 0, 0, 0}, 0x11e7 }, // U11e7	 # ��� ���   = ���
    { {UNITIZE(TQt::Key_Multi_key), 0x11ba, 0x11ae, 0, 0, 0}, 0x11e8 }, // U11e8	 # ��� ���   = ���
    { {UNITIZE(TQt::Key_Multi_key), 0x11ba, 0x11af, 0, 0, 0}, 0x11e9 }, // U11e9	 # ��� ���   = ���
    { {UNITIZE(TQt::Key_Multi_key), 0x11ba, 0x11b8, 0, 0, 0}, 0x11ea }, // U11ea	 # ��� ���   = ���
    { {UNITIZE(TQt::Key_Multi_key), 0x11ba, 0x11ba, 0, 0, 0}, 0x11bb }, // U11bb	 # ��� ���   = ���
    { {UNITIZE(TQt::Key_Multi_key), 0x11bc, 0x11a8, 0, 0, 0}, 0x11ec }, // U11ec	 # ��� ���   = ���
    { {UNITIZE(TQt::Key_Multi_key), 0x11bc, 0x11a8, 0x11a8, 0, 0}, 0x11ed }, // U11ed	 # ��� ��� ���  = ���
    { {UNITIZE(TQt::Key_Multi_key), 0x11bc, 0x11a9, 0, 0, 0}, 0x11ed }, // U11ed	 # ��� ���   = ���
    { {UNITIZE(TQt::Key_Multi_key), 0x11bc, 0x11bc, 0, 0, 0}, 0x11ee }, // U11ee	 # ��� ���   = ���
    { {UNITIZE(TQt::Key_Multi_key), 0x11bc, 0x11bf, 0, 0, 0}, 0x11ef }, // U11ef	 # ��� ���   = ���
    { {UNITIZE(TQt::Key_Multi_key), 0x11c1, 0x11b8, 0, 0, 0}, 0x11f3 }, // U11f3	 # ��� ���   = ���
    { {UNITIZE(TQt::Key_Multi_key), 0x11c1, 0x11bc, 0, 0, 0}, 0x11f4 }, // U11f4	 # ��� ���   = ���
    { {UNITIZE(TQt::Key_Multi_key), 0x11c2, 0x11ab, 0, 0, 0}, 0x11f5 }, // U11f5	 # ��� ���   = ���
    { {UNITIZE(TQt::Key_Multi_key), 0x11c2, 0x11af, 0, 0, 0}, 0x11f6 }, // U11f6	 # ��� ���   = ���
    { {UNITIZE(TQt::Key_Multi_key), 0x11c2, 0x11b7, 0, 0, 0}, 0x11f7 }, // U11f7	 # ��� ���   = ���
    { {UNITIZE(TQt::Key_Multi_key), 0x11c2, 0x11b8, 0, 0, 0}, 0x11f8 }, // U11f8	 # ��� ���   = ���
    { {UNITIZE(TQt::Key_Multi_key), 0x11ce, 0x11c2, 0, 0, 0}, 0x11cf }, // U11cf	 # ��� ���   = ���
    { {UNITIZE(TQt::Key_Multi_key), 0x11dd, 0x11ba, 0, 0, 0}, 0x11de }, // U11de	 # ��� ���   = ���
    { {UNITIZE(TQt::Key_Multi_key), 0x11ec, 0x11a8, 0, 0, 0}, 0x11ed }, // U11ed	 # ��� ���   = ���
    { {UNITIZE(TQt::Key_Multi_key), 0x11f0, 0x11ba, 0, 0, 0}, 0x11f1 }, // U11f1	 # ��� ���   = ���
    { {UNITIZE(TQt::Key_Multi_key), 0x11f0, 0x11eb, 0, 0, 0}, 0x11f2 }, // U11f2	 # ��� ���   = ���
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x1d157, 0x1d165, 0, 0, 0}, 0xd15e }, // U1D15E # MUSICAL SYMBOL HALF NOTE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x1d158, 0x1d165, 0, 0, 0}, 0xd15f }, // U1D15F # MUSICAL SYMBOL QUARTER NOTE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x1d158, 0x1d165, 0x1d16e, 0, 0}, 0xd160 }, // U1D160 # MUSICAL SYMBOL EIGHTH NOTE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x1d158, 0x1d165, 0x1d16f, 0, 0}, 0xd161 }, // U1D161 # MUSICAL SYMBOL SIXTEENTH NOTE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x1d158, 0x1d165, 0x1d170, 0, 0}, 0xd162 }, // U1D162 # MUSICAL SYMBOL THIRTY-SECOND NOTE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x1d158, 0x1d165, 0x1d171, 0, 0}, 0xd163 }, // U1D163 # MUSICAL SYMBOL SIXTY-FOURTH NOTE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x1d158, 0x1d165, 0x1d172, 0, 0}, 0xd164 }, // U1D164 # MUSICAL SYMBOL ONE HUNDRED TWENTY-EIGHTH NOTE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x1d15f, 0x1d16e, 0, 0, 0}, 0xd160 }, // U1D160 # MUSICAL SYMBOL EIGHTH NOTE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x1d15f, 0x1d16f, 0, 0, 0}, 0xd161 }, // U1D161 # MUSICAL SYMBOL SIXTEENTH NOTE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x1d15f, 0x1d170, 0, 0, 0}, 0xd162 }, // U1D162 # MUSICAL SYMBOL THIRTY-SECOND NOTE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x1d15f, 0x1d171, 0, 0, 0}, 0xd163 }, // U1D163 # MUSICAL SYMBOL SIXTY-FOURTH NOTE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x1d15f, 0x1d172, 0, 0, 0}, 0xd164 }, // U1D164 # MUSICAL SYMBOL ONE HUNDRED TWENTY-EIGHTH NOTE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x1d1b9, 0x1d165, 0, 0, 0}, 0xd1bb }, // U1D1BB # MUSICAL SYMBOL MINIMA
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x1d1b9, 0x1d165, 0x1d16e, 0, 0}, 0xd1bd }, // U1D1BD # MUSICAL SYMBOL SEMIMINIMA WHITE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x1d1b9, 0x1d165, 0x1d16f, 0, 0}, 0xd1bf }, // U1D1BF # MUSICAL SYMBOL FUSA WHITE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x1d1ba, 0x1d165, 0, 0, 0}, 0xd1bc }, // U1D1BC # MUSICAL SYMBOL MINIMA BLACK
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x1d1ba, 0x1d165, 0x1d16e, 0, 0}, 0xd1be }, // U1D1BE # MUSICAL SYMBOL SEMIMINIMA BLACK
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x1d1ba, 0x1d165, 0x1d16f, 0, 0}, 0xd1c0 }, // U1D1C0 # MUSICAL SYMBOL FUSA BLACK
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x1d1bb, 0x1d16e, 0, 0, 0}, 0xd1bd }, // U1D1BD # MUSICAL SYMBOL SEMIMINIMA WHITE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x1d1bb, 0x1d16f, 0, 0, 0}, 0xd1bf }, // U1D1BF # MUSICAL SYMBOL FUSA WHITE
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x1d1bc, 0x1d16e, 0, 0, 0}, 0xd1be }, // U1D1BE # MUSICAL SYMBOL SEMIMINIMA BLACK
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x1d1bc, 0x1d16f, 0, 0, 0}, 0xd1c0 }, // U1D1C0 # MUSICAL SYMBOL FUSA BLACK
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x1fbf, 0x0000, 0, 0, 0}, 0x1fcd }, // U1FCD # GREEK PSILI AND VARIA
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x1fbf, 0x0000, 0, 0, 0}, 0x1fce }, // U1FCE # GREEK PSILI AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x1fbf, 0x0027, 0, 0, 0}, 0x1fce }, // U1FCE # GREEK PSILI AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x1fbf, 0x0060, 0, 0, 0}, 0x1fcd }, // U1FCD # GREEK PSILI AND VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x1fbf, 0x007e, 0, 0, 0}, 0x1fcf }, // U1FCF # GREEK PSILI AND PERISPOMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x1fbf, 0x00b4, 0, 0, 0}, 0x1fce }, // U1FCE # GREEK PSILI AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x1fbf, 0x0342, 0, 0, 0}, 0x1fcf }, // U1FCF # GREEK PSILI AND PERISPOMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x1fbf, UNITIZE(TQt::Key_Dead_Grave), 0, 0, 0}, 0x1fcd }, // U1FCD # GREEK PSILI AND VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x1fbf, UNITIZE(TQt::Key_Dead_Acute), 0, 0, 0}, 0x1fce }, // U1FCE # GREEK PSILI AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x1fbf, UNITIZE(TQt::Key_Dead_Tilde), 0, 0, 0}, 0x1fcf }, // U1FCF # GREEK PSILI AND PERISPOMENI
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x1ffe, 0x0000, 0, 0, 0}, 0x1fdd }, // U1FDD # GREEK DASIA AND VARIA
// /* broken */    { {UNITIZE(TQt::Key_Multi_key), 0x1ffe, 0x0000, 0, 0, 0}, 0x1fde }, // U1FDE # GREEK DASIA AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x1ffe, 0x0027, 0, 0, 0}, 0x1fde }, // U1FDE # GREEK DASIA AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x1ffe, 0x0060, 0, 0, 0}, 0x1fdd }, // U1FDD # GREEK DASIA AND VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x1ffe, 0x007e, 0, 0, 0}, 0x1fdf }, // U1FDF # GREEK DASIA AND PERISPOMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x1ffe, 0x00b4, 0, 0, 0}, 0x1fde }, // U1FDE # GREEK DASIA AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x1ffe, 0x0342, 0, 0, 0}, 0x1fdf }, // U1FDF # GREEK DASIA AND PERISPOMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x1ffe, UNITIZE(TQt::Key_Dead_Grave), 0, 0, 0}, 0x1fdd }, // U1FDD # GREEK DASIA AND VARIA
    { {UNITIZE(TQt::Key_Multi_key), 0x1ffe, UNITIZE(TQt::Key_Dead_Acute), 0, 0, 0}, 0x1fde }, // U1FDE # GREEK DASIA AND OXIA
    { {UNITIZE(TQt::Key_Multi_key), 0x1ffe, UNITIZE(TQt::Key_Dead_Tilde), 0, 0, 0}, 0x1fdf }, // U1FDF # GREEK DASIA AND PERISPOMENI
    { {UNITIZE(TQt::Key_Multi_key), 0x2203, 0x0338, 0, 0, 0}, 0x2204 }, // U2204 # THERE DOES NOT EXIST
    { {UNITIZE(TQt::Key_Multi_key), 0x2208, 0x0338, 0, 0, 0}, 0x2209 }, // U2209 # NOT AN ELEMENT OF
    { {UNITIZE(TQt::Key_Multi_key), 0x220b, 0x0338, 0, 0, 0}, 0x220c }, // U220C # DOES NOT CONTAIN AS MEMBER
    { {UNITIZE(TQt::Key_Multi_key), 0x2223, 0x0338, 0, 0, 0}, 0x2224 }, // U2224 # DOES NOT DIVIDE
    { {UNITIZE(TQt::Key_Multi_key), 0x2225, 0x0338, 0, 0, 0}, 0x2226 }, // U2226 # NOT PARALLEL TO
    { {UNITIZE(TQt::Key_Multi_key), 0x223c, 0x0338, 0, 0, 0}, 0x2241 }, // U2241 # NOT TILDE
    { {UNITIZE(TQt::Key_Multi_key), 0x223c, 0x0338, 0, 0, 0}, 0x2247 }, // U2247 # NEITHER APPROXIMATELY NOR ACTUALLY EQUAL TO
    { {UNITIZE(TQt::Key_Multi_key), 0x2243, 0x0338, 0, 0, 0}, 0x2244 }, // U2244 # NOT ASYMPTOTICALLY EQUAL TO
    { {UNITIZE(TQt::Key_Multi_key), 0x2248, 0x0338, 0, 0, 0}, 0x2249 }, // U2249 # NOT ALMOST EQUAL TO
    { {UNITIZE(TQt::Key_Multi_key), 0x224d, 0x0338, 0, 0, 0}, 0x226d }, // U226D # NOT EQUIVALENT TO
    { {UNITIZE(TQt::Key_Multi_key), 0x2261, 0x0338, 0, 0, 0}, 0x2262 }, // U2262 # NOT IDENTICAL TO
    { {UNITIZE(TQt::Key_Multi_key), 0x2264, 0x0338, 0, 0, 0}, 0x2270 }, // U2270 # NEITHER LESS-THAN NOR EQUAL TO
    { {UNITIZE(TQt::Key_Multi_key), 0x2265, 0x0338, 0, 0, 0}, 0x2271 }, // U2271 # NEITHER GREATER-THAN NOR EQUAL TO
    { {UNITIZE(TQt::Key_Multi_key), 0x2272, 0x0338, 0, 0, 0}, 0x2274 }, // U2274 # NEITHER LESS-THAN NOR EQUIVALENT TO
    { {UNITIZE(TQt::Key_Multi_key), 0x2273, 0x0338, 0, 0, 0}, 0x2275 }, // U2275 # NEITHER GREATER-THAN NOR EQUIVALENT TO
    { {UNITIZE(TQt::Key_Multi_key), 0x2276, 0x0338, 0, 0, 0}, 0x2278 }, // U2278 # NEITHER LESS-THAN NOR GREATER-THAN
    { {UNITIZE(TQt::Key_Multi_key), 0x2277, 0x0338, 0, 0, 0}, 0x2279 }, // U2279 # NEITHER GREATER-THAN NOR LESS-THAN
    { {UNITIZE(TQt::Key_Multi_key), 0x227a, 0x0338, 0, 0, 0}, 0x2280 }, // U2280 # DOES NOT PRECEDE
    { {UNITIZE(TQt::Key_Multi_key), 0x227b, 0x0338, 0, 0, 0}, 0x2281 }, // U2281 # DOES NOT SUCCEED
    { {UNITIZE(TQt::Key_Multi_key), 0x227c, 0x0338, 0, 0, 0}, 0x22e0 }, // U22E0 # DOES NOT PRECEDE OR EQUAL
    { {UNITIZE(TQt::Key_Multi_key), 0x227d, 0x0338, 0, 0, 0}, 0x22e1 }, // U22E1 # DOES NOT SUCCEED OR EQUAL
    { {UNITIZE(TQt::Key_Multi_key), 0x2282, 0x0338, 0, 0, 0}, 0x2284 }, // U2284 # NOT A SUBSET OF
    { {UNITIZE(TQt::Key_Multi_key), 0x2282, 0x0338, 0, 0, 0}, 0x2284 }, // U2284 # NOT A SUBSET OF
    { {UNITIZE(TQt::Key_Multi_key), 0x2283, 0x0338, 0, 0, 0}, 0x2285 }, // U2285 # NOT A SUPERSET OF
    { {UNITIZE(TQt::Key_Multi_key), 0x2283, 0x0338, 0, 0, 0}, 0x2285 }, // U2285 # NOT A SUPERSET OF
    { {UNITIZE(TQt::Key_Multi_key), 0x2286, 0x0338, 0, 0, 0}, 0x2288 }, // U2288 # NEITHER A SUBSET OF NOR EQUAL TO
    { {UNITIZE(TQt::Key_Multi_key), 0x2287, 0x0338, 0, 0, 0}, 0x2289 }, // U2289 # NEITHER A SUPERSET OF NOR EQUAL TO
    { {UNITIZE(TQt::Key_Multi_key), 0x2291, 0x0338, 0, 0, 0}, 0x22e2 }, // U22E2 # NOT SQUARE IMAGE OF OR EQUAL TO
    { {UNITIZE(TQt::Key_Multi_key), 0x2292, 0x0338, 0, 0, 0}, 0x22e3 }, // U22E3 # NOT SQUARE ORIGINAL OF OR EQUAL TO
    { {UNITIZE(TQt::Key_Multi_key), 0x22a3, 0x0338, 0, 0, 0}, 0x22ac }, // U22AC # DOES NOT PROVE
    { {UNITIZE(TQt::Key_Multi_key), 0x22a8, 0x0338, 0, 0, 0}, 0x22ad }, // U22AD # NOT TRUE
    { {UNITIZE(TQt::Key_Multi_key), 0x22a9, 0x0338, 0, 0, 0}, 0x22ae }, // U22AE # DOES NOT FORCE
    { {UNITIZE(TQt::Key_Multi_key), 0x22ab, 0x0338, 0, 0, 0}, 0x22af }, // U22AF # NEGATED DOUBLE VERTICAL BAR DOUBLE RIGHT TURNSTILE
    { {UNITIZE(TQt::Key_Multi_key), 0x22b2, 0x0338, 0, 0, 0}, 0x22ea }, // U22EA # NOT NORMAL SUBGROUP OF
    { {UNITIZE(TQt::Key_Multi_key), 0x22b3, 0x0338, 0, 0, 0}, 0x22eb }, // U22EB # DOES NOT CONTAIN AS NORMAL SUBGROUP
    { {UNITIZE(TQt::Key_Multi_key), 0x22b4, 0x0338, 0, 0, 0}, 0x22ec }, // U22EC # NOT NORMAL SUBGROUP OF OR EQUAL TO
    { {UNITIZE(TQt::Key_Multi_key), 0x22b5, 0x0338, 0, 0, 0}, 0x22ed }, // U22ED # DOES NOT CONTAIN AS NORMAL SUBGROUP OR EQUAL
    { {UNITIZE(TQt::Key_Multi_key), 0x2add, 0x0338, 0, 0, 0}, 0x2adc }, // U2ADC # FORKING
// /* broken */    { {UNITIZE(TQt::Key_Dead_Grave), 0x0000, 0, 0, 0, 0}, 0x1eeb }, // U1EEB # LATIN SMALL LETTER U WITH HORN AND GRAVE
// /* broken */    { {UNITIZE(TQt::Key_Dead_Grave), 0x0000, 0, 0, 0, 0}, 0x1edd }, // U1EDD # LATIN SMALL LETTER O WITH HORN AND GRAVE
// /* broken */    { {UNITIZE(TQt::Key_Dead_Grave), 0x0000, 0, 0, 0, 0}, 0x1edc }, // U1EDC # LATIN CAPITAL LETTER O WITH HORN AND GRAVE
// /* broken */    { {UNITIZE(TQt::Key_Dead_Grave), 0x0000, 0, 0, 0, 0}, 0x1eea }, // U1EEA # LATIN CAPITAL LETTER U WITH HORN AND GRAVE
    { {UNITIZE(TQt::Key_Dead_Grave), 0x0020, 0, 0, 0, 0}, 0x0060 }, // grave
    { {UNITIZE(TQt::Key_Dead_Grave), 0x0041, 0, 0, 0, 0}, 0x00c0 }, // U00C0 # LATIN CAPITAL LETTER A WITH GRAVE
    { {UNITIZE(TQt::Key_Dead_Grave), 0x0045, 0, 0, 0, 0}, 0x00c8 }, // U00C8 # LATIN CAPITAL LETTER E WITH GRAVE
    { {UNITIZE(TQt::Key_Dead_Grave), 0x0049, 0, 0, 0, 0}, 0x00cc }, // U00CC # LATIN CAPITAL LETTER I WITH GRAVE
    { {UNITIZE(TQt::Key_Dead_Grave), 0x004e, 0, 0, 0, 0}, 0x01f8 }, // U01F8 # LATIN CAPITAL LETTER N WITH GRAVE
    { {UNITIZE(TQt::Key_Dead_Grave), 0x004f, 0, 0, 0, 0}, 0x00d2 }, // U00D2 # LATIN CAPITAL LETTER O WITH GRAVE
    { {UNITIZE(TQt::Key_Dead_Grave), 0x0055, 0, 0, 0, 0}, 0x00d9 }, // U00D9 # LATIN CAPITAL LETTER U WITH GRAVE
    { {UNITIZE(TQt::Key_Dead_Grave), 0x0057, 0, 0, 0, 0}, 0x1e80 }, // U1E80 # LATIN CAPITAL LETTER W WITH GRAVE
    { {UNITIZE(TQt::Key_Dead_Grave), 0x0059, 0, 0, 0, 0}, 0x1ef2 }, // U1EF2 # LATIN CAPITAL LETTER Y WITH GRAVE
    { {UNITIZE(TQt::Key_Dead_Grave), 0x0061, 0, 0, 0, 0}, 0x00e0 }, // U00E0 # LATIN SMALL LETTER A WITH GRAVE
    { {UNITIZE(TQt::Key_Dead_Grave), 0x0065, 0, 0, 0, 0}, 0x00e8 }, // U00E8 # LATIN SMALL LETTER E WITH GRAVE
    { {UNITIZE(TQt::Key_Dead_Grave), 0x0069, 0, 0, 0, 0}, 0x00ec }, // U00EC # LATIN SMALL LETTER I WITH GRAVE
    { {UNITIZE(TQt::Key_Dead_Grave), 0x006e, 0, 0, 0, 0}, 0x01f9 }, // U01F9 # LATIN SMALL LETTER N WITH GRAVE
    { {UNITIZE(TQt::Key_Dead_Grave), 0x006f, 0, 0, 0, 0}, 0x00f2 }, // U00F2 # LATIN SMALL LETTER O WITH GRAVE
    { {UNITIZE(TQt::Key_Dead_Grave), 0x0075, 0, 0, 0, 0}, 0x00f9 }, // U00F9 # LATIN SMALL LETTER U WITH GRAVE
    { {UNITIZE(TQt::Key_Dead_Grave), 0x0077, 0, 0, 0, 0}, 0x1e81 }, // U1E81 # LATIN SMALL LETTER W WITH GRAVE
    { {UNITIZE(TQt::Key_Dead_Grave), 0x0079, 0, 0, 0, 0}, 0x1ef3 }, // U1EF3 # LATIN SMALL LETTER Y WITH GRAVE
    { {UNITIZE(TQt::Key_Dead_Grave), 0x00c2, 0, 0, 0, 0}, 0x1ea6 }, // U1EA6 # LATIN CAPITAL LETTER A WITH CIRCUMFLEX AND GRAVE
    { {UNITIZE(TQt::Key_Dead_Grave), 0x00ca, 0, 0, 0, 0}, 0x1ec0 }, // U1EC0 # LATIN CAPITAL LETTER E WITH CIRCUMFLEX AND GRAVE
    { {UNITIZE(TQt::Key_Dead_Grave), 0x00d4, 0, 0, 0, 0}, 0x1ed2 }, // U1ED2 # LATIN CAPITAL LETTER O WITH CIRCUMFLEX AND GRAVE
    { {UNITIZE(TQt::Key_Dead_Grave), 0x00dc, 0, 0, 0, 0}, 0x01db }, // U01DB # LATIN CAPITAL LETTER U WITH DIAERESIS AND GRAVE
    { {UNITIZE(TQt::Key_Dead_Grave), 0x00e2, 0, 0, 0, 0}, 0x1ea7 }, // U1EA7 # LATIN SMALL LETTER A WITH CIRCUMFLEX AND GRAVE
    { {UNITIZE(TQt::Key_Dead_Grave), 0x00ea, 0, 0, 0, 0}, 0x1ec1 }, // U1EC1 # LATIN SMALL LETTER E WITH CIRCUMFLEX AND GRAVE
    { {UNITIZE(TQt::Key_Dead_Grave), 0x00f4, 0, 0, 0, 0}, 0x1ed3 }, // U1ED3 # LATIN SMALL LETTER O WITH CIRCUMFLEX AND GRAVE
    { {UNITIZE(TQt::Key_Dead_Grave), 0x00fc, 0, 0, 0, 0}, 0x01dc }, // U01DC # LATIN SMALL LETTER U WITH DIAERESIS AND GRAVE
    { {UNITIZE(TQt::Key_Dead_Grave), 0x0102, 0, 0, 0, 0}, 0x1eb0 }, // U1EB0 # LATIN CAPITAL LETTER A WITH BREVE AND GRAVE
    { {UNITIZE(TQt::Key_Dead_Grave), 0x0103, 0, 0, 0, 0}, 0x1eb1 }, // U1EB1 # LATIN SMALL LETTER A WITH BREVE AND GRAVE
    { {UNITIZE(TQt::Key_Dead_Grave), 0x0112, 0, 0, 0, 0}, 0x1e14 }, // U1E14 # LATIN CAPITAL LETTER E WITH MACRON AND GRAVE
    { {UNITIZE(TQt::Key_Dead_Grave), 0x0113, 0, 0, 0, 0}, 0x1e15 }, // U1E15 # LATIN SMALL LETTER E WITH MACRON AND GRAVE
    { {UNITIZE(TQt::Key_Dead_Grave), 0x014c, 0, 0, 0, 0}, 0x1e50 }, // U1E50 # LATIN CAPITAL LETTER O WITH MACRON AND GRAVE
    { {UNITIZE(TQt::Key_Dead_Grave), 0x014d, 0, 0, 0, 0}, 0x1e51 }, // U1E51 # LATIN SMALL LETTER O WITH MACRON AND GRAVE
    { {UNITIZE(TQt::Key_Dead_Grave), 0x01A0, 0, 0, 0, 0}, 0x1edc }, // U1EDC # LATIN CAPITAL LETTER O WITH HORN AND GRAVE
    { {UNITIZE(TQt::Key_Dead_Grave), 0x01A1, 0, 0, 0, 0}, 0x1edd }, // U1EDD # LATIN SMALL LETTER O WITH HORN AND GRAVE
    { {UNITIZE(TQt::Key_Dead_Grave), 0x01AF, 0, 0, 0, 0}, 0x1eea }, // U1EEA # LATIN CAPITAL LETTER U WITH HORN AND GRAVE
    { {UNITIZE(TQt::Key_Dead_Grave), 0x01B0, 0, 0, 0, 0}, 0x1eeb }, // U1EEB # LATIN SMALL LETTER U WITH HORN AND GRAVE
    { {UNITIZE(TQt::Key_Dead_Grave), 0x0313, 0x0391, 0, 0, 0}, 0x1f0a }, // U1F0A # GREEK CAPITAL LETTER ALPHA WITH PSILI AND VARIA
    { {UNITIZE(TQt::Key_Dead_Grave), 0x0313, 0x0395, 0, 0, 0}, 0x1f1a }, // U1F1A # GREEK CAPITAL LETTER EPSILON WITH PSILI AND VARIA
    { {UNITIZE(TQt::Key_Dead_Grave), 0x0313, 0x0397, 0, 0, 0}, 0x1f2a }, // U1F2A # GREEK CAPITAL LETTER ETA WITH PSILI AND VARIA
    { {UNITIZE(TQt::Key_Dead_Grave), 0x0313, 0x0399, 0, 0, 0}, 0x1f3a }, // U1F3A # GREEK CAPITAL LETTER IOTA WITH PSILI AND VARIA
    { {UNITIZE(TQt::Key_Dead_Grave), 0x0313, 0x039f, 0, 0, 0}, 0x1f4a }, // U1F4A # GREEK CAPITAL LETTER OMICRON WITH PSILI AND VARIA
    { {UNITIZE(TQt::Key_Dead_Grave), 0x0313, 0x03a9, 0, 0, 0}, 0x1f6a }, // U1F6A # GREEK CAPITAL LETTER OMEGA WITH PSILI AND VARIA
    { {UNITIZE(TQt::Key_Dead_Grave), 0x0313, 0x03b1, 0, 0, 0}, 0x1f02 }, // U1F02 # GREEK SMALL LETTER ALPHA WITH PSILI AND VARIA
    { {UNITIZE(TQt::Key_Dead_Grave), 0x0313, 0x03b5, 0, 0, 0}, 0x1f12 }, // U1F12 # GREEK SMALL LETTER EPSILON WITH PSILI AND VARIA
    { {UNITIZE(TQt::Key_Dead_Grave), 0x0313, 0x03b7, 0, 0, 0}, 0x1f22 }, // U1F22 # GREEK SMALL LETTER ETA WITH PSILI AND VARIA
    { {UNITIZE(TQt::Key_Dead_Grave), 0x0313, 0x03b9, 0, 0, 0}, 0x1f32 }, // U1F32 # GREEK SMALL LETTER IOTA WITH PSILI AND VARIA
    { {UNITIZE(TQt::Key_Dead_Grave), 0x0313, 0x03bf, 0, 0, 0}, 0x1f42 }, // U1F42 # GREEK SMALL LETTER OMICRON WITH PSILI AND VARIA
    { {UNITIZE(TQt::Key_Dead_Grave), 0x0313, 0x03c5, 0, 0, 0}, 0x1f52 }, // U1F52 # GREEK SMALL LETTER UPSILON WITH PSILI AND VARIA
    { {UNITIZE(TQt::Key_Dead_Grave), 0x0313, 0x03c9, 0, 0, 0}, 0x1f62 }, // U1F62 # GREEK SMALL LETTER OMEGA WITH PSILI AND VARIA
    { {UNITIZE(TQt::Key_Dead_Grave), 0x0314, 0x0391, 0, 0, 0}, 0x1f0b }, // U1F0B # GREEK CAPITAL LETTER ALPHA WITH DASIA AND VARIA
    { {UNITIZE(TQt::Key_Dead_Grave), 0x0314, 0x0395, 0, 0, 0}, 0x1f1b }, // U1F1B # GREEK CAPITAL LETTER EPSILON WITH DASIA AND VARIA
    { {UNITIZE(TQt::Key_Dead_Grave), 0x0314, 0x0397, 0, 0, 0}, 0x1f2b }, // U1F2B # GREEK CAPITAL LETTER ETA WITH DASIA AND VARIA
    { {UNITIZE(TQt::Key_Dead_Grave), 0x0314, 0x0399, 0, 0, 0}, 0x1f3b }, // U1F3B # GREEK CAPITAL LETTER IOTA WITH DASIA AND VARIA
    { {UNITIZE(TQt::Key_Dead_Grave), 0x0314, 0x039f, 0, 0, 0}, 0x1f4b }, // U1F4B # GREEK CAPITAL LETTER OMICRON WITH DASIA AND VARIA
    { {UNITIZE(TQt::Key_Dead_Grave), 0x0314, 0x03a5, 0, 0, 0}, 0x1f5b }, // U1F5B # GREEK CAPITAL LETTER UPSILON WITH DASIA AND VARIA
    { {UNITIZE(TQt::Key_Dead_Grave), 0x0314, 0x03a9, 0, 0, 0}, 0x1f6b }, // U1F6B # GREEK CAPITAL LETTER OMEGA WITH DASIA AND VARIA
    { {UNITIZE(TQt::Key_Dead_Grave), 0x0314, 0x03b1, 0, 0, 0}, 0x1f03 }, // U1F03 # GREEK SMALL LETTER ALPHA WITH DASIA AND VARIA
    { {UNITIZE(TQt::Key_Dead_Grave), 0x0314, 0x03b5, 0, 0, 0}, 0x1f13 }, // U1F13 # GREEK SMALL LETTER EPSILON WITH DASIA AND VARIA
    { {UNITIZE(TQt::Key_Dead_Grave), 0x0314, 0x03b7, 0, 0, 0}, 0x1f23 }, // U1F23 # GREEK SMALL LETTER ETA WITH DASIA AND VARIA
    { {UNITIZE(TQt::Key_Dead_Grave), 0x0314, 0x03b9, 0, 0, 0}, 0x1f33 }, // U1F33 # GREEK SMALL LETTER IOTA WITH DASIA AND VARIA
    { {UNITIZE(TQt::Key_Dead_Grave), 0x0314, 0x03bf, 0, 0, 0}, 0x1f43 }, // U1F43 # GREEK SMALL LETTER OMICRON WITH DASIA AND VARIA
    { {UNITIZE(TQt::Key_Dead_Grave), 0x0314, 0x03c5, 0, 0, 0}, 0x1f53 }, // U1F53 # GREEK SMALL LETTER UPSILON WITH DASIA AND VARIA
    { {UNITIZE(TQt::Key_Dead_Grave), 0x0314, 0x03c9, 0, 0, 0}, 0x1f63 }, // U1F63 # GREEK SMALL LETTER OMEGA WITH DASIA AND VARIA
    { {UNITIZE(TQt::Key_Dead_Grave), 0x0391, 0, 0, 0, 0}, 0x1fba }, // U1FBA # GREEK CAPITAL LETTER ALPHA WITH VARIA
    { {UNITIZE(TQt::Key_Dead_Grave), 0x0395, 0, 0, 0, 0}, 0x1fc8 }, // U1FC8 # GREEK CAPITAL LETTER EPSILON WITH VARIA
    { {UNITIZE(TQt::Key_Dead_Grave), 0x0397, 0, 0, 0, 0}, 0x1fca }, // U1FCA # GREEK CAPITAL LETTER ETA WITH VARIA
    { {UNITIZE(TQt::Key_Dead_Grave), 0x0399, 0, 0, 0, 0}, 0x1fda }, // U1FDA # GREEK CAPITAL LETTER IOTA WITH VARIA
    { {UNITIZE(TQt::Key_Dead_Grave), 0x039f, 0, 0, 0, 0}, 0x1ff8 }, // U1FF8 # GREEK CAPITAL LETTER OMICRON WITH VARIA
    { {UNITIZE(TQt::Key_Dead_Grave), 0x03a5, 0, 0, 0, 0}, 0x1fea }, // U1FEA # GREEK CAPITAL LETTER UPSILON WITH VARIA
    { {UNITIZE(TQt::Key_Dead_Grave), 0x03a9, 0, 0, 0, 0}, 0x1ffa }, // U1FFA # GREEK CAPITAL LETTER OMEGA WITH VARIA
    { {UNITIZE(TQt::Key_Dead_Grave), 0x03b1, 0, 0, 0, 0}, 0x1f70 }, // U1F70 # GREEK SMALL LETTER ALPHA WITH VARIA
    { {UNITIZE(TQt::Key_Dead_Grave), 0x03b5, 0, 0, 0, 0}, 0x1f72 }, // U1F72 # GREEK SMALL LETTER EPSILON WITH VARIA
    { {UNITIZE(TQt::Key_Dead_Grave), 0x03b7, 0, 0, 0, 0}, 0x1f74 }, // U1F74 # GREEK SMALL LETTER ETA WITH VARIA
    { {UNITIZE(TQt::Key_Dead_Grave), 0x03b9, 0, 0, 0, 0}, 0x1f76 }, // U1F76 # GREEK SMALL LETTER IOTA WITH VARIA
    { {UNITIZE(TQt::Key_Dead_Grave), 0x03bf, 0, 0, 0, 0}, 0x1f78 }, // U1F78 # GREEK SMALL LETTER OMICRON WITH VARIA
    { {UNITIZE(TQt::Key_Dead_Grave), 0x03c5, 0, 0, 0, 0}, 0x1f7a }, // U1F7A # GREEK SMALL LETTER UPSILON WITH VARIA
    { {UNITIZE(TQt::Key_Dead_Grave), 0x03c9, 0, 0, 0, 0}, 0x1f7c }, // U1F7C # GREEK SMALL LETTER OMEGA WITH VARIA
    { {UNITIZE(TQt::Key_Dead_Grave), 0x03ca, 0, 0, 0, 0}, 0x1fd2 }, // U1FD2 # GREEK SMALL LETTER IOTA WITH DIALYTIKA AND VARIA
    { {UNITIZE(TQt::Key_Dead_Grave), 0x03cb, 0, 0, 0, 0}, 0x1fe2 }, // U1FE2 # GREEK SMALL LETTER UPSILON WITH DIALYTIKA AND VARIA
    { {UNITIZE(TQt::Key_Dead_Grave), 0x0415, 0, 0, 0, 0}, 0x0400 }, // U0400 # CYRILLIC CAPITAL LETTER IE WITH GRAVE
    { {UNITIZE(TQt::Key_Dead_Grave), 0x0418, 0, 0, 0, 0}, 0x040d }, // U040D # CYRILLIC CAPITAL LETTER I WITH GRAVE
    { {UNITIZE(TQt::Key_Dead_Grave), 0x0435, 0, 0, 0, 0}, 0x0450 }, // U0450 # CYRILLIC SMALL LETTER IE WITH GRAVE
    { {UNITIZE(TQt::Key_Dead_Grave), 0x0438, 0, 0, 0, 0}, 0x045d }, // U045D # CYRILLIC SMALL LETTER I WITH GRAVE
    { {UNITIZE(TQt::Key_Dead_Grave), 0x1f00, 0, 0, 0, 0}, 0x1f02 }, // U1F02 # GREEK SMALL LETTER ALPHA WITH PSILI AND VARIA
    { {UNITIZE(TQt::Key_Dead_Grave), 0x1f01, 0, 0, 0, 0}, 0x1f03 }, // U1F03 # GREEK SMALL LETTER ALPHA WITH DASIA AND VARIA
    { {UNITIZE(TQt::Key_Dead_Grave), 0x1f08, 0, 0, 0, 0}, 0x1f0a }, // U1F0A # GREEK CAPITAL LETTER ALPHA WITH PSILI AND VARIA
    { {UNITIZE(TQt::Key_Dead_Grave), 0x1f09, 0, 0, 0, 0}, 0x1f0b }, // U1F0B # GREEK CAPITAL LETTER ALPHA WITH DASIA AND VARIA
    { {UNITIZE(TQt::Key_Dead_Grave), 0x1f10, 0, 0, 0, 0}, 0x1f12 }, // U1F12 # GREEK SMALL LETTER EPSILON WITH PSILI AND VARIA
    { {UNITIZE(TQt::Key_Dead_Grave), 0x1f11, 0, 0, 0, 0}, 0x1f13 }, // U1F13 # GREEK SMALL LETTER EPSILON WITH DASIA AND VARIA
    { {UNITIZE(TQt::Key_Dead_Grave), 0x1f18, 0, 0, 0, 0}, 0x1f1a }, // U1F1A # GREEK CAPITAL LETTER EPSILON WITH PSILI AND VARIA
    { {UNITIZE(TQt::Key_Dead_Grave), 0x1f19, 0, 0, 0, 0}, 0x1f1b }, // U1F1B # GREEK CAPITAL LETTER EPSILON WITH DASIA AND VARIA
    { {UNITIZE(TQt::Key_Dead_Grave), 0x1f20, 0, 0, 0, 0}, 0x1f22 }, // U1F22 # GREEK SMALL LETTER ETA WITH PSILI AND VARIA
    { {UNITIZE(TQt::Key_Dead_Grave), 0x1f21, 0, 0, 0, 0}, 0x1f23 }, // U1F23 # GREEK SMALL LETTER ETA WITH DASIA AND VARIA
    { {UNITIZE(TQt::Key_Dead_Grave), 0x1f28, 0, 0, 0, 0}, 0x1f2a }, // U1F2A # GREEK CAPITAL LETTER ETA WITH PSILI AND VARIA
    { {UNITIZE(TQt::Key_Dead_Grave), 0x1f29, 0, 0, 0, 0}, 0x1f2b }, // U1F2B # GREEK CAPITAL LETTER ETA WITH DASIA AND VARIA
    { {UNITIZE(TQt::Key_Dead_Grave), 0x1f30, 0, 0, 0, 0}, 0x1f32 }, // U1F32 # GREEK SMALL LETTER IOTA WITH PSILI AND VARIA
    { {UNITIZE(TQt::Key_Dead_Grave), 0x1f31, 0, 0, 0, 0}, 0x1f33 }, // U1F33 # GREEK SMALL LETTER IOTA WITH DASIA AND VARIA
    { {UNITIZE(TQt::Key_Dead_Grave), 0x1f38, 0, 0, 0, 0}, 0x1f3a }, // U1F3A # GREEK CAPITAL LETTER IOTA WITH PSILI AND VARIA
    { {UNITIZE(TQt::Key_Dead_Grave), 0x1f39, 0, 0, 0, 0}, 0x1f3b }, // U1F3B # GREEK CAPITAL LETTER IOTA WITH DASIA AND VARIA
    { {UNITIZE(TQt::Key_Dead_Grave), 0x1f40, 0, 0, 0, 0}, 0x1f42 }, // U1F42 # GREEK SMALL LETTER OMICRON WITH PSILI AND VARIA
    { {UNITIZE(TQt::Key_Dead_Grave), 0x1f41, 0, 0, 0, 0}, 0x1f43 }, // U1F43 # GREEK SMALL LETTER OMICRON WITH DASIA AND VARIA
    { {UNITIZE(TQt::Key_Dead_Grave), 0x1f48, 0, 0, 0, 0}, 0x1f4a }, // U1F4A # GREEK CAPITAL LETTER OMICRON WITH PSILI AND VARIA
    { {UNITIZE(TQt::Key_Dead_Grave), 0x1f49, 0, 0, 0, 0}, 0x1f4b }, // U1F4B # GREEK CAPITAL LETTER OMICRON WITH DASIA AND VARIA
    { {UNITIZE(TQt::Key_Dead_Grave), 0x1f50, 0, 0, 0, 0}, 0x1f52 }, // U1F52 # GREEK SMALL LETTER UPSILON WITH PSILI AND VARIA
    { {UNITIZE(TQt::Key_Dead_Grave), 0x1f51, 0, 0, 0, 0}, 0x1f53 }, // U1F53 # GREEK SMALL LETTER UPSILON WITH DASIA AND VARIA
    { {UNITIZE(TQt::Key_Dead_Grave), 0x1f59, 0, 0, 0, 0}, 0x1f5b }, // U1F5B # GREEK CAPITAL LETTER UPSILON WITH DASIA AND VARIA
    { {UNITIZE(TQt::Key_Dead_Grave), 0x1f60, 0, 0, 0, 0}, 0x1f62 }, // U1F62 # GREEK SMALL LETTER OMEGA WITH PSILI AND VARIA
    { {UNITIZE(TQt::Key_Dead_Grave), 0x1f61, 0, 0, 0, 0}, 0x1f63 }, // U1F63 # GREEK SMALL LETTER OMEGA WITH DASIA AND VARIA
    { {UNITIZE(TQt::Key_Dead_Grave), 0x1f68, 0, 0, 0, 0}, 0x1f6a }, // U1F6A # GREEK CAPITAL LETTER OMEGA WITH PSILI AND VARIA
    { {UNITIZE(TQt::Key_Dead_Grave), 0x1f69, 0, 0, 0, 0}, 0x1f6b }, // U1F6B # GREEK CAPITAL LETTER OMEGA WITH DASIA AND VARIA
    { {UNITIZE(TQt::Key_Dead_Grave), UNITIZE(TQt::Key_Multi_key), 0x0022, 0x0055, 0, 0}, 0x01db }, // U01DB # LATIN CAPITAL LETTER U WITH DIAERESIS AND GRAVE
    { {UNITIZE(TQt::Key_Dead_Grave), UNITIZE(TQt::Key_Multi_key), 0x0022, 0x0075, 0, 0}, 0x01dc }, // U01DC # LATIN SMALL LETTER U WITH DIAERESIS AND GRAVE
    { {UNITIZE(TQt::Key_Dead_Grave), UNITIZE(TQt::Key_Multi_key), 0x0022, 0x03b9, 0, 0}, 0x1fd2 }, // U1FD2 # GREEK SMALL LETTER IOTA WITH DIALYTIKA AND VARIA
    { {UNITIZE(TQt::Key_Dead_Grave), UNITIZE(TQt::Key_Multi_key), 0x0022, 0x03c5, 0, 0}, 0x1fe2 }, // U1FE2 # GREEK SMALL LETTER UPSILON WITH DIALYTIKA AND VARIA
    { {UNITIZE(TQt::Key_Dead_Grave), UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0391, 0, 0}, 0x1f0b }, // U1F0B # GREEK CAPITAL LETTER ALPHA WITH DASIA AND VARIA
    { {UNITIZE(TQt::Key_Dead_Grave), UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0395, 0, 0}, 0x1f1b }, // U1F1B # GREEK CAPITAL LETTER EPSILON WITH DASIA AND VARIA
    { {UNITIZE(TQt::Key_Dead_Grave), UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0397, 0, 0}, 0x1f2b }, // U1F2B # GREEK CAPITAL LETTER ETA WITH DASIA AND VARIA
    { {UNITIZE(TQt::Key_Dead_Grave), UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0399, 0, 0}, 0x1f3b }, // U1F3B # GREEK CAPITAL LETTER IOTA WITH DASIA AND VARIA
    { {UNITIZE(TQt::Key_Dead_Grave), UNITIZE(TQt::Key_Multi_key), 0x0028, 0x039f, 0, 0}, 0x1f4b }, // U1F4B # GREEK CAPITAL LETTER OMICRON WITH DASIA AND VARIA
    { {UNITIZE(TQt::Key_Dead_Grave), UNITIZE(TQt::Key_Multi_key), 0x0028, 0x03a5, 0, 0}, 0x1f5b }, // U1F5B # GREEK CAPITAL LETTER UPSILON WITH DASIA AND VARIA
    { {UNITIZE(TQt::Key_Dead_Grave), UNITIZE(TQt::Key_Multi_key), 0x0028, 0x03a9, 0, 0}, 0x1f6b }, // U1F6B # GREEK CAPITAL LETTER OMEGA WITH DASIA AND VARIA
    { {UNITIZE(TQt::Key_Dead_Grave), UNITIZE(TQt::Key_Multi_key), 0x0028, 0x03b1, 0, 0}, 0x1f03 }, // U1F03 # GREEK SMALL LETTER ALPHA WITH DASIA AND VARIA
    { {UNITIZE(TQt::Key_Dead_Grave), UNITIZE(TQt::Key_Multi_key), 0x0028, 0x03b5, 0, 0}, 0x1f13 }, // U1F13 # GREEK SMALL LETTER EPSILON WITH DASIA AND VARIA
    { {UNITIZE(TQt::Key_Dead_Grave), UNITIZE(TQt::Key_Multi_key), 0x0028, 0x03b7, 0, 0}, 0x1f23 }, // U1F23 # GREEK SMALL LETTER ETA WITH DASIA AND VARIA
    { {UNITIZE(TQt::Key_Dead_Grave), UNITIZE(TQt::Key_Multi_key), 0x0028, 0x03b9, 0, 0}, 0x1f33 }, // U1F33 # GREEK SMALL LETTER IOTA WITH DASIA AND VARIA
    { {UNITIZE(TQt::Key_Dead_Grave), UNITIZE(TQt::Key_Multi_key), 0x0028, 0x03bf, 0, 0}, 0x1f43 }, // U1F43 # GREEK SMALL LETTER OMICRON WITH DASIA AND VARIA
    { {UNITIZE(TQt::Key_Dead_Grave), UNITIZE(TQt::Key_Multi_key), 0x0028, 0x03c5, 0, 0}, 0x1f53 }, // U1F53 # GREEK SMALL LETTER UPSILON WITH DASIA AND VARIA
    { {UNITIZE(TQt::Key_Dead_Grave), UNITIZE(TQt::Key_Multi_key), 0x0028, 0x03c9, 0, 0}, 0x1f63 }, // U1F63 # GREEK SMALL LETTER OMEGA WITH DASIA AND VARIA
    { {UNITIZE(TQt::Key_Dead_Grave), UNITIZE(TQt::Key_Multi_key), 0x0029, 0x0391, 0, 0}, 0x1f0a }, // U1F0A # GREEK CAPITAL LETTER ALPHA WITH PSILI AND VARIA
    { {UNITIZE(TQt::Key_Dead_Grave), UNITIZE(TQt::Key_Multi_key), 0x0029, 0x0395, 0, 0}, 0x1f1a }, // U1F1A # GREEK CAPITAL LETTER EPSILON WITH PSILI AND VARIA
    { {UNITIZE(TQt::Key_Dead_Grave), UNITIZE(TQt::Key_Multi_key), 0x0029, 0x0397, 0, 0}, 0x1f2a }, // U1F2A # GREEK CAPITAL LETTER ETA WITH PSILI AND VARIA
    { {UNITIZE(TQt::Key_Dead_Grave), UNITIZE(TQt::Key_Multi_key), 0x0029, 0x0399, 0, 0}, 0x1f3a }, // U1F3A # GREEK CAPITAL LETTER IOTA WITH PSILI AND VARIA
    { {UNITIZE(TQt::Key_Dead_Grave), UNITIZE(TQt::Key_Multi_key), 0x0029, 0x039f, 0, 0}, 0x1f4a }, // U1F4A # GREEK CAPITAL LETTER OMICRON WITH PSILI AND VARIA
    { {UNITIZE(TQt::Key_Dead_Grave), UNITIZE(TQt::Key_Multi_key), 0x0029, 0x03a9, 0, 0}, 0x1f6a }, // U1F6A # GREEK CAPITAL LETTER OMEGA WITH PSILI AND VARIA
    { {UNITIZE(TQt::Key_Dead_Grave), UNITIZE(TQt::Key_Multi_key), 0x0029, 0x03b1, 0, 0}, 0x1f02 }, // U1F02 # GREEK SMALL LETTER ALPHA WITH PSILI AND VARIA
    { {UNITIZE(TQt::Key_Dead_Grave), UNITIZE(TQt::Key_Multi_key), 0x0029, 0x03b5, 0, 0}, 0x1f12 }, // U1F12 # GREEK SMALL LETTER EPSILON WITH PSILI AND VARIA
    { {UNITIZE(TQt::Key_Dead_Grave), UNITIZE(TQt::Key_Multi_key), 0x0029, 0x03b7, 0, 0}, 0x1f22 }, // U1F22 # GREEK SMALL LETTER ETA WITH PSILI AND VARIA
    { {UNITIZE(TQt::Key_Dead_Grave), UNITIZE(TQt::Key_Multi_key), 0x0029, 0x03b9, 0, 0}, 0x1f32 }, // U1F32 # GREEK SMALL LETTER IOTA WITH PSILI AND VARIA
    { {UNITIZE(TQt::Key_Dead_Grave), UNITIZE(TQt::Key_Multi_key), 0x0029, 0x03bf, 0, 0}, 0x1f42 }, // U1F42 # GREEK SMALL LETTER OMICRON WITH PSILI AND VARIA
    { {UNITIZE(TQt::Key_Dead_Grave), UNITIZE(TQt::Key_Multi_key), 0x0029, 0x03c5, 0, 0}, 0x1f52 }, // U1F52 # GREEK SMALL LETTER UPSILON WITH PSILI AND VARIA
    { {UNITIZE(TQt::Key_Dead_Grave), UNITIZE(TQt::Key_Multi_key), 0x0029, 0x03c9, 0, 0}, 0x1f62 }, // U1F62 # GREEK SMALL LETTER OMEGA WITH PSILI AND VARIA
    { {UNITIZE(TQt::Key_Dead_Grave), UNITIZE(TQt::Key_Multi_key), 0x002b, 0x004f, 0, 0}, 0x1edc }, // U1EDC # LATIN CAPITAL LETTER O WITH HORN AND GRAVE
    { {UNITIZE(TQt::Key_Dead_Grave), UNITIZE(TQt::Key_Multi_key), 0x002b, 0x0055, 0, 0}, 0x1eea }, // U1EEA # LATIN CAPITAL LETTER U WITH HORN AND GRAVE
    { {UNITIZE(TQt::Key_Dead_Grave), UNITIZE(TQt::Key_Multi_key), 0x002b, 0x006f, 0, 0}, 0x1edd }, // U1EDD # LATIN SMALL LETTER O WITH HORN AND GRAVE
    { {UNITIZE(TQt::Key_Dead_Grave), UNITIZE(TQt::Key_Multi_key), 0x002b, 0x0075, 0, 0}, 0x1eeb }, // U1EEB # LATIN SMALL LETTER U WITH HORN AND GRAVE
    { {UNITIZE(TQt::Key_Dead_Grave), UNITIZE(TQt::Key_Multi_key), 0x0055, 0x0041, 0, 0}, 0x1eb0 }, // U1EB0 # LATIN CAPITAL LETTER A WITH BREVE AND GRAVE
    { {UNITIZE(TQt::Key_Dead_Grave), UNITIZE(TQt::Key_Multi_key), 0x0055, 0x0061, 0, 0}, 0x1eb1 }, // U1EB1 # LATIN SMALL LETTER A WITH BREVE AND GRAVE
    { {UNITIZE(TQt::Key_Dead_Grave), UNITIZE(TQt::Key_Multi_key), 0x005e, 0x0041, 0, 0}, 0x1ea6 }, // U1EA6 # LATIN CAPITAL LETTER A WITH CIRCUMFLEX AND GRAVE
    { {UNITIZE(TQt::Key_Dead_Grave), UNITIZE(TQt::Key_Multi_key), 0x005e, 0x0045, 0, 0}, 0x1ec0 }, // U1EC0 # LATIN CAPITAL LETTER E WITH CIRCUMFLEX AND GRAVE
    { {UNITIZE(TQt::Key_Dead_Grave), UNITIZE(TQt::Key_Multi_key), 0x005e, 0x004f, 0, 0}, 0x1ed2 }, // U1ED2 # LATIN CAPITAL LETTER O WITH CIRCUMFLEX AND GRAVE
    { {UNITIZE(TQt::Key_Dead_Grave), UNITIZE(TQt::Key_Multi_key), 0x005e, 0x0061, 0, 0}, 0x1ea7 }, // U1EA7 # LATIN SMALL LETTER A WITH CIRCUMFLEX AND GRAVE
    { {UNITIZE(TQt::Key_Dead_Grave), UNITIZE(TQt::Key_Multi_key), 0x005e, 0x0065, 0, 0}, 0x1ec1 }, // U1EC1 # LATIN SMALL LETTER E WITH CIRCUMFLEX AND GRAVE
    { {UNITIZE(TQt::Key_Dead_Grave), UNITIZE(TQt::Key_Multi_key), 0x005e, 0x006f, 0, 0}, 0x1ed3 }, // U1ED3 # LATIN SMALL LETTER O WITH CIRCUMFLEX AND GRAVE
    { {UNITIZE(TQt::Key_Dead_Grave), UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0045, 0, 0}, 0x1e14 }, // U1E14 # LATIN CAPITAL LETTER E WITH MACRON AND GRAVE
    { {UNITIZE(TQt::Key_Dead_Grave), UNITIZE(TQt::Key_Multi_key), 0x005f, 0x004f, 0, 0}, 0x1e50 }, // U1E50 # LATIN CAPITAL LETTER O WITH MACRON AND GRAVE
    { {UNITIZE(TQt::Key_Dead_Grave), UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0065, 0, 0}, 0x1e15 }, // U1E15 # LATIN SMALL LETTER E WITH MACRON AND GRAVE
    { {UNITIZE(TQt::Key_Dead_Grave), UNITIZE(TQt::Key_Multi_key), 0x005f, 0x006f, 0, 0}, 0x1e51 }, // U1E51 # LATIN SMALL LETTER O WITH MACRON AND GRAVE
    { {UNITIZE(TQt::Key_Dead_Grave), UNITIZE(TQt::Key_Multi_key), 0x0062, 0x0041, 0, 0}, 0x1eb0 }, // U1EB0 # LATIN CAPITAL LETTER A WITH BREVE AND GRAVE
    { {UNITIZE(TQt::Key_Dead_Grave), UNITIZE(TQt::Key_Multi_key), 0x0062, 0x0061, 0, 0}, 0x1eb1 }, // U1EB1 # LATIN SMALL LETTER A WITH BREVE AND GRAVE
    { {UNITIZE(TQt::Key_Dead_Grave), UNITIZE(TQt::Key_Multi_key), 0x00af, 0x0045, 0, 0}, 0x1e14 }, // U1E14 # LATIN CAPITAL LETTER E WITH MACRON AND GRAVE
    { {UNITIZE(TQt::Key_Dead_Grave), UNITIZE(TQt::Key_Multi_key), 0x00af, 0x004f, 0, 0}, 0x1e50 }, // U1E50 # LATIN CAPITAL LETTER O WITH MACRON AND GRAVE
    { {UNITIZE(TQt::Key_Dead_Grave), UNITIZE(TQt::Key_Multi_key), 0x00af, 0x0065, 0, 0}, 0x1e15 }, // U1E15 # LATIN SMALL LETTER E WITH MACRON AND GRAVE
    { {UNITIZE(TQt::Key_Dead_Grave), UNITIZE(TQt::Key_Multi_key), 0x00af, 0x006f, 0, 0}, 0x1e51 }, // U1E51 # LATIN SMALL LETTER O WITH MACRON AND GRAVE
    { {UNITIZE(TQt::Key_Dead_Grave), UNITIZE(TQt::Key_Dead_Grave), 0, 0, 0, 0}, 0x0060 }, // grave
    { {UNITIZE(TQt::Key_Dead_Grave), UNITIZE(TQt::Key_Dead_Circumflex), 0x0041, 0, 0, 0}, 0x1ea6 }, // U1EA6 # LATIN CAPITAL LETTER A WITH CIRCUMFLEX AND GRAVE
    { {UNITIZE(TQt::Key_Dead_Grave), UNITIZE(TQt::Key_Dead_Circumflex), 0x0045, 0, 0, 0}, 0x1ec0 }, // U1EC0 # LATIN CAPITAL LETTER E WITH CIRCUMFLEX AND GRAVE
    { {UNITIZE(TQt::Key_Dead_Grave), UNITIZE(TQt::Key_Dead_Circumflex), 0x004f, 0, 0, 0}, 0x1ed2 }, // U1ED2 # LATIN CAPITAL LETTER O WITH CIRCUMFLEX AND GRAVE
    { {UNITIZE(TQt::Key_Dead_Grave), UNITIZE(TQt::Key_Dead_Circumflex), 0x0061, 0, 0, 0}, 0x1ea7 }, // U1EA7 # LATIN SMALL LETTER A WITH CIRCUMFLEX AND GRAVE
    { {UNITIZE(TQt::Key_Dead_Grave), UNITIZE(TQt::Key_Dead_Circumflex), 0x0065, 0, 0, 0}, 0x1ec1 }, // U1EC1 # LATIN SMALL LETTER E WITH CIRCUMFLEX AND GRAVE
    { {UNITIZE(TQt::Key_Dead_Grave), UNITIZE(TQt::Key_Dead_Circumflex), 0x006f, 0, 0, 0}, 0x1ed3 }, // U1ED3 # LATIN SMALL LETTER O WITH CIRCUMFLEX AND GRAVE
    { {UNITIZE(TQt::Key_Dead_Grave), UNITIZE(TQt::Key_Dead_Macron), 0x0045, 0, 0, 0}, 0x1e14 }, // U1E14 # LATIN CAPITAL LETTER E WITH MACRON AND GRAVE
    { {UNITIZE(TQt::Key_Dead_Grave), UNITIZE(TQt::Key_Dead_Macron), 0x004f, 0, 0, 0}, 0x1e50 }, // U1E50 # LATIN CAPITAL LETTER O WITH MACRON AND GRAVE
    { {UNITIZE(TQt::Key_Dead_Grave), UNITIZE(TQt::Key_Dead_Macron), 0x0065, 0, 0, 0}, 0x1e15 }, // U1E15 # LATIN SMALL LETTER E WITH MACRON AND GRAVE
    { {UNITIZE(TQt::Key_Dead_Grave), UNITIZE(TQt::Key_Dead_Macron), 0x006f, 0, 0, 0}, 0x1e51 }, // U1E51 # LATIN SMALL LETTER O WITH MACRON AND GRAVE
    { {UNITIZE(TQt::Key_Dead_Grave), UNITIZE(TQt::Key_Dead_Breve), 0x0041, 0, 0, 0}, 0x1eb0 }, // U1EB0 # LATIN CAPITAL LETTER A WITH BREVE AND GRAVE
    { {UNITIZE(TQt::Key_Dead_Grave), UNITIZE(TQt::Key_Dead_Breve), 0x0061, 0, 0, 0}, 0x1eb1 }, // U1EB1 # LATIN SMALL LETTER A WITH BREVE AND GRAVE
    { {UNITIZE(TQt::Key_Dead_Grave), UNITIZE(TQt::Key_Dead_Diaeresis), 0x0055, 0, 0, 0}, 0x01db }, // U01DB # LATIN CAPITAL LETTER U WITH DIAERESIS AND GRAVE
    { {UNITIZE(TQt::Key_Dead_Grave), UNITIZE(TQt::Key_Dead_Diaeresis), 0x0075, 0, 0, 0}, 0x01dc }, // U01DC # LATIN SMALL LETTER U WITH DIAERESIS AND GRAVE
    { {UNITIZE(TQt::Key_Dead_Grave), UNITIZE(TQt::Key_Dead_Diaeresis), 0x03b9, 0, 0, 0}, 0x1fd2 }, // U1FD2 # GREEK SMALL LETTER IOTA WITH DIALYTIKA AND VARIA
    { {UNITIZE(TQt::Key_Dead_Grave), UNITIZE(TQt::Key_Dead_Diaeresis), 0x03c5, 0, 0, 0}, 0x1fe2 }, // U1FE2 # GREEK SMALL LETTER UPSILON WITH DIALYTIKA AND VARIA
    { {UNITIZE(TQt::Key_Dead_Grave), UNITIZE(TQt::Key_Dead_Horn), 0x004f, 0, 0, 0}, 0x1edc }, // U1EDC # LATIN CAPITAL LETTER O WITH HORN AND GRAVE
    { {UNITIZE(TQt::Key_Dead_Grave), UNITIZE(TQt::Key_Dead_Horn), 0x0055, 0, 0, 0}, 0x1eea }, // U1EEA # LATIN CAPITAL LETTER U WITH HORN AND GRAVE
    { {UNITIZE(TQt::Key_Dead_Grave), UNITIZE(TQt::Key_Dead_Horn), 0x006f, 0, 0, 0}, 0x1edd }, // U1EDD # LATIN SMALL LETTER O WITH HORN AND GRAVE
    { {UNITIZE(TQt::Key_Dead_Grave), UNITIZE(TQt::Key_Dead_Horn), 0x0075, 0, 0, 0}, 0x1eeb }, // U1EEB # LATIN SMALL LETTER U WITH HORN AND GRAVE
// /* broken */    { {UNITIZE(TQt::Key_Dead_Acute), 0x0000, 0, 0, 0, 0}, 0x1eda }, // U1EDA # LATIN CAPITAL LETTER O WITH HORN AND ACUTE
// /* broken */    { {UNITIZE(TQt::Key_Dead_Acute), 0x0000, 0, 0, 0, 0}, 0x1ee9 }, // U1EE9 # LATIN SMALL LETTER U WITH HORN AND ACUTE
// /* broken */    { {UNITIZE(TQt::Key_Dead_Acute), 0x0000, 0, 0, 0, 0}, 0x1edb }, // U1EDB # LATIN SMALL LETTER O WITH HORN AND ACUTE
// /* broken */    { {UNITIZE(TQt::Key_Dead_Acute), 0x0000, 0, 0, 0, 0}, 0x1ee8 }, // U1EE8 # LATIN CAPITAL LETTER U WITH HORN AND ACUTE
// /* broken */    { {UNITIZE(TQt::Key_Dead_Acute), 0x0000, 0x004f, 0, 0, 0}, 0x1e4c }, // U1E4C # LATIN CAPITAL LETTER O WITH TILDE AND ACUTE
// /* broken */    { {UNITIZE(TQt::Key_Dead_Acute), 0x0000, 0x0055, 0, 0, 0}, 0x1e78 }, // U1E78 # LATIN CAPITAL LETTER U WITH TILDE AND ACUTE
// /* broken */    { {UNITIZE(TQt::Key_Dead_Acute), 0x0000, 0x006f, 0, 0, 0}, 0x1e4d }, // U1E4D # LATIN SMALL LETTER O WITH TILDE AND ACUTE
// /* broken */    { {UNITIZE(TQt::Key_Dead_Acute), 0x0000, 0x0075, 0, 0, 0}, 0x1e79 }, // U1E79 # LATIN SMALL LETTER U WITH TILDE AND ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), 0x0020, 0, 0, 0, 0}, 0x0027 }, // apostrophe
    { {UNITIZE(TQt::Key_Dead_Acute), 0x0041, 0, 0, 0, 0}, 0x00c1 }, // U00C1 # LATIN CAPITAL LETTER A WITH ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), 0x0043, 0, 0, 0, 0}, 0x0106 }, // U0106 # LATIN CAPITAL LETTER C WITH ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), 0x0045, 0, 0, 0, 0}, 0x00c9 }, // U00C9 # LATIN CAPITAL LETTER E WITH ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), 0x0047, 0, 0, 0, 0}, 0x01f4 }, // U01F4 # LATIN CAPITAL LETTER G WITH ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), 0x0049, 0, 0, 0, 0}, 0x00cd }, // U00CD # LATIN CAPITAL LETTER I WITH ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), 0x004b, 0, 0, 0, 0}, 0x1e30 }, // U1E30 # LATIN CAPITAL LETTER K WITH ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), 0x004c, 0, 0, 0, 0}, 0x0139 }, // U0139 # LATIN CAPITAL LETTER L WITH ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), 0x004d, 0, 0, 0, 0}, 0x1e3e }, // U1E3E # LATIN CAPITAL LETTER M WITH ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), 0x004e, 0, 0, 0, 0}, 0x0143 }, // U0143 # LATIN CAPITAL LETTER N WITH ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), 0x004f, 0, 0, 0, 0}, 0x00d3 }, // U00D3 # LATIN CAPITAL LETTER O WITH ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), 0x0050, 0, 0, 0, 0}, 0x1e54 }, // U1E54 # LATIN CAPITAL LETTER P WITH ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), 0x0052, 0, 0, 0, 0}, 0x0154 }, // U0154 # LATIN CAPITAL LETTER R WITH ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), 0x0053, 0, 0, 0, 0}, 0x015a }, // U015A # LATIN CAPITAL LETTER S WITH ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), 0x0055, 0, 0, 0, 0}, 0x00da }, // U00DA # LATIN CAPITAL LETTER U WITH ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), 0x0057, 0, 0, 0, 0}, 0x1e82 }, // U1E82 # LATIN CAPITAL LETTER W WITH ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), 0x0059, 0, 0, 0, 0}, 0x00dd }, // U00DD # LATIN CAPITAL LETTER Y WITH ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), 0x005a, 0, 0, 0, 0}, 0x0179 }, // U0179 # LATIN CAPITAL LETTER Z WITH ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), 0x0061, 0, 0, 0, 0}, 0x00e1 }, // U00E1 # LATIN SMALL LETTER A WITH ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), 0x0063, 0, 0, 0, 0}, 0x0107 }, // U0107 # LATIN SMALL LETTER C WITH ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), 0x0065, 0, 0, 0, 0}, 0x00e9 }, // U00E9 # LATIN SMALL LETTER E WITH ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), 0x0067, 0, 0, 0, 0}, 0x01f5 }, // U01F5 # LATIN SMALL LETTER G WITH ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), 0x0069, 0, 0, 0, 0}, 0x00ed }, // U00ED # LATIN SMALL LETTER I WITH ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), 0x006b, 0, 0, 0, 0}, 0x1e31 }, // U1E31 # LATIN SMALL LETTER K WITH ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), 0x006c, 0, 0, 0, 0}, 0x013a }, // U013A # LATIN SMALL LETTER L WITH ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), 0x006d, 0, 0, 0, 0}, 0x1e3f }, // U1E3F # LATIN SMALL LETTER M WITH ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), 0x006e, 0, 0, 0, 0}, 0x0144 }, // U0144 # LATIN SMALL LETTER N WITH ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), 0x006f, 0, 0, 0, 0}, 0x00f3 }, // U00F3 # LATIN SMALL LETTER O WITH ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), 0x0070, 0, 0, 0, 0}, 0x1e55 }, // U1E55 # LATIN SMALL LETTER P WITH ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), 0x0072, 0, 0, 0, 0}, 0x0155 }, // U0155 # LATIN SMALL LETTER R WITH ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), 0x0073, 0, 0, 0, 0}, 0x015b }, // U015B # LATIN SMALL LETTER S WITH ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), 0x0075, 0, 0, 0, 0}, 0x00fa }, // U00FA # LATIN SMALL LETTER U WITH ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), 0x0077, 0, 0, 0, 0}, 0x1e83 }, // U1E83 # LATIN SMALL LETTER W WITH ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), 0x0079, 0, 0, 0, 0}, 0x00fd }, // U00FD # LATIN SMALL LETTER Y WITH ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), 0x007a, 0, 0, 0, 0}, 0x017a }, // U017A # LATIN SMALL LETTER Z WITH ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), 0x00c2, 0, 0, 0, 0}, 0x1ea4 }, // U1EA4 # LATIN CAPITAL LETTER A WITH CIRCUMFLEX AND ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), 0x00c5, 0, 0, 0, 0}, 0x01fa }, // U01FA # LATIN CAPITAL LETTER A WITH RING ABOVE AND ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), 0x00c6, 0, 0, 0, 0}, 0x01fc }, // U01FC # LATIN CAPITAL LETTER AE WITH ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), 0x00c7, 0, 0, 0, 0}, 0x1e08 }, // U1E08 # LATIN CAPITAL LETTER C WITH CEDILLA AND ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), 0x00ca, 0, 0, 0, 0}, 0x1ebe }, // U1EBE # LATIN CAPITAL LETTER E WITH CIRCUMFLEX AND ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), 0x00cf, 0, 0, 0, 0}, 0x1e2e }, // U1E2E # LATIN CAPITAL LETTER I WITH DIAERESIS AND ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), 0x00d4, 0, 0, 0, 0}, 0x1ed0 }, // U1ED0 # LATIN CAPITAL LETTER O WITH CIRCUMFLEX AND ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), 0x00d5, 0, 0, 0, 0}, 0x1e4c }, // U1E4C # LATIN CAPITAL LETTER O WITH TILDE AND ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), 0x00d8, 0, 0, 0, 0}, 0x01fe }, // U01FE # LATIN CAPITAL LETTER O WITH STROKE AND ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), 0x00dc, 0, 0, 0, 0}, 0x01d7 }, // U01D7 # LATIN CAPITAL LETTER U WITH DIAERESIS AND ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), 0x00e2, 0, 0, 0, 0}, 0x1ea5 }, // U1EA5 # LATIN SMALL LETTER A WITH CIRCUMFLEX AND ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), 0x00e5, 0, 0, 0, 0}, 0x01fb }, // U01FB # LATIN SMALL LETTER A WITH RING ABOVE AND ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), 0x00e6, 0, 0, 0, 0}, 0x01fd }, // U01FD # LATIN SMALL LETTER AE WITH ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), 0x00e7, 0, 0, 0, 0}, 0x1e09 }, // U1E09 # LATIN SMALL LETTER C WITH CEDILLA AND ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), 0x00ea, 0, 0, 0, 0}, 0x1ebf }, // U1EBF # LATIN SMALL LETTER E WITH CIRCUMFLEX AND ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), 0x00ef, 0, 0, 0, 0}, 0x1e2f }, // U1E2F # LATIN SMALL LETTER I WITH DIAERESIS AND ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), 0x00f4, 0, 0, 0, 0}, 0x1ed1 }, // U1ED1 # LATIN SMALL LETTER O WITH CIRCUMFLEX AND ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), 0x00f5, 0, 0, 0, 0}, 0x1e4d }, // U1E4D # LATIN SMALL LETTER O WITH TILDE AND ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), 0x00f8, 0, 0, 0, 0}, 0x01ff }, // U01FF # LATIN SMALL LETTER O WITH STROKE AND ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), 0x00fc, 0, 0, 0, 0}, 0x01d8 }, // U01D8 # LATIN SMALL LETTER U WITH DIAERESIS AND ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), 0x0102, 0, 0, 0, 0}, 0x1eae }, // U1EAE # LATIN CAPITAL LETTER A WITH BREVE AND ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), 0x0103, 0, 0, 0, 0}, 0x1eaf }, // U1EAF # LATIN SMALL LETTER A WITH BREVE AND ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), 0x0112, 0, 0, 0, 0}, 0x1e16 }, // U1E16 # LATIN CAPITAL LETTER E WITH MACRON AND ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), 0x0113, 0, 0, 0, 0}, 0x1e17 }, // U1E17 # LATIN SMALL LETTER E WITH MACRON AND ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), 0x014c, 0, 0, 0, 0}, 0x1e52 }, // U1E52 # LATIN CAPITAL LETTER O WITH MACRON AND ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), 0x014d, 0, 0, 0, 0}, 0x1e53 }, // U1E53 # LATIN SMALL LETTER O WITH MACRON AND ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), 0x0168, 0, 0, 0, 0}, 0x1e78 }, // U1E78 # LATIN CAPITAL LETTER U WITH TILDE AND ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), 0x0169, 0, 0, 0, 0}, 0x1e79 }, // U1E79 # LATIN SMALL LETTER U WITH TILDE AND ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), 0x01A0, 0, 0, 0, 0}, 0x1eda }, // U1EDA # LATIN CAPITAL LETTER O WITH HORN AND ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), 0x01A1, 0, 0, 0, 0}, 0x1edb }, // U1EDB # LATIN SMALL LETTER O WITH HORN AND ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), 0x01AF, 0, 0, 0, 0}, 0x1ee8 }, // U1EE8 # LATIN CAPITAL LETTER U WITH HORN AND ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), 0x01B0, 0, 0, 0, 0}, 0x1ee9 }, // U1EE9 # LATIN SMALL LETTER U WITH HORN AND ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), 0x0313, 0x0391, 0, 0, 0}, 0x1f0c }, // U1F0C # GREEK CAPITAL LETTER ALPHA WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Dead_Acute), 0x0313, 0x0395, 0, 0, 0}, 0x1f1c }, // U1F1C # GREEK CAPITAL LETTER EPSILON WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Dead_Acute), 0x0313, 0x0397, 0, 0, 0}, 0x1f2c }, // U1F2C # GREEK CAPITAL LETTER ETA WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Dead_Acute), 0x0313, 0x0399, 0, 0, 0}, 0x1f3c }, // U1F3C # GREEK CAPITAL LETTER IOTA WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Dead_Acute), 0x0313, 0x039f, 0, 0, 0}, 0x1f4c }, // U1F4C # GREEK CAPITAL LETTER OMICRON WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Dead_Acute), 0x0313, 0x03a9, 0, 0, 0}, 0x1f6c }, // U1F6C # GREEK CAPITAL LETTER OMEGA WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Dead_Acute), 0x0313, 0x03b1, 0, 0, 0}, 0x1f04 }, // U1F04 # GREEK SMALL LETTER ALPHA WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Dead_Acute), 0x0313, 0x03b5, 0, 0, 0}, 0x1f14 }, // U1F14 # GREEK SMALL LETTER EPSILON WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Dead_Acute), 0x0313, 0x03b7, 0, 0, 0}, 0x1f24 }, // U1F24 # GREEK SMALL LETTER ETA WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Dead_Acute), 0x0313, 0x03b9, 0, 0, 0}, 0x1f34 }, // U1F34 # GREEK SMALL LETTER IOTA WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Dead_Acute), 0x0313, 0x03bf, 0, 0, 0}, 0x1f44 }, // U1F44 # GREEK SMALL LETTER OMICRON WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Dead_Acute), 0x0313, 0x03c5, 0, 0, 0}, 0x1f54 }, // U1F54 # GREEK SMALL LETTER UPSILON WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Dead_Acute), 0x0313, 0x03c9, 0, 0, 0}, 0x1f64 }, // U1F64 # GREEK SMALL LETTER OMEGA WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Dead_Acute), 0x0314, 0x0391, 0, 0, 0}, 0x1f0d }, // U1F0D # GREEK CAPITAL LETTER ALPHA WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Dead_Acute), 0x0314, 0x0395, 0, 0, 0}, 0x1f1d }, // U1F1D # GREEK CAPITAL LETTER EPSILON WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Dead_Acute), 0x0314, 0x0397, 0, 0, 0}, 0x1f2d }, // U1F2D # GREEK CAPITAL LETTER ETA WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Dead_Acute), 0x0314, 0x0399, 0, 0, 0}, 0x1f3d }, // U1F3D # GREEK CAPITAL LETTER IOTA WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Dead_Acute), 0x0314, 0x039f, 0, 0, 0}, 0x1f4d }, // U1F4D # GREEK CAPITAL LETTER OMICRON WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Dead_Acute), 0x0314, 0x03a5, 0, 0, 0}, 0x1f5d }, // U1F5D # GREEK CAPITAL LETTER UPSILON WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Dead_Acute), 0x0314, 0x03a9, 0, 0, 0}, 0x1f6d }, // U1F6D # GREEK CAPITAL LETTER OMEGA WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Dead_Acute), 0x0314, 0x03b1, 0, 0, 0}, 0x1f05 }, // U1F05 # GREEK SMALL LETTER ALPHA WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Dead_Acute), 0x0314, 0x03b5, 0, 0, 0}, 0x1f15 }, // U1F15 # GREEK SMALL LETTER EPSILON WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Dead_Acute), 0x0314, 0x03b7, 0, 0, 0}, 0x1f25 }, // U1F25 # GREEK SMALL LETTER ETA WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Dead_Acute), 0x0314, 0x03b9, 0, 0, 0}, 0x1f35 }, // U1F35 # GREEK SMALL LETTER IOTA WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Dead_Acute), 0x0314, 0x03bf, 0, 0, 0}, 0x1f45 }, // U1F45 # GREEK SMALL LETTER OMICRON WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Dead_Acute), 0x0314, 0x03c5, 0, 0, 0}, 0x1f55 }, // U1F55 # GREEK SMALL LETTER UPSILON WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Dead_Acute), 0x0314, 0x03c9, 0, 0, 0}, 0x1f65 }, // U1F65 # GREEK SMALL LETTER OMEGA WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Dead_Acute), 0x0391, 0, 0, 0, 0}, 0x0386 }, // U0386 # GREEK CAPITAL LETTER ALPHA WITH TONOS
    { {UNITIZE(TQt::Key_Dead_Acute), 0x0395, 0, 0, 0, 0}, 0x0388 }, // U0388 # GREEK CAPITAL LETTER EPSILON WITH TONOS
    { {UNITIZE(TQt::Key_Dead_Acute), 0x0397, 0, 0, 0, 0}, 0x0389 }, // U0389 # GREEK CAPITAL LETTER ETA WITH TONOS
    { {UNITIZE(TQt::Key_Dead_Acute), 0x0399, 0, 0, 0, 0}, 0x038a }, // U038A # GREEK CAPITAL LETTER IOTA WITH TONOS
    { {UNITIZE(TQt::Key_Dead_Acute), 0x039f, 0, 0, 0, 0}, 0x038c }, // U038C # GREEK CAPITAL LETTER OMICRON WITH TONOS
    { {UNITIZE(TQt::Key_Dead_Acute), 0x03a5, 0, 0, 0, 0}, 0x038e }, // U038E # GREEK CAPITAL LETTER UPSILON WITH TONOS
    { {UNITIZE(TQt::Key_Dead_Acute), 0x03a9, 0, 0, 0, 0}, 0x038f }, // U038F # GREEK CAPITAL LETTER OMEGA WITH TONOS
    { {UNITIZE(TQt::Key_Dead_Acute), 0x03b1, 0, 0, 0, 0}, 0x03ac }, // U03AC # GREEK SMALL LETTER ALPHA WITH TONOS
    { {UNITIZE(TQt::Key_Dead_Acute), 0x03b5, 0, 0, 0, 0}, 0x03ad }, // U03AD # GREEK SMALL LETTER EPSILON WITH TONOS
    { {UNITIZE(TQt::Key_Dead_Acute), 0x03b7, 0, 0, 0, 0}, 0x03ae }, // U03AE # GREEK SMALL LETTER ETA WITH TONOS
    { {UNITIZE(TQt::Key_Dead_Acute), 0x03b9, 0, 0, 0, 0}, 0x03af }, // U03AF # GREEK SMALL LETTER IOTA WITH TONOS
    { {UNITIZE(TQt::Key_Dead_Acute), 0x03bf, 0, 0, 0, 0}, 0x03cc }, // U03CC # GREEK SMALL LETTER OMICRON WITH TONOS
    { {UNITIZE(TQt::Key_Dead_Acute), 0x03c5, 0, 0, 0, 0}, 0x03cd }, // U03CD # GREEK SMALL LETTER UPSILON WITH TONOS
    { {UNITIZE(TQt::Key_Dead_Acute), 0x03c9, 0, 0, 0, 0}, 0x03ce }, // U03CE # GREEK SMALL LETTER OMEGA WITH TONOS
    { {UNITIZE(TQt::Key_Dead_Acute), 0x03ca, 0, 0, 0, 0}, 0x0390 }, // U0390 # GREEK SMALL LETTER IOTA WITH DIALYTIKA AND TONOS
    { {UNITIZE(TQt::Key_Dead_Acute), 0x03cb, 0, 0, 0, 0}, 0x03b0 }, // U03B0 # GREEK SMALL LETTER UPSILON WITH DIALYTIKA AND TONOS
    { {UNITIZE(TQt::Key_Dead_Acute), 0x0413, 0, 0, 0, 0}, 0x0403 }, // U0403 # CYRILLIC CAPITAL LETTER GJE
    { {UNITIZE(TQt::Key_Dead_Acute), 0x041a, 0, 0, 0, 0}, 0x040c }, // U040C # CYRILLIC CAPITAL LETTER KJE
    { {UNITIZE(TQt::Key_Dead_Acute), 0x0433, 0, 0, 0, 0}, 0x0453 }, // U0453 # CYRILLIC SMALL LETTER GJE
    { {UNITIZE(TQt::Key_Dead_Acute), 0x043a, 0, 0, 0, 0}, 0x045c }, // U045C # CYRILLIC SMALL LETTER KJE
    { {UNITIZE(TQt::Key_Dead_Acute), 0x1f00, 0, 0, 0, 0}, 0x1f04 }, // U1F04 # GREEK SMALL LETTER ALPHA WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Dead_Acute), 0x1f01, 0, 0, 0, 0}, 0x1f05 }, // U1F05 # GREEK SMALL LETTER ALPHA WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Dead_Acute), 0x1f08, 0, 0, 0, 0}, 0x1f0c }, // U1F0C # GREEK CAPITAL LETTER ALPHA WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Dead_Acute), 0x1f09, 0, 0, 0, 0}, 0x1f0d }, // U1F0D # GREEK CAPITAL LETTER ALPHA WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Dead_Acute), 0x1f10, 0, 0, 0, 0}, 0x1f14 }, // U1F14 # GREEK SMALL LETTER EPSILON WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Dead_Acute), 0x1f11, 0, 0, 0, 0}, 0x1f15 }, // U1F15 # GREEK SMALL LETTER EPSILON WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Dead_Acute), 0x1f18, 0, 0, 0, 0}, 0x1f1c }, // U1F1C # GREEK CAPITAL LETTER EPSILON WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Dead_Acute), 0x1f19, 0, 0, 0, 0}, 0x1f1d }, // U1F1D # GREEK CAPITAL LETTER EPSILON WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Dead_Acute), 0x1f20, 0, 0, 0, 0}, 0x1f24 }, // U1F24 # GREEK SMALL LETTER ETA WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Dead_Acute), 0x1f21, 0, 0, 0, 0}, 0x1f25 }, // U1F25 # GREEK SMALL LETTER ETA WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Dead_Acute), 0x1f28, 0, 0, 0, 0}, 0x1f2c }, // U1F2C # GREEK CAPITAL LETTER ETA WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Dead_Acute), 0x1f29, 0, 0, 0, 0}, 0x1f2d }, // U1F2D # GREEK CAPITAL LETTER ETA WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Dead_Acute), 0x1f30, 0, 0, 0, 0}, 0x1f34 }, // U1F34 # GREEK SMALL LETTER IOTA WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Dead_Acute), 0x1f31, 0, 0, 0, 0}, 0x1f35 }, // U1F35 # GREEK SMALL LETTER IOTA WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Dead_Acute), 0x1f38, 0, 0, 0, 0}, 0x1f3c }, // U1F3C # GREEK CAPITAL LETTER IOTA WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Dead_Acute), 0x1f39, 0, 0, 0, 0}, 0x1f3d }, // U1F3D # GREEK CAPITAL LETTER IOTA WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Dead_Acute), 0x1f40, 0, 0, 0, 0}, 0x1f44 }, // U1F44 # GREEK SMALL LETTER OMICRON WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Dead_Acute), 0x1f41, 0, 0, 0, 0}, 0x1f45 }, // U1F45 # GREEK SMALL LETTER OMICRON WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Dead_Acute), 0x1f48, 0, 0, 0, 0}, 0x1f4c }, // U1F4C # GREEK CAPITAL LETTER OMICRON WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Dead_Acute), 0x1f49, 0, 0, 0, 0}, 0x1f4d }, // U1F4D # GREEK CAPITAL LETTER OMICRON WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Dead_Acute), 0x1f50, 0, 0, 0, 0}, 0x1f54 }, // U1F54 # GREEK SMALL LETTER UPSILON WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Dead_Acute), 0x1f51, 0, 0, 0, 0}, 0x1f55 }, // U1F55 # GREEK SMALL LETTER UPSILON WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Dead_Acute), 0x1f59, 0, 0, 0, 0}, 0x1f5d }, // U1F5D # GREEK CAPITAL LETTER UPSILON WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Dead_Acute), 0x1f60, 0, 0, 0, 0}, 0x1f64 }, // U1F64 # GREEK SMALL LETTER OMEGA WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Dead_Acute), 0x1f61, 0, 0, 0, 0}, 0x1f65 }, // U1F65 # GREEK SMALL LETTER OMEGA WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Dead_Acute), 0x1f68, 0, 0, 0, 0}, 0x1f6c }, // U1F6C # GREEK CAPITAL LETTER OMEGA WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Dead_Acute), 0x1f69, 0, 0, 0, 0}, 0x1f6d }, // U1F6D # GREEK CAPITAL LETTER OMEGA WITH DASIA AND OXIA
// /* broken */    { {UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Multi_key), 0x0000, 0x004f, 0, 0}, 0x01fe }, // U01FE # LATIN CAPITAL LETTER O WITH STROKE AND ACUTE
// /* broken */    { {UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Multi_key), 0x0000, 0x006f, 0, 0}, 0x01ff }, // U01FF # LATIN SMALL LETTER O WITH STROKE AND ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Multi_key), 0x0022, 0x0049, 0, 0}, 0x1e2e }, // U1E2E # LATIN CAPITAL LETTER I WITH DIAERESIS AND ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Multi_key), 0x0022, 0x0055, 0, 0}, 0x01d7 }, // U01D7 # LATIN CAPITAL LETTER U WITH DIAERESIS AND ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Multi_key), 0x0022, 0x0069, 0, 0}, 0x1e2f }, // U1E2F # LATIN SMALL LETTER I WITH DIAERESIS AND ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Multi_key), 0x0022, 0x0075, 0, 0}, 0x01d8 }, // U01D8 # LATIN SMALL LETTER U WITH DIAERESIS AND ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Multi_key), 0x0022, 0x03b9, 0, 0}, 0x0390 }, // U0390 # GREEK SMALL LETTER IOTA WITH DIALYTIKA AND TONOS
    { {UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Multi_key), 0x0022, 0x03c5, 0, 0}, 0x03b0 }, // U03B0 # GREEK SMALL LETTER UPSILON WITH DIALYTIKA AND TONOS
    { {UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0391, 0, 0}, 0x1f0d }, // U1F0D # GREEK CAPITAL LETTER ALPHA WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0395, 0, 0}, 0x1f1d }, // U1F1D # GREEK CAPITAL LETTER EPSILON WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0397, 0, 0}, 0x1f2d }, // U1F2D # GREEK CAPITAL LETTER ETA WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0399, 0, 0}, 0x1f3d }, // U1F3D # GREEK CAPITAL LETTER IOTA WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Multi_key), 0x0028, 0x039f, 0, 0}, 0x1f4d }, // U1F4D # GREEK CAPITAL LETTER OMICRON WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Multi_key), 0x0028, 0x03a5, 0, 0}, 0x1f5d }, // U1F5D # GREEK CAPITAL LETTER UPSILON WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Multi_key), 0x0028, 0x03a9, 0, 0}, 0x1f6d }, // U1F6D # GREEK CAPITAL LETTER OMEGA WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Multi_key), 0x0028, 0x03b1, 0, 0}, 0x1f05 }, // U1F05 # GREEK SMALL LETTER ALPHA WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Multi_key), 0x0028, 0x03b5, 0, 0}, 0x1f15 }, // U1F15 # GREEK SMALL LETTER EPSILON WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Multi_key), 0x0028, 0x03b7, 0, 0}, 0x1f25 }, // U1F25 # GREEK SMALL LETTER ETA WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Multi_key), 0x0028, 0x03b9, 0, 0}, 0x1f35 }, // U1F35 # GREEK SMALL LETTER IOTA WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Multi_key), 0x0028, 0x03bf, 0, 0}, 0x1f45 }, // U1F45 # GREEK SMALL LETTER OMICRON WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Multi_key), 0x0028, 0x03c5, 0, 0}, 0x1f55 }, // U1F55 # GREEK SMALL LETTER UPSILON WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Multi_key), 0x0028, 0x03c9, 0, 0}, 0x1f65 }, // U1F65 # GREEK SMALL LETTER OMEGA WITH DASIA AND OXIA
    { {UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Multi_key), 0x0029, 0x0391, 0, 0}, 0x1f0c }, // U1F0C # GREEK CAPITAL LETTER ALPHA WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Multi_key), 0x0029, 0x0395, 0, 0}, 0x1f1c }, // U1F1C # GREEK CAPITAL LETTER EPSILON WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Multi_key), 0x0029, 0x0397, 0, 0}, 0x1f2c }, // U1F2C # GREEK CAPITAL LETTER ETA WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Multi_key), 0x0029, 0x0399, 0, 0}, 0x1f3c }, // U1F3C # GREEK CAPITAL LETTER IOTA WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Multi_key), 0x0029, 0x039f, 0, 0}, 0x1f4c }, // U1F4C # GREEK CAPITAL LETTER OMICRON WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Multi_key), 0x0029, 0x03a9, 0, 0}, 0x1f6c }, // U1F6C # GREEK CAPITAL LETTER OMEGA WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Multi_key), 0x0029, 0x03b1, 0, 0}, 0x1f04 }, // U1F04 # GREEK SMALL LETTER ALPHA WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Multi_key), 0x0029, 0x03b5, 0, 0}, 0x1f14 }, // U1F14 # GREEK SMALL LETTER EPSILON WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Multi_key), 0x0029, 0x03b7, 0, 0}, 0x1f24 }, // U1F24 # GREEK SMALL LETTER ETA WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Multi_key), 0x0029, 0x03b9, 0, 0}, 0x1f34 }, // U1F34 # GREEK SMALL LETTER IOTA WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Multi_key), 0x0029, 0x03bf, 0, 0}, 0x1f44 }, // U1F44 # GREEK SMALL LETTER OMICRON WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Multi_key), 0x0029, 0x03c5, 0, 0}, 0x1f54 }, // U1F54 # GREEK SMALL LETTER UPSILON WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Multi_key), 0x0029, 0x03c9, 0, 0}, 0x1f64 }, // U1F64 # GREEK SMALL LETTER OMEGA WITH PSILI AND OXIA
    { {UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Multi_key), 0x002b, 0x004f, 0, 0}, 0x1eda }, // U1EDA # LATIN CAPITAL LETTER O WITH HORN AND ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Multi_key), 0x002b, 0x0055, 0, 0}, 0x1ee8 }, // U1EE8 # LATIN CAPITAL LETTER U WITH HORN AND ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Multi_key), 0x002b, 0x006f, 0, 0}, 0x1edb }, // U1EDB # LATIN SMALL LETTER O WITH HORN AND ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Multi_key), 0x002b, 0x0075, 0, 0}, 0x1ee9 }, // U1EE9 # LATIN SMALL LETTER U WITH HORN AND ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Multi_key), 0x002c, 0x0043, 0, 0}, 0x1e08 }, // U1E08 # LATIN CAPITAL LETTER C WITH CEDILLA AND ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Multi_key), 0x002c, 0x0063, 0, 0}, 0x1e09 }, // U1E09 # LATIN SMALL LETTER C WITH CEDILLA AND ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Multi_key), 0x002f, 0x004f, 0, 0}, 0x01fe }, // U01FE # LATIN CAPITAL LETTER O WITH STROKE AND ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Multi_key), 0x002f, 0x006f, 0, 0}, 0x01ff }, // U01FF # LATIN SMALL LETTER O WITH STROKE AND ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Multi_key), 0x0055, 0x0041, 0, 0}, 0x1eae }, // U1EAE # LATIN CAPITAL LETTER A WITH BREVE AND ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Multi_key), 0x0055, 0x0061, 0, 0}, 0x1eaf }, // U1EAF # LATIN SMALL LETTER A WITH BREVE AND ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Multi_key), 0x005e, 0x0041, 0, 0}, 0x1ea4 }, // U1EA4 # LATIN CAPITAL LETTER A WITH CIRCUMFLEX AND ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Multi_key), 0x005e, 0x0045, 0, 0}, 0x1ebe }, // U1EBE # LATIN CAPITAL LETTER E WITH CIRCUMFLEX AND ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Multi_key), 0x005e, 0x004f, 0, 0}, 0x1ed0 }, // U1ED0 # LATIN CAPITAL LETTER O WITH CIRCUMFLEX AND ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Multi_key), 0x005e, 0x0061, 0, 0}, 0x1ea5 }, // U1EA5 # LATIN SMALL LETTER A WITH CIRCUMFLEX AND ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Multi_key), 0x005e, 0x0065, 0, 0}, 0x1ebf }, // U1EBF # LATIN SMALL LETTER E WITH CIRCUMFLEX AND ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Multi_key), 0x005e, 0x006f, 0, 0}, 0x1ed1 }, // U1ED1 # LATIN SMALL LETTER O WITH CIRCUMFLEX AND ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0045, 0, 0}, 0x1e16 }, // U1E16 # LATIN CAPITAL LETTER E WITH MACRON AND ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Multi_key), 0x005f, 0x004f, 0, 0}, 0x1e52 }, // U1E52 # LATIN CAPITAL LETTER O WITH MACRON AND ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0065, 0, 0}, 0x1e17 }, // U1E17 # LATIN SMALL LETTER E WITH MACRON AND ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Multi_key), 0x005f, 0x006f, 0, 0}, 0x1e53 }, // U1E53 # LATIN SMALL LETTER O WITH MACRON AND ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Multi_key), 0x0062, 0x0041, 0, 0}, 0x1eae }, // U1EAE # LATIN CAPITAL LETTER A WITH BREVE AND ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Multi_key), 0x0062, 0x0061, 0, 0}, 0x1eaf }, // U1EAF # LATIN SMALL LETTER A WITH BREVE AND ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Multi_key), 0x006f, 0x0041, 0, 0}, 0x01fa }, // U01FA # LATIN CAPITAL LETTER A WITH RING ABOVE AND ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Multi_key), 0x006f, 0x0061, 0, 0}, 0x01fb }, // U01FB # LATIN SMALL LETTER A WITH RING ABOVE AND ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Multi_key), 0x007e, 0x004f, 0, 0}, 0x1e4c }, // U1E4C # LATIN CAPITAL LETTER O WITH TILDE AND ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Multi_key), 0x007e, 0x0055, 0, 0}, 0x1e78 }, // U1E78 # LATIN CAPITAL LETTER U WITH TILDE AND ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Multi_key), 0x007e, 0x006f, 0, 0}, 0x1e4d }, // U1E4D # LATIN SMALL LETTER O WITH TILDE AND ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Multi_key), 0x007e, 0x0075, 0, 0}, 0x1e79 }, // U1E79 # LATIN SMALL LETTER U WITH TILDE AND ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Multi_key), 0x00af, 0x0045, 0, 0}, 0x1e16 }, // U1E16 # LATIN CAPITAL LETTER E WITH MACRON AND ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Multi_key), 0x00af, 0x004f, 0, 0}, 0x1e52 }, // U1E52 # LATIN CAPITAL LETTER O WITH MACRON AND ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Multi_key), 0x00af, 0x0065, 0, 0}, 0x1e17 }, // U1E17 # LATIN SMALL LETTER E WITH MACRON AND ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Multi_key), 0x00af, 0x006f, 0, 0}, 0x1e53 }, // U1E53 # LATIN SMALL LETTER O WITH MACRON AND ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Dead_Acute), 0, 0, 0, 0}, 0x00b4 }, // acute
    { {UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Dead_Circumflex), 0x0041, 0, 0, 0}, 0x1ea4 }, // U1EA4 # LATIN CAPITAL LETTER A WITH CIRCUMFLEX AND ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Dead_Circumflex), 0x0045, 0, 0, 0}, 0x1ebe }, // U1EBE # LATIN CAPITAL LETTER E WITH CIRCUMFLEX AND ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Dead_Circumflex), 0x004f, 0, 0, 0}, 0x1ed0 }, // U1ED0 # LATIN CAPITAL LETTER O WITH CIRCUMFLEX AND ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Dead_Circumflex), 0x0061, 0, 0, 0}, 0x1ea5 }, // U1EA5 # LATIN SMALL LETTER A WITH CIRCUMFLEX AND ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Dead_Circumflex), 0x0065, 0, 0, 0}, 0x1ebf }, // U1EBF # LATIN SMALL LETTER E WITH CIRCUMFLEX AND ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Dead_Circumflex), 0x006f, 0, 0, 0}, 0x1ed1 }, // U1ED1 # LATIN SMALL LETTER O WITH CIRCUMFLEX AND ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Dead_Tilde), 0x004f, 0, 0, 0}, 0x1e4c }, // U1E4C # LATIN CAPITAL LETTER O WITH TILDE AND ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Dead_Tilde), 0x0055, 0, 0, 0}, 0x1e78 }, // U1E78 # LATIN CAPITAL LETTER U WITH TILDE AND ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Dead_Tilde), 0x006f, 0, 0, 0}, 0x1e4d }, // U1E4D # LATIN SMALL LETTER O WITH TILDE AND ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Dead_Tilde), 0x0075, 0, 0, 0}, 0x1e79 }, // U1E79 # LATIN SMALL LETTER U WITH TILDE AND ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Dead_Macron), 0x0045, 0, 0, 0}, 0x1e16 }, // U1E16 # LATIN CAPITAL LETTER E WITH MACRON AND ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Dead_Macron), 0x004f, 0, 0, 0}, 0x1e52 }, // U1E52 # LATIN CAPITAL LETTER O WITH MACRON AND ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Dead_Macron), 0x0065, 0, 0, 0}, 0x1e17 }, // U1E17 # LATIN SMALL LETTER E WITH MACRON AND ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Dead_Macron), 0x006f, 0, 0, 0}, 0x1e53 }, // U1E53 # LATIN SMALL LETTER O WITH MACRON AND ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Dead_Breve), 0x0041, 0, 0, 0}, 0x1eae }, // U1EAE # LATIN CAPITAL LETTER A WITH BREVE AND ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Dead_Breve), 0x0061, 0, 0, 0}, 0x1eaf }, // U1EAF # LATIN SMALL LETTER A WITH BREVE AND ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Dead_Diaeresis), 0x0049, 0, 0, 0}, 0x1e2e }, // U1E2E # LATIN CAPITAL LETTER I WITH DIAERESIS AND ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Dead_Diaeresis), 0x0055, 0, 0, 0}, 0x01d7 }, // U01D7 # LATIN CAPITAL LETTER U WITH DIAERESIS AND ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Dead_Diaeresis), 0x0069, 0, 0, 0}, 0x1e2f }, // U1E2F # LATIN SMALL LETTER I WITH DIAERESIS AND ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Dead_Diaeresis), 0x0075, 0, 0, 0}, 0x01d8 }, // U01D8 # LATIN SMALL LETTER U WITH DIAERESIS AND ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Dead_Diaeresis), 0x03b9, 0, 0, 0}, 0x0390 }, // U0390 # GREEK SMALL LETTER IOTA WITH DIALYTIKA AND TONOS
    { {UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Dead_Diaeresis), 0x03c5, 0, 0, 0}, 0x03b0 }, // U03B0 # GREEK SMALL LETTER UPSILON WITH DIALYTIKA AND TONOS
    { {UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Dead_Abovering), 0x0041, 0, 0, 0}, 0x01fa }, // U01FA # LATIN CAPITAL LETTER A WITH RING ABOVE AND ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Dead_Abovering), 0x0061, 0, 0, 0}, 0x01fb }, // U01FB # LATIN SMALL LETTER A WITH RING ABOVE AND ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Dead_Cedilla), 0x0043, 0, 0, 0}, 0x1e08 }, // U1E08 # LATIN CAPITAL LETTER C WITH CEDILLA AND ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Dead_Cedilla), 0x0063, 0, 0, 0}, 0x1e09 }, // U1E09 # LATIN SMALL LETTER C WITH CEDILLA AND ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Dead_Horn), 0x004f, 0, 0, 0}, 0x1eda }, // U1EDA # LATIN CAPITAL LETTER O WITH HORN AND ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Dead_Horn), 0x0055, 0, 0, 0}, 0x1ee8 }, // U1EE8 # LATIN CAPITAL LETTER U WITH HORN AND ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Dead_Horn), 0x006f, 0, 0, 0}, 0x1edb }, // U1EDB # LATIN SMALL LETTER O WITH HORN AND ACUTE
    { {UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Dead_Horn), 0x0075, 0, 0, 0}, 0x1ee9 }, // U1EE9 # LATIN SMALL LETTER U WITH HORN AND ACUTE
// /* broken */    { {UNITIZE(TQt::Key_Dead_Circumflex), 0x0000, 0, 0, 0, 0}, 0x207a }, // U207A # SUPERSCRIPT PLUS SIGN
// /* broken */    { {UNITIZE(TQt::Key_Dead_Circumflex), 0x0000, 0, 0, 0, 0}, 0x207c }, // U207C # SUPERSCRIPT EQUALS SIGN
// /* broken */    { {UNITIZE(TQt::Key_Dead_Circumflex), 0x0000, 0, 0, 0, 0}, 0x00b2 }, // U00B2 # SUPERSCRIPT TWO
// /* broken */    { {UNITIZE(TQt::Key_Dead_Circumflex), 0x0000, 0, 0, 0, 0}, 0x2079 }, // U2079 # SUPERSCRIPT NINE
// /* broken */    { {UNITIZE(TQt::Key_Dead_Circumflex), 0x0000, 0, 0, 0, 0}, 0x00b3 }, // U00B3 # SUPERSCRIPT THREE
// /* broken */    { {UNITIZE(TQt::Key_Dead_Circumflex), 0x0000, 0, 0, 0, 0}, 0x00b9 }, // U00B9 # SUPERSCRIPT ONE
// /* broken */    { {UNITIZE(TQt::Key_Dead_Circumflex), 0x0000, 0, 0, 0, 0}, 0x2070 }, // U2070 # SUPERSCRIPT ZERO
// /* broken */    { {UNITIZE(TQt::Key_Dead_Circumflex), 0x0000, 0, 0, 0, 0}, 0x2078 }, // U2078 # SUPERSCRIPT EIGHT
// /* broken */    { {UNITIZE(TQt::Key_Dead_Circumflex), 0x0000, 0, 0, 0, 0}, 0x2074 }, // U2074 # SUPERSCRIPT FOUR
// /* broken */    { {UNITIZE(TQt::Key_Dead_Circumflex), 0x0000, 0, 0, 0, 0}, 0x2075 }, // U2075 # SUPERSCRIPT FIVE
// /* broken */    { {UNITIZE(TQt::Key_Dead_Circumflex), 0x0000, 0, 0, 0, 0}, 0x2077 }, // U2077 # SUPERSCRIPT SEVEN
// /* broken */    { {UNITIZE(TQt::Key_Dead_Circumflex), 0x0000, 0, 0, 0, 0}, 0x00b2 }, // U00B2 # SUPERSCRIPT TWO
// /* broken */    { {UNITIZE(TQt::Key_Dead_Circumflex), 0x0000, 0, 0, 0, 0}, 0x2076 }, // U2076 # SUPERSCRIPT SIX
// /* broken */    { {UNITIZE(TQt::Key_Dead_Circumflex), 0x0000, 0x0041, 0, 0, 0}, 0x1eac }, // U1EAC # LATIN CAPITAL LETTER A WITH CIRCUMFLEX AND DOT BELOW
// /* broken */    { {UNITIZE(TQt::Key_Dead_Circumflex), 0x0000, 0x0045, 0, 0, 0}, 0x1ec6 }, // U1EC6 # LATIN CAPITAL LETTER E WITH CIRCUMFLEX AND DOT BELOW
// /* broken */    { {UNITIZE(TQt::Key_Dead_Circumflex), 0x0000, 0x004f, 0, 0, 0}, 0x1ed8 }, // U1ED8 # LATIN CAPITAL LETTER O WITH CIRCUMFLEX AND DOT BELOW
// /* broken */    { {UNITIZE(TQt::Key_Dead_Circumflex), 0x0000, 0x0061, 0, 0, 0}, 0x1ead }, // U1EAD # LATIN SMALL LETTER A WITH CIRCUMFLEX AND DOT BELOW
// /* broken */    { {UNITIZE(TQt::Key_Dead_Circumflex), 0x0000, 0x0065, 0, 0, 0}, 0x1ec7 }, // U1EC7 # LATIN SMALL LETTER E WITH CIRCUMFLEX AND DOT BELOW
// /* broken */    { {UNITIZE(TQt::Key_Dead_Circumflex), 0x0000, 0x006f, 0, 0, 0}, 0x1ed9 }, // U1ED9 # LATIN SMALL LETTER O WITH CIRCUMFLEX AND DOT BELOW
    { {UNITIZE(TQt::Key_Dead_Circumflex), 0x0020, 0, 0, 0, 0}, 0x005e }, // asciicircum
    { {UNITIZE(TQt::Key_Dead_Circumflex), 0x0028, 0, 0, 0, 0}, 0x207d }, // U207D # SUPERSCRIPT LEFT PARENTHESIS
    { {UNITIZE(TQt::Key_Dead_Circumflex), 0x0029, 0, 0, 0, 0}, 0x207e }, // U207E # SUPERSCRIPT RIGHT PARENTHESIS
    { {UNITIZE(TQt::Key_Dead_Circumflex), 0x002b, 0, 0, 0, 0}, 0x207a }, // U207A # SUPERSCRIPT PLUS SIGN
    { {UNITIZE(TQt::Key_Dead_Circumflex), 0x0030, 0, 0, 0, 0}, 0x2070 }, // U2070 # SUPERSCRIPT ZERO
    { {UNITIZE(TQt::Key_Dead_Circumflex), 0x0031, 0, 0, 0, 0}, 0x00b9 }, // U00B9 # SUPERSCRIPT ONE
    { {UNITIZE(TQt::Key_Dead_Circumflex), 0x0032, 0, 0, 0, 0}, 0x00b2 }, // U00B2 # SUPERSCRIPT TWO
    { {UNITIZE(TQt::Key_Dead_Circumflex), 0x0033, 0, 0, 0, 0}, 0x00b3 }, // U00B3 # SUPERSCRIPT THREE
    { {UNITIZE(TQt::Key_Dead_Circumflex), 0x0034, 0, 0, 0, 0}, 0x2074 }, // U2074 # SUPERSCRIPT FOUR
    { {UNITIZE(TQt::Key_Dead_Circumflex), 0x0035, 0, 0, 0, 0}, 0x2075 }, // U2075 # SUPERSCRIPT FIVE
    { {UNITIZE(TQt::Key_Dead_Circumflex), 0x0036, 0, 0, 0, 0}, 0x2076 }, // U2076 # SUPERSCRIPT SIX
    { {UNITIZE(TQt::Key_Dead_Circumflex), 0x0037, 0, 0, 0, 0}, 0x2077 }, // U2077 # SUPERSCRIPT SEVEN
    { {UNITIZE(TQt::Key_Dead_Circumflex), 0x0038, 0, 0, 0, 0}, 0x2078 }, // U2078 # SUPERSCRIPT EIGHT
    { {UNITIZE(TQt::Key_Dead_Circumflex), 0x0039, 0, 0, 0, 0}, 0x2079 }, // U2079 # SUPERSCRIPT NINE
    { {UNITIZE(TQt::Key_Dead_Circumflex), 0x003d, 0, 0, 0, 0}, 0x207c }, // U207C # SUPERSCRIPT EQUALS SIGN
    { {UNITIZE(TQt::Key_Dead_Circumflex), 0x0041, 0, 0, 0, 0}, 0x00c2 }, // U00C2 # LATIN CAPITAL LETTER A WITH CIRCUMFLEX
    { {UNITIZE(TQt::Key_Dead_Circumflex), 0x0043, 0, 0, 0, 0}, 0x0108 }, // U0108 # LATIN CAPITAL LETTER C WITH CIRCUMFLEX
    { {UNITIZE(TQt::Key_Dead_Circumflex), 0x0045, 0, 0, 0, 0}, 0x00ca }, // U00CA # LATIN CAPITAL LETTER E WITH CIRCUMFLEX
    { {UNITIZE(TQt::Key_Dead_Circumflex), 0x0047, 0, 0, 0, 0}, 0x011c }, // U011C # LATIN CAPITAL LETTER G WITH CIRCUMFLEX
    { {UNITIZE(TQt::Key_Dead_Circumflex), 0x0048, 0, 0, 0, 0}, 0x0124 }, // U0124 # LATIN CAPITAL LETTER H WITH CIRCUMFLEX
    { {UNITIZE(TQt::Key_Dead_Circumflex), 0x0049, 0, 0, 0, 0}, 0x00ce }, // U00CE # LATIN CAPITAL LETTER I WITH CIRCUMFLEX
    { {UNITIZE(TQt::Key_Dead_Circumflex), 0x004a, 0, 0, 0, 0}, 0x0134 }, // U0134 # LATIN CAPITAL LETTER J WITH CIRCUMFLEX
    { {UNITIZE(TQt::Key_Dead_Circumflex), 0x004f, 0, 0, 0, 0}, 0x00d4 }, // U00D4 # LATIN CAPITAL LETTER O WITH CIRCUMFLEX
    { {UNITIZE(TQt::Key_Dead_Circumflex), 0x0053, 0, 0, 0, 0}, 0x015c }, // U015C # LATIN CAPITAL LETTER S WITH CIRCUMFLEX
    { {UNITIZE(TQt::Key_Dead_Circumflex), 0x0055, 0, 0, 0, 0}, 0x00db }, // U00DB # LATIN CAPITAL LETTER U WITH CIRCUMFLEX
    { {UNITIZE(TQt::Key_Dead_Circumflex), 0x0057, 0, 0, 0, 0}, 0x0174 }, // U0174 # LATIN CAPITAL LETTER W WITH CIRCUMFLEX
    { {UNITIZE(TQt::Key_Dead_Circumflex), 0x0059, 0, 0, 0, 0}, 0x0176 }, // U0176 # LATIN CAPITAL LETTER Y WITH CIRCUMFLEX
    { {UNITIZE(TQt::Key_Dead_Circumflex), 0x005a, 0, 0, 0, 0}, 0x1e90 }, // U1E90 # LATIN CAPITAL LETTER Z WITH CIRCUMFLEX
    { {UNITIZE(TQt::Key_Dead_Circumflex), 0x0061, 0, 0, 0, 0}, 0x00e2 }, // U00E2 # LATIN SMALL LETTER A WITH CIRCUMFLEX
    { {UNITIZE(TQt::Key_Dead_Circumflex), 0x0063, 0, 0, 0, 0}, 0x0109 }, // U0109 # LATIN SMALL LETTER C WITH CIRCUMFLEX
    { {UNITIZE(TQt::Key_Dead_Circumflex), 0x0065, 0, 0, 0, 0}, 0x00ea }, // U00EA # LATIN SMALL LETTER E WITH CIRCUMFLEX
    { {UNITIZE(TQt::Key_Dead_Circumflex), 0x0067, 0, 0, 0, 0}, 0x011d }, // U011D # LATIN SMALL LETTER G WITH CIRCUMFLEX
    { {UNITIZE(TQt::Key_Dead_Circumflex), 0x0068, 0, 0, 0, 0}, 0x0125 }, // U0125 # LATIN SMALL LETTER H WITH CIRCUMFLEX
    { {UNITIZE(TQt::Key_Dead_Circumflex), 0x0069, 0, 0, 0, 0}, 0x00ee }, // U00EE # LATIN SMALL LETTER I WITH CIRCUMFLEX
    { {UNITIZE(TQt::Key_Dead_Circumflex), 0x006a, 0, 0, 0, 0}, 0x0135 }, // U0135 # LATIN SMALL LETTER J WITH CIRCUMFLEX
    { {UNITIZE(TQt::Key_Dead_Circumflex), 0x006f, 0, 0, 0, 0}, 0x00f4 }, // U00F4 # LATIN SMALL LETTER O WITH CIRCUMFLEX
    { {UNITIZE(TQt::Key_Dead_Circumflex), 0x0073, 0, 0, 0, 0}, 0x015d }, // U015D # LATIN SMALL LETTER S WITH CIRCUMFLEX
    { {UNITIZE(TQt::Key_Dead_Circumflex), 0x0075, 0, 0, 0, 0}, 0x00fb }, // U00FB # LATIN SMALL LETTER U WITH CIRCUMFLEX
    { {UNITIZE(TQt::Key_Dead_Circumflex), 0x0077, 0, 0, 0, 0}, 0x0175 }, // U0175 # LATIN SMALL LETTER W WITH CIRCUMFLEX
    { {UNITIZE(TQt::Key_Dead_Circumflex), 0x0079, 0, 0, 0, 0}, 0x0177 }, // U0177 # LATIN SMALL LETTER Y WITH CIRCUMFLEX
    { {UNITIZE(TQt::Key_Dead_Circumflex), 0x007a, 0, 0, 0, 0}, 0x1e91 }, // U1E91 # LATIN SMALL LETTER Z WITH CIRCUMFLEX
    { {UNITIZE(TQt::Key_Dead_Circumflex), 0x1ea0, 0, 0, 0, 0}, 0x1eac }, // U1EAC # LATIN CAPITAL LETTER A WITH CIRCUMFLEX AND DOT BELOW
    { {UNITIZE(TQt::Key_Dead_Circumflex), 0x1ea1, 0, 0, 0, 0}, 0x1ead }, // U1EAD # LATIN SMALL LETTER A WITH CIRCUMFLEX AND DOT BELOW
    { {UNITIZE(TQt::Key_Dead_Circumflex), 0x1eb8, 0, 0, 0, 0}, 0x1ec6 }, // U1EC6 # LATIN CAPITAL LETTER E WITH CIRCUMFLEX AND DOT BELOW
    { {UNITIZE(TQt::Key_Dead_Circumflex), 0x1eb9, 0, 0, 0, 0}, 0x1ec7 }, // U1EC7 # LATIN SMALL LETTER E WITH CIRCUMFLEX AND DOT BELOW
    { {UNITIZE(TQt::Key_Dead_Circumflex), 0x1ecc, 0, 0, 0, 0}, 0x1ed8 }, // U1ED8 # LATIN CAPITAL LETTER O WITH CIRCUMFLEX AND DOT BELOW
    { {UNITIZE(TQt::Key_Dead_Circumflex), 0x1ecd, 0, 0, 0, 0}, 0x1ed9 }, // U1ED9 # LATIN SMALL LETTER O WITH CIRCUMFLEX AND DOT BELOW
    { {UNITIZE(TQt::Key_Dead_Circumflex), 0x2212, 0, 0, 0, 0}, 0x207b }, // U207B # SUPERSCRIPT MINUS
    { {UNITIZE(TQt::Key_Dead_Circumflex), 0x4e00, 0, 0, 0, 0}, 0x3192 }, // U3192 # IDEOGRAPHIC ANNOTATION ONE MARK
    { {UNITIZE(TQt::Key_Dead_Circumflex), 0x4e01, 0, 0, 0, 0}, 0x319c }, // U319C # IDEOGRAPHIC ANNOTATION FOURTH MARK
    { {UNITIZE(TQt::Key_Dead_Circumflex), 0x4e09, 0, 0, 0, 0}, 0x3194 }, // U3194 # IDEOGRAPHIC ANNOTATION THREE MARK
    { {UNITIZE(TQt::Key_Dead_Circumflex), 0x4e0a, 0, 0, 0, 0}, 0x3196 }, // U3196 # IDEOGRAPHIC ANNOTATION TOP MARK
    { {UNITIZE(TQt::Key_Dead_Circumflex), 0x4e0b, 0, 0, 0, 0}, 0x3198 }, // U3198 # IDEOGRAPHIC ANNOTATION BOTTOM MARK
    { {UNITIZE(TQt::Key_Dead_Circumflex), 0x4e19, 0, 0, 0, 0}, 0x319b }, // U319B # IDEOGRAPHIC ANNOTATION THIRD MARK
    { {UNITIZE(TQt::Key_Dead_Circumflex), 0x4e2d, 0, 0, 0, 0}, 0x3197 }, // U3197 # IDEOGRAPHIC ANNOTATION MIDDLE MARK
    { {UNITIZE(TQt::Key_Dead_Circumflex), 0x4e59, 0, 0, 0, 0}, 0x319a }, // U319A # IDEOGRAPHIC ANNOTATION SECOND MARK
    { {UNITIZE(TQt::Key_Dead_Circumflex), 0x4e8c, 0, 0, 0, 0}, 0x3193 }, // U3193 # IDEOGRAPHIC ANNOTATION TWO MARK
    { {UNITIZE(TQt::Key_Dead_Circumflex), 0x4eba, 0, 0, 0, 0}, 0x319f }, // U319F # IDEOGRAPHIC ANNOTATION MAN MARK
    { {UNITIZE(TQt::Key_Dead_Circumflex), 0x56db, 0, 0, 0, 0}, 0x3195 }, // U3195 # IDEOGRAPHIC ANNOTATION FOUR MARK
    { {UNITIZE(TQt::Key_Dead_Circumflex), 0x5730, 0, 0, 0, 0}, 0x319e }, // U319E # IDEOGRAPHIC ANNOTATION EARTH MARK
    { {UNITIZE(TQt::Key_Dead_Circumflex), 0x5929, 0, 0, 0, 0}, 0x319d }, // U319D # IDEOGRAPHIC ANNOTATION HEAVEN MARK
    { {UNITIZE(TQt::Key_Dead_Circumflex), 0x7532, 0, 0, 0, 0}, 0x3199 }, // U3199 # IDEOGRAPHIC ANNOTATION FIRST MARK
    { {UNITIZE(TQt::Key_Dead_Circumflex), UNITIZE(TQt::Key_Multi_key), 0x0021, 0x0041, 0, 0}, 0x1eac }, // U1EAC # LATIN CAPITAL LETTER A WITH CIRCUMFLEX AND DOT BELOW
    { {UNITIZE(TQt::Key_Dead_Circumflex), UNITIZE(TQt::Key_Multi_key), 0x0021, 0x0045, 0, 0}, 0x1ec6 }, // U1EC6 # LATIN CAPITAL LETTER E WITH CIRCUMFLEX AND DOT BELOW
    { {UNITIZE(TQt::Key_Dead_Circumflex), UNITIZE(TQt::Key_Multi_key), 0x0021, 0x004f, 0, 0}, 0x1ed8 }, // U1ED8 # LATIN CAPITAL LETTER O WITH CIRCUMFLEX AND DOT BELOW
    { {UNITIZE(TQt::Key_Dead_Circumflex), UNITIZE(TQt::Key_Multi_key), 0x0021, 0x0061, 0, 0}, 0x1ead }, // U1EAD # LATIN SMALL LETTER A WITH CIRCUMFLEX AND DOT BELOW
    { {UNITIZE(TQt::Key_Dead_Circumflex), UNITIZE(TQt::Key_Multi_key), 0x0021, 0x0065, 0, 0}, 0x1ec7 }, // U1EC7 # LATIN SMALL LETTER E WITH CIRCUMFLEX AND DOT BELOW
    { {UNITIZE(TQt::Key_Dead_Circumflex), UNITIZE(TQt::Key_Multi_key), 0x0021, 0x006f, 0, 0}, 0x1ed9 }, // U1ED9 # LATIN SMALL LETTER O WITH CIRCUMFLEX AND DOT BELOW
    { {UNITIZE(TQt::Key_Dead_Circumflex), UNITIZE(TQt::Key_Multi_key), 0x0053, 0x004d, 0, 0}, 0x2120 }, // U2120 # SERVICE MARK
    { {UNITIZE(TQt::Key_Dead_Circumflex), UNITIZE(TQt::Key_Multi_key), 0x0054, 0x004d, 0, 0}, 0x2122 }, // U2122 # TRADE MARK SIGN
    { {UNITIZE(TQt::Key_Dead_Circumflex), UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0061, 0, 0}, 0x00aa }, // U00AA # FEMININE ORDINAL INDICATOR
    { {UNITIZE(TQt::Key_Dead_Circumflex), UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0061, 0, 0}, 0x00aa }, // U00AA # FEMININE ORDINAL INDICATOR
    { {UNITIZE(TQt::Key_Dead_Circumflex), UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0068, 0, 0}, 0x02b0 }, // U02B0 # MODIFIER LETTER SMALL H
    { {UNITIZE(TQt::Key_Dead_Circumflex), UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0068, 0, 0}, 0x02b0 }, // U02B0 # MODIFIER LETTER SMALL H
    { {UNITIZE(TQt::Key_Dead_Circumflex), UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0069, 0, 0}, 0x2071 }, // U2071 # SUPERSCRIPT LATIN SMALL LETTER I
    { {UNITIZE(TQt::Key_Dead_Circumflex), UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0069, 0, 0}, 0x2071 }, // U2071 # SUPERSCRIPT LATIN SMALL LETTER I
    { {UNITIZE(TQt::Key_Dead_Circumflex), UNITIZE(TQt::Key_Multi_key), 0x005f, 0x006a, 0, 0}, 0x02b2 }, // U02B2 # MODIFIER LETTER SMALL J
    { {UNITIZE(TQt::Key_Dead_Circumflex), UNITIZE(TQt::Key_Multi_key), 0x005f, 0x006a, 0, 0}, 0x02b2 }, // U02B2 # MODIFIER LETTER SMALL J
    { {UNITIZE(TQt::Key_Dead_Circumflex), UNITIZE(TQt::Key_Multi_key), 0x005f, 0x006c, 0, 0}, 0x02e1 }, // U02E1 # MODIFIER LETTER SMALL L
    { {UNITIZE(TQt::Key_Dead_Circumflex), UNITIZE(TQt::Key_Multi_key), 0x005f, 0x006c, 0, 0}, 0x02e1 }, // U02E1 # MODIFIER LETTER SMALL L
    { {UNITIZE(TQt::Key_Dead_Circumflex), UNITIZE(TQt::Key_Multi_key), 0x005f, 0x006e, 0, 0}, 0x207f }, // U207F # SUPERSCRIPT LATIN SMALL LETTER N
    { {UNITIZE(TQt::Key_Dead_Circumflex), UNITIZE(TQt::Key_Multi_key), 0x005f, 0x006e, 0, 0}, 0x207f }, // U207F # SUPERSCRIPT LATIN SMALL LETTER N
    { {UNITIZE(TQt::Key_Dead_Circumflex), UNITIZE(TQt::Key_Multi_key), 0x005f, 0x006f, 0, 0}, 0x00ba }, // U00BA # MASCULINE ORDINAL INDICATOR
    { {UNITIZE(TQt::Key_Dead_Circumflex), UNITIZE(TQt::Key_Multi_key), 0x005f, 0x006f, 0, 0}, 0x00ba }, // U00BA # MASCULINE ORDINAL INDICATOR
    { {UNITIZE(TQt::Key_Dead_Circumflex), UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0072, 0, 0}, 0x02b3 }, // U02B3 # MODIFIER LETTER SMALL R
    { {UNITIZE(TQt::Key_Dead_Circumflex), UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0072, 0, 0}, 0x02b3 }, // U02B3 # MODIFIER LETTER SMALL R
    { {UNITIZE(TQt::Key_Dead_Circumflex), UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0073, 0, 0}, 0x02e2 }, // U02E2 # MODIFIER LETTER SMALL S
    { {UNITIZE(TQt::Key_Dead_Circumflex), UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0073, 0, 0}, 0x02e2 }, // U02E2 # MODIFIER LETTER SMALL S
    { {UNITIZE(TQt::Key_Dead_Circumflex), UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0077, 0, 0}, 0x02b7 }, // U02B7 # MODIFIER LETTER SMALL W
    { {UNITIZE(TQt::Key_Dead_Circumflex), UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0077, 0, 0}, 0x02b7 }, // U02B7 # MODIFIER LETTER SMALL W
    { {UNITIZE(TQt::Key_Dead_Circumflex), UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0078, 0, 0}, 0x02e3 }, // U02E3 # MODIFIER LETTER SMALL X
    { {UNITIZE(TQt::Key_Dead_Circumflex), UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0078, 0, 0}, 0x02e3 }, // U02E3 # MODIFIER LETTER SMALL X
    { {UNITIZE(TQt::Key_Dead_Circumflex), UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0079, 0, 0}, 0x02b8 }, // U02B8 # MODIFIER LETTER SMALL Y
    { {UNITIZE(TQt::Key_Dead_Circumflex), UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0079, 0, 0}, 0x02b8 }, // U02B8 # MODIFIER LETTER SMALL Y
    { {UNITIZE(TQt::Key_Dead_Circumflex), UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0263, 0, 0}, 0x02e0 }, // U02E0 # MODIFIER LETTER SMALL GAMMA
    { {UNITIZE(TQt::Key_Dead_Circumflex), UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0263, 0, 0}, 0x02e0 }, // U02E0 # MODIFIER LETTER SMALL GAMMA
    { {UNITIZE(TQt::Key_Dead_Circumflex), UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0266, 0, 0}, 0x02b1 }, // U02B1 # MODIFIER LETTER SMALL H WITH HOOK
    { {UNITIZE(TQt::Key_Dead_Circumflex), UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0266, 0, 0}, 0x02b1 }, // U02B1 # MODIFIER LETTER SMALL H WITH HOOK
    { {UNITIZE(TQt::Key_Dead_Circumflex), UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0279, 0, 0}, 0x02b4 }, // U02B4 # MODIFIER LETTER SMALL TURNED R
    { {UNITIZE(TQt::Key_Dead_Circumflex), UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0279, 0, 0}, 0x02b4 }, // U02B4 # MODIFIER LETTER SMALL TURNED R
    { {UNITIZE(TQt::Key_Dead_Circumflex), UNITIZE(TQt::Key_Multi_key), 0x005f, 0x027b, 0, 0}, 0x02b5 }, // U02B5 # MODIFIER LETTER SMALL TURNED R WITH HOOK
    { {UNITIZE(TQt::Key_Dead_Circumflex), UNITIZE(TQt::Key_Multi_key), 0x005f, 0x027b, 0, 0}, 0x02b5 }, // U02B5 # MODIFIER LETTER SMALL TURNED R WITH HOOK
    { {UNITIZE(TQt::Key_Dead_Circumflex), UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0281, 0, 0}, 0x02b6 }, // U02B6 # MODIFIER LETTER SMALL CAPITAL INVERTED R
    { {UNITIZE(TQt::Key_Dead_Circumflex), UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0281, 0, 0}, 0x02b6 }, // U02B6 # MODIFIER LETTER SMALL CAPITAL INVERTED R
    { {UNITIZE(TQt::Key_Dead_Circumflex), UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0295, 0, 0}, 0x02e4 }, // U02E4 # MODIFIER LETTER SMALL REVERSED GLOTTAL STOP
    { {UNITIZE(TQt::Key_Dead_Circumflex), UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0295, 0, 0}, 0x02e4 }, // U02E4 # MODIFIER LETTER SMALL REVERSED GLOTTAL STOP
    { {UNITIZE(TQt::Key_Dead_Circumflex), UNITIZE(TQt::Key_Dead_Circumflex), 0, 0, 0, 0}, 0x005e }, // asciicircum
    { {UNITIZE(TQt::Key_Dead_Circumflex), UNITIZE(TQt::Key_Dead_Belowdot), 0x0041, 0, 0, 0}, 0x1eac }, // U1EAC # LATIN CAPITAL LETTER A WITH CIRCUMFLEX AND DOT BELOW
    { {UNITIZE(TQt::Key_Dead_Circumflex), UNITIZE(TQt::Key_Dead_Belowdot), 0x0045, 0, 0, 0}, 0x1ec6 }, // U1EC6 # LATIN CAPITAL LETTER E WITH CIRCUMFLEX AND DOT BELOW
    { {UNITIZE(TQt::Key_Dead_Circumflex), UNITIZE(TQt::Key_Dead_Belowdot), 0x004f, 0, 0, 0}, 0x1ed8 }, // U1ED8 # LATIN CAPITAL LETTER O WITH CIRCUMFLEX AND DOT BELOW
    { {UNITIZE(TQt::Key_Dead_Circumflex), UNITIZE(TQt::Key_Dead_Belowdot), 0x0061, 0, 0, 0}, 0x1ead }, // U1EAD # LATIN SMALL LETTER A WITH CIRCUMFLEX AND DOT BELOW
    { {UNITIZE(TQt::Key_Dead_Circumflex), UNITIZE(TQt::Key_Dead_Belowdot), 0x0065, 0, 0, 0}, 0x1ec7 }, // U1EC7 # LATIN SMALL LETTER E WITH CIRCUMFLEX AND DOT BELOW
    { {UNITIZE(TQt::Key_Dead_Circumflex), UNITIZE(TQt::Key_Dead_Belowdot), 0x006f, 0, 0, 0}, 0x1ed9 }, // U1ED9 # LATIN SMALL LETTER O WITH CIRCUMFLEX AND DOT BELOW
// /* broken */    { {UNITIZE(TQt::Key_Dead_Tilde), 0x0000, 0, 0, 0, 0}, 0x1ee0 }, // U1EE0 # LATIN CAPITAL LETTER O WITH HORN AND TILDE
// /* broken */    { {UNITIZE(TQt::Key_Dead_Tilde), 0x0000, 0, 0, 0, 0}, 0x1eee }, // U1EEE # LATIN CAPITAL LETTER U WITH HORN AND TILDE
// /* broken */    { {UNITIZE(TQt::Key_Dead_Tilde), 0x0000, 0, 0, 0, 0}, 0x1eef }, // U1EEF # LATIN SMALL LETTER U WITH HORN AND TILDE
// /* broken */    { {UNITIZE(TQt::Key_Dead_Tilde), 0x0000, 0, 0, 0, 0}, 0x1ee1 }, // U1EE1 # LATIN SMALL LETTER O WITH HORN AND TILDE
    { {UNITIZE(TQt::Key_Dead_Tilde), 0x0020, 0, 0, 0, 0}, 0x007e }, // asciitilde
    { {UNITIZE(TQt::Key_Dead_Tilde), 0x0041, 0, 0, 0, 0}, 0x00c3 }, // U00C3 # LATIN CAPITAL LETTER A WITH TILDE
    { {UNITIZE(TQt::Key_Dead_Tilde), 0x0045, 0, 0, 0, 0}, 0x1ebc }, // U1EBC # LATIN CAPITAL LETTER E WITH TILDE
    { {UNITIZE(TQt::Key_Dead_Tilde), 0x0049, 0, 0, 0, 0}, 0x0128 }, // U0128 # LATIN CAPITAL LETTER I WITH TILDE
    { {UNITIZE(TQt::Key_Dead_Tilde), 0x004e, 0, 0, 0, 0}, 0x00d1 }, // U00D1 # LATIN CAPITAL LETTER N WITH TILDE
    { {UNITIZE(TQt::Key_Dead_Tilde), 0x004f, 0, 0, 0, 0}, 0x00d5 }, // U00D5 # LATIN CAPITAL LETTER O WITH TILDE
    { {UNITIZE(TQt::Key_Dead_Tilde), 0x0055, 0, 0, 0, 0}, 0x0168 }, // U0168 # LATIN CAPITAL LETTER U WITH TILDE
    { {UNITIZE(TQt::Key_Dead_Tilde), 0x0056, 0, 0, 0, 0}, 0x1e7c }, // U1E7C # LATIN CAPITAL LETTER V WITH TILDE
    { {UNITIZE(TQt::Key_Dead_Tilde), 0x0059, 0, 0, 0, 0}, 0x1ef8 }, // U1EF8 # LATIN CAPITAL LETTER Y WITH TILDE
    { {UNITIZE(TQt::Key_Dead_Tilde), 0x0061, 0, 0, 0, 0}, 0x00e3 }, // U00E3 # LATIN SMALL LETTER A WITH TILDE
    { {UNITIZE(TQt::Key_Dead_Tilde), 0x0065, 0, 0, 0, 0}, 0x1ebd }, // U1EBD # LATIN SMALL LETTER E WITH TILDE
    { {UNITIZE(TQt::Key_Dead_Tilde), 0x0069, 0, 0, 0, 0}, 0x0129 }, // U0129 # LATIN SMALL LETTER I WITH TILDE
    { {UNITIZE(TQt::Key_Dead_Tilde), 0x006e, 0, 0, 0, 0}, 0x00f1 }, // U00F1 # LATIN SMALL LETTER N WITH TILDE
    { {UNITIZE(TQt::Key_Dead_Tilde), 0x006f, 0, 0, 0, 0}, 0x00f5 }, // U00F5 # LATIN SMALL LETTER O WITH TILDE
    { {UNITIZE(TQt::Key_Dead_Tilde), 0x0075, 0, 0, 0, 0}, 0x0169 }, // U0169 # LATIN SMALL LETTER U WITH TILDE
    { {UNITIZE(TQt::Key_Dead_Tilde), 0x0076, 0, 0, 0, 0}, 0x1e7d }, // U1E7D # LATIN SMALL LETTER V WITH TILDE
    { {UNITIZE(TQt::Key_Dead_Tilde), 0x0079, 0, 0, 0, 0}, 0x1ef9 }, // U1EF9 # LATIN SMALL LETTER Y WITH TILDE
    { {UNITIZE(TQt::Key_Dead_Tilde), 0x00c2, 0, 0, 0, 0}, 0x1eaa }, // U1EAA # LATIN CAPITAL LETTER A WITH CIRCUMFLEX AND TILDE
    { {UNITIZE(TQt::Key_Dead_Tilde), 0x00ca, 0, 0, 0, 0}, 0x1ec4 }, // U1EC4 # LATIN CAPITAL LETTER E WITH CIRCUMFLEX AND TILDE
    { {UNITIZE(TQt::Key_Dead_Tilde), 0x00d4, 0, 0, 0, 0}, 0x1ed6 }, // U1ED6 # LATIN CAPITAL LETTER O WITH CIRCUMFLEX AND TILDE
    { {UNITIZE(TQt::Key_Dead_Tilde), 0x00e2, 0, 0, 0, 0}, 0x1eab }, // U1EAB # LATIN SMALL LETTER A WITH CIRCUMFLEX AND TILDE
    { {UNITIZE(TQt::Key_Dead_Tilde), 0x00ea, 0, 0, 0, 0}, 0x1ec5 }, // U1EC5 # LATIN SMALL LETTER E WITH CIRCUMFLEX AND TILDE
    { {UNITIZE(TQt::Key_Dead_Tilde), 0x00f4, 0, 0, 0, 0}, 0x1ed7 }, // U1ED7 # LATIN SMALL LETTER O WITH CIRCUMFLEX AND TILDE
    { {UNITIZE(TQt::Key_Dead_Tilde), 0x0102, 0, 0, 0, 0}, 0x1eb4 }, // U1EB4 # LATIN CAPITAL LETTER A WITH BREVE AND TILDE
    { {UNITIZE(TQt::Key_Dead_Tilde), 0x0103, 0, 0, 0, 0}, 0x1eb5 }, // U1EB5 # LATIN SMALL LETTER A WITH BREVE AND TILDE
    { {UNITIZE(TQt::Key_Dead_Tilde), 0x01A0, 0, 0, 0, 0}, 0x1ee0 }, // U1EE0 # LATIN CAPITAL LETTER O WITH HORN AND TILDE
    { {UNITIZE(TQt::Key_Dead_Tilde), 0x01A1, 0, 0, 0, 0}, 0x1ee1 }, // U1EE1 # LATIN SMALL LETTER O WITH HORN AND TILDE
    { {UNITIZE(TQt::Key_Dead_Tilde), 0x01AF, 0, 0, 0, 0}, 0x1eee }, // U1EEE # LATIN CAPITAL LETTER U WITH HORN AND TILDE
    { {UNITIZE(TQt::Key_Dead_Tilde), 0x01B0, 0, 0, 0, 0}, 0x1eef }, // U1EEF # LATIN SMALL LETTER U WITH HORN AND TILDE
    { {UNITIZE(TQt::Key_Dead_Tilde), 0x0313, 0x0391, 0, 0, 0}, 0x1f0e }, // U1F0E # GREEK CAPITAL LETTER ALPHA WITH PSILI AND PERISPOMENI
    { {UNITIZE(TQt::Key_Dead_Tilde), 0x0313, 0x0397, 0, 0, 0}, 0x1f2e }, // U1F2E # GREEK CAPITAL LETTER ETA WITH PSILI AND PERISPOMENI
    { {UNITIZE(TQt::Key_Dead_Tilde), 0x0313, 0x0399, 0, 0, 0}, 0x1f3e }, // U1F3E # GREEK CAPITAL LETTER IOTA WITH PSILI AND PERISPOMENI
    { {UNITIZE(TQt::Key_Dead_Tilde), 0x0313, 0x03a9, 0, 0, 0}, 0x1f6e }, // U1F6E # GREEK CAPITAL LETTER OMEGA WITH PSILI AND PERISPOMENI
    { {UNITIZE(TQt::Key_Dead_Tilde), 0x0313, 0x03b1, 0, 0, 0}, 0x1f06 }, // U1F06 # GREEK SMALL LETTER ALPHA WITH PSILI AND PERISPOMENI
    { {UNITIZE(TQt::Key_Dead_Tilde), 0x0313, 0x03b7, 0, 0, 0}, 0x1f26 }, // U1F26 # GREEK SMALL LETTER ETA WITH PSILI AND PERISPOMENI
    { {UNITIZE(TQt::Key_Dead_Tilde), 0x0313, 0x03b9, 0, 0, 0}, 0x1f36 }, // U1F36 # GREEK SMALL LETTER IOTA WITH PSILI AND PERISPOMENI
    { {UNITIZE(TQt::Key_Dead_Tilde), 0x0313, 0x03c5, 0, 0, 0}, 0x1f56 }, // U1F56 # GREEK SMALL LETTER UPSILON WITH PSILI AND PERISPOMENI
    { {UNITIZE(TQt::Key_Dead_Tilde), 0x0313, 0x03c9, 0, 0, 0}, 0x1f66 }, // U1F66 # GREEK SMALL LETTER OMEGA WITH PSILI AND PERISPOMENI
    { {UNITIZE(TQt::Key_Dead_Tilde), 0x0314, 0x0391, 0, 0, 0}, 0x1f0f }, // U1F0F # GREEK CAPITAL LETTER ALPHA WITH DASIA AND PERISPOMENI
    { {UNITIZE(TQt::Key_Dead_Tilde), 0x0314, 0x0397, 0, 0, 0}, 0x1f2f }, // U1F2F # GREEK CAPITAL LETTER ETA WITH DASIA AND PERISPOMENI
    { {UNITIZE(TQt::Key_Dead_Tilde), 0x0314, 0x0399, 0, 0, 0}, 0x1f3f }, // U1F3F # GREEK CAPITAL LETTER IOTA WITH DASIA AND PERISPOMENI
    { {UNITIZE(TQt::Key_Dead_Tilde), 0x0314, 0x03a5, 0, 0, 0}, 0x1f5f }, // U1F5F # GREEK CAPITAL LETTER UPSILON WITH DASIA AND PERISPOMENI
    { {UNITIZE(TQt::Key_Dead_Tilde), 0x0314, 0x03a9, 0, 0, 0}, 0x1f6f }, // U1F6F # GREEK CAPITAL LETTER OMEGA WITH DASIA AND PERISPOMENI
    { {UNITIZE(TQt::Key_Dead_Tilde), 0x0314, 0x03b1, 0, 0, 0}, 0x1f07 }, // U1F07 # GREEK SMALL LETTER ALPHA WITH DASIA AND PERISPOMENI
    { {UNITIZE(TQt::Key_Dead_Tilde), 0x0314, 0x03b7, 0, 0, 0}, 0x1f27 }, // U1F27 # GREEK SMALL LETTER ETA WITH DASIA AND PERISPOMENI
    { {UNITIZE(TQt::Key_Dead_Tilde), 0x0314, 0x03b9, 0, 0, 0}, 0x1f37 }, // U1F37 # GREEK SMALL LETTER IOTA WITH DASIA AND PERISPOMENI
    { {UNITIZE(TQt::Key_Dead_Tilde), 0x0314, 0x03c5, 0, 0, 0}, 0x1f57 }, // U1F57 # GREEK SMALL LETTER UPSILON WITH DASIA AND PERISPOMENI
    { {UNITIZE(TQt::Key_Dead_Tilde), 0x0314, 0x03c9, 0, 0, 0}, 0x1f67 }, // U1F67 # GREEK SMALL LETTER OMEGA WITH DASIA AND PERISPOMENI
    { {UNITIZE(TQt::Key_Dead_Tilde), 0x03b1, 0, 0, 0, 0}, 0x1fb6 }, // U1FB6 # GREEK SMALL LETTER ALPHA WITH PERISPOMENI
    { {UNITIZE(TQt::Key_Dead_Tilde), 0x03b7, 0, 0, 0, 0}, 0x1fc6 }, // U1FC6 # GREEK SMALL LETTER ETA WITH PERISPOMENI
    { {UNITIZE(TQt::Key_Dead_Tilde), 0x03b9, 0, 0, 0, 0}, 0x1fd6 }, // U1FD6 # GREEK SMALL LETTER IOTA WITH PERISPOMENI
    { {UNITIZE(TQt::Key_Dead_Tilde), 0x03c5, 0, 0, 0, 0}, 0x1fe6 }, // U1FE6 # GREEK SMALL LETTER UPSILON WITH PERISPOMENI
    { {UNITIZE(TQt::Key_Dead_Tilde), 0x03c9, 0, 0, 0, 0}, 0x1ff6 }, // U1FF6 # GREEK SMALL LETTER OMEGA WITH PERISPOMENI
    { {UNITIZE(TQt::Key_Dead_Tilde), 0x03ca, 0, 0, 0, 0}, 0x1fd7 }, // U1FD7 # GREEK SMALL LETTER IOTA WITH DIALYTIKA AND PERISPOMENI
    { {UNITIZE(TQt::Key_Dead_Tilde), 0x03cb, 0, 0, 0, 0}, 0x1fe7 }, // U1FE7 # GREEK SMALL LETTER UPSILON WITH DIALYTIKA AND PERISPOMENI
    { {UNITIZE(TQt::Key_Dead_Tilde), 0x1f00, 0, 0, 0, 0}, 0x1f06 }, // U1F06 # GREEK SMALL LETTER ALPHA WITH PSILI AND PERISPOMENI
    { {UNITIZE(TQt::Key_Dead_Tilde), 0x1f01, 0, 0, 0, 0}, 0x1f07 }, // U1F07 # GREEK SMALL LETTER ALPHA WITH DASIA AND PERISPOMENI
    { {UNITIZE(TQt::Key_Dead_Tilde), 0x1f08, 0, 0, 0, 0}, 0x1f0e }, // U1F0E # GREEK CAPITAL LETTER ALPHA WITH PSILI AND PERISPOMENI
    { {UNITIZE(TQt::Key_Dead_Tilde), 0x1f09, 0, 0, 0, 0}, 0x1f0f }, // U1F0F # GREEK CAPITAL LETTER ALPHA WITH DASIA AND PERISPOMENI
    { {UNITIZE(TQt::Key_Dead_Tilde), 0x1f20, 0, 0, 0, 0}, 0x1f26 }, // U1F26 # GREEK SMALL LETTER ETA WITH PSILI AND PERISPOMENI
    { {UNITIZE(TQt::Key_Dead_Tilde), 0x1f21, 0, 0, 0, 0}, 0x1f27 }, // U1F27 # GREEK SMALL LETTER ETA WITH DASIA AND PERISPOMENI
    { {UNITIZE(TQt::Key_Dead_Tilde), 0x1f28, 0, 0, 0, 0}, 0x1f2e }, // U1F2E # GREEK CAPITAL LETTER ETA WITH PSILI AND PERISPOMENI
    { {UNITIZE(TQt::Key_Dead_Tilde), 0x1f29, 0, 0, 0, 0}, 0x1f2f }, // U1F2F # GREEK CAPITAL LETTER ETA WITH DASIA AND PERISPOMENI
    { {UNITIZE(TQt::Key_Dead_Tilde), 0x1f30, 0, 0, 0, 0}, 0x1f36 }, // U1F36 # GREEK SMALL LETTER IOTA WITH PSILI AND PERISPOMENI
    { {UNITIZE(TQt::Key_Dead_Tilde), 0x1f31, 0, 0, 0, 0}, 0x1f37 }, // U1F37 # GREEK SMALL LETTER IOTA WITH DASIA AND PERISPOMENI
    { {UNITIZE(TQt::Key_Dead_Tilde), 0x1f38, 0, 0, 0, 0}, 0x1f3e }, // U1F3E # GREEK CAPITAL LETTER IOTA WITH PSILI AND PERISPOMENI
    { {UNITIZE(TQt::Key_Dead_Tilde), 0x1f39, 0, 0, 0, 0}, 0x1f3f }, // U1F3F # GREEK CAPITAL LETTER IOTA WITH DASIA AND PERISPOMENI
    { {UNITIZE(TQt::Key_Dead_Tilde), 0x1f50, 0, 0, 0, 0}, 0x1f56 }, // U1F56 # GREEK SMALL LETTER UPSILON WITH PSILI AND PERISPOMENI
    { {UNITIZE(TQt::Key_Dead_Tilde), 0x1f51, 0, 0, 0, 0}, 0x1f57 }, // U1F57 # GREEK SMALL LETTER UPSILON WITH DASIA AND PERISPOMENI
    { {UNITIZE(TQt::Key_Dead_Tilde), 0x1f59, 0, 0, 0, 0}, 0x1f5f }, // U1F5F # GREEK CAPITAL LETTER UPSILON WITH DASIA AND PERISPOMENI
    { {UNITIZE(TQt::Key_Dead_Tilde), 0x1f60, 0, 0, 0, 0}, 0x1f66 }, // U1F66 # GREEK SMALL LETTER OMEGA WITH PSILI AND PERISPOMENI
    { {UNITIZE(TQt::Key_Dead_Tilde), 0x1f61, 0, 0, 0, 0}, 0x1f67 }, // U1F67 # GREEK SMALL LETTER OMEGA WITH DASIA AND PERISPOMENI
    { {UNITIZE(TQt::Key_Dead_Tilde), 0x1f68, 0, 0, 0, 0}, 0x1f6e }, // U1F6E # GREEK CAPITAL LETTER OMEGA WITH PSILI AND PERISPOMENI
    { {UNITIZE(TQt::Key_Dead_Tilde), 0x1f69, 0, 0, 0, 0}, 0x1f6f }, // U1F6F # GREEK CAPITAL LETTER OMEGA WITH DASIA AND PERISPOMENI
    { {UNITIZE(TQt::Key_Dead_Tilde), UNITIZE(TQt::Key_Multi_key), 0x0022, 0x03b9, 0, 0}, 0x1fd7 }, // U1FD7 # GREEK SMALL LETTER IOTA WITH DIALYTIKA AND PERISPOMENI
    { {UNITIZE(TQt::Key_Dead_Tilde), UNITIZE(TQt::Key_Multi_key), 0x0022, 0x03c5, 0, 0}, 0x1fe7 }, // U1FE7 # GREEK SMALL LETTER UPSILON WITH DIALYTIKA AND PERISPOMENI
    { {UNITIZE(TQt::Key_Dead_Tilde), UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0391, 0, 0}, 0x1f0f }, // U1F0F # GREEK CAPITAL LETTER ALPHA WITH DASIA AND PERISPOMENI
    { {UNITIZE(TQt::Key_Dead_Tilde), UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0397, 0, 0}, 0x1f2f }, // U1F2F # GREEK CAPITAL LETTER ETA WITH DASIA AND PERISPOMENI
    { {UNITIZE(TQt::Key_Dead_Tilde), UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0399, 0, 0}, 0x1f3f }, // U1F3F # GREEK CAPITAL LETTER IOTA WITH DASIA AND PERISPOMENI
    { {UNITIZE(TQt::Key_Dead_Tilde), UNITIZE(TQt::Key_Multi_key), 0x0028, 0x03a5, 0, 0}, 0x1f5f }, // U1F5F # GREEK CAPITAL LETTER UPSILON WITH DASIA AND PERISPOMENI
    { {UNITIZE(TQt::Key_Dead_Tilde), UNITIZE(TQt::Key_Multi_key), 0x0028, 0x03a9, 0, 0}, 0x1f6f }, // U1F6F # GREEK CAPITAL LETTER OMEGA WITH DASIA AND PERISPOMENI
    { {UNITIZE(TQt::Key_Dead_Tilde), UNITIZE(TQt::Key_Multi_key), 0x0028, 0x03b1, 0, 0}, 0x1f07 }, // U1F07 # GREEK SMALL LETTER ALPHA WITH DASIA AND PERISPOMENI
    { {UNITIZE(TQt::Key_Dead_Tilde), UNITIZE(TQt::Key_Multi_key), 0x0028, 0x03b7, 0, 0}, 0x1f27 }, // U1F27 # GREEK SMALL LETTER ETA WITH DASIA AND PERISPOMENI
    { {UNITIZE(TQt::Key_Dead_Tilde), UNITIZE(TQt::Key_Multi_key), 0x0028, 0x03b9, 0, 0}, 0x1f37 }, // U1F37 # GREEK SMALL LETTER IOTA WITH DASIA AND PERISPOMENI
    { {UNITIZE(TQt::Key_Dead_Tilde), UNITIZE(TQt::Key_Multi_key), 0x0028, 0x03c5, 0, 0}, 0x1f57 }, // U1F57 # GREEK SMALL LETTER UPSILON WITH DASIA AND PERISPOMENI
    { {UNITIZE(TQt::Key_Dead_Tilde), UNITIZE(TQt::Key_Multi_key), 0x0028, 0x03c9, 0, 0}, 0x1f67 }, // U1F67 # GREEK SMALL LETTER OMEGA WITH DASIA AND PERISPOMENI
    { {UNITIZE(TQt::Key_Dead_Tilde), UNITIZE(TQt::Key_Multi_key), 0x0029, 0x0391, 0, 0}, 0x1f0e }, // U1F0E # GREEK CAPITAL LETTER ALPHA WITH PSILI AND PERISPOMENI
    { {UNITIZE(TQt::Key_Dead_Tilde), UNITIZE(TQt::Key_Multi_key), 0x0029, 0x0397, 0, 0}, 0x1f2e }, // U1F2E # GREEK CAPITAL LETTER ETA WITH PSILI AND PERISPOMENI
    { {UNITIZE(TQt::Key_Dead_Tilde), UNITIZE(TQt::Key_Multi_key), 0x0029, 0x0399, 0, 0}, 0x1f3e }, // U1F3E # GREEK CAPITAL LETTER IOTA WITH PSILI AND PERISPOMENI
    { {UNITIZE(TQt::Key_Dead_Tilde), UNITIZE(TQt::Key_Multi_key), 0x0029, 0x03a9, 0, 0}, 0x1f6e }, // U1F6E # GREEK CAPITAL LETTER OMEGA WITH PSILI AND PERISPOMENI
    { {UNITIZE(TQt::Key_Dead_Tilde), UNITIZE(TQt::Key_Multi_key), 0x0029, 0x03b1, 0, 0}, 0x1f06 }, // U1F06 # GREEK SMALL LETTER ALPHA WITH PSILI AND PERISPOMENI
    { {UNITIZE(TQt::Key_Dead_Tilde), UNITIZE(TQt::Key_Multi_key), 0x0029, 0x03b7, 0, 0}, 0x1f26 }, // U1F26 # GREEK SMALL LETTER ETA WITH PSILI AND PERISPOMENI
    { {UNITIZE(TQt::Key_Dead_Tilde), UNITIZE(TQt::Key_Multi_key), 0x0029, 0x03b9, 0, 0}, 0x1f36 }, // U1F36 # GREEK SMALL LETTER IOTA WITH PSILI AND PERISPOMENI
    { {UNITIZE(TQt::Key_Dead_Tilde), UNITIZE(TQt::Key_Multi_key), 0x0029, 0x03c5, 0, 0}, 0x1f56 }, // U1F56 # GREEK SMALL LETTER UPSILON WITH PSILI AND PERISPOMENI
    { {UNITIZE(TQt::Key_Dead_Tilde), UNITIZE(TQt::Key_Multi_key), 0x0029, 0x03c9, 0, 0}, 0x1f66 }, // U1F66 # GREEK SMALL LETTER OMEGA WITH PSILI AND PERISPOMENI
    { {UNITIZE(TQt::Key_Dead_Tilde), UNITIZE(TQt::Key_Multi_key), 0x002b, 0x004f, 0, 0}, 0x1ee0 }, // U1EE0 # LATIN CAPITAL LETTER O WITH HORN AND TILDE
    { {UNITIZE(TQt::Key_Dead_Tilde), UNITIZE(TQt::Key_Multi_key), 0x002b, 0x0055, 0, 0}, 0x1eee }, // U1EEE # LATIN CAPITAL LETTER U WITH HORN AND TILDE
    { {UNITIZE(TQt::Key_Dead_Tilde), UNITIZE(TQt::Key_Multi_key), 0x002b, 0x006f, 0, 0}, 0x1ee1 }, // U1EE1 # LATIN SMALL LETTER O WITH HORN AND TILDE
    { {UNITIZE(TQt::Key_Dead_Tilde), UNITIZE(TQt::Key_Multi_key), 0x002b, 0x0075, 0, 0}, 0x1eef }, // U1EEF # LATIN SMALL LETTER U WITH HORN AND TILDE
    { {UNITIZE(TQt::Key_Dead_Tilde), UNITIZE(TQt::Key_Multi_key), 0x0055, 0x0041, 0, 0}, 0x1eb4 }, // U1EB4 # LATIN CAPITAL LETTER A WITH BREVE AND TILDE
    { {UNITIZE(TQt::Key_Dead_Tilde), UNITIZE(TQt::Key_Multi_key), 0x0055, 0x0061, 0, 0}, 0x1eb5 }, // U1EB5 # LATIN SMALL LETTER A WITH BREVE AND TILDE
    { {UNITIZE(TQt::Key_Dead_Tilde), UNITIZE(TQt::Key_Multi_key), 0x005e, 0x0041, 0, 0}, 0x1eaa }, // U1EAA # LATIN CAPITAL LETTER A WITH CIRCUMFLEX AND TILDE
    { {UNITIZE(TQt::Key_Dead_Tilde), UNITIZE(TQt::Key_Multi_key), 0x005e, 0x0045, 0, 0}, 0x1ec4 }, // U1EC4 # LATIN CAPITAL LETTER E WITH CIRCUMFLEX AND TILDE
    { {UNITIZE(TQt::Key_Dead_Tilde), UNITIZE(TQt::Key_Multi_key), 0x005e, 0x004f, 0, 0}, 0x1ed6 }, // U1ED6 # LATIN CAPITAL LETTER O WITH CIRCUMFLEX AND TILDE
    { {UNITIZE(TQt::Key_Dead_Tilde), UNITIZE(TQt::Key_Multi_key), 0x005e, 0x0061, 0, 0}, 0x1eab }, // U1EAB # LATIN SMALL LETTER A WITH CIRCUMFLEX AND TILDE
    { {UNITIZE(TQt::Key_Dead_Tilde), UNITIZE(TQt::Key_Multi_key), 0x005e, 0x0065, 0, 0}, 0x1ec5 }, // U1EC5 # LATIN SMALL LETTER E WITH CIRCUMFLEX AND TILDE
    { {UNITIZE(TQt::Key_Dead_Tilde), UNITIZE(TQt::Key_Multi_key), 0x005e, 0x006f, 0, 0}, 0x1ed7 }, // U1ED7 # LATIN SMALL LETTER O WITH CIRCUMFLEX AND TILDE
    { {UNITIZE(TQt::Key_Dead_Tilde), UNITIZE(TQt::Key_Multi_key), 0x0062, 0x0041, 0, 0}, 0x1eb4 }, // U1EB4 # LATIN CAPITAL LETTER A WITH BREVE AND TILDE
    { {UNITIZE(TQt::Key_Dead_Tilde), UNITIZE(TQt::Key_Multi_key), 0x0062, 0x0061, 0, 0}, 0x1eb5 }, // U1EB5 # LATIN SMALL LETTER A WITH BREVE AND TILDE
    { {UNITIZE(TQt::Key_Dead_Tilde), UNITIZE(TQt::Key_Dead_Circumflex), 0x0041, 0, 0, 0}, 0x1eaa }, // U1EAA # LATIN CAPITAL LETTER A WITH CIRCUMFLEX AND TILDE
    { {UNITIZE(TQt::Key_Dead_Tilde), UNITIZE(TQt::Key_Dead_Circumflex), 0x0045, 0, 0, 0}, 0x1ec4 }, // U1EC4 # LATIN CAPITAL LETTER E WITH CIRCUMFLEX AND TILDE
    { {UNITIZE(TQt::Key_Dead_Tilde), UNITIZE(TQt::Key_Dead_Circumflex), 0x004f, 0, 0, 0}, 0x1ed6 }, // U1ED6 # LATIN CAPITAL LETTER O WITH CIRCUMFLEX AND TILDE
    { {UNITIZE(TQt::Key_Dead_Tilde), UNITIZE(TQt::Key_Dead_Circumflex), 0x0061, 0, 0, 0}, 0x1eab }, // U1EAB # LATIN SMALL LETTER A WITH CIRCUMFLEX AND TILDE
    { {UNITIZE(TQt::Key_Dead_Tilde), UNITIZE(TQt::Key_Dead_Circumflex), 0x0065, 0, 0, 0}, 0x1ec5 }, // U1EC5 # LATIN SMALL LETTER E WITH CIRCUMFLEX AND TILDE
    { {UNITIZE(TQt::Key_Dead_Tilde), UNITIZE(TQt::Key_Dead_Circumflex), 0x006f, 0, 0, 0}, 0x1ed7 }, // U1ED7 # LATIN SMALL LETTER O WITH CIRCUMFLEX AND TILDE
    { {UNITIZE(TQt::Key_Dead_Tilde), UNITIZE(TQt::Key_Dead_Tilde), 0, 0, 0, 0}, 0x007e }, // asciitilde
    { {UNITIZE(TQt::Key_Dead_Tilde), UNITIZE(TQt::Key_Dead_Breve), 0x0041, 0, 0, 0}, 0x1eb4 }, // U1EB4 # LATIN CAPITAL LETTER A WITH BREVE AND TILDE
    { {UNITIZE(TQt::Key_Dead_Tilde), UNITIZE(TQt::Key_Dead_Breve), 0x0061, 0, 0, 0}, 0x1eb5 }, // U1EB5 # LATIN SMALL LETTER A WITH BREVE AND TILDE
    { {UNITIZE(TQt::Key_Dead_Tilde), UNITIZE(TQt::Key_Dead_Diaeresis), 0x03b9, 0, 0, 0}, 0x1fd7 }, // U1FD7 # GREEK SMALL LETTER IOTA WITH DIALYTIKA AND PERISPOMENI
    { {UNITIZE(TQt::Key_Dead_Tilde), UNITIZE(TQt::Key_Dead_Diaeresis), 0x03c5, 0, 0, 0}, 0x1fe7 }, // U1FE7 # GREEK SMALL LETTER UPSILON WITH DIALYTIKA AND PERISPOMENI
    { {UNITIZE(TQt::Key_Dead_Tilde), UNITIZE(TQt::Key_Dead_Horn), 0x004f, 0, 0, 0}, 0x1ee0 }, // U1EE0 # LATIN CAPITAL LETTER O WITH HORN AND TILDE
    { {UNITIZE(TQt::Key_Dead_Tilde), UNITIZE(TQt::Key_Dead_Horn), 0x0055, 0, 0, 0}, 0x1eee }, // U1EEE # LATIN CAPITAL LETTER U WITH HORN AND TILDE
    { {UNITIZE(TQt::Key_Dead_Tilde), UNITIZE(TQt::Key_Dead_Horn), 0x006f, 0, 0, 0}, 0x1ee1 }, // U1EE1 # LATIN SMALL LETTER O WITH HORN AND TILDE
    { {UNITIZE(TQt::Key_Dead_Tilde), UNITIZE(TQt::Key_Dead_Horn), 0x0075, 0, 0, 0}, 0x1eef }, // U1EEF # LATIN SMALL LETTER U WITH HORN AND TILDE
// /* broken */    { {UNITIZE(TQt::Key_Dead_Macron), 0x0000, 0x004c, 0, 0, 0}, 0x1e38 }, // U1E38 # LATIN CAPITAL LETTER L WITH DOT BELOW AND MACRON
// /* broken */    { {UNITIZE(TQt::Key_Dead_Macron), 0x0000, 0x004f, 0, 0, 0}, 0x022c }, // U022C # LATIN CAPITAL LETTER O WITH TILDE AND MACRON
// /* broken */    { {UNITIZE(TQt::Key_Dead_Macron), 0x0000, 0x0052, 0, 0, 0}, 0x1e5c }, // U1E5C # LATIN CAPITAL LETTER R WITH DOT BELOW AND MACRON
// /* broken */    { {UNITIZE(TQt::Key_Dead_Macron), 0x0000, 0x006c, 0, 0, 0}, 0x1e39 }, // U1E39 # LATIN SMALL LETTER L WITH DOT BELOW AND MACRON
// /* broken */    { {UNITIZE(TQt::Key_Dead_Macron), 0x0000, 0x006f, 0, 0, 0}, 0x022d }, // U022D # LATIN SMALL LETTER O WITH TILDE AND MACRON
// /* broken */    { {UNITIZE(TQt::Key_Dead_Macron), 0x0000, 0x0072, 0, 0, 0}, 0x1e5d }, // U1E5D # LATIN SMALL LETTER R WITH DOT BELOW AND MACRON
    { {UNITIZE(TQt::Key_Dead_Macron), 0x0020, 0, 0, 0, 0}, 0x00af }, // macron
    { {UNITIZE(TQt::Key_Dead_Macron), 0x0041, 0, 0, 0, 0}, 0x0100 }, // U0100 # LATIN CAPITAL LETTER A WITH MACRON
    { {UNITIZE(TQt::Key_Dead_Macron), 0x0045, 0, 0, 0, 0}, 0x0112 }, // U0112 # LATIN CAPITAL LETTER E WITH MACRON
    { {UNITIZE(TQt::Key_Dead_Macron), 0x0047, 0, 0, 0, 0}, 0x1e20 }, // U1E20 # LATIN CAPITAL LETTER G WITH MACRON
    { {UNITIZE(TQt::Key_Dead_Macron), 0x0049, 0, 0, 0, 0}, 0x012a }, // U012A # LATIN CAPITAL LETTER I WITH MACRON
    { {UNITIZE(TQt::Key_Dead_Macron), 0x004f, 0, 0, 0, 0}, 0x014c }, // U014C # LATIN CAPITAL LETTER O WITH MACRON
    { {UNITIZE(TQt::Key_Dead_Macron), 0x0055, 0, 0, 0, 0}, 0x016a }, // U016A # LATIN CAPITAL LETTER U WITH MACRON
    { {UNITIZE(TQt::Key_Dead_Macron), 0x0059, 0, 0, 0, 0}, 0x0232 }, // U0232 # LATIN CAPITAL LETTER Y WITH MACRON
    { {UNITIZE(TQt::Key_Dead_Macron), 0x0061, 0, 0, 0, 0}, 0x0101 }, // U0101 # LATIN SMALL LETTER A WITH MACRON
    { {UNITIZE(TQt::Key_Dead_Macron), 0x0065, 0, 0, 0, 0}, 0x0113 }, // U0113 # LATIN SMALL LETTER E WITH MACRON
    { {UNITIZE(TQt::Key_Dead_Macron), 0x0067, 0, 0, 0, 0}, 0x1e21 }, // U1E21 # LATIN SMALL LETTER G WITH MACRON
    { {UNITIZE(TQt::Key_Dead_Macron), 0x0069, 0, 0, 0, 0}, 0x012b }, // U012B # LATIN SMALL LETTER I WITH MACRON
    { {UNITIZE(TQt::Key_Dead_Macron), 0x006f, 0, 0, 0, 0}, 0x014d }, // U014D # LATIN SMALL LETTER O WITH MACRON
    { {UNITIZE(TQt::Key_Dead_Macron), 0x0075, 0, 0, 0, 0}, 0x016b }, // U016B # LATIN SMALL LETTER U WITH MACRON
    { {UNITIZE(TQt::Key_Dead_Macron), 0x0079, 0, 0, 0, 0}, 0x0233 }, // U0233 # LATIN SMALL LETTER Y WITH MACRON
    { {UNITIZE(TQt::Key_Dead_Macron), 0x00c4, 0, 0, 0, 0}, 0x01de }, // U01DE # LATIN CAPITAL LETTER A WITH DIAERESIS AND MACRON
    { {UNITIZE(TQt::Key_Dead_Macron), 0x00c6, 0, 0, 0, 0}, 0x01e2 }, // U01E2 # LATIN CAPITAL LETTER AE WITH MACRON
    { {UNITIZE(TQt::Key_Dead_Macron), 0x00d5, 0, 0, 0, 0}, 0x022c }, // U022C # LATIN CAPITAL LETTER O WITH TILDE AND MACRON
    { {UNITIZE(TQt::Key_Dead_Macron), 0x00d6, 0, 0, 0, 0}, 0x022a }, // U022A # LATIN CAPITAL LETTER O WITH DIAERESIS AND MACRON
    { {UNITIZE(TQt::Key_Dead_Macron), 0x00dc, 0, 0, 0, 0}, 0x01d5 }, // U01D5 # LATIN CAPITAL LETTER U WITH DIAERESIS AND MACRON
    { {UNITIZE(TQt::Key_Dead_Macron), 0x00e4, 0, 0, 0, 0}, 0x01df }, // U01DF # LATIN SMALL LETTER A WITH DIAERESIS AND MACRON
    { {UNITIZE(TQt::Key_Dead_Macron), 0x00e6, 0, 0, 0, 0}, 0x01e3 }, // U01E3 # LATIN SMALL LETTER AE WITH MACRON
    { {UNITIZE(TQt::Key_Dead_Macron), 0x00f5, 0, 0, 0, 0}, 0x022d }, // U022D # LATIN SMALL LETTER O WITH TILDE AND MACRON
    { {UNITIZE(TQt::Key_Dead_Macron), 0x00f6, 0, 0, 0, 0}, 0x022b }, // U022B # LATIN SMALL LETTER O WITH DIAERESIS AND MACRON
    { {UNITIZE(TQt::Key_Dead_Macron), 0x00fc, 0, 0, 0, 0}, 0x01d6 }, // U01D6 # LATIN SMALL LETTER U WITH DIAERESIS AND MACRON
    { {UNITIZE(TQt::Key_Dead_Macron), 0x01ea, 0, 0, 0, 0}, 0x01ec }, // U01EC # LATIN CAPITAL LETTER O WITH OGONEK AND MACRON
    { {UNITIZE(TQt::Key_Dead_Macron), 0x01eb, 0, 0, 0, 0}, 0x01ed }, // U01ED # LATIN SMALL LETTER O WITH OGONEK AND MACRON
    { {UNITIZE(TQt::Key_Dead_Macron), 0x0226, 0, 0, 0, 0}, 0x01e0 }, // U01E0 # LATIN CAPITAL LETTER A WITH DOT ABOVE AND MACRON
    { {UNITIZE(TQt::Key_Dead_Macron), 0x0227, 0, 0, 0, 0}, 0x01e1 }, // U01E1 # LATIN SMALL LETTER A WITH DOT ABOVE AND MACRON
    { {UNITIZE(TQt::Key_Dead_Macron), 0x022e, 0, 0, 0, 0}, 0x0230 }, // U0230 # LATIN CAPITAL LETTER O WITH DOT ABOVE AND MACRON
    { {UNITIZE(TQt::Key_Dead_Macron), 0x022f, 0, 0, 0, 0}, 0x0231 }, // U0231 # LATIN SMALL LETTER O WITH DOT ABOVE AND MACRON
    { {UNITIZE(TQt::Key_Dead_Macron), 0x0391, 0, 0, 0, 0}, 0x1fb9 }, // U1FB9 # GREEK CAPITAL LETTER ALPHA WITH MACRON
    { {UNITIZE(TQt::Key_Dead_Macron), 0x0399, 0, 0, 0, 0}, 0x1fd9 }, // U1FD9 # GREEK CAPITAL LETTER IOTA WITH MACRON
    { {UNITIZE(TQt::Key_Dead_Macron), 0x03a5, 0, 0, 0, 0}, 0x1fe9 }, // U1FE9 # GREEK CAPITAL LETTER UPSILON WITH MACRON
    { {UNITIZE(TQt::Key_Dead_Macron), 0x03b1, 0, 0, 0, 0}, 0x1fb1 }, // U1FB1 # GREEK SMALL LETTER ALPHA WITH MACRON
    { {UNITIZE(TQt::Key_Dead_Macron), 0x03b9, 0, 0, 0, 0}, 0x1fd1 }, // U1FD1 # GREEK SMALL LETTER IOTA WITH MACRON
    { {UNITIZE(TQt::Key_Dead_Macron), 0x03c5, 0, 0, 0, 0}, 0x1fe1 }, // U1FE1 # GREEK SMALL LETTER UPSILON WITH MACRON
    { {UNITIZE(TQt::Key_Dead_Macron), 0x0418, 0, 0, 0, 0}, 0x04e2 }, // U04E2 # CYRILLIC CAPITAL LETTER I WITH MACRON
    { {UNITIZE(TQt::Key_Dead_Macron), 0x0423, 0, 0, 0, 0}, 0x04ee }, // U04EE # CYRILLIC CAPITAL LETTER U WITH MACRON
    { {UNITIZE(TQt::Key_Dead_Macron), 0x0438, 0, 0, 0, 0}, 0x04e3 }, // U04E3 # CYRILLIC SMALL LETTER I WITH MACRON
    { {UNITIZE(TQt::Key_Dead_Macron), 0x0443, 0, 0, 0, 0}, 0x04ef }, // U04EF # CYRILLIC SMALL LETTER U WITH MACRON
    { {UNITIZE(TQt::Key_Dead_Macron), 0x1e36, 0, 0, 0, 0}, 0x1e38 }, // U1E38 # LATIN CAPITAL LETTER L WITH DOT BELOW AND MACRON
    { {UNITIZE(TQt::Key_Dead_Macron), 0x1e37, 0, 0, 0, 0}, 0x1e39 }, // U1E39 # LATIN SMALL LETTER L WITH DOT BELOW AND MACRON
    { {UNITIZE(TQt::Key_Dead_Macron), 0x1e5a, 0, 0, 0, 0}, 0x1e5c }, // U1E5C # LATIN CAPITAL LETTER R WITH DOT BELOW AND MACRON
    { {UNITIZE(TQt::Key_Dead_Macron), 0x1e5b, 0, 0, 0, 0}, 0x1e5d }, // U1E5D # LATIN SMALL LETTER R WITH DOT BELOW AND MACRON
    { {UNITIZE(TQt::Key_Dead_Macron), UNITIZE(TQt::Key_Multi_key), 0x0021, 0x004c, 0, 0}, 0x1e38 }, // U1E38 # LATIN CAPITAL LETTER L WITH DOT BELOW AND MACRON
    { {UNITIZE(TQt::Key_Dead_Macron), UNITIZE(TQt::Key_Multi_key), 0x0021, 0x0052, 0, 0}, 0x1e5c }, // U1E5C # LATIN CAPITAL LETTER R WITH DOT BELOW AND MACRON
    { {UNITIZE(TQt::Key_Dead_Macron), UNITIZE(TQt::Key_Multi_key), 0x0021, 0x006c, 0, 0}, 0x1e39 }, // U1E39 # LATIN SMALL LETTER L WITH DOT BELOW AND MACRON
    { {UNITIZE(TQt::Key_Dead_Macron), UNITIZE(TQt::Key_Multi_key), 0x0021, 0x0072, 0, 0}, 0x1e5d }, // U1E5D # LATIN SMALL LETTER R WITH DOT BELOW AND MACRON
    { {UNITIZE(TQt::Key_Dead_Macron), UNITIZE(TQt::Key_Multi_key), 0x0022, 0x0041, 0, 0}, 0x01de }, // U01DE # LATIN CAPITAL LETTER A WITH DIAERESIS AND MACRON
    { {UNITIZE(TQt::Key_Dead_Macron), UNITIZE(TQt::Key_Multi_key), 0x0022, 0x004f, 0, 0}, 0x022a }, // U022A # LATIN CAPITAL LETTER O WITH DIAERESIS AND MACRON
    { {UNITIZE(TQt::Key_Dead_Macron), UNITIZE(TQt::Key_Multi_key), 0x0022, 0x0055, 0, 0}, 0x01d5 }, // U01D5 # LATIN CAPITAL LETTER U WITH DIAERESIS AND MACRON
    { {UNITIZE(TQt::Key_Dead_Macron), UNITIZE(TQt::Key_Multi_key), 0x0022, 0x0061, 0, 0}, 0x01df }, // U01DF # LATIN SMALL LETTER A WITH DIAERESIS AND MACRON
    { {UNITIZE(TQt::Key_Dead_Macron), UNITIZE(TQt::Key_Multi_key), 0x0022, 0x006f, 0, 0}, 0x022b }, // U022B # LATIN SMALL LETTER O WITH DIAERESIS AND MACRON
    { {UNITIZE(TQt::Key_Dead_Macron), UNITIZE(TQt::Key_Multi_key), 0x0022, 0x0075, 0, 0}, 0x01d6 }, // U01D6 # LATIN SMALL LETTER U WITH DIAERESIS AND MACRON
    { {UNITIZE(TQt::Key_Dead_Macron), UNITIZE(TQt::Key_Multi_key), 0x002e, 0x0041, 0, 0}, 0x01e0 }, // U01E0 # LATIN CAPITAL LETTER A WITH DOT ABOVE AND MACRON
    { {UNITIZE(TQt::Key_Dead_Macron), UNITIZE(TQt::Key_Multi_key), 0x002e, 0x004f, 0, 0}, 0x0230 }, // U0230 # LATIN CAPITAL LETTER O WITH DOT ABOVE AND MACRON
    { {UNITIZE(TQt::Key_Dead_Macron), UNITIZE(TQt::Key_Multi_key), 0x002e, 0x0061, 0, 0}, 0x01e1 }, // U01E1 # LATIN SMALL LETTER A WITH DOT ABOVE AND MACRON
    { {UNITIZE(TQt::Key_Dead_Macron), UNITIZE(TQt::Key_Multi_key), 0x002e, 0x006f, 0, 0}, 0x0231 }, // U0231 # LATIN SMALL LETTER O WITH DOT ABOVE AND MACRON
    { {UNITIZE(TQt::Key_Dead_Macron), UNITIZE(TQt::Key_Multi_key), 0x003b, 0x004f, 0, 0}, 0x01ec }, // U01EC # LATIN CAPITAL LETTER O WITH OGONEK AND MACRON
    { {UNITIZE(TQt::Key_Dead_Macron), UNITIZE(TQt::Key_Multi_key), 0x003b, 0x006f, 0, 0}, 0x01ed }, // U01ED # LATIN SMALL LETTER O WITH OGONEK AND MACRON
    { {UNITIZE(TQt::Key_Dead_Macron), UNITIZE(TQt::Key_Multi_key), 0x007e, 0x004f, 0, 0}, 0x022c }, // U022C # LATIN CAPITAL LETTER O WITH TILDE AND MACRON
    { {UNITIZE(TQt::Key_Dead_Macron), UNITIZE(TQt::Key_Multi_key), 0x007e, 0x006f, 0, 0}, 0x022d }, // U022D # LATIN SMALL LETTER O WITH TILDE AND MACRON
    { {UNITIZE(TQt::Key_Dead_Macron), UNITIZE(TQt::Key_Dead_Tilde), 0x004f, 0, 0, 0}, 0x022c }, // U022C # LATIN CAPITAL LETTER O WITH TILDE AND MACRON
    { {UNITIZE(TQt::Key_Dead_Macron), UNITIZE(TQt::Key_Dead_Tilde), 0x006f, 0, 0, 0}, 0x022d }, // U022D # LATIN SMALL LETTER O WITH TILDE AND MACRON
    { {UNITIZE(TQt::Key_Dead_Macron), UNITIZE(TQt::Key_Dead_Macron), 0, 0, 0, 0}, 0x00af }, // macron
    { {UNITIZE(TQt::Key_Dead_Macron), UNITIZE(TQt::Key_Dead_Abovedot), 0x0041, 0, 0, 0}, 0x01e0 }, // U01E0 # LATIN CAPITAL LETTER A WITH DOT ABOVE AND MACRON
    { {UNITIZE(TQt::Key_Dead_Macron), UNITIZE(TQt::Key_Dead_Abovedot), 0x004f, 0, 0, 0}, 0x0230 }, // U0230 # LATIN CAPITAL LETTER O WITH DOT ABOVE AND MACRON
    { {UNITIZE(TQt::Key_Dead_Macron), UNITIZE(TQt::Key_Dead_Abovedot), 0x0061, 0, 0, 0}, 0x01e1 }, // U01E1 # LATIN SMALL LETTER A WITH DOT ABOVE AND MACRON
    { {UNITIZE(TQt::Key_Dead_Macron), UNITIZE(TQt::Key_Dead_Abovedot), 0x006f, 0, 0, 0}, 0x0231 }, // U0231 # LATIN SMALL LETTER O WITH DOT ABOVE AND MACRON
    { {UNITIZE(TQt::Key_Dead_Macron), UNITIZE(TQt::Key_Dead_Diaeresis), 0x0041, 0, 0, 0}, 0x01de }, // U01DE # LATIN CAPITAL LETTER A WITH DIAERESIS AND MACRON
    { {UNITIZE(TQt::Key_Dead_Macron), UNITIZE(TQt::Key_Dead_Diaeresis), 0x004f, 0, 0, 0}, 0x022a }, // U022A # LATIN CAPITAL LETTER O WITH DIAERESIS AND MACRON
    { {UNITIZE(TQt::Key_Dead_Macron), UNITIZE(TQt::Key_Dead_Diaeresis), 0x0055, 0, 0, 0}, 0x01d5 }, // U01D5 # LATIN CAPITAL LETTER U WITH DIAERESIS AND MACRON
    { {UNITIZE(TQt::Key_Dead_Macron), UNITIZE(TQt::Key_Dead_Diaeresis), 0x0061, 0, 0, 0}, 0x01df }, // U01DF # LATIN SMALL LETTER A WITH DIAERESIS AND MACRON
    { {UNITIZE(TQt::Key_Dead_Macron), UNITIZE(TQt::Key_Dead_Diaeresis), 0x006f, 0, 0, 0}, 0x022b }, // U022B # LATIN SMALL LETTER O WITH DIAERESIS AND MACRON
    { {UNITIZE(TQt::Key_Dead_Macron), UNITIZE(TQt::Key_Dead_Diaeresis), 0x0075, 0, 0, 0}, 0x01d6 }, // U01D6 # LATIN SMALL LETTER U WITH DIAERESIS AND MACRON
    { {UNITIZE(TQt::Key_Dead_Macron), UNITIZE(TQt::Key_Dead_Ogonek), 0x004f, 0, 0, 0}, 0x01ec }, // U01EC # LATIN CAPITAL LETTER O WITH OGONEK AND MACRON
    { {UNITIZE(TQt::Key_Dead_Macron), UNITIZE(TQt::Key_Dead_Ogonek), 0x006f, 0, 0, 0}, 0x01ed }, // U01ED # LATIN SMALL LETTER O WITH OGONEK AND MACRON
    { {UNITIZE(TQt::Key_Dead_Macron), UNITIZE(TQt::Key_Dead_Belowdot), 0x004c, 0, 0, 0}, 0x1e38 }, // U1E38 # LATIN CAPITAL LETTER L WITH DOT BELOW AND MACRON
    { {UNITIZE(TQt::Key_Dead_Macron), UNITIZE(TQt::Key_Dead_Belowdot), 0x0052, 0, 0, 0}, 0x1e5c }, // U1E5C # LATIN CAPITAL LETTER R WITH DOT BELOW AND MACRON
    { {UNITIZE(TQt::Key_Dead_Macron), UNITIZE(TQt::Key_Dead_Belowdot), 0x006c, 0, 0, 0}, 0x1e39 }, // U1E39 # LATIN SMALL LETTER L WITH DOT BELOW AND MACRON
    { {UNITIZE(TQt::Key_Dead_Macron), UNITIZE(TQt::Key_Dead_Belowdot), 0x0072, 0, 0, 0}, 0x1e5d }, // U1E5D # LATIN SMALL LETTER R WITH DOT BELOW AND MACRON
// /* broken */    { {UNITIZE(TQt::Key_Dead_Breve), 0x0000, 0x0041, 0, 0, 0}, 0x1eb6 }, // U1EB6 # LATIN CAPITAL LETTER A WITH BREVE AND DOT BELOW
// /* broken */    { {UNITIZE(TQt::Key_Dead_Breve), 0x0000, 0x0061, 0, 0, 0}, 0x1eb7 }, // U1EB7 # LATIN SMALL LETTER A WITH BREVE AND DOT BELOW
    { {UNITIZE(TQt::Key_Dead_Breve), 0x0020, 0, 0, 0, 0}, 0x02d8 }, // breve
    { {UNITIZE(TQt::Key_Dead_Breve), 0x0041, 0, 0, 0, 0}, 0x0102 }, // U0102 # LATIN CAPITAL LETTER A WITH BREVE
    { {UNITIZE(TQt::Key_Dead_Breve), 0x0045, 0, 0, 0, 0}, 0x0114 }, // U0114 # LATIN CAPITAL LETTER E WITH BREVE
    { {UNITIZE(TQt::Key_Dead_Breve), 0x0047, 0, 0, 0, 0}, 0x011e }, // U011E # LATIN CAPITAL LETTER G WITH BREVE
    { {UNITIZE(TQt::Key_Dead_Breve), 0x0049, 0, 0, 0, 0}, 0x012c }, // U012C # LATIN CAPITAL LETTER I WITH BREVE
    { {UNITIZE(TQt::Key_Dead_Breve), 0x004f, 0, 0, 0, 0}, 0x014e }, // U014E # LATIN CAPITAL LETTER O WITH BREVE
    { {UNITIZE(TQt::Key_Dead_Breve), 0x0055, 0, 0, 0, 0}, 0x016c }, // U016C # LATIN CAPITAL LETTER U WITH BREVE
    { {UNITIZE(TQt::Key_Dead_Breve), 0x0061, 0, 0, 0, 0}, 0x0103 }, // U0103 # LATIN SMALL LETTER A WITH BREVE
    { {UNITIZE(TQt::Key_Dead_Breve), 0x0065, 0, 0, 0, 0}, 0x0115 }, // U0115 # LATIN SMALL LETTER E WITH BREVE
    { {UNITIZE(TQt::Key_Dead_Breve), 0x0067, 0, 0, 0, 0}, 0x011f }, // U011F # LATIN SMALL LETTER G WITH BREVE
    { {UNITIZE(TQt::Key_Dead_Breve), 0x0069, 0, 0, 0, 0}, 0x012d }, // U012D # LATIN SMALL LETTER I WITH BREVE
    { {UNITIZE(TQt::Key_Dead_Breve), 0x006f, 0, 0, 0, 0}, 0x014f }, // U014F # LATIN SMALL LETTER O WITH BREVE
    { {UNITIZE(TQt::Key_Dead_Breve), 0x0075, 0, 0, 0, 0}, 0x016d }, // U016D # LATIN SMALL LETTER U WITH BREVE
    { {UNITIZE(TQt::Key_Dead_Breve), 0x0228, 0, 0, 0, 0}, 0x1e1c }, // U1E1C # LATIN CAPITAL LETTER E WITH CEDILLA AND BREVE
    { {UNITIZE(TQt::Key_Dead_Breve), 0x0229, 0, 0, 0, 0}, 0x1e1d }, // U1E1D # LATIN SMALL LETTER E WITH CEDILLA AND BREVE
    { {UNITIZE(TQt::Key_Dead_Breve), 0x0391, 0, 0, 0, 0}, 0x1fb8 }, // U1FB8 # GREEK CAPITAL LETTER ALPHA WITH VRACHY
    { {UNITIZE(TQt::Key_Dead_Breve), 0x0399, 0, 0, 0, 0}, 0x1fd8 }, // U1FD8 # GREEK CAPITAL LETTER IOTA WITH VRACHY
    { {UNITIZE(TQt::Key_Dead_Breve), 0x03a5, 0, 0, 0, 0}, 0x1fe8 }, // U1FE8 # GREEK CAPITAL LETTER UPSILON WITH VRACHY
    { {UNITIZE(TQt::Key_Dead_Breve), 0x03b1, 0, 0, 0, 0}, 0x1fb0 }, // U1FB0 # GREEK SMALL LETTER ALPHA WITH VRACHY
    { {UNITIZE(TQt::Key_Dead_Breve), 0x03b9, 0, 0, 0, 0}, 0x1fd0 }, // U1FD0 # GREEK SMALL LETTER IOTA WITH VRACHY
    { {UNITIZE(TQt::Key_Dead_Breve), 0x03c5, 0, 0, 0, 0}, 0x1fe0 }, // U1FE0 # GREEK SMALL LETTER UPSILON WITH VRACHY
    { {UNITIZE(TQt::Key_Dead_Breve), 0x0410, 0, 0, 0, 0}, 0x04d0 }, // U04D0 # CYRILLIC CAPITAL LETTER A WITH BREVE
    { {UNITIZE(TQt::Key_Dead_Breve), 0x0415, 0, 0, 0, 0}, 0x04d6 }, // U04D6 # CYRILLIC CAPITAL LETTER IE WITH BREVE
    { {UNITIZE(TQt::Key_Dead_Breve), 0x0416, 0, 0, 0, 0}, 0x04c1 }, // U04C1 # CYRILLIC CAPITAL LETTER ZHE WITH BREVE
    { {UNITIZE(TQt::Key_Dead_Breve), 0x0418, 0, 0, 0, 0}, 0x0419 }, // U0419 # CYRILLIC CAPITAL LETTER SHORT I
    { {UNITIZE(TQt::Key_Dead_Breve), 0x0423, 0, 0, 0, 0}, 0x040e }, // U040E # CYRILLIC CAPITAL LETTER SHORT U
    { {UNITIZE(TQt::Key_Dead_Breve), 0x0430, 0, 0, 0, 0}, 0x04d1 }, // U04D1 # CYRILLIC SMALL LETTER A WITH BREVE
    { {UNITIZE(TQt::Key_Dead_Breve), 0x0435, 0, 0, 0, 0}, 0x04d7 }, // U04D7 # CYRILLIC SMALL LETTER IE WITH BREVE
    { {UNITIZE(TQt::Key_Dead_Breve), 0x0436, 0, 0, 0, 0}, 0x04c2 }, // U04C2 # CYRILLIC SMALL LETTER ZHE WITH BREVE
    { {UNITIZE(TQt::Key_Dead_Breve), 0x0438, 0, 0, 0, 0}, 0x0439 }, // U0439 # CYRILLIC SMALL LETTER SHORT I
    { {UNITIZE(TQt::Key_Dead_Breve), 0x0443, 0, 0, 0, 0}, 0x045e }, // U045E # CYRILLIC SMALL LETTER SHORT U
    { {UNITIZE(TQt::Key_Dead_Breve), 0x1ea0, 0, 0, 0, 0}, 0x1eb6 }, // U1EB6 # LATIN CAPITAL LETTER A WITH BREVE AND DOT BELOW
    { {UNITIZE(TQt::Key_Dead_Breve), 0x1ea1, 0, 0, 0, 0}, 0x1eb7 }, // U1EB7 # LATIN SMALL LETTER A WITH BREVE AND DOT BELOW
    { {UNITIZE(TQt::Key_Dead_Breve), UNITIZE(TQt::Key_Multi_key), 0x0021, 0x0041, 0, 0}, 0x1eb6 }, // U1EB6 # LATIN CAPITAL LETTER A WITH BREVE AND DOT BELOW
    { {UNITIZE(TQt::Key_Dead_Breve), UNITIZE(TQt::Key_Multi_key), 0x0021, 0x0061, 0, 0}, 0x1eb7 }, // U1EB7 # LATIN SMALL LETTER A WITH BREVE AND DOT BELOW
    { {UNITIZE(TQt::Key_Dead_Breve), UNITIZE(TQt::Key_Multi_key), 0x002c, 0x0045, 0, 0}, 0x1e1c }, // U1E1C # LATIN CAPITAL LETTER E WITH CEDILLA AND BREVE
    { {UNITIZE(TQt::Key_Dead_Breve), UNITIZE(TQt::Key_Multi_key), 0x002c, 0x0065, 0, 0}, 0x1e1d }, // U1E1D # LATIN SMALL LETTER E WITH CEDILLA AND BREVE
    { {UNITIZE(TQt::Key_Dead_Breve), UNITIZE(TQt::Key_Dead_Breve), 0, 0, 0, 0}, 0x02d8 }, // breve
    { {UNITIZE(TQt::Key_Dead_Breve), UNITIZE(TQt::Key_Dead_Cedilla), 0x0045, 0, 0, 0}, 0x1e1c }, // U1E1C # LATIN CAPITAL LETTER E WITH CEDILLA AND BREVE
    { {UNITIZE(TQt::Key_Dead_Breve), UNITIZE(TQt::Key_Dead_Cedilla), 0x0065, 0, 0, 0}, 0x1e1d }, // U1E1D # LATIN SMALL LETTER E WITH CEDILLA AND BREVE
    { {UNITIZE(TQt::Key_Dead_Breve), UNITIZE(TQt::Key_Dead_Belowdot), 0x0041, 0, 0, 0}, 0x1eb6 }, // U1EB6 # LATIN CAPITAL LETTER A WITH BREVE AND DOT BELOW
    { {UNITIZE(TQt::Key_Dead_Breve), UNITIZE(TQt::Key_Dead_Belowdot), 0x0061, 0, 0, 0}, 0x1eb7 }, // U1EB7 # LATIN SMALL LETTER A WITH BREVE AND DOT BELOW
// /* broken */    { {UNITIZE(TQt::Key_Dead_Abovedot), 0x0000, 0x0053, 0, 0, 0}, 0x1e64 }, // U1E64 # LATIN CAPITAL LETTER S WITH ACUTE AND DOT ABOVE
// /* broken */    { {UNITIZE(TQt::Key_Dead_Abovedot), 0x0000, 0x0053, 0, 0, 0}, 0x1e68 }, // U1E68 # LATIN CAPITAL LETTER S WITH DOT BELOW AND DOT ABOVE
// /* broken */    { {UNITIZE(TQt::Key_Dead_Abovedot), 0x0000, 0x0073, 0, 0, 0}, 0x1e69 }, // U1E69 # LATIN SMALL LETTER S WITH DOT BELOW AND DOT ABOVE
// /* broken */    { {UNITIZE(TQt::Key_Dead_Abovedot), 0x0000, 0x0073, 0, 0, 0}, 0x1e65 }, // U1E65 # LATIN SMALL LETTER S WITH ACUTE AND DOT ABOVE
    { {UNITIZE(TQt::Key_Dead_Abovedot), 0x0020, 0, 0, 0, 0}, 0x02d9 }, // abovedot
    { {UNITIZE(TQt::Key_Dead_Abovedot), 0x0041, 0, 0, 0, 0}, 0x0226 }, // U0226 # LATIN CAPITAL LETTER A WITH DOT ABOVE
    { {UNITIZE(TQt::Key_Dead_Abovedot), 0x0042, 0, 0, 0, 0}, 0x1e02 }, // U1E02 # LATIN CAPITAL LETTER B WITH DOT ABOVE
    { {UNITIZE(TQt::Key_Dead_Abovedot), 0x0043, 0, 0, 0, 0}, 0x010a }, // U010A # LATIN CAPITAL LETTER C WITH DOT ABOVE
    { {UNITIZE(TQt::Key_Dead_Abovedot), 0x0044, 0, 0, 0, 0}, 0x1e0a }, // U1E0A # LATIN CAPITAL LETTER D WITH DOT ABOVE
    { {UNITIZE(TQt::Key_Dead_Abovedot), 0x0045, 0, 0, 0, 0}, 0x0116 }, // U0116 # LATIN CAPITAL LETTER E WITH DOT ABOVE
    { {UNITIZE(TQt::Key_Dead_Abovedot), 0x0046, 0, 0, 0, 0}, 0x1e1e }, // U1E1E # LATIN CAPITAL LETTER F WITH DOT ABOVE
    { {UNITIZE(TQt::Key_Dead_Abovedot), 0x0047, 0, 0, 0, 0}, 0x0120 }, // U0120 # LATIN CAPITAL LETTER G WITH DOT ABOVE
    { {UNITIZE(TQt::Key_Dead_Abovedot), 0x0048, 0, 0, 0, 0}, 0x1e22 }, // U1E22 # LATIN CAPITAL LETTER H WITH DOT ABOVE
    { {UNITIZE(TQt::Key_Dead_Abovedot), 0x0049, 0, 0, 0, 0}, 0x0130 }, // U0130 # LATIN CAPITAL LETTER I WITH DOT ABOVE
    { {UNITIZE(TQt::Key_Dead_Abovedot), 0x004d, 0, 0, 0, 0}, 0x1e40 }, // U1E40 # LATIN CAPITAL LETTER M WITH DOT ABOVE
    { {UNITIZE(TQt::Key_Dead_Abovedot), 0x004e, 0, 0, 0, 0}, 0x1e44 }, // U1E44 # LATIN CAPITAL LETTER N WITH DOT ABOVE
    { {UNITIZE(TQt::Key_Dead_Abovedot), 0x004f, 0, 0, 0, 0}, 0x022e }, // U022E # LATIN CAPITAL LETTER O WITH DOT ABOVE
    { {UNITIZE(TQt::Key_Dead_Abovedot), 0x0050, 0, 0, 0, 0}, 0x1e56 }, // U1E56 # LATIN CAPITAL LETTER P WITH DOT ABOVE
    { {UNITIZE(TQt::Key_Dead_Abovedot), 0x0052, 0, 0, 0, 0}, 0x1e58 }, // U1E58 # LATIN CAPITAL LETTER R WITH DOT ABOVE
    { {UNITIZE(TQt::Key_Dead_Abovedot), 0x0053, 0, 0, 0, 0}, 0x1e60 }, // U1E60 # LATIN CAPITAL LETTER S WITH DOT ABOVE
    { {UNITIZE(TQt::Key_Dead_Abovedot), 0x0054, 0, 0, 0, 0}, 0x1e6a }, // U1E6A # LATIN CAPITAL LETTER T WITH DOT ABOVE
    { {UNITIZE(TQt::Key_Dead_Abovedot), 0x0057, 0, 0, 0, 0}, 0x1e86 }, // U1E86 # LATIN CAPITAL LETTER W WITH DOT ABOVE
    { {UNITIZE(TQt::Key_Dead_Abovedot), 0x0058, 0, 0, 0, 0}, 0x1e8a }, // U1E8A # LATIN CAPITAL LETTER X WITH DOT ABOVE
    { {UNITIZE(TQt::Key_Dead_Abovedot), 0x0059, 0, 0, 0, 0}, 0x1e8e }, // U1E8E # LATIN CAPITAL LETTER Y WITH DOT ABOVE
    { {UNITIZE(TQt::Key_Dead_Abovedot), 0x005a, 0, 0, 0, 0}, 0x017b }, // U017B # LATIN CAPITAL LETTER Z WITH DOT ABOVE
    { {UNITIZE(TQt::Key_Dead_Abovedot), 0x0061, 0, 0, 0, 0}, 0x0227 }, // U0227 # LATIN SMALL LETTER A WITH DOT ABOVE
    { {UNITIZE(TQt::Key_Dead_Abovedot), 0x0062, 0, 0, 0, 0}, 0x1e03 }, // U1E03 # LATIN SMALL LETTER B WITH DOT ABOVE
    { {UNITIZE(TQt::Key_Dead_Abovedot), 0x0063, 0, 0, 0, 0}, 0x010b }, // U010B # LATIN SMALL LETTER C WITH DOT ABOVE
    { {UNITIZE(TQt::Key_Dead_Abovedot), 0x0064, 0, 0, 0, 0}, 0x1e0b }, // U1E0B # LATIN SMALL LETTER D WITH DOT ABOVE
    { {UNITIZE(TQt::Key_Dead_Abovedot), 0x0065, 0, 0, 0, 0}, 0x0117 }, // U0117 # LATIN SMALL LETTER E WITH DOT ABOVE
    { {UNITIZE(TQt::Key_Dead_Abovedot), 0x0066, 0, 0, 0, 0}, 0x1e1f }, // U1E1F # LATIN SMALL LETTER F WITH DOT ABOVE
    { {UNITIZE(TQt::Key_Dead_Abovedot), 0x0067, 0, 0, 0, 0}, 0x0121 }, // U0121 # LATIN SMALL LETTER G WITH DOT ABOVE
    { {UNITIZE(TQt::Key_Dead_Abovedot), 0x0068, 0, 0, 0, 0}, 0x1e23 }, // U1E23 # LATIN SMALL LETTER H WITH DOT ABOVE
    { {UNITIZE(TQt::Key_Dead_Abovedot), 0x0069, 0, 0, 0, 0}, 0x0131 }, // U0131 # LATIN SMALL LETTER DOTLESS I
    { {UNITIZE(TQt::Key_Dead_Abovedot), 0x006d, 0, 0, 0, 0}, 0x1e41 }, // U1E41 # LATIN SMALL LETTER M WITH DOT ABOVE
    { {UNITIZE(TQt::Key_Dead_Abovedot), 0x006e, 0, 0, 0, 0}, 0x1e45 }, // U1E45 # LATIN SMALL LETTER N WITH DOT ABOVE
    { {UNITIZE(TQt::Key_Dead_Abovedot), 0x006f, 0, 0, 0, 0}, 0x022f }, // U022F # LATIN SMALL LETTER O WITH DOT ABOVE
    { {UNITIZE(TQt::Key_Dead_Abovedot), 0x0070, 0, 0, 0, 0}, 0x1e57 }, // U1E57 # LATIN SMALL LETTER P WITH DOT ABOVE
    { {UNITIZE(TQt::Key_Dead_Abovedot), 0x0072, 0, 0, 0, 0}, 0x1e59 }, // U1E59 # LATIN SMALL LETTER R WITH DOT ABOVE
    { {UNITIZE(TQt::Key_Dead_Abovedot), 0x0073, 0, 0, 0, 0}, 0x1e61 }, // U1E61 # LATIN SMALL LETTER S WITH DOT ABOVE
    { {UNITIZE(TQt::Key_Dead_Abovedot), 0x0074, 0, 0, 0, 0}, 0x1e6b }, // U1E6B # LATIN SMALL LETTER T WITH DOT ABOVE
    { {UNITIZE(TQt::Key_Dead_Abovedot), 0x0077, 0, 0, 0, 0}, 0x1e87 }, // U1E87 # LATIN SMALL LETTER W WITH DOT ABOVE
    { {UNITIZE(TQt::Key_Dead_Abovedot), 0x0078, 0, 0, 0, 0}, 0x1e8b }, // U1E8B # LATIN SMALL LETTER X WITH DOT ABOVE
    { {UNITIZE(TQt::Key_Dead_Abovedot), 0x0079, 0, 0, 0, 0}, 0x1e8f }, // U1E8F # LATIN SMALL LETTER Y WITH DOT ABOVE
    { {UNITIZE(TQt::Key_Dead_Abovedot), 0x007a, 0, 0, 0, 0}, 0x017c }, // U017C # LATIN SMALL LETTER Z WITH DOT ABOVE
    { {UNITIZE(TQt::Key_Dead_Abovedot), 0x015a, 0, 0, 0, 0}, 0x1e64 }, // U1E64 # LATIN CAPITAL LETTER S WITH ACUTE AND DOT ABOVE
    { {UNITIZE(TQt::Key_Dead_Abovedot), 0x015b, 0, 0, 0, 0}, 0x1e65 }, // U1E65 # LATIN SMALL LETTER S WITH ACUTE AND DOT ABOVE
    { {UNITIZE(TQt::Key_Dead_Abovedot), 0x0160, 0, 0, 0, 0}, 0x1e66 }, // U1E66 # LATIN CAPITAL LETTER S WITH CARON AND DOT ABOVE
    { {UNITIZE(TQt::Key_Dead_Abovedot), 0x0161, 0, 0, 0, 0}, 0x1e67 }, // U1E67 # LATIN SMALL LETTER S WITH CARON AND DOT ABOVE
    { {UNITIZE(TQt::Key_Dead_Abovedot), 0x017f, 0, 0, 0, 0}, 0x1e9b }, // U1E9B # LATIN SMALL LETTER LONG S WITH DOT ABOVE
    { {UNITIZE(TQt::Key_Dead_Abovedot), 0x1e62, 0, 0, 0, 0}, 0x1e68 }, // U1E68 # LATIN CAPITAL LETTER S WITH DOT BELOW AND DOT ABOVE
    { {UNITIZE(TQt::Key_Dead_Abovedot), 0x1e63, 0, 0, 0, 0}, 0x1e69 }, // U1E69 # LATIN SMALL LETTER S WITH DOT BELOW AND DOT ABOVE
    { {UNITIZE(TQt::Key_Dead_Abovedot), UNITIZE(TQt::Key_Multi_key), 0x0021, 0x0053, 0, 0}, 0x1e68 }, // U1E68 # LATIN CAPITAL LETTER S WITH DOT BELOW AND DOT ABOVE
    { {UNITIZE(TQt::Key_Dead_Abovedot), UNITIZE(TQt::Key_Multi_key), 0x0021, 0x0073, 0, 0}, 0x1e69 }, // U1E69 # LATIN SMALL LETTER S WITH DOT BELOW AND DOT ABOVE
    { {UNITIZE(TQt::Key_Dead_Abovedot), UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0053, 0, 0}, 0x1e64 }, // U1E64 # LATIN CAPITAL LETTER S WITH ACUTE AND DOT ABOVE
    { {UNITIZE(TQt::Key_Dead_Abovedot), UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0073, 0, 0}, 0x1e65 }, // U1E65 # LATIN SMALL LETTER S WITH ACUTE AND DOT ABOVE
    { {UNITIZE(TQt::Key_Dead_Abovedot), UNITIZE(TQt::Key_Multi_key), 0x0063, 0x0053, 0, 0}, 0x1e66 }, // U1E66 # LATIN CAPITAL LETTER S WITH CARON AND DOT ABOVE
    { {UNITIZE(TQt::Key_Dead_Abovedot), UNITIZE(TQt::Key_Multi_key), 0x0063, 0x0073, 0, 0}, 0x1e67 }, // U1E67 # LATIN SMALL LETTER S WITH CARON AND DOT ABOVE
    { {UNITIZE(TQt::Key_Dead_Abovedot), UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0053, 0, 0}, 0x1e64 }, // U1E64 # LATIN CAPITAL LETTER S WITH ACUTE AND DOT ABOVE
    { {UNITIZE(TQt::Key_Dead_Abovedot), UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0073, 0, 0}, 0x1e65 }, // U1E65 # LATIN SMALL LETTER S WITH ACUTE AND DOT ABOVE
    { {UNITIZE(TQt::Key_Dead_Abovedot), UNITIZE(TQt::Key_Dead_Acute), 0x0053, 0, 0, 0}, 0x1e64 }, // U1E64 # LATIN CAPITAL LETTER S WITH ACUTE AND DOT ABOVE
    { {UNITIZE(TQt::Key_Dead_Abovedot), UNITIZE(TQt::Key_Dead_Acute), 0x0073, 0, 0, 0}, 0x1e65 }, // U1E65 # LATIN SMALL LETTER S WITH ACUTE AND DOT ABOVE
    { {UNITIZE(TQt::Key_Dead_Abovedot), UNITIZE(TQt::Key_Dead_Abovedot), 0, 0, 0, 0}, 0x02d9 }, // abovedot
    { {UNITIZE(TQt::Key_Dead_Abovedot), UNITIZE(TQt::Key_Dead_Caron), 0x0053, 0, 0, 0}, 0x1e66 }, // U1E66 # LATIN CAPITAL LETTER S WITH CARON AND DOT ABOVE
    { {UNITIZE(TQt::Key_Dead_Abovedot), UNITIZE(TQt::Key_Dead_Caron), 0x0073, 0, 0, 0}, 0x1e67 }, // U1E67 # LATIN SMALL LETTER S WITH CARON AND DOT ABOVE
    { {UNITIZE(TQt::Key_Dead_Abovedot), UNITIZE(TQt::Key_Dead_Belowdot), 0x0053, 0, 0, 0}, 0x1e68 }, // U1E68 # LATIN CAPITAL LETTER S WITH DOT BELOW AND DOT ABOVE
    { {UNITIZE(TQt::Key_Dead_Abovedot), UNITIZE(TQt::Key_Dead_Belowdot), 0x0073, 0, 0, 0}, 0x1e69 }, // U1E69 # LATIN SMALL LETTER S WITH DOT BELOW AND DOT ABOVE
// /* broken */    { {UNITIZE(TQt::Key_Dead_Diaeresis), 0x0000, 0x004f, 0, 0, 0}, 0x1e4e }, // U1E4E # LATIN CAPITAL LETTER O WITH TILDE AND DIAERESIS
// /* broken */    { {UNITIZE(TQt::Key_Dead_Diaeresis), 0x0000, 0x006f, 0, 0, 0}, 0x1e4f }, // U1E4F # LATIN SMALL LETTER O WITH TILDE AND DIAERESIS
    { {UNITIZE(TQt::Key_Dead_Diaeresis), 0x0020, 0, 0, 0, 0}, 0x005c }, // quotedbl
    { {UNITIZE(TQt::Key_Dead_Diaeresis), 0x0027, 0, 0, 0, 0}, 0x0344 }, // U0344 # COMBINING GREEK DIALYTIKA TONOS
    { {UNITIZE(TQt::Key_Dead_Diaeresis), 0x0041, 0, 0, 0, 0}, 0x00c4 }, // U00C4 # LATIN CAPITAL LETTER A WITH DIAERESIS
    { {UNITIZE(TQt::Key_Dead_Diaeresis), 0x0045, 0, 0, 0, 0}, 0x00cb }, // U00CB # LATIN CAPITAL LETTER E WITH DIAERESIS
    { {UNITIZE(TQt::Key_Dead_Diaeresis), 0x0048, 0, 0, 0, 0}, 0x1e26 }, // U1E26 # LATIN CAPITAL LETTER H WITH DIAERESIS
    { {UNITIZE(TQt::Key_Dead_Diaeresis), 0x0049, 0, 0, 0, 0}, 0x00cf }, // U00CF # LATIN CAPITAL LETTER I WITH DIAERESIS
    { {UNITIZE(TQt::Key_Dead_Diaeresis), 0x004f, 0, 0, 0, 0}, 0x00d6 }, // U00D6 # LATIN CAPITAL LETTER O WITH DIAERESIS
    { {UNITIZE(TQt::Key_Dead_Diaeresis), 0x0055, 0, 0, 0, 0}, 0x00dc }, // U00DC # LATIN CAPITAL LETTER U WITH DIAERESIS
    { {UNITIZE(TQt::Key_Dead_Diaeresis), 0x0057, 0, 0, 0, 0}, 0x1e84 }, // U1E84 # LATIN CAPITAL LETTER W WITH DIAERESIS
    { {UNITIZE(TQt::Key_Dead_Diaeresis), 0x0058, 0, 0, 0, 0}, 0x1e8c }, // U1E8C # LATIN CAPITAL LETTER X WITH DIAERESIS
    { {UNITIZE(TQt::Key_Dead_Diaeresis), 0x0059, 0, 0, 0, 0}, 0x0178 }, // U0178 # LATIN CAPITAL LETTER Y WITH DIAERESIS
    { {UNITIZE(TQt::Key_Dead_Diaeresis), 0x0061, 0, 0, 0, 0}, 0x00e4 }, // U00E4 # LATIN SMALL LETTER A WITH DIAERESIS
    { {UNITIZE(TQt::Key_Dead_Diaeresis), 0x0065, 0, 0, 0, 0}, 0x00eb }, // U00EB # LATIN SMALL LETTER E WITH DIAERESIS
    { {UNITIZE(TQt::Key_Dead_Diaeresis), 0x0068, 0, 0, 0, 0}, 0x1e27 }, // U1E27 # LATIN SMALL LETTER H WITH DIAERESIS
    { {UNITIZE(TQt::Key_Dead_Diaeresis), 0x0069, 0, 0, 0, 0}, 0x00ef }, // U00EF # LATIN SMALL LETTER I WITH DIAERESIS
    { {UNITIZE(TQt::Key_Dead_Diaeresis), 0x006f, 0, 0, 0, 0}, 0x00f6 }, // U00F6 # LATIN SMALL LETTER O WITH DIAERESIS
    { {UNITIZE(TQt::Key_Dead_Diaeresis), 0x0074, 0, 0, 0, 0}, 0x1e97 }, // U1E97 # LATIN SMALL LETTER T WITH DIAERESIS
    { {UNITIZE(TQt::Key_Dead_Diaeresis), 0x0075, 0, 0, 0, 0}, 0x00fc }, // U00FC # LATIN SMALL LETTER U WITH DIAERESIS
    { {UNITIZE(TQt::Key_Dead_Diaeresis), 0x0077, 0, 0, 0, 0}, 0x1e85 }, // U1E85 # LATIN SMALL LETTER W WITH DIAERESIS
    { {UNITIZE(TQt::Key_Dead_Diaeresis), 0x0078, 0, 0, 0, 0}, 0x1e8d }, // U1E8D # LATIN SMALL LETTER X WITH DIAERESIS
    { {UNITIZE(TQt::Key_Dead_Diaeresis), 0x0079, 0, 0, 0, 0}, 0x00ff }, // U00FF # LATIN SMALL LETTER Y WITH DIAERESIS
    { {UNITIZE(TQt::Key_Dead_Diaeresis), 0x00b4, 0, 0, 0, 0}, 0x0344 }, // U0344 # COMBINING GREEK DIALYTIKA TONOS
    { {UNITIZE(TQt::Key_Dead_Diaeresis), 0x00d5, 0, 0, 0, 0}, 0x1e4e }, // U1E4E # LATIN CAPITAL LETTER O WITH TILDE AND DIAERESIS
    { {UNITIZE(TQt::Key_Dead_Diaeresis), 0x00f5, 0, 0, 0, 0}, 0x1e4f }, // U1E4F # LATIN SMALL LETTER O WITH TILDE AND DIAERESIS
    { {UNITIZE(TQt::Key_Dead_Diaeresis), 0x016a, 0, 0, 0, 0}, 0x1e7a }, // U1E7A # LATIN CAPITAL LETTER U WITH MACRON AND DIAERESIS
    { {UNITIZE(TQt::Key_Dead_Diaeresis), 0x016b, 0, 0, 0, 0}, 0x1e7b }, // U1E7B # LATIN SMALL LETTER U WITH MACRON AND DIAERESIS
    { {UNITIZE(TQt::Key_Dead_Diaeresis), 0x0399, 0, 0, 0, 0}, 0x03aa }, // U03AA # GREEK CAPITAL LETTER IOTA WITH DIALYTIKA
    { {UNITIZE(TQt::Key_Dead_Diaeresis), 0x03a5, 0, 0, 0, 0}, 0x03ab }, // U03AB # GREEK CAPITAL LETTER UPSILON WITH DIALYTIKA
    { {UNITIZE(TQt::Key_Dead_Diaeresis), 0x03b9, 0, 0, 0, 0}, 0x03ca }, // U03CA # GREEK SMALL LETTER IOTA WITH DIALYTIKA
    { {UNITIZE(TQt::Key_Dead_Diaeresis), 0x03c5, 0, 0, 0, 0}, 0x03cb }, // U03CB # GREEK SMALL LETTER UPSILON WITH DIALYTIKA
    { {UNITIZE(TQt::Key_Dead_Diaeresis), 0x0406, 0, 0, 0, 0}, 0x0407 }, // U0407 # CYRILLIC CAPITAL LETTER YI
    { {UNITIZE(TQt::Key_Dead_Diaeresis), 0x0410, 0, 0, 0, 0}, 0x04d2 }, // U04D2 # CYRILLIC CAPITAL LETTER A WITH DIAERESIS
    { {UNITIZE(TQt::Key_Dead_Diaeresis), 0x0415, 0, 0, 0, 0}, 0x0401 }, // U0401 # CYRILLIC CAPITAL LETTER IO
    { {UNITIZE(TQt::Key_Dead_Diaeresis), 0x0416, 0, 0, 0, 0}, 0x04dc }, // U04DC # CYRILLIC CAPITAL LETTER ZHE WITH DIAERESIS
    { {UNITIZE(TQt::Key_Dead_Diaeresis), 0x0417, 0, 0, 0, 0}, 0x04de }, // U04DE # CYRILLIC CAPITAL LETTER ZE WITH DIAERESIS
    { {UNITIZE(TQt::Key_Dead_Diaeresis), 0x0418, 0, 0, 0, 0}, 0x04e4 }, // U04E4 # CYRILLIC CAPITAL LETTER I WITH DIAERESIS
    { {UNITIZE(TQt::Key_Dead_Diaeresis), 0x041e, 0, 0, 0, 0}, 0x04e6 }, // U04E6 # CYRILLIC CAPITAL LETTER O WITH DIAERESIS
    { {UNITIZE(TQt::Key_Dead_Diaeresis), 0x0423, 0, 0, 0, 0}, 0x04f0 }, // U04F0 # CYRILLIC CAPITAL LETTER U WITH DIAERESIS
    { {UNITIZE(TQt::Key_Dead_Diaeresis), 0x0427, 0, 0, 0, 0}, 0x04f4 }, // U04F4 # CYRILLIC CAPITAL LETTER CHE WITH DIAERESIS
    { {UNITIZE(TQt::Key_Dead_Diaeresis), 0x042b, 0, 0, 0, 0}, 0x04f8 }, // U04F8 # CYRILLIC CAPITAL LETTER YERU WITH DIAERESIS
    { {UNITIZE(TQt::Key_Dead_Diaeresis), 0x042d, 0, 0, 0, 0}, 0x04ec }, // U04EC # CYRILLIC CAPITAL LETTER E WITH DIAERESIS
    { {UNITIZE(TQt::Key_Dead_Diaeresis), 0x0430, 0, 0, 0, 0}, 0x04d3 }, // U04D3 # CYRILLIC SMALL LETTER A WITH DIAERESIS
    { {UNITIZE(TQt::Key_Dead_Diaeresis), 0x0435, 0, 0, 0, 0}, 0x0451 }, // U0451 # CYRILLIC SMALL LETTER IO
    { {UNITIZE(TQt::Key_Dead_Diaeresis), 0x0436, 0, 0, 0, 0}, 0x04dd }, // U04DD # CYRILLIC SMALL LETTER ZHE WITH DIAERESIS
    { {UNITIZE(TQt::Key_Dead_Diaeresis), 0x0437, 0, 0, 0, 0}, 0x04df }, // U04DF # CYRILLIC SMALL LETTER ZE WITH DIAERESIS
    { {UNITIZE(TQt::Key_Dead_Diaeresis), 0x0438, 0, 0, 0, 0}, 0x04e5 }, // U04E5 # CYRILLIC SMALL LETTER I WITH DIAERESIS
    { {UNITIZE(TQt::Key_Dead_Diaeresis), 0x043e, 0, 0, 0, 0}, 0x04e7 }, // U04E7 # CYRILLIC SMALL LETTER O WITH DIAERESIS
    { {UNITIZE(TQt::Key_Dead_Diaeresis), 0x0443, 0, 0, 0, 0}, 0x04f1 }, // U04F1 # CYRILLIC SMALL LETTER U WITH DIAERESIS
    { {UNITIZE(TQt::Key_Dead_Diaeresis), 0x0447, 0, 0, 0, 0}, 0x04f5 }, // U04F5 # CYRILLIC SMALL LETTER CHE WITH DIAERESIS
    { {UNITIZE(TQt::Key_Dead_Diaeresis), 0x044b, 0, 0, 0, 0}, 0x04f9 }, // U04F9 # CYRILLIC SMALL LETTER YERU WITH DIAERESIS
    { {UNITIZE(TQt::Key_Dead_Diaeresis), 0x044d, 0, 0, 0, 0}, 0x04ed }, // U04ED # CYRILLIC SMALL LETTER E WITH DIAERESIS
    { {UNITIZE(TQt::Key_Dead_Diaeresis), 0x0456, 0, 0, 0, 0}, 0x0457 }, // U0457 # CYRILLIC SMALL LETTER YI
    { {UNITIZE(TQt::Key_Dead_Diaeresis), 0x04d8, 0, 0, 0, 0}, 0x04da }, // U04DA # CYRILLIC CAPITAL LETTER SCHWA WITH DIAERESIS
    { {UNITIZE(TQt::Key_Dead_Diaeresis), 0x04d9, 0, 0, 0, 0}, 0x04db }, // U04DB # CYRILLIC SMALL LETTER SCHWA WITH DIAERESIS
    { {UNITIZE(TQt::Key_Dead_Diaeresis), 0x04e8, 0, 0, 0, 0}, 0x04ea }, // U04EA # CYRILLIC CAPITAL LETTER BARRED O WITH DIAERESIS
    { {UNITIZE(TQt::Key_Dead_Diaeresis), 0x04e9, 0, 0, 0, 0}, 0x04eb }, // U04EB # CYRILLIC SMALL LETTER BARRED O WITH DIAERESIS
    { {UNITIZE(TQt::Key_Dead_Diaeresis), UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0055, 0, 0}, 0x1e7a }, // U1E7A # LATIN CAPITAL LETTER U WITH MACRON AND DIAERESIS
    { {UNITIZE(TQt::Key_Dead_Diaeresis), UNITIZE(TQt::Key_Multi_key), 0x005f, 0x0075, 0, 0}, 0x1e7b }, // U1E7B # LATIN SMALL LETTER U WITH MACRON AND DIAERESIS
    { {UNITIZE(TQt::Key_Dead_Diaeresis), UNITIZE(TQt::Key_Multi_key), 0x007e, 0x004f, 0, 0}, 0x1e4e }, // U1E4E # LATIN CAPITAL LETTER O WITH TILDE AND DIAERESIS
    { {UNITIZE(TQt::Key_Dead_Diaeresis), UNITIZE(TQt::Key_Multi_key), 0x007e, 0x006f, 0, 0}, 0x1e4f }, // U1E4F # LATIN SMALL LETTER O WITH TILDE AND DIAERESIS
    { {UNITIZE(TQt::Key_Dead_Diaeresis), UNITIZE(TQt::Key_Multi_key), 0x00af, 0x0055, 0, 0}, 0x1e7a }, // U1E7A # LATIN CAPITAL LETTER U WITH MACRON AND DIAERESIS
    { {UNITIZE(TQt::Key_Dead_Diaeresis), UNITIZE(TQt::Key_Multi_key), 0x00af, 0x0075, 0, 0}, 0x1e7b }, // U1E7B # LATIN SMALL LETTER U WITH MACRON AND DIAERESIS
    { {UNITIZE(TQt::Key_Dead_Diaeresis), UNITIZE(TQt::Key_Dead_Tilde), 0x004f, 0, 0, 0}, 0x1e4e }, // U1E4E # LATIN CAPITAL LETTER O WITH TILDE AND DIAERESIS
    { {UNITIZE(TQt::Key_Dead_Diaeresis), UNITIZE(TQt::Key_Dead_Tilde), 0x006f, 0, 0, 0}, 0x1e4f }, // U1E4F # LATIN SMALL LETTER O WITH TILDE AND DIAERESIS
    { {UNITIZE(TQt::Key_Dead_Diaeresis), UNITIZE(TQt::Key_Dead_Macron), 0x0055, 0, 0, 0}, 0x1e7a }, // U1E7A # LATIN CAPITAL LETTER U WITH MACRON AND DIAERESIS
    { {UNITIZE(TQt::Key_Dead_Diaeresis), UNITIZE(TQt::Key_Dead_Macron), 0x0075, 0, 0, 0}, 0x1e7b }, // U1E7B # LATIN SMALL LETTER U WITH MACRON AND DIAERESIS
    { {UNITIZE(TQt::Key_Dead_Diaeresis), UNITIZE(TQt::Key_Dead_Diaeresis), 0, 0, 0, 0}, 0x00a8 }, // diaeresis
    { {UNITIZE(TQt::Key_Dead_Abovering), 0x0020, 0, 0, 0, 0}, 0x00b0 }, // degree
    { {UNITIZE(TQt::Key_Dead_Abovering), 0x0041, 0, 0, 0, 0}, 0x00c5 }, // U00C5 # LATIN CAPITAL LETTER A WITH RING ABOVE
    { {UNITIZE(TQt::Key_Dead_Abovering), 0x0055, 0, 0, 0, 0}, 0x016e }, // U016E # LATIN CAPITAL LETTER U WITH RING ABOVE
    { {UNITIZE(TQt::Key_Dead_Abovering), 0x0061, 0, 0, 0, 0}, 0x00e5 }, // U00E5 # LATIN SMALL LETTER A WITH RING ABOVE
    { {UNITIZE(TQt::Key_Dead_Abovering), 0x0075, 0, 0, 0, 0}, 0x016f }, // U016F # LATIN SMALL LETTER U WITH RING ABOVE
    { {UNITIZE(TQt::Key_Dead_Abovering), 0x0077, 0, 0, 0, 0}, 0x1e98 }, // U1E98 # LATIN SMALL LETTER W WITH RING ABOVE
    { {UNITIZE(TQt::Key_Dead_Abovering), 0x0079, 0, 0, 0, 0}, 0x1e99 }, // U1E99 # LATIN SMALL LETTER Y WITH RING ABOVE
    { {UNITIZE(TQt::Key_Dead_Abovering), UNITIZE(TQt::Key_Dead_Abovering), 0, 0, 0, 0}, 0x00b0 }, // degree
    { {UNITIZE(TQt::Key_Dead_Doubleacute), 0x0020, 0, 0, 0, 0}, 0x02dd }, // U2dd
    { {UNITIZE(TQt::Key_Dead_Doubleacute), 0x004f, 0, 0, 0, 0}, 0x0150 }, // U0150 # LATIN CAPITAL LETTER O WITH DOUBLE ACUTE
    { {UNITIZE(TQt::Key_Dead_Doubleacute), 0x0055, 0, 0, 0, 0}, 0x0170 }, // U0170 # LATIN CAPITAL LETTER U WITH DOUBLE ACUTE
    { {UNITIZE(TQt::Key_Dead_Doubleacute), 0x006f, 0, 0, 0, 0}, 0x0151 }, // U0151 # LATIN SMALL LETTER O WITH DOUBLE ACUTE
    { {UNITIZE(TQt::Key_Dead_Doubleacute), 0x0075, 0, 0, 0, 0}, 0x0171 }, // U0171 # LATIN SMALL LETTER U WITH DOUBLE ACUTE
    { {UNITIZE(TQt::Key_Dead_Doubleacute), 0x0423, 0, 0, 0, 0}, 0x04f2 }, // U04F2 # CYRILLIC CAPITAL LETTER U WITH DOUBLE ACUTE
    { {UNITIZE(TQt::Key_Dead_Doubleacute), 0x0443, 0, 0, 0, 0}, 0x04f3 }, // U04F3 # CYRILLIC SMALL LETTER U WITH DOUBLE ACUTE
    { {UNITIZE(TQt::Key_Dead_Doubleacute), UNITIZE(TQt::Key_Dead_Doubleacute), 0, 0, 0, 0}, 0x02dd }, // U2dd
    { {UNITIZE(TQt::Key_Dead_Caron), 0x0020, 0, 0, 0, 0}, 0x02c7 }, // caron
    { {UNITIZE(TQt::Key_Dead_Caron), 0x0041, 0, 0, 0, 0}, 0x01cd }, // U01CD # LATIN CAPITAL LETTER A WITH CARON
    { {UNITIZE(TQt::Key_Dead_Caron), 0x0043, 0, 0, 0, 0}, 0x010c }, // U010C # LATIN CAPITAL LETTER C WITH CARON
    { {UNITIZE(TQt::Key_Dead_Caron), 0x0044, 0, 0, 0, 0}, 0x010e }, // U010E # LATIN CAPITAL LETTER D WITH CARON
    { {UNITIZE(TQt::Key_Dead_Caron), 0x0045, 0, 0, 0, 0}, 0x011a }, // U011A # LATIN CAPITAL LETTER E WITH CARON
    { {UNITIZE(TQt::Key_Dead_Caron), 0x0047, 0, 0, 0, 0}, 0x01e6 }, // U01E6 # LATIN CAPITAL LETTER G WITH CARON
    { {UNITIZE(TQt::Key_Dead_Caron), 0x0048, 0, 0, 0, 0}, 0x021e }, // U021E # LATIN CAPITAL LETTER H WITH CARON
    { {UNITIZE(TQt::Key_Dead_Caron), 0x0049, 0, 0, 0, 0}, 0x01cf }, // U01CF # LATIN CAPITAL LETTER I WITH CARON
    { {UNITIZE(TQt::Key_Dead_Caron), 0x004b, 0, 0, 0, 0}, 0x01e8 }, // U01E8 # LATIN CAPITAL LETTER K WITH CARON
    { {UNITIZE(TQt::Key_Dead_Caron), 0x004c, 0, 0, 0, 0}, 0x013d }, // U013D # LATIN CAPITAL LETTER L WITH CARON
    { {UNITIZE(TQt::Key_Dead_Caron), 0x004e, 0, 0, 0, 0}, 0x0147 }, // U0147 # LATIN CAPITAL LETTER N WITH CARON
    { {UNITIZE(TQt::Key_Dead_Caron), 0x004f, 0, 0, 0, 0}, 0x01d1 }, // U01D1 # LATIN CAPITAL LETTER O WITH CARON
    { {UNITIZE(TQt::Key_Dead_Caron), 0x0052, 0, 0, 0, 0}, 0x0158 }, // U0158 # LATIN CAPITAL LETTER R WITH CARON
    { {UNITIZE(TQt::Key_Dead_Caron), 0x0053, 0, 0, 0, 0}, 0x0160 }, // U0160 # LATIN CAPITAL LETTER S WITH CARON
    { {UNITIZE(TQt::Key_Dead_Caron), 0x0054, 0, 0, 0, 0}, 0x0164 }, // U0164 # LATIN CAPITAL LETTER T WITH CARON
    { {UNITIZE(TQt::Key_Dead_Caron), 0x0055, 0, 0, 0, 0}, 0x01d3 }, // U01D3 # LATIN CAPITAL LETTER U WITH CARON
    { {UNITIZE(TQt::Key_Dead_Caron), 0x005a, 0, 0, 0, 0}, 0x017d }, // U017D # LATIN CAPITAL LETTER Z WITH CARON
    { {UNITIZE(TQt::Key_Dead_Caron), 0x0061, 0, 0, 0, 0}, 0x01ce }, // U01CE # LATIN SMALL LETTER A WITH CARON
    { {UNITIZE(TQt::Key_Dead_Caron), 0x0063, 0, 0, 0, 0}, 0x010d }, // U010D # LATIN SMALL LETTER C WITH CARON
    { {UNITIZE(TQt::Key_Dead_Caron), 0x0064, 0, 0, 0, 0}, 0x010f }, // U010F # LATIN SMALL LETTER D WITH CARON
    { {UNITIZE(TQt::Key_Dead_Caron), 0x0065, 0, 0, 0, 0}, 0x011b }, // U011B # LATIN SMALL LETTER E WITH CARON
    { {UNITIZE(TQt::Key_Dead_Caron), 0x0067, 0, 0, 0, 0}, 0x01e7 }, // U01E7 # LATIN SMALL LETTER G WITH CARON
    { {UNITIZE(TQt::Key_Dead_Caron), 0x0068, 0, 0, 0, 0}, 0x021f }, // U021F # LATIN SMALL LETTER H WITH CARON
    { {UNITIZE(TQt::Key_Dead_Caron), 0x0069, 0, 0, 0, 0}, 0x01d0 }, // U01D0 # LATIN SMALL LETTER I WITH CARON
    { {UNITIZE(TQt::Key_Dead_Caron), 0x006a, 0, 0, 0, 0}, 0x01f0 }, // U01F0 # LATIN SMALL LETTER J WITH CARON
    { {UNITIZE(TQt::Key_Dead_Caron), 0x006b, 0, 0, 0, 0}, 0x01e9 }, // U01E9 # LATIN SMALL LETTER K WITH CARON
    { {UNITIZE(TQt::Key_Dead_Caron), 0x006c, 0, 0, 0, 0}, 0x013e }, // U013E # LATIN SMALL LETTER L WITH CARON
    { {UNITIZE(TQt::Key_Dead_Caron), 0x006e, 0, 0, 0, 0}, 0x0148 }, // U0148 # LATIN SMALL LETTER N WITH CARON
    { {UNITIZE(TQt::Key_Dead_Caron), 0x006f, 0, 0, 0, 0}, 0x01d2 }, // U01D2 # LATIN SMALL LETTER O WITH CARON
    { {UNITIZE(TQt::Key_Dead_Caron), 0x0072, 0, 0, 0, 0}, 0x0159 }, // U0159 # LATIN SMALL LETTER R WITH CARON
    { {UNITIZE(TQt::Key_Dead_Caron), 0x0073, 0, 0, 0, 0}, 0x0161 }, // U0161 # LATIN SMALL LETTER S WITH CARON
    { {UNITIZE(TQt::Key_Dead_Caron), 0x0074, 0, 0, 0, 0}, 0x0165 }, // U0165 # LATIN SMALL LETTER T WITH CARON
    { {UNITIZE(TQt::Key_Dead_Caron), 0x0075, 0, 0, 0, 0}, 0x01d4 }, // U01D4 # LATIN SMALL LETTER U WITH CARON
    { {UNITIZE(TQt::Key_Dead_Caron), 0x007a, 0, 0, 0, 0}, 0x017e }, // U017E # LATIN SMALL LETTER Z WITH CARON
    { {UNITIZE(TQt::Key_Dead_Caron), 0x00dc, 0, 0, 0, 0}, 0x01d9 }, // U01D9 # LATIN CAPITAL LETTER U WITH DIAERESIS AND CARON
    { {UNITIZE(TQt::Key_Dead_Caron), 0x00fc, 0, 0, 0, 0}, 0x01da }, // U01DA # LATIN SMALL LETTER U WITH DIAERESIS AND CARON
    { {UNITIZE(TQt::Key_Dead_Caron), 0x01b7, 0, 0, 0, 0}, 0x01ee }, // U01EE # LATIN CAPITAL LETTER EZH WITH CARON
    { {UNITIZE(TQt::Key_Dead_Caron), 0x0292, 0, 0, 0, 0}, 0x01ef }, // U01EF # LATIN SMALL LETTER EZH WITH CARON
    { {UNITIZE(TQt::Key_Dead_Caron), UNITIZE(TQt::Key_Multi_key), 0x0022, 0x0055, 0, 0}, 0x01d9 }, // U01D9 # LATIN CAPITAL LETTER U WITH DIAERESIS AND CARON
    { {UNITIZE(TQt::Key_Dead_Caron), UNITIZE(TQt::Key_Multi_key), 0x0022, 0x0075, 0, 0}, 0x01da }, // U01DA # LATIN SMALL LETTER U WITH DIAERESIS AND CARON
    { {UNITIZE(TQt::Key_Dead_Caron), UNITIZE(TQt::Key_Dead_Diaeresis), 0x0055, 0, 0, 0}, 0x01d9 }, // U01D9 # LATIN CAPITAL LETTER U WITH DIAERESIS AND CARON
    { {UNITIZE(TQt::Key_Dead_Caron), UNITIZE(TQt::Key_Dead_Diaeresis), 0x0075, 0, 0, 0}, 0x01da }, // U01DA # LATIN SMALL LETTER U WITH DIAERESIS AND CARON
    { {UNITIZE(TQt::Key_Dead_Caron), UNITIZE(TQt::Key_Dead_Caron), 0, 0, 0, 0}, 0x02c7 }, // caron
    { {UNITIZE(TQt::Key_Dead_Cedilla), 0x0020, 0, 0, 0, 0}, 0x00b8 }, // cedilla
    { {UNITIZE(TQt::Key_Dead_Cedilla), 0x0043, 0, 0, 0, 0}, 0x00c7 }, // U00C7 # LATIN CAPITAL LETTER C WITH CEDILLA
    { {UNITIZE(TQt::Key_Dead_Cedilla), 0x0044, 0, 0, 0, 0}, 0x1e10 }, // U1E10 # LATIN CAPITAL LETTER D WITH CEDILLA
    { {UNITIZE(TQt::Key_Dead_Cedilla), 0x0045, 0, 0, 0, 0}, 0x0228 }, // U0228 # LATIN CAPITAL LETTER E WITH CEDILLA
    { {UNITIZE(TQt::Key_Dead_Cedilla), 0x0047, 0, 0, 0, 0}, 0x0122 }, // U0122 # LATIN CAPITAL LETTER G WITH CEDILLA
    { {UNITIZE(TQt::Key_Dead_Cedilla), 0x0048, 0, 0, 0, 0}, 0x1e28 }, // U1E28 # LATIN CAPITAL LETTER H WITH CEDILLA
    { {UNITIZE(TQt::Key_Dead_Cedilla), 0x004b, 0, 0, 0, 0}, 0x0136 }, // U0136 # LATIN CAPITAL LETTER K WITH CEDILLA
    { {UNITIZE(TQt::Key_Dead_Cedilla), 0x004c, 0, 0, 0, 0}, 0x013b }, // U013B # LATIN CAPITAL LETTER L WITH CEDILLA
    { {UNITIZE(TQt::Key_Dead_Cedilla), 0x004e, 0, 0, 0, 0}, 0x0145 }, // U0145 # LATIN CAPITAL LETTER N WITH CEDILLA
    { {UNITIZE(TQt::Key_Dead_Cedilla), 0x0052, 0, 0, 0, 0}, 0x0156 }, // U0156 # LATIN CAPITAL LETTER R WITH CEDILLA
    { {UNITIZE(TQt::Key_Dead_Cedilla), 0x0053, 0, 0, 0, 0}, 0x015e }, // U015E # LATIN CAPITAL LETTER S WITH CEDILLA
    { {UNITIZE(TQt::Key_Dead_Cedilla), 0x0054, 0, 0, 0, 0}, 0x0162 }, // U0162 # LATIN CAPITAL LETTER T WITH CEDILLA
    { {UNITIZE(TQt::Key_Dead_Cedilla), 0x0063, 0, 0, 0, 0}, 0x00e7 }, // U00E7 # LATIN SMALL LETTER C WITH CEDILLA
    { {UNITIZE(TQt::Key_Dead_Cedilla), 0x0064, 0, 0, 0, 0}, 0x1e11 }, // U1E11 # LATIN SMALL LETTER D WITH CEDILLA
    { {UNITIZE(TQt::Key_Dead_Cedilla), 0x0065, 0, 0, 0, 0}, 0x0229 }, // U0229 # LATIN SMALL LETTER E WITH CEDILLA
    { {UNITIZE(TQt::Key_Dead_Cedilla), 0x0067, 0, 0, 0, 0}, 0x0123 }, // U0123 # LATIN SMALL LETTER G WITH CEDILLA
    { {UNITIZE(TQt::Key_Dead_Cedilla), 0x0068, 0, 0, 0, 0}, 0x1e29 }, // U1E29 # LATIN SMALL LETTER H WITH CEDILLA
    { {UNITIZE(TQt::Key_Dead_Cedilla), 0x006b, 0, 0, 0, 0}, 0x0137 }, // U0137 # LATIN SMALL LETTER K WITH CEDILLA
    { {UNITIZE(TQt::Key_Dead_Cedilla), 0x006c, 0, 0, 0, 0}, 0x013c }, // U013C # LATIN SMALL LETTER L WITH CEDILLA
    { {UNITIZE(TQt::Key_Dead_Cedilla), 0x006e, 0, 0, 0, 0}, 0x0146 }, // U0146 # LATIN SMALL LETTER N WITH CEDILLA
    { {UNITIZE(TQt::Key_Dead_Cedilla), 0x0072, 0, 0, 0, 0}, 0x0157 }, // U0157 # LATIN SMALL LETTER R WITH CEDILLA
    { {UNITIZE(TQt::Key_Dead_Cedilla), 0x0073, 0, 0, 0, 0}, 0x015f }, // U015F # LATIN SMALL LETTER S WITH CEDILLA
    { {UNITIZE(TQt::Key_Dead_Cedilla), 0x0074, 0, 0, 0, 0}, 0x0163 }, // U0163 # LATIN SMALL LETTER T WITH CEDILLA
    { {UNITIZE(TQt::Key_Dead_Cedilla), UNITIZE(TQt::Key_Dead_Cedilla), 0, 0, 0, 0}, 0x00b8 }, // cedilla
    { {UNITIZE(TQt::Key_Dead_Ogonek), 0x0041, 0, 0, 0, 0}, 0x0104 }, // U0104 # LATIN CAPITAL LETTER A WITH OGONEK
    { {UNITIZE(TQt::Key_Dead_Ogonek), 0x0045, 0, 0, 0, 0}, 0x0118 }, // U0118 # LATIN CAPITAL LETTER E WITH OGONEK
    { {UNITIZE(TQt::Key_Dead_Ogonek), 0x0049, 0, 0, 0, 0}, 0x012e }, // U012E # LATIN CAPITAL LETTER I WITH OGONEK
    { {UNITIZE(TQt::Key_Dead_Ogonek), 0x004f, 0, 0, 0, 0}, 0x01ea }, // U01EA # LATIN CAPITAL LETTER O WITH OGONEK
    { {UNITIZE(TQt::Key_Dead_Ogonek), 0x0055, 0, 0, 0, 0}, 0x0172 }, // U0172 # LATIN CAPITAL LETTER U WITH OGONEK
    { {UNITIZE(TQt::Key_Dead_Ogonek), 0x0061, 0, 0, 0, 0}, 0x0105 }, // U0105 # LATIN SMALL LETTER A WITH OGONEK
    { {UNITIZE(TQt::Key_Dead_Ogonek), 0x0065, 0, 0, 0, 0}, 0x0119 }, // U0119 # LATIN SMALL LETTER E WITH OGONEK
    { {UNITIZE(TQt::Key_Dead_Ogonek), 0x0069, 0, 0, 0, 0}, 0x012f }, // U012F # LATIN SMALL LETTER I WITH OGONEK
    { {UNITIZE(TQt::Key_Dead_Ogonek), 0x006f, 0, 0, 0, 0}, 0x01eb }, // U01EB # LATIN SMALL LETTER O WITH OGONEK
    { {UNITIZE(TQt::Key_Dead_Ogonek), 0x0075, 0, 0, 0, 0}, 0x0173 }, // U0173 # LATIN SMALL LETTER U WITH OGONEK
    { {UNITIZE(TQt::Key_Dead_Ogonek), UNITIZE(TQt::Key_Dead_Ogonek), 0, 0, 0, 0}, 0x02db }, // ogonek
// /* broken */    { {UNITIZE(TQt::Key_Dead_Ogonek), dead_space, 0, 0, 0, 0}, 0x02db }, // ogonek
// /* broken */    { {UNITIZE(TQt::Key_Dead_Iota), 0x0000, 0x0313, 0x0391, 0, 0}, 0x1f8c }, // U1F8C # GREEK CAPITAL LETTER ALPHA WITH PSILI AND OXIA AND PROSGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Dead_Iota), 0x0000, 0x0313, 0x0391, 0, 0}, 0x1f8a }, // U1F8A # GREEK CAPITAL LETTER ALPHA WITH PSILI AND VARIA AND PROSGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Dead_Iota), 0x0000, 0x0313, 0x0397, 0, 0}, 0x1f9c }, // U1F9C # GREEK CAPITAL LETTER ETA WITH PSILI AND OXIA AND PROSGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Dead_Iota), 0x0000, 0x0313, 0x0397, 0, 0}, 0x1f9a }, // U1F9A # GREEK CAPITAL LETTER ETA WITH PSILI AND VARIA AND PROSGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Dead_Iota), 0x0000, 0x0313, 0x03a9, 0, 0}, 0x1faa }, // U1FAA # GREEK CAPITAL LETTER OMEGA WITH PSILI AND VARIA AND PROSGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Dead_Iota), 0x0000, 0x0313, 0x03a9, 0, 0}, 0x1fac }, // U1FAC # GREEK CAPITAL LETTER OMEGA WITH PSILI AND OXIA AND PROSGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Dead_Iota), 0x0000, 0x0313, 0x03b1, 0, 0}, 0x1f82 }, // U1F82 # GREEK SMALL LETTER ALPHA WITH PSILI AND VARIA AND YPOGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Dead_Iota), 0x0000, 0x0313, 0x03b1, 0, 0}, 0x1f84 }, // U1F84 # GREEK SMALL LETTER ALPHA WITH PSILI AND OXIA AND YPOGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Dead_Iota), 0x0000, 0x0313, 0x03b7, 0, 0}, 0x1f94 }, // U1F94 # GREEK SMALL LETTER ETA WITH PSILI AND OXIA AND YPOGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Dead_Iota), 0x0000, 0x0313, 0x03b7, 0, 0}, 0x1f92 }, // U1F92 # GREEK SMALL LETTER ETA WITH PSILI AND VARIA AND YPOGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Dead_Iota), 0x0000, 0x0313, 0x03c9, 0, 0}, 0x1fa2 }, // U1FA2 # GREEK SMALL LETTER OMEGA WITH PSILI AND VARIA AND YPOGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Dead_Iota), 0x0000, 0x0313, 0x03c9, 0, 0}, 0x1fa4 }, // U1FA4 # GREEK SMALL LETTER OMEGA WITH PSILI AND OXIA AND YPOGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Dead_Iota), 0x0000, 0x0314, 0x0391, 0, 0}, 0x1f8b }, // U1F8B # GREEK CAPITAL LETTER ALPHA WITH DASIA AND VARIA AND PROSGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Dead_Iota), 0x0000, 0x0314, 0x0391, 0, 0}, 0x1f8d }, // U1F8D # GREEK CAPITAL LETTER ALPHA WITH DASIA AND OXIA AND PROSGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Dead_Iota), 0x0000, 0x0314, 0x0397, 0, 0}, 0x1f9b }, // U1F9B # GREEK CAPITAL LETTER ETA WITH DASIA AND VARIA AND PROSGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Dead_Iota), 0x0000, 0x0314, 0x0397, 0, 0}, 0x1f9d }, // U1F9D # GREEK CAPITAL LETTER ETA WITH DASIA AND OXIA AND PROSGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Dead_Iota), 0x0000, 0x0314, 0x03a9, 0, 0}, 0x1fab }, // U1FAB # GREEK CAPITAL LETTER OMEGA WITH DASIA AND VARIA AND PROSGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Dead_Iota), 0x0000, 0x0314, 0x03a9, 0, 0}, 0x1fad }, // U1FAD # GREEK CAPITAL LETTER OMEGA WITH DASIA AND OXIA AND PROSGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Dead_Iota), 0x0000, 0x0314, 0x03b1, 0, 0}, 0x1f83 }, // U1F83 # GREEK SMALL LETTER ALPHA WITH DASIA AND VARIA AND YPOGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Dead_Iota), 0x0000, 0x0314, 0x03b1, 0, 0}, 0x1f85 }, // U1F85 # GREEK SMALL LETTER ALPHA WITH DASIA AND OXIA AND YPOGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Dead_Iota), 0x0000, 0x0314, 0x03b7, 0, 0}, 0x1f95 }, // U1F95 # GREEK SMALL LETTER ETA WITH DASIA AND OXIA AND YPOGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Dead_Iota), 0x0000, 0x0314, 0x03b7, 0, 0}, 0x1f93 }, // U1F93 # GREEK SMALL LETTER ETA WITH DASIA AND VARIA AND YPOGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Dead_Iota), 0x0000, 0x0314, 0x03c9, 0, 0}, 0x1fa5 }, // U1FA5 # GREEK SMALL LETTER OMEGA WITH DASIA AND OXIA AND YPOGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Dead_Iota), 0x0000, 0x0314, 0x03c9, 0, 0}, 0x1fa3 }, // U1FA3 # GREEK SMALL LETTER OMEGA WITH DASIA AND VARIA AND YPOGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Dead_Iota), 0x0000, 0x03b1, 0, 0, 0}, 0x1fb4 }, // U1FB4 # GREEK SMALL LETTER ALPHA WITH OXIA AND YPOGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Dead_Iota), 0x0000, 0x03b1, 0, 0, 0}, 0x1fb2 }, // U1FB2 # GREEK SMALL LETTER ALPHA WITH VARIA AND YPOGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Dead_Iota), 0x0000, 0x03b7, 0, 0, 0}, 0x1fc4 }, // U1FC4 # GREEK SMALL LETTER ETA WITH OXIA AND YPOGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Dead_Iota), 0x0000, 0x03b7, 0, 0, 0}, 0x1fc2 }, // U1FC2 # GREEK SMALL LETTER ETA WITH VARIA AND YPOGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Dead_Iota), 0x0000, 0x03c9, 0, 0, 0}, 0x1ff4 }, // U1FF4 # GREEK SMALL LETTER OMEGA WITH OXIA AND YPOGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Dead_Iota), 0x0000, 0x03c9, 0, 0, 0}, 0x1ff2 }, // U1FF2 # GREEK SMALL LETTER OMEGA WITH VARIA AND YPOGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Dead_Iota), 0x0000, 0x1f00, 0, 0, 0}, 0x1f84 }, // U1F84 # GREEK SMALL LETTER ALPHA WITH PSILI AND OXIA AND YPOGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Dead_Iota), 0x0000, 0x1f00, 0, 0, 0}, 0x1f82 }, // U1F82 # GREEK SMALL LETTER ALPHA WITH PSILI AND VARIA AND YPOGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Dead_Iota), 0x0000, 0x1f01, 0, 0, 0}, 0x1f83 }, // U1F83 # GREEK SMALL LETTER ALPHA WITH DASIA AND VARIA AND YPOGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Dead_Iota), 0x0000, 0x1f01, 0, 0, 0}, 0x1f85 }, // U1F85 # GREEK SMALL LETTER ALPHA WITH DASIA AND OXIA AND YPOGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Dead_Iota), 0x0000, 0x1f08, 0, 0, 0}, 0x1f8a }, // U1F8A # GREEK CAPITAL LETTER ALPHA WITH PSILI AND VARIA AND PROSGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Dead_Iota), 0x0000, 0x1f08, 0, 0, 0}, 0x1f8c }, // U1F8C # GREEK CAPITAL LETTER ALPHA WITH PSILI AND OXIA AND PROSGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Dead_Iota), 0x0000, 0x1f09, 0, 0, 0}, 0x1f8b }, // U1F8B # GREEK CAPITAL LETTER ALPHA WITH DASIA AND VARIA AND PROSGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Dead_Iota), 0x0000, 0x1f09, 0, 0, 0}, 0x1f8d }, // U1F8D # GREEK CAPITAL LETTER ALPHA WITH DASIA AND OXIA AND PROSGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Dead_Iota), 0x0000, 0x1f20, 0, 0, 0}, 0x1f94 }, // U1F94 # GREEK SMALL LETTER ETA WITH PSILI AND OXIA AND YPOGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Dead_Iota), 0x0000, 0x1f20, 0, 0, 0}, 0x1f92 }, // U1F92 # GREEK SMALL LETTER ETA WITH PSILI AND VARIA AND YPOGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Dead_Iota), 0x0000, 0x1f21, 0, 0, 0}, 0x1f93 }, // U1F93 # GREEK SMALL LETTER ETA WITH DASIA AND VARIA AND YPOGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Dead_Iota), 0x0000, 0x1f21, 0, 0, 0}, 0x1f95 }, // U1F95 # GREEK SMALL LETTER ETA WITH DASIA AND OXIA AND YPOGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Dead_Iota), 0x0000, 0x1f28, 0, 0, 0}, 0x1f9c }, // U1F9C # GREEK CAPITAL LETTER ETA WITH PSILI AND OXIA AND PROSGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Dead_Iota), 0x0000, 0x1f28, 0, 0, 0}, 0x1f9a }, // U1F9A # GREEK CAPITAL LETTER ETA WITH PSILI AND VARIA AND PROSGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Dead_Iota), 0x0000, 0x1f29, 0, 0, 0}, 0x1f9d }, // U1F9D # GREEK CAPITAL LETTER ETA WITH DASIA AND OXIA AND PROSGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Dead_Iota), 0x0000, 0x1f29, 0, 0, 0}, 0x1f9b }, // U1F9B # GREEK CAPITAL LETTER ETA WITH DASIA AND VARIA AND PROSGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Dead_Iota), 0x0000, 0x1f60, 0, 0, 0}, 0x1fa2 }, // U1FA2 # GREEK SMALL LETTER OMEGA WITH PSILI AND VARIA AND YPOGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Dead_Iota), 0x0000, 0x1f60, 0, 0, 0}, 0x1fa4 }, // U1FA4 # GREEK SMALL LETTER OMEGA WITH PSILI AND OXIA AND YPOGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Dead_Iota), 0x0000, 0x1f61, 0, 0, 0}, 0x1fa5 }, // U1FA5 # GREEK SMALL LETTER OMEGA WITH DASIA AND OXIA AND YPOGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Dead_Iota), 0x0000, 0x1f61, 0, 0, 0}, 0x1fa3 }, // U1FA3 # GREEK SMALL LETTER OMEGA WITH DASIA AND VARIA AND YPOGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Dead_Iota), 0x0000, 0x1f68, 0, 0, 0}, 0x1fac }, // U1FAC # GREEK CAPITAL LETTER OMEGA WITH PSILI AND OXIA AND PROSGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Dead_Iota), 0x0000, 0x1f68, 0, 0, 0}, 0x1faa }, // U1FAA # GREEK CAPITAL LETTER OMEGA WITH PSILI AND VARIA AND PROSGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Dead_Iota), 0x0000, 0x1f69, 0, 0, 0}, 0x1fad }, // U1FAD # GREEK CAPITAL LETTER OMEGA WITH DASIA AND OXIA AND PROSGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Dead_Iota), 0x0000, 0x1f69, 0, 0, 0}, 0x1fab }, // U1FAB # GREEK CAPITAL LETTER OMEGA WITH DASIA AND VARIA AND PROSGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Dead_Iota), 0x0000, UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0391, 0}, 0x1f8d }, // U1F8D # GREEK CAPITAL LETTER ALPHA WITH DASIA AND OXIA AND PROSGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Dead_Iota), 0x0000, UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0391, 0}, 0x1f8b }, // U1F8B # GREEK CAPITAL LETTER ALPHA WITH DASIA AND VARIA AND PROSGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Dead_Iota), 0x0000, UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0397, 0}, 0x1f9d }, // U1F9D # GREEK CAPITAL LETTER ETA WITH DASIA AND OXIA AND PROSGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Dead_Iota), 0x0000, UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0397, 0}, 0x1f9b }, // U1F9B # GREEK CAPITAL LETTER ETA WITH DASIA AND VARIA AND PROSGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Dead_Iota), 0x0000, UNITIZE(TQt::Key_Multi_key), 0x0028, 0x03a9, 0}, 0x1fab }, // U1FAB # GREEK CAPITAL LETTER OMEGA WITH DASIA AND VARIA AND PROSGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Dead_Iota), 0x0000, UNITIZE(TQt::Key_Multi_key), 0x0028, 0x03a9, 0}, 0x1fad }, // U1FAD # GREEK CAPITAL LETTER OMEGA WITH DASIA AND OXIA AND PROSGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Dead_Iota), 0x0000, UNITIZE(TQt::Key_Multi_key), 0x0028, 0x03b1, 0}, 0x1f85 }, // U1F85 # GREEK SMALL LETTER ALPHA WITH DASIA AND OXIA AND YPOGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Dead_Iota), 0x0000, UNITIZE(TQt::Key_Multi_key), 0x0028, 0x03b1, 0}, 0x1f83 }, // U1F83 # GREEK SMALL LETTER ALPHA WITH DASIA AND VARIA AND YPOGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Dead_Iota), 0x0000, UNITIZE(TQt::Key_Multi_key), 0x0028, 0x03b7, 0}, 0x1f95 }, // U1F95 # GREEK SMALL LETTER ETA WITH DASIA AND OXIA AND YPOGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Dead_Iota), 0x0000, UNITIZE(TQt::Key_Multi_key), 0x0028, 0x03b7, 0}, 0x1f93 }, // U1F93 # GREEK SMALL LETTER ETA WITH DASIA AND VARIA AND YPOGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Dead_Iota), 0x0000, UNITIZE(TQt::Key_Multi_key), 0x0028, 0x03c9, 0}, 0x1fa3 }, // U1FA3 # GREEK SMALL LETTER OMEGA WITH DASIA AND VARIA AND YPOGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Dead_Iota), 0x0000, UNITIZE(TQt::Key_Multi_key), 0x0028, 0x03c9, 0}, 0x1fa5 }, // U1FA5 # GREEK SMALL LETTER OMEGA WITH DASIA AND OXIA AND YPOGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Dead_Iota), 0x0000, UNITIZE(TQt::Key_Multi_key), 0x0029, 0x0391, 0}, 0x1f8a }, // U1F8A # GREEK CAPITAL LETTER ALPHA WITH PSILI AND VARIA AND PROSGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Dead_Iota), 0x0000, UNITIZE(TQt::Key_Multi_key), 0x0029, 0x0391, 0}, 0x1f8c }, // U1F8C # GREEK CAPITAL LETTER ALPHA WITH PSILI AND OXIA AND PROSGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Dead_Iota), 0x0000, UNITIZE(TQt::Key_Multi_key), 0x0029, 0x0397, 0}, 0x1f9a }, // U1F9A # GREEK CAPITAL LETTER ETA WITH PSILI AND VARIA AND PROSGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Dead_Iota), 0x0000, UNITIZE(TQt::Key_Multi_key), 0x0029, 0x0397, 0}, 0x1f9c }, // U1F9C # GREEK CAPITAL LETTER ETA WITH PSILI AND OXIA AND PROSGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Dead_Iota), 0x0000, UNITIZE(TQt::Key_Multi_key), 0x0029, 0x03a9, 0}, 0x1fac }, // U1FAC # GREEK CAPITAL LETTER OMEGA WITH PSILI AND OXIA AND PROSGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Dead_Iota), 0x0000, UNITIZE(TQt::Key_Multi_key), 0x0029, 0x03a9, 0}, 0x1faa }, // U1FAA # GREEK CAPITAL LETTER OMEGA WITH PSILI AND VARIA AND PROSGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Dead_Iota), 0x0000, UNITIZE(TQt::Key_Multi_key), 0x0029, 0x03b1, 0}, 0x1f84 }, // U1F84 # GREEK SMALL LETTER ALPHA WITH PSILI AND OXIA AND YPOGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Dead_Iota), 0x0000, UNITIZE(TQt::Key_Multi_key), 0x0029, 0x03b1, 0}, 0x1f82 }, // U1F82 # GREEK SMALL LETTER ALPHA WITH PSILI AND VARIA AND YPOGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Dead_Iota), 0x0000, UNITIZE(TQt::Key_Multi_key), 0x0029, 0x03b7, 0}, 0x1f94 }, // U1F94 # GREEK SMALL LETTER ETA WITH PSILI AND OXIA AND YPOGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Dead_Iota), 0x0000, UNITIZE(TQt::Key_Multi_key), 0x0029, 0x03b7, 0}, 0x1f92 }, // U1F92 # GREEK SMALL LETTER ETA WITH PSILI AND VARIA AND YPOGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Dead_Iota), 0x0000, UNITIZE(TQt::Key_Multi_key), 0x0029, 0x03c9, 0}, 0x1fa2 }, // U1FA2 # GREEK SMALL LETTER OMEGA WITH PSILI AND VARIA AND YPOGEGRAMMENI
// /* broken */    { {UNITIZE(TQt::Key_Dead_Iota), 0x0000, UNITIZE(TQt::Key_Multi_key), 0x0029, 0x03c9, 0}, 0x1fa4 }, // U1FA4 # GREEK SMALL LETTER OMEGA WITH PSILI AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x0313, 0x0391, 0, 0, 0}, 0x1f88 }, // U1F88 # GREEK CAPITAL LETTER ALPHA WITH PSILI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x0313, 0x0397, 0, 0, 0}, 0x1f98 }, // U1F98 # GREEK CAPITAL LETTER ETA WITH PSILI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x0313, 0x03a9, 0, 0, 0}, 0x1fa8 }, // U1FA8 # GREEK CAPITAL LETTER OMEGA WITH PSILI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x0313, 0x03b1, 0, 0, 0}, 0x1f80 }, // U1F80 # GREEK SMALL LETTER ALPHA WITH PSILI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x0313, 0x03b7, 0, 0, 0}, 0x1f90 }, // U1F90 # GREEK SMALL LETTER ETA WITH PSILI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x0313, 0x03c9, 0, 0, 0}, 0x1fa0 }, // U1FA0 # GREEK SMALL LETTER OMEGA WITH PSILI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x0314, 0x0391, 0, 0, 0}, 0x1f89 }, // U1F89 # GREEK CAPITAL LETTER ALPHA WITH DASIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x0314, 0x0397, 0, 0, 0}, 0x1f99 }, // U1F99 # GREEK CAPITAL LETTER ETA WITH DASIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x0314, 0x03a9, 0, 0, 0}, 0x1fa9 }, // U1FA9 # GREEK CAPITAL LETTER OMEGA WITH DASIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x0314, 0x03b1, 0, 0, 0}, 0x1f81 }, // U1F81 # GREEK SMALL LETTER ALPHA WITH DASIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x0314, 0x03b7, 0, 0, 0}, 0x1f91 }, // U1F91 # GREEK SMALL LETTER ETA WITH DASIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x0314, 0x03c9, 0, 0, 0}, 0x1fa1 }, // U1FA1 # GREEK SMALL LETTER OMEGA WITH DASIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x0342, 0x0313, 0x0391, 0, 0}, 0x1f8e }, // U1F8E # GREEK CAPITAL LETTER ALPHA WITH PSILI AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x0342, 0x0313, 0x0397, 0, 0}, 0x1f9e }, // U1F9E # GREEK CAPITAL LETTER ETA WITH PSILI AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x0342, 0x0313, 0x03a9, 0, 0}, 0x1fae }, // U1FAE # GREEK CAPITAL LETTER OMEGA WITH PSILI AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x0342, 0x0313, 0x03b1, 0, 0}, 0x1f86 }, // U1F86 # GREEK SMALL LETTER ALPHA WITH PSILI AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x0342, 0x0313, 0x03b7, 0, 0}, 0x1f96 }, // U1F96 # GREEK SMALL LETTER ETA WITH PSILI AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x0342, 0x0313, 0x03c9, 0, 0}, 0x1fa6 }, // U1FA6 # GREEK SMALL LETTER OMEGA WITH PSILI AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x0342, 0x0314, 0x0391, 0, 0}, 0x1f8f }, // U1F8F # GREEK CAPITAL LETTER ALPHA WITH DASIA AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x0342, 0x0314, 0x0397, 0, 0}, 0x1f9f }, // U1F9F # GREEK CAPITAL LETTER ETA WITH DASIA AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x0342, 0x0314, 0x03a9, 0, 0}, 0x1faf }, // U1FAF # GREEK CAPITAL LETTER OMEGA WITH DASIA AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x0342, 0x0314, 0x03b1, 0, 0}, 0x1f87 }, // U1F87 # GREEK SMALL LETTER ALPHA WITH DASIA AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x0342, 0x0314, 0x03b7, 0, 0}, 0x1f97 }, // U1F97 # GREEK SMALL LETTER ETA WITH DASIA AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x0342, 0x0314, 0x03c9, 0, 0}, 0x1fa7 }, // U1FA7 # GREEK SMALL LETTER OMEGA WITH DASIA AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x0342, 0x03b1, 0, 0, 0}, 0x1fb7 }, // U1FB7 # GREEK SMALL LETTER ALPHA WITH PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x0342, 0x03b7, 0, 0, 0}, 0x1fc7 }, // U1FC7 # GREEK SMALL LETTER ETA WITH PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x0342, 0x03c9, 0, 0, 0}, 0x1ff7 }, // U1FF7 # GREEK SMALL LETTER OMEGA WITH PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x0342, 0x1f00, 0, 0, 0}, 0x1f86 }, // U1F86 # GREEK SMALL LETTER ALPHA WITH PSILI AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x0342, 0x1f01, 0, 0, 0}, 0x1f87 }, // U1F87 # GREEK SMALL LETTER ALPHA WITH DASIA AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x0342, 0x1f08, 0, 0, 0}, 0x1f8e }, // U1F8E # GREEK CAPITAL LETTER ALPHA WITH PSILI AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x0342, 0x1f09, 0, 0, 0}, 0x1f8f }, // U1F8F # GREEK CAPITAL LETTER ALPHA WITH DASIA AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x0342, 0x1f20, 0, 0, 0}, 0x1f96 }, // U1F96 # GREEK SMALL LETTER ETA WITH PSILI AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x0342, 0x1f21, 0, 0, 0}, 0x1f97 }, // U1F97 # GREEK SMALL LETTER ETA WITH DASIA AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x0342, 0x1f28, 0, 0, 0}, 0x1f9e }, // U1F9E # GREEK CAPITAL LETTER ETA WITH PSILI AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x0342, 0x1f29, 0, 0, 0}, 0x1f9f }, // U1F9F # GREEK CAPITAL LETTER ETA WITH DASIA AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x0342, 0x1f60, 0, 0, 0}, 0x1fa6 }, // U1FA6 # GREEK SMALL LETTER OMEGA WITH PSILI AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x0342, 0x1f61, 0, 0, 0}, 0x1fa7 }, // U1FA7 # GREEK SMALL LETTER OMEGA WITH DASIA AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x0342, 0x1f68, 0, 0, 0}, 0x1fae }, // U1FAE # GREEK CAPITAL LETTER OMEGA WITH PSILI AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x0342, 0x1f69, 0, 0, 0}, 0x1faf }, // U1FAF # GREEK CAPITAL LETTER OMEGA WITH DASIA AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x0342, UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0391, 0}, 0x1f8f }, // U1F8F # GREEK CAPITAL LETTER ALPHA WITH DASIA AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x0342, UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0397, 0}, 0x1f9f }, // U1F9F # GREEK CAPITAL LETTER ETA WITH DASIA AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x0342, UNITIZE(TQt::Key_Multi_key), 0x0028, 0x03a9, 0}, 0x1faf }, // U1FAF # GREEK CAPITAL LETTER OMEGA WITH DASIA AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x0342, UNITIZE(TQt::Key_Multi_key), 0x0028, 0x03b1, 0}, 0x1f87 }, // U1F87 # GREEK SMALL LETTER ALPHA WITH DASIA AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x0342, UNITIZE(TQt::Key_Multi_key), 0x0028, 0x03b7, 0}, 0x1f97 }, // U1F97 # GREEK SMALL LETTER ETA WITH DASIA AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x0342, UNITIZE(TQt::Key_Multi_key), 0x0028, 0x03c9, 0}, 0x1fa7 }, // U1FA7 # GREEK SMALL LETTER OMEGA WITH DASIA AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x0342, UNITIZE(TQt::Key_Multi_key), 0x0029, 0x0391, 0}, 0x1f8e }, // U1F8E # GREEK CAPITAL LETTER ALPHA WITH PSILI AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x0342, UNITIZE(TQt::Key_Multi_key), 0x0029, 0x0397, 0}, 0x1f9e }, // U1F9E # GREEK CAPITAL LETTER ETA WITH PSILI AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x0342, UNITIZE(TQt::Key_Multi_key), 0x0029, 0x03a9, 0}, 0x1fae }, // U1FAE # GREEK CAPITAL LETTER OMEGA WITH PSILI AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x0342, UNITIZE(TQt::Key_Multi_key), 0x0029, 0x03b1, 0}, 0x1f86 }, // U1F86 # GREEK SMALL LETTER ALPHA WITH PSILI AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x0342, UNITIZE(TQt::Key_Multi_key), 0x0029, 0x03b7, 0}, 0x1f96 }, // U1F96 # GREEK SMALL LETTER ETA WITH PSILI AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x0342, UNITIZE(TQt::Key_Multi_key), 0x0029, 0x03c9, 0}, 0x1fa6 }, // U1FA6 # GREEK SMALL LETTER OMEGA WITH PSILI AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x0391, 0, 0, 0, 0}, 0x1fbc }, // U1FBC # GREEK CAPITAL LETTER ALPHA WITH PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x0397, 0, 0, 0, 0}, 0x1fcc }, // U1FCC # GREEK CAPITAL LETTER ETA WITH PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x03a9, 0, 0, 0, 0}, 0x1ffc }, // U1FFC # GREEK CAPITAL LETTER OMEGA WITH PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x03ac, 0, 0, 0, 0}, 0x1fb4 }, // U1FB4 # GREEK SMALL LETTER ALPHA WITH OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x03ae, 0, 0, 0, 0}, 0x1fc4 }, // U1FC4 # GREEK SMALL LETTER ETA WITH OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x03b1, 0, 0, 0, 0}, 0x1fb3 }, // U1FB3 # GREEK SMALL LETTER ALPHA WITH YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x03b7, 0, 0, 0, 0}, 0x1fc3 }, // U1FC3 # GREEK SMALL LETTER ETA WITH YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x03c9, 0, 0, 0, 0}, 0x1ff3 }, // U1FF3 # GREEK SMALL LETTER OMEGA WITH YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x03ce, 0, 0, 0, 0}, 0x1ff4 }, // U1FF4 # GREEK SMALL LETTER OMEGA WITH OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x1f00, 0, 0, 0, 0}, 0x1f80 }, // U1F80 # GREEK SMALL LETTER ALPHA WITH PSILI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x1f01, 0, 0, 0, 0}, 0x1f81 }, // U1F81 # GREEK SMALL LETTER ALPHA WITH DASIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x1f02, 0, 0, 0, 0}, 0x1f82 }, // U1F82 # GREEK SMALL LETTER ALPHA WITH PSILI AND VARIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x1f03, 0, 0, 0, 0}, 0x1f83 }, // U1F83 # GREEK SMALL LETTER ALPHA WITH DASIA AND VARIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x1f04, 0, 0, 0, 0}, 0x1f84 }, // U1F84 # GREEK SMALL LETTER ALPHA WITH PSILI AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x1f05, 0, 0, 0, 0}, 0x1f85 }, // U1F85 # GREEK SMALL LETTER ALPHA WITH DASIA AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x1f06, 0, 0, 0, 0}, 0x1f86 }, // U1F86 # GREEK SMALL LETTER ALPHA WITH PSILI AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x1f07, 0, 0, 0, 0}, 0x1f87 }, // U1F87 # GREEK SMALL LETTER ALPHA WITH DASIA AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x1f08, 0, 0, 0, 0}, 0x1f88 }, // U1F88 # GREEK CAPITAL LETTER ALPHA WITH PSILI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x1f09, 0, 0, 0, 0}, 0x1f89 }, // U1F89 # GREEK CAPITAL LETTER ALPHA WITH DASIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x1f0a, 0, 0, 0, 0}, 0x1f8a }, // U1F8A # GREEK CAPITAL LETTER ALPHA WITH PSILI AND VARIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x1f0b, 0, 0, 0, 0}, 0x1f8b }, // U1F8B # GREEK CAPITAL LETTER ALPHA WITH DASIA AND VARIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x1f0c, 0, 0, 0, 0}, 0x1f8c }, // U1F8C # GREEK CAPITAL LETTER ALPHA WITH PSILI AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x1f0d, 0, 0, 0, 0}, 0x1f8d }, // U1F8D # GREEK CAPITAL LETTER ALPHA WITH DASIA AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x1f0e, 0, 0, 0, 0}, 0x1f8e }, // U1F8E # GREEK CAPITAL LETTER ALPHA WITH PSILI AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x1f0f, 0, 0, 0, 0}, 0x1f8f }, // U1F8F # GREEK CAPITAL LETTER ALPHA WITH DASIA AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x1f20, 0, 0, 0, 0}, 0x1f90 }, // U1F90 # GREEK SMALL LETTER ETA WITH PSILI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x1f21, 0, 0, 0, 0}, 0x1f91 }, // U1F91 # GREEK SMALL LETTER ETA WITH DASIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x1f22, 0, 0, 0, 0}, 0x1f92 }, // U1F92 # GREEK SMALL LETTER ETA WITH PSILI AND VARIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x1f23, 0, 0, 0, 0}, 0x1f93 }, // U1F93 # GREEK SMALL LETTER ETA WITH DASIA AND VARIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x1f24, 0, 0, 0, 0}, 0x1f94 }, // U1F94 # GREEK SMALL LETTER ETA WITH PSILI AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x1f25, 0, 0, 0, 0}, 0x1f95 }, // U1F95 # GREEK SMALL LETTER ETA WITH DASIA AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x1f26, 0, 0, 0, 0}, 0x1f96 }, // U1F96 # GREEK SMALL LETTER ETA WITH PSILI AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x1f27, 0, 0, 0, 0}, 0x1f97 }, // U1F97 # GREEK SMALL LETTER ETA WITH DASIA AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x1f28, 0, 0, 0, 0}, 0x1f98 }, // U1F98 # GREEK CAPITAL LETTER ETA WITH PSILI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x1f29, 0, 0, 0, 0}, 0x1f99 }, // U1F99 # GREEK CAPITAL LETTER ETA WITH DASIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x1f2a, 0, 0, 0, 0}, 0x1f9a }, // U1F9A # GREEK CAPITAL LETTER ETA WITH PSILI AND VARIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x1f2b, 0, 0, 0, 0}, 0x1f9b }, // U1F9B # GREEK CAPITAL LETTER ETA WITH DASIA AND VARIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x1f2c, 0, 0, 0, 0}, 0x1f9c }, // U1F9C # GREEK CAPITAL LETTER ETA WITH PSILI AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x1f2d, 0, 0, 0, 0}, 0x1f9d }, // U1F9D # GREEK CAPITAL LETTER ETA WITH DASIA AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x1f2e, 0, 0, 0, 0}, 0x1f9e }, // U1F9E # GREEK CAPITAL LETTER ETA WITH PSILI AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x1f2f, 0, 0, 0, 0}, 0x1f9f }, // U1F9F # GREEK CAPITAL LETTER ETA WITH DASIA AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x1f60, 0, 0, 0, 0}, 0x1fa0 }, // U1FA0 # GREEK SMALL LETTER OMEGA WITH PSILI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x1f61, 0, 0, 0, 0}, 0x1fa1 }, // U1FA1 # GREEK SMALL LETTER OMEGA WITH DASIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x1f62, 0, 0, 0, 0}, 0x1fa2 }, // U1FA2 # GREEK SMALL LETTER OMEGA WITH PSILI AND VARIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x1f63, 0, 0, 0, 0}, 0x1fa3 }, // U1FA3 # GREEK SMALL LETTER OMEGA WITH DASIA AND VARIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x1f64, 0, 0, 0, 0}, 0x1fa4 }, // U1FA4 # GREEK SMALL LETTER OMEGA WITH PSILI AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x1f65, 0, 0, 0, 0}, 0x1fa5 }, // U1FA5 # GREEK SMALL LETTER OMEGA WITH DASIA AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x1f66, 0, 0, 0, 0}, 0x1fa6 }, // U1FA6 # GREEK SMALL LETTER OMEGA WITH PSILI AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x1f67, 0, 0, 0, 0}, 0x1fa7 }, // U1FA7 # GREEK SMALL LETTER OMEGA WITH DASIA AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x1f68, 0, 0, 0, 0}, 0x1fa8 }, // U1FA8 # GREEK CAPITAL LETTER OMEGA WITH PSILI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x1f69, 0, 0, 0, 0}, 0x1fa9 }, // U1FA9 # GREEK CAPITAL LETTER OMEGA WITH DASIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x1f6a, 0, 0, 0, 0}, 0x1faa }, // U1FAA # GREEK CAPITAL LETTER OMEGA WITH PSILI AND VARIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x1f6b, 0, 0, 0, 0}, 0x1fab }, // U1FAB # GREEK CAPITAL LETTER OMEGA WITH DASIA AND VARIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x1f6c, 0, 0, 0, 0}, 0x1fac }, // U1FAC # GREEK CAPITAL LETTER OMEGA WITH PSILI AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x1f6d, 0, 0, 0, 0}, 0x1fad }, // U1FAD # GREEK CAPITAL LETTER OMEGA WITH DASIA AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x1f6e, 0, 0, 0, 0}, 0x1fae }, // U1FAE # GREEK CAPITAL LETTER OMEGA WITH PSILI AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x1f6f, 0, 0, 0, 0}, 0x1faf }, // U1FAF # GREEK CAPITAL LETTER OMEGA WITH DASIA AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x1f70, 0, 0, 0, 0}, 0x1fb2 }, // U1FB2 # GREEK SMALL LETTER ALPHA WITH VARIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x1f74, 0, 0, 0, 0}, 0x1fc2 }, // U1FC2 # GREEK SMALL LETTER ETA WITH VARIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x1f7c, 0, 0, 0, 0}, 0x1ff2 }, // U1FF2 # GREEK SMALL LETTER OMEGA WITH VARIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x1fb6, 0, 0, 0, 0}, 0x1fb7 }, // U1FB7 # GREEK SMALL LETTER ALPHA WITH PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x1fc6, 0, 0, 0, 0}, 0x1fc7 }, // U1FC7 # GREEK SMALL LETTER ETA WITH PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), 0x1ff6, 0, 0, 0, 0}, 0x1ff7 }, // U1FF7 # GREEK SMALL LETTER OMEGA WITH PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0028, 0x0391, 0}, 0x1f8d }, // U1F8D # GREEK CAPITAL LETTER ALPHA WITH DASIA AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0028, 0x0397, 0}, 0x1f9d }, // U1F9D # GREEK CAPITAL LETTER ETA WITH DASIA AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0028, 0x03a9, 0}, 0x1fad }, // U1FAD # GREEK CAPITAL LETTER OMEGA WITH DASIA AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0028, 0x03b1, 0}, 0x1f85 }, // U1F85 # GREEK SMALL LETTER ALPHA WITH DASIA AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0028, 0x03b7, 0}, 0x1f95 }, // U1F95 # GREEK SMALL LETTER ETA WITH DASIA AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0028, 0x03c9, 0}, 0x1fa5 }, // U1FA5 # GREEK SMALL LETTER OMEGA WITH DASIA AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0029, 0x0391, 0}, 0x1f8c }, // U1F8C # GREEK CAPITAL LETTER ALPHA WITH PSILI AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0029, 0x0397, 0}, 0x1f9c }, // U1F9C # GREEK CAPITAL LETTER ETA WITH PSILI AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0029, 0x03a9, 0}, 0x1fac }, // U1FAC # GREEK CAPITAL LETTER OMEGA WITH PSILI AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0029, 0x03b1, 0}, 0x1f84 }, // U1F84 # GREEK SMALL LETTER ALPHA WITH PSILI AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0029, 0x03b7, 0}, 0x1f94 }, // U1F94 # GREEK SMALL LETTER ETA WITH PSILI AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0029, 0x03c9, 0}, 0x1fa4 }, // U1FA4 # GREEK SMALL LETTER OMEGA WITH PSILI AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0313, 0x0391, 0}, 0x1f8c }, // U1F8C # GREEK CAPITAL LETTER ALPHA WITH PSILI AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0313, 0x0397, 0}, 0x1f9c }, // U1F9C # GREEK CAPITAL LETTER ETA WITH PSILI AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0313, 0x03a9, 0}, 0x1fac }, // U1FAC # GREEK CAPITAL LETTER OMEGA WITH PSILI AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0313, 0x03b1, 0}, 0x1f84 }, // U1F84 # GREEK SMALL LETTER ALPHA WITH PSILI AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0313, 0x03b7, 0}, 0x1f94 }, // U1F94 # GREEK SMALL LETTER ETA WITH PSILI AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0313, 0x03c9, 0}, 0x1fa4 }, // U1FA4 # GREEK SMALL LETTER OMEGA WITH PSILI AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0314, 0x0391, 0}, 0x1f8d }, // U1F8D # GREEK CAPITAL LETTER ALPHA WITH DASIA AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0314, 0x0397, 0}, 0x1f9d }, // U1F9D # GREEK CAPITAL LETTER ETA WITH DASIA AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0314, 0x03a9, 0}, 0x1fad }, // U1FAD # GREEK CAPITAL LETTER OMEGA WITH DASIA AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0314, 0x03b1, 0}, 0x1f85 }, // U1F85 # GREEK SMALL LETTER ALPHA WITH DASIA AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0314, 0x03b7, 0}, 0x1f95 }, // U1F95 # GREEK SMALL LETTER ETA WITH DASIA AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x0027, 0x0314, 0x03c9, 0}, 0x1fa5 }, // U1FA5 # GREEK SMALL LETTER OMEGA WITH DASIA AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x0027, 0x03b1, 0, 0}, 0x1fb4 }, // U1FB4 # GREEK SMALL LETTER ALPHA WITH OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x0027, 0x03b7, 0, 0}, 0x1fc4 }, // U1FC4 # GREEK SMALL LETTER ETA WITH OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x0027, 0x03c9, 0, 0}, 0x1ff4 }, // U1FF4 # GREEK SMALL LETTER OMEGA WITH OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x0027, 0x1f00, 0, 0}, 0x1f84 }, // U1F84 # GREEK SMALL LETTER ALPHA WITH PSILI AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x0027, 0x1f01, 0, 0}, 0x1f85 }, // U1F85 # GREEK SMALL LETTER ALPHA WITH DASIA AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x0027, 0x1f08, 0, 0}, 0x1f8c }, // U1F8C # GREEK CAPITAL LETTER ALPHA WITH PSILI AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x0027, 0x1f09, 0, 0}, 0x1f8d }, // U1F8D # GREEK CAPITAL LETTER ALPHA WITH DASIA AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x0027, 0x1f20, 0, 0}, 0x1f94 }, // U1F94 # GREEK SMALL LETTER ETA WITH PSILI AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x0027, 0x1f21, 0, 0}, 0x1f95 }, // U1F95 # GREEK SMALL LETTER ETA WITH DASIA AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x0027, 0x1f28, 0, 0}, 0x1f9c }, // U1F9C # GREEK CAPITAL LETTER ETA WITH PSILI AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x0027, 0x1f29, 0, 0}, 0x1f9d }, // U1F9D # GREEK CAPITAL LETTER ETA WITH DASIA AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x0027, 0x1f60, 0, 0}, 0x1fa4 }, // U1FA4 # GREEK SMALL LETTER OMEGA WITH PSILI AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x0027, 0x1f61, 0, 0}, 0x1fa5 }, // U1FA5 # GREEK SMALL LETTER OMEGA WITH DASIA AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x0027, 0x1f68, 0, 0}, 0x1fac }, // U1FAC # GREEK CAPITAL LETTER OMEGA WITH PSILI AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x0027, 0x1f69, 0, 0}, 0x1fad }, // U1FAD # GREEK CAPITAL LETTER OMEGA WITH DASIA AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0391, 0, 0}, 0x1f89 }, // U1F89 # GREEK CAPITAL LETTER ALPHA WITH DASIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0397, 0, 0}, 0x1f99 }, // U1F99 # GREEK CAPITAL LETTER ETA WITH DASIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x0028, 0x03a9, 0, 0}, 0x1fa9 }, // U1FA9 # GREEK CAPITAL LETTER OMEGA WITH DASIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x0028, 0x03b1, 0, 0}, 0x1f81 }, // U1F81 # GREEK SMALL LETTER ALPHA WITH DASIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x0028, 0x03b7, 0, 0}, 0x1f91 }, // U1F91 # GREEK SMALL LETTER ETA WITH DASIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x0028, 0x03c9, 0, 0}, 0x1fa1 }, // U1FA1 # GREEK SMALL LETTER OMEGA WITH DASIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x0029, 0x0391, 0, 0}, 0x1f88 }, // U1F88 # GREEK CAPITAL LETTER ALPHA WITH PSILI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x0029, 0x0397, 0, 0}, 0x1f98 }, // U1F98 # GREEK CAPITAL LETTER ETA WITH PSILI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x0029, 0x03a9, 0, 0}, 0x1fa8 }, // U1FA8 # GREEK CAPITAL LETTER OMEGA WITH PSILI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x0029, 0x03b1, 0, 0}, 0x1f80 }, // U1F80 # GREEK SMALL LETTER ALPHA WITH PSILI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x0029, 0x03b7, 0, 0}, 0x1f90 }, // U1F90 # GREEK SMALL LETTER ETA WITH PSILI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x0029, 0x03c9, 0, 0}, 0x1fa0 }, // U1FA0 # GREEK SMALL LETTER OMEGA WITH PSILI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0028, 0x0391, 0}, 0x1f8b }, // U1F8B # GREEK CAPITAL LETTER ALPHA WITH DASIA AND VARIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0028, 0x0397, 0}, 0x1f9b }, // U1F9B # GREEK CAPITAL LETTER ETA WITH DASIA AND VARIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0028, 0x03a9, 0}, 0x1fab }, // U1FAB # GREEK CAPITAL LETTER OMEGA WITH DASIA AND VARIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0028, 0x03b1, 0}, 0x1f83 }, // U1F83 # GREEK SMALL LETTER ALPHA WITH DASIA AND VARIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0028, 0x03b7, 0}, 0x1f93 }, // U1F93 # GREEK SMALL LETTER ETA WITH DASIA AND VARIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0028, 0x03c9, 0}, 0x1fa3 }, // U1FA3 # GREEK SMALL LETTER OMEGA WITH DASIA AND VARIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0029, 0x0391, 0}, 0x1f8a }, // U1F8A # GREEK CAPITAL LETTER ALPHA WITH PSILI AND VARIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0029, 0x0397, 0}, 0x1f9a }, // U1F9A # GREEK CAPITAL LETTER ETA WITH PSILI AND VARIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0029, 0x03a9, 0}, 0x1faa }, // U1FAA # GREEK CAPITAL LETTER OMEGA WITH PSILI AND VARIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0029, 0x03b1, 0}, 0x1f82 }, // U1F82 # GREEK SMALL LETTER ALPHA WITH PSILI AND VARIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0029, 0x03b7, 0}, 0x1f92 }, // U1F92 # GREEK SMALL LETTER ETA WITH PSILI AND VARIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0029, 0x03c9, 0}, 0x1fa2 }, // U1FA2 # GREEK SMALL LETTER OMEGA WITH PSILI AND VARIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0313, 0x0391, 0}, 0x1f8a }, // U1F8A # GREEK CAPITAL LETTER ALPHA WITH PSILI AND VARIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0313, 0x0397, 0}, 0x1f9a }, // U1F9A # GREEK CAPITAL LETTER ETA WITH PSILI AND VARIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0313, 0x03a9, 0}, 0x1faa }, // U1FAA # GREEK CAPITAL LETTER OMEGA WITH PSILI AND VARIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0313, 0x03b1, 0}, 0x1f82 }, // U1F82 # GREEK SMALL LETTER ALPHA WITH PSILI AND VARIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0313, 0x03b7, 0}, 0x1f92 }, // U1F92 # GREEK SMALL LETTER ETA WITH PSILI AND VARIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0313, 0x03c9, 0}, 0x1fa2 }, // U1FA2 # GREEK SMALL LETTER OMEGA WITH PSILI AND VARIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0314, 0x0391, 0}, 0x1f8b }, // U1F8B # GREEK CAPITAL LETTER ALPHA WITH DASIA AND VARIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0314, 0x0397, 0}, 0x1f9b }, // U1F9B # GREEK CAPITAL LETTER ETA WITH DASIA AND VARIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0314, 0x03a9, 0}, 0x1fab }, // U1FAB # GREEK CAPITAL LETTER OMEGA WITH DASIA AND VARIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0314, 0x03b1, 0}, 0x1f83 }, // U1F83 # GREEK SMALL LETTER ALPHA WITH DASIA AND VARIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0314, 0x03b7, 0}, 0x1f93 }, // U1F93 # GREEK SMALL LETTER ETA WITH DASIA AND VARIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x0060, 0x0314, 0x03c9, 0}, 0x1fa3 }, // U1FA3 # GREEK SMALL LETTER OMEGA WITH DASIA AND VARIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x0060, 0x03b1, 0, 0}, 0x1fb2 }, // U1FB2 # GREEK SMALL LETTER ALPHA WITH VARIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x0060, 0x03b7, 0, 0}, 0x1fc2 }, // U1FC2 # GREEK SMALL LETTER ETA WITH VARIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x0060, 0x03c9, 0, 0}, 0x1ff2 }, // U1FF2 # GREEK SMALL LETTER OMEGA WITH VARIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x0060, 0x1f00, 0, 0}, 0x1f82 }, // U1F82 # GREEK SMALL LETTER ALPHA WITH PSILI AND VARIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x0060, 0x1f01, 0, 0}, 0x1f83 }, // U1F83 # GREEK SMALL LETTER ALPHA WITH DASIA AND VARIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x0060, 0x1f08, 0, 0}, 0x1f8a }, // U1F8A # GREEK CAPITAL LETTER ALPHA WITH PSILI AND VARIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x0060, 0x1f09, 0, 0}, 0x1f8b }, // U1F8B # GREEK CAPITAL LETTER ALPHA WITH DASIA AND VARIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x0060, 0x1f20, 0, 0}, 0x1f92 }, // U1F92 # GREEK SMALL LETTER ETA WITH PSILI AND VARIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x0060, 0x1f21, 0, 0}, 0x1f93 }, // U1F93 # GREEK SMALL LETTER ETA WITH DASIA AND VARIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x0060, 0x1f28, 0, 0}, 0x1f9a }, // U1F9A # GREEK CAPITAL LETTER ETA WITH PSILI AND VARIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x0060, 0x1f29, 0, 0}, 0x1f9b }, // U1F9B # GREEK CAPITAL LETTER ETA WITH DASIA AND VARIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x0060, 0x1f60, 0, 0}, 0x1fa2 }, // U1FA2 # GREEK SMALL LETTER OMEGA WITH PSILI AND VARIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x0060, 0x1f61, 0, 0}, 0x1fa3 }, // U1FA3 # GREEK SMALL LETTER OMEGA WITH DASIA AND VARIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x0060, 0x1f68, 0, 0}, 0x1faa }, // U1FAA # GREEK CAPITAL LETTER OMEGA WITH PSILI AND VARIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x0060, 0x1f69, 0, 0}, 0x1fab }, // U1FAB # GREEK CAPITAL LETTER OMEGA WITH DASIA AND VARIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x007e, 0x0028, 0x0391, 0}, 0x1f8f }, // U1F8F # GREEK CAPITAL LETTER ALPHA WITH DASIA AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x007e, 0x0028, 0x0397, 0}, 0x1f9f }, // U1F9F # GREEK CAPITAL LETTER ETA WITH DASIA AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x007e, 0x0028, 0x03a9, 0}, 0x1faf }, // U1FAF # GREEK CAPITAL LETTER OMEGA WITH DASIA AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x007e, 0x0028, 0x03b1, 0}, 0x1f87 }, // U1F87 # GREEK SMALL LETTER ALPHA WITH DASIA AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x007e, 0x0028, 0x03b7, 0}, 0x1f97 }, // U1F97 # GREEK SMALL LETTER ETA WITH DASIA AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x007e, 0x0028, 0x03c9, 0}, 0x1fa7 }, // U1FA7 # GREEK SMALL LETTER OMEGA WITH DASIA AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x007e, 0x0029, 0x0391, 0}, 0x1f8e }, // U1F8E # GREEK CAPITAL LETTER ALPHA WITH PSILI AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x007e, 0x0029, 0x0397, 0}, 0x1f9e }, // U1F9E # GREEK CAPITAL LETTER ETA WITH PSILI AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x007e, 0x0029, 0x03a9, 0}, 0x1fae }, // U1FAE # GREEK CAPITAL LETTER OMEGA WITH PSILI AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x007e, 0x0029, 0x03b1, 0}, 0x1f86 }, // U1F86 # GREEK SMALL LETTER ALPHA WITH PSILI AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x007e, 0x0029, 0x03b7, 0}, 0x1f96 }, // U1F96 # GREEK SMALL LETTER ETA WITH PSILI AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x007e, 0x0029, 0x03c9, 0}, 0x1fa6 }, // U1FA6 # GREEK SMALL LETTER OMEGA WITH PSILI AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x007e, 0x0313, 0x0391, 0}, 0x1f8e }, // U1F8E # GREEK CAPITAL LETTER ALPHA WITH PSILI AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x007e, 0x0313, 0x0397, 0}, 0x1f9e }, // U1F9E # GREEK CAPITAL LETTER ETA WITH PSILI AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x007e, 0x0313, 0x03a9, 0}, 0x1fae }, // U1FAE # GREEK CAPITAL LETTER OMEGA WITH PSILI AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x007e, 0x0313, 0x03b1, 0}, 0x1f86 }, // U1F86 # GREEK SMALL LETTER ALPHA WITH PSILI AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x007e, 0x0313, 0x03b7, 0}, 0x1f96 }, // U1F96 # GREEK SMALL LETTER ETA WITH PSILI AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x007e, 0x0313, 0x03c9, 0}, 0x1fa6 }, // U1FA6 # GREEK SMALL LETTER OMEGA WITH PSILI AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x007e, 0x0314, 0x0391, 0}, 0x1f8f }, // U1F8F # GREEK CAPITAL LETTER ALPHA WITH DASIA AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x007e, 0x0314, 0x0397, 0}, 0x1f9f }, // U1F9F # GREEK CAPITAL LETTER ETA WITH DASIA AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x007e, 0x0314, 0x03a9, 0}, 0x1faf }, // U1FAF # GREEK CAPITAL LETTER OMEGA WITH DASIA AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x007e, 0x0314, 0x03b1, 0}, 0x1f87 }, // U1F87 # GREEK SMALL LETTER ALPHA WITH DASIA AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x007e, 0x0314, 0x03b7, 0}, 0x1f97 }, // U1F97 # GREEK SMALL LETTER ETA WITH DASIA AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x007e, 0x0314, 0x03c9, 0}, 0x1fa7 }, // U1FA7 # GREEK SMALL LETTER OMEGA WITH DASIA AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x007e, 0x03b1, 0, 0}, 0x1fb7 }, // U1FB7 # GREEK SMALL LETTER ALPHA WITH PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x007e, 0x03b7, 0, 0}, 0x1fc7 }, // U1FC7 # GREEK SMALL LETTER ETA WITH PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x007e, 0x03c9, 0, 0}, 0x1ff7 }, // U1FF7 # GREEK SMALL LETTER OMEGA WITH PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x007e, 0x1f00, 0, 0}, 0x1f86 }, // U1F86 # GREEK SMALL LETTER ALPHA WITH PSILI AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x007e, 0x1f01, 0, 0}, 0x1f87 }, // U1F87 # GREEK SMALL LETTER ALPHA WITH DASIA AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x007e, 0x1f08, 0, 0}, 0x1f8e }, // U1F8E # GREEK CAPITAL LETTER ALPHA WITH PSILI AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x007e, 0x1f09, 0, 0}, 0x1f8f }, // U1F8F # GREEK CAPITAL LETTER ALPHA WITH DASIA AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x007e, 0x1f20, 0, 0}, 0x1f96 }, // U1F96 # GREEK SMALL LETTER ETA WITH PSILI AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x007e, 0x1f21, 0, 0}, 0x1f97 }, // U1F97 # GREEK SMALL LETTER ETA WITH DASIA AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x007e, 0x1f28, 0, 0}, 0x1f9e }, // U1F9E # GREEK CAPITAL LETTER ETA WITH PSILI AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x007e, 0x1f29, 0, 0}, 0x1f9f }, // U1F9F # GREEK CAPITAL LETTER ETA WITH DASIA AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x007e, 0x1f60, 0, 0}, 0x1fa6 }, // U1FA6 # GREEK SMALL LETTER OMEGA WITH PSILI AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x007e, 0x1f61, 0, 0}, 0x1fa7 }, // U1FA7 # GREEK SMALL LETTER OMEGA WITH DASIA AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x007e, 0x1f68, 0, 0}, 0x1fae }, // U1FAE # GREEK CAPITAL LETTER OMEGA WITH PSILI AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x007e, 0x1f69, 0, 0}, 0x1faf }, // U1FAF # GREEK CAPITAL LETTER OMEGA WITH DASIA AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0028, 0x0391, 0}, 0x1f8d }, // U1F8D # GREEK CAPITAL LETTER ALPHA WITH DASIA AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0028, 0x0397, 0}, 0x1f9d }, // U1F9D # GREEK CAPITAL LETTER ETA WITH DASIA AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0028, 0x03a9, 0}, 0x1fad }, // U1FAD # GREEK CAPITAL LETTER OMEGA WITH DASIA AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0028, 0x03b1, 0}, 0x1f85 }, // U1F85 # GREEK SMALL LETTER ALPHA WITH DASIA AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0028, 0x03b7, 0}, 0x1f95 }, // U1F95 # GREEK SMALL LETTER ETA WITH DASIA AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0028, 0x03c9, 0}, 0x1fa5 }, // U1FA5 # GREEK SMALL LETTER OMEGA WITH DASIA AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0029, 0x0391, 0}, 0x1f8c }, // U1F8C # GREEK CAPITAL LETTER ALPHA WITH PSILI AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0029, 0x0397, 0}, 0x1f9c }, // U1F9C # GREEK CAPITAL LETTER ETA WITH PSILI AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0029, 0x03a9, 0}, 0x1fac }, // U1FAC # GREEK CAPITAL LETTER OMEGA WITH PSILI AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0029, 0x03b1, 0}, 0x1f84 }, // U1F84 # GREEK SMALL LETTER ALPHA WITH PSILI AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0029, 0x03b7, 0}, 0x1f94 }, // U1F94 # GREEK SMALL LETTER ETA WITH PSILI AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0029, 0x03c9, 0}, 0x1fa4 }, // U1FA4 # GREEK SMALL LETTER OMEGA WITH PSILI AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0313, 0x0391, 0}, 0x1f8c }, // U1F8C # GREEK CAPITAL LETTER ALPHA WITH PSILI AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0313, 0x0397, 0}, 0x1f9c }, // U1F9C # GREEK CAPITAL LETTER ETA WITH PSILI AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0313, 0x03a9, 0}, 0x1fac }, // U1FAC # GREEK CAPITAL LETTER OMEGA WITH PSILI AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0313, 0x03b1, 0}, 0x1f84 }, // U1F84 # GREEK SMALL LETTER ALPHA WITH PSILI AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0313, 0x03b7, 0}, 0x1f94 }, // U1F94 # GREEK SMALL LETTER ETA WITH PSILI AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0313, 0x03c9, 0}, 0x1fa4 }, // U1FA4 # GREEK SMALL LETTER OMEGA WITH PSILI AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0314, 0x0391, 0}, 0x1f8d }, // U1F8D # GREEK CAPITAL LETTER ALPHA WITH DASIA AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0314, 0x0397, 0}, 0x1f9d }, // U1F9D # GREEK CAPITAL LETTER ETA WITH DASIA AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0314, 0x03a9, 0}, 0x1fad }, // U1FAD # GREEK CAPITAL LETTER OMEGA WITH DASIA AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0314, 0x03b1, 0}, 0x1f85 }, // U1F85 # GREEK SMALL LETTER ALPHA WITH DASIA AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0314, 0x03b7, 0}, 0x1f95 }, // U1F95 # GREEK SMALL LETTER ETA WITH DASIA AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x0314, 0x03c9, 0}, 0x1fa5 }, // U1FA5 # GREEK SMALL LETTER OMEGA WITH DASIA AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x03b1, 0, 0}, 0x1fb4 }, // U1FB4 # GREEK SMALL LETTER ALPHA WITH OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x03b7, 0, 0}, 0x1fc4 }, // U1FC4 # GREEK SMALL LETTER ETA WITH OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x03c9, 0, 0}, 0x1ff4 }, // U1FF4 # GREEK SMALL LETTER OMEGA WITH OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x1f00, 0, 0}, 0x1f84 }, // U1F84 # GREEK SMALL LETTER ALPHA WITH PSILI AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x1f01, 0, 0}, 0x1f85 }, // U1F85 # GREEK SMALL LETTER ALPHA WITH DASIA AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x1f08, 0, 0}, 0x1f8c }, // U1F8C # GREEK CAPITAL LETTER ALPHA WITH PSILI AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x1f09, 0, 0}, 0x1f8d }, // U1F8D # GREEK CAPITAL LETTER ALPHA WITH DASIA AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x1f20, 0, 0}, 0x1f94 }, // U1F94 # GREEK SMALL LETTER ETA WITH PSILI AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x1f21, 0, 0}, 0x1f95 }, // U1F95 # GREEK SMALL LETTER ETA WITH DASIA AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x1f28, 0, 0}, 0x1f9c }, // U1F9C # GREEK CAPITAL LETTER ETA WITH PSILI AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x1f29, 0, 0}, 0x1f9d }, // U1F9D # GREEK CAPITAL LETTER ETA WITH DASIA AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x1f60, 0, 0}, 0x1fa4 }, // U1FA4 # GREEK SMALL LETTER OMEGA WITH PSILI AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x1f61, 0, 0}, 0x1fa5 }, // U1FA5 # GREEK SMALL LETTER OMEGA WITH DASIA AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x1f68, 0, 0}, 0x1fac }, // U1FAC # GREEK CAPITAL LETTER OMEGA WITH PSILI AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Multi_key), 0x00b4, 0x1f69, 0, 0}, 0x1fad }, // U1FAD # GREEK CAPITAL LETTER OMEGA WITH DASIA AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Grave), 0x0313, 0x0391, 0, 0}, 0x1f8a }, // U1F8A # GREEK CAPITAL LETTER ALPHA WITH PSILI AND VARIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Grave), 0x0313, 0x0397, 0, 0}, 0x1f9a }, // U1F9A # GREEK CAPITAL LETTER ETA WITH PSILI AND VARIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Grave), 0x0313, 0x03a9, 0, 0}, 0x1faa }, // U1FAA # GREEK CAPITAL LETTER OMEGA WITH PSILI AND VARIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Grave), 0x0313, 0x03b1, 0, 0}, 0x1f82 }, // U1F82 # GREEK SMALL LETTER ALPHA WITH PSILI AND VARIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Grave), 0x0313, 0x03b7, 0, 0}, 0x1f92 }, // U1F92 # GREEK SMALL LETTER ETA WITH PSILI AND VARIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Grave), 0x0313, 0x03c9, 0, 0}, 0x1fa2 }, // U1FA2 # GREEK SMALL LETTER OMEGA WITH PSILI AND VARIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Grave), 0x0314, 0x0391, 0, 0}, 0x1f8b }, // U1F8B # GREEK CAPITAL LETTER ALPHA WITH DASIA AND VARIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Grave), 0x0314, 0x0397, 0, 0}, 0x1f9b }, // U1F9B # GREEK CAPITAL LETTER ETA WITH DASIA AND VARIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Grave), 0x0314, 0x03a9, 0, 0}, 0x1fab }, // U1FAB # GREEK CAPITAL LETTER OMEGA WITH DASIA AND VARIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Grave), 0x0314, 0x03b1, 0, 0}, 0x1f83 }, // U1F83 # GREEK SMALL LETTER ALPHA WITH DASIA AND VARIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Grave), 0x0314, 0x03b7, 0, 0}, 0x1f93 }, // U1F93 # GREEK SMALL LETTER ETA WITH DASIA AND VARIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Grave), 0x0314, 0x03c9, 0, 0}, 0x1fa3 }, // U1FA3 # GREEK SMALL LETTER OMEGA WITH DASIA AND VARIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Grave), 0x03b1, 0, 0, 0}, 0x1fb2 }, // U1FB2 # GREEK SMALL LETTER ALPHA WITH VARIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Grave), 0x03b7, 0, 0, 0}, 0x1fc2 }, // U1FC2 # GREEK SMALL LETTER ETA WITH VARIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Grave), 0x03c9, 0, 0, 0}, 0x1ff2 }, // U1FF2 # GREEK SMALL LETTER OMEGA WITH VARIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Grave), 0x1f00, 0, 0, 0}, 0x1f82 }, // U1F82 # GREEK SMALL LETTER ALPHA WITH PSILI AND VARIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Grave), 0x1f01, 0, 0, 0}, 0x1f83 }, // U1F83 # GREEK SMALL LETTER ALPHA WITH DASIA AND VARIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Grave), 0x1f08, 0, 0, 0}, 0x1f8a }, // U1F8A # GREEK CAPITAL LETTER ALPHA WITH PSILI AND VARIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Grave), 0x1f09, 0, 0, 0}, 0x1f8b }, // U1F8B # GREEK CAPITAL LETTER ALPHA WITH DASIA AND VARIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Grave), 0x1f20, 0, 0, 0}, 0x1f92 }, // U1F92 # GREEK SMALL LETTER ETA WITH PSILI AND VARIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Grave), 0x1f21, 0, 0, 0}, 0x1f93 }, // U1F93 # GREEK SMALL LETTER ETA WITH DASIA AND VARIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Grave), 0x1f28, 0, 0, 0}, 0x1f9a }, // U1F9A # GREEK CAPITAL LETTER ETA WITH PSILI AND VARIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Grave), 0x1f29, 0, 0, 0}, 0x1f9b }, // U1F9B # GREEK CAPITAL LETTER ETA WITH DASIA AND VARIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Grave), 0x1f60, 0, 0, 0}, 0x1fa2 }, // U1FA2 # GREEK SMALL LETTER OMEGA WITH PSILI AND VARIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Grave), 0x1f61, 0, 0, 0}, 0x1fa3 }, // U1FA3 # GREEK SMALL LETTER OMEGA WITH DASIA AND VARIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Grave), 0x1f68, 0, 0, 0}, 0x1faa }, // U1FAA # GREEK CAPITAL LETTER OMEGA WITH PSILI AND VARIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Grave), 0x1f69, 0, 0, 0}, 0x1fab }, // U1FAB # GREEK CAPITAL LETTER OMEGA WITH DASIA AND VARIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Grave), UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0391, 0}, 0x1f8b }, // U1F8B # GREEK CAPITAL LETTER ALPHA WITH DASIA AND VARIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Grave), UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0397, 0}, 0x1f9b }, // U1F9B # GREEK CAPITAL LETTER ETA WITH DASIA AND VARIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Grave), UNITIZE(TQt::Key_Multi_key), 0x0028, 0x03a9, 0}, 0x1fab }, // U1FAB # GREEK CAPITAL LETTER OMEGA WITH DASIA AND VARIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Grave), UNITIZE(TQt::Key_Multi_key), 0x0028, 0x03b1, 0}, 0x1f83 }, // U1F83 # GREEK SMALL LETTER ALPHA WITH DASIA AND VARIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Grave), UNITIZE(TQt::Key_Multi_key), 0x0028, 0x03b7, 0}, 0x1f93 }, // U1F93 # GREEK SMALL LETTER ETA WITH DASIA AND VARIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Grave), UNITIZE(TQt::Key_Multi_key), 0x0028, 0x03c9, 0}, 0x1fa3 }, // U1FA3 # GREEK SMALL LETTER OMEGA WITH DASIA AND VARIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Grave), UNITIZE(TQt::Key_Multi_key), 0x0029, 0x0391, 0}, 0x1f8a }, // U1F8A # GREEK CAPITAL LETTER ALPHA WITH PSILI AND VARIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Grave), UNITIZE(TQt::Key_Multi_key), 0x0029, 0x0397, 0}, 0x1f9a }, // U1F9A # GREEK CAPITAL LETTER ETA WITH PSILI AND VARIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Grave), UNITIZE(TQt::Key_Multi_key), 0x0029, 0x03a9, 0}, 0x1faa }, // U1FAA # GREEK CAPITAL LETTER OMEGA WITH PSILI AND VARIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Grave), UNITIZE(TQt::Key_Multi_key), 0x0029, 0x03b1, 0}, 0x1f82 }, // U1F82 # GREEK SMALL LETTER ALPHA WITH PSILI AND VARIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Grave), UNITIZE(TQt::Key_Multi_key), 0x0029, 0x03b7, 0}, 0x1f92 }, // U1F92 # GREEK SMALL LETTER ETA WITH PSILI AND VARIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Grave), UNITIZE(TQt::Key_Multi_key), 0x0029, 0x03c9, 0}, 0x1fa2 }, // U1FA2 # GREEK SMALL LETTER OMEGA WITH PSILI AND VARIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Acute), 0x0313, 0x0391, 0, 0}, 0x1f8c }, // U1F8C # GREEK CAPITAL LETTER ALPHA WITH PSILI AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Acute), 0x0313, 0x0397, 0, 0}, 0x1f9c }, // U1F9C # GREEK CAPITAL LETTER ETA WITH PSILI AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Acute), 0x0313, 0x03a9, 0, 0}, 0x1fac }, // U1FAC # GREEK CAPITAL LETTER OMEGA WITH PSILI AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Acute), 0x0313, 0x03b1, 0, 0}, 0x1f84 }, // U1F84 # GREEK SMALL LETTER ALPHA WITH PSILI AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Acute), 0x0313, 0x03b7, 0, 0}, 0x1f94 }, // U1F94 # GREEK SMALL LETTER ETA WITH PSILI AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Acute), 0x0313, 0x03c9, 0, 0}, 0x1fa4 }, // U1FA4 # GREEK SMALL LETTER OMEGA WITH PSILI AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Acute), 0x0314, 0x0391, 0, 0}, 0x1f8d }, // U1F8D # GREEK CAPITAL LETTER ALPHA WITH DASIA AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Acute), 0x0314, 0x0397, 0, 0}, 0x1f9d }, // U1F9D # GREEK CAPITAL LETTER ETA WITH DASIA AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Acute), 0x0314, 0x03a9, 0, 0}, 0x1fad }, // U1FAD # GREEK CAPITAL LETTER OMEGA WITH DASIA AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Acute), 0x0314, 0x03b1, 0, 0}, 0x1f85 }, // U1F85 # GREEK SMALL LETTER ALPHA WITH DASIA AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Acute), 0x0314, 0x03b7, 0, 0}, 0x1f95 }, // U1F95 # GREEK SMALL LETTER ETA WITH DASIA AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Acute), 0x0314, 0x03c9, 0, 0}, 0x1fa5 }, // U1FA5 # GREEK SMALL LETTER OMEGA WITH DASIA AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Acute), 0x03b1, 0, 0, 0}, 0x1fb4 }, // U1FB4 # GREEK SMALL LETTER ALPHA WITH OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Acute), 0x03b7, 0, 0, 0}, 0x1fc4 }, // U1FC4 # GREEK SMALL LETTER ETA WITH OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Acute), 0x03c9, 0, 0, 0}, 0x1ff4 }, // U1FF4 # GREEK SMALL LETTER OMEGA WITH OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Acute), 0x1f00, 0, 0, 0}, 0x1f84 }, // U1F84 # GREEK SMALL LETTER ALPHA WITH PSILI AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Acute), 0x1f01, 0, 0, 0}, 0x1f85 }, // U1F85 # GREEK SMALL LETTER ALPHA WITH DASIA AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Acute), 0x1f08, 0, 0, 0}, 0x1f8c }, // U1F8C # GREEK CAPITAL LETTER ALPHA WITH PSILI AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Acute), 0x1f09, 0, 0, 0}, 0x1f8d }, // U1F8D # GREEK CAPITAL LETTER ALPHA WITH DASIA AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Acute), 0x1f20, 0, 0, 0}, 0x1f94 }, // U1F94 # GREEK SMALL LETTER ETA WITH PSILI AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Acute), 0x1f21, 0, 0, 0}, 0x1f95 }, // U1F95 # GREEK SMALL LETTER ETA WITH DASIA AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Acute), 0x1f28, 0, 0, 0}, 0x1f9c }, // U1F9C # GREEK CAPITAL LETTER ETA WITH PSILI AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Acute), 0x1f29, 0, 0, 0}, 0x1f9d }, // U1F9D # GREEK CAPITAL LETTER ETA WITH DASIA AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Acute), 0x1f60, 0, 0, 0}, 0x1fa4 }, // U1FA4 # GREEK SMALL LETTER OMEGA WITH PSILI AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Acute), 0x1f61, 0, 0, 0}, 0x1fa5 }, // U1FA5 # GREEK SMALL LETTER OMEGA WITH DASIA AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Acute), 0x1f68, 0, 0, 0}, 0x1fac }, // U1FAC # GREEK CAPITAL LETTER OMEGA WITH PSILI AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Acute), 0x1f69, 0, 0, 0}, 0x1fad }, // U1FAD # GREEK CAPITAL LETTER OMEGA WITH DASIA AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0391, 0}, 0x1f8d }, // U1F8D # GREEK CAPITAL LETTER ALPHA WITH DASIA AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0397, 0}, 0x1f9d }, // U1F9D # GREEK CAPITAL LETTER ETA WITH DASIA AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Multi_key), 0x0028, 0x03a9, 0}, 0x1fad }, // U1FAD # GREEK CAPITAL LETTER OMEGA WITH DASIA AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Multi_key), 0x0028, 0x03b1, 0}, 0x1f85 }, // U1F85 # GREEK SMALL LETTER ALPHA WITH DASIA AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Multi_key), 0x0028, 0x03b7, 0}, 0x1f95 }, // U1F95 # GREEK SMALL LETTER ETA WITH DASIA AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Multi_key), 0x0028, 0x03c9, 0}, 0x1fa5 }, // U1FA5 # GREEK SMALL LETTER OMEGA WITH DASIA AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Multi_key), 0x0029, 0x0391, 0}, 0x1f8c }, // U1F8C # GREEK CAPITAL LETTER ALPHA WITH PSILI AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Multi_key), 0x0029, 0x0397, 0}, 0x1f9c }, // U1F9C # GREEK CAPITAL LETTER ETA WITH PSILI AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Multi_key), 0x0029, 0x03a9, 0}, 0x1fac }, // U1FAC # GREEK CAPITAL LETTER OMEGA WITH PSILI AND OXIA AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Multi_key), 0x0029, 0x03b1, 0}, 0x1f84 }, // U1F84 # GREEK SMALL LETTER ALPHA WITH PSILI AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Multi_key), 0x0029, 0x03b7, 0}, 0x1f94 }, // U1F94 # GREEK SMALL LETTER ETA WITH PSILI AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Acute), UNITIZE(TQt::Key_Multi_key), 0x0029, 0x03c9, 0}, 0x1fa4 }, // U1FA4 # GREEK SMALL LETTER OMEGA WITH PSILI AND OXIA AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Tilde), 0x0313, 0x0391, 0, 0}, 0x1f8e }, // U1F8E # GREEK CAPITAL LETTER ALPHA WITH PSILI AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Tilde), 0x0313, 0x0397, 0, 0}, 0x1f9e }, // U1F9E # GREEK CAPITAL LETTER ETA WITH PSILI AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Tilde), 0x0313, 0x03a9, 0, 0}, 0x1fae }, // U1FAE # GREEK CAPITAL LETTER OMEGA WITH PSILI AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Tilde), 0x0313, 0x03b1, 0, 0}, 0x1f86 }, // U1F86 # GREEK SMALL LETTER ALPHA WITH PSILI AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Tilde), 0x0313, 0x03b7, 0, 0}, 0x1f96 }, // U1F96 # GREEK SMALL LETTER ETA WITH PSILI AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Tilde), 0x0313, 0x03c9, 0, 0}, 0x1fa6 }, // U1FA6 # GREEK SMALL LETTER OMEGA WITH PSILI AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Tilde), 0x0314, 0x0391, 0, 0}, 0x1f8f }, // U1F8F # GREEK CAPITAL LETTER ALPHA WITH DASIA AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Tilde), 0x0314, 0x0397, 0, 0}, 0x1f9f }, // U1F9F # GREEK CAPITAL LETTER ETA WITH DASIA AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Tilde), 0x0314, 0x03a9, 0, 0}, 0x1faf }, // U1FAF # GREEK CAPITAL LETTER OMEGA WITH DASIA AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Tilde), 0x0314, 0x03b1, 0, 0}, 0x1f87 }, // U1F87 # GREEK SMALL LETTER ALPHA WITH DASIA AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Tilde), 0x0314, 0x03b7, 0, 0}, 0x1f97 }, // U1F97 # GREEK SMALL LETTER ETA WITH DASIA AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Tilde), 0x0314, 0x03c9, 0, 0}, 0x1fa7 }, // U1FA7 # GREEK SMALL LETTER OMEGA WITH DASIA AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Tilde), 0x03b1, 0, 0, 0}, 0x1fb7 }, // U1FB7 # GREEK SMALL LETTER ALPHA WITH PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Tilde), 0x03b7, 0, 0, 0}, 0x1fc7 }, // U1FC7 # GREEK SMALL LETTER ETA WITH PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Tilde), 0x03c9, 0, 0, 0}, 0x1ff7 }, // U1FF7 # GREEK SMALL LETTER OMEGA WITH PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Tilde), 0x1f00, 0, 0, 0}, 0x1f86 }, // U1F86 # GREEK SMALL LETTER ALPHA WITH PSILI AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Tilde), 0x1f01, 0, 0, 0}, 0x1f87 }, // U1F87 # GREEK SMALL LETTER ALPHA WITH DASIA AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Tilde), 0x1f08, 0, 0, 0}, 0x1f8e }, // U1F8E # GREEK CAPITAL LETTER ALPHA WITH PSILI AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Tilde), 0x1f09, 0, 0, 0}, 0x1f8f }, // U1F8F # GREEK CAPITAL LETTER ALPHA WITH DASIA AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Tilde), 0x1f20, 0, 0, 0}, 0x1f96 }, // U1F96 # GREEK SMALL LETTER ETA WITH PSILI AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Tilde), 0x1f21, 0, 0, 0}, 0x1f97 }, // U1F97 # GREEK SMALL LETTER ETA WITH DASIA AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Tilde), 0x1f28, 0, 0, 0}, 0x1f9e }, // U1F9E # GREEK CAPITAL LETTER ETA WITH PSILI AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Tilde), 0x1f29, 0, 0, 0}, 0x1f9f }, // U1F9F # GREEK CAPITAL LETTER ETA WITH DASIA AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Tilde), 0x1f60, 0, 0, 0}, 0x1fa6 }, // U1FA6 # GREEK SMALL LETTER OMEGA WITH PSILI AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Tilde), 0x1f61, 0, 0, 0}, 0x1fa7 }, // U1FA7 # GREEK SMALL LETTER OMEGA WITH DASIA AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Tilde), 0x1f68, 0, 0, 0}, 0x1fae }, // U1FAE # GREEK CAPITAL LETTER OMEGA WITH PSILI AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Tilde), 0x1f69, 0, 0, 0}, 0x1faf }, // U1FAF # GREEK CAPITAL LETTER OMEGA WITH DASIA AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Tilde), UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0391, 0}, 0x1f8f }, // U1F8F # GREEK CAPITAL LETTER ALPHA WITH DASIA AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Tilde), UNITIZE(TQt::Key_Multi_key), 0x0028, 0x0397, 0}, 0x1f9f }, // U1F9F # GREEK CAPITAL LETTER ETA WITH DASIA AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Tilde), UNITIZE(TQt::Key_Multi_key), 0x0028, 0x03a9, 0}, 0x1faf }, // U1FAF # GREEK CAPITAL LETTER OMEGA WITH DASIA AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Tilde), UNITIZE(TQt::Key_Multi_key), 0x0028, 0x03b1, 0}, 0x1f87 }, // U1F87 # GREEK SMALL LETTER ALPHA WITH DASIA AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Tilde), UNITIZE(TQt::Key_Multi_key), 0x0028, 0x03b7, 0}, 0x1f97 }, // U1F97 # GREEK SMALL LETTER ETA WITH DASIA AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Tilde), UNITIZE(TQt::Key_Multi_key), 0x0028, 0x03c9, 0}, 0x1fa7 }, // U1FA7 # GREEK SMALL LETTER OMEGA WITH DASIA AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Tilde), UNITIZE(TQt::Key_Multi_key), 0x0029, 0x0391, 0}, 0x1f8e }, // U1F8E # GREEK CAPITAL LETTER ALPHA WITH PSILI AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Tilde), UNITIZE(TQt::Key_Multi_key), 0x0029, 0x0397, 0}, 0x1f9e }, // U1F9E # GREEK CAPITAL LETTER ETA WITH PSILI AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Tilde), UNITIZE(TQt::Key_Multi_key), 0x0029, 0x03a9, 0}, 0x1fae }, // U1FAE # GREEK CAPITAL LETTER OMEGA WITH PSILI AND PERISPOMENI AND PROSGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Tilde), UNITIZE(TQt::Key_Multi_key), 0x0029, 0x03b1, 0}, 0x1f86 }, // U1F86 # GREEK SMALL LETTER ALPHA WITH PSILI AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Tilde), UNITIZE(TQt::Key_Multi_key), 0x0029, 0x03b7, 0}, 0x1f96 }, // U1F96 # GREEK SMALL LETTER ETA WITH PSILI AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Tilde), UNITIZE(TQt::Key_Multi_key), 0x0029, 0x03c9, 0}, 0x1fa6 }, // U1FA6 # GREEK SMALL LETTER OMEGA WITH PSILI AND PERISPOMENI AND YPOGEGRAMMENI
    { {UNITIZE(TQt::Key_Dead_Iota), UNITIZE(TQt::Key_Dead_Iota), 0, 0, 0, 0}, 0x037a }, // U37a
// /* broken */    { {UNITIZE(TQt::Key_Dead_Iota), dead_space, 0, 0, 0, 0}, 0x037a }, // U37a
    { {UNITIZE(TQt::Key_Dead_Voiced_Sound), 0x3046, 0, 0, 0, 0}, 0x3094 }, // U3094 # HIRAGANA LETTER VU
    { {UNITIZE(TQt::Key_Dead_Voiced_Sound), 0x304b, 0, 0, 0, 0}, 0x304c }, // U304C # HIRAGANA LETTER GA
    { {UNITIZE(TQt::Key_Dead_Voiced_Sound), 0x304d, 0, 0, 0, 0}, 0x304e }, // U304E # HIRAGANA LETTER GI
    { {UNITIZE(TQt::Key_Dead_Voiced_Sound), 0x304f, 0, 0, 0, 0}, 0x3050 }, // U3050 # HIRAGANA LETTER GU
    { {UNITIZE(TQt::Key_Dead_Voiced_Sound), 0x3051, 0, 0, 0, 0}, 0x3052 }, // U3052 # HIRAGANA LETTER GE
    { {UNITIZE(TQt::Key_Dead_Voiced_Sound), 0x3053, 0, 0, 0, 0}, 0x3054 }, // U3054 # HIRAGANA LETTER GO
    { {UNITIZE(TQt::Key_Dead_Voiced_Sound), 0x3055, 0, 0, 0, 0}, 0x3056 }, // U3056 # HIRAGANA LETTER ZA
    { {UNITIZE(TQt::Key_Dead_Voiced_Sound), 0x3057, 0, 0, 0, 0}, 0x3058 }, // U3058 # HIRAGANA LETTER ZI
    { {UNITIZE(TQt::Key_Dead_Voiced_Sound), 0x3059, 0, 0, 0, 0}, 0x305a }, // U305A # HIRAGANA LETTER ZU
    { {UNITIZE(TQt::Key_Dead_Voiced_Sound), 0x305b, 0, 0, 0, 0}, 0x305c }, // U305C # HIRAGANA LETTER ZE
    { {UNITIZE(TQt::Key_Dead_Voiced_Sound), 0x305d, 0, 0, 0, 0}, 0x305e }, // U305E # HIRAGANA LETTER ZO
    { {UNITIZE(TQt::Key_Dead_Voiced_Sound), 0x305f, 0, 0, 0, 0}, 0x3060 }, // U3060 # HIRAGANA LETTER DA
    { {UNITIZE(TQt::Key_Dead_Voiced_Sound), 0x3061, 0, 0, 0, 0}, 0x3062 }, // U3062 # HIRAGANA LETTER DI
    { {UNITIZE(TQt::Key_Dead_Voiced_Sound), 0x3064, 0, 0, 0, 0}, 0x3065 }, // U3065 # HIRAGANA LETTER DU
    { {UNITIZE(TQt::Key_Dead_Voiced_Sound), 0x3066, 0, 0, 0, 0}, 0x3067 }, // U3067 # HIRAGANA LETTER DE
    { {UNITIZE(TQt::Key_Dead_Voiced_Sound), 0x3068, 0, 0, 0, 0}, 0x3069 }, // U3069 # HIRAGANA LETTER DO
    { {UNITIZE(TQt::Key_Dead_Voiced_Sound), 0x306f, 0, 0, 0, 0}, 0x3070 }, // U3070 # HIRAGANA LETTER BA
    { {UNITIZE(TQt::Key_Dead_Voiced_Sound), 0x3072, 0, 0, 0, 0}, 0x3073 }, // U3073 # HIRAGANA LETTER BI
    { {UNITIZE(TQt::Key_Dead_Voiced_Sound), 0x3075, 0, 0, 0, 0}, 0x3076 }, // U3076 # HIRAGANA LETTER BU
    { {UNITIZE(TQt::Key_Dead_Voiced_Sound), 0x3078, 0, 0, 0, 0}, 0x3079 }, // U3079 # HIRAGANA LETTER BE
    { {UNITIZE(TQt::Key_Dead_Voiced_Sound), 0x307b, 0, 0, 0, 0}, 0x307c }, // U307C # HIRAGANA LETTER BO
    { {UNITIZE(TQt::Key_Dead_Voiced_Sound), 0x309d, 0, 0, 0, 0}, 0x309e }, // U309E # HIRAGANA VOICED ITERATION MARK
    { {UNITIZE(TQt::Key_Dead_Voiced_Sound), 0x30a6, 0, 0, 0, 0}, 0x30f4 }, // U30F4 # KATAKANA LETTER VU
    { {UNITIZE(TQt::Key_Dead_Voiced_Sound), 0x30ab, 0, 0, 0, 0}, 0x30ac }, // U30AC # KATAKANA LETTER GA
    { {UNITIZE(TQt::Key_Dead_Voiced_Sound), 0x30ad, 0, 0, 0, 0}, 0x30ae }, // U30AE # KATAKANA LETTER GI
    { {UNITIZE(TQt::Key_Dead_Voiced_Sound), 0x30af, 0, 0, 0, 0}, 0x30b0 }, // U30B0 # KATAKANA LETTER GU
    { {UNITIZE(TQt::Key_Dead_Voiced_Sound), 0x30b1, 0, 0, 0, 0}, 0x30b2 }, // U30B2 # KATAKANA LETTER GE
    { {UNITIZE(TQt::Key_Dead_Voiced_Sound), 0x30b3, 0, 0, 0, 0}, 0x30b4 }, // U30B4 # KATAKANA LETTER GO
    { {UNITIZE(TQt::Key_Dead_Voiced_Sound), 0x30b5, 0, 0, 0, 0}, 0x30b6 }, // U30B6 # KATAKANA LETTER ZA
    { {UNITIZE(TQt::Key_Dead_Voiced_Sound), 0x30b7, 0, 0, 0, 0}, 0x30b8 }, // U30B8 # KATAKANA LETTER ZI
    { {UNITIZE(TQt::Key_Dead_Voiced_Sound), 0x30b9, 0, 0, 0, 0}, 0x30ba }, // U30BA # KATAKANA LETTER ZU
    { {UNITIZE(TQt::Key_Dead_Voiced_Sound), 0x30bb, 0, 0, 0, 0}, 0x30bc }, // U30BC # KATAKANA LETTER ZE
    { {UNITIZE(TQt::Key_Dead_Voiced_Sound), 0x30bd, 0, 0, 0, 0}, 0x30be }, // U30BE # KATAKANA LETTER ZO
    { {UNITIZE(TQt::Key_Dead_Voiced_Sound), 0x30bf, 0, 0, 0, 0}, 0x30c0 }, // U30C0 # KATAKANA LETTER DA
    { {UNITIZE(TQt::Key_Dead_Voiced_Sound), 0x30c1, 0, 0, 0, 0}, 0x30c2 }, // U30C2 # KATAKANA LETTER DI
    { {UNITIZE(TQt::Key_Dead_Voiced_Sound), 0x30c4, 0, 0, 0, 0}, 0x30c5 }, // U30C5 # KATAKANA LETTER DU
    { {UNITIZE(TQt::Key_Dead_Voiced_Sound), 0x30c6, 0, 0, 0, 0}, 0x30c7 }, // U30C7 # KATAKANA LETTER DE
    { {UNITIZE(TQt::Key_Dead_Voiced_Sound), 0x30c8, 0, 0, 0, 0}, 0x30c9 }, // U30C9 # KATAKANA LETTER DO
    { {UNITIZE(TQt::Key_Dead_Voiced_Sound), 0x30cf, 0, 0, 0, 0}, 0x30d0 }, // U30D0 # KATAKANA LETTER BA
    { {UNITIZE(TQt::Key_Dead_Voiced_Sound), 0x30d2, 0, 0, 0, 0}, 0x30d3 }, // U30D3 # KATAKANA LETTER BI
    { {UNITIZE(TQt::Key_Dead_Voiced_Sound), 0x30d5, 0, 0, 0, 0}, 0x30d6 }, // U30D6 # KATAKANA LETTER BU
    { {UNITIZE(TQt::Key_Dead_Voiced_Sound), 0x30d8, 0, 0, 0, 0}, 0x30d9 }, // U30D9 # KATAKANA LETTER BE
    { {UNITIZE(TQt::Key_Dead_Voiced_Sound), 0x30db, 0, 0, 0, 0}, 0x30dc }, // U30DC # KATAKANA LETTER BO
    { {UNITIZE(TQt::Key_Dead_Voiced_Sound), 0x30ef, 0, 0, 0, 0}, 0x30f7 }, // U30F7 # KATAKANA LETTER VA
    { {UNITIZE(TQt::Key_Dead_Voiced_Sound), 0x30f0, 0, 0, 0, 0}, 0x30f8 }, // U30F8 # KATAKANA LETTER VI
    { {UNITIZE(TQt::Key_Dead_Voiced_Sound), 0x30f1, 0, 0, 0, 0}, 0x30f9 }, // U30F9 # KATAKANA LETTER VE
    { {UNITIZE(TQt::Key_Dead_Voiced_Sound), 0x30f2, 0, 0, 0, 0}, 0x30fa }, // U30FA # KATAKANA LETTER VO
    { {UNITIZE(TQt::Key_Dead_Voiced_Sound), 0x30fd, 0, 0, 0, 0}, 0x30fe }, // U30FE # KATAKANA VOICED ITERATION MARK
    { {UNITIZE(TQt::Key_Dead_Semivoiced_Sound), 0x306f, 0, 0, 0, 0}, 0x3071 }, // U3071 # HIRAGANA LETTER PA
    { {UNITIZE(TQt::Key_Dead_Semivoiced_Sound), 0x3072, 0, 0, 0, 0}, 0x3074 }, // U3074 # HIRAGANA LETTER PI
    { {UNITIZE(TQt::Key_Dead_Semivoiced_Sound), 0x3075, 0, 0, 0, 0}, 0x3077 }, // U3077 # HIRAGANA LETTER PU
    { {UNITIZE(TQt::Key_Dead_Semivoiced_Sound), 0x3078, 0, 0, 0, 0}, 0x307a }, // U307A # HIRAGANA LETTER PE
    { {UNITIZE(TQt::Key_Dead_Semivoiced_Sound), 0x307b, 0, 0, 0, 0}, 0x307d }, // U307D # HIRAGANA LETTER PO
    { {UNITIZE(TQt::Key_Dead_Semivoiced_Sound), 0x30cf, 0, 0, 0, 0}, 0x30d1 }, // U30D1 # KATAKANA LETTER PA
    { {UNITIZE(TQt::Key_Dead_Semivoiced_Sound), 0x30d2, 0, 0, 0, 0}, 0x30d4 }, // U30D4 # KATAKANA LETTER PI
    { {UNITIZE(TQt::Key_Dead_Semivoiced_Sound), 0x30d5, 0, 0, 0, 0}, 0x30d7 }, // U30D7 # KATAKANA LETTER PU
    { {UNITIZE(TQt::Key_Dead_Semivoiced_Sound), 0x30d8, 0, 0, 0, 0}, 0x30da }, // U30DA # KATAKANA LETTER PE
    { {UNITIZE(TQt::Key_Dead_Semivoiced_Sound), 0x30db, 0, 0, 0, 0}, 0x30dd }, // U30DD # KATAKANA LETTER PO
// /* broken */    { {UNITIZE(TQt::Key_Dead_Belowdot), 0x0000, 0, 0, 0, 0}, 0x1ee2 }, // U1EE2 # LATIN CAPITAL LETTER O WITH HORN AND DOT BELOW
// /* broken */    { {UNITIZE(TQt::Key_Dead_Belowdot), 0x0000, 0, 0, 0, 0}, 0x1ef1 }, // U1EF1 # LATIN SMALL LETTER U WITH HORN AND DOT BELOW
// /* broken */    { {UNITIZE(TQt::Key_Dead_Belowdot), 0x0000, 0, 0, 0, 0}, 0x1ee3 }, // U1EE3 # LATIN SMALL LETTER O WITH HORN AND DOT BELOW
// /* broken */    { {UNITIZE(TQt::Key_Dead_Belowdot), 0x0000, 0, 0, 0, 0}, 0x1ef0 }, // U1EF0 # LATIN CAPITAL LETTER U WITH HORN AND DOT BELOW
    { {UNITIZE(TQt::Key_Dead_Belowdot), 0x0041, 0, 0, 0, 0}, 0x1ea0 }, // U1EA0 # LATIN CAPITAL LETTER A WITH DOT BELOW
    { {UNITIZE(TQt::Key_Dead_Belowdot), 0x0042, 0, 0, 0, 0}, 0x1e04 }, // U1E04 # LATIN CAPITAL LETTER B WITH DOT BELOW
    { {UNITIZE(TQt::Key_Dead_Belowdot), 0x0044, 0, 0, 0, 0}, 0x1e0c }, // U1E0C # LATIN CAPITAL LETTER D WITH DOT BELOW
    { {UNITIZE(TQt::Key_Dead_Belowdot), 0x0045, 0, 0, 0, 0}, 0x1eb8 }, // U1EB8 # LATIN CAPITAL LETTER E WITH DOT BELOW
    { {UNITIZE(TQt::Key_Dead_Belowdot), 0x0048, 0, 0, 0, 0}, 0x1e24 }, // U1E24 # LATIN CAPITAL LETTER H WITH DOT BELOW
    { {UNITIZE(TQt::Key_Dead_Belowdot), 0x0049, 0, 0, 0, 0}, 0x1eca }, // U1ECA # LATIN CAPITAL LETTER I WITH DOT BELOW
    { {UNITIZE(TQt::Key_Dead_Belowdot), 0x004b, 0, 0, 0, 0}, 0x1e32 }, // U1E32 # LATIN CAPITAL LETTER K WITH DOT BELOW
    { {UNITIZE(TQt::Key_Dead_Belowdot), 0x004c, 0, 0, 0, 0}, 0x1e36 }, // U1E36 # LATIN CAPITAL LETTER L WITH DOT BELOW
    { {UNITIZE(TQt::Key_Dead_Belowdot), 0x004d, 0, 0, 0, 0}, 0x1e42 }, // U1E42 # LATIN CAPITAL LETTER M WITH DOT BELOW
    { {UNITIZE(TQt::Key_Dead_Belowdot), 0x004e, 0, 0, 0, 0}, 0x1e46 }, // U1E46 # LATIN CAPITAL LETTER N WITH DOT BELOW
    { {UNITIZE(TQt::Key_Dead_Belowdot), 0x004f, 0, 0, 0, 0}, 0x1ecc }, // U1ECC # LATIN CAPITAL LETTER O WITH DOT BELOW
    { {UNITIZE(TQt::Key_Dead_Belowdot), 0x0052, 0, 0, 0, 0}, 0x1e5a }, // U1E5A # LATIN CAPITAL LETTER R WITH DOT BELOW
    { {UNITIZE(TQt::Key_Dead_Belowdot), 0x0053, 0, 0, 0, 0}, 0x1e62 }, // U1E62 # LATIN CAPITAL LETTER S WITH DOT BELOW
    { {UNITIZE(TQt::Key_Dead_Belowdot), 0x0054, 0, 0, 0, 0}, 0x1e6c }, // U1E6C # LATIN CAPITAL LETTER T WITH DOT BELOW
    { {UNITIZE(TQt::Key_Dead_Belowdot), 0x0055, 0, 0, 0, 0}, 0x1ee4 }, // U1EE4 # LATIN CAPITAL LETTER U WITH DOT BELOW
    { {UNITIZE(TQt::Key_Dead_Belowdot), 0x0056, 0, 0, 0, 0}, 0x1e7e }, // U1E7E # LATIN CAPITAL LETTER V WITH DOT BELOW
    { {UNITIZE(TQt::Key_Dead_Belowdot), 0x0057, 0, 0, 0, 0}, 0x1e88 }, // U1E88 # LATIN CAPITAL LETTER W WITH DOT BELOW
    { {UNITIZE(TQt::Key_Dead_Belowdot), 0x0059, 0, 0, 0, 0}, 0x1ef4 }, // U1EF4 # LATIN CAPITAL LETTER Y WITH DOT BELOW
    { {UNITIZE(TQt::Key_Dead_Belowdot), 0x005a, 0, 0, 0, 0}, 0x1e92 }, // U1E92 # LATIN CAPITAL LETTER Z WITH DOT BELOW
    { {UNITIZE(TQt::Key_Dead_Belowdot), 0x0061, 0, 0, 0, 0}, 0x1ea1 }, // U1EA1 # LATIN SMALL LETTER A WITH DOT BELOW
    { {UNITIZE(TQt::Key_Dead_Belowdot), 0x0062, 0, 0, 0, 0}, 0x1e05 }, // U1E05 # LATIN SMALL LETTER B WITH DOT BELOW
    { {UNITIZE(TQt::Key_Dead_Belowdot), 0x0064, 0, 0, 0, 0}, 0x1e0d }, // U1E0D # LATIN SMALL LETTER D WITH DOT BELOW
    { {UNITIZE(TQt::Key_Dead_Belowdot), 0x0065, 0, 0, 0, 0}, 0x1eb9 }, // U1EB9 # LATIN SMALL LETTER E WITH DOT BELOW
    { {UNITIZE(TQt::Key_Dead_Belowdot), 0x0068, 0, 0, 0, 0}, 0x1e25 }, // U1E25 # LATIN SMALL LETTER H WITH DOT BELOW
    { {UNITIZE(TQt::Key_Dead_Belowdot), 0x0069, 0, 0, 0, 0}, 0x1ecb }, // U1ECB # LATIN SMALL LETTER I WITH DOT BELOW
    { {UNITIZE(TQt::Key_Dead_Belowdot), 0x006b, 0, 0, 0, 0}, 0x1e33 }, // U1E33 # LATIN SMALL LETTER K WITH DOT BELOW
    { {UNITIZE(TQt::Key_Dead_Belowdot), 0x006c, 0, 0, 0, 0}, 0x1e37 }, // U1E37 # LATIN SMALL LETTER L WITH DOT BELOW
    { {UNITIZE(TQt::Key_Dead_Belowdot), 0x006d, 0, 0, 0, 0}, 0x1e43 }, // U1E43 # LATIN SMALL LETTER M WITH DOT BELOW
    { {UNITIZE(TQt::Key_Dead_Belowdot), 0x006e, 0, 0, 0, 0}, 0x1e47 }, // U1E47 # LATIN SMALL LETTER N WITH DOT BELOW
    { {UNITIZE(TQt::Key_Dead_Belowdot), 0x006f, 0, 0, 0, 0}, 0x1ecd }, // U1ECD # LATIN SMALL LETTER O WITH DOT BELOW
    { {UNITIZE(TQt::Key_Dead_Belowdot), 0x0072, 0, 0, 0, 0}, 0x1e5b }, // U1E5B # LATIN SMALL LETTER R WITH DOT BELOW
    { {UNITIZE(TQt::Key_Dead_Belowdot), 0x0073, 0, 0, 0, 0}, 0x1e63 }, // U1E63 # LATIN SMALL LETTER S WITH DOT BELOW
    { {UNITIZE(TQt::Key_Dead_Belowdot), 0x0074, 0, 0, 0, 0}, 0x1e6d }, // U1E6D # LATIN SMALL LETTER T WITH DOT BELOW
    { {UNITIZE(TQt::Key_Dead_Belowdot), 0x0075, 0, 0, 0, 0}, 0x1ee5 }, // U1EE5 # LATIN SMALL LETTER U WITH DOT BELOW
    { {UNITIZE(TQt::Key_Dead_Belowdot), 0x0076, 0, 0, 0, 0}, 0x1e7f }, // U1E7F # LATIN SMALL LETTER V WITH DOT BELOW
    { {UNITIZE(TQt::Key_Dead_Belowdot), 0x0077, 0, 0, 0, 0}, 0x1e89 }, // U1E89 # LATIN SMALL LETTER W WITH DOT BELOW
    { {UNITIZE(TQt::Key_Dead_Belowdot), 0x0079, 0, 0, 0, 0}, 0x1ef5 }, // U1EF5 # LATIN SMALL LETTER Y WITH DOT BELOW
    { {UNITIZE(TQt::Key_Dead_Belowdot), 0x007a, 0, 0, 0, 0}, 0x1e93 }, // U1E93 # LATIN SMALL LETTER Z WITH DOT BELOW
    { {UNITIZE(TQt::Key_Dead_Belowdot), 0x00c2, 0, 0, 0, 0}, 0x1eac }, // U1EAC # LATIN CAPITAL LETTER A WITH CIRCUMFLEX AND DOT BELOW
    { {UNITIZE(TQt::Key_Dead_Belowdot), 0x00ca, 0, 0, 0, 0}, 0x1ec6 }, // U1EC6 # LATIN CAPITAL LETTER E WITH CIRCUMFLEX AND DOT BELOW
    { {UNITIZE(TQt::Key_Dead_Belowdot), 0x00d4, 0, 0, 0, 0}, 0x1ed8 }, // U1ED8 # LATIN CAPITAL LETTER O WITH CIRCUMFLEX AND DOT BELOW
    { {UNITIZE(TQt::Key_Dead_Belowdot), 0x00e2, 0, 0, 0, 0}, 0x1ead }, // U1EAD # LATIN SMALL LETTER A WITH CIRCUMFLEX AND DOT BELOW
    { {UNITIZE(TQt::Key_Dead_Belowdot), 0x00ea, 0, 0, 0, 0}, 0x1ec7 }, // U1EC7 # LATIN SMALL LETTER E WITH CIRCUMFLEX AND DOT BELOW
    { {UNITIZE(TQt::Key_Dead_Belowdot), 0x00f4, 0, 0, 0, 0}, 0x1ed9 }, // U1ED9 # LATIN SMALL LETTER O WITH CIRCUMFLEX AND DOT BELOW
    { {UNITIZE(TQt::Key_Dead_Belowdot), 0x0102, 0, 0, 0, 0}, 0x1eb6 }, // U1EB6 # LATIN CAPITAL LETTER A WITH BREVE AND DOT BELOW
    { {UNITIZE(TQt::Key_Dead_Belowdot), 0x0103, 0, 0, 0, 0}, 0x1eb7 }, // U1EB7 # LATIN SMALL LETTER A WITH BREVE AND DOT BELOW
    { {UNITIZE(TQt::Key_Dead_Belowdot), 0x01A0, 0, 0, 0, 0}, 0x1ee2 }, // U1EE2 # LATIN CAPITAL LETTER O WITH HORN AND DOT BELOW
    { {UNITIZE(TQt::Key_Dead_Belowdot), 0x01A1, 0, 0, 0, 0}, 0x1ee3 }, // U1EE3 # LATIN SMALL LETTER O WITH HORN AND DOT BELOW
    { {UNITIZE(TQt::Key_Dead_Belowdot), 0x01AF, 0, 0, 0, 0}, 0x1ef0 }, // U1EF0 # LATIN CAPITAL LETTER U WITH HORN AND DOT BELOW
    { {UNITIZE(TQt::Key_Dead_Belowdot), 0x01B0, 0, 0, 0, 0}, 0x1ef1 }, // U1EF1 # LATIN SMALL LETTER U WITH HORN AND DOT BELOW
    { {UNITIZE(TQt::Key_Dead_Belowdot), UNITIZE(TQt::Key_Multi_key), 0x002b, 0x004f, 0, 0}, 0x1ee2 }, // U1EE2 # LATIN CAPITAL LETTER O WITH HORN AND DOT BELOW
    { {UNITIZE(TQt::Key_Dead_Belowdot), UNITIZE(TQt::Key_Multi_key), 0x002b, 0x0055, 0, 0}, 0x1ef0 }, // U1EF0 # LATIN CAPITAL LETTER U WITH HORN AND DOT BELOW
    { {UNITIZE(TQt::Key_Dead_Belowdot), UNITIZE(TQt::Key_Multi_key), 0x002b, 0x006f, 0, 0}, 0x1ee3 }, // U1EE3 # LATIN SMALL LETTER O WITH HORN AND DOT BELOW
    { {UNITIZE(TQt::Key_Dead_Belowdot), UNITIZE(TQt::Key_Multi_key), 0x002b, 0x0075, 0, 0}, 0x1ef1 }, // U1EF1 # LATIN SMALL LETTER U WITH HORN AND DOT BELOW
    { {UNITIZE(TQt::Key_Dead_Belowdot), UNITIZE(TQt::Key_Dead_Horn), 0x004f, 0, 0, 0}, 0x1ee2 }, // U1EE2 # LATIN CAPITAL LETTER O WITH HORN AND DOT BELOW
    { {UNITIZE(TQt::Key_Dead_Belowdot), UNITIZE(TQt::Key_Dead_Horn), 0x0055, 0, 0, 0}, 0x1ef0 }, // U1EF0 # LATIN CAPITAL LETTER U WITH HORN AND DOT BELOW
    { {UNITIZE(TQt::Key_Dead_Belowdot), UNITIZE(TQt::Key_Dead_Horn), 0x006f, 0, 0, 0}, 0x1ee3 }, // U1EE3 # LATIN SMALL LETTER O WITH HORN AND DOT BELOW
    { {UNITIZE(TQt::Key_Dead_Belowdot), UNITIZE(TQt::Key_Dead_Horn), 0x0075, 0, 0, 0}, 0x1ef1 }, // U1EF1 # LATIN SMALL LETTER U WITH HORN AND DOT BELOW
// /* broken */    { {UNITIZE(TQt::Key_Dead_Hook), 0x0000, 0, 0, 0, 0}, 0x1edf }, // U1EDF # LATIN SMALL LETTER O WITH HORN AND HOOK ABOVE
// /* broken */    { {UNITIZE(TQt::Key_Dead_Hook), 0x0000, 0, 0, 0, 0}, 0x1eec }, // U1EEC # LATIN CAPITAL LETTER U WITH HORN AND HOOK ABOVE
// /* broken */    { {UNITIZE(TQt::Key_Dead_Hook), 0x0000, 0, 0, 0, 0}, 0x1eed }, // U1EED # LATIN SMALL LETTER U WITH HORN AND HOOK ABOVE
// /* broken */    { {UNITIZE(TQt::Key_Dead_Hook), 0x0000, 0, 0, 0, 0}, 0x1ede }, // U1EDE # LATIN CAPITAL LETTER O WITH HORN AND HOOK ABOVE
    { {UNITIZE(TQt::Key_Dead_Hook), 0x0041, 0, 0, 0, 0}, 0x1ea2 }, // U1EA2 # LATIN CAPITAL LETTER A WITH HOOK ABOVE
    { {UNITIZE(TQt::Key_Dead_Hook), 0x0045, 0, 0, 0, 0}, 0x1eba }, // U1EBA # LATIN CAPITAL LETTER E WITH HOOK ABOVE
    { {UNITIZE(TQt::Key_Dead_Hook), 0x0049, 0, 0, 0, 0}, 0x1ec8 }, // U1EC8 # LATIN CAPITAL LETTER I WITH HOOK ABOVE
    { {UNITIZE(TQt::Key_Dead_Hook), 0x004f, 0, 0, 0, 0}, 0x1ece }, // U1ECE # LATIN CAPITAL LETTER O WITH HOOK ABOVE
    { {UNITIZE(TQt::Key_Dead_Hook), 0x0055, 0, 0, 0, 0}, 0x1ee6 }, // U1EE6 # LATIN CAPITAL LETTER U WITH HOOK ABOVE
    { {UNITIZE(TQt::Key_Dead_Hook), 0x0059, 0, 0, 0, 0}, 0x1ef6 }, // U1EF6 # LATIN CAPITAL LETTER Y WITH HOOK ABOVE
    { {UNITIZE(TQt::Key_Dead_Hook), 0x0061, 0, 0, 0, 0}, 0x1ea3 }, // U1EA3 # LATIN SMALL LETTER A WITH HOOK ABOVE
    { {UNITIZE(TQt::Key_Dead_Hook), 0x0065, 0, 0, 0, 0}, 0x1ebb }, // U1EBB # LATIN SMALL LETTER E WITH HOOK ABOVE
    { {UNITIZE(TQt::Key_Dead_Hook), 0x0069, 0, 0, 0, 0}, 0x1ec9 }, // U1EC9 # LATIN SMALL LETTER I WITH HOOK ABOVE
    { {UNITIZE(TQt::Key_Dead_Hook), 0x006f, 0, 0, 0, 0}, 0x1ecf }, // U1ECF # LATIN SMALL LETTER O WITH HOOK ABOVE
    { {UNITIZE(TQt::Key_Dead_Hook), 0x0075, 0, 0, 0, 0}, 0x1ee7 }, // U1EE7 # LATIN SMALL LETTER U WITH HOOK ABOVE
    { {UNITIZE(TQt::Key_Dead_Hook), 0x0079, 0, 0, 0, 0}, 0x1ef7 }, // U1EF7 # LATIN SMALL LETTER Y WITH HOOK ABOVE
    { {UNITIZE(TQt::Key_Dead_Hook), 0x00c2, 0, 0, 0, 0}, 0x1ea8 }, // U1EA8 # LATIN CAPITAL LETTER A WITH CIRCUMFLEX AND HOOK ABOVE
    { {UNITIZE(TQt::Key_Dead_Hook), 0x00ca, 0, 0, 0, 0}, 0x1ec2 }, // U1EC2 # LATIN CAPITAL LETTER E WITH CIRCUMFLEX AND HOOK ABOVE
    { {UNITIZE(TQt::Key_Dead_Hook), 0x00d4, 0, 0, 0, 0}, 0x1ed4 }, // U1ED4 # LATIN CAPITAL LETTER O WITH CIRCUMFLEX AND HOOK ABOVE
    { {UNITIZE(TQt::Key_Dead_Hook), 0x00e2, 0, 0, 0, 0}, 0x1ea9 }, // U1EA9 # LATIN SMALL LETTER A WITH CIRCUMFLEX AND HOOK ABOVE
    { {UNITIZE(TQt::Key_Dead_Hook), 0x00ea, 0, 0, 0, 0}, 0x1ec3 }, // U1EC3 # LATIN SMALL LETTER E WITH CIRCUMFLEX AND HOOK ABOVE
    { {UNITIZE(TQt::Key_Dead_Hook), 0x00f4, 0, 0, 0, 0}, 0x1ed5 }, // U1ED5 # LATIN SMALL LETTER O WITH CIRCUMFLEX AND HOOK ABOVE
    { {UNITIZE(TQt::Key_Dead_Hook), 0x0102, 0, 0, 0, 0}, 0x1eb2 }, // U1EB2 # LATIN CAPITAL LETTER A WITH BREVE AND HOOK ABOVE
    { {UNITIZE(TQt::Key_Dead_Hook), 0x0103, 0, 0, 0, 0}, 0x1eb3 }, // U1EB3 # LATIN SMALL LETTER A WITH BREVE AND HOOK ABOVE
    { {UNITIZE(TQt::Key_Dead_Hook), 0x01A0, 0, 0, 0, 0}, 0x1ede }, // U1EDE # LATIN CAPITAL LETTER O WITH HORN AND HOOK ABOVE
    { {UNITIZE(TQt::Key_Dead_Hook), 0x01A1, 0, 0, 0, 0}, 0x1edf }, // U1EDF # LATIN SMALL LETTER O WITH HORN AND HOOK ABOVE
    { {UNITIZE(TQt::Key_Dead_Hook), 0x01AF, 0, 0, 0, 0}, 0x1eec }, // U1EEC # LATIN CAPITAL LETTER U WITH HORN AND HOOK ABOVE
    { {UNITIZE(TQt::Key_Dead_Hook), 0x01B0, 0, 0, 0, 0}, 0x1eed }, // U1EED # LATIN SMALL LETTER U WITH HORN AND HOOK ABOVE
    { {UNITIZE(TQt::Key_Dead_Hook), UNITIZE(TQt::Key_Multi_key), 0x002b, 0x004f, 0, 0}, 0x1ede }, // U1EDE # LATIN CAPITAL LETTER O WITH HORN AND HOOK ABOVE
    { {UNITIZE(TQt::Key_Dead_Hook), UNITIZE(TQt::Key_Multi_key), 0x002b, 0x0055, 0, 0}, 0x1eec }, // U1EEC # LATIN CAPITAL LETTER U WITH HORN AND HOOK ABOVE
    { {UNITIZE(TQt::Key_Dead_Hook), UNITIZE(TQt::Key_Multi_key), 0x002b, 0x006f, 0, 0}, 0x1edf }, // U1EDF # LATIN SMALL LETTER O WITH HORN AND HOOK ABOVE
    { {UNITIZE(TQt::Key_Dead_Hook), UNITIZE(TQt::Key_Multi_key), 0x002b, 0x0075, 0, 0}, 0x1eed }, // U1EED # LATIN SMALL LETTER U WITH HORN AND HOOK ABOVE
    { {UNITIZE(TQt::Key_Dead_Hook), UNITIZE(TQt::Key_Multi_key), 0x0055, 0x0041, 0, 0}, 0x1eb2 }, // U1EB2 # LATIN CAPITAL LETTER A WITH BREVE AND HOOK ABOVE
    { {UNITIZE(TQt::Key_Dead_Hook), UNITIZE(TQt::Key_Multi_key), 0x0055, 0x0061, 0, 0}, 0x1eb3 }, // U1EB3 # LATIN SMALL LETTER A WITH BREVE AND HOOK ABOVE
    { {UNITIZE(TQt::Key_Dead_Hook), UNITIZE(TQt::Key_Multi_key), 0x005e, 0x0041, 0, 0}, 0x1ea8 }, // U1EA8 # LATIN CAPITAL LETTER A WITH CIRCUMFLEX AND HOOK ABOVE
    { {UNITIZE(TQt::Key_Dead_Hook), UNITIZE(TQt::Key_Multi_key), 0x005e, 0x0045, 0, 0}, 0x1ec2 }, // U1EC2 # LATIN CAPITAL LETTER E WITH CIRCUMFLEX AND HOOK ABOVE
    { {UNITIZE(TQt::Key_Dead_Hook), UNITIZE(TQt::Key_Multi_key), 0x005e, 0x004f, 0, 0}, 0x1ed4 }, // U1ED4 # LATIN CAPITAL LETTER O WITH CIRCUMFLEX AND HOOK ABOVE
    { {UNITIZE(TQt::Key_Dead_Hook), UNITIZE(TQt::Key_Multi_key), 0x005e, 0x0061, 0, 0}, 0x1ea9 }, // U1EA9 # LATIN SMALL LETTER A WITH CIRCUMFLEX AND HOOK ABOVE
    { {UNITIZE(TQt::Key_Dead_Hook), UNITIZE(TQt::Key_Multi_key), 0x005e, 0x0065, 0, 0}, 0x1ec3 }, // U1EC3 # LATIN SMALL LETTER E WITH CIRCUMFLEX AND HOOK ABOVE
    { {UNITIZE(TQt::Key_Dead_Hook), UNITIZE(TQt::Key_Multi_key), 0x005e, 0x006f, 0, 0}, 0x1ed5 }, // U1ED5 # LATIN SMALL LETTER O WITH CIRCUMFLEX AND HOOK ABOVE
    { {UNITIZE(TQt::Key_Dead_Hook), UNITIZE(TQt::Key_Multi_key), 0x0062, 0x0041, 0, 0}, 0x1eb2 }, // U1EB2 # LATIN CAPITAL LETTER A WITH BREVE AND HOOK ABOVE
    { {UNITIZE(TQt::Key_Dead_Hook), UNITIZE(TQt::Key_Multi_key), 0x0062, 0x0061, 0, 0}, 0x1eb3 }, // U1EB3 # LATIN SMALL LETTER A WITH BREVE AND HOOK ABOVE
    { {UNITIZE(TQt::Key_Dead_Hook), UNITIZE(TQt::Key_Dead_Circumflex), 0x0041, 0, 0, 0}, 0x1ea8 }, // U1EA8 # LATIN CAPITAL LETTER A WITH CIRCUMFLEX AND HOOK ABOVE
    { {UNITIZE(TQt::Key_Dead_Hook), UNITIZE(TQt::Key_Dead_Circumflex), 0x0045, 0, 0, 0}, 0x1ec2 }, // U1EC2 # LATIN CAPITAL LETTER E WITH CIRCUMFLEX AND HOOK ABOVE
    { {UNITIZE(TQt::Key_Dead_Hook), UNITIZE(TQt::Key_Dead_Circumflex), 0x004f, 0, 0, 0}, 0x1ed4 }, // U1ED4 # LATIN CAPITAL LETTER O WITH CIRCUMFLEX AND HOOK ABOVE
    { {UNITIZE(TQt::Key_Dead_Hook), UNITIZE(TQt::Key_Dead_Circumflex), 0x0061, 0, 0, 0}, 0x1ea9 }, // U1EA9 # LATIN SMALL LETTER A WITH CIRCUMFLEX AND HOOK ABOVE
    { {UNITIZE(TQt::Key_Dead_Hook), UNITIZE(TQt::Key_Dead_Circumflex), 0x0065, 0, 0, 0}, 0x1ec3 }, // U1EC3 # LATIN SMALL LETTER E WITH CIRCUMFLEX AND HOOK ABOVE
    { {UNITIZE(TQt::Key_Dead_Hook), UNITIZE(TQt::Key_Dead_Circumflex), 0x006f, 0, 0, 0}, 0x1ed5 }, // U1ED5 # LATIN SMALL LETTER O WITH CIRCUMFLEX AND HOOK ABOVE
    { {UNITIZE(TQt::Key_Dead_Hook), UNITIZE(TQt::Key_Dead_Breve), 0x0041, 0, 0, 0}, 0x1eb2 }, // U1EB2 # LATIN CAPITAL LETTER A WITH BREVE AND HOOK ABOVE
    { {UNITIZE(TQt::Key_Dead_Hook), UNITIZE(TQt::Key_Dead_Breve), 0x0061, 0, 0, 0}, 0x1eb3 }, // U1EB3 # LATIN SMALL LETTER A WITH BREVE AND HOOK ABOVE
    { {UNITIZE(TQt::Key_Dead_Hook), UNITIZE(TQt::Key_Dead_Horn), 0x004f, 0, 0, 0}, 0x1ede }, // U1EDE # LATIN CAPITAL LETTER O WITH HORN AND HOOK ABOVE
    { {UNITIZE(TQt::Key_Dead_Hook), UNITIZE(TQt::Key_Dead_Horn), 0x0055, 0, 0, 0}, 0x1eec }, // U1EEC # LATIN CAPITAL LETTER U WITH HORN AND HOOK ABOVE
    { {UNITIZE(TQt::Key_Dead_Hook), UNITIZE(TQt::Key_Dead_Horn), 0x006f, 0, 0, 0}, 0x1edf }, // U1EDF # LATIN SMALL LETTER O WITH HORN AND HOOK ABOVE
    { {UNITIZE(TQt::Key_Dead_Hook), UNITIZE(TQt::Key_Dead_Horn), 0x0075, 0, 0, 0}, 0x1eed }, // U1EED # LATIN SMALL LETTER U WITH HORN AND HOOK ABOVE
    { {UNITIZE(TQt::Key_Dead_Horn), 0x004f, 0, 0, 0, 0}, 0x01a0 }, // U01A0 # LATIN CAPITAL LETTER O WITH HORN
    { {UNITIZE(TQt::Key_Dead_Horn), 0x0055, 0, 0, 0, 0}, 0x01af }, // U01AF # LATIN CAPITAL LETTER U WITH HORN
    { {UNITIZE(TQt::Key_Dead_Horn), 0x006f, 0, 0, 0, 0}, 0x01a1 }, // U01A1 # LATIN SMALL LETTER O WITH HORN
    { {UNITIZE(TQt::Key_Dead_Horn), 0x0075, 0, 0, 0, 0}, 0x01b0 }  // U01B0 # LATIN SMALL LETTER U WITH HORN
};

const TQComposeTable TQSimpleInputContext::defaultComposeTable = {
    defaultTable,
    ( sizeof(defaultTable) / sizeof(TQComposeTableElement) )
};
