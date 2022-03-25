cmake_minimum_required(VERSION 3.19)

CPMAddPackage(
  NAME googletest
  GITHUB_REPOSITORY google/googletest
  GIT_TAG release-1.11.0
  VERSION 1.11.0
  OPTIONS "INSTALL_GTEST OFF" "gtest_force_shared_crt"
)

enable_testing()

add_subdirectory(mock)
add_subdirectory(persistance)
add_subdirectory(reward)
add_subdirectory(nexus_http_interface)
add_subdirectory(pool)
