#pragma once
#include <algorithm>
#include <iostream>
#include <memory>
#include "stdlib.h"
#include "PcapFileDevice.h"

class ReadPcapDevice {
public:
    bool init(const std::string& name);
    std::string scanPcapFiles();
    void open();
    void setFilter();
    std::string print();
    bool containsWord(const std::string& str1, const std::string& str2);

private:
    pcpp::IFileReaderDevice*  m_reader{nullptr};
    std::string m_currFileName;
    std::string m_filter;
    unsigned m_count{0};

};
