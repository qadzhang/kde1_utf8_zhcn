/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "tictac.h"
#include <ntqapplication.h>
#include <ntqpainter.h>
#include <ntqdrawutil.h>
#include <ntqcombobox.h>
#include <ntqcheckbox.h>
#include <ntqlabel.h>
#include <ntqlayout.h>
#include <stdlib.h>				// rand() function
#include <ntqdatetime.h>				// seed for rand()


//***************************************************************************
//* TicTacButton member functions
//***************************************************************************

// --------------------------------------------------------------------------
// Creates a TicTacButton
//

TicTacButton::TicTacButton( TQWidget *parent ) : TQPushButton( parent )
{
    t = Blank;					// initial type
}

// --------------------------------------------------------------------------
// Paints TicTacButton
//

void TicTacButton::drawButtonLabel( TQPainter *p )
{
    TQRect r = rect();
    p->setPen( TQPen( white,2 ) );		// set fat pen
    if ( t == Circle ) {
	p->drawEllipse( r.left()+4, r.top()+4, r.width()-8, r.height()-8 );
    } else if ( t == Cross ) {			// draw cross
	p->drawLine( r.topLeft()   +TQPoint(4,4), r.bottomRight()-TQPoint(4,4));
	p->drawLine( r.bottomLeft()+TQPoint(4,-4),r.topRight()   -TQPoint(4,-4));
    }
}


//***************************************************************************
//* TicTacGameBoard member functions
//***************************************************************************

// --------------------------------------------------------------------------
// Creates a game board with N x N buttons and connects the "clicked()"
// signal of all buttons to the "buttonClicked()" slot.
//

TicTacGameBoard::TicTacGameBoard( int n, TQWidget *parent, const char *name )
    : TQWidget( parent, name )
{
    st = Init;					// initial state
    nBoard = n;
    n *= n;					// make square
    comp_starts = false;			// human starts
    buttons = new TicTacButtons(n);		// create real buttons
    btArray = new TicTacArray(n);		// create button model
    TQGridLayout * grid = new TQGridLayout( this, nBoard, nBoard, 4 );
    TQPalette p( blue );
    for ( int i=0; i<n; i++ ) {			// create and connect buttons
	TicTacButton *ttb = new TicTacButton( this );
	ttb->setPalette( p );
	ttb->setEnabled( false );
	connect( ttb, TQ_SIGNAL(clicked()), TQ_SLOT(buttonClicked()) );
	grid->addWidget( ttb, i%nBoard, i/nBoard );
	buttons->insert( i, ttb );
	btArray->at(i) = TicTacButton::Blank;	// initial button type
    }
    TQTime t = TQTime::currentTime();		// set random seed
    srand( t.hour()*12+t.minute()*60+t.second()*60 );
}

TicTacGameBoard::~TicTacGameBoard()
{
    delete buttons;
    delete btArray;
}


// --------------------------------------------------------------------------
// TicTacGameBoard::computerStarts( bool v )
//
// Computer starts if v=true. The human starts by default.
//

void TicTacGameBoard::computerStarts( bool v )
{
    comp_starts = v;
}


// --------------------------------------------------------------------------
// TicTacGameBoard::newGame()
//
// Clears the game board and prepares for a new game
//

void TicTacGameBoard::newGame()
{
    st = HumansTurn;
    for ( int i=0; i<nBoard*nBoard; i++ )
	btArray->at(i) = TicTacButton::Blank;
    if ( comp_starts )
	computerMove();
    else
	updateButtons();
}


// --------------------------------------------------------------------------
// TicTacGameBoard::buttonClicked()		- TQ_SLOT
//
// This slot is activated when a TicTacButton emits the signal "clicked()",
// i.e. the user has clicked on a TicTacButton.
//

void TicTacGameBoard::buttonClicked()
{
    if ( st != HumansTurn )			// not ready
	return;
    int i = buttons->findRef( (TicTacButton*)sender() );
    TicTacButton *b = buttons->at(i);		// get piece that was pressed
    if ( b->type() == TicTacButton::Blank ) {	// empty piece?
	btArray->at(i) = TicTacButton::Circle;
	updateButtons();
	if ( checkBoard( btArray ) == 0 )	// not a winning move?
	    computerMove();
	int s = checkBoard( btArray );
	if ( s ) {				// any winners yet?
	    st = s == TicTacButton::Circle ? HumanWon : ComputerWon;
	    emit finished();
	}
    }
}


// --------------------------------------------------------------------------
// TicTacGameBoard::updateButtons()
//
// Updates all buttons that have changed state
//

void TicTacGameBoard::updateButtons()
{
    for ( int i=0; i<nBoard*nBoard; i++ ) {
	if ( buttons->at(i)->type() != btArray->at(i) )
	    buttons->at(i)->setType( (TicTacButton::Type)btArray->at(i) );
	buttons->at(i)->setEnabled( buttons->at(i)->type() ==
				    TicTacButton::Blank );
    }
}


// --------------------------------------------------------------------------
// TicTacGameBoard::checkBoard()
//
// Checks if one of the players won the game, works for any board size.
//
// Returns:
//  - TicTacButton::Cross  if the player with X buttons won
//  - TicTacButton::Circle if the player with O buttons won
//  - Zero (0) if there is no winner yet
//

int TicTacGameBoard::checkBoard( TicTacArray *a )
{
    int  t = 0;
    int  row, col;
    bool won = false;
    for ( row=0; row<nBoard && !won; row++ ) {	// check horizontal
	t = a->at(row*nBoard);
	if ( t == TicTacButton::Blank )
	    continue;
	col = 1;
	while ( col<nBoard && a->at(row*nBoard+col) == t )
	    col++;
	if ( col == nBoard )
	    won = true;
    }
    for ( col=0; col<nBoard && !won; col++ ) {	// check vertical
	t = a->at(col);
	if ( t == TicTacButton::Blank )
	    continue;
	row = 1;
	while ( row<nBoard && a->at(row*nBoard+col) == t )
	    row++;
	if ( row == nBoard )
	    won = true;
    }
    if ( !won ) {				// check diagonal top left
	t = a->at(0);				//   to bottom right
	if ( t != TicTacButton::Blank ) {
	    int i = 1;
	    while ( i<nBoard && a->at(i*nBoard+i) == t )
		i++;
	    if ( i == nBoard )
		won = true;
	}
    }
    if ( !won ) {				// check diagonal bottom left
	int j = nBoard-1;			//   to top right
	int i = 0;
	t = a->at(i+j*nBoard);
	if ( t != TicTacButton::Blank ) {
	    i++; j--;
	    while ( i<nBoard && a->at(i+j*nBoard) == t ) {
		i++; j--;
	    }
	    if ( i == nBoard )
		won = true;
	}
    }
    if ( !won )					// no winner
	t = 0;
    return t;
}


// --------------------------------------------------------------------------
// TicTacGameBoard::computerMove()
//
// Puts a piece on the game board. Very, very simple.
//

void TicTacGameBoard::computerMove()
{
    int numButtons = nBoard*nBoard;
    int *altv = new int[numButtons];		// buttons alternatives
    int altc = 0;
    int stopHuman = -1;
    TicTacArray a = btArray->copy();
    int i;
    for ( i=0; i<numButtons; i++ ) {		// try all positions
	if ( a[i] != TicTacButton::Blank )	// already a piece there
	    continue;
	a[i] = TicTacButton::Cross;		// test if computer wins
	if ( checkBoard(&a) == a[i] ) {		// computer will win
	    st = ComputerWon;
	    stopHuman = -1;
	    break;
	}
	a[i] = TicTacButton::Circle;		// test if human wins
	if ( checkBoard(&a) == a[i] ) {		// oops...
	    stopHuman = i;			// remember position
	    a[i] = TicTacButton::Blank;		// restore button
	    continue;				// computer still might win
	}
	a[i] = TicTacButton::Blank;		// restore button
	altv[altc++] = i;			// remember alternative
    }
    if ( stopHuman >= 0 )			// must stop human from winning
	a[stopHuman] = TicTacButton::Cross;
    else if ( i == numButtons ) {		// tried all alternatives
	if ( altc > 0 )				// set random piece
	    a[altv[rand()%(altc--)]] = TicTacButton::Cross;
	if ( altc == 0 ) {			// no more blanks
	    st = NobodyWon;
	    emit finished();
	}
    }
    *btArray = a;				// update model
    updateButtons();				// update buttons
    delete[] altv;
}


//***************************************************************************
//* TicTacToe member functions
//***************************************************************************

// --------------------------------------------------------------------------
// Creates a game widget with a game board and two push buttons, and connects
// signals of child widgets to slots.
//

TicTacToe::TicTacToe( int boardSize, TQWidget *parent, const char *name )
    : TQWidget( parent, name )
{
    TQVBoxLayout * l = new TQVBoxLayout( this, 6 );

    // Create a message label

    message = new TQLabel( this );
    message->setFrameStyle( TQFrame::WinPanel | TQFrame::Sunken );
    message->setAlignment( AlignCenter );
    l->addWidget( message );

    // Create the game board and connect the signal finished() to this
    // gameOver() slot

    board = new TicTacGameBoard( boardSize, this );
    connect( board, TQ_SIGNAL(finished()), TQ_SLOT(gameOver()) );
    l->addWidget( board );

    // Create a horizontal frame line

    TQFrame *line = new TQFrame( this );
    line->setFrameStyle( TQFrame::HLine | TQFrame::Sunken );
    l->addWidget( line );

    // Create the combo box for deciding who should start, and
    // connect its clicked() signals to the buttonClicked() slot

    whoStarts = new TQComboBox( this );
    whoStarts->insertItem( "Computer starts" );
    whoStarts->insertItem( "Human starts" );
    l->addWidget( whoStarts );

    // Create the push buttons and connect their clicked() signals
    // to this right slots.

    newGame = new TQPushButton( "Play!", this );
    connect( newGame, TQ_SIGNAL(clicked()), TQ_SLOT(newGameClicked()) );
    quit = new TQPushButton( "Quit", this );
    connect( quit, TQ_SIGNAL(clicked()), tqApp, TQ_SLOT(quit()) );
    TQHBoxLayout * b = new TQHBoxLayout;
    l->addLayout( b );
    b->addWidget( newGame );
    b->addWidget( quit );

    newState();
}


// --------------------------------------------------------------------------
// TicTacToe::newGameClicked()			- TQ_SLOT
//
// This slot is activated when the new game button is clicked.
//

void TicTacToe::newGameClicked()
{
    board->computerStarts( whoStarts->currentItem() == 0 );
    board->newGame();
    newState();
}


// --------------------------------------------------------------------------
// TicTacToe::gameOver()			- TQ_SLOT
//
// This slot is activated when the TicTacGameBoard emits the signal
// "finished()", i.e. when a player has won or when it is a draw.
//

void TicTacToe::gameOver()
{
    newState();					// update text box
}


// --------------------------------------------------------------------------
// Updates the message to reflect a new state.
//

void TicTacToe::newState()
{
    static const char *msg[] = {		// TicTacGameBoard::State texts
	"Click Play to start", "Make your move",
	"You won!", "Computer won!", "It's a draw" };
    message->setText( msg[board->state()] );
    return;
}
