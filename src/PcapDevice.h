#pragma once
#include "PcapFileDevice.h"
#include "PcapLiveDeviceList.h"
#include "log4cplus/logger.h"

class PcapDevice {

struct Counter {
    int count{0};
    pcpp::RawPacketVector m_packetVec;
};

public:
	PcapDevice();
	void init();
	void startCapturing(int time = 600);
	void stopCapturing();
	std::string listOfNetworkInterfaces();
	std::string baseInfo();
	void setNameOfNetworkInterface(const std::string& name);
	std::vector<std::string> convertor();
	std::atomic<bool> capturing{false};
	static constexpr bool isSaveFile{false};
	static void onPacketArrivesAsync(pcpp::RawPacket* packet, pcpp::PcapLiveDevice* dev, void* cookie);

	std::atomic<bool> running{false};
	std::atomic<bool> stop_requested;
	std::atomic<int> timer_seconds;
	std::thread timer_thread;
	std::mutex mtx;
	std::condition_variable cv;

	void start();
	void stop();

private:
	pcpp::PcapLiveDevice* m_device{nullptr};
	pcpp::RawPacketVector m_packetVec;
	std::string m_interfaceIP{"10.10.0.146"};
	std::string m_nameOfNetworkInterface{"none"};
	log4cplus::Logger m_logger;
	Counter m_stats;
};
