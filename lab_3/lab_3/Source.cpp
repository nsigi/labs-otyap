#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
using namespace std;

struct Lex {
	char* type; // тип лексемы
	char* str; //текст лексемы
};

//состояния автомата
enum states { S, A, B, C, N, Q, F, D, G, P, E, Z };

//типы лексем
enum lexType { id, vl, co, ao, eq, wl, kw };

//матрица типов лексем
lexType matrTypes[8][9]{
	//          S   A   B   C   N   Q   F   D   G
	/*letters*/ id, id, co, co, wl, eq, wl, id, id,
	/*nums*/    vl, id, co, co, vl, eq, wl, vl, vl,
	/* +, - */  ao, id, co, co, vl, eq, wl, ao, ao,
	/* > */     co, id, co, co, vl, eq, wl, co, co,
	/* < */     co, id, co, co, vl, eq, wl, co, co,
	/* = */     eq, id, co, co, vl, eq, wl, eq, eq,
	/*other*/   wl, wl, co, co, wl, eq, wl, wl, wl,
	/*sep*/     wl, id, co, co, vl, eq, wl, ao, wl
};

//матрица состояний
states matrSt[8][9]{
	//          S  A  B  C  N  Q  F  D  G
	/*letters*/ A, A, P, P, F, P, F, P, A,
	/*nums*/    N, A, P, P, N, P, F, N, N,
	/* +, - */  E, P, Z, Z, P, Z, P, P, D,
	/* > */     B, P, P, E, P, P, P, P, B,
	/* < */     C, P, P, P, P, P, P, P, C,
	/* = */     Q, P, Z, Z, P, P, P, P, Q,
	/*other*/   F, F, P, P, F, P, F, F, F,
	/*sep*/     G, P, P, P, P, P, P, P, G
};

//определение типа считываемого символа
int setSign(char c) {
	if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')) return 0; //letters - 0
	if (isdigit(c)) return 1; //nums - 1
	if (c == ' ' || c == '\n' || c == '\t' || c == '\0') return 7; //sep - 7
	switch (c) {
	case '+': case '-': return 2; // '+, -' - 2
	case '>': return 3; // '>' - 3
	case '<': return 4; // '<' - 4
	case '=': return 5; // '=' - 5
	}
	return 6; // other - 6
};

//создание лексемы
char* createLex(char* str, int firstPos, int lastPos, lexType t) {
	//удаление лишних нулей перед константой
	if (t == 1)
		while (str[firstPos] == '0' && firstPos < lastPos - 1)
			++firstPos;

	//выделяем память под нашу лексему
	char* lex = new char[lastPos - firstPos + 1];

	int i;
	//копирование подстроки, как лексемы
	for (i = 0; firstPos ^ lastPos; lex[i++] = str[firstPos++]);
	lex[i] = '\0';
	return lex;
}

//проверка идентификатора на то, является ли он ключевым словом
lexType checkKeyword(char*& str) {
	if ((strcmp(str, "do") && strcmp(str, "until") && strcmp(str, "loop")))	return id;
	return kw;
}

//проверка идентификатора на корректность: не больше 5 символов
lexType checkId(char*& str) {
	if (strlen(str) <= 5) return id;
	return wl;
}

//проверка константы на принадлежность допустимому диапозону
lexType checkVl(char*& str) {
	int value = atoi(str);
	if (value > -32769 && value < 32768) return vl;
	return wl;
}

//определение типа лексемы, как строки
char* setType(int t) {
	char* pType = new char[3];
	char* pT = pType;
	switch (t) {
	case 0: {*pType++ = 'i'; *pType++ = 'd'; break;}
	case 1: {*pType++ = 'v'; *pType++ = 'l'; break;}
	case 2: {*pType++ = 'c'; *pType++ = 'o'; break;}
	case 3: {*pType++ = 'a'; *pType++ = 'o'; break;}
	case 4: {*pType++ = 'e'; *pType++ = 'q'; break;}
	case 5: {*pType++ = 'w'; *pType++ = 'l'; break;}
	case 6: {*pType++ = 'k'; *pType++ = 'w'; break;}
	}

	*pType = '\0';
	return pT;
}

//добавление лексемы в соответствующий(-ие) список(-и) лексем
void addLex(Lex lexeme, vector<Lex>& lexs, vector<Lex>& identifiers, vector<Lex>& consts) {
	lexs.push_back(lexeme);
	if (!strcmp(lexeme.type, "vl"))  consts.push_back(lexeme);
	else if (!strcmp(lexeme.type, "id")) identifiers.push_back(lexeme);
}

void worsProcess(char* str, int size, vector<Lex>& lexs, vector<Lex>& identifiers, vector<Lex>& consts) {
	//lexs - список всех лексем
	//identifiers - идентификаторы
	//consts - константы

	int fPos, pos, curCh, tPos; // pos - текущая позиция, fPos - позиция начала новой лексемы, curCh - текущий тип символа, tPos - изменяемая позиция
	fPos = pos = 0;
	lexType type;
	states curSt = S; // текущее состояние автомата
	Lex lexeme; // текущая лексема
	bool stRollback, isEqual;

	while (pos ^ size) {
		curCh = setSign(str[pos]);

		//определяем тип лексемы в соответсвии с матрицей типов лексем
		type = matrTypes[curCh][curSt];

		//задаем начальную позицию лексемы
		if (curSt == G || curSt == S) fPos = pos;

		//определяем состояние в соответсвии с матрицей состояний
		curSt = matrSt[curCh][curSt];

		stRollback = curSt == P;

		//завершающее состояние, допускающее обработку возможного унарного + и -
		if (curSt == Z) {
			isEqual = (curCh == 5);
			tPos = isEqual ? pos + 1 : pos;
			lexeme.str = createLex(str, fPos, tPos, type);
			lexeme.type = setType(type);

			addLex(lexeme, lexs, identifiers, consts);

			curSt = isEqual ? G : D;
			if (isEqual) curSt = G;
			else {
				curSt = D;
				fPos = tPos;
			}
		}

		//записываем лексему в наш список (вектор) по достижении завершающего состояния
		if (stRollback || curSt == E) {
			tPos = stRollback ? pos : pos + 1;
			lexeme.str = createLex(str, fPos, tPos, type);

			//проверка констант и идентификаторов на корректность
			switch (type) {
			case id: { type = checkId(lexeme.str);break; }
			case vl: { type = checkVl(lexeme.str);break; }
			default: break;
			}

			if (!type)
				type = checkKeyword(lexeme.str);
			lexeme.type = setType(type);

			addLex(lexeme, lexs, identifiers, consts);

			curSt = states::S;
		}

		if (!stRollback) ++pos;
	}
}

//функция записи лексем в файл
void output(vector<Lex>& lexs, vector<Lex>& identifiers, vector<Lex>& consts) {
	//открываем файловый поток вывода
	ofstream fOut("output.txt", ios_base::trunc);

	for (int i = 0; i < (int)lexs.size(); ++i)
		fOut << lexs[i].str << '[' << lexs[i].type << ']' << ' ';

	fOut << endl;

	for (int i = 0; i < (int)identifiers.size(); ++i)
		fOut << identifiers[i].str << ' ';

	fOut << endl;

	for (int i = 0; i < (int)consts.size(); ++i)
		fOut << consts[i].str << ' ';

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

		vector<Lex> lexs; //список лексем
		vector<Lex> identifiers;
		vector<Lex> consts;
		worsProcess(str, size, lexs, identifiers, consts);

		output(lexs, identifiers, consts);

		//освобождение памяти для всего списка лексем
		for (int i = 0; i ^ lexs.size(); ++i) {
			delete[] lexs[i].str;
			delete[] lexs[i].type;
		}
		delete[]str;
	}
	else {
		cout << "Ошибка открытия файла";
	}

	return 0;
}