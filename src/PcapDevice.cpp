#include <iostream>
#include "PcapDevice.h"
#include "PcapFileDevice.h"
#include "PcapLiveDeviceList.h"
#include "SystemUtils.h"
#include <sstream>

void PcapDevice::setNameOfNetworkInterface(const std::string& name) {
    m_nameOfNetworkInterface = name;
}

std::string PcapDevice::listOfNetworkInterfaces() {
    auto devList = pcpp::PcapLiveDeviceList::getInstance().getPcapLiveDevicesList();
    std::string res;
    for (const auto& item : devList) {
        res += item->getName() + '\n';
    }
    return res;
}

void PcapDevice::init() {
    //m_device = pcpp::PcapLiveDeviceList::getInstance().getPcapLiveDeviceByIp(m_interfaceIP);
    m_device = pcpp::PcapLiveDeviceList::getInstance().getPcapLiveDeviceByName(m_nameOfNetworkInterface);
    if (!m_device) {
        std::cerr << "Cannot find interface with IPv4 address of '" << m_interfaceIP << "'" << std::endl;
        exit(-1);
    }

    if (!m_device->open()) {
        std::cerr << "Cannot open device" << std::endl;
        exit(-1);
    }
}

void PcapDevice::startCapturing(int time) {
    std::cout << std::endl << "Starting capture with packet vector..." << std::endl;
    m_device->startCapture(m_packetVec);
    capturing = true;
    pcpp::multiPlatformSleep(time);

    stopCapturing();
}

void PcapDevice::stopCapturing() {
    m_device->stopCapture();

    capturing = false;
    std::cout << std::endl << "Stop capture" << std::endl;

    pcpp::PcapFileWriterDevice pcapWriter("output.pcap");
    if (!pcapWriter.open()) {
        std::cerr << "Cannot open output.pcap for writing" << std::endl;
        return;
    }

    pcapWriter.writePackets(m_packetVec);
}
std::vector<std::string> PcapDevice::convertor() {
    if (capturing) {
        return {};
    }
    std::vector<std::string> res; 
    std::stringstream strm;
    for (const auto& packet : m_packetVec) {
        pcpp::Packet parsedPacket(packet);
        for (auto curLayer = parsedPacket.getFirstLayer(); curLayer != NULL; curLayer = curLayer->getNextLayer())
        {
            strm 
                << "Layer type: " << curLayer->getProtocol() << "; " // get layer type
                << "Total data: " << curLayer->getDataLen() << " [bytes]; " // get total length of the layer
                << "Layer data: " << curLayer->getHeaderLen() << " [bytes]; " // get the header length of the layer
                << "Layer payload: " << curLayer->getLayerPayloadSize() << " [bytes]"; // get the payload length of the layer (equals total length minus header length)
        }
        res.push_back(strm.str());
    }
    return res;
}
