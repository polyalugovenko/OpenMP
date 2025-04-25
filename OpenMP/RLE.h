#pragma once
#include <string>
#include <fstream>
#include <map>
#include<vector>
#include <omp.h>
using namespace std;
class RLE
{
	int count;
	char first;
	char second;
	map<char, int> index;
	vector<int> lengh;
public:
	RLE();
	void split(int& count, ofstream& in_code, char& symb);
	void RLE_code(string code_f, string codeRLE, const int& num=10000);
	void RLE_decode(string codeRLE, string decodeRLE, const int& num=10000);
	bool Check(string code, string decodeRLE);
	void RLE_process(string code_f, string codeRLE, string decodeRLE, const int& count_of_symb);
	bool RLE_(string code_f, string codeRLE, string decodeRLE, bool flag, const int& num=10000);
};

