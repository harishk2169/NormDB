#include "Tuple.h"

Tuple::Tuple(vector<string> &attributes, vector<string> &values) {
	if(attributes.size() != values.size()) {
		cout << "Invalid constructor arguments" << endl;
	}
	for(int i = 0; i < (int)attributes.size(); i++) {
		index[attributes[i]] = i;
		row.push_back(values[i]);
	}
}

string Tuple::get(const string &attr) {
	if(index.find(attr) == index.end()) {
		cout << "No such attribute present" << endl;
		return "";
	}
	assert(index[attr] < row.size());
	int ind = 0;
	auto it = row.begin();
	while(ind < row.size()) {
		if(ind == index[attr]) {
			return *it;
		}
		it++;
		ind++;
	}
}

void Tuple::printTuple() {
	for(const string &val : row) {
		cout << val << ' ';
	}
	cout << '\n';
}