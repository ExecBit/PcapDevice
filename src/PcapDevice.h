#pragma once
#include "PcapFileDevice.h"
#include "PcapLiveDeviceList.h"
#include "log4cplus/logger.h"

class PcapDevice {
public:
	PcapDevice();
	void init();
	void startCapturing(int time = 600);
	size_t stopCapturing();
	std::string listOfNetworkInterfaces();
	std::string baseInfo();
	void setNameOfNetworkInterface(const std::string& name);
	std::vector<std::string> convertor();
	std::atomic<bool> capturing{false};
	static constexpr bool isSaveFile{false};
	static void onPacketArrives(pcpp::RawPacket* packet, pcpp::PcapLiveDevice* dev, void* cookie);
	pcpp::PcapLiveDevice* m_device{nullptr};
private:
	pcpp::RawPacketVector m_packetVec;
	std::string m_interfaceIP{"10.10.0.146"};
	std::string m_nameOfNetworkInterface{"none"};
	log4cplus::Logger m_logger;
};
