#include "AITrainer.h"
using namespace std;


struct winnerTracker {
	int idx = 0;
	int score = 0;

	bool operator<(const winnerTracker& other) {
		return score < other.score;
	}

	bool operator>(const winnerTracker& other) {
		return score > other.score;
	}
};

void AITrainer::makeNextGen(vector<IndividualAIVals>& winners) {
	// Reset all scores
	for (auto& indiv : generation) {
		indiv.score = 0;
	}


	/*** Creating the next generation of 45 AIs ***/
	// First 6: the winners, just directly copied
	
	int genIdx = 0;
	for (int i = 0; i < WINNER_COUNT; i++, genIdx++) {
		generation[genIdx].weights = winners[i].weights;
	}
	
	// Next 15: crosses between each winner, randomly chosen between avg and splitPick
	for (int i = 0; i < WINNER_COUNT; i++) {
		for (int j = i+1; j < WINNER_COUNT; j++, genIdx++) {
			vectorMixer* mixer = mixers[rand() % MIXER_COUNT];
			(*mixer)(winners[i].weights, winners[j].weights, 
				generation[genIdx].weights);

			double strength = strengths[rand() % STRENGTH_COUNT];
			vecRandomizer(generation[genIdx].weights, strength);
		}
	}
	
	// Next 18: the winners with some random levels of strength
	for (int i = 0; i < WINNER_COUNT; i++) {
		for (int j = 0; j < 3; j++, genIdx++) {
			generation[genIdx].weights = winners[i].weights;
			
			double strength = strengths[rand() % STRENGTH_COUNT];
			vecRandomizer(generation[genIdx].weights, strength);
		}
	}
	
	// Next 6: totally random
	for (int i = 0; i < 6; i++, genIdx++) {
		for (int wIdx = 0; wIdx < AI_FEATURE_COUNT; wIdx++) {
			generation[genIdx].weights[wIdx] = rand() % 100;
		}
	}
}


vector<IndividualAIVals> AITrainer::findWinners() {
	vector<IndividualAIVals> winners(WINNER_COUNT, IndividualAIVals());
	
	// Pairs are {individual index, individual score}
	vector<winnerTracker> topScores(WINNER_COUNT, {0,0});

	// Initialize the vector; index 0 has the lowest score, 
	// index WINNER_COUNT - 1 has the highest
	for (int i = 0; i < WINNER_COUNT; i++) {
		topScores[i] = {i, generation[i].score};
	}
	sort(topScores.begin(), topScores.end());



	// Find the top WINNER_COUNT scoring indivs
	for (int i = WINNER_COUNT; i < GENERATION_SIZE; i++) {
		// Skip a non-potential winner
		if (generation[i].score <= topScores[0].score) {
			continue;
		}

		// This indiv is a potential winner
		topScores[0] = { i, generation[i].score };
		// Sort this potential winner into place
		for (int j = 0; j < WINNER_COUNT - 1; j++) {
			if (topScores[j] > topScores[j + 1]) {
				swap(topScores[j], topScores[j + 1]);
			}
		}
	}

	// Now the WINNER_COUNT winners have been found
	// Copy the winners into the winners vector
	for (int i = 0; i < WINNER_COUNT; i++) {
		winners[i] = generation[topScores[i].idx];
	}

	return winners;
}


void AITrainer::runAITrainer(int epochs, bool doOutF, string outF) {
	ofstream out = (doOutF) ? ofstream(outF) : ofstream();

	for (int i = 0; i < epochs; i++) {
		for (int j = 0; j < GENERATION_SIZE; j++) {
			// Load weights and run
			ai->weights = generation[j].weights;
			ai->game->run<AI>(true, ai);

			// Log score
			generation[j].score = ai->game->userScore;

			//cout << generation[j].score << endl;

			ai->game->gameCount++;
		}
		//cout << endl << endl;

		vector<IndividualAIVals> winners = findWinners();
		if (doOutF) {
			outputThisGen(out, winners);
		}
		cout << "Generation " << currGeneration << " done\n";


		makeNextGen(winners);
		currGeneration++;
	}

	out.close();
}

void AITrainer::outputThisGen(ofstream& out, vector<IndividualAIVals>& winners) {
	out << "GENERATION " << currGeneration << "\n";
	out << "All individual scores, in order:\n";
	for (int i = 0; i < GENERATION_SIZE; i++) {
		out << generation[i].score << " ";
	}
	out << "\n\n";

	out << "Winners scores | their weights (best on top):\n";
	for (int i = 0; i < WINNER_COUNT; i++) {
		out << winners[i].score << " | ";
		
		for (int j = 0; j < AI_FEATURE_COUNT; j++) {
			out << winners[i].weights[j] << " ";
		}
		out << "\n";
	}
	out << "\n\n\n";
}