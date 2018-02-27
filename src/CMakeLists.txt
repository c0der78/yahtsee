add_executable(${PROJECT_NAME} main.cpp game.cpp game.h ui.cpp ui.h client.cpp client.h matchmaker.cpp matchmaker.h connection.cpp connection.h connection.h)


install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)

