#include <crow.h>  // Include Crow header
#include <pigpio.h>
#include <iostream>
#include <shared_mutex>
#include <mutex>
#include <thread>
#include <chrono>
#include <sonar.h>

#define GPIO_FREQUENCY 50 // engine and servo pwm frequency input
#define ENGINE_PIN 5 // pin for engine pwm
#define SERVO_PIN 18 // pin for servo pwm
#define TRIG_PIN 20  // trigger pin for US sensor
#define ECHO_PIN 19  // echo pin for US sensor
#define TIMEOUT_US 30000
// Define GPIO pin for PWM

// Global variable to track the current PWM percentage
int enginePercentage = 0;
int servoPercentage = 0;
std::mutex engineMutex; // Mutex to ensure thread-safe access to currentPWMPercentage
std::mutex servoMutex;

//Global variebles for US sensor
Sonar sonar;
std::mutex ultrasonicMutex;
double currentDistance = 0.0;
std::mutex limitMutex;
int currentLimit = 100;
int timeCounter = 0;
//Global variebles for temperature sensor

void normalStop() {
    {
        std::unique_lock lock(engineMutex);
        std::unique_lock lock(servoMutex);

        gpioPWM(ENGINE_PIN, 80);
    }
}

void emergencyStopTurn() {
    {
        std::unique_lock lock(engineMutex);
        std::unique_lock lock(servoMutex);

        gpioPWM(ENGINE_PIN, 94);
        gpioPWM(ENGINE_PIN, 88);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        gpioPWM(ENGINE_PIN, 84);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        gpioPWM(ENGINE_PIN, 80);
    }
}

void emergencyStopBackingOff(double initialDistance) {
    {
        std::unique_lock lock(engineMutex);
        std::unique_lock lock(servoMutex);
        std::unique_lock lock(ultrasonicMutex);

        gpioPWM(ENGINE_PIN, 85);
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
        gpioPWM(ENGINE_PIN, 80);
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
        gpioPWM(ENGINE_PIN, 73);
        while (sonar.distance(TIMEOUT_US)>initialDistance);
        gpioPWM(ENGINE_PIN, 80);
    }
}

void updateDistance() {
    while(true) {
        std::cout << "Begining distance measure" << std::endl;
        
        

        double distance = sonar.distance(TIMEOUT_US);
        if (distance >= 0) {
            std::cout << "Distance: " << distance << " cm" << std::endl;
        } else {
            std::cerr << "Measurement failed." << std::endl;
        }

        {
            std::unique_lock lock(ultrasonicMutex);
            currentDistance = distance;
        }

        if(distance < 30) {
            std::shared_lock lock2(engineMutex);
            if(enginePercentage > 90)
            {
                lock2.unlock();
                emergencyStopBackingOff(distance);
            }
            else
            {
                if(enginePercentage > 70) {
                    lock2.unlock();
                    emergencyStopTurn();
                }
                else
                {
                    lock2.unlock();
                    normalStop();
                }
            }
        }

        if(distance<200) timeCounter = std::min(timeCounter+1,40);
        if(distance>=200) timeCounter = std::max(timeCounter-1,0);

        if(timeCounter > 20)
        {
            std::unique_lock(limitMutex);
            currentLimit = std::min(50 + (int)distance/4,100);
        }
        else
        {
            std::unique_lock(limitMutex);
            currentLimit = 100;
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
    }
}

int main() {
    // Initialize pigpio
    if (gpioInitialise() < 0) {
        std::cerr << "Failed to initialize pigpio!" << std::endl;
        return -1;
    }

    std::cout << "Starting libcamera-vid stream..." << std::endl;
    std::system("libcamera-vid -t 0 --inline --listen -o tcp://0.0.0.0:8554 &");

    // Set up PWM pin
    gpioSetMode(ENGINE_PIN, PI_OUTPUT);
    gpioSetPWMrange(ENGINE_PIN, 1024);
    gpioSetPWMfrequency(ENGINE_PIN, GPIO_FREQUENCY);
    gpioSetMode(SERVO_PIN, PI_OUTPUT);
    gpioSetPWMrange(SERVO_PIN, 1024);
    gpioSetPWMfrequency(SERVO_PIN, GPIO_FREQUENCY);

    //Ultrasonic sensor
    gpioSetMode(TRIG_PIN, PI_OUTPUT);
    gpioSetMode(ECHO, PI_INPUT);
    gpioWrite(TRIG_PIN, PI_LOW);
    auto task = std::async(std::launch::async, updateDistance);
    sonar.init(TRIG_PIN, ECHO_PIN);

    crow::SimpleApp app;

    // POST route for /engine to set PWM value
    CROW_ROUTE(app, "/engine").methods(crow::HTTPMethod::POST)([](const crow::request& req) {
        try {
            // Parse the value from the request body
            auto body = crow::json::load(req.body);
            if (!body || !body.has("value")) {
                return crow::response(400, "Invalid request, 'value' is required.");
            }

            int pwmValue = body["value"].i();
            if (pwmValue < 0 || pwmValue > 100) {
                return crow::response(400, "Value must be between 0 and 100.");
            }
            // 70 - 80 - 90

            if(pwmValue > 50){
                std::shared_lock lock(limitMutex);
                pwmValue = std::min(currentLimit/2+50,pwmValue);
            }

            int dutyCycle = pwmValue / 5 + 70;

            // Set the PWM duty cycle on the GPIO pin
            gpioPWM(ENGINE_PIN, dutyCycle);

            // Update the global current PWM value
            {
                std::unique_lock lock(engineMutex);
                enginePercentage = pwmValue;
            }

            std::ostringstream msg;
            msg << "PWM set to " << pwmValue << "% (Duty Cycle: " << dutyCycle << ").";
            return crow::response(200, msg.str());
        } catch (const std::exception& e) {
            return crow::response(500, std::string("Internal Server Error: ") + e.what());
        }
    });

    CROW_ROUTE(app, "/turn").methods(crow::HTTPMethod::POST)([](const crow::request& req) {
        try {
            // Parse the value from the request body
            auto body = crow::json::load(req.body);
            if (!body || !body.has("value")) {
                return crow::response(400, "Invalid request, 'value' is required.");
            }

            int pwmValue = body["value"].i();
            if (pwmValue < 0 || pwmValue > 100) {
                return crow::response(400, "Value must be between 0 and 100.");
            }
            // 74 - 84 - 94
            int dutyCycle = pwmValue / 5 + 74;

            // Set the PWM duty cycle on the GPIO pin
            gpioPWM(SERVO_PIN, dutyCycle);

            // Update the global current PWM value
            {
                std::unique_lock lock(servoMutex);
                servoPercentage = pwmValue;
            }

            std::ostringstream msg;
            msg << "PWM set to " << pwmValue << "% (Duty Cycle: " << dutyCycle << ").";
            return crow::response(200, msg.str());
        } catch (const std::exception& e) {
            return crow::response(500, std::string("Internal Server Error: ") + e.what());
        }
    });

    CROW_ROUTE(app, "/distance").methods(crow::HTTPMethod::GET)([]() {
        crow::json::wvalue response;
        {
            std::shared_lock lock(ultrasonicMutex);
            response["value"] = currentDistance;
        }
        return crow::response(200, response);
    });

    // GET route for /engine to get the current PWM value
    CROW_ROUTE(app, "/engine").methods(crow::HTTPMethod::GET)([]() {
        crow::json::wvalue response;
        {
            std::shared_lock lock(engineMutex);
            response["value"] = enginePercentage;
        }
        return crow::response(200, response);
    });

    CROW_ROUTE(app, "/servo").methods(crow::HTTPMethod::GET)([]() {
// Ensure thread-safe access
        crow::json::wvalue response;
        {
            std::shared_lock lock(servoMutex); 
            response["value"] = servoPercentage;
        }
        return crow::response(200, response);
    });

    std::cout << "Server is running on http://192.168.137.13:18080" << std::endl;

    // Run the server on port 18080
    app.port(18080).multithreaded().run();

    // Cleanup pigpio on exit
    gpioPWM(SERVO_PIN, 84);
    gpioPWM(ENGINE_PIN, 80);
    gpioTerminate();
    return 0;
}

