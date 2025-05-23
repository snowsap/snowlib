cmake_minimum_required (VERSION 4.0)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_STANDARD_REQUIRED True)

if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("SnowLib")


add_executable (SnowLib "main.cpp" "main.h" )

target_include_directories(SnowLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/window ${CMAKE_CURRENT_SOURCE_DIR})


target_sources( 
	SnowLib PUBLIC 
	main.cpp
	main.h 
	window/window.h
	window/window.cpp
)

find_package(glfw3 CONFIG REQUIRED)	
target_link_libraries(SnowLib PUBLIC glfw)

find_package(glad CONFIG REQUIRED)
target_link_libraries(SnowLib PUBLIC glad::glad)

find_package(OpenGL REQUIRED)
target_link_libraries(SnowLib PUBLIC OpenGL::GL)
