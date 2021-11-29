#include <bits/stdc++.h>

#include "Tuple.h"
#define BUCKET_SIZE 4

using namespace std;

// struct Tuple {
// 	string AUTHOR_ID;
// 	string BOOK_ID;
// 	string AUTHOR_NAME;
// 	string BOOK;
// 	Tuple(string auth_id, string b_id, string auth_name, string b_name) : 
// 		AUTHOR_ID(auth_id),
// 		BOOK_ID(b_id), 
// 		AUTHOR_NAME(auth_name), 
// 		BOOK(b_name) {}
	
// 	void printTuple() {
// 		cout << AUTHOR_ID << "\t" << BOOK_ID << "\t" << AUTHOR_NAME << "\t" << BOOK << endl;
// 	}
// };

struct Bucket {
	int localDepth;
	int size;
	array<Tuple*, BUCKET_SIZE> data;
	Bucket() : localDepth(1), size(0) {}
	Bucket(int lDepth) : localDepth(lDepth), size(0) {
		for(Tuple *&b : data) b = nullptr;
	}
};

class ExtendibleHash {
private:
	vector<Bucket *> directories;
	vector<string> attributes;
	unordered_map<string, set<string>> searchBookID;
	int globalDepth;

public:
	ExtendibleHash(vector<string> &attributes) {
		globalDepth = 0;
		directories.push_back(new Bucket(globalDepth));
		this->attributes = attributes;
	}

	~ExtendibleHash() {
		for(Bucket *&b : directories) {
			for(Tuple *&t : b->data) delete t;
			delete b;
		}
		directories.clear();
		searchBookID.clear();
	}

	int getHash(string key) {
		int res = 0, MOD = (1 << globalDepth);
		for(char c : key) {
			res += (int)c % MOD;
		}
		return res % MOD;
	}

	void doubleDirectories() {
		int sz = directories.size();
		globalDepth++;
		for(int i = 0; i < sz; i++) { 
			directories.push_back(directories[i]);
		}
	}

	void bucketSplit(int idx, int l_depth) {
		Bucket *b = directories[idx];
		array<Tuple *, BUCKET_SIZE> temp = b->data;
		for(Tuple *&t : b->data) t = nullptr;
		b->size = 0;
		for(Tuple *t : temp) {
			if(t == nullptr) continue;
			int curr_idx = getHash(t->get("BOOK_ID"));
			if(curr_idx != idx && directories[idx] == directories[curr_idx]) {
				directories[curr_idx] = new Bucket(b->localDepth);
			}
		}
		for(Tuple *t : temp) {
			if(t == nullptr) continue;
			insert(t);
		}
	}

	void insert(Tuple *row) {
		int idx = getHash(row->get("BOOK_ID"));
		Bucket *b = directories[idx];
		if(b->size >= BUCKET_SIZE) {
			if(b->localDepth == globalDepth) {
				doubleDirectories();
			}
			b->localDepth++;
			bucketSplit(idx, b->localDepth);
			// insert(row);
			idx = getHash(row->get("BOOK_ID"));
			b = directories[idx];
			for(Tuple *&t : b->data) {
				if(t == nullptr) { 
					t = row;
					b->size++;
					searchBookID[t->get("AUTHOR_NAME").substr(0, 3)].insert(t->get("BOOK_ID"));
					searchBookID[t->get("BOOK").substr(0, 3)].insert(t->get("BOOK_ID"));
					return;
				}
			}
		} else {
			for(Tuple *&t : b->data) {
				if(t == nullptr) {
					t = row;
					b->size++;
					searchBookID[t->get("AUTHOR_NAME").substr(0, 3)].insert(t->get("BOOK_ID"));
					searchBookID[t->get("BOOK").substr(0, 3)].insert(t->get("BOOK_ID"));
					return;
				}
			}
		}
	}

	Tuple *getValue(string id) {
		int idx = getHash(id);
		if(idx >= (int)directories.size()) {
			return nullptr;
		}
		for(Tuple *&t : directories[idx]->data) {
			if(t == nullptr) continue;
			if(t->get("BOOK_ID") == id) {
				return t;
			}
		}
		return nullptr;
	}

	void searchBook(string query) {
		if(searchBookID.find(query) == searchBookID.end()) {
			cout << "No Results Found...\n";
			return;
		}
		set<string> &temp = searchBookID[query];
		cout << "RESULTS....\n";
		for(string id : temp) {
			Tuple *t = getValue(id);
			if(t == nullptr) continue;
			t->printTuple();
		}
	}
	
	// returns true if removed successfully else false
	bool removeValue(string id) {
		int idx = getHash(id);
		if(idx >= (int)directories.size()) return false;
		for(Tuple *&t : directories[idx]->data) {
			if(t == nullptr) continue;
			if(t->get("BOOK_ID") == id) {
				delete t;
				t = nullptr;
				return true;
			}
		}
		return false;
	}

	ExtendibleHash *SELECT(const string &attr, const string &val) {
		ExtendibleHash *res = new ExtendibleHash(attributes);
		for(int i = 0; i < (int)directories.size(); i++) {
			Bucket *b = directories[i];
			for(Tuple *&t : b->data) {
				if(t == nullptr) {
					continue;
				}
				if(t->get(attr) == val) {
					Tuple *tmp = new Tuple(*t);
					res->insert(tmp);
				}
			}
		}
		return res;
	}


	ExtendibleHash *PROJECT(vector<string> &attrs) {
		ExtendibleHash *res = new ExtendibleHash(attributes);
		for(int i = 0; i < (int)directories.size(); i++) {
			Bucket *b = directories[i];
			for(Tuple *&t : b->data) {
				if(t == nullptr) {
					continue;
				}
				vector<string> vals;
				for(const string &str : attrs) {
					vals.push_back(t->get(str));
				}
				Tuple *tmp = new Tuple(attrs, vals);
				res->insert(tmp);
			}
		}
		return res;
	}


	ExtendibleHash *JOIN(ExtendibleHash *&other) {
		vector<string> attrs = other->attributes;
		ExtendibleHash *res = new ExtendibleHash(attributes);
		for(int i = 0; i < (int)directories.size(); i++) {
			Bucket *b = directories[i];
			for(Tuple *&t : b->data) {
				if(t == nullptr) {
					continue;
				}
				vector<string> vals;
				for(const string &str : attrs) {
					vals.push_back(t->get(str));
				}
				Tuple *tmp = new Tuple(attrs, vals);
				res->insert(tmp);
			}
		}
		return res;
	}

	void printEverything() {
		cout << "\n---------------------\n";
		for(int i = 0; i < (int)directories.size(); i++) {
			cout << "\nDirectory #" << i << "\t=>\t" << "Local Depth: ";
			Bucket *b = directories[i];
			cout << b->localDepth << "\n\n";
			for(Tuple *&t : b->data) {
				if(t == nullptr) {
					cout << "NULL VALUE\n";
					continue;
				}
				t->printTuple();
			}
			cout << "\n---------------------\n";
		}
	}
};

int atoi(string s) {
	int res = 0;
	for(int i = 0; i < (int)s.size(); i++)
		res = res * 10 + s[i] - '0';	
	return res;
}

int main() {
	cin.clear();
	cin.sync();
	string tmp;
	getline(cin, tmp);
	int m = atoi(tmp);
	getline(cin, tmp);
	int n = atoi(tmp);
	vector<string> attributes;
	for(int i = 0; i < m; i++) {
		getline(cin, tmp);
		attributes.push_back(tmp);
	}
	ExtendibleHash table(attributes);
	for(int i = 0; i < n; i++) {
		vector<string> values;
		for(int j = 0; j < m; j++) {
			getline(cin, tmp);
			values.push_back(tmp);
		}
		Tuple *row = new Tuple(attributes, values);
		// cout << "INSERTING : \"" << inp[0] << '\t' << inp[1] << '\t' << inp[2] << "\t" << inp[3] << "\"\n";
		table.insert(row);
	}
	table.printEverything();
	cout << "\n-------------QUERIES------------\n";
	cout << "Enter Book Name or Author Name to search: " << endl;;

	string searchQuery;
	while(getline(cin, searchQuery)) {
		cout << "\nSearching \"" << searchQuery << "\"" << endl;
		table.searchBook(searchQuery);
		cout << "---------------------\n";
	}
	return 0;
}
