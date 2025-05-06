include(FetchContent)
add_executable(test_tape test_tape.cpp)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.11.0
)
FetchContent_MakeAvailable(googletest)

FetchContent_Declare(
  tomlplusplus
  GIT_REPOSITORY https://github.com/marzer/tomlplusplus.git
  GIT_TAG        v3.4.0
)
FetchContent_MakeAvailable(tomlplusplus)

target_link_libraries(test_tape 
	GTest::gtest
	GTest::gtest_main 
	tomlplusplus::tomlplusplus
	tape_lib
)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/tape.toml ${CMAKE_CURRENT_BINARY_DIR}/tape.toml COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/input.txt ${CMAKE_CURRENT_BINARY_DIR}/input.txt COPYONLY)


add_test(NAME test_tape COMMAND test_tape WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})