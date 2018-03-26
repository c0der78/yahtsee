add_executable(${PROJECT_NAME} main.cpp game.cpp game.h game_ui.cpp game_ui.h client.cpp client.h matchmaker.cpp matchmaker.h
        connection.cpp connection.h connection.h connection_factory.cpp connection_factory.h player.cpp player.h
        connection_state.cpp connection_state.h event_manager.cpp event_manager.h alert_box.cpp alert_box.h game_logic.cpp
        game_logic.h multiplayer.cpp multiplayer.h game_input.cpp game_input.h game_state.cpp game_state.h
        state_manager.cpp state_manager.h input_manager.cpp input_manager.h renderable.h updatable.h game_manager.h
        player_manager.cpp player_manager.h)

target_link_libraries(${PROJECT_NAME} imgui imgui_demo rj_log rj_net rj_net_http rj_net_async rj_dice rj_string
        ${URIPARSER_LIBRARIES} ${OpenGL_LIBRARIES})

install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)

