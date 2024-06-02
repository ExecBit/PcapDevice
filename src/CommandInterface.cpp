#include "CommandInterface.h"
#include <iostream>
#include <string>
#include <cstdlib>

CommandInterface::CommandInterface() {
    // Initialize main menu commands
    mainMenuCommands = {
        {"q", [this]() { 
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
        {"default", []() { std::cout << "Setting option...\n"; }},
        {"chin", []() { std::cout << "Setting option...\n"; }},
        {"filter", [&]() { timer.setFilter(); }},
        {"x", [&]() { timer.stop(); }},
        {"start", [&]() { timer.start(); }}
    };

    readFileMenuCommands = {
        {"back", [this]() { 
            if (!menuStack.empty()) {
                menuStack.pop();
                showMainMenu();
            }
        }},
        {"default", []() { std::cout << "Setting option...\n"; }},
        {"print", [&]() { std::cout << m_readPcapDev.print(); }},
        {"setFilter", [&]() { m_readPcapDev.setFilter(); }},
        {"open", [&]() { m_readPcapDev.open(); }}
    };

    // Start in the main menu
    menuStack.push(&mainMenuCommands);
}

void CommandInterface::run() {
    timer.setNameOfNetworkInterface("enp8s0");
    timer.init();

    std::string command;

    while (true) {
//        clearScreen();
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
