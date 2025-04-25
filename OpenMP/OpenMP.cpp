#include <vector>
#include <map>
#include <list>
#include <string>
#include <sstream>
#include "Node.h"
#include "Huffman.h"
#include "RLE.h"
#include <omp.h>
using namespace std;

char alphabet[] = { 'a','v','y','k','s','t','e','b','d','l','p','o' };
const size_t size_of_al = size(alphabet);
int num = 10000;

string generate_file() {
    ofstream myfile("myfile.txt");
    int alphabet_size = size_of_al;
    string str = "";
#pragma omp parallel 
    {
        string local_str = "";
#pragma omp for
        for (int i = 0; i < num; i++) {
            local_str += alphabet[rand() % alphabet_size];
        }
#pragma omp critical
        str += local_str;
    }
    myfile << str;
    myfile.close();
    return str;
}

vector<string> split_file(string file_name, int num_parts) {

    ifstream myfile(file_name);
    ostringstream ss;
    ss << myfile.rdbuf();
    string file_contents = ss.str();
    myfile.close();

    int size_of_file = file_contents.size();
    int size_of_part = size_of_file / num_parts;
    vector<string> parts;
    for (int i = 0; i < num_parts; i++) {
        int start = i * size_of_part;
        int finish = (i == num_parts - 1) ? file_contents.size() : (start + size_of_part);
        parts.push_back(file_contents.substr(start, finish - start));
    }

    return parts;
}

int size_of_file(string code_f1, bool t, string& result, string code_f2 = " ", bool l = true)
{
    int size_decode = 0;
    ifstream in_f(code_f1, ios::ate | ios::binary);
    int size_code = in_f.tellg();
    in_f.close();
    ifstream in_h(code_f2, ios::ate | ios::binary);
    if (l)
        size_decode = in_h.tellg();
    else
        size_decode = in_h.tellg() / 8;
    in_h.close();
    if (t)
    {
        return size_code;
    }
    else {
        result = result + "Compression coefficient = " + to_string(((double)size_decode / (double)size_code) * 100) + "%";
        cout << result << endl;
        result = "";
        return 0;
    }
}

int main()
{
    srand(time(0));

    string file_str = generate_file();
    vector<string> file_parts = split_file("myfile.txt", omp_get_max_threads());
    string result = "";
    Huffman huff;
    huff.Huffman_code(size_of_al, file_str, "codeHuf.txt", "decodeHuff.txt", file_parts, num);
    result = "Huffman: ";
    size_of_file("myfile.txt", false, result, "codeHuf.txt", false);
    cout << endl;
    RLE rle;
    rle.RLE_process("myfile.txt", "rle.txt", "rle_decode.txt", num);
    result = "\nRLE: ";
    size_of_file("myfile.txt", false, result, "rle.txt", false);

    cout << "\n\nPART 2\n" << endl << endl;


    cout << "Huffman -> RLE:" << endl;
    Huffman one;
    vector<string> part_of_code_Huf = one.Huffman_(size_of_al, file_str, "Huffman_first.txt", "", file_parts, false);
    RLE two;
    int a = size_of_file("Huffman_first.txt", true, result);
    if (two.RLE_("Huffman_first.txt", "RLE_second.txt", "RLE_second_decode.txt", true, a)) {
        one.decoding_process(part_of_code_Huf, "Huff_first_decode.txt");
        one.check_counter("myfile.txt", "Huff_first_decode.txt");
    }
    result = "\nHuffman->RLE:";
    size_of_file("myfile.txt", false, result, "RLE_second.txt", false);

    cout << "\n\nRLE -> Huffman:" << endl;
    RLE one_2;
    one_2.RLE_("myfile.txt", "RLE_first.txt", "", false);
    Huffman two_2;
    vector<string>RLE_split = split_file("RLE_first.txt", omp_get_max_threads());
    string s_RLE = "";
    for (int i = 0; i < RLE_split.size(); i++) {
        s_RLE += RLE_split[i];
    }
    two_2.Huffman_(size_of_al, s_RLE, "Huffman_second.txt", "Huffman_desecond.txt", RLE_split, true, "RLE_first.txt");
    one_2.RLE_decode("Huffman_desecond.txt", "RLE_first_decode.txt");
    one_2.Check("myfile.txt", "RLE_first_decode.txt");
    result = "\nRLE -> Huffman:";
    size_of_file("myfile.txt", false, result, "Huffman_second.txt", false);
}
