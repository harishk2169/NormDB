#pragma once
#include <bits/stdc++.h>
#include <fcntl.h>
#include <unistd.h>

using namespace std;

string addTable(string tableName, vector<pair<string, string>> &cols);
string dropTable(string name);
string insertInto(string name, vector<string> data);
vector<vector<string>> viewTable(string name);
string takeSnap(string name);
vector<vector<string>> showSnap(string name);
void load(string fileName);
void flush(string fileName);