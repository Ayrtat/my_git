// ConsoleApplication5.cpp: определяет точку входа для консольного приложения.
//
#include "stdafx.h"
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <sstream>

using namespace std;


int main()
{
	int N;
	string get_n;
	getline(cin, get_n);
	istringstream get_N(get_n);
	get_N >> N;

	get_N.clear();
	getline(cin, get_n);
	get_N.str(get_n);

	int value;
	vector<int> A;
	while (get_N >> value) {
		A.push_back(value);
	}

	float min = 1000000000;

	for (int i = 0; i < A.size() ; i++) {
		int sum = A[i];
		int n = 1;
		int fixed = i;

		if (i == A.size() - 1) break;
		while (true) {
			if ((fixed + 1) >= A.size()) break;
			if (A[fixed + 1] <= sum / n) {
				sum += A[fixed + 1];
				++n; fixed++;
				continue;
			}
			else break;			
		}
		(min > (float)sum / n) ? min = (float)sum / n : min = min;
		//cout << min << endl;
	}
	cout << min;
	cin >> N;



//	cin >> N;
}

//system("PAUSE");
