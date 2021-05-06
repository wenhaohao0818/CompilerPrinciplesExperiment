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

string get_preprocessor_directives(FILE *fp)
{
    fseek(fp, -1, SEEK_CUR);
    string word;
    char ch = fgetc(fp);
    while (ch != '\n')
    {
        word.append(1, ch);
        ch = fgetc(fp);
    }
    return "pretreatment:" + word;
}

string get_char_const(FILE *fp)
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
        return "97:" + word;
    }
    // '\''
    return "getCharError";
}

string get_string_const(FILE *fp)
{
    char ch;
    ch = fgetc(fp);
    string word;
    while (ch != '"')
    {
        word.append(1, ch);
        ch = fgetc(fp);
        if (ch == '\n')
            return "getStringError";
    }
    return "98:" + word;
}

bool isDigit(char p)
{
    return (p >= '0' && p <= '9');
}
bool isLegalNum(char ch)
{
    return ch == '.' || ch == 'f' || ch == '-' || ch == 'x' || ch == 'b' || ch == 'L' || ch == 'l' || ch == 'e' || ch == 'u' || ch == 'a' || ch == 'A' || ch == 'b' || ch == 'B' || ch == 'C' || ch == 'c' || ch == 'D' || ch == 'd' || ch == 'e' || ch == 'E' || ch == 'F' || ch == 'f';
}
string get_num_const(FILE *fp)
{
    fseek(fp, -1, SEEK_CUR);
    char ch = fgetc(fp);
    string word;
    do
    {
        word.append(1, ch);
        ch = fgetc(fp);
    } while (isDigit(ch) || isLegalNum(ch));
    // cout << "99:" << word << endl;
    fseek(fp, -1, SEEK_CUR);
    return "99:" + word;
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

string get_ID_word(FILE *fp)
{

    fseek(fp, -1, SEEK_CUR);
    char ch = fgetc(fp);
    // if (ch == '\n')
    // {
    //     fseek(fp, -3, SEEK_CUR);
    //     ch = fgetc(fp);
    // }
    string word;
    while ((isLetter(ch) || isDigit(ch) || ch == '_'))
    {
        word.append(1, ch);
        ch = fgetc(fp);
    }
    fseek(fp, -1, SEEK_CUR);

    int i = 0;
    for (i = 0; i < keyword.size(); i++)
    {
        if (!keyword[i].compare(word))
        {

            return to_string(i + 2) + ":" + word;
        }
    }

    return "1:" + word;
}

bool isOperator(char p, FILE *fp)
{
    char ch2 = fgetc(fp);
    // cout << "p " << p << "  ch2 " << ch2 << "=" << endl;
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

string get_operator(FILE *fp)
{
    char ch = fgetc(fp);
    char ch2 = fgetc(fp);
    // cout << "ch:" << ch << "ch2:" << ch2 << "=" << endl;
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
                fseek(fp, -1, SEEK_CUR);
                return to_string(i + keyword.size() + 2) + ":" + t1;
            }
        }
        else
        {
            if (!(coper[i].compare(t2)))
            {
                return to_string(i + keyword.size() + 2) + ":" + t2;
            }
        }
    }
    fseek(fp, -1, SEEK_CUR);
    return "unknowoperator";
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

string get_delimiter(char p)
{
    int i;
    string ret;
    for (i = 0; i < delimiter.size(); i++)
    {
        if (p == delimiter[i])
        {
            ret = to_string(i + coper.size() + keyword.size() + 2) + ":" + p;
            break;
        }
    }
    return ret;
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

int analyzer(FILE *fp)
{
    char ch;
    do
    {
        ch = fgetc(fp);
        // cout << "<  " << ch << "  >" << endl;
        if (ch == EOF)
            break;
        delAnnotation(&ch, fp); //跳过注释
        if (ch == '\n' || ch == ' ')
        {
            continue;
        }

        if (ch == '#') //预处理语句
        {
            cout << get_preprocessor_directives(fp) << endl;
        }
        else if (ch == '\'') //字符常量
        {
            cout << get_char_const(fp) << endl;
        }
        else if (ch == '"') //字符串常量
        {
            cout << get_string_const(fp) << endl;
        }
        else if (isDigit(ch)) //数值常量
        {
            cout << get_num_const(fp) << endl;
        }
        else if (isTag(ch, fp)) //标识符&关键字
        {
            cout << get_ID_word(fp) << endl;
        }
        else if (isDelimiter(ch)) //分隔符
        {
            cout << get_delimiter(ch) << endl;
        }
        else if (isOperator(ch, fp)) //运算符
        {
            cout << get_operator(fp) << endl;
        }
        else
        {
            cout << "Unknow char:" << ch << endl;
        }
    } while (ch != EOF);
    return 0;
}
int main()
{
    FILE *fp = fopen("in.txt", "r");
    // FILE *fp = fopen("Ex-01.cpp", "r");
    if (fp == NULL)
        printf("no found file!");
    analyzer(fp);
    fclose(fp);
    // system("Pause");
    return 0;
}