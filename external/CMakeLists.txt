# Google Test
ExternalProject_Add(googletest
        URL https://github.com/google/googletest/archive/release-1.8.0.tar.gz
        CMAKE_ARGS
        -DCMAKE_BUILD_TYPE=Release
        -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
        -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
        -DCMAKE_MAKE_PROGRAM=${CMAKE_MAKE_PROGRAM}
        -DCMAKE_GENERATOR=${CMAKE_GENERATOR}
        -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
        -DCMAKE_EXE_LINKER_FLAGS=${CMAKE_EXE_LINKER_FLAGS}
        BUILD_COMMAND ${CMAKE_COMMAND} --build .
        INSTALL_COMMAND ""
        BUILD_BYPRODUCTS
        "${CMAKE_CURRENT_BINARY_DIR}/googletest-prefix/src/googletest-build/googlemock/gtest/libgtest.a"
        "${CMAKE_CURRENT_BINARY_DIR}/googletest-prefix/src/googletest-build/googlemock/gtest/libgtest_main.a"
        "${CMAKE_CURRENT_BINARY_DIR}/googletest-prefix/src/googletest-build/googlemock/libgmock.a"
        "${CMAKE_CURRENT_BINARY_DIR}/googletest-prefix/src/googletest-build/googlemock/libgmock_main.a")

# Google Test/Mock include dirs
ExternalProject_Get_Property(googletest SOURCE_DIR)
set(GTEST_INCLUDE_DIRS ${SOURCE_DIR}/googletest/include)
set(GMOCK_INCLUDE_DIRS ${SOURCE_DIR}/googlemock/include)
set(GTEST_INCLUDE_DIRS ${GTEST_INCLUDE_DIRS} PARENT_SCOPE)
set(GMOCK_INCLUDE_DIRS ${GMOCK_INCLUDE_DIRS} PARENT_SCOPE)

# Google Test/Mock libraries
ExternalProject_Get_Property(googletest BINARY_DIR)
set(GTEST_LIBS_DIR "${BINARY_DIR}/googlemock/gtest")
set(GMOCK_LIBS_DIR "${BINARY_DIR}/googlemock")
set(GTEST_LIBRARIES "${GTEST_LIBS_DIR}/libgtest.a" "${GTEST_LIBS_DIR}/libgtest_main.a")
set(GMOCK_LIBRARIES "${GMOCK_LIBS_DIR}/libgmock.a" "${GMOCK_LIBS_DIR}/libgmock_main.a")
set(GTEST_LIBRARIES ${GTEST_LIBRARIES} PARENT_SCOPE)
set(GMOCK_LIBRARIES ${GMOCK_LIBRARIES} PARENT_SCOPE)

# combined dependency / includes / libraries
set(EXTERNAL_DEPENDENCIES
        googletest
        PARENT_SCOPE)
set(EXTERNAL_INCLUDE_DIRS
        ${GTEST_INCLUDE_DIRS}
        ${GMOCK_INCLUDE_DIRS}
        ${LIBBACKTRACE_INCLUDE_DIRS} PARENT_SCOPE)
set(EXTERNAL_LIBRARIES
        ${GTEST_LIBRARIES}
        ${GMOCK_LIBRARIES} PARENT_SCOPE)
