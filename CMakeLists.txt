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


# ------------------------------------------------------------
# 2. Библиотека nlohmann/json (загружаем и подключаем)
# ------------------------------------------------------------
FetchContent_Declare(
  nlohmann_json
  URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(nlohmann_json)

enable_testing()

add_subdirectory(src)
add_subdirectory(tests)