#!/bin/bash

/usr/bin/cmake --no-warn-unused-cli -DCMAKE_BUILD_TYPE:STRING=Debug -DBUILD_TESTS:STRING=0 -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -DCMAKE_C_COMPILER:FILEPATH=/usr/bin/gcc -DCMAKE_CXX_COMPILER:FILEPATH=/usr/bin/g++ -S/home/ilyasha/projects/tuiDraft -B/home/ilyasha/projects/tuiDraft/build && /usr/bin/cmake --build /home/ilyasha/projects/tuiDraft/build --config Debug --target all --
