#include <cstdint>
#include <vector>
#include <iostream>
#include <string>
#include <sstream>

#include "utility.h"
#include "registers.h"

uint64_t get_register_value(pid_t pid, reg r) {
    user_regs_struct regs;
    ptrace(PTRACE_GETREGS, pid, nullptr, &regs);
    auto it = std::find_if(begin(g_register_descriptors), end(g_register_descriptors),
                           [r](auto&& rd) { return rd.r == r; });

    return *(reinterpret_cast<uint64_t*>(&regs) + (it - begin(g_register_descriptors)));
}

void set_register_value(pid_t pid, reg r, uint64_t value) {
    user_regs_struct regs;
    ptrace(PTRACE_GETREGS, pid, nullptr, &regs);
    auto it = std::find_if(begin(g_register_descriptors), end(g_register_descriptors),
                           [r](auto&& rd) { return rd.r == r; });

    *(reinterpret_cast<uint64_t*>(&regs) + (it - begin(g_register_descriptors))) = value;
    ptrace(PTRACE_SETREGS, pid, nullptr, &regs);
}

uint64_t get_register_value_from_dwarf_register (pid_t pid, unsigned regnum) {
    auto it = std::find_if(begin(g_register_descriptors), end(g_register_descriptors),
                           [regnum](auto&& rd) { return rd.dwarf_r == regnum; });
    if (it == end(g_register_descriptors)) {
        throw std::out_of_range{"Unknown dwarf register"};
    }

    return ::get_register_value(pid, it->r);
}

std::string get_register_name(reg r) {
    auto it = std::find_if(begin(g_register_descriptors), end(g_register_descriptors),
                           [r](auto&& rd) { return rd.r == r; });
    return it->name;
}

reg get_register_from_name(const std::string& name) {
    auto it = std::find_if(begin(g_register_descriptors), end(g_register_descriptors),
                           [name](auto&& rd) { return rd.name == name; });
    return it->r;
}

std::vector<std::string> split(const std::string &s, char delimiter) {
    std::vector<std::string> out{};
    std::stringstream ss {s};
    std::string item;

    while (std::getline(ss,item,delimiter)) {
        out.push_back(item);
    }

    return out;
}

bool is_prefix(const std::string& s, const std::string& of) {
    if (s.size() > of.size()) return false;
    return std::equal(s.begin(), s.end(), of.begin());
}
