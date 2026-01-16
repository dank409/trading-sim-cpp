cmake_minimum_required(VERSION 3.16)
project(trading_sim VERSION 1.0.0 LANGUAGES CXX)

# C++17 standard required
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable warnings for better code quality
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
    add_compile_options(-Wall -Wextra -Wpedantic)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    add_compile_options(/W4)
endif()

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Source files
set(SOURCES
    src/Asset.cpp
    src/Position.cpp
    src/Portfolio.cpp
    src/PriceHistory.cpp
    src/Market.cpp
    src/Order.cpp
    src/OrderBook.cpp
    src/MatchingEngine.cpp
    src/RiskManager.cpp
    src/TradeLog.cpp
    src/EquityCurve.cpp
    src/PerformanceStats.cpp
    src/AssetAnalytics.cpp
    src/ReportGenerator.cpp
    src/SimulationConfig.cpp
    src/Simulation.cpp
    src/InteractiveUI.cpp
    src/AutoRunner.cpp
    src/main.cpp
)

# Create executable
add_executable(trading_sim ${SOURCES})

# Set target properties
target_include_directories(trading_sim PRIVATE ${CMAKE_SOURCE_DIR}/include)
