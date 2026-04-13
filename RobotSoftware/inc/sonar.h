#ifndef DEF_SONAR
#define DEF_SONAR

#include <iostream>
#include <chrono>
#include <thread>
#include <pigpio.h>

class Sonar {
public:
    Sonar();
    void init(int trigger, int echo);
    double distance(int timeout);

private:
    void recordPulseLength();
    int trigger;
    int echo;
    volatile uint32_t startTimeUsec;
    volatile uint32_t endTimeUsec;
    double distanceMeters;
    uint32_t travelTimeUsec;
    uint32_t now;
};

Sonar::Sonar() {}

void Sonar::init(int trigger, int echo) {
    this->trigger = trigger;
    this->echo = echo;

    gpioSetMode(trigger, PI_OUTPUT);
    gpioSetMode(echo, PI_INPUT);
    gpioWrite(trigger, PI_LOW);

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
}

double Sonar::distance(int timeout) {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    // Send trigger pulse
    gpioWrite(trigger, PI_HIGH);
    gpioDelay(10);  // 10 microseconds
    gpioWrite(trigger, PI_LOW);

    // Wait for echo start
    now = gpioTick();
    while (gpioRead(echo) == PI_LOW && (gpioTick() - now) < (uint32_t)timeout);

    if ((gpioTick() - now) >= (uint32_t)timeout) {
        std::cerr << "Echo start timeout!" << std::endl;
        return -1.0;
    }

    recordPulseLength();

    travelTimeUsec = endTimeUsec - startTimeUsec;
    distanceMeters = ((travelTimeUsec / 1000000.0) * 340.29) / 2 * 100;  // cm

    return distanceMeters;
}

void Sonar::recordPulseLength() {
    startTimeUsec = gpioTick();
    while (gpioRead(echo) == PI_HIGH);
    endTimeUsec = gpioTick();
}
#endif
