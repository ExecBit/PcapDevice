#pragma once

#include "PcapDevice.h"
#include <stack>

class CommandInterface {
public:
    CommandInterface();

    void run();

private:
    void processCommand(const std::string& command);
    void readFileMenu();

    void showMainMenu();
    void showCaptureMenu();
    void executeCommand(const std::string& command);

    std::unordered_map<std::string, std::function<void()>> mainMenuCommands;
    std::unordered_map<std::string, std::function<void()>> captureMenuCommands;
    std::stack<std::unordered_map<std::string, std::function<void()>>*> menuStack;

    void clearScreen();
    void prompt();
    void invalidCommand();

    PcapDevice timer;
};

