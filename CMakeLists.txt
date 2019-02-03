cmake_minimum_required (VERSION 2.6)
project (cg_framework)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

if ("${THREADMAN_ROOT_PATH}" STREQUAL "")
	message(FATAL_ERROR "THREADMAN_ROOT_PATH is not set, but is required.")
else()
	message(STATUS "THREADMAN_ROOT_PATH: " ${THREADMAN_ROOT_PATH})
endif()


if (WIN32)
	if ("${CUSTOM_FREEGLUT_PATH}" STREQUAL "")
		message(FATAL_ERROR "CUSTOM_FREEGLUT_PATH is not set, but is required.")
	else()
		message(STATUS "CUSTOM_FREEGLUT_PATH: " ${CUSTOM_FREEGLUT_PATH})
	endif()
	#Because CMake is dumb, for some reason it will search for glut.lib in GLUT_ROOT_PATH/Release
	#but by default it is located in GLUT_ROOT_PATH/lib, so make sure to rename the lib folder to Release
	SET(GLUT_ROOT_PATH "${CUSTOM_FREEGLUT_PATH}")
else()
link_libraries(
	pthread
)
endif()

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

include_directories(
	${THREADMAN_ROOT_PATH}
	${OPENGL_INCLUDE_DIR}
	${GLUT_INCLUDE_DIR}
)

 
link_libraries(
	${OPENGL_LIBRARIES}
	${GLUT_LIBRARIES}
)

set (THREADMAN_HEADERS
	${THREADMAN_ROOT_PATH}/timer.h
	${THREADMAN_ROOT_PATH}/threadman.h
)

set(HEADERS
	defs.h
	color.h
	camera.h
	vector.h
	matrix.h
	${THREADMAN_HEADERS}
)

set(SOURCES 
	main.cpp
	camera.cpp
)

add_executable(cg_framework ${SOURCES} ${HEADERS})

if (WIN32)
add_custom_command(TARGET cg_framework POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${GLUT_ROOT_PATH}/bin/freeglut.dll"
		$<TARGET_FILE_DIR:cg_framework>)
endif()
