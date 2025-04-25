#pragma once
#include <omp.h>
class Huffman
{
	vector<bool> code;// код символа
	map<char, vector<bool> > table; //символ - код
	map<char, int> weights; //символ - частота
	list<Node*> nodes; // список узлов
	Node* root;
	int counter;
	double price;
	string bin;
public:
	Huffman();
	void create_table(Node* root, const size_t &n);
	void weights_generate(string &s);
	void begin_nodes();
	void create_tree();
	vector<string> coding_process(vector<string>& parts, string f_name);
	void price_of_coding(const int& count_of_symb);
	string decoding_process(vector<string>& code, string f_name);
	void Huffman_code(const size_t& n, string& full_str, string code_name, string decode_name, vector<string> parts, int &count_of_symb);
	void check_counter(string code_f, string decode_Huff);
	vector<string> Huffman_(const size_t& n, string& full_str, string code_name, string decode_name, vector<string> parts, bool flag, string decode_check="");
};

