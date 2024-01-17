//не нужно смотреть на разделители, когда их вводит пользователь (выход за границы массива в проверке в 43 строчке)
//буква Є и ® потер€лись

#include <iostream>
#include <fstream>
#include <vector>
#include <set>
using namespace std;

//функци€ обнаружени€ разделителей '\t' и '\n' 
bool chSep(char a, char b) {
	if ((a == 92) && (b == 't' || b == 'n'))
		return true;
	return false;
}

vector<char*> findWords(char* str, int size) {
	int i, k, st;
	string c;

	//множество цифр
	set <char> nums;
	for (i = 48; i ^ 58; nums.insert(i++));

	//множество русских букв
	set <char> letters;
	for (char i = 192; i; letters.insert(i++));

	//множество специальных символов
	set <char> specCh;
	specCh.insert(' ');
	specCh.insert('\t');
	specCh.insert('\n');
	specCh.insert('\0');

	vector<char*> words;//найденые слова

	char* word = new char[size] {};//обрабатываееое корректное слово, удовлетвор€ющее условие

	for (i = st = k = 0; i ^ size; ++i)
	{
		//проверка на наличие разделителей и соответсвенные действи€
		if (chSep(str[i], str[i + 1])) {
			if (word[0]) st = 3;
			else {
				st = 4;
				str[i + 1] = ' ';
			}

			continue;
		}

		switch (st) {
			// состо€ние поиска первого верного символа (только цифры)
		case 0:
			c = str[i];
			if (nums.find(str[i]) != nums.end()) {
				word[k++] = str[i];
				(specCh.find(str[i + 1]) != specCh.end()) ? st = 3 : st = 1;//проверка на то, что слово состоит из одной цифры
			}
			else if ((specCh.find(str[i]) == specCh.end())) st = 4;
			break;
			// состо€ние чтение русских символов и цифр 
		case 1:
			c = str[i];
			if (letters.find(str[i]) != letters.end())
				word[k++] = str[i];
			else if ((nums.find(str[i]) != nums.end())) {//проверка на завершение слова
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
			//состо€не проверки заверешени€ слова
		case 2:
			c = str[i];
			if (specCh.find(str[i + 1]) != specCh.end()) {//провер€ем, введен ли спец символ (' ', '\n', '\t')
				word[k++] = str[i];
				st = 3;
			}
			else if ((nums.find(str[i]) != nums.end()) || (letters.find(str[i]) != letters.end())) {//если слово не закончилось, продалжаем его считывани€, возвраща€сь в состо€ние 1
				word[k++] = str[i];
				st = 1;
			}
			else {//если же встречачетс€ любой другой символ, отличный от удовлетвор€щих (цифр и русских букв), то переводим автомат в допускающее состо€ние 4
				k = 0;
				st = 4;
				delete[]word;
				word = new char[size] {};
			}
			break;
			//заключительное состо€ние
		case 3:
			words.push_back(word);
			word = new char[size] {};
			k = st = 0;
			break;
			//допускающее состо€ние, когда входной символ отличный от тех, которые удовлетвор€ют условию
		case 4:
			c = str[i];
			if (specCh.find(str[i]) != specCh.end())  st = 0;
			break;
		}
	}

	//освобождение пам€ти дл€ последенего слова
	delete[]word;

	return words;
}

//функци€ записи слов в файл и вывод в консоль
void outputW(vector<char*> words, int size) {
	//открываем файловый поток вывода
	ofstream fOut("output.txt", ios_base::trunc);

	for (int i = 0; i < size;++i) {
		cout << words[i] << ' ';
		fOut << words[i] << ' ';

	}

	//закрываем поток вывода
	fOut.close();
}

int main()
{
	setlocale(LC_ALL, "Russian");

	//открываем файловый поток вывода
	ifstream fIn("input.txt");

	//узнаем количество читаемых символов в файле и возвращаемс€ обратно в начало файла
	fIn.seekg(0, ios::end);
	int size = fIn.tellg();
	++size;
	fIn.seekg(0, ios::beg);

	char* str = new char[size] {};

	fIn.getline(str, size, '\0');

	//закрываем поток ввода
	fIn.close();

	vector<char*> words = findWords(str, size);//список слов

	int nSize = words.size();

	outputW(words, nSize);

	//освобождение пам€ти дл€ всего списка слов
	for (int i = 0; i ^ nSize; ++i)
		delete[] words[i];
	delete[]str;

	return 0;
}