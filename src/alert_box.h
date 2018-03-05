//
// Created by Ryan Jennings on 2018-03-04.
//

#ifndef YAHTSEE_ALERT_BOX_H
#define YAHTSEE_ALERT_BOX_H

#include <vector>
#include <string>
#include <functional>

namespace yahtsee {

    namespace ui {

        class AlertBox {

        };

        class AlertInput {
        public:
            typedef std::function<void(const AlertBox &a)> Delegate;

            AlertInput(const Delegate &delegate);

            AlertInput(const std::string &message, const Delegate &delegate = nullptr);

            AlertInput(const char *message, const Delegate &delegate = nullptr);

            AlertInput(const std::initializer_list<std::string> &messages, const Delegate &delegate = nullptr);

            AlertInput(const std::vector<std::string> &messages, const Delegate &delegate = nullptr);

            operator std::string() const;
            operator std::vector<std::string>() const;

            const Delegate &delegate() const;

        private:
            std::vector<std::string> values_;
            Delegate delegate_;
        };
    }
}


#endif //YAHTSEE_ALERT_BOX_H
