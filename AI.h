#ifndef _AI_H_
#define _AI_H_

#include "Utility.h"
#include "Game.h"
#include <queue>
#include <stack>
using namespace std;

struct BfsCoords {
	int d;
	int v;
	int h;

	BfsCoords(int dI, int vI, int hI) : d(dI), v(vI), h(hI) {}
};

class AI {
public:
	friend class AITrainer;

	vector<pair<char, BfsCoords>> moveSet = {
		{'R', {0, 0, 1}},
		{'L', {0, 0, -1}},
		{'T', {1, 0, 0}},
		{'D', {0, 2, 0}},
		{'W', {0, 1, 0}},
	};


	// The game itself
	Game* game = nullptr;

	// Game info
	int& topWithCell = game->topRowWithBlock;
	vector<int>& blocksPerRow = game->blocksPerRow;
	vector<vector<int>>& board = game->gameBoard;

	// Saved stack of moves
	stack<char> moves;


	// Learned weights
	vector<double> weights = vector<double>(AI_FEATURE_COUNT, 1);


	/*** DROP PATH FINDERS ***/
	/* 
	* REQUIRES: game->currPiece is in its starting position;
	*			game->heldPiece exists;
	*			the current contents of this.moves don't matter
	* MODIFIES: moves
	* EFFECTS:  Finds the series of moves the AI must output to drop
	*			the next piece in the way it believes is best, and
	*			puts that series of moves in the moves stack
	* CALLS:    findBestDrop
	*/
	void getMovePath();

	/*
	* REQUIRES: game->currPiece is in its starting position;
	* MODIFIES: pathOfDrop
	* EFFECTS:  Finds the series of moves the AI must output to drop
	*			game->currPiece in the way it believes is best, records
	*			that series in pathOfDrop, and 
	*			returns the cost of that drop
	* CALLS:    evaluatePosition, Piece.newPiece, 
	*			Game.moveAllowed, backtrackPath,
	*			Game.placePiece, Game.clearRows,
	*			Game.getRowsToClear, fillRows, removePiece
	* NOTES:	Also modifies game, topWithCell, blocksPerRow, board,
	*			but  changes should be reversed/undone;
	*			the code is buggy if not
	*/
	double findBestDrop(stack<char>& pathOfDrop);

	/*
	* REQUIRES: bfsMap is a map correctly computed by findBestDrop();
	*			end is a coordinate that has been reached on bfsMap;
	*			startHeight is the highest position reached on bfsMap
	* MODIFIES: path
	* EFFECTS:	Finds the series of moves needed to get from the 
	*			start position in the bfsMap to the given end 
	*			coordinate, and stores it in path
	*/
	void backtrackPath(vector<vector<vector<char>>>& bfsMap,
		BfsCoords& end, int startHeight,
		stack<char>& path);


	/*** AI EVALUATOR ***/
	/*
	* REQUIRES: The board state is valid; rowsCleared is the 
	*			number of rows cleared by the last dropped piece
	* MODIFIES: None
	* EFFECTS:  Uses learned parameters to determine the cost of the
	*			current position, which is returned
	*			(A lower cost means a better percieved position)
	*/
	double evaluatePosition(int rowsCleared);


	/*** BOARD MANIPULATION HELPERS ***/
	/*
	* REQUIRES:	rowsToFill's ints are in the range [0, BOARD_HEIGHT);
	*			the board has room to fill these rows
	* MODIFIES:	game (board, blocksPerRow, topWithCell)
	* EFFECTS:	Inserts full rows at the indices specified, pushing
	*			other rows upwards accordingly
	*/
	void fillRows(vector<int>& rowsToFill);

	/*
	* REQUIRES: The board cells at game->currPiece's coords are filled
	* MODIFIES: game (board, topWithCell)
	* EFFECTS:	Sets game->currPiece's coordinates on the board to 0
	*			(Empties the board where currPiece is)
	*/
	void removePiece();

public:
	/*** CONSTRUCTOR ***/
	AI(Game* gameIn) : game(gameIn) {}


	/*** OUTPUT ***/
	/*
	* REQUIRES: game is awaiting a move
	* MODIFIES: moves
	* EFFECTS:	Returns and pops the top of the move stack if it isn't empty.
	*			Finds the best sequence of next moves if it is empty.
	* CALLS:    getMovePath
	*/
	char makeMove();
};

#endif