#include "Huffman.h"
#include <cmath>
using namespace std;

void generateFile(const  vector<char>& alphabet, const  string& file_name) {
	int rank, size;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	// Вычисляем количество символов для каждого процесса
	int chars_per_process = file_size / size;
	int remainder = file_size % size;

	// Добавляем остаток к первым процессам
	int my_chars = chars_per_process + (rank < remainder ? 1 : 0);

	// Генерируем случайные символы
	vector<char> my_data(my_chars);
	for (int i = 0; i < my_chars; ++i) {
		my_data[i] = alphabet[rand() % alphabet.size()];
	}

	// Определяем смещения для MPI_Gatherv
	vector<int> recvcounts(size, 0);
	vector<int> displs(size, 0);
	for (int i = 0; i < size; ++i) {
		recvcounts[i] = chars_per_process + (i < remainder ? 1 : 0);
		if (i > 0) {
			displs[i] = displs[i - 1] + recvcounts[i - 1];
		}
	}

	// Собираем данные от всех процессов в процессе с рангом 0
	vector<char> all_data(file_size);
	MPI_Gatherv(my_data.data(), my_chars, MPI_CHAR,
		all_data.data(), recvcounts.data(), displs.data(),
		MPI_CHAR, 0, MPI_COMM_WORLD);

	// Процесс с рангом 0 записывает данные в файл
	if (rank == 0) {
		ofstream outfile(file_name);
		outfile.write(all_data.data(), file_size);
		outfile.close();
	}

}

void create_table(Node* root, size_t& n, map<char, vector<string>>& table, vector<string>& code)
{
	if (root->left != NULL)
	{
		code.push_back("0");
		create_table(root->left, n, table, code);
	}

	if (root->right != NULL)
	{
		code.push_back("1");
		create_table(root->right, n, table, code);
	}
	if (root->left == NULL && root->right == NULL)
	{
		if (code.size() == 0) { code.push_back("0"); }
		table[root->symb] = code;

	}

	if (table.size() == n)
	{
		return;
	};
	code.pop_back();
}

// считаем вероятности появления символов
map<char, int> weights_generate(string& s)
{
	map<char, int> weights;
	for (int i = 0; i < s.size(); i++)
	{
		weights[s[i]]++;
	}
	return weights;
}

//создаем узлы для каждого символа
list <Node*> begin_nodes(map<char, int>& weights)
{
	list <Node*> nodes;
	for (auto it = weights.begin(); it != weights.end(); it++)
	{
		Node* node = new Node();
		node->symb = it->first;
		node->weight = it->second;
		nodes.push_back(node);
	}
	return nodes;
}

//строим дерево
Node* create_tree(list <Node*>& nodes)
{
	Node* root;
	while (nodes.size() != 1)
	{
		nodes.sort([](const Node* l, const Node* r) { return l->weight < r->weight; });

		Node* left_s = nodes.front();
		nodes.pop_front();
		Node* right_s = nodes.front();
		nodes.pop_front();

		Node* parent = new Node(left_s, right_s);
		nodes.push_back(parent);
	}
	root = nodes.front();
	return root;
}



string Huffman_encoder(string& text) {
	string result = "";
	map<char, vector<string>> table;
	size_t n = 0;
	vector<string> code;

	map<char, int> weights = weights_generate(text);
	n = weights.size();

	list<Node*> nodes = begin_nodes(weights);

	Node* root = create_tree(nodes);

	create_table(root, n, table, code);
	for (int i = 0; i < text.size(); i++) {
		vector<string> v = table[text[i]];
		for (int j = 0; j < v.size(); j++) {
			result += v[j];
		}
	}

	return result;
}


void Huffman_parallel_encoder(const  string& fin_name, const  string& fout_name) {
	int rank, size;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	 string part;
	 string coded_part;

	if (rank == 0) {
		vector< string> parts = split_file(fin_name, size);

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

	coded_part = Huffman_encoder(part);

	if (rank == 0) {
		 vector< string> coded_parts(size);
		coded_parts[0] = coded_part;
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

void Huffman_parallel_decoder(const  string& fin_name, const  string& fout_name, const string& origiinal) {
	int rank, size;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	 string part;
	string original_part;
	 string decoded_part;

	if (rank == 0) {
		 vector< string> parts = split_file_decoder(fin_name);
		vector<string> original_parts = split_file(origiinal, size);
		for (int i = 1; i < size; i++) {
			MPI_Send(parts[i].data(), parts[i].size(), MPI_CHAR, i, 0, MPI_COMM_WORLD);
		}
		for (int i = 1; i < size; i++) {
			MPI_Send(original_parts[i].data(), original_parts[i].size(), MPI_CHAR, i, 0, MPI_COMM_WORLD);
		}
		part = parts[0];  
		original_part = original_parts[0];
	}
	else {
		MPI_Status status;
		int count;

		MPI_Probe(0, 0, MPI_COMM_WORLD, &status);
		MPI_Get_count(&status, MPI_CHAR, &count);
		part.resize(count);
		MPI_Recv(&part[0], count, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		MPI_Probe(0, 0, MPI_COMM_WORLD, &status);
		MPI_Get_count(&status, MPI_CHAR, &count);
		original_part.resize(count);
		MPI_Recv(&original_part[0], count, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	}

	decoded_part = Huffman_decoder(original_part, part);

	if (rank == 0) {
		 vector< string> decoded_parts(size);
		decoded_parts[0] =  move(decoded_part);
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

string Huffman_decoder(string& text_old, string& text_new) {
	map<char, int> weights = weights_generate(text_old);
	list<Node*> nodes = begin_nodes(weights);
	Node* root = create_tree(nodes);
	Node* pointer = root;
	string local = "";

	if (root->left == NULL && root->right == NULL) {
		for (int i = 0; i < text_new.size(); i++) {
			local += text_old[i];
		}
	}
	else {
		for (int i = 0; i < text_new.size(); i++) {
			char one_s = text_new[i];
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
	}
	return local;
}


vector<string> split_file(string file_name, int size) {

	ifstream myfile(file_name);
	ostringstream ss;
	ss << myfile.rdbuf();
	string file_contents = ss.str();
	myfile.close();

	int size_of_file = file_contents.size();
	int size_of_part = size_of_file / size;
	vector<string> parts;
	for (int i = 0; i < size; i++) {
		int start = i * size_of_part;
		int finish = (i == size - 1) ? file_contents.size() : (start + size_of_part);
		parts.push_back(file_contents.substr(start, finish - start));
	}

	return parts;
}


 vector< string> split_file_decoder(const  string& filename) {
	 ifstream file(filename);
	 string line;
	string fileContent = "";
	 vector<int> numbers;

	if (getline(file, line)) {
		 istringstream iss(line);
		int number;
		while (iss >> number) {
			numbers.push_back(number);
		}
	}
	
	while (getline(file, line)) {
		fileContent += line + "\n";
	}
	file.close();
	 vector< string> result(numbers.size());
	int current_pos = 0; 
	for (int i = 0; i < result.size(); i++) {
		result[i] = fileContent.substr(current_pos, numbers[i]);
		current_pos += numbers[i];
	}

	return result;
}