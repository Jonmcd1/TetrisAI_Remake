#ifndef _UTILITY_H_
#define _UTILITY_H_

#include <iostream>
#include <string>
#include <vector>
using namespace std;

const int BOARD_WIDTH = 12;
const int BOARD_HEIGHT = 24;
const int BOTTOM_ROW_HEIGHT = BOARD_HEIGHT - 1;
const int PIECE_SIZE = 4;

const int POSSIBLE_MOVE_COUNT = 96;
const int MAX_MOVE_LENGTH = 10;

const int GENERATION_POPULATION = 60;
const int COST_FACTOR_COUNT = 10;
const int MAX_COST_FACTOR = 200;

template <typename T>
void print3DMap(vector<vector<vector<T>>>& map) {
	for (auto layer : map) {
		for (auto row : layer) {
			for (auto cell : row) {
				cout << cell;
			}
			cout << "\n";
		}
		cout << "\n\n\n";
	}

	return;
}

#endif