cmake_minimum_required(VERSION 2.6)
project(decision_making)

# Check for Lua and add include path
find_package(Lua51 REQUIRED)
include_directories(${LUA_INCLUDE_DIR})

# Check for ARGoS3
find_package(PkgConfig)
pkg_check_modules(ARGOS REQUIRED argos3_simulator)

# Add ARGoS paths
include_directories(${ARGOS_INCLUDE_DIRS})
link_directories(${ARGOS_LIBRARY_DIRS})

# Create the loop function library
add_library(decision_making SHARED decision_making.h decision_making.cpp)
target_link_libraries(decision_making
  ${ARGOS_LIBRARIES}
  ${LUA_LIBRARIES}
  argos3plugin_simulator_entities
  argos3plugin_simulator_footbot
  argos3plugin_simulator_media)
