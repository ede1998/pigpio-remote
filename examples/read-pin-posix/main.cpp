#include <iostream>
#include <pigpio-remote/BasicIo.h>
#include <pigpio-remote/PiConnection.h>

constexpr unsigned int GPIO_PIN = 12;

int main(int argc, char **argv)
{
    std::string ip = argc <= 1 ? "127.0.0.1" : argv[1];
    std::string port = argc <= 2 ? "8888" : argv[2];

    std::cout << "Connecting to " << ip << ":" << port << "..." << std::endl;

    pigpio_remote::PiConnection connection;
    pigpio_remote::BasicIo io(connection);
    auto result = connection.Connect(ip, std::stoul(port));

    if (result == pigpio_remote::ConnectionError::SUCCESS)
    {
        std::cout << "Successfully connected." << std::endl;
    }
    else
    {
        std::cout << "Connection failed with error code " << static_cast<int>(result) << std::endl;
        return 0;
    }

    auto set_mode_result = io.SetMode(GPIO_PIN, pigpio_remote::GpioMode::PI_INPUT);
    if (set_mode_result == pigpio_remote::PigpioError::PI_OK)
    {
        std::cout << "Pin " << GPIO_PIN << " set to input." << std::endl;
    }
    else
    {
        std::cout << "Could not set pin " << GPIO_PIN << " to input. Error code: " << static_cast<int>(set_mode_result) << std::endl;
        connection.Stop();
        return 0;
    }

    auto pull_up_result = io.SetPullUpDown(GPIO_PIN, pigpio_remote::GpioPullUpDown::PI_PUD_UP);
    if (pull_up_result == pigpio_remote::PigpioError::PI_OK)
    {
        std::cout << "Pin " << GPIO_PIN << " has pull up configured now." << std::endl;
    }
    else
    {
        std::cout << "Could not set pull up for pin " << GPIO_PIN << ". Error code: " << static_cast<int>(set_mode_result) << std::endl;
        connection.Stop();
        return 0;
    }

    for (int i = 0; i < 100; ++i)
    {
        auto read_result = io.Read(GPIO_PIN);
        if (read_result.Error != pigpio_remote::PigpioError::PI_OK)
        {
            std::cout << "Error code " << static_cast<int>(read_result.Error) << "when trying to read pin." << std::endl;
        }

        std::cout << "Pin " << GPIO_PIN << " reads " << static_cast<int>(*read_result.Value) << std::endl;
        sleep(2);
    }


    connection.Stop();

    return 0;
}