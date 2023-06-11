#ifndef _AITRAINER_H_
#define _AITRAINER_H_

#include <random>
#include <fstream>
#include "AI.h"
#include "Game.h"
#include "Utility.h"
using namespace std;

struct IndividualAIVals {
	vector<double> weights = vector<double>(AI_FEATURE_COUNT, 0);

	int score = 0;

	void operator=(const IndividualAIVals& other) {
		weights = other.weights;
		score = other.score;
	}
};

// Functors
class vectorMixer {
	// Empty parent class
	// Allows a vector of the following functors to exist
public:
	virtual void operator() (vector<double>& v1, vector<double>& v2,
		vector<double>& out) = 0;
};

class avgMixer : public vectorMixer {
public:
	void operator() (vector<double>& v1, vector<double>& v2,
					vector<double>& out) {
		for (int i = 0; i < v1.size(); i++) {
			out[i] = (v1[i] + v2[i]) / 2.0;
		}
	}
};

class splitPickMixer : public vectorMixer {
public:
	void operator() (vector<double>& v1, vector<double>& v2,
					vector<double>& out) {
		for (int i = 0; i < v1.size(); i++) {
			int choice = rand() % 2;
			out[i] = (choice == 0) ? v1[i] : v2[i];
		}
	}
};

class vectorRandomizer {
private:
	// For random gen
	std::default_random_engine generator;
	std::normal_distribution<double> Normal_Dist =
		normal_distribution<double>(0.0, 1.0);

public:
	void operator() (vector<double>& v, double strength) {
		for (int i = 0; i < AI_FEATURE_COUNT; i++) {
			double randomFactor = Normal_Dist(generator);
			v[i] += strength * randomFactor;

			// Bounds the weights on range [0, 100]
			v[i] = max(v[i], 0.0);
			v[i] = min(v[i], 100.0);
		}
	}
};




class AITrainer {
private:
	int currGeneration = 0;

	AI* ai = nullptr;

	vector<IndividualAIVals> generation =
		vector<IndividualAIVals>(GENERATION_SIZE, IndividualAIVals());

	// For random gen
	vectorRandomizer vecRandomizer = vectorRandomizer();

	vector<double> strengths = { 0, 0.1, 0.2, 0.5, 1, 2, 5 };
	int STRENGTH_COUNT = 7;

	// Annoying
	avgMixer mixer1 = avgMixer();
	splitPickMixer mixer2 = splitPickMixer();
	vector<vectorMixer*> mixers = { &mixer1, &mixer2 };
	int MIXER_COUNT = 2;

	void makeNextGen(vector<IndividualAIVals>& winners);

	// Find the top X individuals by score
	vector<IndividualAIVals> findWinners();

	void outputThisGen(ofstream& out, vector<IndividualAIVals>& winners);

public:
	AITrainer(AI* aiIn) {
		ai = aiIn;
	}

	void runAITrainer(int epochs, bool doOutF, string outF);


};

#endif