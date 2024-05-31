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

FetchContent_Declare(
  log4cplus
  GIT_REPOSITORY https://github.com/log4cplus/log4cplus.git
  GIT_TAG REL_2_1_1  # или конкретный тег или ветку
)
FetchContent_MakeAvailable(log4cplus)

FetchContent_Declare(
  CLI11
  GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git
  GIT_TAG v2.4.2  # или конкретный тег или ветку
)
FetchContent_MakeAvailable(CLI11)

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
                        log4cplus
                        CLI11::CLI11
)
