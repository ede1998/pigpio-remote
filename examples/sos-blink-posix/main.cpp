#include <iostream>
#include <pigpio-remote/BasicIo.h>
#include <pigpio-remote/PiConnection.h>

constexpr unsigned int GPIO_PIN = 17;
constexpr unsigned int LONG_DURATION = 4;
constexpr unsigned int SHORT_DURATION = 1;

void Blink(pigpio_remote::BasicIo &io, unsigned int duration)
{
    io.Write(GPIO_PIN, pigpio_remote::GpioLevel::PI_HIGH);
    sleep(duration);
    io.Write(GPIO_PIN, pigpio_remote::GpioLevel::PI_LOW);
    sleep(SHORT_DURATION);
}

void SendS(pigpio_remote::BasicIo &io)
{
    std::cout << "Sending S." << std::endl;
    Blink(io, LONG_DURATION);
    Blink(io, LONG_DURATION);
    Blink(io, LONG_DURATION);
}

void SendO(pigpio_remote::BasicIo &io)
{
    std::cout << "Sending O." << std::endl;
    Blink(io, SHORT_DURATION);
    Blink(io, SHORT_DURATION);
    Blink(io, SHORT_DURATION);
}

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

    auto set_mode_result = io.SetMode(GPIO_PIN, pigpio_remote::GpioMode::PI_OUTPUT);
    if (set_mode_result == pigpio_remote::PigpioError::PI_OK)
    {
        std::cout << "Pin " << GPIO_PIN << " set to output." << std::endl;
    }
    else
    {
        std::cout << "Could not set pin " << GPIO_PIN << " to output. Error code: " << set_mode_result << std::endl;
        connection.Stop();
        return 0;
    }

    SendS(io);
    SendO(io);
    SendS(io);

    connection.Stop();

    return 0;
}