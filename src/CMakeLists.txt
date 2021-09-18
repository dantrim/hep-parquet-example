
add_library(event-gen SHARED)

target_sources(event-gen
    PRIVATE
    event_generator.cpp
)

target_include_directories(event-gen PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(tools)
