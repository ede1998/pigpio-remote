#ifndef _PIGPIO_WRAPPER_PICONNECTION
#define _PIGPIO_WRAPPER_PICONNECTION 1

#include "../../src/platform/NoNagleSyncClient.h"
#include "PigpioError.h"
#include "../../src/socket_command.h"
#include <string>
#include <chrono>

struct SendCommandResult
{
    int Result;
    PigpioError Error;

    static SendCommandResult create(int result)
    {
        SendCommandResult res;
        res.Error = PigpioError::PI_OK;
        res.Result = result;
        return res;
    }

    static SendCommandResult create(PigpioError error)
    {
        SendCommandResult res;
        res.Error = error;
        return res;
    }
};

template <typename result_t, typename error_t>
PigpioResult<result_t, error_t> make_result(SendCommandResult result)
{
    if (result.Error == PigpioError::PI_OK)
    {
        return PigpioResult<result_t, error_t>(static_cast<result_t>(result.Result));
    }
    else
    {
        return PigpioResult<result_t, error_t>(result.Error);
    }
}

class PiConnection
{
private:
    NoNagleSyncClient _client;

public:
    PiConnection();
    ~PiConnection();
    ConnectionError connect(uint16_t port = 0);
    ConnectionError connect(const std::string &addr, uint16_t port = 0);
    ConnectionError connect(const char *addr, uint16_t port = 0);
    bool connected() const;

    void stop();

    SendCommandResult send_command(SocketCommand command, uint32_t parameter1, uint32_t parameter2 = UNUSED_PARAMETER);

    NoNagleSyncClient get_connection();
    static constexpr uint16_t DEFAULT_PORT = 8888;
    static constexpr char DEFAULT_ADDRESS[] = "localhost";
    static constexpr char ENV_ADDRESS[] = "PIGPIO_ADDR";
    static constexpr char ENV_PORT[] = "PIGPIO_PORT";
    static constexpr uint32_t UNUSED_PARAMETER = 0;
};

#endif // _PIGPIO_WRAPPER_PICONNECTION