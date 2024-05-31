#pragma once
#include "PcapFileDevice.h"
#include "PcapLiveDeviceList.h"
#include "log4cplus/logger.h"

class PcapDevice {
public:
	PcapDevice();
	void init();
	void startCapturing(int time = 600);
	void stopCapturing();
	std::string listOfNetworkInterfaces();
	void setNameOfNetworkInterface(const std::string& name);
	std::vector<std::string> convertor();
	std::atomic<bool> capturing{false};
private:
	pcpp::RawPacketVector m_packetVec;
	pcpp::PcapLiveDevice* m_device{nullptr};
	std::string m_interfaceIP{"10.10.0.146"};
	std::string m_nameOfNetworkInterface{"none"};
	log4cplus::Logger m_logger;
};
