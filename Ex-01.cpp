#include <iostream>
#include <string>
#include <vector>
using namespace std;
vector<char> delimiter = {'(', ')', '{', '}', '[', ']', ',', ';', '.', ':'}; //delimiter
vector<string> keyword = {                                                   //keyword
    "auto", "break", "case", "char", "const", "continue", "default", "do", "double", "else", "enum",
    "extern", "float", "for", "goto", "if", "int", "long", "register", "return", "short", "signed",
    "sizeof", "static", "struct", "switch", "typedef", "union", "unsigned", "void", "volatile", "while",
    "inline", "restrict", "_Bool", "_Complex", "_Imaginary", "_Alignas", "_Alignof", "_Atomic",
    "_Static_assert", "_Noreturn", "_Thread_local", "_Generic"

};
vector<string> coper = { //Operator
    "+", "-", "*", "/", "%", "=", "|", "&", "^", "!", "<", ">",
    "++", "--", "*=", "/=", "+=", "-=", "==", ">=", "<=", "!=", "%=", "||", "&&", "<<", ">>"};

int get_preprocessor_directives(FILE *fp)
{
    fseek(fp, -1, SEEK_CUR);
    string word;
    char ch = fgetc(fp);
    while (ch != '\n')
    {
        word.append(1, ch);
        ch = fgetc(fp);
    }
    cout << "pretreatment:" << word << endl;
    return 0;
}

int get_char_const(FILE *fp)
{
    char ch;
    string word;
    ch = fgetc(fp);
    word.append(1, ch);
    if (ch == '\\')
    {
        char ch2 = fgetc(fp);
        word.append(1, ch2);
    }
    ch = fgetc(fp);
    if (ch == '\'')
    {
        cout << "97:" << word << endl;
    }
    return 0;
}

int get_string_const(FILE *fp)
{
    char ch;
    ch = fgetc(fp);
    string word;
    while (ch != '"')
    {
        word.append(1, ch);
        ch = fgetc(fp);
    }
    cout << "98:" << word << endl;
    return 0;
}

bool isDigit(char p)
{
    return (p >= '0' && p <= '9');
}

int get_num_const(FILE *fp)
{
    fseek(fp, -1, SEEK_CUR);
    char ch = fgetc(fp);
    string word;
    do
    {
        word.append(1, ch);
        ch = fgetc(fp);
    } while (isDigit(ch));
    cout << "99:" << word << endl;
    fseek(fp, -1, SEEK_CUR);
    return 0;
}

bool isLetter(char ch)
{
    return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
}

bool isTag(char ch, FILE *fp)
{
    char ch2 = fgetc(fp);
    if (ch2 == EOF)
        return false;
    fseek(fp, -1, SEEK_CUR);
    if (ch == '_' || isLetter(ch))
    {
        return true;
    }
    return false;
}

int get_ID_word(FILE *fp)
{

    fseek(fp, -1, SEEK_CUR);
    char ch = fgetc(fp);
    if (ch == '\n')
    {
        fseek(fp, -3, SEEK_CUR);
        ch = fgetc(fp);
    }
    string word;
    while ((isLetter(ch) || isDigit(ch) || ch == '_'))
    {
        word.append(1, ch);
        ch = fgetc(fp);
    }

    int i = 0;
    for (i = 0; i < keyword.size(); i++)
    {
        if (!keyword[i].compare(word))
        {
            cout << i + 2 << ":" << word << endl;
            return 0;
        }
    }
    cout << "1:" << word << endl;
    fseek(fp, -1, SEEK_CUR);
    return 0;
}

bool isOperator(char p, FILE *fp)
{
    char ch2 = fgetc(fp);
    char op[3] = {p, ch2, '\0'};
    string t2(op);
    string t1(1, p);
    int i;
    for (i = coper.size() - 1; i >= 12; i--)
    {
        if (!(coper[i].compare(t2)))
        {
            fseek(fp, -2, SEEK_CUR);
            return true;
        }
    }
    for (i = 11; i >= 0; i--)
    {
        if (!(coper[i].compare(t1)))
        {
            fseek(fp, -2, SEEK_CUR);
            return true;
        }
    }
    fseek(fp, -1, SEEK_CUR);
    return false;
}

int get_operator(FILE *fp)
{
    char ch = fgetc(fp);
    char ch2 = fgetc(fp);
    char op[3] = {ch, ch2, '\0'};
    string t1(1, ch);
    string t2(op);
    int i;
    for (i = coper.size() - 1; i >= 0; i--)
    {
        if (i <= 11)
        {
            if (!(coper[i].compare(t1)))
            {
                cout << i + keyword.size() + 2 << ":" << t1 << endl;
                fseek(fp, -1, SEEK_CUR);
                return 0;
            }
        }
        else
        {
            if (!(coper[i].compare(t2)))
            {
                cout << i + keyword.size() + 2 << ":" << t2 << endl;
                return 0;
            }
        }
    }
    fseek(fp, -1, SEEK_CUR);
    return 0;
}

bool isDelimiter(char p)
{
    int i;
    for (i = 0; i < delimiter.size(); i++)
    {
        if (p == delimiter[i])
            return true;
    }
    return false;
}

int get_delimiter(char p)
{
    int i;
    for (i = 0; i < delimiter.size(); i++)
    {
        if (p == delimiter[i])
        {
            cout << i + coper.size() + keyword.size() + 2 << ":" << p << endl;
            return 0;
        }
    }
    return 1;
}

void delAnnotation(char *ch, FILE *fp)
{
    char ch2;
    ch2 = fgetc(fp);
    if (ch2 == EOF)
        return;
    if (*ch == '/' && ch2 == '/')
    {
        while (*ch != '\n')
        {
            *ch = fgetc(fp);
        }
    }
    if (*ch == '/' && ch2 == '*')
    {
        *ch = fgetc(fp);
        ch2 = fgetc(fp);
        while (!(*ch == '*' && ch2 == '/'))
        {
            *ch = ch2;
            ch2 = fgetc(fp);
        }
        *ch = fgetc(fp);
        ch2 = fgetc(fp);
        if (!(*ch == '*' && ch2 == '/'))
        {
            fseek(fp, -2, SEEK_CUR);
            *ch = fgetc(fp);
            return;
        }
        return;
    }
    fseek(fp, -1, SEEK_CUR);
}

int main()
{
    FILE *fp = fopen("in.txt", "r");
    // FILE *fp = fopen("Ex-01.cpp", "r");
    if (fp == NULL)
        printf("no found file!");

    char ch;
    do
    {
        ch = fgetc(fp);
        if (ch == EOF)
            break;
        delAnnotation(&ch, fp); //跳过注释
        // cout << "<   " << ch << "   >" << endl;
        if (ch == '\n' || ch == ' ')
        {
            continue;
        }
        if (ch == '#') //预处理语句
        {
            get_preprocessor_directives(fp);
        }
        else if (ch == '\'') //字符常量
        {
            get_char_const(fp);
        }
        else if (ch == '"') //字符串常量
        {
            get_string_const(fp);
        }
        else if (isDigit(ch)) //数值常量
        {
            get_num_const(fp);
        }
        else if (isTag(ch, fp)) //标识符&关键字
        {
            // cout << "in:" << ch << endl;
            get_ID_word(fp);
        }
        else if (isDelimiter(ch)) //分隔符
        {
            get_delimiter(ch);
        }
        else if (isOperator(ch, fp)) //运算符
        {
            get_operator(fp);
        }
        else
        {
            cout << "Unknow char:" << ch << endl;
        }

    } while (ch != EOF);
    fclose(fp);
    // system("Pause");
    return 0;
}