include_directories(${ARCHIVE_INCLUDE_DIR} ${MINIUPNPC_INCLUDE_DIR})

add_executable(${PROJECT_NAME} alert_box.cpp caca_game.cpp client.cpp connection.cpp connection_state.cpp game.cpp game_actions.cpp game_display.cpp game_state.cpp main.cpp matchmaker.cpp player.cpp)

target_link_libraries(${PROJECT_NAME} ${CACA_LIBRARIES} ${CURL_LIBRARIES} ${ARCHIVE_LIBRARY} ${ARG3DICE_LIBRARIES} ${ARG3JSON_LIBRARIES} ${JSONC_LIBRARIES} ${ARG3NET_LIBRARIES} ${ARG3STR_LIBRARIES} ${ARG3LOG_LIBRARIES} ${UUID_LIBRARIES} ${MINIUPNPC_LIBRARY}) 

install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)