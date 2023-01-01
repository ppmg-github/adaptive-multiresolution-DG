include(GoogleTest)

add_executable(test_main test_main.cpp test_Quad.cpp test_subs.cpp test_VecMultiD.cpp)
target_link_libraries(test_main
    PUBLIC
    gtest_main
    adamdg
)

gtest_discover_tests(test_main)