#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <cstdlib>
#include <climits>
#include <unistd.h>
using namespace std;

int k;
string input;
vector<int> cost;
vector<vector<int> > follow;
vector<int> S;

void fileInput() {
	ifstream in(input);
	int numberOfViews, numberOfEdges;
	in >> numberOfViews;
	for (int i = 0; i <= numberOfViews; i++) {
		cost.push_back(0);
		follow.push_back(vector<int>{i});
	}
	for (int i = 1; i <= numberOfViews; i++) {
		int _number, _cost;
		in >> _number >> _cost;
		cost[_number] = _cost;
	}
	in >> numberOfEdges;
	while (numberOfEdges--) {
		int _followed, _following;
		in >> _followed >> _following;
		for (int i = 1; i <= numberOfViews; i++)
			for (int j = 0; j < follow[i].size(); j++)
				if (follow[i][j] == _followed)
					for (int k = 0; k < follow[_following].size(); k++)
						follow[i].push_back(follow[_following][k]);
	}
	for (int i = 1; i < follow.size(); i++) {
		sort(follow[i].begin(), follow[i].end());
		for (int j = 0; j < follow[i].size() - 1; j++)
            if (follow[i][j] == follow[i][j + 1]) {
                follow[i].erase(follow[i].begin() + j);
                j--;
            }
	}
}

int benefit(int v) {
	int Bw = 0;
	for (int j = 0; j < follow[v].size(); j++) {
		int w = follow[v][j];
		vector<int> U;
		for (int i = 0; i < S.size(); i++)
			for (int k = 0; k < follow[S[i]].size(); k++)
				if (follow[S[i]][k] == w)
					U.push_back(S[i]);
		int min = INT_MAX, u;
		for (int i = 0; i < U.size(); i++)
			if (cost[U[i]] < min) {
				min = cost[U[i]];
				u = U[i];
			}
		if (cost[v] < cost[u])
			Bw += (cost[u] - cost[v]);
	}
	return Bw;
}

void algo() {
	int max_size = 0, top_view;
	for (int i = 1; i < follow.size(); i++)
		if (follow[i].size() > max_size) {
			top_view = i;
			max_size = follow[i].size();
		}
	S = {top_view};
	while (k--) {
		int max_v, max_benefit = 0;
		for (int i = 1; i < follow.size(); i++)
			if (benefit(i) > max_benefit) {
				max_v = i;
				max_benefit = benefit(i);
			}
		S.push_back(max_v);
	}
}

int main(int argc, char** argv) {
	int c;
	while ((c = getopt(argc, argv, "f:k:")) != -1) {
		switch(c) {
			case 'f':
				input = optarg;
			case 'k':
				k = atoi(optarg);
		}
	}
	fileInput();
	algo();
	for (int i = 0; i < S.size() - 1; i++)
		cout << S[i] << " -> ";
	cout << S[S.size() - 1] << endl;
	return 0;
}
