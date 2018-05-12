file(GLOB DI_HEADERS ${CMAKE_SOURCE_DIR}/src/*)
file(GLOB_RECURSE DI_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
add_library(di STATIC
        ${DI_HEADERS}
        ${DI_SRC})

target_link_libraries(di
        ${Boost_LIBRARIES})
