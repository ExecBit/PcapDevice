#include <iostream>
#include <memory>
#include "PcapFileDevice.h"
#include "PcapLiveDeviceList.h"
#include "SystemUtils.h"
#include "PcapDevice.h"
#include "log4cplus/configurator.h"
#include "log4cplus/loggingmacros.h"

void clearScreen() {
    // Используем escape-последовательность для очистки экрана
    std::cout << "\033[2J\033[1;1H";
}

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
	std::cout 
		<< "Options:" << std::endl
		<< std::endl
		<< "    start\t:Start capture traffic\n"
		<< "    stop\t:Stop capture traffic\n" 
		<< "    print\t:Print traffic\n"
		<< "    opt\t:Options\n"
		<< "    help\t:Print info\n"
		<< "    status\t:Print status\n"
		<< "    q   \t:Exit"
		<< std::endl;
}

void printOptions() {
	std::cout 
		<< "Options:" << std::endl
		<< std::endl
		<< "    fil\t:Set Filter\n"
		<< "    chin\t:Change interface\n" 
		<< "    print\t:Print traffic\n"
		<< "    help\t:Print info\n"
		<< "    status\t:Print status\n"
		<< "    q   \t:Exit"
		<< std::endl;
}

void options(PcapDevice& pdev) {
    printOptions();
    while (true) {
        std::cout << "command: ";
        std::string input;
        std::getline(std::cin, input);

        clearScreen();
        if (input == "fil") {
            std::cout << "Set filter" << std::endl;
            continue;
        } else if (input == "chin") {
            std::cout << "Change network interface" << std::endl;
            continue;
        } else if (input == "print"){
            std::cout << "printing packages" << std::endl;
            for (const auto& item : pdev.convertor()) {
                std::cout << item << '\n';
            }
            continue;
        } else if (input == "help"){
            printOptions();
            continue;
        } else if (input == "q"){
            std::cout << "Exit" << std::endl;
            break;
        std::cout << "Wrong command, try again" << std::endl;
        }
    }
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

    printUsage();
    while (true) {
        std::cout << "command: ";
        std::string input;
        std::getline(std::cin, input);

        clearScreen();
        if (input == "status") {
            if (pdev.capturing) {
                std::cout << "Capturing is going" << std::endl;
                continue;
            }
            std::cout << "Capturing is not going" << std::endl;
            continue;
        } else if (input == "start") {
            if (!pdev.capturing) {
                std::cout << "Start Capturing....." << std::endl;
                std::thread([&] { pdev.startCapturing(); }).detach();
                continue;
            }
            std::cout << "ERROR: capturing is going" << std::endl;
        } else if (input == "stop") {
            if (pdev.capturing) {
                std::cout << "Stop capturing...." << std::endl;
                const auto numberCapPac = pdev.stopCapturing();
                std::cout << numberCapPac << " packets captured" << std::endl;
                continue;
            }
            std::cout << "ERROR: capturing is not going" << std::endl;
        } else if (input == "opt") {
            std::cout << "==========options=========" << std::endl;
            options(pdev);
            continue;
        } else if (input == "print"){
            std::cout << "printing packages" << std::endl;
            for (const auto& item : pdev.convertor()) {
                std::cout << item << '\n';
            }
            continue;
        } else if (input == "help"){
            printUsage();
            continue;
        } else if (input == "q"){
            std::cout << "Exit" << std::endl;
            break;
        std::cout << "Wrong command, try again" << std::endl;
        }
    }

    return 0;
}

/*
#include <iostream>
#include <memory>
#include "PcapFileDevice.h"
#include "PcapLiveDeviceList.h"
#include "SystemUtils.h"
#include "PcapDevice.h"
#include "log4cplus/configurator.h"
#include "log4cplus/loggingmacros.h"

void clearScreen() {
    // Используем escape-последовательность для очистки экрана
    std::cout << "\033[2J\033[1;1H";
}

struct Counter {
    int count{0};
};

static void onPacketArrives(pcpp::RawPacket* packet, pcpp::PcapLiveDevice* dev, void* cookie)
{
    // extract the stats object form the cookie
    Counter* stats = (Counter*)cookie;

    // parsed the raw packet
    pcpp::Packet parsedPacket(packet);

    std::stringstream strm;
    for (auto curLayer = parsedPacket.getFirstLayer(); curLayer != NULL; curLayer = curLayer->getNextLayer())
    {
        strm << "No. " << stats->count << '\t' 
            << "Layer type: " << curLayer->getProtocol() << "; " // get layer type
            << "Total data: " << curLayer->getDataLen() << " [bytes]; " // get total length of the layer
            << "Layer data: " << curLayer->getHeaderLen() << " [bytes]; " // get the header length of the layer
            << "Layer payload: " << curLayer->getLayerPayloadSize() << " [bytes]\n"; // get the payload length of the layer (equals total length minus header length)
    }
    stats->count += 1;
    std::cout << strm.str();
    // collect stats from packet
 //   stats->consumePacket(parsedPacket);
}

int main() {
    log4cplus::PropertyConfigurator::doConfigure("../config/logger.cfg");
    auto mainLogger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("main"));

    auto device = pcpp::PcapLiveDeviceList::getInstance().getPcapLiveDeviceByName("enp8s0");

    if (!device) {
        exit(-1);
    }

    if (!device->open()) {
        exit(-1);
    }

    Counter k;

    device->startCapture(onPacketArrives, &k);

    pcpp::multiPlatformSleep(5);

    device->stopCapture();

    return 0;
}
*/
