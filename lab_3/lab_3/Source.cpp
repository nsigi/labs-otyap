#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
using namespace std;

struct Lex {
	char* type; // ��� �������
	char* str; //����� �������
};

//��������� ��������
enum states { S, A, B, C, N, Q, F, D, G, P, E, Z };

//���� ������
enum lexType { id, vl, co, ao, eq, wl, kw };

//������� ����� ������
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

//������� ���������
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

//����������� ���� ������������ �������
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

//�������� �������
char* createLex(char* str, int firstPos, int lastPos, lexType t) {
	//�������� ������ ����� ����� ����������
	if (t == 1)
		while (str[firstPos] == '0' && firstPos < lastPos - 1)
			++firstPos;

	//�������� ������ ��� ���� �������
	char* lex = new char[lastPos - firstPos + 1];

	int i;
	//����������� ���������, ��� �������
	for (i = 0; firstPos ^ lastPos; lex[i++] = str[firstPos++]);
	lex[i] = '\0';
	return lex;
}

//�������� �������������� �� ��, �������� �� �� �������� ������
lexType checkKeyword(char*& str) {
	if ((strcmp(str, "do") && strcmp(str, "until") && strcmp(str, "loop")))	return id;
	return kw;
}

//�������� �������������� �� ������������: �� ������ 5 ��������
lexType checkId(char*& str) {
	if (strlen(str) <= 5) return id;
	return wl;
}

//�������� ��������� �� �������������� ����������� ���������
lexType checkVl(char*& str) {
	int value = atoi(str);
	if (value > -32769 && value < 32768) return vl;
	return wl;
}

//����������� ���� �������, ��� ������
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

//���������� ������� � ���������������(-��) ������(-�) ������
void addLex(Lex lexeme, vector<Lex>& lexs, vector<Lex>& identifiers, vector<Lex>& consts) {
	lexs.push_back(lexeme);
	if (!strcmp(lexeme.type, "vl"))  consts.push_back(lexeme);
	else if (!strcmp(lexeme.type, "id")) identifiers.push_back(lexeme);
}

void worsProcess(char* str, int size, vector<Lex>& lexs, vector<Lex>& identifiers, vector<Lex>& consts) {
	//lexs - ������ ���� ������
	//identifiers - ��������������
	//consts - ���������

	int fPos, pos, curCh, tPos; // pos - ������� �������, fPos - ������� ������ ����� �������, curCh - ������� ��� �������, tPos - ���������� �������
	fPos = pos = 0;
	lexType type;
	states curSt = S; // ������� ��������� ��������
	Lex lexeme; // ������� �������
	bool stRollback, isEqual;

	while (pos ^ size) {
		curCh = setSign(str[pos]);

		//���������� ��� ������� � ����������� � �������� ����� ������
		type = matrTypes[curCh][curSt];

		//������ ��������� ������� �������
		if (curSt == G || curSt == S) fPos = pos;

		//���������� ��������� � ����������� � �������� ���������
		curSt = matrSt[curCh][curSt];

		stRollback = curSt == P;

		//����������� ���������, ����������� ��������� ���������� �������� + � -
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

		//���������� ������� � ��� ������ (������) �� ���������� ������������ ���������
		if (stRollback || curSt == E) {
			tPos = stRollback ? pos : pos + 1;
			lexeme.str = createLex(str, fPos, tPos, type);

			//�������� �������� � ��������������� �� ������������
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

//������� ������ ������ � ����
void output(vector<Lex>& lexs, vector<Lex>& identifiers, vector<Lex>& consts) {
	//��������� �������� ����� ������
	ofstream fOut("output.txt", ios_base::trunc);

	for (int i = 0; i < (int)lexs.size(); ++i)
		fOut << lexs[i].str << '[' << lexs[i].type << ']' << ' ';

	fOut << endl;

	for (int i = 0; i < (int)identifiers.size(); ++i)
		fOut << identifiers[i].str << ' ';

	fOut << endl;

	for (int i = 0; i < (int)consts.size(); ++i)
		fOut << consts[i].str << ' ';

	//��������� ����� ������
	fOut.close();
}

int main() {
	setlocale(LC_ALL, "Russian");

	//��������� �������� ����� �����
	ifstream fIn("input.txt");
	if (fIn) {
		//������ ���������� �������� �������� � ����� � ������������ ������� � ������ �����
		fIn.seekg(0, ios::end);
		int size = fIn.tellg();
		++size;
		fIn.seekg(0, ios::beg);

		char* str = new char[size] {};

		fIn.getline(str, size, '\0');

		//��������� ����� �����
		fIn.close();

		vector<Lex> lexs; //������ ������
		vector<Lex> identifiers;
		vector<Lex> consts;
		worsProcess(str, size, lexs, identifiers, consts);

		output(lexs, identifiers, consts);

		//������������ ������ ��� ����� ������ ������
		for (int i = 0; i ^ lexs.size(); ++i) {
			delete[] lexs[i].str;
			delete[] lexs[i].type;
		}
		delete[]str;
	}
	else {
		cout << "������ �������� �����";
	}

	return 0;
}