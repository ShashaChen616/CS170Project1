#include <iostream>
#include <vector>
#include <queue>
#include <set>

using namespace std;

//node represent a state in the puzzle
struct Node {
	//current state of the puzzle
	vector<vector<int>> state;
	int costToNode, heuCost;
	Node* parent;

	//constructor
	Node(vector<vector<int>> aState, int aCostToNode, int aHeuCost, Node* aParent = nullptr) : state(aState), costToNode(aCostToNode), heuCost(aHeuCost), parent(aParent) {}

	//total cost = costToNode + heuCost
	int totalCost() const {
		return costToNode + heuCost;
	}

	//compares nodes by the totalCost value
	bool operator>(const Node& other) const {
		return totalCost() > other.totalCost();
	}

};

//defines the goal state of the 8-puzzle
vector<vector<int>> goalState = { {1,2,3},{4,5,6},{7,8,0} };

//poosible moves for the blank space: right, down, left, up
vector<pair<int, int>> moves = { {0,1},{1,0},{0,-1},{-1,0} };

//checks if a given state is the goal state
bool isGoal(const vector<vector<int>>& state) {
	return state == goalState;
}

//counts the misplaced tiles in the current state compared to the goal state
int misplacedTiles(const vector<vector<int>>& state) {
	//a counter that counts the number of misplaced tile
	int numOfTile = 0;

	//a loop that run through each row and each column
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			//if the tile is 0, and not equal to the goalState, number of misplaced tile + 1
			//keeps adding the numOfTile until the loop finished
			if (state[i][j] != 0 && state[i][j] != goalState[i][j]) {
				numOfTile++;
			}
		}
	}
	return numOfTile;
}

//calculates the manhattan distance for each tile
int manhattanDistance(const vector<vector<int>>& state) {
	//manhattan distance 
	int distance = 0;
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			//ignore 0 here because the empty space does not contribute to the distance
			if (state[i][j] != 0) {
				//goal positions of the tile
				int goalX = (state[i][j] - 1) / 3;
				int goalY = (state[i][j] - 1) % 3;
				distance += abs(i - goalX) + abs(j - goalY);
				/* example of this algorithm
				*  for a puzzle of:
				*  1 2 3
				*  4 5 6
				*  0 7 8
				*  in here, 7 is at (2,1), while its goal position should be (2,0), so the distance = |2-2| + |1-0| = 1;
				*/
			}
		}
	}
	return distance;
}

//expand a node by generating all valid child states
vector<Node*> expand(Node* node, int typeOfHeu) {
	//stores the child nodes
	vector<Node*> childrenNode;

	//finds the coordinates of the empty space 0;
	int zeroX, zeroY;
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			if (node->state[i][j] == 0) {
				zeroX = i;
				zeroY = j;
			}
		}
	}

	//generates child state for each possible move
	for (int k = 0; k < moves.size(); ++k) {
		//direction of the move
		int directionX = moves[k].first;
		int directionY = moves[k].second;
		//new position of the blank space
		int newX = zeroX + directionX;
		int newY = zeroY + directionY;

		//check if the move if within bounds
		if (newX >= 0 && newX < 3 && newY >= 0 && newY < 3) {
			//copy the current state and swap the position of the current state with the new state
			vector<vector<int>> newState = node->state;
			swap(newState[zeroX][zeroY], newState[newX][newY]);

			//check which algorithm is selected, default = UCS, has no heuristic
			int heu = 0;
			// Misplaced Tile heuristic
			if (typeOfHeu == 1) {
				heu = misplacedTiles(newState);
			}
			// Manhattan Distance heuristic
			else if (typeOfHeu == 2) {
				heu = manhattanDistance(newState);
			}

			//creates a new node for the child state
			childrenNode.push_back(new Node(newState, node->costToNode + 1, heu, node));
		} 
	}
	return childrenNode;
}

//general search algorithm
void generalSearch(vector<vector<int>> puzzle, int typeOfHeu) {
	struct NodeComparator {
		//compares nodes by the totalCost
		bool operator()(const Node* lhs, const Node* rhs) const {
			return lhs->totalCost() > rhs->totalCost();
		}
	};

	//create a priority queue (MAKE_QUEUE equivalent)
	priority_queue<Node*, vector<Node*>, NodeComparator> nodes;

	//check which algorithm is selected, default = UCS, has no heuristic
	int heu = 0;
	// Misplaced Tile heuristic
	if (typeOfHeu == 1) {
		heu = misplacedTiles(puzzle);  
	}
	// Manhattan Distance heuristic
	else if (typeOfHeu == 2) {
		heu = manhattanDistance(puzzle);  
	}
	
	//push the puzzle into the queue
	nodes.push(new Node(puzzle, 0, heu));

	//tracks visited states
	set<vector<vector<int>>> visited;

	int maxQueueSize = 0;
	int nodesExpanded = 0;

	//
	while (true) {
		//if the queue is empty, return false
		if (nodes.empty()) {
			cout << "Failure: No solution found." << endl;
			return;
		}

		//generates the max size of the queue
		maxQueueSize = max(maxQueueSize, static_cast<int>(nodes.size()));

		//set the node with the smallest totalCost value, and removes it from the queue
		Node* node = nodes.top();
		nodes.pop();

		//prints the solution path
		//cout << "The best state to expand with g(n) = " << node->costToNode << " and h(n) = " << node->heuCost << " is: \n";
		cout << "Expanding node with g(n) = " << node->costToNode
			<< " and h(n) = " << node->heuCost
			<< " => f(n) = " << node->totalCost() << endl;

		for (const auto& row : node->state) {
			for (int number : row) {
				cout << number << " ";
			}
			cout << endl;
		}
		cout << "------------\n";


		//check if reaches to the goal
		if (isGoal(node->state)) {
			//stores the solution path
			vector<Node*> path;

			//backtrack to the root
			while (node) {
				path.push_back(node);
				node = node->parent;
			}
			//reverse the path to get the correct order
			reverse(path.begin(), path.end());

			cout << "Goal state!\n\n";

			//print the solution path
			/*
			for (const Node* n : path) {
				for (const auto& row : n->state) {
					for (int number : row) {
						cout << number << " ";
					}
					cout << "------------\n";
				}
			}*/

			//print the results
			cout << "Solution depth: " << path.size() - 1 << endl;
			cout << "Number of nodes expanded: " << nodesExpanded << endl;
			cout << "Max queue size: " << maxQueueSize << endl;
			return;
		}

		//if the state has been visited, skip it
		if (visited.find(node->state) != visited.end()) {
			continue;
		}

		//mark the state as visited
		visited.insert(node->state);

		//increase nodes expanded
		nodesExpanded++;

		//expand the node to generate child states
		vector<Node*> children = expand(node, typeOfHeu);

		//add each child to the queue
		for (int i = 0; i < children.size(); i++) {
			nodes.push(children[i]);
		}
	}

}

int main() {
	//create a 3x3 array using vector
	vector<vector<int>> puzzle(3, vector<int>(3));
	cout << "Enter the 8-puzzle you want to solve (HINT: enter 3 numbers (0-8 no repeat) in a row (3 rows total), use space to separate each number, press 'enter key' for next row, use 0 for the blank space): \n";

	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			cin >> puzzle[i][j];
		}
	}

	//select an algorithm
	int choice;
	cout << "Select the algorithm you want to use to solve the puzzle (HINT: enter 1-3 only):\n"
		<< "1. Uniform Cost Search\n"
		<< "2. A* with Misplaced Tile heuristic\n"
		<< "3. A* with Mangattan Distance heuristic\n";
	cin >> choice;

	//call the function
	generalSearch(puzzle, choice - 1);

	return 0;
}