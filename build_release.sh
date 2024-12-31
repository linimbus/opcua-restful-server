#!/bin/bash
cmake --preset=release && cmake --build build --parallel 4 && cmake --install build
