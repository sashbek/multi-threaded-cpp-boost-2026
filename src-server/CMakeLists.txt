set(Boost_NO_SYSTEM_PATHS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_STATIC_RUNTIME ON)
set(BOOST_ALL_DYN_LINK OFF)

if (NOT Boost_ROOT AND EXISTS $ENV{BOOSTROOT})
    set(Boost_ROOT $ENV{BOOSTROOT})
endif()

#find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)
find_package(Boost REQUIRED)

set(SERVER_SOURCES
        main.cpp
        gameroom/GameRoom.cpp
        manager/PlayerManager.cpp
        network/Server.cpp
        network/Session.cpp
)

set(SERVER_HEADERS
        gameroom/GameRoom.hpp
        manager/PlayerManager.hpp
        network/Server.hpp
        network/Session.hpp
)

if (WIN32)
    add_executable(${PROJECT_NAME}-server WIN32
            ${SERVER_HEADERS}
            ${SERVER_SOURCES}
    )
else ()
    add_executable(${PROJECT_NAME}-server
            ${SERVER_HEADERS}
            ${SERVER_SOURCES}
    )
endif ()

target_include_directories(${PROJECT_NAME}-server 
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(${PROJECT_NAME}-server PRIVATE boost MiniClash-common)

set_target_properties(${PROJECT_NAME}-server PROPERTIES
    PUBLIC_HEADER "${SERVER_HEADERS}"
)
