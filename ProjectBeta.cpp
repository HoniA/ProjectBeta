// ProjectBeta.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <random>
#include <vector>
#include <assert.h>
#include<algorithm>
#include <time.h>

using namespace std;

#define SMALLRAND (double)rand()/RAND_MAX*0.1 //falls between 0 and 0.1
#define GREED_RAND (double)rand()/RAND_MAX // falls between 0 and 1
#define MOVE_RAND (double)rand()/RAND_MAX*3 //  integer between 0 and 3 (four actions)

class grid {
public:
	int xmax;
	int ymax;
	int goalPosX;
	int goalPosY;
	int goalState;
	vector<int> xcoord;
	vector<int> ycoord;
	vector<int> states;
	vector<double> rewardTable;

	void init(int x, int y, int goalx, int goaly);
	void coordToStates();
	void createRewardTable();
};

void grid::init(int x, int y, int goalx, int goaly) {
	//doing a different type of init function than previous assignments for more readable and efficient code
	//max x and y coordinates and position of goal come in as parameters
	xmax = x;
	ymax = y;
	goalPosX = goalx;
	goalPosY = goaly;
	goalState = goalx + goaly*(xmax + 1);

	for (int n = 0; n < xmax; n++)
	{
		xcoord.push_back(n);
	}

	for (int m = 0; m < ymax; m++)
	{
		ycoord.push_back(m);
	}
}

void grid::coordToStates() {
	//convert coordinates from two vectors into one state number
	// go through every row
	for (int n = 0; n < ymax+1; n++)
	{
		// go through every column
		for (int m = 0; m < xmax+1; m++)
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
	void updateQ(double action, vector<double> reward, double currentState);

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

	userx = (double)rand() / RAND_MAX*(xmax);
	usery = (double)rand() / RAND_MAX*(ymax);

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
		ypos = y;
	}
}

void agent::moveAgent(grid g) 
{
	int n = GREED_RAND;
	//outside of loop because i need this sent as parameter to update function
	int m=0; // default
	
	if (n < epsilon)
	{
		//random move
		m = MOVE_RAND+0.5;
	}
	else
	{
		//greedy move
		int h = *max_element(qTable[state].begin(), qTable[state].end());
		for (int j = 0; j < 4; j++)
		{
			if (qTable[state][j] = h)
			{
				m = j;
			}
		}
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

	else
	{
		//move down
		ypos--;
	} 
	
	//cout << m;
	//check for bumpers
	checkBumper(g, xpos, ypos);

	double newState = xpos + ypos*(g.xmax + 1);

	//update the q table 
	updateQ(m,g.rewardTable, newState);
	numMoves++;
}

void agent::updateQ(double action, vector<double> reward, double currentState) 
{
	//this for previous state and action after said action is taken
	double maxQ = *max_element(qTable[currentState].begin(), qTable[currentState].end());
	qTable[state][action] = qTable[state][action] + alpha*(reward.at(currentState)+gamma*maxQ - qTable[state][action]);
	state = currentState;
}

int main()
{
	srand(time(NULL));

	// initialize variables
	int xmax = 5;
	int ymax = 5;
	int goalx = 2;
	int goaly = 2;
	
	// create grid
	grid gridWorld;
	gridWorld.init(xmax, ymax, goalx, goaly);
	gridWorld.coordToStates();
	gridWorld.createRewardTable();

	//create agent
	agent doubleOseven; 
	doubleOseven.init();
	doubleOseven.placeAgent(gridWorld.xcoord, gridWorld.ycoord, gridWorld.xmax, gridWorld.ymax);
	doubleOseven.initQ(gridWorld.states);

	cout << "Agent's state: " << doubleOseven.state << endl;
	cout << "Goal State: " << gridWorld.goalState << endl;
	system("pause");

	while (doubleOseven.state != gridWorld.goalState)
	{
		doubleOseven.moveAgent(gridWorld);
	}

	cout << "Goal was at: " << gridWorld.goalPosX << ", " << gridWorld.goalPosY << endl;
	cout << "Agent ended at" << doubleOseven.xpos << ", " << doubleOseven.ypos << endl;
	cout << "Number of moves: " << doubleOseven.numMoves << endl;

	system("pause");
    return 0;
}

