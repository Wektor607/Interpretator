import os
def main():
    words = {"и": "and", "логический": "boolean", "делать": "do", "иначе": "else", "если": "if(", "ложь": "false", "целочисленный": "int", "нет": "not", "или": "or", "программа": "program", "ввод": "read(", "правда": "true", "цикл": "while(", "вывод": "write(", "строковый": "string"}
    print("Введите название теста: ")
    test_name = input()
    if(os.path.isfile(f'tests/test_en.txt')):
        os.remove(f'tests/test_en.txt')
    with open(f"tests/{test_name}.txt", 'r') as file:
        string = (file.read()).split("\n")
        for small_str1 in string:
            small_str1 = small_str1.split("\t")
            for small_str2 in small_str1:
                small_str2 = small_str2.split(" ")
                for small_str3 in small_str2:
                    small_str3 = small_str3.split("(")
                    for i in range(len(small_str3)):
                        if(small_str3[i] in words.keys()):
                            small_str3[i] = words[small_str3[i]]
                    with open(f'tests/test_en.txt', 'a') as new_file:
                        word_list= []
                        for symbol in small_str3:
                            line = symbol
                            if(symbol == 'program' or symbol == '{' or symbol == '}'):
                                line += '\n'
                            elif(symbol != 'program' and symbol != '{' and symbol != '}' and (';' in symbol)):
                                line += '\n'
                            elif(symbol != 'read(' and symbol != 'write(' and symbol != 'if(' and symbol != 'while('):
                                line += ' '
                            word_list.append(line)
                        new_file.writelines(word_list)

if __name__ == "__main__":
    main()