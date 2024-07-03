#include <minivim.h>

int main(int argc, char **argv)
{
    if (argc == 3)
    {
        auto minivim = MiniVim(argv[2], argv[1]);
        minivim.run();
    }
    else if (argc == 2)
    {
        auto minivim = MiniVim(argv[1], "-t");
        minivim.run();
    }
    else
    {
        auto minivim = MiniVim("", "");
        minivim.run();
    }
    return 0;
}