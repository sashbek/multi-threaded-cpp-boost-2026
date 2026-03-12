include(FetchContent)

FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.14.0
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
        ${PROJECT_NAME}_tests
        test_suites/ProjectIntegrationTestSuite.cpp
        main_tests.cpp
        server_tests.cpp
        ${PROJECT_SOURCE_DIR}/src-server/gameroom/GameRoom.cpp
        ${PROJECT_SOURCE_DIR}/src-server/manager/PlayerManager.cpp
        ${PROJECT_SOURCE_DIR}/src-server/network/Server.cpp
        ${PROJECT_SOURCE_DIR}/src-server/network/Session.cpp
)

target_link_libraries(
        ${PROJECT_NAME}_tests
        GTest::gtest_main
        lib_common
)

target_include_directories(
        ${PROJECT_NAME}_tests
        PUBLIC
        ${PROJECT_SOURCE_DIR}
        ${PROJECT_SOURCE_DIR}/src-server
)

include(GoogleTest)

gtest_discover_tests(${PROJECT_NAME}_tests)
