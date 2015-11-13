add_executable(yahtsee alert_box.cpp client.cpp connection_state.cpp game_actions.cpp game_state.cpp main.cpp player.cpp caca_game.cpp connection.cpp game.cpp game_display.cpp log.cpp matchmaker.cpp)

include_directories(/usr/local/include)

target_link_libraries(yahtsee arg3net arg3json caca arg3dice arg3str archive)
