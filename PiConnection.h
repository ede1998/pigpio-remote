#ifndef _PIGPIO_WRAPPER_PICONNECTION
#define _PIGPIO_WRAPPER_PICONNECTION 1

#include "NoNagleSyncClient.h"
#include "result.h"
#include "PigpioError.h"
#include "socket_command.h"
#include <string>

enum class PiConnectionError
{
    SUCCESS = 0,
    TIME_OUT = 1,
    INVALID_SERVER = 2,
    TRUNCATED = 3,
    INVALID_RESPONSE = 4
};

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

class PiConnection
{
private:
    NoNagleSyncClient _client;
    std::pair<int, PigpioError> test;
public:
    PiConnection();
    ~PiConnection();
    PiConnectionError connect(uint16_t port = 0);
    PiConnectionError connect(const std::string &addr, uint16_t port = 0);
    PiConnectionError connect(const char *addr, uint16_t port = 0);
    PiConnectionError connect(const IPAddress &addr, uint16_t port = 0);
    bool connected() const;

    void stop();

    SendCommandResult send_command(SocketCommand command, uint32_t parameter1, uint32_t parameter2);

    NoNagleSyncClient get_connection();
    static constexpr uint16_t DEFAULT_PORT = 8888;
    static constexpr char DEFAULT_ADDRESS[] = "localhost";
    static constexpr char ENV_ADDRESS[] = "PIGPIO_ADDR";
    static constexpr char ENV_PORT[] = "PIGPIO_PORT";
};

#endif // _PIGPIO_WRAPPER_PICONNECTION