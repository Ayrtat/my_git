//https://www.hackerrank.com/challenges/deque-stl/problem
#include <iostream>
#include <deque>
#include <string>
#include <sstream>

using namespace std;


int main() {
	string input;
	getline(cin, input);
	istringstream Ts(input);
	int T;
	Ts >> T;

	while(T--) {
		getline(cin, input);
		istringstream N_K(input);
	
		int N, K, N_f;

		N_K >> N; N_K >> K;


		N_f = N;
		getline(cin, input);
		N_K.clear();
		N_K.str(input);
		
		deque<int> in;

		while(N--) { 
			int value;
			N_K >> value;
			in.push_back(value);
		}

		if(in.size() % 2 == 0) in.push_back(0);

		//--K;
		--K;
		while(K--) {
			deque<int> temp;

			for(auto it = in.begin(); it != in.end()-1; it++) {
				if(*it <= *(it+1)) temp.push_back(*(it+1));
				else temp.push_back(*it);
			}
			in.clear();

			for(auto i : temp) { in.push_back(i); }

		}

		for(auto i : in) cout << i << " "; 
		cout << endl;
		
	}
}
