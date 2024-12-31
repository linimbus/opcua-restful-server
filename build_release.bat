set http_proxy=http://192.168.3.3:8090
set https_proxy=http://192.168.3.3:8090
set HTTP_PROXY=http://192.168.3.3:8090
set HTTPS_PROXY=http://192.168.3.3:8090

cmake --preset=win64 && cmake --build build
