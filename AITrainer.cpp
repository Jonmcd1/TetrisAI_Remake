#include "AITrainer.h"
using namespace std;

/*** EVOLUTION FUNCTIONS ***/
vector<IndividualAIVals> AITrainer::findWinners() {
	// Tracks the top scorers from this generation
	vector<IndividualAIVals> winners(WINNER_COUNT, IndividualAIVals());

	// This just exists to avoid needing to copy a ton of lists of weights
	vector<winnerTracker> topScores(WINNER_COUNT, { 0,0 });


	// Initialize the winners; index 0 has the lowest score, 
	// index WINNER_COUNT - 1 has the highest
	for (int i = 0; i < WINNER_COUNT; i++) {
		topScores[i] = { i, generation[i].score };
	}
	sort(topScores.begin(), topScores.end());

	// Find the top WINNER_COUNT scoring individuals
	for (int i = WINNER_COUNT; i < GENERATION_SIZE; i++) {
		// Skip individual if it can't possibly be a winner
		if (generation[i].score <= topScores[0].score) {
			continue;
		}

		// If this individual is a potential winner
		topScores[0] = { i, generation[i].score };
		// Sort this potential winner into place in the winners list
		for (int j = 0; j < WINNER_COUNT - 1; j++) {
			if (topScores[j] > topScores[j + 1]) {
				swap(topScores[j], topScores[j + 1]);
			}
		}
	}

	// Now the WINNER_COUNT winners have been found
	// Copy them into the winners vector
	for (int i = 0; i < WINNER_COUNT; i++) {
		winners[i] = generation[topScores[i].idx];
	}

	return winners;
}

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
	
	// Next 15: crosses between each winner
	// For each winner
	for (int i = 0; i < WINNER_COUNT; i++) {
		// For each winner the one chosen above hasn't been crossed with yet
		for (int j = i+1; j < WINNER_COUNT; j++, genIdx++) {
			// Choose how to cross them (avg vs splitPick)
			vectorMixer* mixer = mixers[rand() % MIXER_COUNT];
			// Cross them
			(*mixer)(winners[i].weights, winners[j].weights, 
				generation[genIdx].weights);

			// Choose how much to randomize the new individual's weights
			double strength = strengths[rand() % STRENGTH_COUNT];
			// Randomize them
			vecRandomizer(generation[genIdx].weights, strength);
		}
	}
	
	// Next 18: the winners with some randomization
	for (int i = 0; i < WINNER_COUNT; i++) {
		// Repeate each winner 3 times
		for (int j = 0; j < 3; j++, genIdx++) {
			generation[genIdx].weights = winners[i].weights;
			
			// Randomize the new individual's weights like before
			double strength = strengths[rand() % STRENGTH_COUNT];
			vecRandomizer(generation[genIdx].weights, strength);
		}
	}
	
	// Last 6: totally random weights
	for (int i = 0; i < 6; i++, genIdx++) {
		for (int wIdx = 0; wIdx < AI_FEATURE_COUNT; wIdx++) {
			generation[genIdx].weights[wIdx] = rand() % 100;
		}
	}
}


/*** DATA OUTPUT FUNCTIONS ***/
void AITrainer::outputThisGen(ofstream& out, vector<IndividualAIVals>& winners) {
	// Print generation #
	out << "GENERATION " << currGeneration << "\n";

	// Print generation scores
	out << "All individual scores, in order:\n";
	for (int i = 0; i < GENERATION_SIZE; i++) {
		out << generation[i].score << " ";
	}
	out << "\n\n";

	// Print winner scores and weights
	out << "Winners scores | their weights (best on bottom):\n";
	for (int i = 0; i < WINNER_COUNT; i++) {
		out << winners[i].score << " | ";

		for (int j = 0; j < AI_FEATURE_COUNT; j++) {
			out << winners[i].weights[j] << " ";
		}
		out << "\n";
	}
	out << "\n\n\n";
}

void AITrainer::loadState(string inF) {
	ifstream in(inF);

	if (!in.is_open()) {
		cout << "Error opening file. Quitting...\n";
		return;
	}

	string junk;

	// Get the generation #
	in >> junk >> currGeneration;

	// Don't mind the getline and >> mixing
	for (int i = 0; i < 5; i++) {
		getline(in, junk);
	}
	
	// Load the weights
	vector<IndividualAIVals> winners(WINNER_COUNT, IndividualAIVals());
	for (int i = 0; i < WINNER_COUNT; i++) {
		in >> junk >> junk;

		for (int j = 0; j < AI_FEATURE_COUNT; j++) {
			in >> winners[i].weights[j];
		}
	}

	// Load the generation
	makeNextGen(winners);

	in.close();
}

void AITrainer::saveState(string outF) {
	vector<IndividualAIVals> winners = findWinners();

	ofstream finalOut = ofstream(outF);

	if (!finalOut.is_open()) {
		cout << "Error opening file. Quitting...\n";
		return;
	}

	outputThisGen(finalOut, winners);
	finalOut.close();
}


/*** DRIVER ***/
void AITrainer::runAITrainer(int epochs, bool doOutF, string outF) {
	// Initialize outstream
	ofstream out = ofstream();
	if (doOutF) { 
		out = ofstream(outF);
	}
	if (doOutF && !out.is_open()) {
		cout << "Error opening file. Quitting...\n";
		return;
	}

	// Run
	vector<IndividualAIVals> winners;
	for (int i = 0; i < epochs; i++) {
		currGeneration++;

		// For each individual
		for (int j = 0; j < GENERATION_SIZE; j++) {
			// Load weights and run
			ai->weights = generation[j].weights;
			ai->game->run<AI>(true, ai, false);

			// Record score
			generation[j].score = ai->game->userScore;
		}

		// Find and output winners, if necessary
		winners = findWinners();
		if (doOutF) {
			outputThisGen(out, winners);
		}
		
		// Tell the user how much progress has been made
		cout << "Generation " << currGeneration << " done\n";

		if (i < epochs - 1) {
			// Evolve the next generation
			makeNextGen(winners);
		}
	}
	out.close();
}
