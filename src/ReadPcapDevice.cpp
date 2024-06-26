#include "ReadPcapDevice.h"
#include <filesystem>
#include <memory>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <RawPacket.h>
#include <Packet.h>
#include <PcapFileDevice.h>
#include <PcapPlusPlusVersion.h>
#include <SystemUtils.h>
#include <getopt.h>

bool ReadPcapDevice::init(const std::string& name) {
// open a pcap/pcapng file for reading
    auto* m_reader = pcpp::IFileReaderDevice::getReader(name);

    if (!m_reader) {
//        LOG4CPLUS_FATAL(m_logger, LOG4CPLUS_TEXT("Cannot find interface with name: " + m_nameOfNetworkInterface));
	return false;
    }

    if (!m_reader->open()) {
 //       LOG4CPLUS_FATAL(m_logger, LOG4CPLUS_TEXT("Cannot open device"));
	return false;
    }
    return true;
}

bool ReadPcapDevice::containsWord(const std::string& str1, const std::string& str2) {
    std::istringstream iss(str2);
    std::string word;
    while (iss >> word) {
        if (str1.find(word) != std::string::npos) {
            return true;
        }
    }
    return false;
}

void ReadPcapDevice::open(std::string& buf) {
    const auto files = scanPcapFiles();
    if (files.empty()) {
	buf += "Pcap files not found\n";	
	return;
    }

    std::cout << "==========List Of Files==========\n"
	<< files << "> ";

    std::string input;
    while (true) {
	std::cout << "\033[2J\033[1;1H";  // ANSI escape codes to clear screen
	std::cout << "==========List Of Files==========\n"
	<< files << "> ";

	std::cin >> input; 

	if (containsWord(input, files)) {
	    m_currFile = input;
	    break;
	}
        std::cout << "Wrong file, try again\n";
    }
    
    if (!init(input)) {
	std::cout << "ERROR: Can not read file\n";
    }
}

std::string ReadPcapDevice::scanPcapFiles() {
    // Получаем текущую директорию
    std::string ext1{".pcap"}, ext2{".pcapng"};
    std::filesystem::path currentPath = std::filesystem::current_path();

    std::stringstream strm;
    // Итерируемся по всем файлам в текущей директории
    for (const auto& entry : std::filesystem::directory_iterator(currentPath)) {
        if (entry.is_regular_file()) {
            std::string extension = entry.path().extension().string();
            if (extension == ext1 || extension == ext2) {
                strm << entry.path().filename().string() << std::endl;
            }
        }
    }
    return strm.str();
}

void ReadPcapDevice::setFilter(std::string& buf) {
    std::string input;
    std::cout << ": ";
    std::cin >> input;

    m_filter = input;

    buf += "Set filter: " + input + '\n';
}

std::string ReadPcapDevice::print() {
    pcpp::RawPacket rawPacket;
    std::stringstream strm;
    pcpp::PcapFileReaderDevice reader(m_currFile);
    if (!reader.open()) {
        std::cerr << "Error opening the pcap file: " << m_currFile << std::endl;
        return "";
    }

    if (!m_filter.empty() && !reader.setFilter(m_filter)) {
        reader.close();
        return "Error setting the filter: " + m_filter + '\n';
    }

    while (reader.getNextPacket(rawPacket)) {
	// parse the raw packet into a parsed packet
	pcpp::Packet parsedPacket(&rawPacket);

	// print packet to string
	strm << "\nNo. " + std::to_string(m_count++) + '\t' + parsedPacket.toString() + '\n';
    }

    reader.close();
    return strm.str();
}

