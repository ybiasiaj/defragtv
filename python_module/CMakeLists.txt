cmake_minimum_required (VERSION 3.8)
set(CMAKE_CXX_STANDARD 20)

project ("ioq3_py_module")

message("fmt : ${fmt_DIR}")
message("python : ${python_DIR}")
message("pybind11 : ${pybind11_DIR}")

set(IOQ3_DIR "${PARENT_DIR}/ioq3")

set(IOQ3_DEBUG_BINARY_PATH "${IOQ3_DIR}/build/debug-msvc12-x86")
set(IOQ3_RELEASE_BINARY_PATH "${IOQ3_DIR}/build/debug-msvc12-x86")
set(IOQ3_BINARY_PATH "${IOQ3_DEBUG_BINARY_PATH}")
message("ioq3 : ${IOQ3_DIR}")

add_compile_definitions(
	-DPY_MODULE=1
	-DNOMINMAX
	-DWIN32_LEAN_AND_MEAN
)
add_library(ioq3_py_module STATIC
	"interpreter.hpp"
	"module.hpp"
	"module.cpp"
)
target_include_directories(ioq3_py_module PUBLIC
	"${fmt_DIR}/include"
	"${python_DIR}/include"
	"${pybind11_DIR}/include"
)
target_link_libraries(ioq3_py_module
	"${python_DIR}/libs/python37.lib"
)
