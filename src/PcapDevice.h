#pragma once
#include "PcapFileDevice.h"
#include "PcapFilter.h"
#include "PcapLiveDeviceList.h"
#include "log4cplus/logger.h"

class PcapDevice {

struct Counter {
    int count{0};
    std::string filter;
    pcpp::RawPacketVector packetVec;
};

public:
	PcapDevice();
	void init();
	std::string listOfNetworkInterfaces();
	void setNameOfNetworkInterface(const std::string& name);
	void setFilter();
	std::vector<std::string> convertor();
	static void onPacketArrivesAsync(pcpp::RawPacket* packet, pcpp::PcapLiveDevice* dev, void* cookie);

	void start();
	void stop();

private:
	void startCapturing();
	void stopCapturing();
	pcpp::PcapLiveDevice* m_device{nullptr};
	pcpp::RawPacketVector m_packetVec;
	std::string m_nameOfNetworkInterface{"none"};
	log4cplus::Logger m_logger;
	Counter m_stats;

	std::atomic<bool> m_running{false};
	std::atomic<bool> m_stopRequested;
	std::thread m_devThread;
	std::mutex m_mtx;
	std::condition_variable m_conVar;

};
