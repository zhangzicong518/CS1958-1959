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
    std::string filename, section, status, warnunsave, warnread, TransToUnix, TransToDos, query, result, type;

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
};

#include <minivim.h>

// to reset the correct number of column after curse-moveoperation
void MiniVim::ColumnReset()
{
    if (cursor_column > COLSS - IndexLength)
    {
        topcolumn = cursor_column - (COLSS - IndexLength);
        cursor_column = COLSS - IndexLength;
    }
    else
        topcolumn = 0;
    update();
}

// open a new fork to run command in shell and get result
int MiniVim::ExecCmd(const char *cmd, std::string &result)
{
    FILE *pipe = popen(cmd, "r");
    if (!pipe)
    {
        pclose(pipe);
        return -1;
    }
    char buf[bufferlen + 1];
    while (fgets(buf, bufferlen, pipe) != NULL)
        result.append(buf);
    pclose(pipe);
    return 0;
}

// to check the type of file
// return true if the file is dos type
bool MiniVim::TypeCheck(std::string s)
{
    if (s.find("CRLF") != s.npos) // find CRLF is dos
        return true;
    else
        return false;
}

// get the relative path of the file
std::string MiniVim::AddressProcess(std::string s)
{
    std::string tmp = {};
    size_t len;
    if ((len = s.find_last_of("/")) != s.npos)
    {
        address = s.substr(0, len + 1);
        tmp = s.substr(len + 1);
    }
    else
        tmp = s;
    return tmp;
}

MiniVim::MiniVim(const std::string &file, std::string ttype = "-t")
{
    cursor_line = 0;
    cursor_column = 0;
    topcolumn = 0;
    topline = 0;
    real_column = 0;
    real_line = 0;
    IndexLength = 0;
    tim = 0;
    oldtim = 0;
    isdos = false;
    type = ttype;
    mode = 'n';
    status = "Normal Mode";
    warnunsave = "Your chang hasn't been saved.(add ! if force to quit)";
    warnread = "Your file is read-only.";
    TransToUnix = "dos2unix ";
    TransToDos = "unix2dos ";
    query = "file ";
    address = "/mnt/d/Project/Project2_Minivim/minivim/bin/";
    section = {};
    if (file.empty())
        filename = "untitled.txt";
    else
    {
        filename = AddressProcess(file);
        if (filename.empty())
            filename = "untitled.txt";
        if (ExecCmd((query + filename).c_str(), result) != -1)
            isdos = TypeCheck(result);
        // to nake it compatible for dos file
        if (isdos)
            ExecCmd((TransToUnix + filename).c_str(), result);
    }
    load_file();
    init();
}

MiniVim::~MiniVim()
{
    // to restore the original type of a file
    if (isdos)
        system((TransToDos + filename).c_str());
    refresh();
    noraw();
    echo();
    endwin();
}

void MiniVim::PrintStatus()
{
    section = section = " COLS: " + std::to_string(real_column + 1) +
                        " | ROWS: " + std::to_string(real_line + 1) +
                        " | FILE: " + filename;
    init_pair(INF_COLOR_NUM, COLOR_GREEN, COLOR_BLUE);
    attron(A_BOLD);
    attron(COLOR_PAIR(INF_COLOR_NUM));
    for (int i = IndexLength; i < COLS; i++)
        mvprintw(LINES - 2, i, " ");
    mvprintw(LINES - 2, 0, status.c_str());
    mvprintw(LINES - 2, COLS - section.length(), section.c_str());
    attroff(COLOR_PAIR(INF_COLOR_NUM));
    attroff(A_BOLD);
}

// calc the length of a number
int MiniVim::calc(int number)
{
    int tmp = 0;
    while (number)
    {
        number /= 10;
        tmp++;
    }
    return tmp;
}

void MiniVim::PrintIndex()
{
    // topline represent the index of firs line
    // so the index of last line is topline + LINES -3
    // so the set number of last line is topline + LINES -2
    int len;
    IndexLength = calc(topline + LINES - 2);
    init_pair(IND_COLOR_NUM, COLOR_YELLOW, COLOR_BLACK);
    attron(COLOR_PAIR(IND_COLOR_NUM));
    for (int i = 0; i < LINES - 2; i++)
    {
        move(i, 0);
        clrtoeol();
        len = calc(i + topline + 1);
        mvprintw(i, IndexLength - len, "%d", topline + i + 1);
    }
    attroff(COLOR_PAIR(IND_COLOR_NUM));
}

void MiniVim::PrintContent()
{
    int len;
    for (int i = 0; i < LINES - 2; i++)
    {
        move(i, IndexLength);
        clrtoeol();
        // clear the rest of content caused by many times ENTER
        if (i + topline >= content.size())
            continue;
        len = std::min((int)content[topline + i].length() - 1 - topcolumn + 1, COLSS - IndexLength);
        if (len <= 0)
            continue;
        mvprintw(i, IndexLength, content[topline + i].substr(topcolumn, len).c_str());
        clrtoeol();
    }
}

void MiniVim::PrintCurse()
{
    move(cursor_line, IndexLength + cursor_column);
}

void MiniVim::Print()
{
    PrintStatus();
    PrintIndex();
    PrintContent();
    PrintCurse();
}

void MiniVim::update()
{
    real_column = now_column;
    real_line = now_line;
}

void MiniVim::NormalBack()
{
    if (mode == 'n' && now_column == content[now_line].length())
        left();
    PrintCurse();
}

void MiniVim::up()
{
    update();
    // if the curse_line is the first line of the screen
    if ((--cursor_line) < 0 && topline > 0)
        topline--;
    cursor_line = std::max(0, cursor_line);
    real_column = std::min(now_column, (int)content[now_line].length());
    if (real_column <= COLSS - IndexLength)
    {
        topcolumn = 0;
        cursor_column = real_column;
    }
    else
    {
        topcolumn = real_column - (COLSS - IndexLength);
        cursor_column = COLSS - IndexLength;
    }
    update();
}

void MiniVim::down()
{
    update();
    // if now_line is not the last line of a file
    if (now_line < content.size() - 1)
        cursor_line++;
    // if the line is over the last line of a screen
    if (cursor_line > LINES - 3)
    {
        topline++;
        cursor_line--;
    }
    real_column = std::min(now_column, (int)content[now_line].length());
    if (real_column <= COLSS - IndexLength)
    {
        topcolumn = 0;
        cursor_column = real_column;
    }
    else
    {
        topcolumn = real_column - (COLSS - IndexLength);
        cursor_column = COLSS - IndexLength;
    }
    update();
}

void MiniVim::left()
{
    update();
    if ((--cursor_column) < 0 && topcolumn > 0)
        topcolumn--;
    cursor_column = std::max(0, cursor_column);
    update();
}

void MiniVim::right()
{
    update();
    if ((++cursor_column) >= COLSS - IndexLength)
    {
        topcolumn++;
        cursor_column--;
    }
    cursor_column = std::min(cursor_column, (int)content[now_line].length() - topcolumn);
    update();
}

void MiniVim::backspace()
{
    update();
    // if the curse is at the beginning of a line
    // then it need to go back to the end of a upperline
    if (now_column == 0 && now_line > 0)
    {
        cursor_column = content[cursor_line + topline - 1].length();
        content[cursor_line + topline - 1] += content[cursor_line + topline];
        remove(cursor_line + topline);
        ColumnReset();
        up();
    }
    else if (now_column > 0)
    {
        content[cursor_line + topline].erase((--cursor_column) + topcolumn, 1);
        if (cursor_column < 0)
        {
            cursor_column++;
            topcolumn--;
        }
        update();
    }
}

void MiniVim::enter()
{
    if (now_column < content[now_line].length())
    {
        insert(content[now_line].substr(now_column, content[now_line].length() - now_column), now_line + 1);
        content[now_line].erase(now_column, content[now_line].length() - now_column);
    }
    else
        insert(" ", now_line + 1); // crate a new line when at the end
    cursor_column = 0;
    topcolumn = 0;
    update();
    down();
}

// command dd to erase a whole line
void MiniVim::delte()
{
    remove(cursor_line + topline);
    if (cursor_line + topline + 1 > (int)content.size())
    {
        if (cursor_line > 0)
            cursor_line--;
        else if (cursor_line == 0 && topline > 0)
            topline--;
        else
            content.push_back("");
    }
    cursor_column = 0;
    update();
    Print();
}

// go to the beginning of next word
void MiniVim::moveforward()
{
    // if flag = 0 hasn't found a blanket
    // if flag = 1 has found a blanket
    // if flag = 2 has found the frist char after balnket
    int pos = 0, flag = 0;
    for (int i = now_column; i < (int)content[now_line].size(); i++)
    {
        if (flag == 0 && content[now_line][i] == ' ')
            flag = 1;
        if (flag == 1 && content[now_line][i] != ' ')
        {
            flag = 2;
            pos = i;
            break;
        }
    }
    if (flag == 2)
    {
        cursor_column = pos;
        ColumnReset();
    }
    else
    {
        // can't find then switch ot the begin of a new line
        if (now_line == (int)content.size() - 1)
            content.push_back("");
        cursor_column = 0;
        topcolumn = 0;
        down();
    }
    update();
}

void MiniVim::movebackward()
{
    // if flag = 0 hasn't found a blanket
    // if flag = 1 has found a blanket
    // if flag = 2 has found the frist char befor balnket
    // if flag = 3 has found a whole word before blanket
    int pos = 0, flag = 0;
    for (int i = std::min(now_column - 1, (int)content[now_line].length() - 1); i >= 0; i--)
    {
        if (flag == 0 && content[now_line][i] == ' ')
        {
            flag = 1;
            if (pos != 0)
            {
                flag = 3;
                break;
            }
        }
        if (flag == 0)
            pos = i;
        if (flag == 1 && content[now_line][i] != ' ')
        {
            flag = 2;
            pos = 0;
        }
        if (flag == 2 && content[now_line][i] == ' ')
        {
            flag = 3;
            pos = i + 1;
            break;
        }
    }
    // if can't find at cursor_line
    if (flag == 1 || now_column == 0)
    {
        if (now_line == 0) // if now at the first line then move curse beginning
            pos = 0;
        else
        {
            up();
            cursor_column = content[now_line].length();
            ColumnReset();
            for (int i = std::min(now_column, (int)content[now_line].length() - 1); i >= 0; i--)
            {
                if (flag == 0 && content[now_line][i] == ' ')
                {
                    flag = 1;
                    if (pos != 0)
                    {
                        flag = 3;
                        break;
                    }
                }
                if (flag == 0)
                    pos = i;
                if (flag == 1 && content[now_line][i] != ' ')
                {
                    flag = 2;
                    pos = 0;
                }
                if (flag == 2 && content[now_line][i] == ' ')
                {
                    flag = 3;
                    pos = i + 1;
                    break;
                }
            }
        }
    }
    cursor_column = pos;
    ColumnReset();
    update();
}

int MiniVim::CommandCheck(std::string s)
{
    history.push_back("");
    if (s == ":w")
        return 2;
    if (s == ":q")
        return 3;
    if (s == ":q!")
        return 4;
    if (s == ":wq")
        return 5;
    if (s.empty())
        return 6;
    if (s.find(":jmp") != s.npos)
        return 7;
    if (s.find(":sub") != s.npos)
        return 8;
    return 0;
}

int MiniVim::NormalCheck(std::string s)
{
    int opt = 0;
    if (s == "dd")
        return 1;
    if (s == "O")
        return 2;
    if (s == "$")
        return 3;
    if (s == "w")
        return 4;
    if (s == "b")
        return 5;
    return 0;
}

void MiniVim::CommandClear()
{
    move(LINES - 1, 0);
    clrtoeol();
    PrintCurse();
}

void MiniVim::Normal_Mode()
{
    int ch, len = 0, opt = 0;
    std::string s{};
    status = "Normal Mode";
    mode = 'n';
    while (mode == 'n')
    {
        NormalBack();
        Print();
        ch = getch();
        switch (ch)
        {
        case ':':
            mode = 'c';
            break;
        case 'i':
            mode = 'i';
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
        case KEY_BACKSPACE:
            if (len > 0)
                s.erase(--len, 1);
            move(LINES - 1, 0);
            clrtoeol();
            mvprintw(LINES - 1, 0, s.c_str());
            break;
        default:
            s.push_back(ch);
            len++;
            mvprintw(LINES - 1, 0, s.c_str());
            if ((opt = NormalCheck(s)) != 0 || len >= 3)
            {
                switch (opt)
                {
                case 1: // dd
                    tim++;
                    delte();
                    break;
                case 2: // O
                    topcolumn = 0;
                    cursor_column = 0;
                    update();
                    break;
                case 3: // $
                    cursor_column = content[now_line].length();
                    ColumnReset();
                    break;
                case 4: // w
                    moveforward();
                    break;
                case 5: // b
                    movebackward();
                    break;
                default:
                    break;
                }
                len = 0;
                opt = 0;
                s = {};
                move(LINES - 1, 0);
                clrtoeol();
            }
            PrintCurse();
            break;
        }
    }
    if (mode == 'c')
    {
        CommandClear();
        Command_Mode();
        return;
    }
    if (mode == 'i')
    {
        left();
        CommandClear();
        Insert_Mode();
        return;
    }
}

void MiniVim::Insert_Mode()
{
    status = "Insert Mode";
    mode = 'i';
    int ch;
    while (mode == 'i')
    {
        Print();
        ch = getch();
        switch (ch)
        {
        case 27:
            mode = 'c';
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
        case 10:
            tim++;
            enter();
            break;
        case KEY_BACKSPACE:
            tim++;
            backspace();
            break;
        case KEY_BTAB:
        case KEY_CATAB:
        case KEY_CTAB:
        case 9:
            tim++;
            content[now_line].insert(now_column, 2, ' ');
            right();
            right();
            break;
        default:
            tim++;
            content[now_line].insert(now_column, 1, ch);
            right();
            break;
        }
    }
    Normal_Mode();
}

// out put thw warning information
void MiniVim::Warn(std::string s)
{
    CommandClear();
    init_pair(WAR_COLOR_NUM, COLOR_WHITE, COLOR_RED);
    attron(COLOR_PAIR(WAR_COLOR_NUM));
    mvprintw(LINES - 1, 0, s.c_str());
    attroff(COLOR_PAIR(WAR_COLOR_NUM));
}

void MiniVim::jump(std::string s)
{
    int tar = 0;
    for (int i = 0; i < (int)s.size(); i++)
        if (isdigit(s[i]))
            tar = tar * 10 + s[i] - '0';
    tar = std::min(tar, (int)content.size()) - 1;
    tar = std::max(0, tar);
    // if need to refresh the file window
    if (tar > topline + LINES - 3 || tar < topline)
    {
        topline = tar;
        cursor_line = 0;
        cursor_column = 0;
    }
    else
    {
        cursor_line = tar;
        cursor_column = 0;
    }
    update();
    Print();
}

void MiniVim::substitute(std::string s)
{
    std::string tar = {}, rep = {};
    size_t lef = 0, rig = 0, pos;
    // to get the first word
    lef = s.find_first_of("\"", rig + 1);
    if (lef == s.npos)
        return;
    rig = s.find_first_of("\"", lef + 1);
    if (rig == s.npos)
        return;
    if (rig - lef - 1 <= 0)
        return;
    tar = s.substr(lef + 1, rig - lef - 1);
    // to get the second word
    lef = s.find_first_of("\"", rig + 1);
    if (lef == s.npos)
        return;
    rig = s.find_first_of("\"", lef + 1);
    if (rig == s.npos)
        return;
    if (rig - lef - 1 <= 0)
        return;
    rep = s.substr(lef + 1, rig - lef - 1);
    // replace the tar by rep
    for (int i = 0; i < (int)content.size(); i++)
    {
        pos = 0;
        while ((pos = content[i].find(tar, pos)) != content[i].npos)
        {
            content[i].replace(pos, tar.length(), rep);
            pos++;
        }
    }
    Print();
}

void MiniVim::Command_Mode()
{
    status = "Command Mode";
    int ch, opt, now = 0, pos = 1;
    history.push_back(":");
    PrintStatus();
    CommandClear();
    mvprintw(LINES - 1, 0, ":");
loop:
    do
    {
        now = (int)history.size() - 1;
        pos = history[now].length();
        while ((ch = getch()) != '\n')
        {
            switch (ch)
            {
            case 27:
                opt = 1;
                break;
            case KEY_UP:
                now = std::max(0, now - 1);
                pos = (int)history[now].length();
                break;
            case KEY_DOWN:
                now = std::min((int)history.size() - 1, now + 1);
                pos = (int)history[now].length();
                break;
            case KEY_LEFT:
                pos = std::max(0, pos - 1);
                break;
            case KEY_RIGHT:
                pos = std::min((int)history[now].length(), pos + 1);
                break;
            case KEY_BACKSPACE:
                if (pos > 0)
                    history[now].erase(--pos, 1);
                break;
            default:
                history[now].insert(pos++, 1, ch);
                break;
            }
            if (opt == 1)
                break;
            CommandClear();
            mvprintw(LINES - 1, 0, history[now].c_str());
            move(LINES - 1, pos);
        }
        CommandClear();
        if (opt == 1)
            break;
    } while ((opt = CommandCheck(history[now])) == 0);
    switch (opt)
    {
    case 1: // ESC
        if (history[now].length() == 0)
            history.erase(history.begin() + now);
        Normal_Mode();
        break;
    case 2: // w
        if (type == "-R")
            Warn(warnread);
        else
            save();
        goto loop;
        break;
    case 3: // q
        if (oldtim != tim)
        {
            Warn(warnunsave);
            goto loop;
        }
        break;
    case 4: // q!
        break;
    case 5: // wq
        if (type == "-R")
        {
            Warn(warnread);
            goto loop;
        }
        else
            save();
        break;
    case 6: // empty
        if (history[now].length() == 0)
            history.erase(history.begin() + now);
        goto loop;
        break;
    case 7: // jump
        jump(history[now]);
        goto loop;
        break;
    case 8: // search & substitution
        substitute(history[now]);
        goto loop;
        break;
    }
}

void MiniVim::run()
{
    Normal_Mode();
}

void MiniVim::init()
{
    initscr();
    raw();
    noecho();
    keypad(stdscr, true);
    use_default_colors();
    start_color();
}

void MiniVim::append(std::string &line)
{
    line = tabschange(line);
    content.push_back(line);
}

void MiniVim::remove(int number)
{
    content.erase(content.begin() + number);
}

void MiniVim::insert(std::string line, int number)
{
    content.insert(content.begin() + number, line);
}

// replace all the '\t' by " "
std::string MiniVim::tabschange(std::string &s)
{
    std::size_t pos = s.find('\t');
    return pos == s.npos ? s : tabschange(s.replace(pos, 1, " "));
}

void MiniVim::load_file()
{
    std::ifstream ifile(address + filename);
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
    {
        std::string buffer{};
        append(buffer);
    }
}

void MiniVim::save()
{
    std::ofstream ofile(address + filename);
    if (ofile.is_open())
    {
        for (int i = 0; i < (int)content.size(); i++)
            ofile << content[i] << '\n';
        ofile.close();
    }
    oldtim = tim;
}

int main(int argc, char **argv)
{
    if (argc == 3)
    {
        auto minivim = MiniVim(argv[2], argv[1]);
        minivim.run();
    }
    else if (argc == 2)
    {
        auto minivim = MiniVim(argv[1]);
        minivim.run();
    }
    else
    {
        auto minivim = MiniVim("");
        minivim.run();
    }
    return 0;
}