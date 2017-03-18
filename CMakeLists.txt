PROJECT(M_threadpool)
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
#FIND_PACKAGE(libevnet REQUIRED)

ENABLE_TESTING()

SET(M_VERSION_MAJOR 1)
SET(M_VERSION_MINOR 0)
SET(M_VERSION_PATCH 0)
SET(M_VERSION ${M_VERSION_MAJOR}.${M_VERSION_MINOR}.${M_VERSION_PATCH})

# Default is OFF so that libary users can build it directly without patch file
OPTION(ENABLE_TEST "Build test files" ON)

SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
SET(TEST_BINARY test)
SET(EXAMPLE_BINARY ${PROJECT_NAME}_example)

# set compiler flags
IF(ENABLE_TEST)
    # disable optimization for testing version
    set(CCXX_FLAGS "-g2 -Wall -std=c++11 -fPIC -lpthread")
ELSE()
    set(CCXX_FLAGS "-O2 -Wall -std=c++11 -fPIC -lpthread")
ENDIF()

#for 32-bit build
#set(CCXX_FLAGS "-Wall -O2 -g -DNDEBUG -m32")
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CCXX_FLAGS} -march=native")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CCXX_FLAGS}")

MESSAGE(STATUS "start build")
MESSAGE(STATUS "CMAKE_CXX_FLAGS: " ${CMAKE_CXX_FLAGS})


ADD_SUBDIRECTORY(src)
IF(ENABLE_TEST)
    ADD_SUBDIRECTORY(example)
ENDIF()

SEPARATE_ARGUMENTS(CMD_ARGS)
# ADD_TEST(delivery_test ${PROJECT_SOURCE_DIR}/bin/${TEST_BINARY})

ADD_CUSTOM_TARGET(example
	COMMAND ./${EXAMPLE_BINARY} ${CMD_ARGS}
	WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}
	COMMENT "Run examples"
)


ADD_CUSTOM_TARGET(memcheck
	COMMAND command -v valgrind >/dev/null 2>&1 && valgrind -v --leak-check=full --show-reachable=yes
	--undef-value-errors=yes --track-origins=yes --num-callers=50 ./${EXAMPLE_BINARY} ${CMD_ARGS}
	WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}
	COMMENT "Check memory usage with valgrind"
)


ADD_CUSTOM_TARGET(cleanup
	COMMAND rm -rf *
	WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
	COMMENT "Remove all temp files"
)
