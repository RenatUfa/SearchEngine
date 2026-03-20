include (GoogleTest)

# 4. Тестовый исполняемый файл

add_executable(test_inverted_index test_inverted_index.cpp)

target_link_libraries(test_inverted_index
  PRIVATE
    gtest_main
    nlohmann_json::nlohmann_json
    inverted_index_lib
)
gtest_discover_tests(test_inverted_index)


add_executable(test_search_server test_search_server.cpp)

target_link_libraries(test_search_server
  PRIVATE
    gtest_main
    nlohmann_json::nlohmann_json
    inverted_index_lib
)

gtest_discover_tests(test_search_server)