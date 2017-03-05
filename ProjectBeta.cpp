// ProjectBeta.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <random>
#include <vector>
#include <assert.h>
#include<algorithm>

using namespace std;

#define SMALLRAND (double)rand()/RAND_MAX*0.1 //falls between 0 and 0.1
#define GREED_RAND (double)rand()/RAND_MAX // falls between 0 and 1
#define MOVE_RAND rand()/RAND_MAX*3 //  integer between 0 and 3 (four actions)

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
	rewardTable.at(goalState) = 100;
}

class agent {
public:
	int xpos; // bounded between vertical "bumpers"
	int ypos; // bounded between horizontal "bumpers"
	int state; // funxtion of x and y and xmax (from grid)
	int numMoves;
	double epsilon = 0.1; // greedy value
	double alpha = 0.1; // learning value
	double gamma = 0.9;

	vector <vector<double>> qTable;

	void init();
	void placeAgent(vector<int> x, vector<int> y, int xmax, int ymax);
	void initQ(vector<int> s);
	void moveAgent(grid g);
	void checkBumper(grid g, int x, int y);
	void updateQ(double action, vector<double> reward);

};

void agent::init() {
	// unrealistic positions for init function, created real ones in main program
	xpos = -1;
	ypos = -1;
	state = -1;
	numMoves = 0; 
}

void agent::placeAgent(vector<int> x, vector<int> y, int xmax, int ymax) {
	int userx;
	int usery;

	userx = rand() / RAND_MAX*xmax;
	usery = rand() / RAND_MAX*ymax;

	// if statements are redundant because the agent position is now randomized instead of being a user input
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

	state = xpos + ypos*(xmax + 1);
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

void agent::checkBumper(grid g, int x, int y)
{
	if (x < 0)
	{
		xpos = 0;
	}
	else if (x > g.xmax)
	{
		xpos = g.xmax;
	}
	else
	{
		xpos = x;
	}

	if (y < 0)
	{
		ypos = 0;
	}
	else if (y > g.ymax)
	{
		ypos = g.ymax;
	}
	else
	{
		ypos = g.ymax;
	}
}

void agent::moveAgent(grid g) 
{
	int n = GREED_RAND;
	//outside of loop because i need this sent as parameter to update function
	double m;

	if (n < epsilon)
	{
		//random move
		m = MOVE_RAND;
	}
	else
	{
		//greedy move
		m = *max_element(qTable[state].begin(), qTable[state].end());
	}

	if (m == 0)
	{
		//move left
		xpos--;
	}

	else if (m == 1)
	{
		//move up
		ypos++;
	}

	else if (m == 2)
	{
		//move right
		xpos++;

	}

	else if (m == 3)
	{
		//move down
		ypos--;
	}

	//check for bumpers
	checkBumper(g, xpos, ypos);

	state = xpos + ypos*(g.xmax + 1);

	//update the q table 
	updateQ(m,g.rewardTable);
	numMoves++;
}

void agent::updateQ(double action, vector<double> reward) 
{
	//this for previous state and action after said action is taken
	qTable[state - 1][action] = qTable[state-1][action] + alpha*(reward.at(state)+gamma**max_element(qTable[state].begin(),qTable[state].end()- qTable[state - 1][action]));
}

int main()
{

	system("pause");
    return 0;
}

