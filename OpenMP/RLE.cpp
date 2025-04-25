#include "RLE.h"
#include <iostream>
RLE::RLE()
{
	count = 1;
}

void RLE:: split(int& count, ofstream& in_code, char& symb)
{
	if (count / 259 != 0)
	{
		int n = count / 259;
		int ost = count % 259 - 4;
		for (int i = 0; i < n; i++)
		{
			in_code << '#' << (char)(255) << symb;
		}
		in_code << '#' << (char)ost << symb;
	}
	else
	{
		in_code << '#' << (char)(count - 4) << symb;
	}
}

void RLE::RLE_code(string code_f, string codeRLE, const int& num)
{
	ifstream code_file(code_f, ios::in | ios::binary); //исходный файл для чтения
	ofstream RLE_code_file_w(codeRLE, ios::out | ios::binary);//закодированный файл для записи
	int k = 1;
	first = code_file.get();
	while (second != EOF)
	{
		second = code_file.get();
		while (second == first)
		{
			count++;
			second = code_file.get();
		}
		if (count >= 4)
		{
			split(count, RLE_code_file_w, first);
			k += count;
		}
		else
		{
			code_file.clear();
			code_file.seekg(k);
			second = code_file.get();
			if (second == EOF && num == k - 1)
				break;
			RLE_code_file_w << first;
			k++;
		}
		if (second != EOF && num != k - 1)
			first = second;
		count = 1;
	}
	RLE_code_file_w.close();
	code_file.close();
}

void RLE::RLE_decode(string codeRLE, string decodeRLE, const int& numm)
{
	ifstream RLE_code_file_r(codeRLE, ios::in | ios::binary);//закодированный файл для чтения
	ofstream RLE_decode_file_w(decodeRLE, ios::out | ios::binary);//декодированный файл для записи
	int k = 0;
	    while (!RLE_code_file_r.eof())
		{
		        char c = RLE_code_file_r.get();
		        if (c == '#')
		        {
		            int num = (int)RLE_code_file_r.get()+4;
		            char symb = RLE_code_file_r.get();
		            for (int i = 0; i < num; i++)
		            {
						RLE_decode_file_w << symb;
		            }
					k += num;
		        }
		        else
		        {
		            if (c == EOF)
		            {
		                break;
		            }
					RLE_decode_file_w << c;
					k++;
		        }
	    }
		RLE_decode_file_w.close();
		RLE_code_file_r.close();
}


bool RLE::Check(string code_f, string decodeRLE)
{
	ifstream code_file(code_f, ios::in | ios::binary); //исходный файл для чтения
	ifstream RLE_decode_file_r(decodeRLE, ios::in | ios::binary);//декодированный файл для чтения
	while (!code_file.eof()) {
		if (code_file.get() != RLE_decode_file_r.get()) {
			cout << "\nRLE: Decoding is wrong!" << endl;
			code_file.close();
			RLE_decode_file_r.close();
			return false;
		}
	}
	cout << "\nRLE: The decoding was successful!" << endl;
	code_file.close();
	RLE_decode_file_r.close();
	return true;
}

void RLE::RLE_process(string code_f, string codeRLE, string decodeRLE, const int& count_of_symb)
{
	RLE_code(code_f, codeRLE);
	RLE_decode(codeRLE, decodeRLE);
	Check(code_f, decodeRLE);
	index.clear();
	lengh.clear();
}


bool RLE::RLE_(string code_f, string codeRLE, string decodeRLE, bool flag, const int& num)
{
	bool res = true;
	RLE_code(code_f, codeRLE, num);
	if (flag) {
		RLE_decode(codeRLE, decodeRLE);
		res = Check(code_f, decodeRLE);
	}
	index.clear();
	lengh.clear();
	return res;
}
