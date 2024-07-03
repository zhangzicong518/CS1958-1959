//
// Created by 86136 on 2022/12/13.
//

#ifndef MINIVIM_HEADER_H
#define MINIVIM_HEADER_H

#include<ncurses.h>
#include<iostream>
#include<string>
#include<map>
#include<string.h>
using namespace std;

#define REG_COLOR_NUM 1
#define CUS_COLOR_NUM 2
#define Win stdscr
#define len (input.size())
#define t (x>strsize(y+line)? strsize(y+line):x)
#define strsize(x) input.find(x)->second.size()

void Insert_Mode(WINDOW* infowin);
void Cmd_Mode(WINDOW* cmdwin);
void Content_Refresh();
void Ins(int ch);
void New_Line();
void init(WINDOW* infowin, WINDOW* cmdwin);


WINDOW *infowin,*cmdwin;
map<int,string> input;
map<int,bool> Is_Same_Line;
int line = 0;
int x = 0, y = 0;
int ch;
bool Quiting = false;
bool trunc = false;
bool read_only = false;

char *open;
FILE* fp=NULL;

#endif //MINIVIM_HEADER_H
