#include "RLE.h"
#include <cmath>

using namespace std;
string split(int& count, char& symb)
{
	string one_code = "";
	if (count / 259 != 0)
	{
		int n = count / 259;
		int ost = count % 259 - 4;
		for (int i = 0; i < n; i++)
		{
			one_code = one_code + '#' + (char)(255) + symb;
		}
		one_code = one_code + '#' + (char)ost + symb;
	}
	else
	{
		one_code = one_code + '#' + (char)(count - 4) + symb;
	}
	return one_code;
}

string RLE_code(const string& codeRLE)
{
	istringstream code_file(codeRLE); //исходный файл для чтения
	ostringstream RLE_code_string_w;//закодированный файл для записи
	int count = 1;
	int k = 1;
	char first = code_file.get();
	while (!code_file.eof())
	{
		char second = code_file.get();
		while (second == first)
		{
			count++;
			second = code_file.get();
		}
		if (count >= 4)
		{
			RLE_code_string_w << split(count, first);
			k += count;
		}
		else
		{
			code_file.clear();
			code_file.seekg(k);
			second = code_file.get();
			if (second == EOF && k == codeRLE.size() - 1)
				break;
			RLE_code_string_w << first;
			k++;
		}
		if (second != EOF)
			first = second;
		count = 1;
	}
	return RLE_code_string_w.str();
}

string RLE_decode(const string& codeRLE)
{
	istringstream RLE_code_string_r(codeRLE);//закодированный файл для чтения
	ostringstream RLE_decode_string_w;//декодированный файл для записи
	int k = 0;
	while (!RLE_code_string_r.eof())
	{
		char c = RLE_code_string_r.get();
		if (c == '#')
		{
			int num = (int)RLE_code_string_r.get() + 4;
			char symb = RLE_code_string_r.get();
			for (int i = 0; i < num; i++)
			{
				RLE_decode_string_w << symb;
			}
			k += num;
		}
		else
		{
			if (c == EOF)
			{
				break;
			}
			RLE_decode_string_w << c;
			k++;
		}
	}
	return RLE_decode_string_w.str();
}



void RLE_parallel_code(const  string& fin_name, const  string& fout_name) {
	int rank, size;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	 vector< string> parts;
	 string part;
	 string coded_part;

	if (rank == 0) {
		parts = split_file(fin_name, size);
		for (int i = 0; i < parts.size(); i++) {
		}
		for (int i = 1; i < size; i++) {
			MPI_Send(parts[i].data(), parts[i].size(), MPI_CHAR, i, 0, MPI_COMM_WORLD);
		}
		part = parts[0];
	}
	else {
		MPI_Status status;
		MPI_Probe(0, 0, MPI_COMM_WORLD, &status);
		int count;
		MPI_Get_count(&status, MPI_CHAR, &count);
		part.resize(count);
		MPI_Recv(&part[0], count, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	}

	coded_part = RLE_code(part);

	if (rank == 0) {
		 vector< string> coded_parts(size);
		coded_parts[0] =  coded_part;
		for (int i = 1; i < size; i++) {
			MPI_Status status;
			MPI_Probe(i, 0, MPI_COMM_WORLD, &status);
			int count;
			MPI_Get_count(&status, MPI_CHAR, &count);
			coded_parts[i].resize(count);
			MPI_Recv(&coded_parts[i][0], count, MPI_CHAR, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}

		 ofstream fout(fout_name);
		for (const auto& cp : coded_parts) {
			fout << cp.size() << ' ';
		}
		fout << '\n';
		for (const auto& cp : coded_parts) {
			fout << cp;
		}
		fout.close();
	}
	else {
		MPI_Send(coded_part.data(), coded_part.size(), MPI_CHAR, 0, 0, MPI_COMM_WORLD);
	}
}

void RLE_parallel_decode(const  string& fin_name, const  string& fout_name) {

	int rank, size;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	 vector< string> parts;
	 string part;
	 string decoded_part;

	if (rank == 0) {
		 vector< string> parts = split_file_decoder(fin_name);
		for (int i = 1; i < size; i++) {
			MPI_Send(parts[i].data(), parts[i].size(), MPI_CHAR, i, 0, MPI_COMM_WORLD);
		}
		part = parts[0]; 
	}
	else {
		MPI_Status status;
		MPI_Probe(0, 0, MPI_COMM_WORLD, &status);
		int count;
		MPI_Get_count(&status, MPI_CHAR, &count);
		part.resize(count);
		MPI_Recv(&part[0], count, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	}

	decoded_part = RLE_decode(part);

	if (rank == 0) {
		 vector< string> decoded_parts(size);
		decoded_parts[0] =  decoded_part;
		for (int i = 1; i < size; i++) {
			MPI_Status status;
			MPI_Probe(i, 0, MPI_COMM_WORLD, &status);
			int count;
			MPI_Get_count(&status, MPI_CHAR, &count);
			decoded_parts[i].resize(count);
			MPI_Recv(&decoded_parts[i][0], count, MPI_CHAR, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}

		 ofstream fout(fout_name,  ios::binary);
		for (const auto& dp : decoded_parts) {
			fout << dp;
		}
		fout.close();
	}
	else {
		MPI_Send(decoded_part.data(), decoded_part.size(), MPI_CHAR, 0, 0, MPI_COMM_WORLD);
	}
}

///////////////////////////////////


bool compareFiles(const  string& file1_name, const  string& file2_name) {
	 ifstream file1(file1_name);
	 ifstream file2(file2_name);

	const int buff_size = 500;
	char buff[buff_size];
	while (!file1.eof() and !file2.eof()) {
		file1.get(buff, buff_size);
		 string line1(buff);

		file2.get(buff, buff_size);
		 string line2(buff);

		if (line1 != line2) {
			return false;
		}
	}

	return !(file1.eof() != file2.eof());
}

bool check_counter(const string& code_f, const string& decode_Huff) {
	int rank, size;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	string part_orig;
	string part_decode;

	if (rank == 0) {
		vector< string> parts = split_file(code_f, size);
		vector<string> original_parts = split_file(decode_Huff, size);
		for (int i = 1; i < size; i++) {
			MPI_Send(parts[i].data(), parts[i].size(), MPI_CHAR, i, 0, MPI_COMM_WORLD);
		}
		for (int i = 1; i < size; i++) {
			MPI_Send(original_parts[i].data(), original_parts[i].size(), MPI_CHAR, i, 0, MPI_COMM_WORLD);
		}
		part_orig = parts[0];
		part_decode = original_parts[0];
	}
	else {
		MPI_Status status;
		int count;

		// Получение размера строки
		MPI_Probe(0, 0, MPI_COMM_WORLD, &status);
		MPI_Get_count(&status, MPI_CHAR, &count);
		part_orig.resize(count);
		MPI_Recv(part_orig.data(), count, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		MPI_Probe(0, 0, MPI_COMM_WORLD, &status);
		MPI_Get_count(&status, MPI_CHAR, &count);
		part_decode.resize(count);
		MPI_Recv(part_decode.data(), count, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	}

	// Проверка на равенство частей строк
	bool local_check = part_orig == part_decode;
	// Сбор результатов проверок на процессе 0
	bool global_check;
	MPI_Reduce(&local_check, &global_check, 1, MPI_C_BOOL, MPI_LAND, 0, MPI_COMM_WORLD);

	if (rank == 0) {
		return global_check;
	}
	else return true;
	// Возвращаем результат только на процессе 0
}



string size_of_file(const string &code_f1, const string &code_f2)
{
	string result;
	int size_decode = 0;
	ifstream in_f(code_f1, ios::ate | ios::binary);
	int size_code = in_f.tellg();
	in_f.close();
	ifstream in_h(code_f2, ios::ate | ios::binary);
	size_decode = in_h.tellg() / 8;
	in_h.close();
	result = to_string(((double)size_decode / (double)size_code) * 100) + "%";
	return result;
}