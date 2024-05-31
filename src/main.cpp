#include <iostream>
#include <memory>
#include "PcapFileDevice.h"
#include "PcapLiveDeviceList.h"
#include "SystemUtils.h"
#include "PcapDevice.h"
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include "log4cplus/configurator.h"
#include "log4cplus/loggingmacros.h"


#include <cmath>       // for sin
using namespace ftxui;

bool containsWord(const std::string& str1, const std::string& str2) {
    std::istringstream iss(str2);
    std::string word;
    while (iss >> word) {
        if (str1.find(word) != std::string::npos) {
            return true;
        }
    }
    return false;
}
void printUsage() {
	std::cout << std::endl
		<< "Options:" << std::endl
		<< std::endl
		<< "    start start_capture_traffic        : Input directory" << std::endl
		<< "    stop  stop_capture_traffic         : Don't include sub-directories (default is include them)" << std::endl
		<< "    print print traffic  : Criteria to search in Berkeley Packet Filter (BPF) syntax (http://biot.com/capstats/bpf.html)" << std::endl
		<< std::endl;
}

int main() {
    log4cplus::PropertyConfigurator::doConfigure("../config/logger.cfg");
    auto mainLogger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("main"));

    PcapDevice pdev;
    auto listDev = pdev.listOfNetworkInterfaces();

    std::cout << "List of Network Interfaces\n" <<
    "=========================\n" + listDev + "=========================\n";

    std::string input_int;
    while (true) {
        std::cout << "Please, enter interface: ";

        std::getline(std::cin, input_int);

        if (containsWord(input_int, listDev)) {
            break;
        }
        std::cout << "Wrong interface, try again\n";
    }

    std::string name;
    if (auto it = listDev.find(input_int); it != std::string::npos) {
        name = listDev.substr(it, 6);
        LOG4CPLUS_INFO(mainLogger, LOG4CPLUS_TEXT("Entered network interface:\t" + listDev.substr(it, 6)));
    }

    pdev.setNameOfNetworkInterface(name);
    pdev.init();

    while (true) {
        printUsage();

        std::string input;
        std::getline(std::cin, input);

        if (input == "start") {
            if (!pdev.capturing) {
                std::cout << "Start Capturing....." << std::endl;
                std::thread([&] { pdev.startCapturing(); }).detach();
                continue;
            }
            std::cout << "ERROR: capturing is going" << std::endl;
        } else if (input == "stop") {
            if (pdev.capturing) {
                std::cout << "Stop capturing...." << std::endl;
                pdev.stopCapturing();
                continue;
            }
            std::cout << "ERROR: capturing is not going" << std::endl;
        } else if (input == "print"){
            std::cout << "printing packages" << std::endl;
        } else if (input == "q"){
            std::cout << "Выход из программы." << std::endl;
            break;
        }
    }

    return 0;
}
