#include "BasicIo.h"

#include "socket_command.h"

BasicIo::BasicIo(PiConnection &connection)
    : _connection(connection)
{
}

GpioModeError BasicIo::set_mode(unsigned int gpio, GpioMode mode)
{
    return GpioModeError(PigpioError::PI_OK);
}

tl::optional<GpioMode> BasicIo::get_mode(unsigned int gpio)
{
    return tl::optional<GpioMode>();
}

GpioPullUpDownError BasicIo::set_pull_up_down(unsigned int gpio, GpioPullUpDown pud)
{
    return GpioPullUpDownError(PigpioError::PI_OK);
}

PigpioResult<GpioLevel, GpioReadError> BasicIo::gpio_read(unsigned int gpio)
{
    auto result = this->_connection.send_command(SocketCommand::PI_CMD_READ, gpio, 0);
    return result.Error == PigpioError::PI_OK ? PigpioResult<GpioLevel, GpioReadError>(static_cast<GpioLevel>(result.Result))
                                              : PigpioResult<GpioLevel, GpioReadError>(result.Error);
}

GpioWriteError BasicIo::gpio_write(unsigned int gpio, GpioLevel level)
{
    return GpioWriteError(PigpioError::PI_OK);
}
