cmake_minimum_required (VERSION 3.10.0)
project(arrow_table)

find_package(Arrow REQUIRED)
find_package(Boost REQUIRED)

set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -s -lrt -Werror -Wno-reorder -fPIC -std=c++14")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -s -lrt -Werror -Wno-reorder -fPIC -std=c++14")

add_executable(${CMAKE_PROJECT_NAME} 
	${CMAKE_SOURCE_DIR}/src/arrow_table.cpp
	${CMAKE_SOURCE_DIR}/src/parse_xml/src/parse_xml.cpp
)


target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC 
  ${CMAKE_SOURCE_DIR}/include
  ${CMAKE_SOURCE_DIR}/src/parse_xml/include
  ${CMAKE_SOURCE_DIR}/src/utils/log
)

target_link_libraries(${CMAKE_PROJECT_NAME} 
	PRIVATE Arrow::arrow_shared
	-lboost_system
	-lboost_filesystem
)
