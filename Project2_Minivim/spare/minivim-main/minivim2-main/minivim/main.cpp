#include "header.h"


int main(int argc, char *argv[]){
    if(argc){
        for(int i = 0;i < argc;i++){
            if(argv[i][0] == '-'){
                if(argv[i][1] == 'r')read_only = true;
                if(argv[i][1] == 't')trunc = true;
            }
            else open = argv[i];
        }
        if(read_only) fp = fopen(open, "r");
        else fp = fopen(open, "r+");
        if(trunc || fp == NULL) fp = fopen(open, "w");
    }

    init(infowin, cmdwin);

    wmove(Win, y, t);
    Insert_Mode(infowin);
    while(true){
        Content_Refresh();

        char c[20]="line:    col=   ";
        if(line + y >= 100)c[5] = (line + y) / 100 + '0';
        if(line + y >= 10)c[6] = ((line + y) % 100) / 10 + '0';
        c[7] = ((line + y) % 10) + '0';
        if(t > 9)
            c[14] = t / 10 + '0';
        c[15] = t % 10 + '0';
        mvwprintw(infowin, 0, 0, c);
        wrefresh(infowin);
        wrefresh(Win);
        wmove(Win, y, t);

        if(Quiting)break;

        ch=getch();
        switch(ch){
            case KEY_LEFT:
                x=t;
                if(x) {
                    if(x >= strsize(line + y))x = strsize(line + y) - 1;
                    x--;
                }
                else if(y == 3 && line){
                    line--;
                    x = strsize(line + y) - 1;
                }else{
                    y--;
                    x = strsize(line + y) - 1;
                }
                break;
            case KEY_RIGHT:
                x=t;
                if(x < input[line+y].size() - 1)x++;
                else if(y == 7&&line < len - 7){
                    line++;
                    x=0;
                }else if(y < 7){
                    y++;
                    x = 0;
                }
                break;
            case KEY_UP:
                if(y){
                    if(y == 3 && line){
                        line--;
                    }
                    else y--;
                }
                break;
            case KEY_DOWN:
                if(y == 7 && line < len - 7){
                    line++;
                }else if(y < 7 && line < len - 7){
                    y++;
                }
                break;
            case ':':
                Cmd_Mode(cmdwin);
                break;
            case 'i':
                Insert_Mode(infowin);
                break;
            case 27:
                Quiting = true;
            default:
                break;
        }
    }

    endwin();

    return 0;
}