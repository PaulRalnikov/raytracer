#!/usr/bin/env bash
mkdir build
cmake -DCMAKE_BUILD_TYPE=Release -B build
cmake --build build
