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
    void open(std::string& buf);
    void setFilter(std::string& buf);
    std::string print();
    bool containsWord(const std::string& str1, const std::string& str2);

private:
    pcpp::IFileReaderDevice*  m_reader{nullptr};
    std::string m_filter;
    std::string m_currFile;
    unsigned m_count{0};

};
