#!/bin/bash

DIR_PATH=$(pwd)

/usr/bin/cmake --no-warn-unused-cli -DCMAKE_BUILD_TYPE:STRING=Debug -DBUILD_TESTS:STRING=0 -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -DPCAPPP_BUILD_EXAMPLES=ON -DCMAKE_C_COMPILER:FILEPATH=/usr/bin/gcc -DCMAKE_CXX_COMPILER:FILEPATH=/usr/bin/g++ -S$DIR_PATH -B$DIR_PATH/build && /usr/bin/cmake --build $DIR_PATH/build --config Debug --target all && mkdir .settings && rm .settings/compile_commands.json && ln -sf $DIR_PATH/build/compile_commands.json .settings/
