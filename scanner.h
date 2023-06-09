#pragma once
//#include "lex.h"
//#include "type_of_lex.h"
#include "lex.h"
//#include "table_label.h"
//#include "table_ident.h"
#include "table_str.h"
using namespace std;

extern table_ident TID;
//extern const char * translator[];
extern table_str TSTR;
class Scanner 
{
	enum state {H, IDENT, NUMB, ALE, NEQ, EQEQ, QUOTE};
	static const char * TW[];
	static const type_of_lex words[];
	static const char * TD[];
	static const type_of_lex dlms[];

	state CS;
	FILE *fp;
	char c;
	int buf_top;

	void clear();
	void add();
	int look(const char *buf, const char **list);
public:
	char buf[100];
	char gc();
	int count_str = 1; // счётчик строк 
	int count_col = 1; // счётчик столбцов
	bool err_scaner = false;

	Lex get_lex();
	Scanner (const char* program);
};