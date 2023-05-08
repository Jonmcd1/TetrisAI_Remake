#ifndef _AI_H_
#define _AI_H_

#include "Utility.h"
#include "Game.h"
#include <queue>
using namespace std;

class AI {
private:
	Game* game = nullptr;

	int gensDone = 0;

	int topWithCell = 23;

public:
	AI(Game* gameIn) : game(gameIn) {}

	vector<char> findDrops();
};

/*

class AI {
private:
	char possibleMoveCombos[POSSIBLE_MOVE_COUNT][MAX_MOVE_LENGTH] = {' '};
	int possibleMoveCombosLens[POSSIBLE_MOVE_COUNT] = {0};
	
	int gensCompleted = 0;

	double genVals[GENERATION_POPULATION][COST_FACTOR_COUNT] = {0};
	double genScores[GENERATION_POPULATION] = {0};

public:
	/**
	 * Requires: Nothing
	 * Modifies: Nothing
	 * Effects:  Constructor for AI
	 *\/
	AI();


	/**
	 * Requires: Nothing
	 * Modifies: Nothing
	 * Effects:  Creates a random value in the range [-1, 1], with a strong weight towards 0
	 *\/
	double randomGen();


	/**
	 * Requires: Nothing
	 * Modifies: genVals, genScores, outputFile
	 * Effects:  creates a new generation from the 6 winners of the old one,
	 *			 and replaces genVals with the new generation; also sets genScores to 0
	 *\/
	void createNewGen();


	/**
	 * Requires: Nothing
	 * Modifies: winnerList
	 * Effects:  Of the 45 individuals in genVals, 
	 *			 finds and adds the 6 highest scorers to winnerList
	 *\/
	void findWinners(int winnerList[GENERATION_POPULATION]);


	/**
	 * Requires: Nothing
	 * Modifies: output file
	 * Effects:  Writes the current values of genVals to a file to save the gen
	 *\/
	void saveGen(ofstream& outs, int genNum, int genWinners[6]);


	/**
	 * Requires: Nothing
	 * Modifies: genValsCopy
	 * Effects:  copies genVals to genValsCopy
	 *\/
	void copyGenVals(double genValsCopy[GENERATION_POPULATION][COST_FACTOR_COUNT]);




	/**
	 * Requires: Nothing
	 * Modifies: Nothing
	 * Effects:  Finds the cost of a position, given cost weights
	 *\/
	double evaluatePosition(Game game, double bumpCostW, double holeCostW, 
							double nonTetW, double tetW, double blocksOverHoleCostW,
							double blocksInRightLaneW, double topHeightW,
							double oneWideCostW, double holdIW,
							double wasteIW, char pieceUsed, int tempTest);


	/**
	 * Requires: Nothing
	 * Modifies: Nothing
	 * Effects:  Finds the bumpCost of a position
	 *\/
	double findBumpCost(Game game);


	/**
	 * Requires: Nothing
	 * Modifies: Nothing
	 * Effects:  Finds the holeCost of a position
	 *\/
	double findHoleCost(Game game);


	/**
	 * Requires: Nothing
	 * Modifies: Nothing
	 * Effects:  Finds the nonTet of a position
	 *\/
	double findNonTet(Game game);


	/**
	 * Requires: Nothing
	 * Modifies: Nothing
	 * Effects:  Finds the blocksOverHoleCost of a position
	 *\/
	double findBlocksOverHoleCost(Game game);


	/**
	 * Requires: Nothing
	 * Modifies: Nothing
	 * Effects:  Finds the blocksInRightLane of a position
	 *\/
	double findBlocksInRightLaneCost(Game game);


	/**
	 * Requires: Nothing
	 * Modifies: Nothing
	 * Effects:  Finds the tet of a position
	 *\/
	double findTet(Game game);

	/**
	 * Requires: Nothing
	 * Modifies: Nothing
	 * Effects:  Finds the topHeight of a position
	 *\/
	double findTopHeightCost(Game game);

	// FIXME: Add a new parameter for preventing deep, 1-wide gaps from forming (apart from right lane)
	/**
	 * Requires: Nothing
	 * Modifies: Nothing
	 * Effects:  Finds the oneWideCost of a position
	 *\/
	double findOneWideCost(Game game);

	// FIXME: Add a BENEFICIAL parameter for having an I block held
	/**
	 * Requires: Nothing
	 * Modifies: Nothing
	 * Effects:  Finds if an I block is held
	 *\/
	double findHoldI(Game game);
	
	// FIXME: Add a new parameter for preventing using an I block if it doesn't score a tetris
	//		  This will be useful for the new game mechanic
	/**
	 * Requires: Nothing
	 * Modifies: Nothing
	 * Effects:  Finds if an I block was used and a tetris wasn't scored
	 *\/
	double findWasteI(Game game, char pieceUsed);


	/**
	 * Requires: Nothing
	 * Modifies: Nothing
	 * Effects:  Finds the best move in a position and returns the index of the
	 *			 best move in possibleMoveCombos
	 *\/
	int findBestMove(Game game, int currIndivIndex);


	/**
	 * Requires: Nothing
	 * Modifies: topHeights
	 * Effects:  Sets the values in topHeights to the heights of the highest occupied
	 *			 tiles per column
	 *\/
	void assignTopHeights(int topHeights[BOARD_WIDTH], Game game);

	/**
	 * Requires: Nothing
	 * Modifies: Nothing
	 * Effects:  sets possMoves to possibleMoveCombos
	 *\/
	void assignPossibleMoveCombos(char possMoves[POSSIBLE_MOVE_COUNT][MAX_MOVE_LENGTH]);

	/**
	 * Requires: Nothing
	 * Modifies: Nothing
	 * Effects:  sets possMovesLens to possibleMoveCombosLens
	 *\/
	void assignPossibleMoveCombosLens(int possMovesLens[POSSIBLE_MOVE_COUNT]);

	/**
	 * Requires: Nothing
	 * Modifies: Nothing
	 * Effects:  ...
	 *\/
	double getAGenVal(int i, int j);

	/**
	 * Requires: Nothing
	 * Modifies: Nothing
	 * Effects:  sets the corresponding genScores val to an individual's score
	 *\/
	void setScoreVal(int score, int currIndivIndex);

	/**
	 * Requires: Nothing
	 * Modifies: Nothing
	 * Effects:  Sets scores to 0
	 *\/
	void resetScores();
};

*/


#endif