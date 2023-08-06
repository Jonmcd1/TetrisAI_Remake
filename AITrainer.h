#ifndef _AITRAINER_H_
#define _AITRAINER_H_

#include <random>
#include <fstream>
#include "AI.h"
#include "Game.h"
#include "Utility.h"

using namespace std;

/****** USEFUL STRUCTS ******/
// Useful for logging AI weights and data
struct IndividualAIVals {
	vector<double> weights = vector<double>(AI_FEATURE_COUNT, 0);

	int score = 0;

	void operator=(const IndividualAIVals& other) {
		weights = other.weights;
		score = other.score;
	}
};

// Useful for tracking data from AITrainer's generation member variable;
// sorts by score
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



/****** VECTOR MIXER FUNCTORS ******/
// Parent class
class vectorMixer {
	// Empty parent class
	// Allows a vector of the following functors to exist
public:
	virtual void operator() (vector<double>& v1, vector<double>& v2,
		vector<double>& out) = 0;
};

// Child class
class avgMixer : public vectorMixer {
public:
	/*
	* REQUIRES: v1, v2, and out are vaild vectors of the same size
	* MODIFIES: out
	* EFFECTS:	each element of out is set to the average of the corresponding
	*			elements in v1 and v2
	*/
	void operator() (vector<double>& v1, vector<double>& v2,
					vector<double>& out) {
		for (int i = 0; i < v1.size(); i++) {
			out[i] = (v1[i] + v2[i]) / 2.0;
		}
	}
};

// Child class
class splitPickMixer : public vectorMixer {
public:
	/*
	* REQUIRES: v1, v2, and out are vaild vectors of the same size
	* MODIFIES: out
	* EFFECTS:	each element of out is randomly set to the one of the two 
	*			corresponding elements in v1 and v2
	*/
	void operator() (vector<double>& v1, vector<double>& v2,
					vector<double>& out) {
		for (int i = 0; i < v1.size(); i++) {
			int choice = rand() % 2;
			out[i] = (choice == 0) ? v1[i] : v2[i];
		}
	}
};


/****** VECTOR RNG FUNCTOR ******/
class vectorRandomizer {
private:
	// For standard normal distribution random number generation
	std::default_random_engine generator;
	std::normal_distribution<double> Normal_Dist =
		normal_distribution<double>(0.0, 1.0);

public:
	/*
	* REQUIRES: v is a valid vector
	* MODIFIES: v
	* EFFECTS:	Adds a normally distributed random variation to each
	*			element in v, the magnitude of which is controlled by
	*			strength; the result is bounded by [0, 100]
	*/
	void operator() (vector<double>& v, double strength) {
		for (int i = 0; i < v.size(); i++) {
			double randomFactor = Normal_Dist(generator);
			v[i] += strength * randomFactor;

			// Bounds the weights on range [0, 100]
			v[i] = max(v[i], 0.0);
			v[i] = min(v[i], 100.0);
		}
	}
};



/****** AI TRAINER ******/
class AITrainer {
private:
	/*** MEMBER VARIABLES ***/
	// Metadata
	int currGeneration = 0;

	// AI to be used for training
	AI* ai = nullptr;

	// Stores a list of AI weight sets, representing a generation of AIs
	// Elements of this list are "individuals"
	vector<IndividualAIVals> generation =
		vector<IndividualAIVals>(GENERATION_SIZE, IndividualAIVals());

	// Randomizes vectors' elements
	vectorRandomizer vecRandomizer = vectorRandomizer();

	// Controls the magnitude of random variation
	vector<double> strengths = { 0, 0.1, 0.2, 0.5, 1, 2, 5 };
	int STRENGTH_COUNT = 7;

	// Mixes two vectors' elements in different ways
	avgMixer mixer1 = avgMixer();
	splitPickMixer mixer2 = splitPickMixer();
	vector<vectorMixer*> mixers = { &mixer1, &mixer2 };
	int MIXER_COUNT = 2;


	/*** EVOLUTION FUNCTIONS ***/
	/*
	* REQUIRES: generation vector has correct scores and weights
	* MODIFIES: None
	* EFFECTS:  Returns a list containing copies of the WINNER_COUNT 
	*			highest scoring individuals in the current generation
	*/
	vector<IndividualAIVals> findWinners();

	/*
	* REQUIRES: Each individual has a correct score and defined weights
	* MODIFIES: generation
	* EFFECTS:  Produces the next generation using the provided winners' weights.
	*			Copies the winners and various random variations of them into the
	*			generation vector, as well as some completely random individuals
	*/
	void makeNextGen(vector<IndividualAIVals>& winners);


	/*** DATA OUTPUT FUNCTION ***/
	/*
	* REQUIRES: generation and winners have correct scores and weights;
	*			currGeneration is accurate
	* MODIFIES: out
	* EFFECTS:  Outputs gen # and all scores from this generation, plus the 
	*			scores and weights of the winners (in increasing score order)
	*/
	void outputThisGen(ofstream& out, vector<IndividualAIVals>& winners);


public:
	/*** CONSTRUCTOR ***/
	/*
	* REQUIRES: aiIn is a valid AI object
	* MODIFIES: this
	* EFFECTS:  Constructs an AITrainer object
	*/
	AITrainer(AI* aiIn) : ai(aiIn) {}


	/*** DATA OUTPUT FUNCTIONS ***/
	/*
	* REQUIRES: inF is the path to a file output by AITrainer.saveState()
	* MODIFIES: winners[i].weights
	* EFFECTS:	Loads all winner weights from the savestate and uses them to
	*			produce a new generation.
	* CALLS:    makeNextGen
	*/
	void loadState(string inF);

	/*
	* REQUIRES:	outF is a valid file path
	* MODIFIES:	outF's file
	* EFFECTS:	See AITrainer.outputThisGen
	* CALLS:	outputThisGen
	*/
	void saveState(string outF);


	/*** DRIVER ***/
	/*
	* REQUIRES: this.ai is a valid AI pointer,
	*			epochs > 0, outF is a valid text file name IF doOutF == true
	* MODIFIES: this, file specified by outF
	* EFFECTS:	Runs <epochs> generations of the AI, and outputs the data from
	*			each generation if doOutF is true
	* CALLS:	findWinners, makeNextGen, outputThisGen
	*/
	void runAITrainer(int epochs, bool doOutF, string outF);
};

#endif