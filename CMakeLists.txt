cmake_minimum_required(VERSION 3.14)
project(search_engine)

# Устанавливаем стандарт C++ для всего проекта
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Для совместимости со старыми проектами (например, googletest)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

# Политика для извлечения временных меток (подавляет предупреждение)
if(POLICY CMP0135)
  cmake_policy(SET CMP0135 NEW)
endif()

# Включаем поддержку FetchContent
include(FetchContent)

# ------------------------------------------------------------
# 1. Google Test (загружаем и подключаем)
# ------------------------------------------------------------
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE   # подавляет предупреждение CMP0135
)
# Для Windows может потребоваться эта опция (совместимость CRT)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

# Подключаем модуль GoogleTest для функции gtest_discover_tests
include(GoogleTest)

# Включаем поддержку CTest (для запуска тестов через ctest)
enable_testing()

# ------------------------------------------------------------
# 2. Библиотека nlohmann/json (загружаем и подключаем)
# ------------------------------------------------------------
FetchContent_Declare(
  nlohmann_json
  URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(nlohmann_json)

# ------------------------------------------------------------
# 3. Основное приложение
# ------------------------------------------------------------
add_executable(search_engine main.cpp InvertedIndex.cpp InvertedIndex.hpp SearchServer.cpp SearchServer.hpp Entry.hpp)

target_link_libraries(search_engine PRIVATE nlohmann_json::nlohmann_json)

# Если в проекте есть другие исходные файлы, добавьте их здесь:
# target_sources(search_engine PRIVATE src/file1.cpp src/file2.cpp)
# и укажите пути к заголовкам:
# target_include_directories(search_engine PRIVATE include)

# ------------------------------------------------------------
# 4. Тестовый исполняемый файл
# ------------------------------------------------------------
# Предполагается, что тесты находятся в файле tests/test_main.cpp
# Создайте этот файл, если его ещё нет.

# 4. Тестовый исполняемый файл

add_executable(test_inverted_index tests/test_inverted_index.cpp InvertedIndex.cpp)
add_executable(test_search_server tests/test_search_server.cpp SearchServer.cpp InvertedIndex.cpp)

# Линкуем тесты с Google Test (имена целей могут быть gtest и gtest_main)


target_link_libraries(test_inverted_index
  PRIVATE
    gtest_main
    nlohmann_json::nlohmann_json
)

target_link_libraries(test_search_server
  PRIVATE
    gtest_main
    nlohmann_json::nlohmann_json
)

# Если нужны дополнительные исходники основного проекта, раскомментируйте:
# target_sources(search_engine_tests PRIVATE ../src/some_file.cpp)


target_include_directories(test_inverted_index PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(test_search_server PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Регистрируем тесты

gtest_discover_tests(test_inverted_index)
gtest_discover_tests(test_search_server)