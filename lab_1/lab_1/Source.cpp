//�� ����� �������� �� �����������, ����� �� ������ ������������ (����� �� ������� ������� � �������� � 43 �������)
//����� � � � ����������

#include <iostream>
#include <fstream>
#include <vector>
#include <set>
using namespace std;

//������� ����������� ������������ '\t' � '\n' 
bool chSep(char a, char b) {
	if ((a == 92) && (b == 't' || b == 'n'))
		return true;
	return false;
}

vector<char*> findWords(char* str, int size) {
	int i, k, st;
	string c;

	//��������� ����
	set <char> nums;
	for (i = 48; i ^ 58; nums.insert(i++));

	//��������� ������� ����
	set <char> letters;
	for (char i = 192; i; letters.insert(i++));

	//��������� ����������� ��������
	set <char> specCh;
	specCh.insert(' ');
	specCh.insert('\t');
	specCh.insert('\n');
	specCh.insert('\0');

	vector<char*> words;//�������� �����

	char* word = new char[size] {};//�������������� ���������� �����, ��������������� �������

	for (i = st = k = 0; i ^ size; ++i)
	{
		//�������� �� ������� ������������ � �������������� ��������
		if (chSep(str[i], str[i + 1])) {
			if (word[0]) st = 3;
			else {
				st = 4;
				str[i + 1] = ' ';
			}

			continue;
		}

		switch (st) {
			// ��������� ������ ������� ������� ������� (������ �����)
		case 0:
			c = str[i];
			if (nums.find(str[i]) != nums.end()) {
				word[k++] = str[i];
				(specCh.find(str[i + 1]) != specCh.end()) ? st = 3 : st = 1;//�������� �� ��, ��� ����� ������� �� ����� �����
			}
			else if ((specCh.find(str[i]) == specCh.end())) st = 4;
			break;
			// ��������� ������ ������� �������� � ���� 
		case 1:
			c = str[i];
			if (letters.find(str[i]) != letters.end())
				word[k++] = str[i];
			else if ((nums.find(str[i]) != nums.end())) {//�������� �� ���������� �����
				st = 2;
				--i;
			}
			else {
				delete[]word;
				word = new char[size] {};
				k = 0;
				--i;
				st = 4;
			}
			break;
			//�������� �������� ����������� �����
		case 2:
			c = str[i];
			if (specCh.find(str[i + 1]) != specCh.end()) {//���������, ������ �� ���� ������ (' ', '\n', '\t')
				word[k++] = str[i];
				st = 3;
			}
			else if ((nums.find(str[i]) != nums.end()) || (letters.find(str[i]) != letters.end())) {//���� ����� �� �����������, ���������� ��� ����������, ����������� � ��������� 1
				word[k++] = str[i];
				st = 1;
			}
			else {//���� �� ������������ ����� ������ ������, �������� �� �������������� (���� � ������� ����), �� ��������� ������� � ����������� ��������� 4
				k = 0;
				st = 4;
				delete[]word;
				word = new char[size] {};
			}
			break;
			//�������������� ���������
		case 3:
			words.push_back(word);
			word = new char[size] {};
			k = st = 0;
			break;
			//����������� ���������, ����� ������� ������ �������� �� ���, ������� ������������� �������
		case 4:
			c = str[i];
			if (specCh.find(str[i]) != specCh.end())  st = 0;
			break;
		}
	}

	//������������ ������ ��� ����������� �����
	delete[]word;

	return words;
}

//������� ������ ���� � ���� � ����� � �������
void outputW(vector<char*> words, int size) {
	//��������� �������� ����� ������
	ofstream fOut("output.txt", ios_base::trunc);

	for (int i = 0; i < size;++i) {
		cout << words[i] << ' ';
		fOut << words[i] << ' ';

	}

	//��������� ����� ������
	fOut.close();
}

int main()
{
	setlocale(LC_ALL, "Russian");

	//��������� �������� ����� ������
	ifstream fIn("input.txt");

	//������ ���������� �������� �������� � ����� � ������������ ������� � ������ �����
	fIn.seekg(0, ios::end);
	int size = fIn.tellg();
	++size;
	fIn.seekg(0, ios::beg);

	char* str = new char[size] {};

	fIn.getline(str, size, '\0');

	//��������� ����� �����
	fIn.close();

	vector<char*> words = findWords(str, size);//������ ����

	int nSize = words.size();

	outputW(words, nSize);

	//������������ ������ ��� ����� ������ ����
	for (int i = 0; i ^ nSize; ++i)
		delete[] words[i];
	delete[]str;

	return 0;
}