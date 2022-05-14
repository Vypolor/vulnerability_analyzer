#pragma once

#include <cstdint>
#include <sys/ptrace.h>
#include <aio.h>

class breakpoint {
public:
    breakpoint() = default;
    breakpoint(pid_t pid, std::intptr_t addr) : m_pid{pid}, m_addr{addr}, m_enabled{false}, m_saved_data{} {}

    void enable();

    void disable();

    bool is_enabled() const;

    auto get_address() const -> std::intptr_t;
private:
    pid_t m_pid;
    std::intptr_t m_addr;
    bool m_enabled;
    uint8_t m_saved_data; //data which used to be at the breakpoint address
};
