/*
*/

#include "structureparser.h"

#include <ntqstring.h>
#include <ntqlistview.h>

StructureParser::StructureParser( TQListView * t )
                : TQXmlDefaultHandler()
{
    setListView( t );
}

void StructureParser::setListView( TQListView * t )
{
    table = t;
    table->setSorting( -1 );
    table->addColumn( "Qualified name" );
    table->addColumn( "Namespace" );
}

bool StructureParser::startElement( const TQString& namespaceURI,
                                    const TQString& ,
                                    const TQString& qName,
                                    const TQXmlAttributes& attributes)
{
    TQListViewItem * element;

    if ( ! stack.isEmpty() ){
	TQListViewItem *lastChild = stack.top()->firstChild();
	if ( lastChild ) {
	    while ( lastChild->nextSibling() )
		lastChild = lastChild->nextSibling();
	}
	element = new TQListViewItem( stack.top(), lastChild, qName, namespaceURI );
    } else {
	element = new TQListViewItem( table, qName, namespaceURI );
    }
    stack.push( element );
    element->setOpen( true );

    if ( attributes.length() > 0 ) {
	for ( int i = 0 ; i < attributes.length(); i++ ) {
	    new TQListViewItem( element, attributes.qName(i), attributes.uri(i) );
	}
    }
    return true;
}

bool StructureParser::endElement( const TQString&, const TQString&,
                                  const TQString& )
{
    stack.pop();
    return true;
}
