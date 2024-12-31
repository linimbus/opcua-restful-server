include_directories(include)

file(GLOB_RECURSE SRC_FILES src/*.cc)

set(EXECUTABLE_NAME "opcua-restful-server")

set(VCPKG_INSTALLED_DIR "${CMAKE_SOURCE_DIR}/build/vcpkg_installed")

add_executable(${EXECUTABLE_NAME} ${SRC_FILES})

find_package(glog CONFIG REQUIRED)
find_package(gflags CONFIG REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(open62541 CONFIG REQUIRED)
find_package(RapidJSON CONFIG REQUIRED)
find_package(workflow CONFIG REQUIRED)
find_package(unofficial-mysql-connector-cpp CONFIG REQUIRED)

target_link_libraries(${EXECUTABLE_NAME} PRIVATE
    glog::glog gflags::gflags rapidjson
    open62541::open62541
    unofficial::mysql-connector-cpp::connector
    unofficial::mysql-connector-cpp::connector-jdbc

    ${VCPKG_INSTALLED_DIR}/x64-linux/lib/libworkflow.a
    ${VCPKG_INSTALLED_DIR}/x64-linux/lib/libmysqlclient.a
    OpenSSL::SSL OpenSSL::Crypto
    ${VCPKG_INSTALLED_DIR}/x64-linux/lib/libz.a
    ${VCPKG_INSTALLED_DIR}/x64-linux/lib/libzstd.a
    ${VCPKG_INSTALLED_DIR}/x64-linux/lib/libncurses++.a
    -lresolv
)

install(TARGETS ${EXECUTABLE_NAME} DESTINATION bin
    PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ
                GROUP_EXECUTE GROUP_READ
                WORLD_EXECUTE WORLD_READ)