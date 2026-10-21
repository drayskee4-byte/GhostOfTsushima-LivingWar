# Ghost of Tsushima: Living War - ASI plugin (x64 Windows, MSVC)
include(FetchContent)
# MinHook (BSD-2): compiled straight into the plugin so it shares our static CRT.
FetchContent_Declare(minhook GIT_REPOSITORY https://github.com/TsudaKageyu/minhook.git GIT_TAG v1.3.3)
FetchContent_GetProperties(minhook)
if(NOT minhook_POPULATED)
  FetchContent_Populate(minhook)
endif()

add_library(LivingWar SHARED
  src/dllmain.cpp
  src/plugin.cpp
  src/got_adapter.cpp
  src/pattern.cpp
  src/ini.cpp
  src/recon.cpp
  ${minhook_SOURCE_DIR}/src/buffer.c
  ${minhook_SOURCE_DIR}/src/hook.c
  ${minhook_SOURCE_DIR}/src/trampoline.c
  ${minhook_SOURCE_DIR}/src/hde/hde64.c
)
target_include_directories(LivingWar PRIVATE ${minhook_SOURCE_DIR}/include)
target_link_libraries(LivingWar PRIVATE lw_core shell32 user32)
target_compile_definitions(LivingWar PRIVATE WIN32_LEAN_AND_MEAN NOMINMAX _CRT_SECURE_NO_WARNINGS)
# Ultimate ASI Loader loads *.asi from <game>\plugins\ (and the game root).
set_target_properties(LivingWar PROPERTIES SUFFIX ".asi" OUTPUT_NAME "LivingWar")
