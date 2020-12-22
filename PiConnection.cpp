#include "PiConnection.h"

#include <cstdlib>
#include <limits>

typedef void* pthread_t;
#include "pigpio1/command.h"

constexpr char PiConnection::ENV_ADDRESS[];
constexpr char PiConnection::ENV_PORT[];
constexpr char PiConnection::DEFAULT_ADDRESS[];

uint16_t determine_port(uint16_t original_port)
{
    if (original_port != 0)
    {
        return original_port;
    }

    const char *env = std::getenv(PiConnection::ENV_PORT);
    auto port = env == nullptr ? 0 : std::strtoul(env, nullptr, 0);

    if (port > std::numeric_limits<uint16_t>::max())
    {
        return 0;
    }

    if (port != 0)
    {
        return port;
    }

    return PiConnection::DEFAULT_PORT;
}

PiConnectionError PiConnection::connect(uint16_t port)
{
    port = determine_port(port);
    const char *env = std::getenv(PiConnection::ENV_ADDRESS);
    const char *addr = env ? env : PiConnection::DEFAULT_ADDRESS;
    return this->connect(addr, port);
}

PiConnectionError PiConnection::connect(const std::string &addr, uint16_t port)
{
    return this->connect(addr.c_str(), port);
}

PiConnectionError PiConnection::connect(const char *addr, uint16_t port)
{
    port = determine_port(port);
    if (port == 0)
    {
        return PiConnectionError::INVALID_SERVER;
    }
    auto result = this->_client.connect(addr, port);
    return static_cast<PiConnectionError>(result);
}

PiConnectionError PiConnection::connect(const IPAddress &addr, uint16_t port)
{
    port = determine_port(port);
    if (port == 0)
    {
        return PiConnectionError::INVALID_SERVER;
    }
    auto result = this->_client._connect(addr, port);
    return static_cast<PiConnectionError>(result);
}

bool PiConnection::connected() const
{
    return const_cast<NoNagleSyncClient &>(this->_client).connected();
}

void PiConnection::stop()
{
    this->_client.stop();
}

SendCommandResult PiConnection::send_command(SocketCommand command, uint32_t parameter1, uint32_t parameter2) 
{
    if (!this->connected())
    {
        return SendCommandResult::create(PigpioError::pigif_unconnected_pi);
    }

    cmdCmd_t cmd;
    cmd.cmd = static_cast<uint32_t>(command);
    cmd.p1 = parameter1;
    cmd.p2 = parameter2;
    cmd.res = 0;

    //_pml(pi);

    if (this->_client.immediate_write(reinterpret_cast<uint8_t *>(&cmd), sizeof(cmd)) != sizeof(cmd))
    {
        //_pmu(pi);
        return SendCommandResult::create(PigpioError::pigif_bad_send);
    }

    if (this->_client.read(reinterpret_cast<uint8_t *>(&cmd), sizeof(cmd)) != sizeof(cmd))
    {
        //_pmu(pi);
        return SendCommandResult::create(PigpioError::pigif_bad_recv);
    }

    //if (rl) _pmu(pi);

    return cmd.res >= 0 ? SendCommandResult::create(cmd.res)
                        : SendCommandResult::create(static_cast<PigpioError>(cmd.res));
}

NoNagleSyncClient PiConnection::get_connection()
{
    return this->_client;
}

PiConnection::PiConnection()
{
}

PiConnection::~PiConnection()
{
    this->stop();
}
