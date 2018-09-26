#pragma GCC optimize "O3,omit-frame-pointer,inline"
#include <iostream>
#include <algorithm>
#include <fstream>
#include <vector>
#include <string>
#include <climits>
#include <random>
#include <chrono>
#include <cmath>
#include <ctime>
#include <unistd.h>
using namespace std;

const int m = 100;  // number of random projection vectors
int n;  // number of objects
int dimension;  // dimension of each object
string file_path;  // path of database file
vector<vector<unsigned char> > data;  // objects

// construct a trivial random generator engine from a time-based seed
default_random_engine generator(chrono::system_clock::now().time_since_epoch().count());

// read from database
void read() {
    unsigned char temp;
    ifstream file(file_path, ios::binary);
    for (int i = 0; i < 16; i++)
    	file.read((char*)&temp, sizeof(temp));
	data.resize(n);
	for (int i = 0; i < n; i++) {
		data[i].resize(dimension);
        for (int j = 0; j < dimension; j++)
			file.read((char*)&data[i][j], sizeof(temp));
	}
}

// initialize
void init(int argc, char** argv) {
	for (int i = 0; i < argc; i++) {
		if ((string)argv[i] == "-n")
			n = atoi(argv[i + 1]);
		if ((string)argv[i] == "-d")
			dimension = atoi(argv[i + 1]);
		if ((string)argv[i] == "-f")
			file_path = argv[i + 1];
	}
	read();
}

// print an image with location i
void print(int i) {
	for (int j = 0; j < dimension; j++) {
       	if (data[i][j] > 0)
       		cout << " ";
       	else
       		cout << "*";
		if (j % 28 == 27)
			cout << endl;
		if (j == 783)
			cout << endl;
    }
}

// print a pair by calling method print
void print_pair(pair<int, int> p) {
	print(p.first);
	print(p.second);
}

// generate random projection vectors
vector<vector<double> > generate() {
	vector<vector<double> > projector;
	projector.resize(m);
	normal_distribution<double> distribution;
	for (int i = 0; i < m; i++) {
		projector[i].resize(dimension);
		for (int j = 0; j < dimension; j++)
			projector[i][j] = distribution(generator);
	}
	return projector;
}

// dot product of a random projection vector and a object vector
double dot_product(vector<double> a, vector<unsigned char> o) {
	double result = 0;
	for (int i = 0; i < dimension; i++)
		result += a[i] * (double)o[i];
	return result;
}

// euclidean distance of closest pair candidates
double euclidean_distance(pair<int, int> cp) {
	int sq = 0;
	for (int i = 0; i < dimension; i++)
		sq += pow((int)data[cp.first][i], 2) + pow((int)data[cp.second][i], 2);
	return sqrt((double)sq);
}

// comparing function
bool comp(pair<int, double> x, pair<int, double> y) {
	return x.second < y.second;
}

// solve by sorting
pair<int, int> closest_pair_line(vector<pair<int, double> > line) {
	sort(line.begin(), line.end(), comp);
	double min = INT_MAX;
	pair<int, int> cp;
	for (int i = 1; i < n; i++)
		if (abs(line[i].second - line[i - 1].second) < min) {
			min = abs(line[i].second - line[i - 1].second);
			cp = pair<int, int>(line[i].first, line[i - 1].first);
		}
	return cp;
}

// find the median
double find_the_median(vector<pair<int, double> > line, int k) {
	if (line.size() == 0)
		return 0;
	if (line.size() < 2)
		return line[0].second;
	uniform_int_distribution<> distribution(0, line.size() - 1);
	double splitter = line[distribution(generator)].second;
	vector<pair<int, double> > S1;  // S-
	vector<pair<int, double> > S2;  // S+
	for (int i = 0; i < (int)line.size(); i++) {
		if (line[i].second < splitter)
			S1.push_back(line[i]);
		if (line[i].second > splitter)
			S2.push_back(line[i]);
	}
	if ((int)S1.size() == k - 1)
		return splitter;
	else if ((int)S1.size() >= k)
		return find_the_median(S1, k);
	else
		return find_the_median(S2, k - 1 - S1.size());
}

// help to find the minimum in the algorithm 3
pair<double, pair<int, int> > min_helper(vector<pair<double, pair<int, int> > > pairs) {
	double min = INT_MAX;
	pair<double, pair<int, int> > result = pair<double, pair<int, int> >(INT_MAX, pair<int, int>(0, 0));
	for (int i = 0; i < (int)pairs.size(); i++)
		if (pairs[i].first < min) {
			min = pairs[i].first;
			result = pairs[i];
		}
	return result;
}

// algo 3
pair<double, pair<int, int> > closest_pair_median(vector<pair<int, double> > line) {
	if (line.size() <= 1)
		return pair<double, pair<int, int> >(INT_MAX, pair<int, int>(0, 0));
	if (line.size() == 2) {
		if (line[0].second > line[1].second) {
			pair<int, double> temp = line[0];
			line[0] = line[1];
			line[1] = temp;
		}
		return pair<double, pair<int, int> >(abs(line[0].second - line[1].second), pair<int, int>(line[0].first, line[1].first));
	}
	double median = find_the_median(line, line.size() / 2);
	vector<pair<int, double> > S1;
	vector<pair<int, double> > S2;
	pair<int, double> max = pair<int, double>(0, INT_MIN / 2);
	pair<int, double> min = pair<int, double>(0, INT_MAX / 2);
	for (int i = 0; i < (int)line.size(); i++) {
		if (line[i].second <= median) {
			S1.push_back(line[i]);
			if (line[i].second > max.second)
				max = line[i];
		}
		if (line[i].second > median) {
			S2.push_back(line[i]);
			if (line[i].second < min.second)
				min = line[i];
		}
	}
	pair<double, pair<int, int> > cp1 = closest_pair_median(S1);
	pair<double, pair<int, int> > cp2 = closest_pair_median(S2);
	pair<double, pair<int, int> > cp12 = pair<double, pair<int, int> >(abs(max.second - min.second), pair<int, int>(max.first, min.first));
	return min_helper({cp1, cp2, cp12});
}

// algo 4
pair<double, pair<int, int> > closest_pair_pivot(vector<pair<int, double> > line) {
	if (line.size() <= 1)
		return pair<double, pair<int, int> >(INT_MAX, pair<int, int>(0, 0));
	if (line.size() == 2) {
		if (line[0].second > line[1].second) {
			pair<int, double> temp = line[0];
			line[0] = line[1];
			line[1] = temp;
		}
		return pair<double, pair<int, int> >(abs(line[0].second - line[1].second), pair<int, int>(line[0].first, line[1].first));
	}
	uniform_int_distribution<> distribution(0, line.size() - 1);
	double pivot = line[distribution(generator)].second;
	vector<pair<int, double> > S1;
	vector<pair<int, double> > S2;
	pair<int, double> max = pair<int, double>(0, INT_MIN / 2);
	pair<int, double> min = pair<int, double>(0, INT_MAX / 2);
	for (int i = 0; i < (int)line.size(); i++) {
		if (line[i].second <= pivot) {
			S1.push_back(line[i]);
			if (line[i].second > max.second)
				max = line[i];
		}
		if (line[i].second > pivot) {
			S2.push_back(line[i]);
			if (line[i].second < min.second)
				min = line[i];
		}
	}
	pair<double, pair<int, int> > cp1 = closest_pair_median(S1);
	pair<double, pair<int, int> > cp2 = closest_pair_median(S2);
	pair<double, pair<int, int> > cp12 = pair<double, pair<int, int> >(abs(max.second - min.second), pair<int, int>(max.first, min.first));
	return min_helper({cp1, cp2, cp12});
}

// algo 2
vector<vector<pair<int, double> > > random_projection() {
	vector<vector<double> > projector = generate();
	vector<vector<pair<int, double> > > result;
	result.resize(m);
	for (int i = 0; i < n; i++)
		for (int j = 0; j < m; j++)
			result[j].push_back(pair<int, double>(i, dot_product(projector[j], data[i])));
	return result;
}

// algo 1
pair<int, int> closest_pair(int mode) {
	vector<vector<pair<int, double> > > projected = random_projection();
	pair<int, int> temp = pair<int, int>(0, 0);
	pair<int, int> cp = pair<int, int>(0, 0);
	double min = INT_MAX;
	for (int i = 0; i < m; i++) {
		if (mode == 0)
			temp = closest_pair_line(projected[i]);
		if (mode == 1)
			temp = closest_pair_median(projected[i]).second;
		if (mode == 2)
			temp = closest_pair_pivot(projected[i]).second;
		double dist = euclidean_distance(temp);
		if (dist < min) {
			min = dist;
			cp = temp;
		}
	}
	return cp;
}

int main(int argc, char** argv) {
	init(argc, argv);
	for (int mode = 0; mode < 3; mode++) {
		long start = clock();
		print_pair(closest_pair(mode));
		if (mode == 0)
			cout << "By sorting: ";
		if (mode == 1)
			cout << "By Divide-and-Conquer with median: ";
		if (mode == 2)
			cout << "By Divide-and-Conquer with pivot: ";
		cout << (double)(clock() - start) / CLOCKS_PER_SEC << "s" << endl << endl;
	}
	return 0;
}

