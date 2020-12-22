#include "BasicIo.h"

#include "socket_command.h"

BasicIo::BasicIo(PiConnection &connection)
    : _connection(connection)
{
}

GpioModeError BasicIo::set_mode(unsigned int gpio, GpioMode mode)
{
    auto result = this->_connection.send_command(SocketCommand::PI_CMD_MODES, gpio, static_cast<uint32_t>(mode));
    return GpioModeError(result.Error);
}

PigpioResult<GpioMode, GpioReadError> BasicIo::get_mode(unsigned int gpio)
{
    auto result = this->_connection.send_command(SocketCommand::PI_CMD_MODEG, gpio);
    return make_result<GpioMode, GpioReadError>(result);
}

GpioPullUpDownError BasicIo::set_pull_up_down(unsigned int gpio, GpioPullUpDown pud)
{
    auto result = this->_connection.send_command(SocketCommand::PI_CMD_PUD, gpio, static_cast<uint32_t>(pud));
    return GpioPullUpDownError(result.Error);
}

PigpioResult<GpioLevel, GpioReadError> BasicIo::gpio_read(unsigned int gpio)
{
    auto result = this->_connection.send_command(SocketCommand::PI_CMD_READ, gpio);
    return make_result<GpioLevel, GpioReadError>(result);
}

GpioWriteError BasicIo::gpio_write(unsigned int gpio, GpioLevel level)
{
    auto result = this->_connection.send_command(SocketCommand::PI_CMD_WRITE, gpio);
    return GpioWriteError(result.Error);
}
