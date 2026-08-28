#!/usr/bin/env ruby

# xcompose2q.rb: X Compose table to QSimpleInputContext table translator
# Copyright (C) 2004 immodule for TQt Project.  All rights reserved.
#
# This file is written to contribute to Trolltech AS under their own
# licence. You may use this file under your TQt license. Following
# description is copied from their original file headers. Contact
# immodule-qt@freedesktop.org if any conditions of this licensing are
# not clear to you.

# This file may be distributed under the terms of the Q Public License
# as defined by Trolltech AS of Norway and appearing in the file
# LICENSE.QPL included in the packaging of this file.
#
# This file may be distributed and/or modified under the terms of the
# GNU General Public License version 2 as published by the Free Software
# Foundation and appearing in the file LICENSE.GPL included in the
# packaging of this file.
#
# Licensees holding valid TQt Enterprise Edition or TQt Professional Edition
# licenses may use this file in accordance with the TQt Commercial License
# Agreement provided with the Software.
#
# This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
# WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
#
# See http://www.trolltech.com/pricing.html or email sales@trolltech.com for
#   information about TQt Commercial License Agreements.
# See http://www.trolltech.com/qpl/ for QPL licensing information.
# See http://www.trolltech.com/gpl/ for GPL licensing information.
#
# Contact info@trolltech.com if any conditions of this licensing are
# not clear to you.


# Usage:
#   xcompose2q.rb /usr/X11R6/lib/X11/locale/en_US.UTF-8/Compose

require 'uconv'

# translation table for lazy sorting
raw2key = {
  "0y1120" => "UNITIZE(TQt::Key_Multi_key)",
  "0y1250" => "UNITIZE(TQt::Key_Dead_Grave)",
  "0y1251" => "UNITIZE(TQt::Key_Dead_Acute)",
  "0y1252" => "UNITIZE(TQt::Key_Dead_Circumflex)",
  "0y1253" => "UNITIZE(TQt::Key_Dead_Tilde)",
  "0y1254" => "UNITIZE(TQt::Key_Dead_Macron)",
  "0y1255" => "UNITIZE(TQt::Key_Dead_Breve)",
  "0y1256" => "UNITIZE(TQt::Key_Dead_Abovedot)",
  "0y1257" => "UNITIZE(TQt::Key_Dead_Diaeresis)",
  "0y1258" => "UNITIZE(TQt::Key_Dead_Abovering)",
  "0y1259" => "UNITIZE(TQt::Key_Dead_Doubleacute)",
  "0y125a" => "UNITIZE(TQt::Key_Dead_Caron)",
  "0y125b" => "UNITIZE(TQt::Key_Dead_Cedilla)",
  "0y125c" => "UNITIZE(TQt::Key_Dead_Ogonek)",
  "0y125d" => "UNITIZE(TQt::Key_Dead_Iota)",
  "0y125e" => "UNITIZE(TQt::Key_Dead_Voiced_Sound)",
  "0y125f" => "UNITIZE(TQt::Key_Dead_Semivoiced_Sound)",
  "0y1260" => "UNITIZE(TQt::Key_Dead_Belowdot)",
  "0y1261" => "UNITIZE(TQt::Key_Dead_Hook)",
  "0y1262" => "UNITIZE(TQt::Key_Dead_Horn)",
}

# X's KeySym to Unicode translation table. The unicode value is
# extracted from
# QETWidget::translateKeyEventInternal(). Translation for some
# keys are not available
x2q = {
  "Multi_key"              => "0y1120",
  "dead_grave"             => "0y1250",
  "dead_acute"             => "0y1251",
  "dead_circumflex"        => "0y1252",
  "dead_tilde"             => "0y1253",
  "dead_macron"            => "0y1254",
  "dead_breve"             => "0y1255",
  "dead_abovedot"          => "0y1256",
  "dead_diaeresis"         => "0y1257",
  "dead_abovering"         => "0y1258",
  "dead_doubleacute"       => "0y1259",
  "dead_caron"             => "0y125a",
  "dead_cedilla"           => "0y125b",
  "dead_ogonek"            => "0y125c",
  "dead_iota"              => "0y125d",
  "dead_voiced_sound"      => "0y125e",
  "dead_semivoiced_sound"  => "0y125f",
  "dead_belowdot"          => "0y1260",
  "dead_hook"              => "0y1261",
  "dead_horn"              => "0y1262",
  
  "0" => "0x0030",
  "1" => "0x0031",
  "2" => "0x0032",
  "3" => "0x0033",
  "4" => "0x0034",
  "5" => "0x0035",
  "6" => "0x0036",
  "7" => "0x0037",
  "8" => "0x0038",
  "9" => "0x0039",
  "A" => "0x0041",
  "AE" => "0x00c6",
  "Abreve" => "0x0102",
  "Acircumflex" => "0x00c2",
  "Adiaeresis" => "0x00c4",
  "Arabic_alef" => "0x0627",
  "Arabic_waw" => "0x0648",
  "Arabic_yeh" => "0x064a",
  "Aring" => "0x00c5",
  "B" => "0x0042",
  "C" => "0x0043",
  "Ccedilla" => "0x00c7",
  "Cyrillic_A" => "0x0410",
  "Cyrillic_CHE" => "0x0427",
  "Cyrillic_E" => "0x042d",
  "Cyrillic_GHE" => "0x0413",
  "Cyrillic_I" => "0x0418",
  "Cyrillic_IE" => "0x0415",
  "Cyrillic_KA" => "0x041a",
  "Cyrillic_O" => "0x041e",
  "Cyrillic_U" => "0x0423",
  "Cyrillic_YERU" => "0x042b",
  "Cyrillic_ZE" => "0x0417",
  "Cyrillic_ZHE" => "0x0416",
  "Cyrillic_a" => "0x0430",
  "Cyrillic_che" => "0x0447",
  "Cyrillic_e" => "0x044d",
  "Cyrillic_ghe" => "0x0433",
  "Cyrillic_i" => "0x0438",
  "Cyrillic_ie" => "0x0435",
  "Cyrillic_ka" => "0x043a",
  "Cyrillic_o" => "0x043e",
  "Cyrillic_u" => "0x0443",
  "Cyrillic_yeru" => "0x044b",
  "Cyrillic_ze" => "0x0437",
  "Cyrillic_zhe" => "0x0436",
  "D" => "0x0044",
  "E" => "0x0045",
  "Ecircumflex" => "0x00ca",
  "Emacron" => "0x0112",
  "F" => "0x0046",
  "G" => "0x0047",
  "Greek_ALPHA" => "0x0391",
  "Greek_EPSILON" => "0x0395",
  "Greek_ETA" => "0x0397",
  "Greek_IOTA" => "0x0399",
  "Greek_OMEGA" => "0x03a9",
  "Greek_OMICRON" => "0x039f",
  "Greek_RHO" => "0x03a1",
  "Greek_UPSILON" => "0x03a5",
  "Greek_alpha" => "0x03b1",
  "Greek_alphaaccent" => "0x03ac",
  "Greek_epsilon" => "0x03b5",
  "Greek_eta" => "0x03b7",
  "Greek_etaaccent" => "0x03ae",
  "Greek_iota" => "0x03b9",
  "Greek_iotadieresis" => "0x03ca",
  "Greek_omega" => "0x03c9",
  "Greek_omegaaccent" => "0x03ce",
  "Greek_omicron" => "0x03bf",
  "Greek_rho" => "0x03c1",
  "Greek_upsilon" => "0x03c5",
  "Greek_upsilondieresis" => "0x03cb",
  "H" => "0x0048",
  "I" => "0x0049",
  "Idiaeresis" => "0x00cf",
  "J" => "0x004a",
  "K" => "0x004b",
  "KP_0" => "0x0000",
  "KP_1" => "0x0000",
  "KP_2" => "0x0000",
  "KP_3" => "0x0000",
  "KP_4" => "0x0000",
  "KP_5" => "0x0000",
  "KP_6" => "0x0000",
  "KP_7" => "0x0000",
  "KP_8" => "0x0000",
  "KP_9" => "0x0000",
  "KP_Add" => "0x0000",
  "KP_Divide" => "0x0000",
  "KP_Equal" => "0x0000",
  "KP_Space" => "0x0000",
  "L" => "0x004c",
  "M" => "0x004d",
  "N" => "0x004e",
  "O" => "0x004f",
  "Ocircumflex" => "0x00d4",
  "Odiaeresis" => "0x00d6",
  "Ohorn" => "0x0000",
  "Omacron" => "0x014c",
  "Ooblique" => "0x00d8",
  "Otilde" => "0x00d5",
  "P" => "0x0050",
  "Q" => "0x0051",
  "R" => "0x0052",
  "S" => "0x0053",
  "Sacute" => "0x015a",
  "Scaron" => "0x0160",
  "T" => "0x0054",
  "U" => "0x0055",
  "Udiaeresis" => "0x00dc",
  "Uhorn" => "0x0000",
  "Ukrainian_I" => "0x0406",
  "Ukrainian_i" => "0x0456",
  "Umacron" => "0x016a",
  "Utilde" => "0x0168",
  "V" => "0x0056",
  "W" => "0x0057",
  "X" => "0x0058",
  "Y" => "0x0059",
  "Z" => "0x005a",
  "a" => "0x0061",
  "abreve" => "0x0103",
  "acircumflex" => "0x00e2",
  "acute" => "0x00b4",
  "adiaeresis" => "0x00e4",
  "ae" => "0x00e6",
  "ampersand" => "0x0026",
  "apostrophe" => "0x0027",
  "approximate" => "0x223c",
  "aring" => "0x00e5",
  "asciicircum" => "0x005e",
  "asciitilde" => "0x007e",
  "asterisk" => "0x002a",
  "at" => "0x0040",
  "b" => "0x0062",
  "backslash" => "0x005c",
  "bar" => "0x007c",
  "braceleft" => "0x007b",
  "braceright" => "0x007d",
  "bracketleft" => "0x005b",
  "bracketright" => "0x005d",
  "c" => "0x0063",
  "ccedilla" => "0x00e7",
  "colon" => "0x003a",
  "combining_acute" => "0x0000",
  "combining_belowdot" => "0x0000",
  "combining_grave" => "0x0000",
  "combining_hook" => "0x0000",
  "combining_tilde" => "0x0000",
  "comma" => "0x002c",
  "d" => "0x0064",
  "diaeresis" => "0x00a8",
  "dollar" => "0x0024",
  "e" => "0x0065",
  "ecircumflex" => "0x00ea",
  "emacron" => "0x0113",
  "equal" => "0x003d",
  "exclam" => "0x0021",
  "f" => "0x0066",
  "g" => "0x0067",
  "grave" => "0x0060",
  "greater" => "0x003e",
  "greaterthanequal" => "0x2265",
  "h" => "0x0068",
  "hebrew_aleph" => "0x05d0",
  "hebrew_bet" => "0x05d1",
  "hebrew_beth" => "0x05d1",
  "hebrew_dalet" => "0x05d3",
  "hebrew_daleth" => "0x05d3",
  "hebrew_finalkaph" => "0x05da",
  "hebrew_finalpe" => "0x05e3",
  "hebrew_gimel" => "0x05d2",
  "hebrew_gimmel" => "0x05d2",
  "hebrew_he" => "0x05d4",
  "hebrew_kaph" => "0x05db",
  "hebrew_kuf" => "0x05e7",
  "hebrew_lamed" => "0x05dc",
  "hebrew_mem" => "0x05de",
  "hebrew_nun" => "0x05e0",
  "hebrew_pe" => "0x05e4",
  "hebrew_qoph" => "0x05e7",
  "hebrew_resh" => "0x05e8",
  "hebrew_samech" => "0x05e1",
  "hebrew_samekh" => "0x05e1",
  "hebrew_shin" => "0x05e9",
  "hebrew_taf" => "0x05ea",
  "hebrew_taw" => "0x05ea",
  "hebrew_tet" => "0x05d8",
  "hebrew_teth" => "0x05d8",
  "hebrew_waw" => "0x05d5",
  "hebrew_yod" => "0x05d9",
  "hebrew_zade" => "0x05e6",
  "hebrew_zadi" => "0x05e6",
  "hebrew_zain" => "0x05d6",
  "hebrew_zayin" => "0x05d6",
  "i" => "0x0069",
  "identical" => "0x2261",
  "idiaeresis" => "0x00ef",
  "includedin" => "0x2282",
  "includes" => "0x2283",
  "j" => "0x006a",
  "k" => "0x006b",
  "kana_A" => "0x30a2",
  "kana_CHI" => "0x30c1",
  "kana_E" => "0x30a8",
  "kana_FU" => "0x30d5",
  "kana_HA" => "0x30cf",
  "kana_HE" => "0x30d8",
  "kana_HI" => "0x30d2",
  "kana_HO" => "0x30db",
  "kana_I" => "0x30a4",
  "kana_KA" => "0x30ab",
  "kana_KE" => "0x30b1",
  "kana_KI" => "0x30ad",
  "kana_KO" => "0x30b3",
  "kana_KU" => "0x30af",
  "kana_MA" => "0x30de",
  "kana_ME" => "0x30e1",
  "kana_MI" => "0x30df",
  "kana_MO" => "0x30e2",
  "kana_MU" => "0x30e0",
  "kana_NA" => "0x30ca",
  "kana_NE" => "0x30cd",
  "kana_NI" => "0x30cb",
  "kana_NO" => "0x30ce",
  "kana_NU" => "0x30cc",
  "kana_O" => "0x30aa",
  "kana_RA" => "0x30e9",
  "kana_RE" => "0x30ec",
  "kana_RI" => "0x30ea",
  "kana_RO" => "0x30ed",
  "kana_RU" => "0x30eb",
  "kana_SA" => "0x30b5",
  "kana_SE" => "0x30bb",
  "kana_SHI" => "0x30b7",
  "kana_SO" => "0x30bd",
  "kana_SU" => "0x30b9",
  "kana_TA" => "0x30bf",
  "kana_TE" => "0x30c6",
  "kana_TO" => "0x30c8",
  "kana_TSU" => "0x30c4",
  "kana_U" => "0x30a6",
  "kana_WA" => "0x30ef",
  "kana_WO" => "0x30f2",
  "kana_YA" => "0x30e4",
  "kana_YO" => "0x30e8",
  "kana_YU" => "0x30e6",
  "l" => "0x006c",
  "leftarrow" => "0x2190",
  "leftcaret" => "0x003c",
  "leftshoe" => "0x2282",
  "less" => "0x003c",
  "lessthanequal" => "0x2264",
  "m" => "0x006d",
  "macron" => "0x00af",
  "minus" => "0x002d",
  "n" => "0x006e",
  "numbersign" => "0x0023",
  "o" => "0x006f",
  "ocircumflex" => "0x00f4",
  "odiaeresis" => "0x00f6",
  "ohorn" => "0x0000",
  "omacron" => "0x014d",
  "oslash" => "0x00f8",
  "otilde" => "0x00f5",
  "p" => "0x0070",
  "parenleft" => "0x0028",
  "parenright" => "0x0029",
  "percent" => "0x0025",
  "period" => "0x002e",
  "plus" => "0x002b",
  "q" => "0x0071",
  "question" => "0x003f",
  "quotedbl" => "0x0022",
  "r" => "0x0072",
  "rightarrow" => "0x2192",
  "rightcaret" => "0x003e",
  "rightshoe" => "0x2283",
  "righttack" => "0x22a3",
  "s" => "0x0073",
  "sacute" => "0x015b",
  "scaron" => "0x0161",
  "semicolon" => "0x003b",
  "slash" => "0x002f",
  "space" => "0x0020",
  "t" => "0x0074",
  "u" => "0x0075",
  "udiaeresis" => "0x00fc",
  "uhorn" => "0x0000",
  "umacron" => "0x016b",
  "underbar" => "0x005f",
  "underscore" => "0x005f",
  "utilde" => "0x0169",
  "v" => "0x0076",
  "w" => "0x0077",
  "x" => "0x0078",
  "y" => "0x0079",
  "z" => "0x007a",
}

readlines.collect {
  |line|

  broken = ""
  seq = ["0", "0", "0", "0", "0", "0"]
  index = 0
  rhs = "0x0000"
  comment = ""

  case line
  when /^(#|XCOMM)(.*)$/
    comment = "// #{$2}\n"
    rhs = :isCommentLine
  when /^$/
    comment = "\n"
    rhs = :isCommentLine
  else
    line.scan(/<([^>]+)>/) {
      |args|
      keysym = args[0]
      unit = x2q[keysym] || keysym.sub(/^U([\da-fA-F]{4,5})$/, "0x\\1")
      if (/^(0x(0000|[\da-fA-F]{5,})|dead_space)$/ =~ unit)
	broken = "// /* broken */"
      end
      seq[index] = unit
      index += 1
    }

    Regexp.new(':\s+"(.+)"\s+(.+)$', nil, 'u') =~ line
    rhs = Uconv::u8tou16($1).unpack('v')[0]
    rhs = format("0x%04x", rhs)
    comment = $2
  end

  entry = {
    :broken => broken,
    :seq => seq,
    :rhs => rhs,
    :comment => comment
  }
}.sort {
  |entry_a, entry_b|

  entry_a[:seq] <=> entry_b[:seq]
}.each {
  |entry|

  broken, seq, rhs, comment = entry.values_at(:broken, :seq, :rhs, :comment)
  if (rhs == :isCommentLine)
    print comment
  else
    seq = seq.collect {
      |unit|
      raw2key[unit] || unit
    }
    print "#{broken}    { {#{seq.join(", ")}}, #{rhs} }, // #{comment}\n"
  end
}
