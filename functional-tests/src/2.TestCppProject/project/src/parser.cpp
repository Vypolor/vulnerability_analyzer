#include "parser.h"
#include <string.h>
#include <unistd.h>
#include <fstream>

using namespace std;
ArgParser::ArgParser(int argc, char* argv[]): _argc(argc), _argv(argv) {}

string ArgParser::getProgName() {
    return _progName;
}

bool ArgParser::fileExist(){
    fstream fileStream;
    fileStream.open(_progName);
    if(fileStream.fail()){
        cout<<"File does not exist"<<endl;
        return false;
    }
    return true;
}
bool ArgParser::parse() {
    int opt=0;
    string ProgName=string(_argv[1]);
    if(ProgName.find("-")!=0){
        _progName=ProgName;
    }
    while((opt = getopt(_argc, _argv, opts)) != -1){
        switch(opt){
            case 'h':
                help();
                return false;
            case 'p':
                if(!_progName.empty()){
                    cout<<"Incorrect args"<<endl;
                    return false;
                }
                _progName=string(optarg);
                break;
            default:
                help();
                return false;
        }
    }
    if(_progName.empty() || !fileExist()){
        return false;
    }
    return true;
}

void ArgParser::help() {
    cout<<"Help"<<endl;
}


