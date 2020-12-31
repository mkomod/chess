#include <iostream>


std::string PIECES[] = {
    "empty", "w-pawn", "w-knight", "w-bishop", "w-rook", "w-queen", "w-king",
	     "b-pawn", "b-knight", "b-bishop", "b-rook", "b-queen", "b-king"
};

std::string boardPieces[] = {
    " ", "♙", "♘", "♗", "♖", "♕", "♔", "♟", "♞", "♝", "♜", "♛", "♚"
};

enum Pieces {
    na,
    wp, wkn, wb, wr, wq, wk, // white pieces
    bp, bkn, bb, br, bq, bk // black pieces
};

enum Turn {
    white, black
};

struct Game {
    Turn turn;	// 0: white, 1: black
    bool status;
    bool whiteCanCastle;
    bool blackCanCastle;
    bool whiteInCheck;
    bool blackInCheck;
    int whiteKingPos[2];
    int blackKingPos[2];
    Pieces board[8][8];

    void initGame();
    bool Move(char move[4]);
    bool moveBishop(int, int, int, int);
    bool moveKing(int, int, int, int);
    bool moveKnight(int, int, int, int);
    bool movePawn(int, int, int, int);
    bool moveQueen(int, int, int, int);
    bool moveRook(int, int, int, int);
    bool inCheck(Turn, int tx = -1, int ty = -1);
    bool hasOppPiece(int, int);
};

std::ostream& operator<< (std::ostream &os, Game g);


int
main() {
    std::clog.setstate(std::ios_base::failbit);
    Game game;
    game.initGame();
    char move[4];
    bool validMove = false;
    while (game.status) {
	system("clear");
	std::cout << "\n\n\n\n\n" << game;
	std::cin >> move;
	validMove = false;
	if (move[0] == 'q') {
	    break;
	}
	while (!validMove) {
	    validMove = game.Move(move);
	    if (!validMove) {
		std::cout << "Invalid move\n";
		std::cin >> move;
	    } else {
		break;
	    }
	}
    }
    return 0;
}


std::ostream& operator<< (std::ostream &os, Game g) {
    char letters[8] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'};
    Pieces p = na;
    for (int i = 7; i >= 0; --i) {
	std::cout << i + 1 << "\t";
	for (int j = 0; j < 8; ++ j) {
	    p = g.board[j][i];
	    std::cout << boardPieces[p] << "\t";
	}
	std::cout << "\n";
    }
    std::cout << "\t";
    for (int i = 0; i != 8; ++i) {
	std::cout << letters[i] << "\t";
    }
    bool in_check = g.turn == white ? g.whiteInCheck : g.blackInCheck;
    std::cout << "\n" << "Turn: " << g.turn << "\t In check: " << in_check << 
	"\t Move: ";
    return os;
}


void Game::initGame() {
    turn = white;
    status = 1;
    whiteCanCastle = true;
    blackCanCastle = true;
    whiteInCheck = false;
    blackInCheck = false;
    whiteKingPos[0] = 4;
    whiteKingPos[1] = 0;
    blackKingPos[0] = 4;
    blackKingPos[1] = 7;

    Pieces nboard[8][8] = {
	{wr,  wp, na, na, na, na, bp, br},
	{wkn, wp, na, na, na, na, bp, bkn},
	{wb,  wp, na, na, na, na, bp, bb},
	{wq,  wp, na, na, na, na, bp, bq},
	{wk,  wp, na, na, na, na, bp, bk},
	{wb,  wp, na, na, na, na, bp, bb},
	{wkn, wp, na, na, na, na, bp, bkn},
	{wr,  wp, na, na, na, na, bp, br},
    };
    std::copy(&nboard[0][0], &nboard[7][7] + 1, &board[0][0]);
}


bool Game::Move(char move[4]) {
    bool valid = true;
    int fx = (int) move[0] - 97;
    int fy = (int) move[1] - 49;
    int tx = (int) move[2] - 97;
    int ty = (int) move[3] - 49;
    Pieces from = board[fx][fy];
   
    std::clog << PIECES[(int) from] << "\t";
    std::clog << "(" << fx << ", " << fy << ")\t(" << tx << ", " << ty << ")\n";

    // move within board
    if ((fx >= 0 && fx <= 7) && !(fy >= 0 && fy <= 7) &&
	(tx >= 0 && tx <= 7) && !(ty >= 0 && ty <= 7)) {
	std::clog << "Moving out of board";
	return false;
    }

    // not moving
    if (fx == tx && fy == ty) {
	std::clog << "Moving to same square";
	return false;
    }

    // moving piece of same color
    if (hasOppPiece(fx, fy)) {
	std::clog << "Trying to move opponents piece";
	return false;
    }
    
    switch(from) {
	case wp : case bp:
	    valid = movePawn(fx, fy, tx, ty);
	    break;
	case wkn : case bkn:
	    valid = moveKnight(fx, fy, tx, ty);
	    break;
	case wb : case bb:
	    valid = moveBishop(fx, fy, tx, ty);
	    break;
	case wr : case br:
	    valid = moveRook(fx, fy, tx, ty);
	    break;
	case wq : case bq:
	    valid = moveQueen(fx, fy, tx, ty);
	    break;
	case wk : case bk:
	    valid = moveKing(fx, fy, tx, ty);
	    break;
	default :
	    return false;
    }

    if (valid) {
	board[tx][ty] = from;
	board[fx][fy] = na;

	// check to see if we're in check
	whiteInCheck = inCheck(white);
	blackInCheck = inCheck(black);

	// if the player has made a move and is still in check then
	// the move is invalid
	if ((turn == white && whiteInCheck == true) || 
	    (turn == black && blackInCheck == true)) {
	    board[tx][ty] = na;
	    board[fx][fy] = from;
	    return false;
	}
	
	// if me made a king move which is valid and we're not in check
	// then we update the kings position and can no longer castle
	if (from == wk) {
	    whiteCanCastle = false; 
	} else if (from == bk) {
	    blackCanCastle = false;
	}
	
	// change turn
	turn == white ? turn = black : turn = white;	
    }

    return valid;
}


bool Game::hasOppPiece(int tx, int ty) {
    // checks if a square has a pieces of the opposite colour
    bool hasOpponentsPiece = true;
    Pieces tpiece = board[tx][ty];

    if (turn == white && !(tpiece == bp || tpiece == bkn || 
	  tpiece == bb || tpiece == br || tpiece == bq)) {
	hasOpponentsPiece = false;
    }
    if (turn == black && !(tpiece == wp || tpiece == wkn || 
	  tpiece == wb || tpiece == wr || tpiece == wq)) {
	hasOpponentsPiece = false;
    }
    return hasOpponentsPiece;
}


bool Game::movePawn(int fx, int fy, int tx, int ty) {
    Pieces tpiece = board[tx][ty];
    
    // is the pawn capturing a piece
    if ((tx == fx - 1 || tx == fx + 1) && !hasOppPiece(tx, ty)) {
	std::clog << "Moving out of column and not capturing\n";
	return false;
    }

    // is the pawn on the starting row and moving two steps
    if (turn == white && !(ty == fy + 1 || (fy == 1 && ty == fy + 2))) {
	std::clog << "W: Moving more than one square or" << 
	    " more than two when not on starting row\n";
	return false;
    }

    if (turn == black && !(ty == fy - 1 || (fy == 6 && ty == fy - 2))) {
	std::clog << "B: Moving more than one square or" << 
	    " more than two when not on starting row\n";
	return false;
    }

    return true;
}


bool Game::moveKnight(int fx, int fy, int tx, int ty) {
    Pieces tpiece = board[tx][ty];
    
    bool hasOppPieceOrEmpty = (tpiece == na) || hasOppPiece(tx, ty);
    bool correctMovement = (
	((tx == fx + 2 || tx == fx - 2) && (ty == fy - 1 || ty == fy + 1)) ||
        ((tx == fx + 1 || tx == fx - 1) && (ty == fy - 2 || ty == fy + 2)));

    if (!(hasOppPieceOrEmpty && correctMovement)) {
	std::clog << "Moving to non-empty: " << hasOppPieceOrEmpty << 
	    "\t" << "Moving in correct manner: " << correctMovement;
	return false;
    }
	
    return true;
}


bool Game::moveBishop(int fx, int fy, int tx, int ty) {
    Pieces tpiece = board[tx][ty];
    
    // if the bishop is moving diagonally then the gradient of the
    // movement is 1 or -1
    float grad = ((float) fx - (float) tx) / ((float) fy - (float) ty);
    if (!(grad == 1 || grad == -1)) {
	std::clog << "Not moving diagonally, grad:" << grad << "\n";
	return false;
    }

    int decx = tx < fx ? -1 : 1;
    int decy = ty < fy ? -1 : 1;
    int j = fy + decy;
    
    // are the squares between the final and starting square are not obstructed
    for (int i = fx + decx; i != tx; i += decx) {
	if (board[i][j] != na) {
	    std::clog << "(" << i <<  ", " << j << ") not empty\n";
	    return false;
	}
	j += decy;
    }

    if (!(hasOppPiece(tx, ty) || tpiece == na))
	return false;

    return true;
}


bool Game::moveRook(int fx, int fy, int tx, int ty) {
    // not moving in straight line
    if (!((fx == tx && fy != ty) || (fx != tx && fy == ty))) {
	std::clog << "Rook not moving in straight line\n";
	return false;
    }
    
    // are the squares between the final and starting square are not obstructed
    if (fx == tx) {
	int decy = ty < fy ? -1 : 1;
	for (int j = fy + decy; j != ty; j += decy) {
	    if (board[fx][j] != na) {
		std::clog << "(" << fx <<  ", " << j << ") not empty\n";
		return false;
	    }
	}
    } 

    if (fy == ty) {
	int decx = tx < fx ? -1 : 1;
	for (int i = fx + decx; i != tx; i += decx) {
	    if (board[i][fy] != na) {
		std::clog << "(" << i <<  ", " << fy << ") not empty\n";
		return false;
	    }
	}
    }

    if (!(hasOppPiece(tx, ty) || board[tx][ty] == na))
	return false;

    return true;
}


bool Game::moveQueen(int fx, int fy, int tx, int ty) {
    // moves like rook or bishop
    if (!(moveRook(fx, fy, tx, ty) || moveBishop(fx, fy, tx, ty)))
	return false;
    return true;
}


bool Game::moveKing(int fx, int fy, int tx, int ty) {
    if ((blackCanCastle && !blackInCheck) || 
	(whiteCanCastle && ! whiteInCheck)) {

	int cty = -1;
	Pieces rook = na;

	if (turn == white && whiteCanCastle) {
	    cty = 0;
	    rook = wr;
	} else if (turn == black && blackCanCastle) {
	    cty = 7;
	    rook = br;
	}

	// castle kings
	if (fx == 4 && tx == 6 && ty == cty && 
		board[5][cty] == na && board[6][cty] == na) {
	    board[7][cty] = na;
	    board[5][cty] = rook;
	    return true;
	}
	// castle queens
	if (fx == 4 && tx == 2 && ty == cty && board[1][cty] == na && 
		board[2][cty] == na && board[3][cty] == na) {
	    board[0][cty] = na;
	    board[3][cty] = rook;
	    return true;
	}
    }

    if (!(tx == fx + 1 || tx == fx - 1 || ty == fy + 1 || ty == fy - 1)) {
	return false;
    }
    
    if (!(hasOppPiece(tx, ty) || board[tx][ty] == na))
	return false;
   
    if (inCheck(turn, tx, ty))
	return false;
    
    return true;
}


bool Game::inCheck(Turn sideToCheck, int tx, int ty) {
    bool inCheck = false;
    bool switched = false;
    Pieces from = na;

    
    // locate the king if tx, ty not given
    if (tx == -1 && ty == -1) {
	for (int i = 0; i != 8; ++i) {
	    for (int j = 0; j != 8; ++j) {
		if (sideToCheck == white && board[i][j] == wk) {
		    tx = i;
		    ty = j;
		} else if (sideToCheck == black && board[i][j] == bk) {
		    tx = i;
		    ty = j;
		}
	    }
	}
    }

    sideToCheck == white ? board[tx][ty] = wp : board[tx][ty] = bp;
    if (sideToCheck == white && turn == white) {
	turn = black;
	switched = true;
    } else if (sideToCheck == black && turn == black) {
	turn = white;
	switched = true;
    }
    
    for (int i = 0; i != 8; ++i) {
	for (int j = 0; j != 8; ++j) {
	    from = board[i][j];
	    if (sideToCheck == white) {
		// check if any of the black pieces can deliver check
		switch(from) {
		    case bp:
			inCheck = movePawn(i, j, tx, ty);
			std::clog << "BP" << "\t C: " << inCheck << "\n";
			break;
		    case bkn:
			inCheck = moveKnight(i, j, tx, ty);
			std::clog << "BK" << "\t C: " << inCheck << "\n";
			break;
		    case bb:
			inCheck = moveBishop(i, j, tx, ty);
			std::clog << "BB" << "\t C: " << inCheck << "\n";
			break;
		    case br:
			inCheck = moveRook(i, j, tx, ty);
			std::clog << "BR" << "\t C: " << inCheck << "\n";
			break;
		    case bq:
			inCheck = moveQueen(i, j, tx, ty);
			std::clog << "BQ" << "\t C: " << inCheck << "\n";
			break;
		    default :
			inCheck = false;
		}
	    } else {
		switch(from) {
		    case wp:
			inCheck = movePawn(i, j, tx, ty);
			std::clog << "WP" << "\t C: " << inCheck << "\n";
			break;
		    case wkn:
			inCheck = moveKnight(i, j, tx, ty);
			std::clog << "WK" << "\t C: " << inCheck << "\n";
			break;
		    case wb:
			inCheck = moveBishop(i, j, tx, ty);
			std::clog << "WB" << "\t C: " << inCheck << "\n";
			break;
		    case wr:
			inCheck = moveRook(i, j, tx, ty);
			std::clog << "WR" << "\t C: " << inCheck << "\n";
			break;
		    case wq:
			inCheck = moveQueen(i, j, tx, ty);
			std::clog << "WQ" << "\t C: " << inCheck << "\n";
			break;
		    default :
			inCheck = false;
		}
	    }
	    // stop searching through pieces if we're in check
	    if (inCheck) goto checkFound;
	}
    }
    
checkFound:
    // set the piece back to a king
    sideToCheck == white ? board[tx][ty] = wk : board[tx][ty] = bk;
    if (switched) {
	turn == white ? turn = black : turn = white;
    }

    return inCheck;
}
