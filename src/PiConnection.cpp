#include "../include/pigpio-remote/PiConnection.h"

#include <cstdlib>
#include <limits>

#include "pigpio-communication/command.h"

constexpr const char *PiConnection::ENV_ADDRESS;
constexpr const char *PiConnection::ENV_PORT;
constexpr const char *PiConnection::DEFAULT_ADDRESS;

uint16_t DeterminePort(uint16_t original_port)
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

ConnectionError PiConnection::Connect(uint16_t port)
{
    port = DeterminePort(port);
    const char *env = std::getenv(PiConnection::ENV_ADDRESS);
    const char *addr = env != nullptr ? env : PiConnection::DEFAULT_ADDRESS;
    return this->Connect(addr, port);
}

ConnectionError PiConnection::Connect(const std::string &addr, uint16_t port)
{
    return this->Connect(addr.c_str(), port);
}

ConnectionError PiConnection::Connect(const char *addr, uint16_t port)
{
    if (this->Connected())
    {
        return ConnectionError::ALREADY_CONNECTED;
    }

    port = DeterminePort(port);
    if (port == 0)
    {
        return ConnectionError::INVALID_SERVER;
    }

    return this->_client.Connect(addr, port);
}

bool PiConnection::Connected() const
{
    return this->_client.Connected();
}

void PiConnection::Stop()
{
    // if (false) // notify stuff connected (not implemented yet)
    // {
    //     auto result = this->send_command(SocketCommand::PI_CMD_NC, /* replace with notification_handle*/ 0);
    // }
    this->_client.Stop();
}

SendCommandResult PiConnection::SendCommand(SocketCommand command, uint32_t parameter1, uint32_t parameter2)
{
    if (!this->Connected())
    {
        return SendCommandResult::Create(PigpioError::pigif_unconnected_pi);
    }

    cmdCmd_t cmd;
    cmd.Cmd = static_cast<uint32_t>(command);
    cmd.P1 = parameter1;
    cmd.P2 = parameter2;
    cmd.Res = 0;

    //_pml(pi);

    if (this->_client.Write(reinterpret_cast<uint8_t *>(&cmd), sizeof(cmd)) != sizeof(cmd))
    {
        //_pmu(pi);
        return SendCommandResult::Create(PigpioError::pigif_bad_send);
    }

    if (this->_client.Read(reinterpret_cast<uint8_t *>(&cmd), sizeof(cmd)) != sizeof(cmd))
    {
        //_pmu(pi);
        return SendCommandResult::Create(PigpioError::pigif_bad_recv);
    }

    //if (rl) _pmu(pi);

    return cmd.Res >= 0 ? SendCommandResult::Create(cmd.Res)
                        : SendCommandResult::Create(static_cast<PigpioError>(cmd.Res));
}

NoNagleSyncClient PiConnection::GetConnection()
{
    return this->_client;
}

PiConnection::~PiConnection()
{
    this->Stop();
}
