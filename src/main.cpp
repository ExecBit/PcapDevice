#include <iostream>
#include <memory>
#include "PcapFileDevice.h"
#include "PcapLiveDeviceList.h"
#include "SystemUtils.h"
#include "PcapDevice.h"

int main() {
    PcapDevice pdev;
    pdev.init();
    pdev.startCapturing(10);

    return 0;
}
