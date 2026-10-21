cmake_minimum_required(VERSION 3.16)
project(GhostOfTsushimaLivingWar VERSION 0.2.0 LANGUAGES C CXX)

# Static CRT everywhere on MSVC so players don't need a particular VC++ redistributable.
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

option(LW_BUILD_TOOLS "Build lwsim command-line simulator" ON)
option(LW_BUILD_TESTS "Build unit tests" ON)
option(LW_BUILD_PLUGIN "Build the Ghost of Tsushima ASI plugin (Windows/MSVC only)" ${WIN32})

if(MSVC)
  add_compile_options($<$<COMPILE_LANGUAGE:CXX>:/W4> $<$<COMPILE_LANGUAGE:CXX>:/permissive-> /utf-8 $<$<COMPILE_LANGUAGE:CXX>:/EHsc>)
else()
  add_compile_options(-Wall -Wextra -Wno-unused-parameter)
endif()

# ---- engine-agnostic core -------------------------------------------------------
add_library(lw_core STATIC
  core/src/json.cpp
  core/src/model.cpp
  core/src/config.cpp
  core/src/util.cpp
  core/src/names.cpp
  core/src/profile.cpp
  core/src/world.cpp
  core/src/save.cpp
  core/src/living_war.cpp
  core/src/report.cpp
)
target_include_directories(lw_core PUBLIC core/include)

# ---- tools ---------------------------------------------------------------------
if(LW_BUILD_TOOLS)
  add_executable(lwsim tools/lwsim/main.cpp tools/lwsim/mock_adapter.cpp)
  target_link_libraries(lwsim PRIVATE lw_core)
endif()

# ---- tests ---------------------------------------------------------------------
if(LW_BUILD_TESTS)
  enable_testing()
  add_executable(lw_tests tests/test_main.cpp tools/lwsim/mock_adapter.cpp)
  target_include_directories(lw_tests PRIVATE tools/lwsim)
  target_link_libraries(lw_tests PRIVATE lw_core)
  add_test(NAME lw_tests COMMAND lw_tests)
endif()

# ---- Ghost of Tsushima plugin (ASI, loaded by Ultimate ASI Loader) -------------
if(LW_BUILD_PLUGIN AND WIN32)
  add_subdirectory(plugin)
endif()
