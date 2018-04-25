add_executable(${PROJECT_NAME} main.cpp game.cpp game.h game_ui.cpp game_ui.h client.cpp client.h matchmaker.cpp matchmaker.h
        connection.cpp connection.h connection.h connection_factory.cpp connection_factory.h player.cpp player.h
        connection_state.cpp connection_state.h event_manager.cpp event_manager.h game_logic.cpp
        game_logic.h multiplayer.cpp multiplayer.h game_input.cpp game_input.h game_state.cpp game_state.h
        state_manager.cpp state_manager.h input_manager.cpp input_manager.h renderable.h updatable.h
        player_manager.cpp player_manager.h)

target_link_libraries(${PROJECT_NAME} imgui imgui_demo ${LIBLOG_LIBRARIES} ${LIBNET_LIBRARIES} ${LIBNETHTTP_LIBRARIES}
        ${LIBNETASYNC_LIBRARIES} ${LIBDICE_LIBRARIES} ${LIBSTR_LIBRARIES}
        ${LIBURIPARSER_LIBRARIES} ${OpenGL_LIBRARIES} ${CURSES_LIBRARIES} ${CURSES_MENU_LIBRARY})

install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)

