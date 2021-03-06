#ifndef PIGPIO_REMOTE_INCLUDE_PIGPIO_REMOTE_PIGPIOERROR_H
#define PIGPIO_REMOTE_INCLUDE_PIGPIO_REMOTE_PIGPIOERROR_H

#include "../../external/tl/optional.hpp"
#include "../../src/communication/ErrorCode.h"

namespace pigpio_remote
{
    using communication::PigpioError;

    /**
     * @brief Represents an error that occurred during communcation with the Pigpio daemon.
     * 
     * This class wraps a \p PigpioError. When it is casted to that type or int, the internal
     * error code is checked against the \p allowed_errors. If the stored error is forbidden,
     * \p PigpioError::PI_UNKNOWN_ERROR is returned instead.
     * 
     * There are various usings for that class that define a specific set of allowed errors.
     * @tparam allowed_errors Error that may occur.
     */
    template <communication::PigpioError... allowed_errors>
    class BasicPigpioErrorView
    {
    private:
        const PigpioError _error;

        template <typename = void>
        bool ErrorAllowed() const
        {
            return false;
        }

        // This way, code to compare all allowed errors to the one that occurred is generated at compile time.
        // Source to make it work: https://stackoverflow.com/a/51000072
        template <PigpioError e, PigpioError... others>
        bool ErrorAllowed() const
        {
            return this->_error == e || this->ErrorAllowed<others...>();
        }

        bool IsErrorAllowed() const
        {
            return ErrorAllowed<allowed_errors...>();
        }

    public:
        /**
         * @brief Construct a new BasicPigpioErrorView object.
         * 
         * @param error Error code that is stored within the class.
         */
        //NOLINTNEXTLINE(hicpp-explicit-conversions) Allows using initializer lists for return and makes life much easier.
        BasicPigpioErrorView(const PigpioError error)
            : _error(error)
        {
        }

        /**
         * @brief Compares the class and a \p PigpioError.
         * 
         * If the contained \p PigpioError in the class is not an \p allowed_error, \p PigpioError::PI_UNEXPECTED_ERROR is used
         * for comparison.
         * 
         * @return true if same error
         * @return false if different error
         */
        bool operator==(PigpioError error) const
        {
            auto externally_visible_error = static_cast<PigpioError>(*this);
            return externally_visible_error == error;
        }

        /**
         * @brief Compares the class and a \p PigpioError.
         * 
         * If the contained \p PigpioError in the class is not an \p allowed_error, \p PigpioError::PI_UNEXPECTED_ERROR is used
         * for comparison.
         * 
         * @return true if different error
         * @return false if same error
         */
        bool operator!=(PigpioError error) const
        {
            return !(*this == error);
        }
        
        /**
         * @brief Get the stored `PigpioError`.
         * 
         * If the error is not allowed, `PigpioError::PI_UNEXPECTED_ERROR` is returned instead.
         * 
         * @return PigpioError the error code
         */
        explicit operator PigpioError() const
        {
            return this->IsErrorAllowed() ? this->_error : PigpioError::PI_UNEXPECTED_ERROR;
        }

        /**
         * @brief Get the stored `PigpioError`.
         * 
         * If the error is not allowed, `PigpioError::PI_UNEXPECTED_ERROR` is returned instead.
         * 
         * @return PigpioError the error code
         */
        explicit operator int() const
        {
            return static_cast<int>(static_cast<PigpioError>(*this));
        }

        /**
         * @brief Get the Error object without checking if the error is allowed.
         * 
         * @return the stored error 
         */
        PigpioError GetInternalError() const
        {
            return this->_error;
        }
    };

    template <PigpioError... allowed_errors>
    using PigpioErrorView = BasicPigpioErrorView<PigpioError::PI_OK,
                                                 PigpioError::pigif_bad_send,
                                                 PigpioError::pigif_bad_recv,
                                                 PigpioError::pigif_unconnected_pi, allowed_errors...>;

    /**
     * @brief Stores a result or an error depending on what happened.
     * 
     * @tparam ValueT type of the result
     * @tparam ErrorT error type that might occur
     */
    template <typename ValueT, typename ErrorT>
    class PigpioResult
    {
    public:
        //NOLINTNEXTLINE(misc-non-private-member-variables-in-classes) Making this private seems stupid.
        tl::optional<ValueT> Value;
        //NOLINTNEXTLINE(misc-non-private-member-variables-in-classes) Making this private seems stupid.
        ErrorT Error;

        //NOLINTNEXTLINE(hicpp-explicit-conversions) Allows using initializer lists for return and makes life much easier.
        PigpioResult(ValueT value)
            : Value(value), Error(ErrorT(PigpioError::PI_OK))
        {
        }

        //NOLINTNEXTLINE(hicpp-explicit-conversions) Allows using initializer lists for return and makes life much easier.
        PigpioResult(PigpioError error)
            : Error(ErrorT(error))
        {
        }
    };

} // namespace pigpio_remote
#endif // PIGPIO_REMOTE_INCLUDE_PIGPIO_REMOTE_PIGPIOERROR_H