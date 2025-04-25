#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <map>
#include <list>
#include <algorithm>
#include "Node.h"
#include <mpi.h>
#include <sstream>

using namespace std;
const unsigned short file_size = 10000;

void generateFile(const vector<char>& alphabet, const  string& file_name);
void create_table(Node* root, size_t& n, map<char, vector<string>>& table, vector<string>& code);
map<char, int> weights_generate(string& s);
list <Node*> begin_nodes(map<char, int>& weights);
Node* create_tree(list <Node*>& nodes);

void Huffman_parallel_encoder(const std::string& fin_name, const std::string& fout_name);
void Huffman_parallel_decoder(const std::string& fin_name, const std::string& fout_name, const string& origiinal);
string Huffman_decoder(string& text_old, string& text_new);
string Huffman_encoder(string& text);


vector<string> split_file(string file_name, int size);
std::vector<std::string> split_file_decoder(const std::string& filename);
