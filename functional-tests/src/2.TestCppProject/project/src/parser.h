#include <iostream>

using namespace std;
class ArgParser {
    const char *opts="hp:";
    string _progName; //name_prog
    int _argc;
    char **_argv;
    void help();
    bool fileExist();
public:
    ArgParser(int argc, char **argv);
    bool parse();
    string getProgName();
};

