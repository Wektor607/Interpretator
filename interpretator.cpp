#include "interpretator.h"
Interpretator::Interpretator (const char * program):pars(program)
{
	//throw "I am here";
};

void Interpretator::interpretation()
{
    cout << "/////////////////////// Результат программы //////////////////////" << endl;
	pars.analyze(); // сканирование служебных и не служебных слов, в случае если нет ошибок, то разбиваем на блоки лексем(блоком может быть цикл, условный оператор)
    if(pars.err_parser == false)
    {
        try
        {
        	E.execute(pars.prog); // в случае успешного сканирования происходит испольнение программы
        	cout << endl << "Конец работы программы" << endl;   
        }
        catch(const char * c)
        {
        	cout << "\x1b[37;41m" << c << "\x1b[0m" << endl;
        }
    }
    else
    {
        cout << "\x1b[37;41m" << "Программа завершила работу с ошибкой!" << "\x1b[0m" << endl;
    }
}