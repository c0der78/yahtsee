//
// Created by Ryan Jennings on 2018-02-26.
//

#include "game.h"

#include <imgui/imgui.h>

using namespace std;
using namespace rj;

namespace yahtsee {


    Game::Game(int seed) {

    }

    Game &Game::load() {
        ImGui::CreateContext();
        return *this;
    }

    Game &Game::begin() {

        ImGuiIO &io = ImGui::GetIO();
        unsigned char *tex_pixels = NULL;
        int tex_w, tex_h;
        io.Fonts->GetTexDataAsRGBA32(&tex_pixels, &tex_w, &tex_h);

        io.DisplaySize = ImVec2(1920, 1080);
        io.DeltaTime = 1.0f / 60.0f;

        return *this;
    }

    bool Game::on() const {
        return false;
    }

    Game &Game::update() {

        ImGui::NewFrame();

        ImGui::ShowDemoWindow();

        ImGui::Render();

        return *this;
    }

    Game &Game::end() {

        ImGui::DestroyContext(ImGui::GetCurrentContext());
        return *this;
    }

    std::unique_ptr<GameLogic> &Game::logic() {
        return logic_;
    }

}

