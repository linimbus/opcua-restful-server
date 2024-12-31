#!/bin/bash

export http_proxy=http://192.168.3.3:8090
export https_proxy=http://192.168.3.3:8090
export HTTP_PROXY=http://192.168.3.3:8090
export HTTPS_PROXY=http://192.168.3.3:8090

cmake --preset=linux && cmake --build build --parallel 4 && cmake --install build
