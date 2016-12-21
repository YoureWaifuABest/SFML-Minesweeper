/*
 * Licensed under the MIT license.
 * Read LICENSE for more information.
 *
 * github: https://github.com/YoureWaifuABest/small-c-minesweeper
 */
#include <iostream>
#include <sodium.h>
#include <exception>
#include "main.h"

using std::cout; using std::endl;

int main(int argc, char *argv[])
{
	std::vector< std::vector<int> > grid;

	grid.resize(GRIDSIZE, std::vector<int>(GRIDSIZE, 0));

	/* 
	 * Initialize libsodium
	 * If libsodium did not correctly initialize,
	 * abort program.
	 */
	if (sodium_init() == -1) {
		std::cerr << "Failed to initialize libsodium!" << endl;
		throw std::exception();
	}

	render_grid_graphical(grid);

	return 0;
}
