
add_executable(gen-dataset gen_dataset.cpp)
target_link_libraries(gen-dataset PRIVATE parquet-writer event-gen)
