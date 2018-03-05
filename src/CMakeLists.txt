add_executable(${PROJECT_NAME} main.cpp game.cpp game.h game_ui.cpp game_ui.h client.cpp client.h matchmaker.cpp matchmaker.h
        connection.cpp connection.h connection.h connection_factory.cpp connection_factory.h player.cpp player.h
        connection_state.cpp connection_state.h game_action.cpp game_action.h alert_box.cpp alert_box.h game_logic.cpp game_logic.h)

target_link_libraries(${PROJECT_NAME} imgui imgui_demo rj_log rj_net rj_net_http rj_net_async rj_dice rj_string ${URIPARSER_LIBRARIES})

install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)

