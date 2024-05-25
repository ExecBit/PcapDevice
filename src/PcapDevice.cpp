#include <iostream>
#include "PcapDevice.h"
#include "PcapFileDevice.h"
#include "PcapLiveDeviceList.h"
#include "SystemUtils.h"

void PcapDevice::init() {
    m_device = pcpp::PcapLiveDeviceList::getInstance().getPcapLiveDeviceByIp(m_interfaceIP);
    if (!m_device) {
        std::cerr << "Cannot find interface with IPv4 address of '" << m_interfaceIP << "'" << std::endl;
        return;
    }

    if (!m_device->open()) {
        std::cerr << "Cannot open device" << std::endl;
        return;
    }
}

void PcapDevice::startCapturing(int time) {
    pcpp::RawPacketVector packetVec;

    std::cout << std::endl << "Starting capture with packet vector..." << std::endl;
    m_device->startCapture(packetVec);

    pcpp::multiPlatformSleep(time);

    m_device->stopCapture();

    std::cout << std::endl << "Stop capture" << std::endl;

    pcpp::PcapFileWriterDevice pcapWriter("output.pcap");
    if (!pcapWriter.open()) {
        std::cerr << "Cannot open output.pcap for writing" << std::endl;
        return;
    }

    pcapWriter.writePackets(packetVec);
}
