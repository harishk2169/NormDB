#pragma once
#include <bits/stdc++.h>

using namespace std;

class Tuple {
private:
	unordered_map<string, int> index;	// {attribute, position}
	list<string> row;
public:
	Tuple(vector<string> &attributes, vector<string> &values);
	string get(const string &attr);
	void printTuple();
};