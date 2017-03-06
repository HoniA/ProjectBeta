// ProjectBeta.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <random>
#include <vector>
#include <assert.h>
#include<algorithm>
#include <time.h>
#include <fstream>

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
	int initXPos;
	int initYPos;
	int xpos; // bounded between vertical "bumpers"
	int ypos; // bounded between horizontal "bumpers"
	int initState; //only holds the initial state
	int state; // funxtion of x and y and xmax (from grid)
	int numMoves;
	double epsilon = 0.1; // greedy value
	double alpha = 0.1; // learning value
	double gamma = 0.9;

	vector<vector<double>> initqTable; // only holds initial q table
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
		initXPos = 0;
	else if (userx > size(x) - 1)
		initXPos = size(x) - 1;
	else
		initXPos = userx;

	if (usery < 0)
		initYPos = 0;
	else if (usery > size(y) - 1)
		initYPos = size(y) - 1;
	else
		initYPos = usery;
	
	xpos = initXPos;
	ypos = initYPos;
	initState = xpos + ypos*(xmax + 1);
	state = initState;
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
		initqTable.push_back(actions);
		// Clear actions before resetting them
		actions.clear();
	}

	qTable = initqTable;
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

void runQLearner(grid g, agent a, ofstream &fout, int r, int e);

void TestD(vector<vector<double>> Q, vector<double> r);

void TestE(agent a);

void TestF(int moves);

int main()
{
	srand(time(NULL));

	ofstream fout;
	fout.clear();
	fout.open("LearningCurveData.txt");

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
	
	fout << "Run" << "\t" << "Episode" << "\t" << "Number of Moves" << endl;

	//do some Q learning
	for (int i = 0; i < 30; i++)
	{
		for (int j = 0; j < 50; j++)
		{
			runQLearner(gridWorld, doubleOseven, fout, i, j);
		}
	}
	fout.close();
	TestD(doubleOseven.qTable, gridWorld.rewardTable);
	TestE(doubleOseven);
	TestF(doubleOseven.numMoves);

	system("pause");
    return 0;
}

void runQLearner(grid g, agent a, ofstream &fout, int r, int e)
{
	//cout << "Initial X Pos: " << a.xpos << " Initial Y Pos: " << a.ypos << "Initial State: " << a.state << endl;
	while (a.state != g.goalState)
	{
		a.moveAgent(g);
	}

	fout << r << "\t" << e << "\t" << a.numMoves << endl;

/*cout << "Goal was at: " << g.goalPosX << ", " << g.goalPosY << endl;
cout << "Agent ended at" << a.xpos << ", " << a.ypos << endl;
cout << "Number of moves: " << a.numMoves << endl;*/

a.xpos = a.initXPos;
a.ypos = a.initYPos;
a.state = a.initState;
//reset number of moves to zero
a.numMoves = 0;
}

void TestD(vector<vector<double>> Q, vector<double> s)
{
	int numStates = size(s);
	// loop through q table
	// check if any are greater than 100 (reward of goal state)
	for (int i = 0; i < numStates; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			assert(Q[i][j] <= 100);
		}
	}
}

void TestE(agent a)
{
	// only runs after reaching goal state
	assert(a.state = a.initState);
	assert(a.initqTable != a.qTable);
}

void TestF(int moves)
{
	//don't know what optimal number is yet
}

void TestG()
{
	// dunno how to do this yet
	// new state representation
	// run same q-learning process
}