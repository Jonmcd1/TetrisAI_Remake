#ifndef _AI_H_
#define _AI_H_

#include "Utility.h"
#include "Game.h"
#include <queue>
#include <stack>
using namespace std;

class AI {
private:
	// The game itself
	Game* game = nullptr;

	// Game info
	int& topWithCell = game->topRowWithBlock;
	vector<int>& blocksPerRow = game->blocksPerRow;
	vector<vector<int>>& board = game->gameBoard;

	// Saved stack of moves
	stack<char> moves;


	int gensDone = 0;

public:
	AI(Game* gameIn) : game(gameIn) {}

	void findDrops();

	// Assumes the piece is in position to be placed on board
	double evaluatePosition();

	// Fills the rows at the specified indices with tiles
	void fillRows(vector<int>& rowsToFill);

	char makeMove();
};

#endif