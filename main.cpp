
#include "grep.h"

int main(int argc, char *argv[])
{
    Grep grep(argc, argv);
    grep.Execute();

    return 0;
}
