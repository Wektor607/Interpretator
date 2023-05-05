#include "parser.h"

template <class T, class T_EL>
void from_st(T & st, T_EL & i)
{
    i = st.top(); // взятие верхнего элемента стека
    st.pop();
}

void Parser::gl() // распознаем, что за лексема была подана и получаем
{
    curr_lex = scan.get_lex();    // тип данных лексема, которая содержит всю информацию
    cout << curr_lex;
    c_type = curr_lex.get_type(); // тип символа (LEX_....)
    c_val = curr_lex.get_value(); // номер слова в таблице TID (массив имен переменных)
    c_str = curr_lex.get_str();   // значение(название) лексемы
}
Parser::Parser (const char *program): scan(program), prog(1000)  {}

void Parser::analyze() // проверка поданной программы на ошибки
{
    gl();
    Prog();
    if((err_parser == false) && (scan.err_scaner == false))
    {
        err_parser = (err_parser && scan.err_scaner);
    }
}

void Parser::Prog()
{
    try
    {
        if (c_type == LEX_PROGRAM)
            gl();
        else
            throw curr_lex;
        
        if(c_type == LEX_LBRACE)
        {
            gl();
            D1(); // объявление переменных разного типа
            B();
        }
        else
            throw curr_lex;
        if(c_type != LEX_RBRACE)
            throw curr_lex;
        else
        {
            gl();
            if(c_type != LEX_EOF)
            {
                throw " Программа завершила работу с ошибками ";
            }
        }
        if(TL.check_TL() == false)
        {
            throw " Некоторые переменные не были идентифицированы ";
        }
    }
    catch(Lex l)
    {
        cout << "\x1b[37;41m" << "Строка: " << scan.count_str << " Номер символа: " << scan.count_col << " Неизвестная лексема" << l << "Ошибка описания программы" << "\x1b[0m" << endl;
        err_parser = true;
    }
    catch(char *l)
    {
        cout << "\x1b[37;41m" << "Строка: " << scan.count_str << " Номер символа: " << scan.count_col << l << "\x1b[0m" << endl;
        err_parser = true;
    }
    catch(const char *l)
    {
        cout << "\x1b[37;41m" << "Строка: " << scan.count_str << " Номер символа: " << scan.count_col << l << "\x1b[0m" << endl;
        err_parser = true;
    }
}

void Parser::D1() // поле переменных
{
    try
    {
        while ((c_type == LEX_INT) || (c_type == LEX_BOOLEAN) || (c_type == LEX_STR))
        {    
            D(); 
            if(c_type != LEX_SEMICOLON)
                throw curr_lex;
            prog.push_back(Lex(LEX_SEMICOLON, LEX_SEMICOLON));
            gl();
        }
    }
    catch(Lex l)
    {
        cout << "\x1b[37;41m" << "Строка: " << scan.count_str << " Номер символа: " << scan.count_col << " Неизвестная лексема" << l << "Ошибка инициализации" << "\x1b[0m" << endl;
        err_parser = true;
        gl();
    }
    catch(const char *l)
    {
        cout << "\x1b[37;41m" << "Строка: " << scan.count_str << " Номер символа: " << scan.count_col << l << "\x1b[0m" << endl;
        err_parser = true;
        gl();
    }
}

void Parser::D()
{
    type_of_lex tmp = c_type;
    if (c_type == LEX_INT || c_type == LEX_BOOLEAN || c_type == LEX_STR)
    {
        gl();
        st_lex.push(tmp);
        if(c_type == LEX_ID)
        {
            TID[c_val].put_declare();
            TID[c_val].put_type(tmp);
        }
        Perem();
        while (c_type == LEX_COMMA)
        {
            gl();
            st_lex.push(tmp);
            try
            {
                if(c_type == LEX_ID)
                {
                    if (TID[c_val].get_declare() == true)
                        throw " Повторная инициализация ";
                    TID[c_val].put_declare();
                    TID[c_val].put_type(tmp);
                }
            }
            catch(const char *l)
            {   
                cout << "\x1b[37;41m" << "Строка: " << scan.count_str << " Номер символа: " << scan.count_col << l << "\x1b[0m" << endl;
                err_parser = true;
                while (c_type != LEX_COMMA)
                    gl();
            }
            Perem();
        }
    }
    else
        throw curr_lex;
}

void Parser::Perem()
{
    
    int tmp_c_val = c_val;
    if (c_type == LEX_ID)
    {
        st_int.push(c_val);
        gl();
        if (c_type == LEX_ASSIGN)
        {
            st_lex.push(TID[tmp_c_val].get_type());
            st_lex.push(LEX_ID);
            prog.push_back(Lex(POLIZ_ADRESS, tmp_c_val));
            tmp_c_val = c_val;
            gl();
            Const();
            prog.push_back(Lex(LEX_ASSIGN, LEX_ASSIGN));
        }
    }
    else 
    {
        if (c_type == LEX_COMMA || c_type == LEX_SEMICOLON){}
        else
            throw curr_lex;
    }
}

void Parser::Const() 
{
    if(c_type == LEX_STR)
    {
        st_lex.push(LEX_STR);
        eq_type();
        prog.push_back(curr_lex);
        gl();
    }
    else 
    if (c_type == LEX_TRUE)
    {
        st_lex.push(LEX_BOOLEAN);
        eq_type();
        prog.push_back(Lex(LEX_TRUE, LEX_TRUE));
        gl();
    }
    else 
    if (c_type == LEX_FALSE)
    {
        st_lex.push(LEX_BOOLEAN);
        eq_type();
        prog.push_back(Lex(LEX_FALSE, LEX_FALSE));
        gl();
    }
    else 
    if (c_type == LEX_NUM)
    {
        st_lex.push(LEX_INT);
        eq_type();
        prog.push_back(curr_lex);
        gl();
    }
    else 
        throw curr_lex;
}

void Parser::B() 
{
    while(c_type != LEX_RBRACE && c_type != LEX_EOF)
    {
        S();
    }
}

void Parser::S()
{
    try
    {
        int pl0,pl1,pl2,pl3;
        if (c_type == LEX_RBRACE)
        {

        }
        else if(c_type == LEX_IF)
        {
            try
            {
                gl();
                Eor();
                eq_bool();
                pl2 = prog.get_free();
                prog.blank();
                prog.push_back((Lex(POLIZ_FGO, POLIZ_FGO)));
                if(c_type == LEX_LBRACE)
                {
                    gl();
                    S();
                    while (c_type == LEX_SEMICOLON)
                    {
                        prog.push_back(Lex(LEX_SEMICOLON, LEX_SEMICOLON));
                        gl();
                        S();
                    }
                    pl3 = prog.get_free();
                    if (c_type != LEX_RBRACE)
                    {
                        throw curr_lex;
                    }

                    gl();
                    if(c_type == LEX_ELSE)
                    {
                        gl();
                        if(c_type == LEX_LBRACE)
                        {
                            prog.blank();
                            prog.push_back(Lex(POLIZ_GO, POLIZ_GO));
                            prog.push_back(Lex(POLIZ_LABEL, prog.get_free()), pl2);
                            gl();
                            S();
                            while (c_type == LEX_SEMICOLON)
                            {
                                prog.push_back(Lex(LEX_SEMICOLON, LEX_SEMICOLON));
                                gl();
                                S();
                            }
                            prog.push_back(Lex(POLIZ_LABEL, prog.get_free()), pl3);
                        }
                        if (c_type != LEX_RBRACE)
                        {
                            throw curr_lex;
                        }
                        gl();
                    }
                    else 
                    {
                        prog.push_back(Lex(POLIZ_LABEL, prog.get_free()), pl2);
                        S();
                    }
                }
                else throw curr_lex;
            }
            catch(Lex l)
            {
                cout << "\x1b[37;41m" << "Строка: " << scan.count_str << " Номер символа: " << scan.count_col << " Неизвестная лексема " << l << " Неверное описание условного оператора " << "\x1b[0m" << endl;
                err_parser = true;
                gl();
            }
            catch(const char *l)
            {
                cout << "\x1b[37;41m" << "Строка: " << scan.count_str << " Номер символа: " << scan.count_col << l << "\x1b[0m" << endl;
                err_parser = true;
                gl();
            }
            catch(char c)
            {
                cout << "\x1b[37;41m" << "Строка: " << scan.count_str << " Номер символа: " << scan.count_col << " Неизвестный символ " << c << " Неверное описание условного оператора " << "\x1b[0m" << endl;
                err_parser = true;
                gl();
            }
        }
        else if(c_type == LEX_WHILE)
        {
            try
            {
                pl0 = prog.get_free();
                gl();
                Eor();
                eq_bool();
                pl1 = prog.get_free();
                prog.blank();
                prog.push_back(Lex(POLIZ_FGO, POLIZ_FGO));
                if (c_type == LEX_LBRACE)
                {
                    gl();
                    S();
                    while (c_type == LEX_SEMICOLON)
                    {
                        prog.push_back(Lex(LEX_SEMICOLON, LEX_SEMICOLON));
                        gl();
                        S();
                    }
                    prog.push_back(Lex(POLIZ_LABEL, pl0));
                    prog.push_back(Lex(POLIZ_GO, POLIZ_GO));
                    prog.push_back(Lex(POLIZ_LABEL, prog.get_free()), pl1);
                    if (c_type != LEX_RBRACE)
                        throw curr_lex;
                    gl();
                    S();
                }
                else 
                    throw curr_lex;
            }
            catch(Lex l)
            {
                cout << "\x1b[37;41m" << "Строка: " << scan.count_str << " Номер символа: " << scan.count_col << " Неизвестная лексема " << l << " Неверное описание цикла WHILE " << "\x1b[0m" << endl;
                err_parser = true;
                gl();
            }
            catch(const char *l)
            {
                cout << "\x1b[37;41m" << "Строка: " << scan.count_str << " Номер символа: " << scan.count_col << l << "\x1b[0m" << endl;
                err_parser = true;
                gl();
            }
            catch(char c)
            {
                cout << "\x1b[37;41m" << "Строка: " << scan.count_str << " Номер символа: " << scan.count_col << " Неизвестный символ " << c << " Неверное описание цикла WHILE " << "\x1b[0m" << endl;
                err_parser = true;
                gl();
            }
        }
        else if (c_type == LEX_DO)
        {
            try
            {
                gl();
                pl0 = prog.get_free();
                if (c_type == LEX_LBRACE)
                {
                    gl();
                    S();
                    while (c_type == LEX_SEMICOLON)
                    {
                        prog.push_back(Lex(LEX_SEMICOLON, LEX_SEMICOLON));
                        gl();
                        S();
                    }
                    
                    if (c_type == LEX_RBRACE)
                    {
                        gl();
                    }
                    else
                    {
                        throw curr_lex;

                    }
                }
                else 
                    throw curr_lex;
                if (c_type == LEX_WHILE)
                {
                    gl();
                    if (c_type == LEX_LPAREN)
                    {    
                        Eor();
                        eq_bool();
                        pl1 = prog.get_free();
                        prog.push_back(Lex(POLIZ_LABEL, pl1 + 4));
                        prog.push_back(Lex(POLIZ_FGO, POLIZ_FGO));
                        prog.push_back(Lex(POLIZ_LABEL, pl0));
                        prog.push_back(Lex(POLIZ_GO, POLIZ_GO));   
                        if (c_type != LEX_SEMICOLON)
                            throw curr_lex;
                        prog.push_back(Lex(LEX_SEMICOLON, LEX_SEMICOLON));
                        gl();
                        S();
                    }
                    else
                    {
                        throw curr_lex;
                    }
                }
                else
                {
                    throw curr_lex;
                } 
            }
            catch(Lex l)
            {
                cout << "\x1b[37;41m" << "Строка: " << scan.count_str << " Номер символа: " << scan.count_col << " Неизвестная лексема " << l << " Неверное описание цикла DO WHILE " << "\x1b[0m" << endl;
                err_parser = true;
                gl();
            }
            catch(const char *l)
            {
                cout << "\x1b[37;41m" << "Строка: " << scan.count_str << " Номер символа: " << scan.count_col << l << "\x1b[0m" << endl;
                err_parser = true;
                gl();
            }
            catch(char c)
            {
                cout << "\x1b[37;41m" << "Строка: " << scan.count_str << " Номер символа: " << scan.count_col << " Неизвестный символ " << c << " Неверное описание цикла DO WHILE " << "\x1b[0m" << endl;
                err_parser = true;
                gl();
            }
        }
        else if(c_type == LEX_READ)
        {
            try
            {
                gl();
                if(c_type == LEX_LPAREN)
                {
                    gl();
                    if(c_type == LEX_ID)
                    {
                        check_id_in_read();
                        prog.push_back(Lex(POLIZ_ADRESS, c_val));
                        gl();
                    }
                    else 
                        throw curr_lex;
                    if(c_type == LEX_RPAREN)
                    {
                        gl();
                        prog.push_back(Lex(LEX_READ, LEX_READ));
                    }
                    else
                        throw curr_lex;
                }
                else 
                    throw curr_lex;
            }
            catch(Lex l)
            {
                cout << "\x1b[37;41m" << "Строка: " << scan.count_str << " Номер символа: " << scan.count_col << " Неизвестная лексема " << l << " Неверное описание ввода " << "\x1b[0m" << endl;
                err_parser = true;
                gl();
            }
            catch(const char *l)
            {
                cout << "\x1b[37;41m" << "Строка: " << scan.count_str << " Номер символа: " << scan.count_col << l << "\x1b[0m" << endl;
                err_parser = true;
                gl();
            }
            catch(char c)
            {
                cout << "\x1b[37;41m" << "Строка: " << scan.count_str << " Номер символа: " << scan.count_col << " Неизвестный символ " << c << " Неверное описание ввода " << "\x1b[0m" << endl;
                err_parser = true;
                gl();
            }
        }
        else if(c_type == LEX_WRITE)
        {   
            try
            {
                gl();
                if(c_type == LEX_LPAREN)
                {
                    gl();
                    Eor();
                    while(c_type == LEX_COMMA)
                    {
                        gl();
                        Eor();
                    }
                    if (c_type == LEX_RPAREN)
                    {
                        prog.push_back(Lex(LEX_WRITE, LEX_WRITE));
                        gl();
                    }
                    else 
                        throw curr_lex;
                }
                else 
                    throw curr_lex;
            }
            catch(Lex l)
            {
                cout << "\x1b[37;41m" << "Строка: " << scan.count_str << " Номер символа: " << scan.count_col << " Неизвестная лексема " << l << " \n Неверное описание для вывода " << "\x1b[0m" << endl;
                err_parser = true;
                while (c_type != LEX_QUOTE && c_type != LEX_SEMICOLON && c_type != EOF && c_type != LEX_RPAREN)
                {
                    gl();
                }
            }
            catch(const char *l)
            {
                cout << "\x1b[37;41m" << "Строка: " << scan.count_str << " Номер символа: " << scan.count_col << l << "\x1b[0m" << endl;
                err_parser = true;
                while (c_type != LEX_QUOTE && c_type != LEX_SEMICOLON && c_type != EOF)
                {
                    gl();
                }
            }
            catch(char c)
            {
                cout << "\x1b[37;41m" << "Строка: " << scan.count_str << " Номер символа: " << scan.count_col << " Неизвестный символ " << c << " \n Неверное описание для вывода " << "\x1b[0m" << endl;
                err_parser = true;
                while (c != '\"' && c != ';' && c != EOF && c != ')')
                {
                    c = scan.gc();
                }
            }
        }
        else if(c_type == LEX_ID)
        {
            try
            {
                if(TID[c_val].get_declare() == false)
                {
                    throw " Не объявлено ";
                }
                type_of_lex tmp = TID[c_val].get_type();
                st_lex.push(tmp);
                st_lex.push(LEX_ID);
                prog.push_back(Lex(POLIZ_ADRESS, c_val));

                gl();
                if (c_type == LEX_ASSIGN)
                {
                    gl();
                    Eor();
                    eq_type();
                    prog.push_back(Lex(LEX_ASSIGN, LEX_ASSIGN));
                }
                else 
                if(c_type == LEX_PLUSEQ)
                {
                    //int tmp_c_val = c_val;
                    if (tmp == LEX_BOOLEAN) 
                        throw " Операция сложения неприменима к логическим выражениям";
                    if(tmp != LEX_STR)
                    {
                        gl();
                        Eor();
                        eq_type();
                        prog.push_back(Lex(LEX_PLUSEQ, LEX_PLUSEQ));
                    }
                    else
                        throw " Операция сложения неприменима к строкам ";
                }
                else
                if(c_type == LEX_MINUSEQ)
                {
                    if(tmp == LEX_BOOLEAN) 
                        throw " Операция вычитания неприменима к логическим выражениям";
                    if(tmp != LEX_STR)
                    {
                        gl();
                        Eor();
                        eq_type();
                        prog.push_back(Lex(LEX_MINUSEQ, LEX_MINUSEQ));
                    }
                    else
                        throw " Операция вычитания неприменима к строкам ";
                }
                else
                    throw curr_lex;
            }
            catch(Lex l)
            {
                cout << "\x1b[37;41m" << "Строка: " << scan.count_str << " Номер символа: " << scan.count_col << " Неизвестная лексема " << l << " Ошибка в описании оператора присваивание " << "\x1b[0m" << endl;
                err_parser = true;
                 
                gl();
            }
            catch(const char *l)
            {
                cout << "\x1b[37;41m" << "Строка: " << scan.count_str << " Номер символа: " << scan.count_col << l << "\x1b[0m" << endl;
                err_parser = true;
                 while (c_type != LEX_SEMICOLON)
                    {
                        gl();
                    }
            }
            catch(char c)
            {
                cout << "\x1b[37;41m" << "Строка: " << scan.count_str << " Номер символа: " << scan.count_col << " Неизвестный символ " << c << " Ошибка в описании оператора присваивание " << "\x1b[0m" << endl;
                err_parser = true;
                gl();
            }
        }
        else if (c_type == LEX_SEMICOLON)
        {
            prog.push_back(Lex(LEX_SEMICOLON, LEX_SEMICOLON));
            gl();
        }
        else if(c_type == LEX_MINUS)
        {
            try
            {
                gl();
                if(c_type == LEX_ID)
                {
                    check_id();
                    if(TID[c_val].get_type() != LEX_INT)
                    {
                        throw " Вычитание: Ошибка в операции вычитания ";
                    }
                    Eor();
                    eq_type();
                    gl();
                }
            }
            catch(const char *l)
            {
                cout << "\x1b[37;41m" << "Строка: " << scan.count_str << " Номер символа: " << scan.count_col << " Неизвестный символ " << l << "\x1b[0m" << endl;
                err_parser = true;
                gl();
            }
        }
        else if(c_type == LEX_PLUS)
        {
            try
            {
                gl();
                if(c_type == LEX_ID)
                {
                    check_id();
                    if(TID[c_val].get_type() != LEX_INT)
                    {
                        throw " Вычитание: Ошибка в операции вычитания ";
                    }
                    Eor();
                    eq_type();
                    gl();
                }
            }
            catch(const char *l)
            {
                cout << "\x1b[37;41m" << "Строка: " << scan.count_str << " Номер символа: " << scan.count_col << " Неизвестный символ " << l << "\x1b[0m" << endl;
                err_parser = true;
                gl();
            }
        }
        else 
            throw curr_lex;
    }
    catch(Lex l)
    {
        cout << "\x1b[37;41m" << "Строка: " << scan.count_str << " Номер символа: " << scan.count_col << " Неизвестная лексема " << l << " Неверное описание выражения " << "\x1b[0m" << endl;
        err_parser = true;
        gl();
        while((c_type != LEX_SEMICOLON) && (c_type != LEX_RBRACE) && (c_type != LEX_EOF))
        {
            gl();
        }
    }
    catch(const char *l)
    {
        cout << "\x1b[37;41m" << "Строка: " << scan.count_str << " Номер символа: " << scan.count_col << l << "\x1b[0m" << endl;
        err_parser = true;
        gl();
        while((c_type != LEX_SEMICOLON) && (c_type != LEX_RBRACE) && (c_type != LEX_EOF))
        {
            gl();
        }
    }
    catch(char c)
    {
        cout << "\x1b[37;41m" << "Строка: " << scan.count_str << " Номер символа: " << scan.count_col << " Неизвестный символ " << c << " Неверное описание выражения " << "\x1b[0m" << endl;
        err_parser = true;
        gl();
        while((c_type != LEX_SEMICOLON) && (c_type != LEX_RBRACE) && (c_type != LEX_EOF))
        {
            gl();
        }
    }
}

void Parser::Eor()
{
    Eand();
    while(c_type == LEX_OR)
    {
        st_lex.push(c_type);
        gl(); 
        if (c_type == LEX_ID)
        {
            if(TID[c_val].get_declare() == false)
            {
                throw " Не объявлено ";
            }
        }
        Eor(); 
        check_op();
    }
}

void Parser::Eand()
{
    E();
    while(c_type == LEX_AND)
    {
        st_lex.push(c_type);
        gl(); 
        if (c_type == LEX_ID)
        {
            if(TID[c_val].get_declare() == false)
            {
                throw " Не объявлено ";
            }
        }
        E(); 
        check_op();
    }
}

void Parser::E()
{
    E1();
    if(c_type == LEX_EQ || c_type == LEX_LSS || c_type == LEX_GTR || 
        c_type == LEX_LEQ || c_type == LEX_GEQ || c_type == LEX_NEQ || c_type == LEX_ASSIGN || c_type == LEX_PLUSEQ || c_type == LEX_MINUSEQ) 
    {
        st_lex.push(c_type);
        gl(); 
        if (c_type == LEX_ID)
        {
            if(TID[c_val].get_declare() == false)
            {
                throw " Не объявлено ";
            }
        }
        E1();
        check_op();
    }
}

void Parser::E1()
{
    type_of_lex tmp;
    T();
    while (c_type == LEX_PLUS || c_type == LEX_MINUS || c_type == LEX_OR)
    {
        st_lex.push(c_type);
        gl();
        if (c_type == LEX_MINUS || c_type == LEX_PLUS)
        {
            tmp = c_type;
            gl();
            if (c_type == LEX_ID)
            {
                if(TID[c_val].get_declare() == false)
                {
                    throw " Не объявлено ";
                }

                if(TID[c_val].get_type() == LEX_INT)
                {
                    prog.push_back(Lex(LEX_NUM, 0));
                    prog.push_back(Lex(LEX_ID, c_val));
                    st_lex.push(c_type);
                    prog.push_back(Lex(tmp, 0));
                }
            }
            else if(c_type == LEX_NUM)
            {
                prog.push_back(Lex(LEX_NUM, 0));
                F();
                prog.push_back(Lex(tmp, 0));
            }
            else
            {
                throw curr_lex;
            }
        }
        else
        {
            if (c_type == LEX_ID)
            {
                if(TID[c_val].get_declare() == false)
                {
                    throw " Не объявлено ";
                }
            }
            T();
        }
        check_op();
    }
}

void Parser::T()
{
    F();
    while (c_type == LEX_MULT || c_type == LEX_DIV || c_type == LEX_AND || c_type == LEX_MOD)
    {
        st_lex.push(c_type);
        gl();
        if (c_type == LEX_ID)
        {
            if(TID[c_val].get_declare() == false)
            {
                throw " Не объявлено ";
            }
        }
        F();
        check_op(); //правильный ли результат выражения 
    }
}


void Parser::F()
{
    if (c_type == LEX_ID)
    {
        if(TID[c_val].get_declare() == false)
        {
            throw " Не объявлено ";
        }
        type_of_lex tmp = TID[c_val].get_type();
        st_lex.push(tmp);
        prog.push_back(Lex(curr_lex));
        gl();
    }
    else if (c_type == LEX_STR)
    {        
        st_lex.push(LEX_STR);
        prog.push_back(curr_lex);;
        gl();
    }
    else if (c_type == LEX_NUM)
    {
        st_lex.push(LEX_INT);
        prog.push_back(curr_lex);
        gl();
    }
    else if (c_type == LEX_TRUE)
    {
        st_lex.push(LEX_BOOLEAN);
        prog.push_back(Lex(LEX_TRUE, LEX_TRUE));
        gl();
    }
    else if (c_type == LEX_FALSE)
    {
        st_lex.push(LEX_BOOLEAN);
        prog.push_back(Lex(LEX_FALSE, LEX_FALSE));
        gl(); 
    }
    else if (c_type == LEX_NOT)
    {
        gl();
        F();
        check_not();
    }
    else if (c_type == LEX_LPAREN)
    {
        gl();
        E();
        if (c_type == LEX_RPAREN)
            gl();
        else 
        {
            throw curr_lex;
        }
    }
    else
        throw curr_lex;
}

void Parser::check_declare(type_of_lex type) // проверка, что переменная объявлена
{
    int i;
    while (!st_int.empty()) 
    {
        from_st(st_int, i);
        if (TID[i].get_declare())
            throw curr_lex;
        else
        {
            TID[i].put_declare();
            TID[i].put_type(type);
        }
    }
}

void Parser::check_id() // проверка объявлены переменные или нет. Есть ли в таблицах
{
    if (TID[c_val].get_declare() == true)
        st_lex.push(TID[c_val].get_type());
    else  
        throw "Не объявлено";
}

void Parser::check_op() // проверка реализуемости операторов между двумя выражениями
{
    type_of_lex t1, t2, op, result = LEX_BOOLEAN; 
    from_st(st_lex, t2);
    from_st(st_lex, op);
    from_st(st_lex, t1);
    if (t1 == t2)
    {
        if (t1 == LEX_INT)
        {
            if (op == LEX_PLUS || op == LEX_MINUS || op == LEX_MULT || op == LEX_DIV || op == LEX_MOD)
                result = LEX_INT;
            else if (op == LEX_AND || op == LEX_OR) 
            {
                throw " Недопустимый тип операторов ";
            }
            else
                result = LEX_BOOLEAN;
        }
        if (t1 == LEX_BOOLEAN)
        {
            if (op == LEX_AND || op == LEX_OR || op == LEX_EQ || op == LEX_NEQ)
                result = LEX_BOOLEAN;
            else 
                throw " Недопустимый тип операторов ";
        }
        if (t1 == LEX_STR)
        {
            if (op == LEX_PLUS)
                result = LEX_STR;
            else if (op == LEX_EQ || op == LEX_NEQ || op == LEX_GTR || op == LEX_LSS || op == LEX_GEQ || op == LEX_LEQ)
                result = LEX_BOOLEAN;
            else 
                throw " Недопустимый тип операторов ";
        }
        st_lex.push(result);
        if (op >= words_size)
        {
            prog.push_back(Lex(op, op % words_size + 1));
        }
        else 
            prog.push_back(Lex(op, op));
    }
    else
    {
        throw " Недопустимый тип операторов ";
    }
}

void Parser::check_not() // проверка, что первое выражение not 
{
    if (st_lex.top() != LEX_BOOLEAN)
        throw "Недопустимый тип в операторе отрицания";
    else 
        prog.push_back(Lex(LEX_NOT, LEX_NOT));
}

void Parser::eq_type() // проверка присваивания и равенства (выражение = выражение)
{
    type_of_lex t = st_lex.top();
    st_lex.pop();
    if(st_lex.top() != LEX_ID)
        throw "Неверный тип в :=";
    st_lex.pop();
    if (t != st_lex.top())
        throw "Неверный тип в :=";
    st_lex.pop();
}

void Parser::eq_bool() // проверка что вверху стека находится условный оператор или оператор цикла
{
    if (st_lex.top() != LEX_BOOLEAN)
        throw "Выражение не логическое";
    st_lex.pop();
}

void Parser::check_id_in_read() // проверка что переменная объявлена => ее можно считывать
{
    if (!TID[c_val].get_declare())
        throw "Не объявлена";
}
