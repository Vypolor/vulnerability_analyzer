#pragma once
#include <cstdint>
#include <sys/personality.h>
#include <unistd.h>

#include "registers.h"

uint64_t get_register_value(pid_t pid, reg r);

void set_register_value(pid_t pid, reg r, uint64_t value);

uint64_t get_register_value_from_dwarf_register(pid_t pid, unsigned regnum);

std::string get_register_name(reg r);

reg get_register_from_name(const std::string &name);

std::vector<std::string> split(const std::string &s, char delimiter);

bool is_prefix(const std::string& s, const std::string& of);
