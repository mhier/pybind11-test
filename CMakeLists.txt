cmake_minimum_required(VERSION 3.16)
project(example)

set(PYBIND11_FINDPYTHON ON)
find_package(pybind11 REQUIRED)

find_program(STUBGEN stubgen REQUIRED)

add_library(MockApplicationCore SHARED MockApplicationCore.cpp)
target_link_libraries(MockApplicationCore PUBLIC pybind11::embed)

add_executable(example main.cpp)
target_link_libraries(example PRIVATE MockApplicationCore)

pybind11_add_module(PyApplicationCore PyApplicationCore.cpp)
target_link_libraries(PyApplicationCore PRIVATE MockApplicationCore)

configure_file(${CMAKE_SOURCE_DIR}/test.py ${CMAKE_BINARY_DIR}/test.py COPYONLY)

add_custom_command(TARGET PyApplicationCore POST_BUILD
  COMMAND ${STUBGEN} --include-docstrings -m PyApplicationCore -o .
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  COMMENT "Use stubgen to create .pyi for statement completion")

# install Python modules to correct platform-dependent directory (if installing to system prefix)
if("${CMAKE_INSTALL_PREFIX}" STREQUAL "/usr" OR "${CMAKE_INSTALL_PREFIX}" STREQUAL "/usr/local")
set(install_path ${Python_SITEARCH})
else()
set(install_path "lib/python${Python_VERSION_MAJOR}.${Python_VERSION_MINOR}/site-packages")
endif()

set_property(TARGET MockApplicationCore PROPERTY INSTALL_RPATH_USE_LINK_PATH TRUE)
set_property(TARGET PyApplicationCore PROPERTY INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/lib)

install(TARGETS PyApplicationCore LIBRARY DESTINATION ${install_path})
install(TARGETS MockApplicationCore LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/PyApplicationCore.pyi DESTINATION ${install_path})