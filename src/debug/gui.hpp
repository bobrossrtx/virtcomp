#pragma once
#include <vector>
#include <string>
#include <cstdint>

class Gui {
public:
    Gui(const std::string& title = "VirtComp Debugger");
    void set_title(const std::string& title);
    void run_vm(const std::vector<uint8_t>& program);
private:
    std::string window_title;
};
