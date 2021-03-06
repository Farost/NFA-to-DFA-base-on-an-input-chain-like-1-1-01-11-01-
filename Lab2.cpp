// Цепочки вида Z=1^+1*01(11+01)^
// ^ - положительная итерация (от 1 до inf)
// * - итерация от 0 до inf
// 1*01((0*1+00)(1+0)^((1+0))*)

#include "stdafx.h"
#include <iostream>
#include <string>
#include <vector>
#include <map>

static std::string alphabet[72] = { "S", "A", "B", "C", "D", "E", "F", "H", "G", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "T", "U", "V", "W", "X", "Y", "Z",
									"s", "a", "b", "c", "d", "e", "f", "h", "g", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "t", "u", "v", "w", "x", "y", "z" };

class FSMChain {
private:
	std::vector<std::string> letts;
	std::vector<std::string> letters;
	std::vector<std::string> states;
	unsigned letuk = 0;
	unsigned numalph = 0;
	std::vector<std::vector<std::string>> table;
	void PushBackTable();
	int FindLetIndex(std::string str);
	void DeleteExcess();
	void MergeExcess();
	int MakeNFSMPrivate();
	void AppendTableCell(unsigned i1, unsigned j1, std::string str);
	void OptimizeDFSMStep1();
	void OptimizeDFSMStep2();
	void OptimizeDFSMStep3();
	void OptimizeDFSMStep4();
	void OptimizeDFSMStep5();
	unsigned CheckE();
	unsigned FindIndexTable(char ch);
	
	
public:
	FSMChain();
	FSMChain(std::string string);
	//FSMChain(FSMChain *fsmchain);
	~FSMChain() { letts.clear();  }
	std::string chain;
	

	void PrintLetts();
	int CheckChain();
	int OptimizeChain();
	int AnalyzeChain();
	int CreateTable();
	void CreateLetters();
	void MakeNFSM();
	void MakeDFSM();
	void PrintTable();
	void PrintLetters();
	void PrintStates();
	
};

FSMChain::FSMChain() {
	chain = "";
}

FSMChain::FSMChain(std::string string) {
	chain = string;
}


void FSMChain::AppendTableCell(unsigned i1, unsigned j1, std::string str) {
	
	if (table[i1][j1] == "_")
		table[i1][j1] = str;
	else if (str != "_") {
		table[i1][j1].append(str);
		for (unsigned i = 0; i < table[i1][j1].size() - 1; ++i)
			for (unsigned j = i + 1; j < table[i1][j1].size(); ++j)
				if (table[i1][j1][i] == table[i1][j1][j]) 
					table[i1][j1].erase(table[i1][j1].begin() + j--);
	}
		
}


void FSMChain::PrintLetts() {
	if (!letts.size()) std::cout << "Letts are empty." << std::endl;
	else for (unsigned i = 0; i < letts.size(); ++i) std::cout << letts[i] << std::endl;
}

int FSMChain::CheckChain() {
	if (!chain.length()) return 0;
	int bopen = 0, bclose = 0;
	unsigned id = 0;

	for (; id < chain.length(); ++id) {
		switch (chain[id]) {
		case '0':

			break;

		case '1':

			break;

		case '(':
			++bopen;
			if (id == chain.length() - 1)
				return -8;       // Ошибка: незавершенная операция в конце выражения

			break;

		case ')':
			if (id == 0)
				return -4;       // Ошибка: операция от пустоты
			++bclose;
			if (bopen < bclose) return -3;     // Ошибка: закрывающих скобок больше, чем открывающих
			if (chain[id - 1] == '(')
				return -6;       // Ошибка: пустые скобки
			if (chain[id - 1] == '+')
				return -7;       // Ошибка: незавершенный "+"

			break;

		case '^':
			if (id == 0)
				return -4;       // Ошибка: операция от пустоты
			if (chain[id - 1] == '(' or chain[id - 1] == '+' or chain[id - 1] == '*' or chain[id - 1] == '^')
				return -5;       // Ошибка: некорректный цикл (после (, +, * или ^))


			break;

		case '*':
			if (id == 0)
				return -4;       // Ошибка: операция от пустоты
			if (chain[id - 1] == '(' or chain[id - 1] == '+' or chain[id - 1] == '*' or chain[id - 1] == '^')
				return -5;       // Ошибка: некорректный цикл (после (, +, * или ^))


			break;

		case '+':
			if (id == 0) return -4;       // Ошибка: операция от пустоты
			else
				if (chain[id - 1] == '(')
					return -2;       // Ошибка: символ "+" после скобки
			if (id == chain.length() - 1)
				return -8;       // Ошибка: незавершенная операция в конце выражения

			

			break;

		default:
			return -1;               // Ошибка: символ вне алфавита операций и букв
		}
	}

	if (bopen != bclose) return -9; // Ошибка: не соблюден баланс скобок

	return 1;

}

int FSMChain::OptimizeChain() {
	int num = 0, dont = 0, inside = 0, outside = 0;
	std::string::iterator uk1 = chain.begin(), uk2 = chain.begin();
	for (unsigned i = 0; i < chain.length() && !num; ++i) {
		if (chain[i] == '(') {
			uk1 = chain.begin() + i;
			inside = 0;
		}
		else if (chain[i] == '+') {
			++inside;
		}
		else if (chain[i] == ')') {
			uk2 = chain.begin() + i;
			if (chain.length() != i + 1)
				if (chain[i + 1] == '*' || chain[i + 1] == '^')
					++outside;
			if (!outside && !inside) {
				chain.erase(uk2);
				chain.erase(uk1);
				++num;
			}
		}
		else if (chain[i] == ' ')
			chain.erase(chain.begin() + i);
	}
	
	return num;
}

int FSMChain::AnalyzeChain() {
	if (!chain.length()) return -1;
	unsigned id = 0, dont = 0, dont2 = 0;
	int bopen = 0, bclose = 0;
	std::string::iterator uk1 = chain.begin(), uk2 = chain.begin();

	
	for (; id < chain.length(); ++id) {
		switch (chain[id]) {
		case '0':
			dont2 = 0;
			if (chain.length() != id + 1 && !dont)
				if (chain[id + 1] == '^' || chain[id + 1] == '*') {
					dont2 = 1;
					++id;
					std::string str(chain.begin() + id - 1, chain.begin() + id + 1);
					letts.push_back(str);
				}
					

			if (!dont && !dont2) {
				std::string str(1, chain[id]);
				letts.push_back(str);
			}

			break;

		case '1':
			dont2 = 0;
			if (chain.length() != id + 1 && !dont)
				if (chain[id + 1] == '^' || chain[id + 1] == '*') {
					dont2 = 1;
					++id;
					std::string str(chain.begin() + id - 1, chain.begin() + id + 1);
					letts.push_back(str);
				}


			if (!dont && !dont2) {
				std::string str(1, chain[id]);
				letts.push_back(str);
			}


			break;

		case '(':
			++bopen;
			if (bopen == bclose + 1) {
				uk1 = chain.begin() + id;
				dont = 1;
			}
			

			break;

		case ')':
			++bclose;
			if (bopen == bclose) {
				uk2 = chain.begin() + id;
				if (chain.length() != id + 1) {
					if (chain[id + 1] != '^' && chain[id + 1] != '*')
						dont = 0;
				}
				else dont = 0;
			}

			if (!dont) {
				std::string str(uk1, uk2 + 1);
				letts.push_back(str);
			}

			break;

		case '^':
			if (chain[id - 1] == ')' && bopen == bclose) {
				++uk2;
				dont = 0;
			}

			if (!dont) {
				std::string str(uk1, uk2 + 1);
				letts.push_back(str);
			}
			

			break;

		case '*':
			if (chain[id - 1] == ')' && bopen == bclose) {
				++uk2;
				dont = 0;
			}

			if (!dont) {
				std::string str(uk1, uk2 + 1);
				letts.push_back(str);
			}


			break;

		case '+':
			if (!dont) {
				std::string str(1, chain[id]);
				letts.push_back(str);
			}

			break;

		default:
			return -2;
		}

	}	

	return 0;
}

void FSMChain::CreateLetters() {
	letters.push_back("0");
	letters.push_back("1");
	letters.push_back("e");
	letters.push_back("END?");
	letuk = 2;
	unsigned short k = 0;

	for (unsigned i = 0; i < letts.size(); ++i) 
		if (letts[i] != "+") {
			k = 0;
			for (unsigned j = 0; j < letters.size(); ++j)
				if (letters[j] == letts[i]) ++k;
			if (!k) 
				letters.insert(letters.begin() + letuk++, letts[i]);
		}
}

void FSMChain::PushBackTable() {
	states.push_back(alphabet[numalph++]);
	std::vector<std::string> str;
	for (unsigned i = 0; i < letters.size(); ++i) {
		str.push_back("_");
	}
	table.push_back(str);
}

int FSMChain::FindLetIndex(std::string str) {
	for (unsigned i = 0; i < letters.size(); ++i)
		if (letters[i] == str) return i;
	return -1;
}

int FSMChain::CreateTable() {
	if (!letts.size()) return 0;
	CreateLetters();
	PushBackTable();

	unsigned curr = 0;
	for (unsigned i = 0; i < letts.size(); ++i) {
		if (curr > 35) return -1;
		if (letts[i] != "+") {
			PushBackTable();
			int id = FindLetIndex(letts[i]);
			if (table[curr][id] == "_")
				table[curr][id] = states[states.size() - 1];
			else table[curr][id].append(states[states.size() - 1]);

			curr = states.size() - 1;
		}
		else {
			table[curr][table[curr].size() - 1] = "1";
			curr = 0;
		}
	}
	table[curr][table[curr].size() - 1] = "1";
	return 1;
}

void FSMChain::PrintLetters() {
	std::cout << "   ";
	for (unsigned i = 0; i < letters.size(); ++i)
		std::cout << letters[i] << "| ";
	std::cout << std::endl;
}

void FSMChain::PrintStates() {
	for (unsigned i = 0; i < states.size(); ++i)
		std::cout << states[i] << " ";
	std::cout << std::endl;
}

void FSMChain::PrintTable() {
	if (!table.size()) std::cout << "Table is empty." << std::endl;
	else {
		PrintLetters();
		for (unsigned i = 0; i < states.size(); ++i) {
			std::cout << states[i] << "| ";
			for (unsigned j = 0; j < letters.size(); ++j) {
				std::cout << table[i][j];
				if ((int)letters[j].length() - (int)table[i][j].length() >= 0)
					for (unsigned k = 0; k < letters[j].length() - table[i][j].length(); ++k)
						std::cout << " ";
				std::cout << "| ";
			}
			std::cout << std::endl;
		}
	}
}


void FSMChain::DeleteExcess() {
	int k;
	for (unsigned j = 2; j < letters.size() - 1; ++j) { 
		k = 0;
		for (unsigned i = 0; i < states.size(); ++i)
			if (table[i][j] != "_") ++k;
		if (!k) {
			letters.erase(letters.begin() + j);
			for (unsigned p = 0; p < table.size(); ++p)
				table[p].erase(table[p].begin() + j);
			--j;
		}
	}
}

void FSMChain::MergeExcess() {
	for (unsigned i = 0; i < letters.size() - 2; ++i) 
		for (unsigned j = i + 1; j < letters.size() - 1; ++j)
			if (letters[i] == letters[j]) {
				for (unsigned i2 = 0; i2 < states.size(); ++i2) {
					if (table[i2][j] != "_")
						if (table[i2][i] == "_")
							table[i2][i] = table[i2][j];
						else
							table[i2][i].append(table[i2][j]);
					table[i2].erase(table[i2].begin() + j);
				}
				letters.erase(letters.begin() + j);
				--j;
			}
	unsigned k;
	for (unsigned i = 0; i < states.size() - 1; ++i)
		for (unsigned i2 = i + 1; i2 < states.size(); ++i2) {
			k = 0;
			for (unsigned j = 0; j < letters.size() && !k; ++j)
				if (table[i][j] != table[i2][j]) ++k;
			if (!k) {
				table.erase(table.begin() + i2);
				for (unsigned i3 = 0; i3 < states.size() - 1; ++i3)
					for (unsigned j3 = 0; j3 < letters.size(); ++j3)
						for (unsigned p = 0; p < table[i3][j3].length(); ++p)
							if (table[i3][j3][p] == states[i2][0])
								table[i3][j3][p] = states[i][0];
				states.erase(states.begin() + i2--);
			}
		} 
}

int FSMChain::MakeNFSMPrivate() {
	int did = 0;
	for (unsigned i = 0; i < states.size(); ++i) {
		for (unsigned j = 2; j < letters.size() - 2; ++j) {
			if (table[i][j] != "_") {
				std::string str2;
				unsigned k = j + 1;

				if (letters[j][letters[j].length() - 1] == '^' && (letters[j].length() == 2 || letters[j][0] == '(')) {
					std::string str = letters[j].substr(0, letters[j].length() - 1);
					letters.insert(letters.begin() + j + 1, str);
					for (unsigned p1 = 0; p1 < table.size(); ++p1)
						table[p1].insert(table[p1].begin() + j + 1, "_");

					PushBackTable();

					AppendTableCell(i, j + 1, states[states.size() - 1]);
					AppendTableCell(table.size() - 1, j + 1, states[states.size() - 1]);
					AppendTableCell(table.size() - 1, table[table.size() - 1].size() - 2, table[i][j]);
					table[i][j] = "_";
					++did;
				}

				if (letters[j][letters[j].length() - 1] == '*' && (letters[j].length() == 2 || letters[j][0] == '(')) {
					std::string str = letters[j].substr(0, letters[j].length() - 1);
					letters.insert(letters.begin() + j + 1, str);
					for (unsigned p1 = 0; p1 < table.size(); ++p1)
						table[p1].insert(table[p1].begin() + j + 1, "_");

					PushBackTable();

					AppendTableCell(i, table[i].size() - 2, states[states.size() - 1]);
					AppendTableCell(table.size() - 1, j + 1, states[states.size() - 1]);
					AppendTableCell(table.size() - 1, table[table.size() - 1].size() - 2, table[i][j]);
					table[i][j] = "_";
					++did;
				}

				if (letters[j][0] == '(' && letters[j][letters[j].length() - 1] == ')') {
					std::string str = letters[j].substr(1, letters[j].length() - 2);

					std::size_t pos = -1;
					std::size_t pos2 = pos + 1;
					std::vector<std::string> vec;
					while ((pos = str.find('+', pos + 1)) != std::string::npos) {
						vec.push_back(str.substr(pos2, pos - pos2));
						pos2 = pos + 1;
					}
					vec.push_back(str.substr(pos2));

					for (unsigned p = 0; p < vec.size(); ++p, ++k) {
						letters.insert(letters.begin() + k, vec[p]);
						for (unsigned p1 = 0; p1 < table.size(); ++p1)
							if (p1 != i)
								table[p1].insert(table[p1].begin() + k, "_");
							else
								table[p1].insert(table[p1].begin() + k, table[i][j]);
					}
					table[i][j] = "_";
					++did;
				}

				if ((letters[j][0] == '0' || letters[j][0] == '1') && letters[j].length() > 1) {
					unsigned jump = 1;
					if (letters[j][1] == '*' || letters[j][1] == '^')
						if (letters[j].length() > 2) ++jump;
						else break;
						std::string str = letters[j].substr(0, jump);
						std::string str2 = letters[j].substr(jump);
						letters.insert(letters.begin() + j + 1, str2);
						letters.insert(letters.begin() + j + 1, str);
						for (unsigned p1 = 0; p1 < table.size(); ++p1) {
							table[p1].insert(table[p1].begin() + j + 1, "_");
							table[p1].insert(table[p1].begin() + j + 1, "_");
						}
						PushBackTable();

						AppendTableCell(i, j + 1, states[states.size() - 1]);
						AppendTableCell(table.size() - 1, j + 2, table[i][j]);
						table[i][j] = "_";
						++did;
				}
			}
		}
	}
	return did;
}

void FSMChain::MakeNFSM() {
	while (MakeNFSMPrivate()) {
		PrintTable();
		std::cout << "-------------------------------------------------------------------------------------" << std::endl;
		DeleteExcess();
		PrintTable();
		std::cout << "-------------------------------------------------------------------------------------" << std::endl;
		MergeExcess();
		PrintTable();
		std::cout << "-------------------------------------------------------------------------------------" << std::endl;
	}
}


void FSMChain::OptimizeDFSMStep1() {    // Избавление от состояний, не порождающих терминальных цепочек
	std::vector<std::string> set;
	for (unsigned i = 0; i < states.size(); ++i)
		if (table[i][table[i].size() - 1] == "1" || table[i][table[i].size() - 2] != "_")
			set.push_back(states[i]);

	int flag;
	do {
		flag = 0;
		for (unsigned i = 0; i < states.size(); ++i)
			for (unsigned j = 0; j < letters.size() - 1; ++j)
				if (table[i][j] != "_")
					for (unsigned k = 0; k < table[i][j].length(); ++k)
						for (unsigned p = 0; p < set.size(); ++p)
							if (table[i][j][k] == set[p][0]) 
								if (std::find(set.begin(), set.end(), states[i]) == set.end()) {
									set.push_back(states[i]);
									++flag;
								}
	} while (flag);

	for (unsigned i = 0; i < states.size(); ++i) 
		if (std::find(set.begin(), set.end(), states[i]) == set.end()) {
			states.erase(states.begin() + i);
			table.erase(table.begin() + i--);
		}
		
}

unsigned FSMChain::CheckE() {
	if (letters[letters.size() - 2] != "e") 
		return -1;
	for (unsigned i = 0; i < states.size(); ++i)
		if (table[i][table[i].size() - 2] != "_") return i;
	return -1;
}

unsigned FSMChain::FindIndexTable(char ch) {
	for (unsigned i = 0; i < states.size(); ++i)
		if (ch == states[i][0]) return i;
	return -1;
}

void FSMChain::OptimizeDFSMStep2() { // Избавление от e-символов
	unsigned id = -1, k;
	while ((id = CheckE()) != -1) {
		k = FindIndexTable(table[id][table[id].size() - 2][0]);
		if (k == -1) {
			std::cout << "CRITICAL ERROR! CANNOT FIND STATE IN STEP2" << std::endl;
			return;
		}
		
		if (table[id][table[id].size() - 2].length() == 1)
			table[id][table[id].size() - 2][0] = '_';
		else
			table[id][table[id].size() - 2].erase(0);

		for (unsigned j = 0; j < letters.size(); ++j)
			AppendTableCell(id, j, table[k][j]);
		DeleteExcess();
		MergeExcess();
		PrintTable();
		std::cout << std::endl;
	}
}



void FSMChain::OptimizeDFSMStep3() { // Объединение состояний
	std::vector<std::string> set;
	std::vector<std::string> archset;
	std::vector<std::vector<unsigned>> ids;
	std::vector<std::vector<char>> bans;
	int recurs;
	do {
		recurs = 0;
		for (unsigned i = 0; i < states.size(); ++i)
			for (unsigned j = 0; j < letters.size(); ++j)
				if (table[i][j].length() > 1 && std::find(set.begin(), set.end(), table[i][j]) == set.end()) {
					set.push_back(table[i][j]);
					++recurs;
				}
		for (unsigned i = 0; i < set.size(); ++i) {
			std::vector<unsigned> id;
			for (unsigned j = 0; j < set[i].length(); ++j)
				id.push_back(FindIndexTable(set[i][j]));
			ids.push_back(id);
			id.clear();
		}

		for (unsigned i = 0; i < set.size(); ++i) {
			PushBackTable();
			std::vector<char> ban;
			for (unsigned j = 0; j < set[i].length(); ++j) {
				ban.push_back(set[i][j]);
				for (unsigned k = 0; k < letters.size(); ++k)
					AppendTableCell(table.size() - 1, k, table[ids[i][j]][k]);
			}
			archset.push_back(states[states.size() - 1]);
			bans.push_back(ban);
		}



		for (unsigned i = 0; i < set.size(); ++i) 
			for (unsigned p = 0; p < states.size(); ++p)
				for (unsigned j = 0; j < letters.size(); ++j)
					if (table[p][j] == set[i]) table[p][j] = states[states.size() - recurs + i];

		for (unsigned i = 0; i < states.size(); ++i)
			for (unsigned j = 0; j < letters.size(); ++j)
				if (table[i][j].length() > 1) {
					unsigned p = 0;
					for (; p < archset.size() && archset[p][0] != table[i][j][0]; ++p);
					if (p < archset.size())
						for (unsigned k = 1; k < table[i][j].length(); ++k)
							for (unsigned m = 0; m < bans[p].size(); ++m)
								if (bans[p][m] == table[i][j][k]) 
									table[i][j].erase(table[i][j].begin() + k);
				}
		
		MergeExcess();
		set.clear();
		ids.clear();
	} while (recurs);
}

void FSMChain::OptimizeDFSMStep4() { // Избавление от недостижимых состояний
	std::vector<char> set;
	set.push_back(states[0][0]);
	unsigned p = -1, i;
	do {
		++p;
		i = FindIndexTable(set[p]);
		if (i == -1) {
			std::cout << "CRITICAL ERROR! CANNOT FIND STATE IN STEP3" << std::endl;
			return;
		}
		for (unsigned j = 0; j < letters.size() - 1; ++j)
			if (table[i][j] != "_")
				for (unsigned k = 0; k < table[i][j].length(); ++k)
					if (std::find(set.begin(), set.end(), table[i][j][k]) == set.end() && (states[i][0] != table[i][j][k])) {
						set.push_back(table[i][j][k]);
					}
	} while (p != set.size() - 1);


	for (unsigned i = 0; i < states.size(); ++i)
		if (std::find(set.begin(), set.end(), states[i][0]) == set.end()) {
			states.erase(states.begin() + i);
			table.erase(table.begin() + i--);
			PrintTable();
			std::cout << std::endl;
		}
	MergeExcess();
}

void FSMChain::OptimizeDFSMStep5() {              // Устранение эквивалентных состояний
	std::vector<std::vector<std::string>> sets;
	std::vector<std::string> set;
	std::vector<std::vector<unsigned>> ids;
	for (unsigned i = 0; i < states.size(); ++i)
		if (table[i][table[i].size() - 1] != "1")
			set.push_back(states[i]);
	if (set.size()) {
		sets.push_back(set);
		set.clear();
	}
	for (unsigned i = 0; i < states.size(); ++i)
		if (table[i][table[i].size() - 1] == "1")
			set.push_back(states[i]);
	if (set.size()) {
		sets.push_back(set);
		set.clear();
	}

	for (unsigned i = 0; i < sets.size(); ++i) {
		std::vector<unsigned> id;
		for (unsigned j = 0; j < sets[i].size(); ++j) {
			unsigned k = FindIndexTable(sets[i][j][0]);
			id.push_back(k);
		}
		ids.push_back(id);
	}


	unsigned flag, left;
	do {
		flag = 0;
		for (unsigned key = 0; key < 2; ++key) {

			for (unsigned i = 0; i < sets.size(); ++i)
				for (unsigned j = 0; j < sets[i].size(); ++j)
					for (unsigned k = 0; k < sets.size(); ++k)
						/*if (table[FindIndexTable(sets[i][j][0])][key] == "_") ids[i][j] = 10;
						else*/ if (std::find(sets[k].begin(), sets[k].end(), table[FindIndexTable(sets[i][j][0])][key]) != sets[k].end())
							ids[i][j] = k;
			left = -1;
			for (unsigned i = 0; i < sets.size(); ++i) {
				unsigned main = ids[i][0];
				unsigned last = main;
				for (unsigned j = 1; j < sets[i].size(); ++j) {
				
					if (ids[i][j] != main) {
						++flag;
						if (ids[i][j] == last) {
							sets[left].push_back(sets[i][j]);
							ids[left].push_back(ids[i][j]);
							sets[i].erase(sets[i].begin() + j);
							ids[i].erase(ids[i].begin() + j);
						}
						if (left == -1) 
							left = sets.size();
						
						set.push_back(sets[i][j]);
						std::vector<unsigned> id;
						id.push_back(ids[i][j]);
						last = ids[i][j];
						sets.insert(sets.begin() + left, set);
						set.clear();
						ids.insert(ids.begin() + left, id);
						sets[i].erase(sets[i].begin() + j);
						ids[i].erase(ids[i].begin() + j);
					}
				}
			}
		}
	} while (flag);
	

	for (unsigned i = 0; i < sets.size(); ++i)
		if (sets[i].size() > 1) 
			for (unsigned j = 1; j < sets[i].size(); ++j) {
				unsigned num = FindIndexTable(sets[i][j][0]);
				for (unsigned i2 = 0; i2 < states.size(); ++i2)
					for (unsigned j2 = 0; j2 < letters.size(); ++j2)
						if (table[i2][j2] == sets[i][j])
							table[i2][j2] = sets[i][0];
				states.erase(states.begin() + num);
				table.erase(table.begin() + num);
			}


	/*for (unsigned i = 0; i < sets.size(); ++i)
		for (unsigned j = 0; j < sets[i].size(); ++j)
			std::cout << sets[i][j] << std::endl;*/

}


void FSMChain::MakeDFSM() {
	std::cout << "FIRST STEP:" << std::endl;
	OptimizeDFSMStep1();
	PrintTable();
	std::cout << "-------------------------------------------------------------------------------------" << std::endl;
	std::cout << "SECOND STEP:" << std::endl;
	OptimizeDFSMStep2();
	std::cout << "-------------------------------------------------------------------------------------" << std::endl;
	std::cout << "THIRD STEP:" << std::endl;
	OptimizeDFSMStep3();
	PrintTable();
	std::cout << "-------------------------------------------------------------------------------------" << std::endl;
	std::cout << "FOURTH STEP:" << std::endl;
	OptimizeDFSMStep4();
	PrintTable();
	std::cout << "-------------------------------------------------------------------------------------" << std::endl;
	std::cout << "FIFTH STEP:" << std::endl;
	OptimizeDFSMStep5();
	PrintTable();
}


void MainTest();

int main()
{
	int again = 1;
	do {
		FSMChain MyChain;
		int k = 0;
		std::cout << "Do you want the main test (1) or your expression (0)?" << std::endl;
		std::cin >> k;
		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

		if (k == 1) MainTest();
		else {

		

		do {
			std::cout << "Give me the expression. For letters use only 0 and 1." << std::endl;
			std::getline(std::cin, MyChain.chain);

			k = MyChain.CheckChain();
			switch (k) {
			case 1:
				std::cout << "All is good." << std::endl;
				break;
			case -1:
				std::cout << "ERROR: incorrect symbol found (outside of the alphabet)." << std::endl;
				break;
			case -2:
				std::cout << "ERROR: symbol '+' right after the opening bracket '('." << std::endl;
				break;
			case -3:
				std::cout << "ERROR: the amount of closing ')' is higher than the amount of opening '('" << std::endl;
				break;
			case -4:
				std::cout << "ERROR: Operation on the emptiness." << std::endl;
				break;
			case -5:
				std::cout << "ERROR: incorrect circle (after (, +, * или ^))." << std::endl;
				break;
			case -6:
				std::cout << "ERROR: empty brackets." << std::endl;
				break;
			case -7:
				std::cout << "ERROR: unfinished '+'." << std::endl;
				break;
			case -8:
				std::cout << "ERROR: unfinished opeartion at the end of chain." << std::endl;
				break;
			case -9:
				std::cout << "ERROR: no bracket's balance." << std::endl;
				break;
			}
		} while (k != 1);


		int m = 0;
		do {
			k = MyChain.OptimizeChain();
			if (k) ++m;
		} while (k);
		if (m) std::cout << "We've optimized your chain: " << MyChain.chain << std::endl;

		std::cout << "Now i will analyze your chain:" << std::endl;
		MyChain.AnalyzeChain();
		MyChain.PrintLetts();

		std::cout << "----------------------" << std::endl;
		if (MyChain.CreateTable() < 1) {
			std::cout << "ERROR in making Table." << std::endl;
		}
		else {
			MyChain.PrintStates();
			MyChain.PrintTable();
			std::cout << "----------------------" << std::endl << "Making NFSM:" << std::endl;
			MyChain.MakeNFSM();
			MyChain.PrintStates();
			MyChain.PrintTable();
			std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl << "Making DFSM:" << std::endl;
			MyChain.MakeDFSM();
		}


		std::cout << "Again? (1/0)" << std::endl;
		std::cin >> again;
		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		}
	} while (again);


	std::cout << "Okay, Good Bye!" << std::endl;
	system("Pause");
    return 0;
}


void MainTest() {
	int k;
	std::string str[6] = { "1^+1*01(11+01)^", "1^+1*00(00+11)*", "(0+1)*(1+00)(0+1)*", "(11+01)*(1*+1*01)", "(0+1)*+00+(01+10)", "11+(0+10)(10)*" };
	for (int i = 0; i < 6; ++i) {
		FSMChain MyChain(str[i]);
		std::cout << "CHAIN #" << i + 1 << ": " << MyChain.chain << std::endl;
		k = MyChain.CheckChain();
		if (k != 1) std::cout << "ERROR IN THE CHAIN #" << i + 1 << "!" << std::endl;
		int m = 0;
		do {
			k = MyChain.OptimizeChain();
			if (k) ++m;
		} while (k);
		if (m) std::cout << "Optimized chain #" << i + 1 << ": " << MyChain.chain << std::endl;

		std::cout << "Analyzing chain #" << i + 1 << ":" << std::endl;
		MyChain.AnalyzeChain();
		MyChain.PrintLetts();
		std::cout << "----------------------" << std::endl;
		if (MyChain.CreateTable() < 1) {
			std::cout << "ERROR in making Table." << std::endl;
		}
		else {
			MyChain.PrintStates();
			MyChain.PrintTable();
			std::cout << "----------------------" << std::endl << "Making NFSM:" << std::endl;
			MyChain.MakeNFSM();
			MyChain.PrintStates();
			MyChain.PrintTable();
			std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl << "Making DFSM:" << std::endl;
			MyChain.MakeDFSM();


		}
	}
	
}