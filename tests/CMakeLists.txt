cmake_minimum_required(VERSION 3.19)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/e2239ee6043f73722e7aa812a459f54a28552929.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_subdirectory(mock)
add_subdirectory(api)
add_subdirectory(persistance)
add_subdirectory(reward)
add_subdirectory(nexus_http_interface)
add_subdirectory(pool)
