// ProjectBeta.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <random>
#include <vector>
#include <assert.h>

using namespace std;

#define SMALLRAND (double)rand()/RAND_MAX*0.1 //falls between 0 and 0.1

class grid {
public:
	int xmax;
	int ymax;
	int goalPosX;
	int goalPosY;
	vector<int> xcoord;
	vector<int> ycoord;
	vector<int> states;
	vector<double> rewardTable;

	void init();
	void coordToStates();
	void createRewardTable();
};

void grid::init() {
//start by setting values that don't make sense. they will be specified as true values later
	xmax = -1;
	ymax = -1;
	goalPosX = -1;
	goalPosY = -1;
}

void grid::coordToStates() {
	//convert coordinates from two vectors into one state number
	// go through every row
	for (int n = 0; n < ymax; n++)
	{
		// go through every column
		for (int m = 0; m < xmax; m++)
		{
			// create state values
			states.push_back(n + m*(xmax + 1));
		}
	}
}

void grid::createRewardTable() {
	//go through entire state vector (whole grid)
	for (int n = 0; n < size(states); n++)
	{
		// reward for every state is -1 
		rewardTable.push_back(-1);
	}

	// find the state that holds the goal
	int goalState = goalPosX + goalPosY*(xmax + 1);

	// reward at goal is 100
	rewardTable[goalState] = 100;
}

class agent {
public:
	int xpos; // bounded between vertical "bumpers"
	int ypos; // bounded between horizontal "bumpers"
	vector <vector<double>> qTable;

	void init();
	void placeAgent(vector<int> x, vector<int> y);
	void initQ(vector<int> s);

};

void agent::init() {
	// unrealistic positions for init function, created real ones in main program
	xpos = -1;
	ypos = -1;
}

void agent::placeAgent(vector<int> x, vector<int> y) {
	int userx;
	int usery;

	//User chooses where to place agent, might randomize this instead
	cout << "Place agent's x position (positive integer): ";
	cin >> userx;
	cout << endl << "Place agent's y position (positive integer): ";
	cin >> usery;

	if (userx < 0)
		xpos = 0;
	else if (userx > size(x) - 1)
		xpos = size(x) - 1;
	else
		xpos = userx;

	if (usery < 0)
		ypos = 0;
	else if (usery > size(y) - 1)
		ypos = size(y) - 1;
	else
		ypos = usery;
}

void agent::initQ(vector<int> s) {
	// set initial q values for every state at about 0 with minor differences to fix issue of equal values
	vector<double> actions;

	for (int n = 0; n < size(s); n++)
	{
		for (int m = 0; m < 4; m++)
		{
			actions.push_back(SMALLRAND);
		}
		qTable.push_back(actions);
		// Clear actions before resetting them
		actions.clear();
	}
}

int main()
{
    return 0;
}

