#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <vector>
using namespace std;

struct Lex {
	bool valid; //������������ �������
	char* str; //����� �������
};

//����������� ���� ������������ ��������
int setSign(char c) {
	if (c >= '0' && c <= '9') return 0; //nums - 0
	if ((c >= '�' && c <= '�') || (c == '�' || c == '�')) return 1; //letters - 1
	if (c == ' ' || c == '\n' || c == '\t' || c == '\0') return 2; //specCh - 2
	return 3; // other - 3
};

//��������� ��������
enum states { S, A, B, F, C };

//����������� �������
char* createLex(char* str, int firstPos, int lastPos) {
	//�������� ������ ��� ���� �������
	char* word = new char[lastPos - firstPos + 1];

	int i;
	//������������ ����� ���������, ��� �������
	for (i = 0; firstPos ^ lastPos; word[i++] = str[firstPos++]);
	word[i] = '\0';
	return word;
}

void findWords(char* str, int size, vector<Lex>& words) {
	//words - �������� �����

	int fPos, pos; // pos - ������� �������, fPos - ������� ������ ������ �����
	fPos = pos = 0;
	states curSt = states::S; // ������� ��������� ��������
	Lex lexeme; // ������� �������

	//������� ���������
	states matr[4][4]{
		//          S, A, B, F
		/*nums*/    A, B, A, F,
		/*letters*/ F, A, A, F,
		/*specCh*/  S, C, C, S,
		/*other*/   F, F, F, F
	};

	while (pos ^ size) {
		int curCh = setSign(str[pos]);

		//������ ��������� ������� �����, ����� � ��������� ��������� �������������� ������������ ������
		if (curSt == states::S && curCh != 2) {
			fPos = pos;
			lexeme.valid = true;
		}

		//���������� ��������� � ����������� � �������� ���������
		curSt = matr[curCh][curSt];

		//����������� ������� �� ������������ � ����������� � ��������
		if (curSt == states::F || (curSt == states::C && setSign(str[pos - 1]) != 0))
			lexeme.valid = false;

		//���������� ����� � ��� ������ (������), ���� ������� ������� � ����������� ��������� � ������� ���������
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

//������� ������ ���� � ���� � ����� � �������
void outputW(vector<Lex> words, int size) {
	//��������� �������� ����� ������
	ofstream fOut("output.txt", ios_base::trunc);

	for (int i = 0; i < size;++i) {
		cout << words[i].str << ' ';
		fOut << words[i].str << ' ';
	}

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

		vector<Lex> words; //������ ����
		findWords(str, size, words);

		int nSize = words.size();

		outputW(words, nSize);

		//������������ ������ ��� ����� ������ ����
		for (int i = 0; i ^ nSize; ++i)
			delete[] words[i].str;
		delete[]str;
	}
	else {
		cout << "������ �������� �����";
	}

	return 0;
}