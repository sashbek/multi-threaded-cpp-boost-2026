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

add_library(lib_common SHARED ${COMMON_SOURCES} ${COMMON_HEADERS})

target_link_libraries(lib_common PUBLIC boost)

target_include_directories(lib_common
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

set_target_properties(lib_common PROPERTIES
    PUBLIC_HEADER "${COMMON_HEADERS}"
)
