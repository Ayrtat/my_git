// ConsoleApplication5.cpp: определяет точку входа для консольного приложения.
//
#include <string>
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

class Matrix {
private:
public:
	vector <vector <int>> M;
	int num_r, num_c;
	Matrix();
	Matrix(int num_rows, int num_cols);
	void Reset(int num_rowr, int num_cols);
	int At(int num_rows, int num_cols) const;
	int& At(int num_rows, int num_cols);
	int GetNumRows() const;
	int GetNumColumns() const;
	inline bool empty() { return M.empty(); }
};

Matrix::Matrix() { num_r = num_c = 0; }
Matrix::Matrix(int num_rows, int num_cols) {
	Reset(num_rows, num_cols);
}

int Matrix::GetNumRows() const {
	return num_r;
}

int Matrix::GetNumColumns() const {
	return num_c;
}


void Matrix::Reset(int num_rows, int num_cols) {
	if (num_rows < 0 || num_cols < 0) throw std::out_of_range("out of range");
	

	num_r = num_rows;
	num_c = num_cols;

	if(num_r == 0 || num_c == 0) return;

	M.clear();
	for(int i = 0; i < num_rows; i++) {
		M.push_back(vector<int> (num_cols,0));	
	}
}

int Matrix::At(int num_rows, int num_cols) const {
	if (num_rows < 0 || num_cols < 0 || num_rows >= this->num_r || num_cols >= this->num_c) 
		throw std::out_of_range("out of range");

	return M[num_rows][num_cols];

}

int& Matrix::At(int num_rows, int num_cols) {
	if (num_rows < 0 || num_cols < 0 || num_rows >= this->num_r || num_cols >= this->num_c || M.empty()) 
		throw std::out_of_range("out of range");

	return M[num_rows][num_cols];
}

istream& operator>> (istream &i, Matrix &m) {
	string put;
	getline(i, put);
	if(put.empty()) return cin;

	istringstream sizes; 
	sizes.str(put);

	string value;
	int quant = 0;

	while(sizes >> value) {
		quant++;
	}

	sizes.clear();
	sizes.str(put);

	if(quant != 2) throw std::invalid_argument ("invalid argument#1");	
	int r = 0, c = 0;
	sizes >> r;
	sizes >> c;

	m.Reset(r, c); 	
	if (r == 0 || c == 0) return cin;
	
	

	int ri, cj; ri = cj = 0;
	int put_value;


	while (getline(i, put)) {
		
		istringstream line(put);
		cj = 0;
		while (line >> put_value) {
			m.At(ri, cj) = put_value;
			cj++;
		}
		ri++;
		if(cj >= c && ri >= r) break;
		put.clear();
	}

	return i;
}

ostream& operator<<(ostream &o, Matrix m) {
	o << m.GetNumRows() << " " << m.GetNumColumns() << "\n";

	for (int i = 0; i < m.GetNumRows(); i++) {
		for (int j = 0; j < m.GetNumColumns(); j++) {
			o << m.At(i, j) << " ";
		}
		o << "\n";
	}
	return o;
}

bool operator== (Matrix a, Matrix b) {
	if(a.empty() && b.empty()) return true;

	if (a.GetNumRows() == b.GetNumRows() && a.GetNumColumns() == b.GetNumColumns()) {
		for (int i = 0; i < a.GetNumRows(); i++) {
			for (int j = 0; j < a.GetNumColumns(); j++) {
				if (a.At(i, j) != b.At(i, j)) return false;
			}
		}
		return true;
	}
	else return false;
}

Matrix operator+ (Matrix a, Matrix b) {
	Matrix result;

	if(a.empty() && b.empty()) return result;

	if (a.GetNumRows() == b.GetNumRows() && a.GetNumColumns() == b.GetNumColumns()) {
		result.Reset(a.GetNumRows(), a.GetNumColumns()); //create new matrix
		for (int i = 0; i < a.GetNumRows(); i++) {
			for (int j = 0; j < a.GetNumColumns(); j++) {
				result.At(i, j) = a.At(i, j) + b.At(i, j);
			}
		}
		return result;
	}
	else { throw std::invalid_argument ("invalid argument"); }
}


int main() {
	Matrix one;
	Matrix two(3,2);

	

	cin >> one; //>> two;
	cout << one << endl;
	return 0;
}
