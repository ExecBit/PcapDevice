#include <iostream>
#include <memory>
#include "PcapFileDevice.h"
#include "PcapLiveDeviceList.h"
#include "SystemUtils.h"
#include "PcapDevice.h"
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>

using namespace ftxui;

std::mutex packets_mutex;

int main() {
    PcapDevice pdev;
    auto listDev = pdev.listOfNetworkInterfaces();
    std::string name;
    if (auto it = listDev.find("enp8s0"); it != std::string::npos) {
        name = listDev.substr(it, 6);
        std::cout << "Entered network interface:\t" << listDev.substr(it, 6) << '\n';
    }

    pdev.setNameOfNetworkInterface(name);
    pdev.init();
//    pdev.startCapturing(10);

    auto screen = ScreenInteractive::Fullscreen();
    auto captureButton = Button("Start Capture", [&] {
        if (!pdev.capturing) {
            std::thread([&] { pdev.startCapturing(); }).detach();
        }
    });

    auto stopButton = Button("Stop Capture", [&] {
        if (pdev.capturing) {
            pdev.stopCapturing();
        }
    });

    auto exitButton = Button("Exit", screen.ExitLoopClosure());

    auto renderer = Renderer([&] {
        Elements packetElements;
        {
//            std::lock_guard<std::mutex> lock(packets_mutex);
            auto packets = pdev.convertor();
            for (const auto& packet : packets) {
                packetElements.push_back(text(packet));
            }
        }

        return vbox({
            hbox(captureButton->Render(), stopButton->Render(), exitButton->Render()),
            separator(),
            vbox() | yframe | vscroll_indicator,
        });
    });

    auto container = Container::Vertical({captureButton, stopButton, exitButton});
    auto mainComponent = Container::Vertical({
        container,
        renderer,
    });

    screen.Loop(container);
//    screen.Loop(mainComponent);
    auto packets = pdev.convertor();
    for (auto item : packets) {
        std::cout << item;
    }
    return 0;
}
