#include <bits/stdc++.h>

#include "Tuple.h"

#define BUCKET_SIZE 4

using namespace std;

struct Bucket {
	int size;
	array<Tuple*, BUCKET_SIZE> data;
	vector<Tuple *> overflows;
	Bucket() : size(0) {
		for(Tuple *&b : data) b = nullptr;
	}
};

class LinearHash {
private:
	vector<Bucket *> buckets;
	vector<string> attributes;
	unordered_map<string, set<string>> searchBookID;
	int next_idx;
	int level, N, count;
	double load_factor;

public:
	LinearHash(vector<string> &attributes) {
		level = 1;
		next_idx = 0;
		count = 0;
		load_factor = 0.75;
		buckets.push_back(new Bucket());
		buckets.push_back(new Bucket());
		this->attributes = attributes;
	}

	int getHash(string key) {
		int res = 0, MOD = (1 << level);
		for(char c : key) {
			res += (int)c % MOD;
		}
		return res % MOD;
	}

	bool splitRequired() {
		double value = (double)count / (double)(BUCKET_SIZE * buckets.size());
		return value > load_factor;
	}

	void clearBucket(Bucket *b) {
		b->size = 0;
		for(Tuple *&t : b->data) t = nullptr;
		b->overflows.clear();
	}

	void bucketSplit() {
		buckets.push_back(new Bucket());
		Bucket *b = buckets[next_idx];
		array<Tuple *, BUCKET_SIZE> temp = b->data;
		vector<Tuple *> tmp_overflows = b->overflows;
		count -= b->size;
		count -= tmp_overflows.size();
		clearBucket(b);
		next_idx = (next_idx + 1) % (1 << level);
		level++;	// Distribute the new elements with h_(level + 1)
		for(Tuple *&t : temp) {
			if(t) insert(t);
		}
		for(Tuple *&t : tmp_overflows) {
			if(t) insert(t);
		}
		level--;
		if(next_idx == 0) level++;
	}

	void insert(Tuple *row) {
		int idx = getHash(row->get("BOOK_ID"));
		Bucket *b = buckets[idx];
		if(b->size >= BUCKET_SIZE) {
			b->overflows.push_back(row);
			searchBookID[row->get("AUTHOR_NAME").substr(0, 3)].insert(row->get("BOOK_ID"));
			searchBookID[row->get("BOOK").substr(0, 3)].insert(row->get("BOOK_ID"));
			count++;
		} else {
			for(Tuple *&t : b->data) {
				if(t == nullptr) {
					t = row;
					b->size++;
					count++;
					searchBookID[t->get("AUTHOR_NAME").substr(0, 3)].insert(t->get("BOOK_ID"));
					searchBookID[t->get("BOOK").substr(0, 3)].insert(t->get("BOOK_ID"));
					break;
				}
			}
		}
		if(splitRequired()) {
			bucketSplit();
		}
	}

	Tuple *getValue(string id) {
		int idx = getHash(id);
		if(idx >= (int)buckets.size()) {
			return nullptr;
		}
		for(Tuple *&t : buckets[idx]->data) {
			if(t == nullptr) continue;
			if(t->get("BOOK_ID") == id) {
				return t;
			}
		}
		for(Tuple *&t : buckets[idx]->overflows) {
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

	LinearHash *SELECT(const string &attr, const string &val) {
		LinearHash *res = new LinearHash(attributes);
		for(int i = 0; i < (int)buckets.size(); i++) {
			Bucket *b = buckets[i];
			for(Tuple *&t : b->data) {
				if(t == nullptr) {
					continue;
				}
				if(t->get(attr) == val) {
					Tuple *tmp = new Tuple(*t);
					res->insert(tmp);
				}
			}
			if(b->overflows.size()) {
				for(Tuple *&t : b->overflows) {
					if(t == nullptr) {
						continue;
					}
					if(t->get(attr) == val) {
						Tuple *tmp = new Tuple(*t);
						res->insert(tmp);
					}
				}
			}
		}
		return res;
	}

	LinearHash *PROJECT(vector<string> &attrs) {
		LinearHash *res = new LinearHash(attributes);
		for(int i = 0; i < (int)buckets.size(); i++) {
			Bucket *b = buckets[i];
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
			if(b->overflows.size()) {
				for(Tuple *&t : b->overflows) {
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
		}
		return res;
	}


	LinearHash *JOIN(LinearHash *&other) {
		vector<string> attrs = other->attributes;
		LinearHash *res = new LinearHash(attributes);
		for(int i = 0; i < (int)buckets.size(); i++) {
			Bucket *b = buckets[i];
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
			if(b->overflows.size()) {
				for(Tuple *&t : b->overflows) {
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
		}
		return res;
	}

	void printEverything() {
		cout << "\n---------------------\n";
		for(int i = 0; i < (int)buckets.size(); i++) {
			cout << "\nBuckets #" << i << "\n";
			Bucket *b = buckets[i];
			for(Tuple *&t : b->data) {
				if(t == nullptr) {
					cout << "NULL VALUE\n";
					continue;
				}
				t->printTuple();
			}
			if(b->overflows.size()) {
				cout << "Overflows : \n";
				for(Tuple *&t : b->overflows) {
					if(t == nullptr) {
						cout << "NULL VALUE\n";
						continue;
					}
					t->printTuple();
				}
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
	LinearHash table(attributes);
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
	// string bID;
	// while(getline(cin, bID)) {
	// 	cout << "\nBOOK_ID searched : " << bID;
	// 	cout << "\nThe tuple retrieved from table is : \n";
	// 	Tuple *value = table.getValue(bID);
	// 	if(value)
	// 		value->printTuple();
	// 	else
	// 		cout << "value with BOOK_ID : " << bID << " not found!!\n";
	// }
	string searchQuery;
	while(getline(cin, searchQuery)) {
		cout << "\nSearching \"" << searchQuery << "\"" << endl;
		table.searchBook(searchQuery);
		cout << "---------------------\n";
	}
	return 0;
}
