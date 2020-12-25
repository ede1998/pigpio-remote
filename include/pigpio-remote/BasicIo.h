#ifndef PIGPIO_REMOTE_INCLUDE_PIGPIO_REMOTE_BASICIO_H
#define PIGPIO_REMOTE_INCLUDE_PIGPIO_REMOTE_BASICIO_H

#include "PiConnection.h"
#include "PigpioError.h"

namespace pigpio_remote
{

    /**
     * @brief Different modes a GPIO pin may be set to.
     */
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

    /**
     * @brief Represents the state of the pull up / down resistor of
     * the GPIO pin.
     */
    enum class GpioPullUpDown
    {
        PI_PUD_OFF = 0,
        PI_PUD_DOWN = 1,
        PI_PUD_UP = 2
    };

    /**
     * @brief State of the GPIO pin. Either 0 or 1. Everything else is
     * just another fancy name for either 0 or 1.
     */
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

    /**
     * @brief Allows control of GPIO pins in a very basic manner.
     */
    class BasicIo
    {
    private:
        PiConnection &_connection;

    public:
        /**
         * @brief Construct a new Basic Io object
         * 
         * The passed connection may or may not be opened already.
         * 
         * @param connection connection to an Pigpio daemon
         */
        explicit BasicIo(PiConnection &connection);
        
        /**
         * @brief Set the GPIO mode. 
         * 
         * @param gpio Any of the Raspberry Pi's GPIO pins [0-53]
         * @param mode `GpioMode` to set the pin to
         * @return GpioModeError `PigpioError::PI_OK` or an error code
         */
        GpioModeError SetMode(unsigned int gpio, GpioMode mode);

        /**
         * @brief Get the GPIO mode.
         * 
         * @param gpio Any of the Raspberry Pi's GPIO pins [0-53]
         * @return PigpioResult<GpioMode, GpioReadError>  The mode of the pin or an error code.
         */
        PigpioResult<GpioMode, GpioReadError> GetMode(unsigned int gpio);

        /**
         * @brief Set or clear the GPIO pull-up/down resistor.
         * 
         * @param gpio Any of the Raspberry Pi's GPIO pins [0-53]
         * @param pud State the resistor should be set to
         * @return GpioPullUpDownError `PigpioError::PI_OK` or an error code
         */
        GpioPullUpDownError SetPullUpDown(unsigned int gpio, GpioPullUpDown pud);

        /**
         * @brief Read the GPIO level.
         * 
         * @param gpio Any of the Raspberry Pi's GPIO pins [0-53]
         * @return PigpioResult<GpioLevel, GpioReadError> The pin level or an error code.
         */
        PigpioResult<GpioLevel, GpioReadError> Read(unsigned int gpio);

        /**
         * @brief Write the GPIO level.
         * 
         * @param gpio Any of the Raspberry Pi's GPIO pins [0-53]
         * @param level Level to set the pin to
         * @return GpioWriteError `PigpioError::PI_OK` or an error code
         */
        GpioWriteError Write(unsigned int gpio, GpioLevel level);
    };

} // namespace pigpio_remote

#endif // PIGPIO_REMOTE_INCLUDE_PIGPIO_REMOTE_BASICIO_H
