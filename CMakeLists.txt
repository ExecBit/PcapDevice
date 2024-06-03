cmake_minimum_required(VERSION 3.10)
project(PcapViewer)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)

FetchContent_Declare(
    PcapPlusPlus
    GIT_REPOSITORY https://github.com/seladb/PcapPlusPlus
    GIT_TAG v23.09
)
FetchContent_MakeAvailable(PcapPlusPlus)

FetchContent_Declare(
  log4cplus
  GIT_REPOSITORY https://github.com/log4cplus/log4cplus.git
  GIT_TAG REL_2_1_1
)
FetchContent_MakeAvailable(log4cplus)

file(GLOB_RECURSE SOURCES src/*.cpp src/*.h )
add_executable(pcap_viewer ${SOURCES})

target_include_directories(pcap_viewer PRIVATE 
    ${pcapplusplus_SOURCE_DIR}/3rdParty/PCAP 
    ${pcapplusplus_SOURCE_DIR}/Common 
    ${pcapplusplus_SOURCE_DIR}/Packet 
    ${pcapplusplus_SOURCE_DIR}/ProtocolParsers 
    ${pcapplusplus_SOURCE_DIR}/Pcap++)

target_link_libraries(pcap_viewer
                        Common++
                        Packet++
                        Pcap++
                        log4cplus
)
