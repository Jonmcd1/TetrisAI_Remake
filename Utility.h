#ifndef _UTILITY_H_
#define _UTILITY_H_

#include <iostream>
#include <string>
#include <vector>

const int BOARD_WIDTH = 12;
const int BOARD_HEIGHT = 24;
const int BOTTOM_ROW_HEIGHT = BOARD_HEIGHT - 1;
const int PIECE_SIZE = 4;

const int AI_FEATURE_COUNT = 8;

const int GENERATION_SIZE = 45;
const int WINNER_COUNT = 6;

// The most complex class in this program
struct Coord {
	int vert = -1;
	int horiz = -1;
};


/*
* REQUIRES: vec is defined, turns >= 0
* MODIFIES: vec
* EFFECTS:  Coord.vert += dv, Coord.horiz += dh, and the whole vector is
*			rotated 90 degree counterclockwise *turns* times, using the 
*			first value in vec as the pivot
*/
void moveCoords(std::vector<Coord>& vec, int dv, int dh, int turns);


/*
* REQUIRES: map is a defined 3D vector of a type with a defined outstream
* MODIFIES: None
* EFFECTS:  Prints the map to stdout by iterating through each element of the
*			outermost vector, then printing their contents in separate grids
*/
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