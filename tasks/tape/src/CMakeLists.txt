add_library(tape_lib "")

target_sources(tape_lib 
	PRIVATE
		sort_tape.cpp
		tape.cpp
	PUBLIC
		sort_tape.hpp
		tape.hpp
)

target_include_directories(tape_lib PUBLIC
	"${PROJECT_SOURCE_DIR}/src"
)

# target_include_directories(tape_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

include(FetchContent)
FetchContent_Declare(
  tomlplusplus
  GIT_REPOSITORY https://github.com/marzer/tomlplusplus.git
  GIT_TAG        v3.4.0
)
FetchContent_MakeAvailable(tomlplusplus)

target_link_libraries(tape_lib 
	PRIVATE 
	tomlplusplus::tomlplusplus
)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/tape.toml ${CMAKE_CURRENT_BINARY_DIR}/tape.toml COPYONLY)


set(TAPE_TOML ${CMAKE_SOURCE_DIR}/tape/tape.toml)


