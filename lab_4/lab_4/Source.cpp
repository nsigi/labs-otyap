#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
using namespace std;

//состояния автомата
enum lexStates { S, A, B, N, F, C, D, U1, U2, U3, U, L1, L2, L, O, I1, I, X, P, E }; //состояния для матрицы лексического анализатора
enum synStates { s, a, b, c, d, e, g, h, i, j, k, l, b1, e1, i1, f }; //состояния для матрицы синтаксического анализатора

enum lexType { id, vl, co, ao, eq, wl, dO, lp, un, lo, mi }; //типы лексем

//матрица состояний лексического анализатора
lexStates matrLex[18][17]{
	//			     S, A, B, N, F, C, D, U1, U2, U3, U, L1, L2, L, O, I1, I
	/*letters*/		 A, A, P, F, F, A, A, A,  A,  A,  A, A,  A,  A, A, A,  A,
	/*nums*/         N, A, P, N, F, A, A, A,  A,  A,  A, A,  A,  A, A, A,  A,
	/* +, -, /, * */ E, P, P, P, P, P, X, X,  X,  X,  X, X,  X,  X, X, X,  X,
	/* > */			 E, P, E, P, P, P, X, X,  X,  X,  X, X,  X,  X, X, X,  X,
	/* < */			 B, P, P, P, P, P, X, X,  X,  X,  X, X,  X,  X, X, X,  X,
	/* = */			 E, P, P, P, P, P, X, X,  X,  X,  X, X,  X,  X, X, X,  X,
	/*other*/		 F, F, P, F, F, F, F, F,  F,  F,  F, F,  F,  F, F, F,  F,
	/*sep*/			 S, P, P, P, P, P, X, X,  X,  X,  X, X,  X,  X, X, X,  X,
	/*d*/			 D, A, P, F, F, A, A, A,  A,  A,  A, A,  A,  A, A, A,  C,
	/*o*/			 O, A, P, F, F, A, C, A,  A,  A,  A, L2, L,  A, A, A,  A,
	/*u*/			U1, A, P, F, F, A, A, A,  A,  A,  A, A,  A,  A, A, A,  A,
	/*n*/			 A, A, P, F, F, A, A, U2, A,  A,  A, A,  A,  A, A, I,  A,
	/*t*/			 A, A, P, F, F, A, A, A, U3,  A,  A, A,  A,  A, A, A,  A,
	/*i*/			 A, A, P, F, F, A, A, A,  A,  U,  A, A,  A,  A, A, A,  A,
	/*l*/			L1, A, P, F, F, A, A, A,  A,  A,  C, A,  A,  A, A, A,  A,
	/*p*/			 A, A, P, F, F, A, A, A,  A,  A,  A, A,  A,  C, A, A,  A,
	/*a*/			I1, A, P, F, F, A, A, A,  A,  A,  A, A,  A,  A, A, A,  A,
	/*r*/			 A, A, P, F, F, A, A, A,  A,  A,  A, A,  A,  C, C, A,  A
};

//матрица состояний синтаксического анализатора
synStates matrSyn[11][16]{
	//     s, a, b, c, d, e, g, h, i, j, k, l, b1, e1, i1, f
	/*id*/ f, f, c, g, e, g, f, i, f, k, f, f, f,  f,  f,  f,
	/*vl*/ f, f, c, f, e, f, f, i, f, k, f, f, c,  e,  i,  f,
	/*co*/ f, f, f, d, f, f, f, f, f, f, f, f, f,  f,  f,  f,
	/*ao*/ f, f, f, f, f, f, f, f, j, f, j, f, f,  f,  f,  f,
	/*eq*/ f, f, f, d, f, f, h, f, f, f, f, f, f,  f,  f,  f,
	/*wl*/ f, f, f, f, f, f, f, f, f, f, f, f, f,  f,  f,  f,
	/*do*/ a, f, f, f, f, f, f, f, f, f, f, a, f,  f,  f,  f,
	/*lp*/ f, f, f, f, f, f, f, f, l, f, l, f, f,  f,  f,  f,
	/*un*/ f, b, f, f, f, f, f, f, f, f, f, f, f,  f,  f,  f,
	/*lo*/ f, f, f, b, f, b, f, f, f, f, f, f, f,  f,  f,  f,
	/*mi*/ f, f,b1, f,e1, f, f,i1, j, f, j, f, f,  f,  f,  f
};

struct Lex {
	lexType type; // тип лексемы
	char* textType; //текст типа лексемы
	char* str; //текст лексемы
};

//определение типа считываемого сиимвола
int getSign(char c) {
	switch (c) {
	case '+': case '-': case '/': case '*': return 2; // '+, -, /, *' - 2
	case '>': return 3; // '>' - 3
	case '<': return 4; // '<' - 4
	case '=': return 5; // '=' - 5
	case 'd': return 8; // 'd' - 8
	case 'o': return 9; // 'o' - 9
	case 'u': return 10; // 'u' - 10
	case 'n': return 11; // 'n' - 11
	case 't': return 12; // 't' - 12
	case 'i': return 13; // 'i' - 13
	case 'l': return 14; // 'l' - 14
	case 'p': return 15; // 'p' - 15
	case 'a': return 16; // 'a' - 16
	case 'r': return 17; // 'r' - 17
	}

	if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')) return 0; //letters - 0
	if (c >= '0' && c <= '9') return 1; //nums - 1
	if (c == ' ' || c == '\n' || c == '\t' || c == '\0') return 7; //sep - 7

	return 6; // other - 6
}

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

//проверка лексемы на корректность
lexType checkId(char*& str) {
	if (strlen(str) <= 5)
		return id;
	return wl;
}

//проверка константы на корректность
lexType checkVl(char*& str) {
	if (abs(atoi(str)) < 32769)
		return vl;
	return wl;
}

//установка текста лексемы в соответсвии с её типом 
char* setTextType(int t) {
	char* pType = new char[3];
	char* pT = pType;
	switch (t) {
	case 0: {*pType++ = 'i'; *pType++ = 'd'; break;}
	case 1: {*pType++ = 'v'; *pType++ = 'l'; break;}
	case 2: {*pType++ = 'c'; *pType++ = 'o'; break;}
	case 3:
	case 10: {*pType++ = 'a'; *pType++ = 'o'; break;}
	case 4: {*pType++ = 'e'; *pType++ = 'q'; break;}
	case 5: {*pType++ = 'w'; *pType++ = 'l'; break;}
	case 6: {*pType++ = 'd'; *pType++ = 'o'; break;}
	case 7: {*pType++ = 'l'; *pType++ = 'p'; break;}
	case 8: {*pType++ = 'u'; *pType++ = 'n'; break;}
	case 9: {*pType++ = 'l'; *pType++ = 'o'; break;}
	}

	*pType = '\0';
	return pT;
}

//определение типа лексемы
lexType detType(lexStates prevSt, int curCh, Lex lexeme) {
	switch (prevSt) {
	case A: case U1: case U2: case U3: case L1: case L2: case I1: return checkId(lexeme.str);
	case N: return checkVl(lexeme.str);
	case D: return dO;
	case U: return un;
	case L: return lp;
	case O: case I: return lo;
	case F: return wl;
	case B: return co;
	case S: {switch (curCh) {
	case 2: return ao;
	case 3: case 4: return co;
	case 5: return eq;
	}
	}
	default: return wl;
	}
}

//лексический анализ
void lexAnalys(char* str, int size, vector<Lex>& lexs) {
	//lexs - найденые лексемы
	int fPos, pos, curCh; // pos - текущая позиция, fPos - позиция начала нового слова
	pos = 0;
	lexStates curSt, prevSt;// текущее и предыдущее состояния автомата соответственно
	curSt = prevSt = S;
	Lex lexeme; // текущая лексема
	bool stStay; //флаг состояния Р

	while (pos ^ size) {
		curCh = getSign(str[pos]);// определяем тип текущего символа 

		//задаем начальную позицию слова, когда в начальном состоянии обнаруживается непробельный символ
		if (curSt == S) fPos = pos;

		//определяем состояние в соответсвии с матрицей состояний
		curSt = matrLex[curCh][curSt];

		stStay = curSt == P;

		//состояние, когда лексема подозрительная на ключевое слово, на самом деле не ключевое слово
		if (curSt == X) {
			curSt = E;
			prevSt = A;
			--pos;
		}

		//записываем слово в наш список (вектор), если автомат перешел в завершающие состояние 
		if (stStay || curSt == E) {
			int tPos = stStay ? pos : pos + 1;
			lexeme.str = createLex(str, fPos, tPos);

			lexeme.type = (!stStay && str[pos] == '-') ? (getSign(str[pos + 1]) == 1 ? mi : ao) : detType(prevSt, curCh, lexeme);
			lexeme.textType = setTextType(lexeme.type);
			lexs.push_back(lexeme);

			curSt = S;
		}

		if (!stStay) ++pos;
		if (curSt != C) prevSt = curSt; //С - предзаписывающее состояние ключевых слов
	}
}

//вывод списка ожидаемых (-ой) лексем (-ы) в случае нарушения конструкции
void errMes(synStates prevSt, ofstream& fout, int pos) {
	fout << pos << ' ';
	cout << pos << ' ';
	switch (prevSt)
	{
	case s: case l: { fout << "do"; cout << "do"; return; }
	case a: { fout << "un";  cout << "un"; return; }
	case b: case d: case h: case j: { fout << "id "; cout << "id "; }
	case b1: case e1: case i1: { fout << "vl"; cout << "vl"; return; }
	case c: { fout << "co eq "; cout << "co eq "; }
	case e: { fout << "id lo"; cout << "id lo"; return; }
	case g: { fout << "eq"; cout << "eq"; return; }
	case i: case k: { fout << "ao lp";  cout << "ao lp"; return; }
	default:
		return;
	}
}

//синтаксический анализ
void synAnalys(vector<Lex>& lexs, ofstream& fout) {
	int pos; //индекс текущей лексемы
	int size = lexs.size();
	synStates curSt, prevSt;// текущее и предыдущее состояния автомата соответственно
	curSt = prevSt = s;

	for (pos = 0; pos ^ size; ++pos, prevSt = curSt) {
		//проверяем константу, не равна ли она 32768 (не выходит ли из диапозона)
		if (lexs[pos].type == vl && !strcmp(lexs[pos].str, "32768") && !(prevSt == b1 || prevSt == e1 || prevSt == i1))  curSt = f;

		//определяем состояние в соответсвии с матрицей состояний
		curSt = matrSyn[lexs[pos].type][curSt];

		//фиксация нарушения конструкции
		if (curSt == f) {
			errMes(prevSt, fout, pos);
			return;
		}
	}

	if (curSt == l)	fout << "OK"; //ошибок в конструкции (-ях) нет
	else errMes(prevSt, fout, pos); //если не найдено ошибок внутри конструкции (-ий), но одна или несколько из них не закончены (прерваны) -
									//выводим нарушение конструкции с указанием места, где ожидалась лексема
}

int main() {
	setlocale(LC_ALL, "Russian");

	//открываем файловый поток ввода
	ifstream fin("input.txt");
	if (fin) {
		//узнаем количество читаемых символов в файле и возвращаемся обратно в начало файла
		fin.seekg(0, ios::end);
		int size = fin.tellg();
		++size;
		fin.seekg(0, ios::beg);

		char* str = new char[size] {};

		fin.getline(str, size, '\0');

		//закрываем поток ввода
		fin.close();

		vector<Lex> lexs; //список лексем
		lexAnalys(str, size, lexs);

		size = lexs.size();

		//открываем файловый поток вывода
		ofstream fout("output.txt", ios_base::trunc);

		for (int i = 0; i ^ size; ++i) {
			fout << lexs[i].str << '[' << lexs[i].textType << ']' << ' ';
			cout << lexs[i].str << '[' << lexs[i].textType << ']' << ' ';
		}
		fout << endl;
		cout << endl;

		synAnalys(lexs, fout);

		//закрываем поток вывода
		fout.close();

		//освобождение памяти для всего списка слов
		for (int i = 0; i ^ size; ++i) {
			delete[] lexs[i].str;
			delete[] lexs[i].textType;
		}
		delete[]str;
	}
	else {
		cout << "Ошибка открытия файла";
	}

	return 0;
}