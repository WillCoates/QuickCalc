cmake_minimum_required(VERSION 3.12)
project(quickcalc VERSION 0.1 LANGUAGES CXX)

add_library(libquickcalc STATIC
    src/lexer.cpp include/lexer.hpp
    src/ast.cpp include/ast.hpp
    src/parser.cpp include/parser.hpp
    src/executor.cpp include/executor.hpp
)

target_compile_features(libquickcalc PUBLIC cxx_std_17)
target_include_directories(libquickcalc PUBLIC include)

add_executable(quickcalc
    src/main.cpp
)

target_link_libraries(quickcalc PUBLIC libquickcalc)

find_package(GTest)
if(${GTEST_FOUND})
    enable_testing()
    include(GoogleTest)
    
    add_executable(unittests
        test/lexer.cpp
        test/parser.cpp
        test/executor.cpp
        test/executorstate.cpp
    )
    
    target_link_libraries(unittests PUBLIC libquickcalc GTest::GTest GTest::Main)

    add_executable(integrationtests
        test/integration.cpp
    )

    target_link_libraries(integrationtests PUBLIC libquickcalc GTest::GTest GTest::Main)

    gtest_discover_tests(unittests)
    gtest_discover_tests(integrationtests)
else()
    message(WARNING "Google Test must be installed in order to execute tests")
endif()
