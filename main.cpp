#include <iostream>
#include <fstream>
#include <string>

#include "AI.h"
#include "AITrainer.h"
#include "Game.h"
#include "Utility.h"
using namespace std;


void aiModeSelector(Game& game);
void trainAISelector(Game& game, AI& ai);

int main() {
	Game game;
	
	// Main menu
	while (true) {
		cout << "Enter mode (type the number): \n"
			<< "   1) Play a normal game\n"
			<< "   2) AI mode\n"
			<< "   3) Exit\n"
			<< "$ ";
		string mode;
		getline(cin, mode);

		if (mode == "1") {
			game.run<AI>(false);
		}
		else if (mode == "2") {
			aiModeSelector(game);
		}
		else if (mode == "3") {
			cout << "Exited Program!\n";
			break;
		}
		else {
			cout << "Error on input mode. Please type a number 1-3.\n";
		}
	}


	return 0;

	/*
	// Getting AI Mode
	int isAIMode = 0;
	string AIMode;
	cout << "AI mode? (y/n): ";
	getline(cin, AIMode);
	if (AIMode == "y") {
		isAIMode = 1;
	}
	else if (AIMode == "n") {
		isAIMode = 0;
	}

	if (isAIMode == 0) {
		Game manualGame;
		while (!manualGame.getIsLost()) {
			manualGame.gameTick(0, 0, ' ');
		}
		return 0;
	}

	ofstream outf;
	outf.open("generations.txt");

	AI ai;
	char possMoves[POSSIBLE_MOVE_COUNT][MAX_MOVE_LENGTH] = {' '};
	ai.assignPossibleMoveCombos(possMoves);
	int possMovesLens[POSSIBLE_MOVE_COUNT] = {0};
	ai.assignPossibleMoveCombosLens(possMovesLens);
	// Runs 1000 generations
	for (int i = 0; i < 10001; i++) {
		cout << endl << "Generation " << i << endl;
		// For each individual in the generation
		for (int j = 0; j < GENERATION_POPULATION; j++) {
			//string junk;
			//getline(cin, junk);
			Game game;
			// Each individual will make 1000 moves
			double movesMadeCount = 0;
			for (int k = 0; k < 100000; k++) {
				//if (i != 0) {
					string junk;
					getline(cin, junk);
				//}
				// Only play if the game isn't lost
				if (!game.getIsLost()) {
					movesMadeCount++;
					int bestMoveCombo = ai.findBestMove(game, j);
					game.setMoveEnded(false);
					// make the move in the real game; bestMoveCombo is the index of 
					// the best list in possMoves
					for (int l = 0; l < possMovesLens[bestMoveCombo]; l++) {
						if (game.getMoveValid() && !game.getMoveEnded()) {
							game.gameTick(1, 1, possMoves[bestMoveCombo][l]);
						}
					}
					while (!game.getMoveEnded()) {
						game.gameTick(1, 1, ' ');
					}
				}
				else {
					break;
				}
				//if (i != 0) {
					game.printBoard();
				//}
			}
			cout << game.getUserScore() << endl;
			if (!game.getIsLost()) {
				cout << "Win!" << endl;
			}
			//game.printBoard();
			if (game.getUserScore() > 0) {
				if (!game.getIsLost()) {
					ai.setScoreVal(game.getUserScore(), j);
				}
				else if (game.getIsLost()) {
					ai.setScoreVal(game.getUserScore() / 2, j);
				}
			}
			else {
				ai.setScoreVal(movesMadeCount * 0.1, j);
			}
		}

		// Saving the generation
		if ((i < 100 && (i % 10) == 0) || ((i % 50) == 0)) {
			int genWinners[GENERATION_POPULATION] = { 0 };
			ai.findWinners(genWinners);
			ai.saveGen(outf, i, genWinners);
		}


		ai.createNewGen();
		ai.resetScores();
	}

	outf.close();
	return 0;
	*/
}

void aiModeSelector(Game &game) {
	AI gameAI(&game);

	// AI Menu
	while (true) {
		cout << "Enter AI mode (type the number): \n"
			<< "   1) Watch an AI play\n"
			<< "   2) Load AI State (generated by trainer)\n"
			<< "   3) Train AI\n"
			<< "   4) Exit\n"
			<< "$ ";
		string mode;
		getline(cin, mode);

		if (mode == "1") {
			game.run<AI>(true, &gameAI);
		}
		else if (mode == "2") {
			cout << "\nEnter file containing state (must be output from AI "
				<< "Trainer's 'Save Trainer State' option): \n"
				<< "$ ";
			string presetFName;
			getline(cin, presetFName);
			gameAI.loadTrainerState(presetFName);
		}
		else if (mode == "3") {
			trainAISelector(game, gameAI);
		}
		else if (mode == "4") {
			cout << "Exited AI Mode!\n";
			break;
		}
		else {
			cout << "Error on input mode. Please type a number 1-4.\n";
		}
	}
}

void trainAISelector(Game& game, AI& ai) {
	AITrainer trainer(&ai);

	// AI Trainer Menu
	while (true) {
		cout << "Enter AI Trainer mode (type the number): \n"
			<< "   1) Load Trainer State\n"
			<< "   2) Run Trainer\n"
			<< "   3) Save Trainer State\n"
			<< "   4) Exit\n"
			<< "$ ";
		string mode;
		getline(cin, mode);

		if (mode == "1") {
			cout << "\nEnter file containing state (must be output from AI "
				<< "Trainer's 'Save Trainer State' option): \n"
				<< "$ ";
			
			string stateFName;
			getline(cin, stateFName);
			trainer.loadState(stateFName);

			cout << "State Loaded!\n"
				<< "$ ";
		}
		else if (mode == "2") {
			int nEpochs = 0;
			cout << "Enter number of epochs to train: \n"
				<< "$ ";
			string input;
			getline(cin, input);

			// Handling invalid stoi inputs
			try {
				nEpochs = stoi(input);
				if (nEpochs < 0) {
					throw (1);
				}
			}
			catch (...) {
				cout << "Invalid input. Please input a positive 32-bit integer.\n";
				continue;
			}


			bool doOutput;
			while (true) {
				cout << "Output summary of each epoch? [Y/n]\n"
					<< "$ ";
				string input;
				getline(cin, input);

				if (input == "Y") {
					doOutput = true;
					break;
				}
				else if (input == "n") {
					doOutput = false;
					break;
				}
				else {
					cout << "Invalid input, type 'Y' or 'n'\n";
				}
			}

			string logFName = "";
			if (doOutput) {
				cout << "Enter output filename: \n"
					<< "$ ";
				getline(cin, logFName);
			}

			trainer.runAITrainer(nEpochs, doOutput, logFName);
		}
		else if (mode == "3") {
			cout << "\nEnter state output filename: \n"
				<< "$ ";

			string stateFName;
			getline(cin, stateFName);
			trainer.saveState(stateFName);

			cout << "State Saved!\n"
				<< "$ ";
		}
		else if (mode == "4") {
			cout << "Exited AI Trainer Mode!\n";
			break;
		}
		else {
			cout << "Error on input mode. Please type a number 1-4.\n";
		}
	}

}

