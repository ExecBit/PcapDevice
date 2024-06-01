#pragma once

#include "PcapDevice.h"

class CommandInterface {
public:
    CommandInterface();

    void run();

private:
    void processCommand(const std::string& command);

    PcapDevice timer;
};

