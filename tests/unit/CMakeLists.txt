include_directories(${EXTERNAL_INCLUDE_DIRS})

file(GLOB DI_HEADERS ${CMAKE_SOURCE_DIR}/src/*)
file(GLOB_RECURSE DI_TESTS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
add_executable(unit_tests
        ${DI_HEADERS}
        ${DI_TESTS})

target_link_libraries(unit_tests
        di
        ${EXTERNAL_LIBRARIES}
        ${Boost_LIBRARIES}
        ${RT_LIBRARY})

add_dependencies(unit_tests
        di
        ${EXTERNAL_DEPENDENCIES})

add_test(NAME run-unit-tests COMMAND unit_tests)
