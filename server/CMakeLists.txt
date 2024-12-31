include_directories(include)

file(GLOB_RECURSE SRC_FILES src/*.cc)

set(EXECUTABLE_NAME "opcua-restful-server")

set(VCPKG_INSTALLED_DIR "${CMAKE_SOURCE_DIR}/build/vcpkg_installed")

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    include_directories("${CMAKE_SOURCE_DIR}/mysql/include/jdbc")
    link_directories("${CMAKE_SOURCE_DIR}/mysql/libs")
    set(DEPENDS_LIBS 
        ${VCPKG_INSTALLED_DIR}/x64-windows/lib/workflow.lib
        "mysqlcppconn.lib" "WS2_32.lib"
    )
else()
    set(DEPENDS_LIBS
        ${VCPKG_INSTALLED_DIR}/x64-linux/lib/libworkflow.a
        mysqlcppconn
    )
endif()

add_executable(${EXECUTABLE_NAME} ${SRC_FILES})

find_package(glog CONFIG REQUIRED)
find_package(gflags CONFIG REQUIRED)
find_package(open62541 CONFIG REQUIRED)
find_package(RapidJSON CONFIG REQUIRED)
find_package(workflow CONFIG REQUIRED)

target_link_libraries(${EXECUTABLE_NAME} PRIVATE
    glog::glog gflags::gflags rapidjson
    open62541::open62541
    ${DEPENDS_LIBS}
)

install(TARGETS ${EXECUTABLE_NAME} DESTINATION bin
    PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ
                GROUP_EXECUTE GROUP_READ
                WORLD_EXECUTE WORLD_READ)