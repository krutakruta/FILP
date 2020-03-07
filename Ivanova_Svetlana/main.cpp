# include "char_ver.h"
# include "string_ver.h"


int main(int argc, char *argv[]) {
    const char *filename;
    if (argv[1] == "--help")
        cout << "main [-c | -s] [input filename]";
    if (argc > 2)
        filename = argv[2];
    else
        filename = "input.txt";
    if (argv[1] == "-c")
        char_ver(filename);
    else if (argv[1] == "-s")
        str_ver(filename);
    else
    {
        char_ver(filename);
        str_ver(filename);
    }
    return 0;
}
