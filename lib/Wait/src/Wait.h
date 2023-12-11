//
// Created by Oliver on 12.05.2023.
//

#ifndef GADGET_WAIT_H
#define GADGET_WAIT_H

class Wait {
    private:
        unsigned long previousMillis = 0;

    public:
        unsigned long interval;

        Wait() {
            interval = 100;
        }

        explicit Wait(int pInterval) {
            interval = pInterval;
        }

        bool run();
};


#endif //GADGET_WAIT_H
