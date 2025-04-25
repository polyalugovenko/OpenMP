#include <iostream>
#include "Huffman.h"
#include "RLE.h"

using namespace std;


int main(int argc, char** argv)
{
    MPI_Init(&argc, &argv);
    setlocale(LC_ALL, "Russian");
    srand(time(0));

    vector<char> alphabet = { 'a', 'b', 'c', 'p', 'o', 'k', '1', '2', '4' };

    generateFile(alphabet, "myfile.txt");


    Huffman_parallel_encoder("myfile.txt", "Huffman_code.txt");
    Huffman_parallel_decoder("Huffman_code.txt", "Huffman_decode.txt", "myfile.txt");

    RLE_parallel_code("myfile.txt", "RLE_code.txt");
    RLE_parallel_decode("RLE_code.txt", "RLE_decode.txt");

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    bool check1 = check_counter("myfile.txt", "Huffman_decode.txt");
    if (rank == 0) {
        string res1 = "\nHuff: decoding was ";
        check1 ? res1 += "successful\n" : res1 += "wrong\n";
        cout << res1 << endl;
    }

    bool check2 = check_counter("myfile.txt", "RLE_decode.txt");
    if (rank == 0) {
        string res2 = "RLE: decoding was ";
        check2 ? res2 += "successful\n" : res2 += "wrong\n";
        cout << res2 << endl;
    }
   
    if (rank == 0) {
        cout << "\nHuffman compression coefficient = " << size_of_file("myfile.txt", "Huffman_code.txt") << endl;
        cout << "\nRLE compression coefficient = " << size_of_file("myfile.txt", "RLE_code.txt") << endl;
    }

    if (rank == 0) { cout << "\n\nPART 2\n\n\n"; }
    Huffman_parallel_encoder("myfile.txt", "Huffman_code_first.txt");
    RLE_parallel_code("Huffman_code_first.txt", "RLE_code_second.txt");
    RLE_parallel_decode("RLE_code_second.txt", "RLE_decode_second.txt");
    Huffman_parallel_decoder("RLE_decode_second.txt", "Huffman_decode_first.txt", "myfile.txt");



    RLE_parallel_code("myfile.txt", "RLE_code_first.txt");
    Huffman_parallel_encoder("RLE_code_first.txt", "Huffman_code_second.txt");
    Huffman_parallel_decoder("Huffman_code_second.txt", "Huffman_decode_second.txt", "RLE_code_first.txt");
    RLE_parallel_decode("Huffman_decode_second.txt", "RLE_decode_first.txt");

    bool check3 = check_counter("myfile.txt", "RLE_decode_first.txt");
    if (rank == 0) {
        string res1 = "RLE -> Huffman: decoding was ";
        check3 ? res1 += "successful\n" : res1 += "wrong\n";
        cout << res1 << endl;
    }

    bool check4 = check_counter("myfile.txt", "Huffman_decode_first.txt");
    if (rank == 0) {
        string res2 = "Huffman -> RLE: decoding was ";
        check4 ? res2 += "successful\n" : res2 += "wrong\n";
        cout << res2 << endl;
    }
    if (rank == 0) {
        cout << "\nHuffman->RLE compression coefficient = " << size_of_file("myfile.txt", "RLE_code_second.txt") << endl;
        cout << "\nRLE->Huffman compression coefficient = " << size_of_file("myfile.txt", "Huffman_code_second.txt") << endl<<endl;
    }

    MPI_Finalize();
}

