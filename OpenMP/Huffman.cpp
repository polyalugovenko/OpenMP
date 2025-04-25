#include <vector>
#include <map>
#include <list>
#include <string>
#include"Node.h"
#include "Huffman.h"


Huffman::Huffman()
{
    counter = 0;
    price = 0;
}
void Huffman::create_table(Node* root, const size_t &n)
{
    if (root->left != NULL)
    {
        code.push_back(0);
        create_table(root->left,n);
    }
   
    if (root->right != NULL)
    {
        code.push_back(1);
        create_table(root->right,n);
    }
    if (root->left == NULL && root->right == NULL)
    {
        table[root->symb] = code;
    }

    if (table.size() == n)
    {
        return;
    };
    code.pop_back();
}

// считаем вероятности появления символов
void Huffman::weights_generate(string &s)
{
#pragma omp parallel
    {
#pragma omp for schedule(static)
        for (int i = 0; i < s.size(); i++)
        {
#pragma omp atomic
            weights[s[i]]++;
        }
    }
}

//создаем узлы для каждого символа
void Huffman::begin_nodes()
{
    for (auto it = weights.begin(); it != weights.end(); it++)
    {
        Node* node = new Node();
        node->symb = it->first;
        node->weight = it->second;
        nodes.push_back(node);
    }
}

//строим дерево
void Huffman::create_tree()
{
    while (nodes.size() != 1)
    {
        nodes.sort([](const Node* l, const Node* r){ return l->weight < r->weight;});

        Node* left_s = nodes.front();
        nodes.pop_front();
        Node* right_s = nodes.front();
        nodes.pop_front();

        Node* parent = new Node(left_s, right_s);
        nodes.push_back(parent);
    }
    root = nodes.front();
}

vector<string> Huffman::coding_process(vector<string> &parts, string f_name)
{
    vector<string> code_file(omp_get_max_threads());
#pragma omp parallel
    {
#pragma omp for
    for (int k = 0; k < omp_get_max_threads(); k++) {
        string local = "";
        for (int i = 0; i < parts[k].size(); i++) {
            vector<bool> v = table[parts[k][i]];
            for (int j = 0; j < v.size(); j++) {
                local += to_string(v[j]);
            }
        }
        code_file[k] = local;
    }
    }

    string code_h = "";
    for (int i = 0; i < code_file.size(); i++) {
        code_h += code_file[i];
    }
    ofstream co_file(f_name);
    co_file << code_h;
    co_file.close();
    return code_file;
}

string Huffman::decoding_process(vector<string>& code, string f_name)
{
    vector<string> decode_file(omp_get_max_threads());
#pragma omp parallel
    {
        Node* pointer = root;
#pragma omp for
        for (int k = 0; k < omp_get_max_threads(); k++) {
            string local = "";
            for (int i = 0; i < code[k].size(); i++) {
                char one_s = code[k][i];
                if (one_s == '1')
                {
                    pointer = pointer->right;
                }
                else if (one_s == '0')
                {
                    pointer = pointer->left;
                }
                if (pointer->left == NULL && pointer->right == NULL)
                {
                    local += pointer->symb;
                    pointer = root;
                }
            }
            decode_file[k] = local;
           // cout << "thread_num decode_0: " << omp_get_thread_num() << endl;
        }
    }

    string decode = "";
    for (int i = 0; i < decode_file.size(); i++) {
        decode += decode_file[i];
    }
    ofstream de_file(f_name);
    de_file << decode;
    de_file.close();
    return decode;
}


void Huffman::price_of_coding(const int& count_of_symb) {
    double local_price = 0.0;
#pragma omp parallel for reduction(+:local_price) 
    for (size_t i = 0; i < weights.size(); ++i) {
        auto it = next(weights.begin(), i);
        double probability = (double)it->second / (double)count_of_symb;
        local_price += table[it->first].size() * probability;
    }

    price += local_price;
    cout << "\nPrice_of_coding = " << price << endl<<endl;
}



void Huffman::check_counter(string code_f, string decode_Huff)
{
    ifstream code_file(code_f, ios::in | ios::binary); //исходный файл для чтения
    ifstream huff_code_file(decode_Huff, ios::in | ios::binary);//декодированный файл для чтения
    while (!code_file.eof()) {
        if (code_file.get() != huff_code_file.get()) {
            cout << "Huff: Decoding is wrong!\n" << endl;
            code_file.close();
            huff_code_file.close();
            return;
        }
    }
    cout << "\nHuff: The decoding was successful!" << endl;
    code_file.close();
    huff_code_file.close();
}


void Huffman::Huffman_code(const size_t& n, string& full_str, string code_name, string decode_name, vector<string> parts, int& count_of_symb)
{
    weights_generate(full_str);
    begin_nodes();
    create_tree();
    create_table(root, n);
    vector<string> code_part = coding_process(parts, code_name);
    string decode_str = decoding_process(code_part, decode_name);
    check_counter("myfile.txt", decode_name);
    price_of_coding(count_of_symb);
    code.clear();
    table.clear();
    weights.clear();
    nodes.clear();
}

vector <string> Huffman::Huffman_(const size_t& n, string& full_str, string code_name, string decode_name, vector<string> parts, bool flag, string decode_check)
{
    vector<string> code_part;
    weights_generate(full_str);
    int num = weights.size();
    begin_nodes();
    create_tree();
    if (flag) {
        create_table(root, num);
        vector<string> code_part = coding_process(parts, code_name);
        string decode_str = decoding_process(code_part, decode_name);
        check_counter(decode_check, decode_name);
    }
    else {
        create_table(root, num);
        code_part = coding_process(parts, code_name);
    }
    code.clear();
    table.clear();
    weights.clear();
    nodes.clear();
    return code_part;
}

