#ifndef ELEMENT_H
#define ELEMENT_H

#include <ntqcolor.h>
#include <ntqnamespace.h>
#include <ntqstring.h>
#include <ntqvaluevector.h>

class Element;

typedef TQValueVector<Element> ElementVector;

/*
    Elements are valid if they have a value which is > EPSILON.
*/
const double EPSILON = 0.0000001; // Must be > INVALID.


class Element
{
public:
    enum { INVALID = -1 };
    enum { NO_PROPORTION = -1 };
    enum { MAX_PROPOINTS = 3 }; // One proportional point per chart type

    Element( double value = INVALID, TQColor valueColor = TQt::gray,
	     int valuePattern = TQt::SolidPattern,
	     const TQString& label = TQString::null,
	     TQColor labelColor = TQt::black ) {
	init( value, valueColor, valuePattern, label, labelColor );
	for ( int i = 0; i < MAX_PROPOINTS * 2; ++i )
	    m_propoints[i] = NO_PROPORTION;
    }
    ~Element() {}

    bool isValid() const { return m_value > EPSILON; }

    double value() const { return m_value; }
    TQColor valueColor() const { return m_valueColor; }
    int valuePattern() const { return m_valuePattern; }
    TQString label() const { return m_label; }
    TQColor labelColor() const { return m_labelColor; }
    double proX( int index ) const;
    double proY( int index ) const;

    void set( double value = INVALID, TQColor valueColor = TQt::gray,
	      int valuePattern = TQt::SolidPattern,
	      const TQString& label = TQString::null,
	      TQColor labelColor = TQt::black ) {
	init( value, valueColor, valuePattern, label, labelColor );
    }
    void setValue( double value ) { m_value = value; }
    void setValueColor( TQColor valueColor ) { m_valueColor = valueColor; }
    void setValuePattern( int valuePattern );
    void setLabel( const TQString& label ) { m_label = label; }
    void setLabelColor( TQColor labelColor ) { m_labelColor = labelColor; }
    void setProX( int index, double value );
    void setProY( int index, double value );

private:
    void init( double value, TQColor valueColor, int valuePattern,
	       const TQString& label, TQColor labelColor );

    double m_value;
    TQColor m_valueColor;
    int m_valuePattern;
    TQString m_label;
    TQColor m_labelColor;
    double m_propoints[2 * MAX_PROPOINTS];
};


TQTextStream &operator<<( TQTextStream&, const Element& );
TQTextStream &operator>>( TQTextStream&, Element& );

#endif
