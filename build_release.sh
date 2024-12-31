#!/bin/bash
cmake --preset=linux && cmake --build build --parallel 4 && cmake --install build
