cmake_minimum_required(VERSION 3.10)
project(PcapViewer)

set(CMAKE_CXX_STANDARD 17)

# FetchContent для FTXUI
include(FetchContent)

FetchContent_Declare(
  ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI.git
  GIT_TAG v5.0.0
)
FetchContent_MakeAvailable(ftxui)

FetchContent_Declare(
    PcapPlusPlus
    GIT_REPOSITORY https://github.com/seladb/PcapPlusPlus
    GIT_TAG v23.09
)
FetchContent_MakeAvailable(PcapPlusPlus)

file(GLOB_RECURSE SOURCES src/*.cpp src/*.h )
add_executable(pcap_viewer ${SOURCES})

# Линкирование библиотек PcapPlusPlus
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
                        ftxui::dom
                        ftxui::screen
                        ftxui::component
)
