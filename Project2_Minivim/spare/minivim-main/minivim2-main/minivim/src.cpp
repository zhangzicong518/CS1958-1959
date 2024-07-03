#include"header.h"

void init(WINDOW* infowin, WINDOW* cmdwin){
    initscr();
    raw();
    noecho();
    keypad(Win, true);

    start_color();
    init_pair(REG_COLOR_NUM, COLOR_WHITE, COLOR_BLACK);
    init_pair(CUS_COLOR_NUM, COLOR_YELLOW, COLOR_CYAN);

    wbkgd(Win, COLOR_PAIR(REG_COLOR_NUM));
    wrefresh(Win);

    infowin = newwin(1, COLS, 10, 0);
    wbkgd(infowin, COLOR_PAIR(CUS_COLOR_NUM));
    wrefresh(infowin);

    cmdwin = newwin(1, COLS, 11, 0);
    wbkgd(cmdwin, COLOR_PAIR(REG_COLOR_NUM));
    wrefresh(cmdwin);

    input.insert(pair<int,string>(1,""));
    Is_Same_Line.insert((pair<int,bool>(1,false)));

    if(fp != NULL && !trunc){
        char temp[100];
        (fgets(temp, 100, fp));
        while(!feof(fp)){
            string tem=temp;
            input.insert(pair<int,string>(len,tem));
            fgets(temp, 100, fp);
        }
    }
}

void Content_Refresh(){
    char c[100];
    wclear(Win);
    for(int i=0;i<10;i++){
        if(line+i>len)break;
        strcpy(c, input[line+i].c_str());
        mvwprintw(Win, i, 0, c);
    }
}

void Insert_Mode(WINDOW* infowin){
    Content_Refresh();
    bool q=false;

    while(true){
        Content_Refresh();

        char c[30]="line:    col=    len=   ";
        if(line + y >= 100)c[5] = (line + y) / 100 + '0';
        if(line + y >= 10)c[6] = ((line + y) % 100) / 10 + '0';
        c[7] = ((line + y) % 10) + '0';
        if(t > 9)
            c[14] = t / 10 + '0';
        c[15] = t % 10 + '0';
        c[22] = len / 10 + '0';
        c[23] = len % 10 + '0';
        mvwprintw(infowin, 0, 0, c);
        wrefresh(Win);
        wrefresh(infowin);
        wmove(Win, y, t);

        if(q)return;

        ch=getch();
        switch(ch){
            case KEY_LEFT:
                x = t;
                if(x) {
                    if(x >= strsize(line + y))x = strsize(line + y) - 1;
                    x--;
                }
                else if(y == 3 && line){
                    line--;
                    Content_Refresh();
                    x = strsize(line + y) - 1;
                }else{
                    y--;
                    x = strsize(line + y) - 1;
                }
                break;
            case KEY_RIGHT:
                x = t;
                if(x<input[line+y].size()-1)x++;
                else if(y==7&&line<len-7){
                    line++;
                    x=0;
                }else if(y<7){
                    y++;
                    x=0;
                }
                break;
            case KEY_UP:
                if(y){
                    if(y == 3&&line){
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
            case 27:
                q = true;
                break;
            case 8:

                x--;
                break;
            case 9:
                for(int i=0;i<4;i++)Ins(' ');
                x=t+4;
                break;
            case '\n':
                New_Line();
                x=0;

                break;
            default:
                x = t;
                Ins(ch);
                x = t + 1;
                break;
        }
    }
}

void Ins(int c){
    if(strsize(line + y) < COLS){
        string temp=input[line+y];
        for(int i=strsize(line+y);i>x;i--)temp[i]=temp[i-1];
        temp[x]=ch;
        input[line+y]=temp;
    }
}

void New_Line(){
    cout<<1;
    string temp="";
    input.insert(pair<int,string>(len,temp));
    Is_Same_Line.insert(pair<int,bool>(len - 1,false));
    for(int i = len - 1; i > line + y + 1; i--){
        input[i]=input[i-1];
    }
    input[line+y+1]=input[line+y].substr(t);
    temp=input[line+y];
    temp[t]='\0';
    if(y==7&&line<len-7){
        line++;
    }else if(y<7){
        y++;
    }
    x=0;
}

void Cmd_Mode(WINDOW* cmdwin){
    return;
}