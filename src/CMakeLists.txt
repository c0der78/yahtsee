
add_executable(${PROJECT_NAME} alert_box.cpp caca_game.cpp client.cpp connection.cpp connection_state.cpp game.cpp game_actions.cpp game_display.cpp game_state.cpp main.cpp matchmaker.cpp player.cpp)

target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC ${CACA_INCLUDE_DIRS} SYSTEM PUBLIC ${RJNET_INCLUDE_DIRS} SYSTEM PUBLIC ${ARCHIVE_INCLUDE_DIR} SYSTEM PUBLIC ${MINIUPNPC_INCLUDE_DIR})

target_link_libraries(${PROJECT_NAME} ${CACA_LIBRARIES} ${CURL_LIBRARIES} ${ARCHIVE_LIBRARY} ${RJDICE_LIBRARIES} ${RJNET_LIBRARIES} ${RJASYNC_LIBRARIES} ${RJHTTP_LIBRARIES} ${RJSTR_LIBRARIES} ${RJLOG_LIBRARIES} ${URIPARSER_LIBRARIES} ${UUID_LIBRARIES} ${MINIUPNPC_LIBRARY})

install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)

