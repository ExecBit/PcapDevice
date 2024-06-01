#include <iostream>
#include "PcapDevice.h"
#include "PcapFileDevice.h"
#include "PcapLiveDeviceList.h"
#include "SystemUtils.h"
#include <sstream>

#include "log4cplus/loggingmacros.h"

PcapDevice::PcapDevice() : m_logger(log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("PcapDevice")))  {
    LOG4CPLUS_INFO(m_logger, LOG4CPLUS_TEXT("PcapDevice instance created."));
}

void PcapDevice::setNameOfNetworkInterface(const std::string& name) {
    LOG4CPLUS_INFO(m_logger, LOG4CPLUS_TEXT("PcapDevice set name of network interface: " + name));
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
    m_device = pcpp::PcapLiveDeviceList::getInstance().getPcapLiveDeviceByName(m_nameOfNetworkInterface);
    if (!m_device) {
        LOG4CPLUS_FATAL(m_logger, LOG4CPLUS_TEXT("Cannot find interface with name: " + m_nameOfNetworkInterface));
        exit(-1);
    }

    if (!m_device->open()) {
        LOG4CPLUS_FATAL(m_logger, LOG4CPLUS_TEXT("Cannot open device"));
        exit(-1);
    }
}

void PcapDevice::startCapturing(int time) {
    LOG4CPLUS_INFO(m_logger, LOG4CPLUS_TEXT("Starting capture"));
    m_device->startCaptureBlockingMode(onPacketArrives, &m_stats, time);
    capturing = true;

    stopCapturing();
}

void PcapDevice::stopCapturing() {
    m_device->stopCapture();
    m_packetVec = m_stats.m_packetVec; 

    capturing = false;
    LOG4CPLUS_INFO(m_logger, LOG4CPLUS_TEXT("Stop capture"));

    pcpp::PcapFileWriterDevice pcapWriter("output.pcap");
    if (!pcapWriter.open()) {
        LOG4CPLUS_WARN(m_logger, LOG4CPLUS_TEXT("Cannot open output.pcap for writing"));
        return;
    }

    pcapWriter.writePackets(m_stats.m_packetVec);

    std::cout << m_stats.count << " packets captured" << std::endl;
}

std::vector<std::string> PcapDevice::convertor() {
    if (capturing) {
        return {};
    }
    std::vector<std::string> res; 
    std::stringstream strm;
    unsigned count{0};
    for (const auto& packet : m_packetVec) {
        pcpp::Packet parsedPacket(packet);
        for (auto curLayer = parsedPacket.getFirstLayer(); curLayer != NULL; curLayer = curLayer->getNextLayer())
        {
            strm << "No. " << count << '\t' 
                << "Layer type: " << curLayer->getProtocol() << "; " // get layer type
                << "Total data: " << curLayer->getDataLen() << " [bytes]; " // get total length of the layer
                << "Layer data: " << curLayer->getHeaderLen() << " [bytes]; " // get the header length of the layer
                << "Layer payload: " << curLayer->getLayerPayloadSize() << " [bytes]\n"; // get the payload length of the layer (equals total length minus header length)
        }
        ++count;
        res.push_back(strm.str());
    }
    return res;
}

bool PcapDevice::onPacketArrives(pcpp::RawPacket* packet, pcpp::PcapLiveDevice* dev, void* cookie)
{
    // extract the stats object form the cookie
    Counter* stats = (Counter*)cookie;
    // parsed the raw packet
    auto* newPack = new pcpp::RawPacket(*packet);
    stats->m_packetVec.pushBack(newPack);
    pcpp::Packet parsedPacket(packet);

    std::stringstream strm;
    strm << "No. " << stats->count << '\t' << parsedPacket.toString() << '\n';
    for (auto curLayer = parsedPacket.getFirstLayer(); curLayer != NULL; curLayer = curLayer->getNextLayer())
    {
///     strm << "No. " << stats->count << '\t' 
///         << "Layer type: " << curLayer->getProtocol() << "; " // get layer type
///         << "Total data: " << curLayer->getDataLen() << " [bytes]; " // get total length of the layer
///         << "Layer data: " << curLayer->getHeaderLen() << " [bytes]; " // get the header length of the layer
///         << "Layer payload: " << curLayer->getLayerPayloadSize() << " [bytes]\n"; // get the payload length of the layer (equals total length minus header length)
    }
    stats->count += 1;
    std::cout << strm.str();
    // collect stats from packet
 //   stats->consumePacket(parsedPacket);
    return false;
}

std::string baseInfo() {
    return "";
}
