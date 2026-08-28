/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef TICTAC_H
#define TICTAC_H


#include <ntqpushbutton.h>
#include <ntqptrvector.h>

class TQComboBox;
class TQLabel;


// --------------------------------------------------------------------------
// TicTacButton implements a single tic-tac-toe button
//

class TicTacButton : public TQPushButton
{
    TQ_OBJECT
public:
    TicTacButton( TQWidget *parent );
    enum Type { Blank, Circle, Cross };
    Type	type() const		{ return t; }
    void	setType( Type type )	{ t = type; repaint(); }
    TQSizePolicy sizePolicy() const
    { return TQSizePolicy( TQSizePolicy::Preferred, TQSizePolicy::Preferred ); }
    TQSize sizeHint() const { return TQSize( 32, 32 ); }
    TQSize minimumSizeHint() const { return TQSize( 10, 10 ); }
protected:
    void	drawButtonLabel( TQPainter * );
private:
    Type t;
};

// Using template vector to make vector-class of TicTacButton.
// This vector is used by the TicTacGameBoard class defined below.

typedef TQPtrVector<TicTacButton>	TicTacButtons;
typedef TQMemArray<int>		TicTacArray;


// --------------------------------------------------------------------------
// TicTacGameBoard implements the tic-tac-toe game board.
// TicTacGameBoard is a composite widget that contains N x N TicTacButtons.
// N is specified in the constructor.
//

class TicTacGameBoard : public TQWidget
{
    TQ_OBJECT
public:
    TicTacGameBoard( int n, TQWidget *parent=0, const char *name=0 );
   ~TicTacGameBoard();
    enum	State { Init, HumansTurn, HumanWon, ComputerWon, NobodyWon };
    State	state() const		{ return st; }
    void	computerStarts( bool v );
    void        newGame();
signals:
    void	finished();			// game finished
private slots:
    void	buttonClicked();
private:
    void        setState( State state ) { st = state; }
    void	updateButtons();
    int	checkBoard( TicTacArray * );
    void	computerMove();
    State	st;
    int		nBoard;
    bool	comp_starts;
    TicTacArray *btArray;
    TicTacButtons *buttons;
};


// --------------------------------------------------------------------------
// TicTacToe implements the complete game.
// TicTacToe is a composite widget that contains a TicTacGameBoard and
// two push buttons for starting the game and quitting.
//

class TicTacToe : public TQWidget
{
    TQ_OBJECT
public:
    TicTacToe( int boardSize=3, TQWidget *parent=0, const char *name=0 );
private slots:
    void	newGameClicked();
    void	gameOver();
private:
    void	newState();
    TQComboBox	*whoStarts;
    TQPushButton *newGame;
    TQPushButton *quit;
    TQLabel	*message;
    TicTacGameBoard *board;
};


#endif // TICTAC_H
