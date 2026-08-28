#include "element.h"

#include <ntqstringlist.h>
#include <ntqtextstream.h>

const char FIELD_SEP = ':';
const char PROPOINT_SEP = ';';
const char XY_SEP = ',';


void Element::init( double value, TQColor valueColor, int valuePattern,
		    const TQString& label, TQColor labelColor )
{
    m_value = value;
    m_valueColor = valueColor;
    if ( valuePattern < TQt::SolidPattern || valuePattern > TQt::DiagCrossPattern )
	valuePattern = TQt::SolidPattern;
    m_valuePattern = valuePattern;
    m_label = label;
    m_labelColor = labelColor;
}


void Element::setValuePattern( int valuePattern )
{
    if ( valuePattern < TQt::SolidPattern || valuePattern > TQt::DiagCrossPattern )
	valuePattern = TQt::SolidPattern;
    m_valuePattern = valuePattern;
}


double Element::proX( int index ) const
{
    Q_ASSERT(index >= 0 && index < MAX_PROPOINTS);
    return m_propoints[2 * index];
}


double Element::proY( int index ) const
{
    Q_ASSERT(index >= 0 && index < MAX_PROPOINTS);
    return m_propoints[(2 * index) + 1];
}


void Element::setProX( int index, double value )
{
    Q_ASSERT(index >= 0 && index < MAX_PROPOINTS);
    m_propoints[2 * index] = value;
}


void Element::setProY( int index, double value )
{
    Q_ASSERT(index >= 0 && index < MAX_PROPOINTS);
    m_propoints[(2 * index) + 1] = value;
}


TQTextStream &operator<<( TQTextStream &s, const Element &element )
{
    s << element.value() << FIELD_SEP
      << element.valueColor().name() << FIELD_SEP
      << element.valuePattern() << FIELD_SEP
      << element.labelColor().name() << FIELD_SEP;

    for ( int i = 0; i < Element::MAX_PROPOINTS; ++i ) {
	s << element.proX( i ) << XY_SEP << element.proY( i );
	s << ( i == Element::MAX_PROPOINTS - 1 ? FIELD_SEP : PROPOINT_SEP );
    }

    s << element.label() << '\n';

    return s;
}


TQTextStream &operator>>( TQTextStream &s, Element &element )
{
    TQString data = s.readLine();
    element.setValue( Element::INVALID );

    int errors = 0;
    bool ok;

    TQStringList fields = TQStringList::split( FIELD_SEP, data );
    if ( fields.count() >= 4 ) {
	double value = fields[0].toDouble( &ok );
	if ( !ok )
	    errors++;
	TQColor valueColor = TQColor( fields[1] );
	if ( !valueColor.isValid() )
	    errors++;
	int valuePattern = fields[2].toInt( &ok );
	if ( !ok )
	    errors++;
	TQColor labelColor = TQColor( fields[3] );
	if ( !labelColor.isValid() )
	    errors++;
	TQStringList propoints = TQStringList::split( PROPOINT_SEP, fields[4] );
	TQString label = data.section( FIELD_SEP, 5 );

	if ( !errors ) {
	    element.set( value, valueColor, valuePattern, label, labelColor );
	    int i = 0;
	    for ( TQStringList::iterator point = propoints.begin();
		i < Element::MAX_PROPOINTS && point != propoints.end();
		++i, ++point ) {
		errors = 0;
		TQStringList xy = TQStringList::split( XY_SEP, *point );
		double x = xy[0].toDouble( &ok );
		if ( !ok || x <= 0.0 || x >= 1.0 )
		    errors++;
		double y = xy[1].toDouble( &ok );
		if ( !ok || y <= 0.0 || y >= 1.0 )
		    errors++;
		if ( errors )
		    x = y = Element::NO_PROPORTION;
		element.setProX( i, x );
		element.setProY( i, y );
	    }
	}
    }

    return s;
}

