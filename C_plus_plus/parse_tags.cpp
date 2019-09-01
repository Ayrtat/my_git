// ConsoleApplication5.cpp: определяет точку входа для консольного приложения.
//
#include "stdafx.h"
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>

using namespace std;

#define ERROR cout << "finite machine has gotten wrong state!\n"; exit(1);





string analize(string str) {
	istringstream proc(str);
	string result, temp;
	proc >> temp;
	result += temp + "~";
	while(proc >> temp)
		result += temp;
	return result;
}

string take_out_tag(string str) {
	istringstream proc(str);
	string result;
	proc >> result;
	return result;
}

int main() {
	int N, Q; 
	string first;

	getline(cin, first);
	istringstream in(first);
	in >> N;
	in >> Q;
	
	if (N > 20 || Q > 20) exit(1);

	string input;
	vector <string> q;

	while (N--) {
		string s;
		//cin >> s;
		getline(cin, s);
		if (s.size() > 200) exit(1);
		input += s;
	}

	while (Q--) {
		string s;
		getline(cin, s);
		if (s.size() > 200) exit(1);
		q.push_back(s);
	}

	cout << "\n" << input << "\n";

	char fm_state = '0';
	string current_state;
	string created_node;
	vector <string> nodes;
	bool closed = 0;
	string what_find;
	string::iterator it;
	string::reverse_iterator itt;
	string taken;
	for (auto i : input) {
		
		switch (fm_state) {
		case '0':
			switch (i) {
			case '<':
				fm_state = '<';
				continue;
			default:
				ERROR
			}
		case '<':
			switch (i) {
			case '>':
				current_state.empty() ? nodes.push_back(analize(created_node)) : nodes.push_back(current_state + analize(created_node));
				taken = take_out_tag(created_node);
				current_state += taken + ".";
				created_node.clear();
				fm_state = '>';
				continue;
			case '/':
				current_state.pop_back();
				itt = current_state.rbegin();
				while (*itt != '.') {
					current_state.pop_back(); itt++;
					if (current_state.empty()) break;
				}
				fm_state = '/';
				continue;
			default:
				created_node += i;
				continue;

			}
		case '>':
			switch (i) {
			case '<':
				fm_state = '<';
				continue;
			default:
				ERROR
			}
		case '/':
			switch (i) {
			case '>':
				fm_state = '>';
				continue;
			case '/' || '<':
				ERROR
			default: continue;
			}
		}
	}

	for (auto what_find : q) {
		bool found = 0;
		for (auto where_find : nodes) {
			auto it = search(where_find.begin(), where_find.end(), what_find.begin(), what_find.end());
			if (it != where_find.end()) {
				found = 1; 
				auto iter = find(where_find.begin(), where_find.end(), '=');
				string out(iter+2, where_find.end()-1);
				cout << out << "\n";

			}
		}
		if (!found) cout << "Not Found!\n";
	}
	cin >> N;
}

//system("PAUSE");
