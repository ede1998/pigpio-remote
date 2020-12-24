#ifndef PIGPIO_REMOTE_INCLUDE_PIGPIO_REMOTE_BASICIO_H 
#define PIGPIO_REMOTE_INCLUDE_PIGPIO_REMOTE_BASICIO_H 

#include "PiConnection.h"
#include "PigpioError.h"

enum class GpioMode
{
    PI_INPUT = 0,
    PI_OUTPUT = 1,
    PI_ALT0 = 4,
    PI_ALT1 = 5,
    PI_ALT2 = 6,
    PI_ALT3 = 7,
    PI_ALT4 = 3,
    PI_ALT5 = 2
};

enum class GpioPullUpDown
{
    PI_PUD_OFF  = 0,
    PI_PUD_DOWN = 1,
    PI_PUD_UP   = 2
};

enum class GpioLevel
{
    PI_OFF = 0,
    PI_CLEAR = PI_OFF,
    PI_LOW = PI_OFF,
    PI_ON = 1,
    PI_SET = PI_ON,
    PI_HIGH = PI_ON,
};


using GpioReadError = PigpioErrorView<PigpioError::PI_BAD_GPIO>;

using GpioModeError = PigpioErrorView<PigpioError::PI_BAD_GPIO,
                                      PigpioError::PI_BAD_MODE,
                                      PigpioError::PI_NOT_PERMITTED>;

using GpioPullUpDownError = PigpioErrorView<PigpioError::PI_BAD_GPIO,
                                            PigpioError::PI_BAD_PUD,
                                            PigpioError::PI_NOT_PERMITTED>;

using GpioWriteError = PigpioErrorView<PigpioError::PI_BAD_GPIO,
                                       PigpioError::PI_BAD_LEVEL,
                                       PigpioError::PI_NOT_PERMITTED>;

class BasicIo
{
private:
    PiConnection &_connection;

public:
    explicit BasicIo(PiConnection &connection);

    GpioModeError SetMode(unsigned int gpio, GpioMode mode);
    PigpioResult<GpioMode, GpioReadError> GetMode(unsigned int gpio);
    GpioPullUpDownError SetPullUpDown(unsigned int gpio, GpioPullUpDown pud);
    PigpioResult<GpioLevel, GpioReadError> Read(unsigned int gpio);
    GpioWriteError Write(unsigned int gpio, GpioLevel level);
};

#endif // PIGPIO_REMOTE_INCLUDE_PIGPIO_REMOTE_BASICIO_H
