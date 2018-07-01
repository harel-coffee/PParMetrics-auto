# Install script for directory: /home/s1736883/Work/NAS/nauseous

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/home/s1736883/Work/PParMetrics/install")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/." TYPE DIRECTORY MESSAGE_LAZY FILES "/home/s1736883/Work/NAS/nauseous/config")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/scripts" TYPE DIRECTORY MESSAGE_LAZY FILES "/home/s1736883/Work/NAS/nauseous/utils/scripts/install_tree/")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/preamble/" TYPE DIRECTORY MESSAGE_LAZY FILES "/home/s1736883/Work/PParMetrics/benchmarks/preamble/")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/home/s1736883/Work/PParMetrics/benchmarks/BT/cmake_install.cmake")
  include("/home/s1736883/Work/PParMetrics/benchmarks/CG/cmake_install.cmake")
  include("/home/s1736883/Work/PParMetrics/benchmarks/DC/cmake_install.cmake")
  include("/home/s1736883/Work/PParMetrics/benchmarks/EP/cmake_install.cmake")
  include("/home/s1736883/Work/PParMetrics/benchmarks/FT/cmake_install.cmake")
  include("/home/s1736883/Work/PParMetrics/benchmarks/IS/cmake_install.cmake")
  include("/home/s1736883/Work/PParMetrics/benchmarks/LU/cmake_install.cmake")
  include("/home/s1736883/Work/PParMetrics/benchmarks/MG/cmake_install.cmake")
  include("/home/s1736883/Work/PParMetrics/benchmarks/SP/cmake_install.cmake")
  include("/home/s1736883/Work/PParMetrics/benchmarks/UA/cmake_install.cmake")

endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "/home/s1736883/Work/PParMetrics/benchmarks/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
