cmake_minimum_required(VERSION 3.20)

project(monsys)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)  

find_package(Crow CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(cxxopts CONFIG REQUIRED)


add_executable(app
    src/main.cpp
)

target_include_directories(app PRIVATE include)

target_sources(app PRIVATE
    src/readers/CpuStatReader.cpp
    src/readers/DiskStatReader.cpp
    src/readers/MemoryStatReader.cpp
    src/readers/NetworkStatReader.cpp
    src/readers/ProcessStatReader.cpp

    src/server/WebsocketManager.cpp
    src/server/routes.cpp
    
    src/StatCollector.cpp
    src/Calculators.cpp
)

target_link_libraries(app PRIVATE
    Crow::Crow
    nlohmann_json::nlohmann_json
    cxxopts::cxxopts
)

target_compile_definitions(app PRIVATE
    $<$<CONFIG:Debug>:ENABLE_DEBUG_ROUTES>
    $<$<CONFIG:Release>:ENABLE_RELEASE_PORTS>
)