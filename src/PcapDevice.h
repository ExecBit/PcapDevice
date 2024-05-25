#pragma once
#include "PcapFileDevice.h"
#include "PcapLiveDeviceList.h"

class PcapDevice {
public:
	void init();
	void startCapturing(int time);
private:
	pcpp::PcapLiveDevice* m_device;
	std::string m_interfaceIP{"10.10.0.146"};
};
