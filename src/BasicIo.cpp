#include "../include/pigpio-remote/BasicIo.h"

#include "communication/Command.h"

namespace pigpio_remote
{

    using namespace communication;
    using namespace internal;

    BasicIo::BasicIo(PiConnection &connection)
        : _connection(connection)
    {
    }

    GpioModeError BasicIo::SetMode(unsigned int gpio, GpioMode mode)
    {
        auto result = this->_connection.SendCommand(Command::PI_CMD_MODES, gpio, static_cast<uint32_t>(mode));
        return {result.Error};
    }

    PigpioResult<GpioMode, GpioReadError> BasicIo::GetMode(unsigned int gpio)
    {
        auto result = this->_connection.SendCommand(Command::PI_CMD_MODEG, gpio);
        return MakeResult<GpioMode, GpioReadError>(result);
    }

    GpioPullUpDownError BasicIo::SetPullUpDown(unsigned int gpio, GpioPullUpDown pud)
    {
        auto result = this->_connection.SendCommand(Command::PI_CMD_PUD, gpio, static_cast<uint32_t>(pud));
        return {result.Error};
    }

    PigpioResult<GpioLevel, GpioReadError> BasicIo::Read(unsigned int gpio)
    {
        auto result = this->_connection.SendCommand(Command::PI_CMD_READ, gpio);
        return MakeResult<GpioLevel, GpioReadError>(result);
    }

    GpioWriteError BasicIo::Write(unsigned int gpio, GpioLevel level)
    {
        auto result = this->_connection.SendCommand(Command::PI_CMD_WRITE, gpio, static_cast<uint32_t>(level));
        return {result.Error};
    }

} // namespace pigpio_remote