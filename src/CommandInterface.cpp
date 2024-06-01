#include "CommandInterface.h"
#include <iostream>
#include <string>
#include <cstdlib>
/*
CommandInterface::CommandInterface() {}

void CommandInterface::run() {
    timer.setNameOfNetworkInterface("enp8s0");
    timer.init();

    std::string command;

    while (true) {
	std::cout 
                << "===================================\n"
		<< "Options:" << std::endl
		<< "    start\t:Start capture traffic\n"
		<< "    x\t:Stop capture traffic\n" 
		<< "    print\t:Print traffic\n"
		<< "    read\t:ReadFile\n"
		<< "    help\t:Print info\n"
		<< "    status\t:Print status\n"
		<< "    q   \t:Exit"
                << "\n===================================\n";
        std::cout << "command: ";
        std::cin >> command;

        // Clear the screen before processing the command
        std::system("clear");

        processCommand(command);
    }
}

void CommandInterface::readFileMenu() {

    std::string command;

    while (true) {
	std::cout 
                << "===================================\n"
		<< "FileMenu:" << std::endl
		<< "    start\t:Start capture traffic\n"
		<< "    x\t:Stop capture traffic\n" 
		<< "    print\t:Print traffic\n"
		<< "    read\t:ReadFile\n"
		<< "    help\t:Print info\n"
		<< "    status\t:Print status\n"
		<< "    q   \t:Exit"
                << "\n===================================\n";
        std::cout << "command: ";
        std::cin >> command;

        // Clear the screen before processing the command
        std::system("clear");

        processCommand(command);
    }
}

void CommandInterface::processCommand(const std::string& command) {
    if (command == "start") {
        timer.start();
    } else if (command == "stop") {
        timer.stop();
    } else if (command == "read") {
        readFileMenu(); 
    } else if (command == "x") {
        timer.stop();
    } else if (command == "q") {
        exit(0);
    } else {
        std::cout << "Invalid command" << std::endl;
    }
}
*/

CommandInterface::CommandInterface() {
    // Initialize main menu commands
    mainMenuCommands = {
        {"start", [&]() { timer.start(); }},
        {"x", [&]() { timer.stop(); }},
        {"print", []() { std::cout << "Printing...\n"; }},
        {"Capture", [this]() { 
            menuStack.push(&captureMenuCommands); 
            showCaptureMenu();
        }},
        {"q", [this]() { 
            std::cout << "Exiting...\n"; 
            exit(0); 
        }}
    };

    // Initialize options menu commands
    captureMenuCommands = {
        {"start", [&]() { timer.start(); }},
        {"x", [&]() { timer.stop(); }},
        {"setFilter", []() { std::cout << "Setting option...\n"; }},
        {"chin", []() { std::cout << "Setting option...\n"; }},
        {"back", [this]() { 
            if (!menuStack.empty()) {
                menuStack.pop();
                showMainMenu();
            }
        }}
    };

    // Start in the main menu
    menuStack.push(&mainMenuCommands);
}

/*
void CommandInterface::run() {
    while (true) {
        clearScreen();
        prompt();
        std::string command;
        std::cin >> command;
        executeCommand(command);
    }
}
*/

void CommandInterface::run() {
    timer.setNameOfNetworkInterface("enp8s0");
    timer.init();

    std::string command;

    while (true) {
////////std::cout 
////////        << "===================================\n"
////////	<< "Options:" << std::endl
////////	<< "    start\t:Start capture traffic\n"
////////	<< "    x\t:Stop capture traffic\n" 
////////	<< "    print\t:Print traffic\n"
////////	<< "    read\t:ReadFile\n"
////////	<< "    help\t:Print info\n"
////////	<< "    status\t:Print status\n"
////////	<< "    q   \t:Exit"
////////        << "\n===================================\n";
////////std::cout << "command: ";
        prompt();
        std::cin >> command;
        clearScreen();

        // Clear the screen before processing the command
//        std::system("clear");

        //processCommand(command);
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
    std::cout << "Options Menu:\n";
    for (const auto& command : captureMenuCommands) {
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
        }
    }
    std::cout << "> ";
}

void CommandInterface::invalidCommand() {
    std::cout << "Invalid command. Please try again.\n";
}
