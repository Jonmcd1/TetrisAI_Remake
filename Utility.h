#ifndef _UTILITY_H_
#define _UTILITY_H_

#include <iostream>
#include <string>
#include <vector>

const int BOARD_WIDTH = 12;
const int BOARD_HEIGHT = 24;
const int BOTTOM_ROW_HEIGHT = BOARD_HEIGHT - 1;
const int PIECE_SIZE = 4;

//const int POSSIBLE_MOVE_COUNT = 96;
//const int MAX_MOVE_LENGTH = 10;

//const int GENERATION_POPULATION = 60;
//const int COST_FACTOR_COUNT = 10;
//const int MAX_COST_FACTOR = 200;

const int AI_FEATURE_COUNT = 8;

const int GENERATION_SIZE = 45;
const int WINNER_COUNT = 6;


struct Coord {
	int vert = -1;
	int horiz = -1;
};


// Turns are counterclockwise
// First coord in vector is assumed to be the pivot
void moveCoords(std::vector<Coord>& vec, int dv, int dh, int turns);


template <typename T>
void print3DMap(std::vector<std::vector<std::vector<T>>>& map) {
	for (auto layer : map) {
		for (auto row : layer) {
			for (auto cell : row) {
				std::cout << cell;
			}
			std::cout << "\n";
		}
		std::cout << "\n\n\n";
	}

	return;
}

#endif