#include <iostream>
#include <memory>
#include "CommandInterface.h"
#include "PcapFileDevice.h"
#include "PcapLiveDeviceList.h"
#include "SystemUtils.h"
#include "PcapDevice.h"
#include "log4cplus/configurator.h"
#include "log4cplus/loggingmacros.h"
#include <filesystem>

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

void clearLogConf(const std::string& filePath) {
    std::ofstream ofs;
    ofs.open(filePath, std::ofstream::out | std::ofstream::trunc);
    ofs.close();
}

int main() {
    clearLogConf("info.log");
    log4cplus::PropertyConfigurator::doConfigure("../config/logger.cfg");
    auto mainLogger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("main"));

    CommandInterface cmd;
    cmd.run();

    return 0;
}
