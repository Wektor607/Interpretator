#include <iostream>
#include <string>
#include <stack>
#include <stdlib.h>
#include <cstring>
#include <fstream>
#include "interpretator.h"


using namespace std;

int main(int argc, const char * argv[])
{
	char str[100];
	if (argc == 2)
	{
		strcpy(str, argv[1]);
		strcat(str,".txt");
		ifstream file(str);
		if(!file)
		{
			cout << "\x1b[37;41m" << "Проблема с файлом" << "\x1b[0m" << endl;
			return 0;
		}

		Interpretator Model(str);
		Model.interpretation(); // вызов компилятора
	}
	else
		cout << "Проблема с файлом" << endl;
	return 0;
}
