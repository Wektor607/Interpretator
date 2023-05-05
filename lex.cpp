#include "lex.h"

Lex::Lex()
{
	t_lex = LEX_NULL;
	v_lex = 0;
}

Lex::Lex(type_of_lex t)
{
	t_lex = t;
	v_lex = 0;
}

Lex::Lex(type_of_lex t, int v)
{
	t_lex = t;
	v_lex = v;
}

Lex::Lex(type_of_lex t, char * tmp)
{
	t_lex = t;
	str_lex = new char[1000];
	str_lex = strcpy(str_lex, tmp);
}

type_of_lex Lex::get_type()
{
	return t_lex;
}

char * Lex::get_str()
{
	return str_lex;
}

int Lex::get_value()
{
	return v_lex;
}

std::ostream& operator << (std::ostream &s, Lex l)
{
	return s;
}