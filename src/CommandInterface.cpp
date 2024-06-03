#include "CommandInterface.h"
#include <iostream>
#include <string>
#include <cstdlib>
#include "PcapDevice.h"
#include "log4cplus/configurator.h"
#include "log4cplus/loggingmacros.h"

CommandInterface::CommandInterface() {
    // Initialize main menu commands
    mainMenuCommands = {
        {"exit", [this]() { 
            std::cout << "Exiting...\n"; 
            exit(0); 
        }},
        {"read", [this]() { 
            menuStack.push(&readFileMenuCommands); 
            showReadFileMenu();
        }},
        {"capture", [this]() { 
            menuStack.push(&captureMenuCommands); 
            showCaptureMenu();
        }}
    };

    // Initialize options menu commands
    captureMenuCommands = {
        {"back", [this]() { 
            if (!menuStack.empty()) {
                menuStack.pop();
                showMainMenu();
            }
        }},
        {"chin", [&]() {
            std::string name;
            if (!networkInterfaceMenu(name)) {
                buf += "ERROR: wrong network interface\n";
            }
            
            timer.setNameOfNetworkInterface(name);
            buf += "Entered network interface - " + name + '\n';
        }},
        {"filter", [&]() { timer.setFilter(); }},
        {"x", [&]() { 
            timer.stop(buf); 
        }},
        {"start", [&]() { timer.start(buf); }}
    };

    readFileMenuCommands = {
        {"back", [this]() { 
            if (!menuStack.empty()) {
                menuStack.pop();
                showMainMenu();
            }
        }},
        {"print", [&]() { buf += m_readPcapDev.print(); }},
        {"filter", [&]() { m_readPcapDev.setFilter(buf); }},
        {"open", [&]() { m_readPcapDev.open(buf); }}
    };

    // Start in the main menu
    menuStack.push(&mainMenuCommands);
}

bool CommandInterface::containsWord(const std::string& str1, const std::string& str2) {
    std::istringstream iss(str2);
    std::string word;
    while (iss >> word) {
        if (str1.find(word) != std::string::npos) {
            return true;
        }
    }
    return false;
}

bool CommandInterface::networkInterfaceMenu(std::string& name) {
    clearScreen();
    auto listDev = timer.listOfNetworkInterfaces();

    std::cout << "List of Network Interfaces\n" <<
    "=========================\n" + listDev + "=========================\n";

    std::string input_int;
    while (true) {
        std::cout << "Please, enter interface: ";

        std::cin >> input_int;

        if (containsWord(input_int, listDev)) {
            break;
        }
        std::cout << "Wrong interface, try again\n";
    }

    if (auto it = listDev.find(input_int); it != std::string::npos) {
        name = listDev.substr(it, 6);
        return true;
    }

    return false;
}

void CommandInterface::run() {
    std::string name;
    if (!networkInterfaceMenu(name)) {
        std::cout << "ERROR: wrong network interface\n";
    }
    
    timer.setNameOfNetworkInterface(name);
    timer.init();


    std::string command;

    while (true) {
        clearScreen();
        if (!buf.empty()) {
            std::cout << "\n===========================\n"
                << buf 
                << "\n===========================\n";
            buf.clear();
        }

        prompt();
        std::cin >> command;

        executeCommand(command);
    }
}

void CommandInterface::showMainMenu() {
    std::cout << "Main Menu:\n";
    for (const auto& command : mainMenuCommands) {
        std::cout << " - " << command.first << "\n";
    }
}

void CommandInterface::showCaptureMenu() {
    std::cout << "Capture Menu:\n";
    for (const auto& command : captureMenuCommands) {
        std::cout << " - " << command.first << "\n";
    }
}

void CommandInterface::showReadFileMenu() {
    std::cout << "ReadFile Menu:\n";
    for (const auto& command : readFileMenuCommands) {
        std::cout << " - " << command.first << "\n";
    }
}

void CommandInterface::executeCommand(const std::string& command) {
    if (!menuStack.empty() && menuStack.top()->count(command)) {
        (*menuStack.top())[command]();
    } else {
        invalidCommand();
    }
}

void CommandInterface::clearScreen() {
    std::cout << "\033[2J\033[1;1H";  // ANSI escape codes to clear screen
}

void CommandInterface::prompt() {
    if (!menuStack.empty()) {
        if (menuStack.top() == &mainMenuCommands) {
            showMainMenu();
        } else if (menuStack.top() == &captureMenuCommands) {
            showCaptureMenu();
        } else if (menuStack.top() == &readFileMenuCommands) {
            showReadFileMenu();
        }
    }
    std::cout << "> ";
}

void CommandInterface::invalidCommand() {
    std::cout << "Invalid command. Please try again.\n";
}
