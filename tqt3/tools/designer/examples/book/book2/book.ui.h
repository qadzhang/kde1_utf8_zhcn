void BookForm::primeInsertAuthor( TQSqlRecord * buffer )
{
    TQSqlQuery query;
    query.exec( "UPDATE sequence SET sequence = sequence + 1 WHERE tablename='author';" );
    query.exec( "SELECT sequence FROM sequence WHERE tablename='author';" );
    if ( query.next() ) {
	buffer->setValue( "id", query.value( 0 ) );
    }
}

