#include <iostream>
#include "PcapDevice.h"
#include "PcapFileDevice.h"
#include "PcapFilter.h"
#include "PcapLiveDeviceList.h"
#include "SystemUtils.h"
#include <sstream>
#include <string>

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

void PcapDevice::start() {
    if (!m_running) {
        m_stopRequested = false;
        m_devThread = std::thread(&PcapDevice::startCapturing, this);
        m_running = true;
        std::cout << "Capture started" << std::endl;
    } else {
        std::cout << "Capture is already running" << std::endl;
    }
}

void PcapDevice::stop() {
    if (m_running) {
        {
            std::lock_guard<std::mutex> lock(m_mtx);
            m_stopRequested = true;
        }
        m_conVar.notify_all();
        m_devThread.join();
        m_running = false;
    } else {
        std::cout << "Capture is not running" << std::endl;
    }
}

void PcapDevice::startCapturing() {
    LOG4CPLUS_INFO(m_logger, LOG4CPLUS_TEXT("Starting capture"));

    m_device->startCapture(onPacketArrivesAsync, &m_stats);
    while (!m_stopRequested) {
        {
            std::unique_lock<std::mutex> lock(m_mtx);
            m_conVar.wait_for(lock, std::chrono::seconds(1), [this] { return m_stopRequested.load(); });
            if (m_stopRequested) break;
        }
    }

    stopCapturing();
}

void PcapDevice::stopCapturing() {
    m_device->stopCapture();
    m_packetVec = m_stats.packetVec; 

    LOG4CPLUS_INFO(m_logger, LOG4CPLUS_TEXT("Stop capture"));

    pcpp::PcapFileWriterDevice pcapWriter("output.pcap");
    if (!pcapWriter.open()) {
        LOG4CPLUS_WARN(m_logger, LOG4CPLUS_TEXT("Cannot open output.pcap for writing"));
        return;
    }

    pcapWriter.writePackets(m_stats.packetVec);

    std::cout << m_stats.count << " packets captured" << std::endl;
}

std::vector<std::string> PcapDevice::convertor() {
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

void PcapDevice::onPacketArrivesAsync(pcpp::RawPacket* packet, pcpp::PcapLiveDevice* dev, void* cookie) {
    // extract the stats object form the cookie
    Counter* stats = (Counter*)cookie;

    pcpp::BPFStringFilter filter{stats->filter};
    if (!filter.matchPacketWithFilter(packet)) {
        return;
    }

    // parsed the raw packet
    auto* newPack = new pcpp::RawPacket(*packet);
    stats->packetVec.pushBack(newPack);
    pcpp::Packet parsedPacket(packet);

    std::cout << "\nNo. " + std::to_string(stats->count++) + '\t' + parsedPacket.toString() + '\n';
}

void PcapDevice::setFilter() {
    std::string input;
    std::cout << ": ";
    std::cin >> input;

    if (m_device->setFilter(input)) {
        m_stats.filter = input;
        LOG4CPLUS_INFO(m_logger, LOG4CPLUS_TEXT("Set filter " + input));
        return;
    }
    std::cout << "ERROR set filter\n";
    LOG4CPLUS_WARN(m_logger, LOG4CPLUS_TEXT("Set filter: " + input + " failed!"));
}
