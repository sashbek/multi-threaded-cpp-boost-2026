set(COMMON_SOURCES
    game/CardsLibrary.cpp
    game/CardEffects.cpp
)

set(COMMON_HEADERS
    game/CardsLibrary.hpp
    game/CardEffects.hpp
    model/Card.hpp
    model/GameState.hpp
    model/Position.hpp
    model/Unit.hpp
)

add_library(${PROJECT_NAME}-common STATIC ${COMMON_SOURCES} ${COMMON_HEADERS})

target_include_directories(${PROJECT_NAME}-common 
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}
)

set_target_properties(${PROJECT_NAME}-common PROPERTIES
    PUBLIC_HEADER "${COMMON_HEADERS}"
)
