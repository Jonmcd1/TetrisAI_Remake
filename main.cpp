#include <iostream>
#include <fstream>
#include <string>
// #include <chrono>
// #include <thread>

#include "AI.h"
#include "AITrainer.h"
#include "Game.h"
#include "Utility.h"
using namespace std;
// using namespace std::this_thread; // sleep_for, sleep_until
// using namespace std::chrono; // nanoseconds, system_clock, seconds


int main() {
	Game game;
	
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
			while (true) {
				AI gameAI(&game);
				
				cout << "Enter AI mode (type the number): \n"
					<< "   1) Watch an AI play\n"
					<< "   2) Train AI\n"
					<< "   3) Exit\n"
					<< "$ ";
				string aiMode;
				getline(cin, aiMode);

				if (aiMode == "1") {
					gameAI.weights = { 0.132189, 4.77676, 27.2359, 9.85004, 30.868, 1.97888, 3.59599, 2.42627 };
					// gameAI.weights = {0, 6.87562, 0, 0, 0, 6.84836, 0, 0 };
					game.run<AI>(true, &gameAI);
				}
				else if (aiMode == "2") {
					AITrainer trainer(&gameAI);
					trainer.runAITrainer(5, true, "Output_Logs/worstPoss.txt");
				}
				else if (aiMode == "3") {
					cout << "Exited AI Mode!\n";
					break;
				}
				else {
					cout << "Error on input mode. Please type '1', '2', or '3'\n";
				}
			}
		}
		else if (mode == "3") {
			cout << "Exited Program!\n";
			break;
		}
		else {
			cout << "Error on input mode. Please type '1', '2', or '3'\n";
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









/*
void aiSelected(Game& game) {
	do {
		// Get savestate mode
		cout << "Enter option (type the number): \n"
			<< "   1) Run AI from scratch\n"
			<< "   2) Run AI with prior progress\n"
			<< "   3) Exit\n"
			<< "$ ";
		string modeLoadAI;
		cin >> modeLoadAI;
		
		if (modeLoadAI == "1" || modeLoadAI == "2") {
			game.reset();
			AI gameAI(game);

			// Use saved progress
			if (modeLoadAI == "2") {
				ifstream inF("generations.txt");
				gameAI.loadSave(inF);
				inF.close();
			}

			// Get running mode
			cout << "Run training mode or testing mode? (type the number)\n"
				<< "   1) Training: Will run generations of the AI to "
				<< "train it; not printed. (Will overwrite progress file)\n"
				<< "   2) Testing: Will have the AI play one game; printed.\n"
				<< "$ ";
			string modeRunAI;
			cin >> modeRunAI;

			// Training
			if (modeRunAI == "1") {
				// Get generation count
				cout << "Number of generations (enter a whole number): \n"
					<< "$ ";
				// TODO: Error checking
				int genCount;
				cin >> genCount;

				// Run Training
				gameAI.runTraining(genCount);

				// Save this progress
				ofstream outF("generations.txt");
				gameAI.logSave(outF);
				outF.close();
			}
			// Testing
			else if (modeRunAI == "2") {
				// Get print mode
				cout << "Run compact or normal print mode? (type the number)\n"
					<< "   1) Compact only prints each piece's final position.\n"
					<< "   2) Normal prints each move entry.\n"
					<< "$ ";
				string modePrintAI;
				cin >> modePrintAI;

				// Valid mode
				if (modePrintAI == "1" || modePrintAI == "2") {
					// NOTE: Takes a bool, where the bool means COMPACT
					gameAI.runTest(modePrintAI == "2");
				}
				// Error
				else {
					cout << "Unrecognized mode. Exiting...\n";
				}
			}
			// Error
			else {
				cout << "Unrecognized mode. Exiting...\n";
			}

		}
		else if (modeLoadAI == "3") {
			cout << "Exited AI Mode!";
			break;
		}
		else {
			cout << "Error on input option. Please type '1', '2', or '3'\n";
		}
	} while (true);

	return;
}
*/