#include <chrono>
#include <iostream>
#include <pigpio-remote/I2cDevice.h>
#include <pigpio-remote/PiConnection.h>
#include <thread>

constexpr uint8_t I2C_ADDRESS = 0x21;

void ToggleAll(pigpio_remote::I2cDevice &device)
{
    uint8_t data = 0;

    for (int i = 0; i < 20; ++i)
    {
        data ^= 0xFFU;
        auto write_result = device.WriteByte(data);
        if (write_result == pigpio_remote::PigpioError::PI_OK)
        {
            std::cout << "Wrote " << static_cast<int>(data) << " successfully." << std::endl;
        }
        else
        {
            std::cout << "Write failed with error code " << static_cast<int>(write_result) << "." << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
}

int main(int argc, char **argv)
{
    std::string ip = argc <= 1 ? "127.0.0.1" : argv[1];
    std::string port = argc <= 2 ? "8888" : argv[2];

    std::cout << "Connecting to " << ip << ":" << port << "..." << std::endl;

    pigpio_remote::PiConnection connection;
    pigpio_remote::I2cDevice device(connection);
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

    auto open_result = device.Open(I2C_ADDRESS);
    if (open_result == pigpio_remote::PigpioError::PI_OK)
    {
        std::cout << "Opened I2C connection to device on address " << std::hex << static_cast<int>(device.GetAddress()) << "." << std::endl;
    }
    else
    {
        std::cout << "Could not open connection. Error code: " << static_cast<int>(open_result) << std::endl;
        connection.Stop();
        return 0;
    }

    ToggleAll(device);

    device.Close();
    connection.Stop();

    return 0;
}