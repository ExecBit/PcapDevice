#include <iostream>
#include <memory>
#include "PcapFileDevice.h"
#include "PcapLiveDeviceList.h"
#include "SystemUtils.h"
#include "PcapDevice.h"
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include "log4cplus/configurator.h"
#include "log4cplus/loggingmacros.h"


#include <cmath>       // for sin
using namespace ftxui;

std::mutex packets_mutex;

int main() {
    log4cplus::PropertyConfigurator::doConfigure("../log.properties");
    auto mainLogger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("main"));

    PcapDevice pdev;
    auto listDev = pdev.listOfNetworkInterfaces();
    std::string name;
    if (auto it = listDev.find("enp8s0"); it != std::string::npos) {
        name = listDev.substr(it, 6);
        LOG4CPLUS_INFO(mainLogger, LOG4CPLUS_TEXT("Entered network interface:\t" + listDev.substr(it, 6)));
    }

    pdev.setNameOfNetworkInterface(name);
    pdev.init();

  auto screen = ScreenInteractive::Fullscreen();

  // ---------------------------------------------------------------------------
  // MAIN
  // ---------------------------------------------------------------------------
    int shift = 0;
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

  auto main = Renderer([&] {
    return vbox({
                hbox(captureButton->Render(), stopButton->Render()),
                separator(),
                vbox() | yframe | vscroll_indicator,
            });
  });


  // ---------------------------------------------------------------------------
  // settings
  // ---------------------------------------------------------------------------

  const std::vector<std::string> compiler_entries = {
      "gcc",
      "clang",
      "emcc",
      "game_maker",
      "Ada compilers",
      "ALGOL 60 compilers",
      "ALGOL 68 compilers",
      "Assemblers (Intel *86)",
      "Assemblers (Motorola 68*)",
      "Assemblers (Zilog Z80)",
      "Assemblers (other)",
      "BASIC Compilers",
      "BASIC interpreters",
      "Batch compilers",
      "C compilers",
      "Source-to-source compilers",
      "C++ compilers",
      "C# compilers",
      "COBOL compilers",
      "Common Lisp compilers",
      "D compilers",
      "DIBOL/DBL compilers",
      "ECMAScript interpreters",
      "Eiffel compilers",
      "Fortran compilers",
      "Go compilers",
      "Haskell compilers",
      "Java compilers",
      "Pascal compilers",
      "Perl Interpreters",
      "PHP compilers",
      "PL/I compilers",
      "Python compilers",
      "Scheme compilers and interpreters",
      "Smalltalk compilers",
      "Tcl Interpreters",
      "VMS Interpreters",
      "Rexx Interpreters",
      "CLI compilers",
  };

  int compiler_selected = 0;
  Component compiler = Radiobox(&compiler_entries, &compiler_selected);

  std::array<std::string, 8> options_label = {
      "-Wall",
      "-Werror",
      "-lpthread",
      "-O3",
      "-Wabi-tag",
      "-Wno-class-conversion",
      "-Wcomma-subscript",
      "-Wno-conversion-null",
  };
  std::array<bool, 8> options_state = {
      false, false, false, false, false, false, false, false,
  };

  std::vector<std::string> input_entries;
  int input_selected = 0;
  Component input = Menu(&input_entries, &input_selected);

  auto input_option = InputOption();
  std::string input_add_content;
  input_option.on_enter = [&] {
    input_entries.push_back(input_add_content);
    input_add_content = "";
  };
  Component input_add = Input(&input_add_content, "input files", input_option);

  std::string executable_content_ = "";
  Component executable_ = Input(&executable_content_, "executable");

  Component flags = Container::Vertical({
      Checkbox(&options_label[0], &options_state[0]),
      Checkbox(&options_label[1], &options_state[1]),
      Checkbox(&options_label[2], &options_state[2]),
      Checkbox(&options_label[3], &options_state[3]),
      Checkbox(&options_label[4], &options_state[4]),
      Checkbox(&options_label[5], &options_state[5]),
      Checkbox(&options_label[6], &options_state[6]),
      Checkbox(&options_label[7], &options_state[7]),
  });

  auto compiler_component = Container::Horizontal({
      compiler,
      flags,
      Container::Vertical({
          executable_,
          Container::Horizontal({
              input_add,
              input,
          }),
      }),
  });

  auto render_command = [&] {
    Elements line;
    // Compiler
    line.push_back(text(compiler_entries[compiler_selected]) | bold);
    // flags
    for (int i = 0; i < 8; ++i) {
      if (options_state[i]) {
        line.push_back(text(" "));
        line.push_back(text(options_label[i]) | dim);
      }
    }
    // Executable
    if (!executable_content_.empty()) {
      line.push_back(text(" -o ") | bold);
      line.push_back(text(executable_content_) | color(Color::BlueLight) |
                     bold);
    }
    // Input
    for (auto& it : input_entries) {
      line.push_back(text(" " + it) | color(Color::RedLight));
    }
    return line;
  };

  auto compiler_renderer = Renderer(compiler_component, [&] {
    auto compiler_win = window(text("Compiler"),
                               compiler->Render() | vscroll_indicator | frame);
    auto flags_win =
        window(text("Flags"), flags->Render() | vscroll_indicator | frame);
    auto executable_win = window(text("Executable:"), executable_->Render());
    auto input_win =
        window(text("Input"), hbox({
                                  vbox({
                                      hbox({
                                          text("Add: "),
                                          input_add->Render(),
                                      }) | size(WIDTH, EQUAL, 20) |
                                          size(HEIGHT, EQUAL, 1),
                                      filler(),
                                  }),
                                  separator(),
                                  input->Render() | vscroll_indicator | frame |
                                      size(HEIGHT, EQUAL, 3) | flex,
                              }));
    return vbox({
               hbox({
                   compiler_win,
                   flags_win,
                   vbox({
                       executable_win | size(WIDTH, EQUAL, 20),
                       input_win | size(WIDTH, EQUAL, 60),
                   }),
                   filler(),
               }) | size(HEIGHT, LESS_THAN, 8),
               hflow(render_command()) | flex_grow,
           }) |
           flex_grow;
  });

      auto analyze = Renderer([&] {

        return hbox({
                   vbox({
                       text("analyze") | flex,
                   }) | flex,
               }) |
               flex;
      });

    // ---------------------------------------------------------------------------
    // Tabs
    // ---------------------------------------------------------------------------

    int tab_index = 0;
    std::vector<std::string> tab_entries = {
      "main", "compiler", "analyze",
    };
    auto tab_selection =
      Menu(&tab_entries, &tab_index, MenuOption::HorizontalAnimated());
    auto tab_content = Container::Tab(
      {
          main,
          compiler_renderer,
          analyze
      },
      &tab_index);

    auto exit_button = Button(
      "Exit", [&] { screen.Exit(); }, ButtonOption::Animated());

    auto main_container = Container::Vertical({
      Container::Horizontal({
          tab_selection,
          exit_button,
      }),
      tab_content,
    });

    auto main_renderer = Renderer(main_container, [&] {
    return vbox({
        text("PcapDevice") | bold | hcenter,
        hbox({
            tab_selection->Render() | flex,
            exit_button->Render(),
        }),
        tab_content->Render() | flex,
    });
    });

    std::atomic<bool> refresh_ui_continue = true;
    std::thread refresh_ui([&] {
    while (refresh_ui_continue) {
      using namespace std::chrono_literals;
      std::this_thread::sleep_for(0.05s);
      // The |shift| variable belong to the main thread. `screen.Post(task)`
      // will execute the update on the thread where |screen| lives (e.g. the
      // main thread). Using `screen.Post(task)` is threadsafe.
      screen.Post([&] { shift++; });
      // After updating the state, request a new frame to be drawn. This is done
      // by simulating a new "custom" event to be handled.
      screen.Post(Event::Custom);
    }
    });

    screen.Loop(main_renderer);
    refresh_ui_continue = false;
    refresh_ui.join();
    return 0;
}
