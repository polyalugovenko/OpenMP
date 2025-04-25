#pragma once
#include "Huffman.h"
#include <map>


bool compareFiles(const string& file1_name, const string& file2_name);

string size_of_file(const string& code_f1, const string& code_f2);

bool check_counter(const string &code_f, const string &decode_Huff);

string split(int& count, char& symb);
string RLE_code(const string& codeRLE);
string RLE_decode(const string& codeRLE);

void RLE_parallel_code(const string& fin_name, const string& fout_name);
void RLE_parallel_decode(const string& fin_name, const string& fout_name);