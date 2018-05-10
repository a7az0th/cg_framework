cmake_minimum_required (VERSION 2.6)
project (opengl_test)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

if (WIN32)
	#Because CMake is dumb, for some reason it will search for glut.lib in GLUT_ROOT_PATH/Release
	#but by default it is located in GLUT_ROOT_PATH/lib, so make sure to rename the lib folder to Release
	SET(GLUT_ROOT_PATH "${CUSTOM_FREEGLUT_PATH}")
endif()

MESSAGE(STATUS "GLUT_ROOT_PATH: " ${GLUT_ROOT_PATH})


find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

include_directories(
	${OPENGL_INCLUDE_DIR}
	${GLUT_INCLUDE_DIR}
)
 
link_libraries(
	${OPENGL_LIBRARIES}
	${GLUT_LIBRARIES}
)

set(HEADERS
	defs.h
	color.h
	camera.h
	vector.h
	matrix.h
)

set(SOURCES 
	main.cpp
	camera.cpp
)

add_executable(opengl_test ${SOURCES} ${HEADERS})

if (WIN32)
add_custom_command(TARGET opengl_test POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${GLUT_ROOT_PATH}/bin/freeglut.dll"
		$<TARGET_FILE_DIR:opengl_test>)
endif()
