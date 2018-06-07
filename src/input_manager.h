//
// Created by Ryan Jennings on 2018-03-13.
//

#ifndef YAHTSEE_INPUT_MANAGER_H
#define YAHTSEE_INPUT_MANAGER_H

#include "updatable.h"
#include <string>

namespace yahtsee {

    class Inputer {
    public:
        virtual ~Inputer() {}
        virtual int get() const = 0;
    };

    class InputManager : public Updatable {
    public:
        InputManager();

        void update();

        int get() const;

    private:
        int value_;
        std::unique_ptr<Inputer> input_;
    };
}

#endif //YAHTSEE_INPUT_MANAGER_H
