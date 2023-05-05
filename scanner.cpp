#include "scanner.h"

const char *
Scanner::TW    [] = {"", "and", "boolean", "do", "else", "if", "false", "int", "not", "or", "program", "read", "true", "while", "write", "string", NULL};
 
const char *
Scanner::TD    [] = {";", ",", "=", "(", ")", "{", "}", "==", "<", ">", "+", "-", "*", "/", "%", "\\*", "*\\", "<=", "!=", ">=", "+=", "-=", "\"", NULL};

const type_of_lex 
Scanner::words[] = {LEX_NULL, LEX_AND, LEX_BOOLEAN, LEX_DO, LEX_ELSE, LEX_IF, LEX_FALSE, LEX_INT, LEX_NOT, LEX_OR, LEX_PROGRAM, LEX_READ, LEX_TRUE, LEX_WHILE, LEX_WRITE, LEX_STR, LEX_NULL};

const type_of_lex
Scanner::dlms[] = {LEX_SEMICOLON, LEX_COMMA, LEX_ASSIGN, LEX_LPAREN, LEX_RPAREN, LEX_LBRACE, LEX_RBRACE, LEX_EQ, LEX_LSS, LEX_GTR, LEX_PLUS, LEX_MINUS, LEX_MULT, LEX_DIV, LEX_MOD, LEX_FRONTSLASH, LEX_BACKSLASH, LEX_LEQ, LEX_NEQ, LEX_GEQ, LEX_PLUSEQ, LEX_MINUSEQ, LEX_QUOTE, LEX_NULL}; 

int words_size = 18;
char Scanner::gc () // считывание символа из файла
{
    return (c = fgetc (fp));
}

void Scanner::clear() // каждый раз чистим буфер для нового слова
{
    buf_top = 0;
    for (int i = 0; i < 100; i++)
        buf[i] = '\0';
}

void Scanner::add() // добавление буквы
{
    buf[buf_top++] = c;
}

Scanner::Scanner(const char * program) // открываем файл на чтение
{
    fp = fopen(program, "r");
    CS = H; // тут лежит служебное слово
    clear();
}

int Scanner::look(const char * buf, const char **list) // перебираем массив TW/TD, ищем нужное слово
{
    int i = 0;
    while (list[i])
    {
        if (!strcmp(buf, list[i]))
            return i;
        i++;
    }
    return 0;
}

Lex Scanner::get_lex() // получение лексемы со всеми ее полями
{
    int d = 0, j;
    CS = H; // служебное слово в начале
    clear();
    do
    {
        c = gc();
        if(c == EOF)
        {
            return Lex(LEX_EOF, LEX_EOF);
        }
        switch(CS)
        {
            case H:
                if (c == ' ' || c == '\n' || c == '\r' || c == '\t')
                {
                    if(c == '\n')
                    {
                        count_str++;
                        count_col = 1;
                    }
                    if(c == ' ' || c == '\t')
                    {
                        count_col++;
                    }
                }
                else if (isalpha(c))
                {
                    add();
                    CS = IDENT;
                }
                else if (isdigit(c))
                {
                    d = c - '0';
                    CS = NUMB;
                }
                else if (c == '<' || c == '>' || c == '+' || c == '-' || c == '*' || c == '/' || c == '%')
                {
                    add();
                    CS = ALE;
                }
                else if(c == ';')
                {
                    add();
                    j = look(buf, TD);
                    clear();
                    count_col++;
                    return Lex(LEX_SEMICOLON, j);
                }
                else if(c == ',')
                {
                    add();
                    j = look(buf, TD);
                    clear();
                    count_col++;
                    return Lex(LEX_COMMA, j);
                }
                else if (c == '"')
                {
                    CS = QUOTE;
                }
                else if(c == '(')
                {
                    add();
                    j = look(buf, TD);
                    clear();
                    return Lex(LEX_LPAREN, j);
                }
                else if(c == ')')
                {
                    add();
                    j = look(buf, TD);
                    clear();
                    return Lex(LEX_RPAREN, j);
                }
                else if(c == '{')
                {
                    add();
                    j = look(buf, TD);
                    clear();
                    return Lex(LEX_LBRACE, j);
                }
                else if(c == '}')
                {
                    add();
                    j = look(buf, TD);
                    clear();
                    return Lex(LEX_RBRACE, j);
                }
                else if (c == '\\')
                {
                    try
                    {
                        c = gc();
                        if(c != '*') 
                            throw c;
                        char c0 = gc();
                        c = gc();
                        do 
                        {
                            if ((c0 == '*') && (c == '\\'))
                                break;
                            c0 = c;
                            c = gc();
                            
                        }
                        while (c != EOF);
                        
                        if ( c == EOF)
                        {
                            throw "Комментарий закончен";
                        }
                        cout << endl;
                        CS = H;
                        break;
                    }
                    catch(char c)
                    {
                        cout << "\x1b[37;41m" << "Строка: " << count_str << " Номер символа: " << count_col << " Неизвестный символ" << c << "Неверное описание комментария" << "\x1b[0m" << endl;
                        err_scaner = true;
                    }
                    catch( const char* c)
                    {
                        cout << "\x1b[37;41m" << c << "\x1b[0m" << endl;
                        err_scaner = true;
                    }

                }
                else if (c == '!')
                {
                    add();
                    CS = NEQ;
                }
                else if (c == '=')
                {
                    add();
                    CS = EQEQ;
                }
                else
                {
                    try
                    {
                        add();
                        throw c;
                    }
                    catch(char c)
                    {
                        cout << "\x1b[37;41m" << "Строка: " << count_str << " Номер символа: " << count_col << " Неизвестный символ: " << c << "\x1b[0m" << endl;
                        err_scaner = true;
                    }
                    catch( const char *c)
                    {
                        cout << "\x1b[37;41m" << c << "\x1b[0m" << endl;
                        err_scaner = true;
                    }
                }
                break;
            case IDENT:
                if (isalpha(c) || isdigit(c))
                    add();
                else
                {
                    if ((j = look(buf, TW)) > 0)
                    {
                        ungetc(c, fp);
                        clear();
                        count_col++;
                        return Lex(words[j], j);
                    }
                    else
                    {
                        try
                        {
                            if(!isalpha(c) && !isdigit(c) && (j = look(&c, TW) < 0))
                            {
                                add();
                                throw c;
                            }
                        }
                        catch(char c)
                        {
                            cout << "\x1b[37;41m" << "Строка: " << count_str << " Номер символа: " << count_col << " Неизвестный символ: " << c << "\x1b[0m" << endl;
                            err_scaner = true;
                        }
                        catch( const char *c)
                        {
                            cout << "\x1b[37;41m" << c << "\x1b[0m" << endl;
                            err_scaner = true;
                        }
                        j = TID.put(buf);
                        TID[j].put_value(j);
                        ungetc(c, fp);
                        clear();
                        count_col++;
                        return Lex(LEX_ID, j);
                    }
                }
                break;
            case NUMB:
                if (isdigit(c))
                    d = d * 10 + (c - '0'); // перевод числа из char в int
                else
                {
                    ungetc(c, fp); // нужно, чтобы не пропустить запятую
                    clear();
                    return Lex(LEX_NUM, d);
                }
                break;
            case ALE:
                if (c == '=')
                {
                    add();
                    j = look(buf, TD);
                    clear();
                    count_col++;
                    return Lex(dlms[j], j);
                }
                else 
                {
                    j = look(buf, TD);
                    ungetc(c, fp);
                    clear();
                    count_col++;
                    return Lex(dlms[j], j);
                }
            case NEQ:
                if (c == '=')
                {
                    add();
                    j = look(buf, TD);
                    clear();
                    count_col++;
                    return Lex(LEX_NEQ, j);
                }
                else
                    cout << "\x1b[37;41m" << "Ошибка с !=" << "\x1b[0m" << endl;
                break;
            case EQEQ:
            {

                if (c == '=')
                {
                    add();
                    j = look(buf, TD);
                    clear();
                    count_col++;
                    return Lex(LEX_EQ, j);
                }
                else
                {
                    j = look(buf, TD);
                    clear();
                    ungetc(c, fp);
                    return Lex(LEX_ASSIGN, j);
                }
                break;
            }
            case QUOTE:
            {
                while (c != '"' && c != ';' && c != ')')
                {   
                    add();
                    c = gc();
                }
                c = gc();
                if(c != ' ' && c != ',' && c != ')' && c != ';' && !isalpha(c) && !isdigit(c))
                {
                    ungetc(c, fp);
                    throw c;
                }
                else
                {
                    j = TSTR.put(buf);
                    ungetc(c, fp);
                    clear();
                    return Lex(LEX_STR, j);
                }
            }
        }
    }
    while(1);
};
