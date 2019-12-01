# CMakeList.txt : CMake project for Il2CppHook, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Add source to this project's executable.
add_executable (Il2CppHook "src/Il2CppHook.cpp" "include/Il2CppHook.h")

add_library ("il2cpp-hook" SHARED "src/Il2CppHook.cpp" "src/utils/utils.cpp")

include_directories("include/" "~/Downloads/ndk-bundle/")
find_library(log-lib, log)

target_link_libraries("il2cpp-hook" ${log-lib})
