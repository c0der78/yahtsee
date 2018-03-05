//
// Created by Ryan Jennings on 2018-03-04.
//

#include "alert_box.h"

namespace yahtsee {
    namespace ui {
        AlertInput::AlertInput(const Delegate &delegate) : delegate_(delegate) {}

        AlertInput::AlertInput(const std::string &message, const Delegate &delegate) :
            delegate_(delegate) {
            values_.push_back(message);
        }


        AlertInput::AlertInput(const char *message, const Delegate &delegate) :
                delegate_(delegate) {
            if (message != nullptr) {
                values_.push_back(message);
            }
        }

        AlertInput::AlertInput(const std::vector<std::string> &messages, const Delegate &delegate) :
                values_(messages), delegate_(delegate) {
        }

        AlertInput::AlertInput(const std::initializer_list<std::string> &messages, const Delegate &delegate) :
                values_(messages), delegate_(delegate) {
        }

        AlertInput::operator std::string() const {
            if (values_.empty()) {
                return "";
            }
            return values_.front();
        }
        AlertInput::operator std::vector<std::string>() const {
            return values_;
        }

        const AlertInput::Delegate &AlertInput::delegate() const {
            return delegate_;
        }
    }
}