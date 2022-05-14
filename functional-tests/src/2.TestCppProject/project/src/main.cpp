#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/personality.h>
#include <unistd.h>
#include <iostream>

#include "parser.h"
#include "debugger.h"

void execute_debugee (const std::string& prog_name) {
    if (ptrace(PTRACE_TRACEME, 0, 0, 0) < 0) {
        std::cerr << "Error in ptrace\n";
        return;
    }
    execl(prog_name.c_str(), prog_name.c_str(), nullptr);
}

int main(int argc, char* argv[]) {
    ArgParser args(argc, argv);
    if (!args.parse()) {
        return 1;
    }

    auto prog = args.getProgName();

    auto pid = fork();
    if (pid == 0) {
        //child
        personality(ADDR_NO_RANDOMIZE);
        execute_debugee(prog);
    }
    else if (pid >= 1)  {
        //parent
        std::cout << "Started debugging process " << pid << '\n';
        debugger dbg{prog, pid};
        dbg.run();
    }
}
