#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <vector>
using namespace std;

struct Lex {
	bool valid; //корректность лексемы
	char* str; //текст лексемы
};

//определение типа считываемого сиимвола
int setSign(char c) {
	if (c >= '0' && c <= '9') return 0; //nums - 0
	if ((c >= 'А' && c <= 'я') || (c == 'ё' || c == 'Ё')) return 1; //letters - 1
	if (c == ' ' || c == '\n' || c == '\t' || c == '\0') return 2; //specCh - 2
	return 3; // other - 3
};

//состояния автомата
enum states { S, A, B, F, C };

//составление лексемы
char* createLex(char* str, int firstPos, int lastPos) {
	//выделяем память под нашу лексему
	char* word = new char[lastPos - firstPos + 1];

	int i;
	//копируование части подстроки, как лексемы
	for (i = 0; firstPos ^ lastPos; word[i++] = str[firstPos++]);
	word[i] = '\0';
	return word;
}

void findWords(char* str, int size, vector<Lex>& words) {
	//words - найденые слова

	int fPos, pos; // pos - текущая позиция, fPos - позиция начала нового слова
	fPos = pos = 0;
	states curSt = states::S; // текущее состояние автомата
	Lex lexeme; // текущая лексема

	//матрица состояний
	states matr[4][4]{
		//          S, A, B, F
		/*nums*/    A, B, A, F,
		/*letters*/ F, A, A, F,
		/*specCh*/  S, C, C, S,
		/*other*/   F, F, F, F
	};

	while (pos ^ size) {
		int curCh = setSign(str[pos]);

		//задаем начальную позицию слова, когда в начальном состоянии обнаруживается непробельный символ
		if (curSt == states::S && curCh != 2) {
			fPos = pos;
			lexeme.valid = true;
		}

		//определяем состояние в соответсвии с матрицей состояний
		curSt = matr[curCh][curSt];

		//определение лексемы на корректность в соответсвии с заданием
		if (curSt == states::F || (curSt == states::C && setSign(str[pos - 1]) != 0))
			lexeme.valid = false;

		//записываем слово в наш список (вектор), если автомат перешел в завершающие состояние и лексема корректна
		if (curSt == states::C) {
			if (lexeme.valid) {
				lexeme.str = createLex(str, fPos, pos);
				words.push_back(lexeme);
			}

			curSt = states::S;
		}
		++pos;
	}
}

//функция записи слов в файл и вывод в консоль
void outputW(vector<Lex> words, int size) {
	//открываем файловый поток вывода
	ofstream fOut("output.txt", ios_base::trunc);

	for (int i = 0; i < size;++i) {
		cout << words[i].str << ' ';
		fOut << words[i].str << ' ';
	}

	//закрываем поток вывода
	fOut.close();
}

int main() {
	setlocale(LC_ALL, "Russian");

	//открываем файловый поток ввода
	ifstream fIn("input.txt");
	if (fIn) {
		//узнаем количество читаемых символов в файле и возвращаемся обратно в начало файла
		fIn.seekg(0, ios::end);
		int size = fIn.tellg();
		++size;
		fIn.seekg(0, ios::beg);

		char* str = new char[size] {};

		fIn.getline(str, size, '\0');

		//закрываем поток ввода
		fIn.close();

		vector<Lex> words; //список слов
		findWords(str, size, words);

		int nSize = words.size();

		outputW(words, nSize);

		//освобождение памяти для всего списка слов
		for (int i = 0; i ^ nSize; ++i)
			delete[] words[i].str;
		delete[]str;
	}
	else {
		cout << "Ошибка открытия файла";
	}

	return 0;
}