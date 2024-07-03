#ifndef minivim
#include <string>
#include <cstdio>
#include <vector>
#include <iostream>
#include <fstream>
#include <ncurses.h>

#define INF_COLOR_NUM 1
#define IND_COLOR_NUM 2
#define WAR_COLOR_NUM 3
#define now_column cursor_column + topcolumn
#define now_line cursor_line + topline
#define bufferlen 50
#define COLSS COLS - 1 // to avoid unnecessay problem caused by the '\0' of a string

class MiniVim
{
    int IndexLength, tim, oldtim;
    int cursor_line, cursor_column, topline, topcolumn, real_column, real_line;
    char mode;
    bool isdos;
    std::vector<std::string> content, history;
    std::string filename, section, status, warnunsave, warnread, TransToUnix, TransToDos, query, result, type, address;

public:
    MiniVim(const std::string &, std::string);
    ~MiniVim();
    void run();

private:
    void up();
    void left();
    void right();
    void down();
    void enter();
    void backspace();
    void delte();
    void moveforward();
    void movebackward();
    void init();
    void load_file();
    void update();
    void save();
    void insert(std::string, int);
    void append(std::string &);
    void remove(int);
    void Normal_Mode();
    void NormalBack();
    void Insert_Mode();
    void Command_Mode();
    void CommandClear();
    void PrintStatus();
    void PrintIndex();
    void PrintContent();
    void PrintCurse();
    void Print();
    void ColumnReset();
    void jump(std::string);
    void substitute(std::string);
    void Warn(std::string);
    int CommandCheck(std::string);
    int NormalCheck(std::string);
    int calc(int);
    int ExecCmd(const char *, std::string &);
    bool TypeCheck(std::string);
    std::string tabschange(std::string &);
    std::string AddressProcess(std::string);
};

#endif