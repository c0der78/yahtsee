//
// Created by Ryan Jennings on 2018-03-13.
//

#include "input_manager.h"
#include <curses.h>

namespace yahtsee {

    class CursesInput : public Inputer {
    public:
        int get() const {
            return getch();
        }
    };

    InputManager::InputManager() : input_(std::make_unique<CursesInput>()) {
    }

    void InputManager::update() {
        value_ = input_->get();
    }

    int InputManager::get() const {
        return value_;
    }

}