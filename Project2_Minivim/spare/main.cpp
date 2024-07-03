#include <ncurses.h>
#include <vector>
#include <fstream>
#include <string>
#include <cstdio>

#define REG_COLOR_NUM 1
#define FIL_COLOR_NUM 2
#define INF_COLOR_NUM 3
#define COM_COLOR_NUM 4
#define IND_COLOR_NUM 5

int mode, num, cursor_line, cursor_column, maxline, maxcolumn, topline, topcolumn, old_topcolumn = -1, old_topline = -1;
std::vector<std::string> content;
WINDOW *Win, *File, *Info, *Com, *Ind;
std::string s, filename;

void Init();
void Run();
void Clear();
int check();
void Normal_Mode();
void Normal_Init();
void Command_Mode();
void Command_Init();
void Insert_Init();
void Insert_Mode();
void insert(std::string line, int number);
void append(std::string line);
void remove(int number);
void open();
void up();
void down();
void left();
void right();
void enter();
void backspace();

int main(int argc, char *argv[])
{
    filename = argv[2];
    if (argv[1] == "-t")
        mode = 1;
    else if (argv[1] == "-R")
        mode = 2;
    open();
    Init();
    Run();
    Clear();
    return 0;
}

void debug()
{
    wrefresh(Com);
    wmove(File, cursor_line, cursor_column);
    wrefresh(File);
}

void open()
{
    std::ifstream ifile(filename);
    if (ifile.is_open())
    {
        while (!ifile.eof())
        {
            std::string buffer;
            std::getline(ifile, buffer);
            append(buffer);
        }
    }
    else
        append("");
}

void insert(std::string line, int number)
{
    content.insert(content.begin() + number, line);
}

void append(std::string line)
{
    content.push_back(line);
}

void remove(int number)
{
    content.erase(content.begin() + number);
}

void up()
{
    if ((--cursor_line) < 0 && topline > 0)
        topline--;
    cursor_line = std::max(0, cursor_line);
    cursor_column = std::min(cursor_column, (int)content[cursor_line + topline].length());
}

void down()
{
    if (topline + cursor_line < content.size() - 1)
        cursor_line++;
    if (cursor_line - topline + 1 > maxline)
    {
        topline++;
        cursor_line--;
    }
    cursor_column = std::min(cursor_column, (int)content[cursor_line + topline].length());
}

void left()
{
    if ((--cursor_column) < 0 && topcolumn > 0)
        topcolumn--;
    cursor_column = std::max(0, cursor_column);
}

void right()
{
    if ((++cursor_column) - topcolumn + 1 > maxcolumn)
        topcolumn++;
    cursor_column = std::min(cursor_column, maxcolumn);
}

void backspace()
{
    if (cursor_column == 0 && cursor_line + topline > 0)
    {
        cursor_column = content[cursor_line + topline - 1].length();
        remove(cursor_line + topline);
        if (cursor_column > maxcolumn + topcolumn)
        {
            topcolumn = cursor_column - maxcolumn;
            cursor_column = maxcolumn;
        }
        up();
    }
    else if (cursor_column > 0)
        content[cursor_line + topline].erase((--cursor_column) + topcolumn, 1);
    else if (cursor_column == 0 && topcolumn > 0)
    {
        content[cursor_line + topline].erase((--cursor_column) + topcolumn, 1);
        topcolumn--;
    }
}

void enter()
{
    if ((cursor_column + topcolumn) < content[cursor_line + topline].length())
    {
        insert(content[cursor_line + topline].substr(cursor_column + topcolumn, content[cursor_line + topline].length() - cursor_column + topcolumn), cursor_line + topline + 1);
        content[cursor_line + topline].erase(cursor_column + topcolumn, content[cursor_line + topline].length() - cursor_column + topcolumn);
    }
    else
        insert("", cursor_line + topline + 1);
    down();
}

void PrintIndex()
{
    for (int i = 0; i <= maxline; i++)
    {
        mvwprintw(Ind, i, 0, (topline + i >= 100 ? "%d" : (topline + i >= 10 ? " %d" : "  %d")), topline + 1 + i);
        wrefresh(Ind);
    }
    wmove(File, cursor_line, cursor_column);
    wrefresh(File);
}

void PrintContent()
{
    int len, number;
    for (int i = 0; i <= std::min((int)content.size() - 1, maxline); i++)
    {
        number = topline + i;
        if (topcolumn > content[number].length())
            continue;
        len = std::min((int)content[number].length(), maxcolumn);
        mvwprintw(File, i, 0, "%s", content[number].substr(topcolumn, len));
    }
    wrefresh(File);
}

void scr_refresh()
{
    if (old_topcolumn != topcolumn || old_topline != topline)
    {
        wclear(File);
        PrintContent();
        PrintIndex();
        debug();
        old_topcolumn = topcolumn;
        old_topline = topline;
    }
    wmove(File, cursor_line, cursor_column);
    wrefresh(File);
}

void Init()
{
    initscr();
    raw();
    noecho();
    keypad(stdscr, true);

    start_color();
    init_pair(REG_COLOR_NUM, COLOR_WHITE, COLOR_BLACK);
    init_pair(FIL_COLOR_NUM, COLOR_WHITE, COLOR_BLACK);
    init_pair(INF_COLOR_NUM, COLOR_GREEN, COLOR_BLUE);
    init_pair(COM_COLOR_NUM, COLOR_WHITE, COLOR_BLACK);
    init_pair(IND_COLOR_NUM, COLOR_YELLOW, COLOR_BLACK);

    wbkgd(stdscr, COLOR_PAIR(REG_COLOR_NUM));
    wrefresh(stdscr);

    getmaxyx(stdscr, maxline, maxcolumn);
    Win = newwin(maxline, maxcolumn, 0, 0);
    File = subwin(Win, maxline - 2, maxcolumn - 3, 0, 3);
    keypad(File, true);
    wbkgd(File, COLOR_PAIR(FIL_COLOR_NUM));
    wrefresh(File);

    Ind = subwin(Win, maxline - 2, 3, 0, 0);
    wbkgd(Ind, COLOR_PAIR(IND_COLOR_NUM));
    wrefresh(Ind);

    Info = subwin(Win, 1, maxcolumn, maxline - 2, 0);
    keypad(Info, true);
    wbkgd(Info, COLOR_PAIR(INF_COLOR_NUM));
    wrefresh(Info);

    Com = subwin(Win, 1, maxcolumn, maxline - 1, 0);
    keypad(Com, true);
    wbkgd(Com, COLOR_PAIR(COM_COLOR_NUM));
    wrefresh(Com);

    getmaxyx(File, maxline, maxcolumn);
    mvwprintw(File, maxline / 3, maxcolumn / 2 - 10, "Welcome to Minivim!");
    mvwprintw(File, maxline / 3 + 1, maxcolumn / 2 - 9, "Press i to insert");
    wrefresh(File);
}

void Run()
{
    Normal_Mode();
}

void Clear()
{
    wclear(Win);
    wrefresh(Win);
    delwin(Win);
    noraw();
    echo();
    endwin();
}

void Normal_Init()
{
    wclear(Info);
    mvwprintw(Info, 0, 0, "Normal Mode %s %d,%d", filename, cursor_line, cursor_column);
    wrefresh(Info);
    wmove(File, cursor_line, cursor_column);
    wrefresh(File);
}

void Command_Init()
{
    wclear(Info);
    mvwprintw(Info, 0, 0, "Command Mode %s %d,%d", filename, cursor_line, cursor_column);
    wrefresh(Info);
    mvwprintw(Com, 0, 0, ":");
    wrefresh(Com);
}

void Insert_Init()
{
    wclear(Info);
    mvwprintw(Info, 0, 0, "Insert Mode %s %d,%d", filename, cursor_line, cursor_column);
    wrefresh(Info);
    scr_refresh();
    wmove(File, cursor_line, cursor_column);
    wrefresh(File);
}

void Insert_Mode()
{
    int ch;
    Insert_Init();
    while ((ch = getch()) != 27)
    {
        switch (ch)
        {
        case 127:
        case KEY_BACKSPACE:
            backspace();
            break;
        case KEY_LEFT:
            left();
            break;
        case KEY_UP:
            up();
            break;
        case KEY_RIGHT:
            right();
            break;
        case KEY_DOWN:
            down();
            break;
        case KEY_ENTER:
            enter();
            break;
        case 9:
            content[cursor_line + topline].insert(cursor_column + topcolumn, 2, ' ');
            right();
            right();
        default:
            content[cursor_line + topline].insert(cursor_column + topcolumn, 1, ch);
            right();
            break;
        }
        scr_refresh();
        ch = getch();
    }
    getyx(File, cursor_line, cursor_column);
    Normal_Mode();
}

int check()
{
    int opt = 0, len;
    len = s.length();
    for (int i = 0; i < len; i++)
    {
        if (s[i] == 'q')
            return opt = -1;
        if (s[i] == 27)
            return opt = 1;
    }
    return opt;
}

void Command_Mode()
{
    int ch = 0, opt = 0;
    Command_Init();
    while ((opt = check()) == 0)
    {
        s.clear();
        while ((ch = getch()) != '\n')
        {
            s = s + (char)ch;
            if (ch == 27)
                break;
            wprintw(Com, "%c", ch);
            wrefresh(Com);
        }
    }
    if (opt == 1)
        Normal_Mode();
}

void Normal_Mode()
{
    Normal_Init();
    int ch = getch();
    while (ch != 'i' && ch != ':')
    {
        switch (ch)
        {
        case KEY_LEFT:
            left();
            break;
        case KEY_UP:
            up();
            break;
        case KEY_RIGHT:
            right();
            break;
        case KEY_DOWN:
            down();
            break;
        default:
            break;
        }
        scr_refresh();
        ch = getch();
    }
    if (ch == ':')
        Command_Mode();
    if (ch == 'i')
        Insert_Mode();
}
