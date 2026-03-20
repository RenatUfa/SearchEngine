add_library(inverted_index_lib InvertedIndex.cpp ConverterJson.cpp SearchServer.cpp)
target_include_directories(inverted_index_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
)
target_link_libraries(inverted_index_lib PUBLIC nlohmann_json::nlohmann_json)
add_executable(search_engine main.cpp)
target_link_libraries(search_engine PRIVATE inverted_index_lib)