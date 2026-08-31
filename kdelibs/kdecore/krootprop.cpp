/* This file is part of the KDE libraries
    Copyright (C) 1997 Mark Donohoe (donohoe@kde.org)

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/

#include "krootprop.h"

KRootProp::KRootProp()
{
	kde_display = KApplication::desktop()->x11Display();
	screen = DefaultScreen(kde_display);
    root = RootWindow(kde_display, screen);
	at = 0;
}

KRootProp::~KRootProp()
{
	sync();
}

void KRootProp::sync()
{
	if ( !propDict.isEmpty() ) {
	
		QDictIterator <QString> it( propDict );
		QString propString;
		QString keyvalue;

    	while ( it.current() ) {

			QString *value = propDict.find( it.currentKey() );
        	keyvalue.sprintf( "%s=%s\n", it.currentKey(), value->data() );
			propString += keyvalue;
        	++it;
		}

		XChangeProperty(kde_display, root, at,
			XA_STRING, 8, PropModeReplace,
			// [2026-08-31] 长度须按字节计：data() 输出 UTF-8 字节而 length()
			// 是 UTF-16 字符数，含中文即截断（与 kwm.cpp setQStringProperty 同族修法）
			(unsigned char *)propString.data(),
			strlen( propString.data() ) );

		propDict.clear();
	}
}

void KRootProp::setProp( const QString& rProp )
{
	Atom type;
	int format;
	unsigned long nitems;
	unsigned long bytes_after;
	char *buf;
	
	// If a property has already been opened write
	// the dictionary back to the root window
	
	if( at )
		sync();

	if( !rProp.isEmpty() ) {
  		at = XInternAtom( kde_display, rProp.data(), False);
		
		XGetWindowProperty( kde_display, root, at, 0, 256,
			False, XA_STRING, &type, &format, &nitems, &bytes_after,
			(unsigned char **)&buf);
			
		// Parse through the property string stripping out key value pairs
		// and putting them in the dictionary
		
		QString s(buf);
		QString keypair;
		int i=0;
		QString key;
		QString value;
		
		while(s.length() >0 ) {
			
			// parse the string for first key-value pair separator '\n'
			
			i = s.find("\n");
			if(i == -1)
				i = s.length();
		
			// extract the key-values pair and remove from string
			
			keypair = s.left(i);
			s.remove(0,i+1);
			
			// split key and value and add to dictionary
			
			keypair.simplifyWhiteSpace();
			
			i = keypair.find( "=" );
			if( i != -1 ) {
				key = keypair.left( i );
				value = keypair.right( keypair.length() - i - 1 );
				propDict.insert( key.data(), new QString( value.data() ) );
			}
		}
	}
}

QString KRootProp::readEntry( const QString& rKey, 
			    const char* pDefault ) const 
{
	// [2026-08-31] 修复原实现两处缺陷：① key 不存在且给了 pDefault 时对 NULL
	// 指针调 sprintf 必崩（原代码 aValue 从未指向有效对象）；② pDefault 被当
	// 格式串——内含 % 会按格式符读取不存在的变参（UB），改为直接按 UTF-8 构串
	if( !propDict.isEmpty() ) {

		QString *aValue = propDict[ rKey.data() ];

		if ( aValue )
			return *aValue;
	}

	QString aValue;

	if ( pDefault )
		aValue = TQString::fromUtf8( pDefault );

	return aValue;
}

int KRootProp::readNumEntry( const QString& rKey, int nDefault ) const
{
  bool ok;
  int rc;

  QString aValue = readEntry( rKey );
  if( aValue.isNull() )
	return nDefault;
  else
	{
	  rc = aValue.toInt( &ok );
	  return( ok ? rc : 0 );
	}
}


QFont KRootProp::readFontEntry( const QString& rKey, 
							  const QFont* pDefault ) const
{
  QFont aRetFont;

  QString aValue = readEntry( rKey );
  if( !aValue.isNull() )
	{
	  // find first part (font family)
	  int nIndex = aValue.find( ',' );
	  if( nIndex == -1 )
		return aRetFont;
	  aRetFont.setFamily( aValue.left( nIndex ) );
	  
	  // find second part (point size)
	  int nOldIndex = nIndex;
	  nIndex = aValue.find( ',', nOldIndex+1 );
	  if( nIndex == -1 )
		return aRetFont;
	  aRetFont.setPointSize( aValue.mid( nOldIndex+1, 
										 nIndex-nOldIndex-1 ).toInt() );

	  // find third part (style hint)
	  nOldIndex = nIndex;
	  nIndex = aValue.find( ',', nOldIndex+1 );
	  if( nIndex == -1 )
		return aRetFont;
	  aRetFont.setStyleHint( (QFont::StyleHint)aValue.mid( nOldIndex+1, 
													nIndex-nOldIndex-1 ).toUInt() );

	  // find fourth part (char set)
	  nOldIndex = nIndex;
	  nIndex = aValue.find( ',', nOldIndex+1 );
	  if( nIndex == -1 )
		return aRetFont;
	  aRetFont.setCharSet( (QFont::CharSet)aValue.mid( nOldIndex+1, 
									   nIndex-nOldIndex-1 ).toUInt() );

	  // find fifth part (weight)
	  nOldIndex = nIndex;
	  nIndex = aValue.find( ',', nOldIndex+1 );
	  if( nIndex == -1 )
		return aRetFont;
	  aRetFont.setWeight( aValue.mid( nOldIndex+1,
									  nIndex-nOldIndex-1 ).toUInt() );

	  // find sixth part (font bits)
	  uint nFontBits = aValue.right( aValue.length()-nIndex-1 ).toUInt();
	  if( nFontBits & 0x01 )
		aRetFont.setItalic( true );
	  if( nFontBits & 0x02 )
		aRetFont.setUnderline( true );
	  if( nFontBits & 0x04 )
		aRetFont.setStrikeOut( true );
	  if( nFontBits & 0x08 )
		aRetFont.setFixedPitch( true );
	  if( nFontBits & 0x20 )
		aRetFont.setRawMode( true );
	}
  else if( pDefault )
	aRetFont = *pDefault;

  return aRetFont;
}


QColor KRootProp::readColorEntry( const QString& rKey,
								const QColor* pDefault ) const
{
  QColor aRetColor;
  int nRed = 0, nGreen = 0, nBlue = 0;

  QString aValue = readEntry( rKey );
  if( !aValue.isNull() )
	{
  	  bool bOK;
	  
	  // Support #ffffff style colour naming.
	  // Help ease transistion from legacy KDE setups
	  if( aValue.find("#") == 0 ) {
	  	aRetColor.setNamedColor( aValue );
		return aRetColor;
	  }
		
	  // find first part (red)
	  int nIndex = aValue.find( ',' );
	  if( nIndex == -1 )
		return aRetColor;
	  nRed = aValue.left( nIndex ).toInt( &bOK );
	  
	  // find second part (green)
	  int nOldIndex = nIndex;
	  nIndex = aValue.find( ',', nOldIndex+1 );
	  if( nIndex == -1 )
		return aRetColor;
	  nGreen = aValue.mid( nOldIndex+1,
						   nIndex-nOldIndex-1 ).toInt( &bOK );

	  // find third part (blue)
	  nBlue = aValue.right( aValue.length()-nIndex-1 ).toInt( &bOK );

	  aRetColor.setRgb( nRed, nGreen, nBlue );
	}
  else if( pDefault )
	aRetColor = *pDefault;

  return aRetColor;
}

QString KRootProp::writeEntry( const QString& rKey, const QString& rValue )
{
	// [2026-08-31] 修复原实现：先 new 再按存在性改指导致首对象泄漏；且 aValue
	// 永不为 NULL，尾部判空 sprintf 是反逻辑死代码——QDict::replace 本就完成
	// 「覆盖旧值并接管其删除」，直接写入即可
	propDict.replace( rKey.data(), new QString( rValue.data() ) );

	return rValue;
}

QString KRootProp::writeEntry( const QString& rKey, int nValue )
{
  QString aValue;

  aValue.setNum( nValue );

  return writeEntry( rKey, aValue );
}

QString KRootProp::writeEntry( const QString& rKey, const QFont& rFont )
{
  QString aValue;
  UINT8 nFontBits = 0;
  // this mimics get_font_bits() from qfont.cpp
  if( rFont.italic() )
	nFontBits = nFontBits | 0x01;
  if( rFont.underline() )
	nFontBits = nFontBits | 0x02;
  if( rFont.strikeOut() )
	nFontBits = nFontBits | 0x04;
  if( rFont.fixedPitch() )
	nFontBits = nFontBits | 0x08;
  if( rFont.rawMode() )
	nFontBits = nFontBits | 0x20;

  aValue.sprintf( "%s,%d,%d,%d,%d,%d", rFont.family(), rFont.pointSize(),
				  rFont.styleHint(), rFont.charSet(), rFont.weight(),
				  nFontBits );

  return writeEntry( rKey, aValue );
}

QString KRootProp::writeEntry( const QString& rKey, const QColor& rColor )
{
  QString aValue;
  aValue.sprintf( "%d,%d,%d", rColor.red(), rColor.green(), rColor.blue() );

  return writeEntry( rKey, aValue );
}
