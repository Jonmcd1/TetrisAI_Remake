#include "Utility.h"
using namespace std;


void moveCoords(vector<Coord>& vec, int dv, int dh, int turns) {
	// Changes vert
	if (dv) {
		for (auto& coord : vec) {
			coord.vert += dv;
		}
	}

	// Changes horiz
	if (dh) {
		for (auto& coord : vec) {
			coord.horiz += dh;
		}
	}

	// Changes rotation
	if (turns) {
		Coord& pivot = vec[0];
		for (int i = 0; i < turns % 4; i++) {
			// 1st value is pivot, so it gets skipped
			for (int j = 1; j < vec.size(); j++) {
				Coord& cell = vec[j];

				// Runs a 90 degree counterclockwise rotation
				int tempV = cell.vert;
				cell.vert = pivot.vert - (cell.horiz - pivot.horiz);
				cell.horiz = pivot.horiz + (tempV - pivot.vert);
			}
		}
	}
}

