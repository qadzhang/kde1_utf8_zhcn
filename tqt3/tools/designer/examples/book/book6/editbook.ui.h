void EditBookForm::primeInsertBook( TQSqlRecord * buffer )
{
    TQSqlQuery query;  
    query.exec( "UPDATE sequence SET sequence = sequence + 1 WHERE tablename='book';" );  
    query.exec( "SELECT sequence FROM sequence WHERE tablename='book';" );  
    if ( query.next() ) {  
	buffer->setValue( "id", query.value( 0 ) );  
    }     
}

