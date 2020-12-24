#ifndef _PIGPIO_WRAPPER_PICONNECTION
#define _PIGPIO_WRAPPER_PICONNECTION 1

#include "../../src/platform/NoNagleSyncClient.h"
#include "PigpioError.h"
#include "../../src/pigpio-communication/SocketCommand.h"
#include <string>
#include <chrono>

struct SendCommandResult
{
    int Result;
    PigpioError Error;

    static SendCommandResult Create(int result)
    {
        SendCommandResult res;
        res.Error = PigpioError::PI_OK;
        res.Result = result;
        return res;
    }

    static SendCommandResult Create(PigpioError error)
    {
        SendCommandResult res;
        res.Error = error;
        return res;
    }
};

template <typename ResultT, typename ErrorT>
PigpioResult<ResultT, ErrorT> MakeResult(SendCommandResult result)
{
    if (result.Error == PigpioError::PI_OK)
    {
        return PigpioResult<ResultT, ErrorT>(static_cast<ResultT>(result.Result));
    }

    return PigpioResult<ResultT, ErrorT>(result.Error);
}

class PiConnection
{
private:
    NoNagleSyncClient _client;

public:
    PiConnection() = default;
    ~PiConnection();
    ConnectionError Connect(uint16_t port = 0);
    ConnectionError Connect(const std::string &addr, uint16_t port = 0);
    ConnectionError Connect(const char *addr, uint16_t port = 0);
    bool Connected() const;

    void Stop();

    SendCommandResult SendCommand(SocketCommand command, uint32_t parameter1, uint32_t parameter2 = UNUSED_PARAMETER);

    NoNagleSyncClient GetConnection();
    static constexpr uint16_t DEFAULT_PORT = 8888;
    static constexpr const char *DEFAULT_ADDRESS = "localhost";
    static constexpr const char *ENV_ADDRESS = "PIGPIO_ADDR";
    static constexpr const char *ENV_PORT = "PIGPIO_PORT";
    static constexpr uint32_t UNUSED_PARAMETER = 0;
};

#endif // _PIGPIO_WRAPPER_PICONNECTION