cmake_minimum_required(VERSION 3.3 FATAL_ERROR)

# For an anoying error
set(Geant4_DIR "/home/john/Programs/Geant4/geant4.10.06-install/lib/Geant4-10.6.2" )

project(muon_lab VERSION 0.1
  DESCRIPTION "Simulation of a muon lab experiment"
  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Wshadow \
-Wcast-align -fPIC -march=native -O0 -ggdb")

option(WITH_GEANT4_UIVIS "Build with Geant4 UI and Vis drivers" ON)
# Find Geant4
if(WITH_GEANT4_UIVIS)
  find_package(Geant4 REQUIRED ui_all vis_all)
else()
  find_package(Geant4 REQUIRED)
endif()

include(${Geant4_USE_FILE}) # Auto configure includes/flags
include_directories(${PROJECT_SOURCE_DIR}/include) # Add -I type paths

find_package(ROOT 6 REQUIRED)
include(${ROOT_USE_FILE})
include_directories(${ROOT_INCLUDE_DIRS})

find_package(gossip REQUIRED)
include_directories(${gossip_INCLUDE_DIRS})


# We presume the existence of three directories
file(GLOB_RECURSE sources ${PROJECT_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE headers ${PROJECT_SOURCE_DIR}/include/*.hh)
file(GLOB_RECURSE macros RELATIVE ${PROJECT_SOURCE_DIR} macros/*.mac)

# Enable macros for out-of-source build
foreach(_file ${macros})
  configure_file(
    ${_file}
    ${PROJECT_BINARY_DIR}/${_file}
    COPYONLY
    )
endforeach()

set(BME_SCRIPTS
  init_vis.mac
  vis.mac
  gui.mac
  )

foreach(_script ${BME_SCRIPTS})
  configure_file(
    ${_script}
    ${PROJECT_BINARY_DIR}/${_script}
    COPYONLY
    )
endforeach()
# Compile application
add_executable(muon_lab main.cpp ${sources} ${headers})
add_executable(waveform_generator waveform_generator.cpp)
# Link it to Geant4
target_link_libraries(muon_lab ${Geant4_LIBRARIES} ${ROOT_LIBRARIES} ${gossip_LIBRARIES})
target_link_libraries(waveform_generator ${ROOT_LIBRARIES} ${gossip_LIBRARIES})

install(TARGETS muon_lab DESTINATION bin)
