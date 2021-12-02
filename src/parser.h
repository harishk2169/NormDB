#pragma once
#include <bits/stdc++.h>

using namespace std;

class Parser
{
	// private:
public:
	unordered_set<string> all_clause_set;
	string query, tabelname;
	vector<string> query_token;
	unordered_set<string> clause_set;
	int idx_token = 1;

	unordered_map<int, string> error_table = {
		{0, "Select clause is missing.\n"},
		{1, "From clause is missing.\n"},
		{2, "Order By clause is not well structured.\n"},
		{3, "Column name not provided.\n"},
		{4, "Table name not provided.\n"},
		{5, "No SET clause found.\n"}
	};

	// public:
	vector<vector<string>> mat;

	Parser(string in_query);

	void choose_select();
	void choose_create(vector<pair<string, string>> &val);
	void choose_insert(vector<string> &val);
	void choose_delete();
	void choose_drop();
	void choose_update();
	string preprocess(string q);
	void tokenize();

	// create
	void extract_create(vector<pair<string, string>> &val);

	// read / select
	int extract_column();
	int extract_table();
	int extract_condition();
	int extract_sort_column();

	// update
	void extract_update();
	// delete
	void extract_delete();

	// insert
	void extract_insert(vector<string> &val);

	// drop
	void extract_drop();

	bool check_clause_present(string t);
	string remove_punct(string x);

	void print_str();
	void print_mat();
	void print_clauses();
};

void print_table(vector<pair<string, string>> &val);
void conditionTable(vector<vector<string>> &t, vector<string> &res, string cond);
void sortColumn(vector<vector<string>> &t, vector<vector<string>> &res, string col);
char* process_Query(string query);