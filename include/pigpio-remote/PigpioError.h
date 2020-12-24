#ifndef __PIGPIOERROR_H__
#define __PIGPIOERROR_H__

#include "../../external/tl/optional.hpp"
#include "../../src/pigpio-communication/ErrorCode.h"

template <PigpioError... allowed_errors>
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
    BasicPigpioErrorView(const PigpioError error)
        : _error(error)
    {
    }

    operator PigpioError() const
    {
        return this->IsErrorAllowed() ? this->_error : PigpioError::PI_UNEXPECTED_ERROR;
    }

    operator int() const
    {
        return static_cast<int>(static_cast<PigpioError>(*this));
    }

    PigpioError GetError() const
    {
        return this->_error;
    }
};

template <PigpioError... allowed_errors>
using PigpioErrorView = BasicPigpioErrorView<PigpioError::PI_OK,
                                             PigpioError::pigif_bad_send,
                                             PigpioError::pigif_bad_recv,
                                             PigpioError::pigif_unconnected_pi, allowed_errors...>;

template <typename ValueT, typename ErrorT>
class PigpioResult
{
public:
    tl::optional<ValueT> Value;
    ErrorT Error;

    PigpioResult(ValueT value)
        : Value(value), Error(ErrorT(PigpioError::PI_OK))
    {
    }

    PigpioResult(PigpioError error)
        : Error(ErrorT(error))
    {
    }
};

#endif // __PIGPIOERROR_H__