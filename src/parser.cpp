#include "parser.h"
#include "disk.h"
#define MAX_INPUT_SIZE 4096

Parser::Parser(string in_query)
{
	vector<string> clauses{"select", "from", "where", "order", "by"};
	for (int i = 0; i < clauses.size(); i++)
	{
		all_clause_set.insert(clauses[i]);
	}
	query = preprocess(in_query);
	tokenize();
}

void Parser::choose_select()
{
	extract_column();
	extract_table();
	if (clause_set.find("where") != clause_set.end())
	{
		extract_condition();
	}
	if (clause_set.find("order") != clause_set.end())
	{
		extract_sort_column();
	}

}
void Parser::choose_create(vector<pair<string, string>> &val)
{
	extract_create(val);
}
void Parser::choose_insert(vector<string> &val)
{
	extract_insert(val);
}
void Parser::choose_delete()
{
	extract_delete();
}
void Parser::choose_drop()
{
	extract_drop();
}
void Parser::choose_update()
{
	extract_update();
}
string Parser::preprocess(string q)
{
	transform(q.begin(), q.end(), q.begin(), ::tolower);
	return q;
}
void Parser::tokenize()
{
	stringstream ss(query);
	string token;
	while (ss >> token)
	{
		query_token.push_back(token);
		if (all_clause_set.find(token) != all_clause_set.end())
		{
			clause_set.insert(token);
		}
	}
}

// create
void Parser::extract_create(vector<pair<string, string>> &val)
{
	idx_token++;
	tabelname = query_token[idx_token++];
	while (idx_token < query_token.size())
	{
		string x = remove_punct(query_token[idx_token++]);
		string y = remove_punct(query_token[idx_token++]);
		val.push_back(make_pair(x, y));
	}
}

// read / select
int Parser::extract_column()
{
	vector<string> column_name{"column_name"};
	if (!check_clause_present("select") || query_token[0] != "select")
	{
		cout << error_table[0];
		return 1;
	}
	else if (!check_clause_present("from"))
	{
		cout << error_table[1];
		return 1;
	}

	while (query_token[idx_token] != "from")
	{
		// a switch case for aggregate functions. "distinct", "count", "avg", "sum"
		column_name.push_back(query_token[idx_token++]);
	}

	if (column_name.size() == 1)
	{
		cout << error_table[3];
		return 1;
	}

	mat.push_back(column_name);
	idx_token++;
	return 0;
}
int Parser::extract_table()
{
	vector<string> tabel_name{"table_name"};

	while (idx_token < query_token.size() && query_token[idx_token] != "where" && query_token[idx_token] != "order")
	{
		tabelname = query_token[idx_token];
		tabel_name.push_back(query_token[idx_token]);
		idx_token++;
	}

	if (tabel_name.size() == 1)
	{
		cout << error_table[4];
		return 1;
	}

	mat.push_back(tabel_name);
	idx_token++;
	return 0;
}
int Parser::extract_condition()
{
	vector<string> condition{"condition"};
	while (idx_token < query_token.size() && query_token[idx_token] != "order")
	{
		condition.push_back(remove_punct(query_token[idx_token]));
		idx_token++;
	}
	idx_token++;
	if (condition.size() > 1)
	{
		mat.push_back(condition);
	}
	return 0;
}
int Parser::extract_sort_column()
{
	vector<string> sort_column{"sort_column"};
	if (idx_token < query_token.size() && query_token[idx_token] != "by")
	{
		cout << error_table[2];
		return 1;
	}
	idx_token++;
	while (idx_token < query_token.size())
	{
		sort_column.push_back(query_token[idx_token]);
		idx_token++;
	}
	idx_token++;
	if (sort_column.size() > 1)
	{
		mat.push_back(sort_column);
	}
	return 0;
}

// update
void Parser::extract_update()
{
	vector<string> update_thing{"update_items"};
	string temp;

	tabelname = query_token[idx_token++];
	if (query_token[idx_token++] != "set")
	{
		cout << error_table[5];
		return;
	}

	while (query_token[idx_token] != "where" && idx_token < query_token.size())
	{
		if (query_token[idx_token] == ",")
		{
			update_thing.push_back(temp);
			idx_token++;
			temp = "";
		}
		else
		{
			temp += query_token[idx_token++] + " ";
		}
	}
	if (temp != "")
	{
		update_thing.push_back(temp);
		temp = "";
	}
	mat.push_back(update_thing);
	idx_token++;
	extract_condition();
}
// delete
void Parser::extract_delete()
{
	tabelname = query_token[2];
	if (query_token[3] == "where")
	{
		idx_token = 4;
		extract_condition();
	}
}

// insert
void Parser::extract_insert(vector<string> &val)
{
	tabelname = query_token[2];
	int i = 4;
	while (i < query_token.size())
	{
		val.push_back(remove_punct(query_token[i++]));
	}
}

// drop
void Parser::extract_drop()
{
	tabelname = query_token[2];
}

bool Parser::check_clause_present(string t)
{
	if (clause_set.find(t) == clause_set.end())
	{
		return false;
	}
	return true;
}
string Parser::remove_punct(string x)
{
	string temp = "";
	for (int j = 0; j < x.size(); j++)
	{
		if (x[j] == '(' || x[j] == ',' || x[j] == ')' || x[j] == ';')
			continue;
		temp += x[j];
	}
	return temp;
}

void Parser::print_str()
{
	cout << query;
}
void Parser::print_mat()
{
	for (auto x : mat)
	{
		for (auto y : x)
		{
			cout << y << "\t";
		}
		cout << "\n";
	}
}
void Parser::print_clauses()
{
	for (auto x : clause_set)
	{
		cout << x << "\t";
	}
	cout << "\n";
}

void print_table(vector<pair<string, string>> &val)
{
	for (auto x : val)
	{
		cout << x.first << " " << x.second << endl;
	}
}

void conditionTable(vector<vector<string>> &t, vector<string> &res, string cond)
{
	int col_idx = -1, row_idx = -1;
	string cond_colum = "";
	string cond_value = "";
	string temp = "";
	for (auto x : cond)
	{
		if (x != '=')
		{
			temp += x;
		}
		else
		{
			cond_colum = temp;
			temp = "";
		}
	}
	cond_value = temp;
	for (int i = 0; i < t[0].size(); i++)
	{
		if (t[0][i] == cond_colum)
		{
			col_idx = i;
			break;
		}
	}
	for (int j = 1; j < t.size(); j++)
	{
		if (t[j][col_idx] == cond_value)
		{
			row_idx = j;
		}
	}
	for (auto x : t[row_idx])
	{
		res.push_back(x);
	}
}

void sortColumn(vector<vector<string>> &t, vector<vector<string>> &res, string col)
{
	int col_idx = -1;
	for (int i = 0; i < t[0].size(); i++)
	{
		if (t[0][i] == col)
		{
			col_idx = i;
			break;
		}
	}
	vector<string> temp;
	unordered_map<string, int> indexes;
	for (int j = 1; j < t.size(); j++)
	{
		indexes[t[j][col_idx]] = j;
		temp.push_back(t[j][col_idx]);
	}
	sort(temp.begin(), temp.end());
	for (int i = 0; i < temp.size(); i++)
	{
		res.push_back(t[indexes[temp[i]]]);
	}
}


// int main()
// {
// 	// string query = "Create table abc (column1 int, column2 var, column3 varchar);";
// 	// string query = "DELETE FROM abc WHERE hey!=hey";
// 	// string query = "DROP TABLE abc ";
// 	// string query = "UPDATE table_name SET column1=value1 , column2=value2 WHERE a=a;";
// 	// string query = "Insert INTO table1 Values (a, b, c)";

// 	vector<vector<string>> tabel1{
// 		{"column1", "column2", "column3"},
// 		{"c1_val1", "c2_val1", "c3_val1"},
// 		{"c1_val3", "c2_val3", "c3_val3"},
// 		{"c1_val2", "c2_val2", "c3_val2"},

// 	};
// 	vector<vector<string>> sorted_table;
// 	vector<string> res;

// 	// string query = "Select * FROM tabel1 WHERE column2=c2_val2";
// 	string query = "SElECT * FROM tabel1 ORDER BY column1";
// 	Parser tes(query);
// 	vector<string> query_token = tes.query_token;
// 	vector<pair<string, string>> tabel_val;
// 	vector<string> row_val;
// 	unordered_map<string, int> query_table{
// 		{"select", 1},
// 		{"create", 2},
// 		{"insert", 3},
// 		{"drop", 4},
// 		{"delete", 5},
// 		{"update", 6}};

// 	switch (query_table[query_token[0]])
// 	{
// 	case 1:
// 		tes.choose_select();
// 		tes.print_mat();
// 		if (tes.clause_set.find("where") != tes.clause_set.end())
// 		{
// 			conditionTable(tabel1, res, tes.mat[2][1]);
// 			for (auto x : res)
// 			{
// 				cout << x << " ";
// 			}
// 		}
// 		if (tes.clause_set.find("order") != tes.clause_set.end())
// 		{
// 			sortColumn(tabel1, sorted_table, tes.mat[2][1]);
// 			for (auto x : sorted_table)
// 			{
// 				for (auto y : x)
// 				{
// 					cout << y << " ";
// 				}
// 				cout << endl;
// 			}
// 		}
// 		break;
// 	case 2:
// 		tes.choose_create(tabel_val);
// 		cout << tes.tabelname << endl;
// 		print_table(tabel_val);
// 		break;
// 	case 3:
// 		tes.choose_insert(row_val);
// 		cout << tes.tabelname << endl;
// 		for (auto x : row_val)
// 		{
// 			cout << x << " ";
// 		}
// 		break;

// 	case 4:
// 		tes.choose_drop();
// 		cout << tes.tabelname << endl;

// 		break;
// 	case 5:
// 		tes.choose_delete();
// 		cout << tes.tabelname << endl;
// 		tes.print_mat();
// 		break;
// 	case 6:
// 		tes.choose_update();
// 		cout << tes.tabelname << endl;
// 		tes.print_mat();
// 		break;
// 	}
// };

void StringToCharPointer(char *to, string &str) {
	bzero(to, MAX_INPUT_SIZE);
	int i = 0;
	for(char c : str) {
		to[i++] = c;
	}
	to[i] = '\0';
	while(i<MAX_INPUT_SIZE){
		to[i]='\0';
		i++;
	}
}

void vectorToCharPointer(char *to, vector<vector<string>> &from) {
	bzero(to, MAX_INPUT_SIZE);
	int i = 0;
	if(from.empty()) {
		strcpy(to, "!!EMPTY!!!!");
		return;
	}
	for(auto &vec : from) {
		for(string &str : vec) {
			for(char c : str) {
				to[i++] = c;
			}
			to[i++] = '\t';
		}
		to[i++] = '\n';
	}
	to[i] = '\0';
	while(i<MAX_INPUT_SIZE){
		to[i]='\0';
		i++;
	}
}

char* process_Query(string query){
 	load("table");

    string op;
	vector<vector<string>> data;
	char *out = (char *) malloc(MAX_INPUT_SIZE);
	vector<vector<string>> sorted_table;
	vector<vector<string>> conditioned_table;
	vector<string> res;

	// string query = "Select * FROM tabel1 WHERE column2=c2_val2";
	// string query = "SElECT * FROM tabel1 ORDER BY column1";
	// string query = "Take SNAPSHOT table1";
	// string query = "VIEW SNAPSHOT table1";
	Parser tes(query);

	vector<string> query_token = tes.query_token;
	vector<pair<string, string>> tabel_val;

	vector<string> row_val;
	unordered_map<string, int> query_table{
		{"select", 1},
		{"create", 2},
		{"insert", 3},
		{"drop", 4},
		{"delete", 5},
		{"update", 6},
		{"take",7},
		{"view",8}};

	switch (query_table[query_token[0]])
	{
	case 1:
		tes.choose_select();
		// cout << tes.tabelname;
		data = viewTable(tes.tabelname);
		if (tes.clause_set.find("where") != tes.clause_set.end())
		{
			conditionTable(data, res, tes.mat[2][1]);
			conditioned_table.push_back(res);
			vectorToCharPointer(out, conditioned_table);
			break;

		}
		if (tes.clause_set.find("order") != tes.clause_set.end())
		{
			sortColumn(data, sorted_table, tes.mat[2][1]);
			vectorToCharPointer(out, sorted_table);
			break;	
		}
		vectorToCharPointer(out, data);
		break;
	case 2:

		tes.choose_create(tabel_val);
        
        op=addTable(tes.tabelname,tabel_val);
		StringToCharPointer(out, op);

        // TODO: process OP to char * and return
		
		break;
	case 3:
		tes.choose_insert(row_val);
        op=insertInto(tes.tabelname,row_val);
		StringToCharPointer(out,op);
        // TODO: process OP to char * and return
		break;

	case 4:
		tes.choose_drop();
        op=dropTable(tes.tabelname);
		StringToCharPointer(out,op);

		break;
	case 5:
		tes.choose_delete();
		// cout << tes.tabelname << endl;
		tes.print_mat();
		break;
	case 6:
		tes.choose_update();
		// cout << tes.tabelname << endl;
		tes.print_mat();
		break;
	case 7:
		op=takeSnap(tes.query_token[2]);
		StringToCharPointer(out,op);
		break;
	case 8:
		data=showSnap(tes.query_token[2]);
		vectorToCharPointer(out, data);
		break;
	}
	flush("table");
	return out;
}
