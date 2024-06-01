#include "CommandInterface.h"
#include <iostream>
#include <string>
#include <cstdlib>

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
		<< "    opt\t:Options\n"
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
    } else if (command == "x") {
        timer.stop();
    } else if (command == "q") {
        exit(0);
    } else {
        std::cout << "Invalid command" << std::endl;
    }
}
