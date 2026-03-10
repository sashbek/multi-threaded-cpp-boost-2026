#set(Boost_NO_SYSTEM_PATHS ON)
#set(Boost_USE_MULTITHREADED ON)
#set(Boost_USE_STATIC_LIBS ON)
#set(Boost_USE_STATIC_RUNTIME ON)
#set(BOOST_ALL_DYN_LINK OFF)
#
#if (NOT Boost_ROOT AND EXISTS $ENV{BOOSTROOT})
#    set(Boost_ROOT $ENV{BOOSTROOT})
#endif()

#find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)
#find_package(Boost REQUIRED)

add_library(${PROJECT_NAME}-common
        ./model/Card.hpp
        ./model/GameState.hpp
        ./model/Position.hpp
        ./model/Unit.hpp
        ./game/CardsLibrary.hpp
        ./game/CardsLibrary.cpp
)

#target_link_libraries(${PROJECT_NAME}-server PRIVATE boost)
