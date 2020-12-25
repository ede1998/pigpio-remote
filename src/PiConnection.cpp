#include "../include/pigpio-remote/PiConnection.h"

#include <cstdlib>
#include <limits>

#include "communication/CommandBlock.h"

namespace pigpio_remote
{

    using namespace internal;
    using namespace communication;
    using namespace platform;

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

    CommandResult PiConnection::SendCommand(Command command, uint32_t parameter1, uint32_t parameter2)
    {
        if (!this->Connected())
        {
            return CommandResult::Create(PigpioError::pigif_unconnected_pi);
        }

        CommandBlock cmd;
        cmd.Cmd = static_cast<uint32_t>(command);
        cmd.P1 = parameter1;
        cmd.P2 = parameter2;
        cmd.Res = 0;

        //_pml(pi);

        if (this->_client.Write(reinterpret_cast<uint8_t *>(&cmd), sizeof(cmd)) != sizeof(cmd))
        {
            //_pmu(pi);
            return CommandResult::Create(PigpioError::pigif_bad_send);
        }

        if (this->_client.Read(reinterpret_cast<uint8_t *>(&cmd), sizeof(cmd)) != sizeof(cmd))
        {
            //_pmu(pi);
            return CommandResult::Create(PigpioError::pigif_bad_recv);
        }

        //if (rl) _pmu(pi);

        return cmd.Res >= 0 ? CommandResult::Create(cmd.Res)
                            : CommandResult::Create(static_cast<PigpioError>(cmd.Res));
    }

    internal::CommandResult PiConnection::SendCommand(communication::Command command, const std::array<uint32_t, 3> &parameters, const std::vector<uint8_t> &data)
    {
        if (!this->Connected())
        {
            return CommandResult::Create(PigpioError::pigif_unconnected_pi);
        }

        int i;
        CommandBlock cmd;

        cmd.Cmd = static_cast<uint32_t>(command);
        cmd.P1 = parameters[0];
        cmd.P2 = parameters[1];
        cmd.P3 = parameters[2];

        // _pml(pi);

        if (this->_client.Write(reinterpret_cast<uint8_t *>(&cmd), sizeof(cmd)) != sizeof(cmd))
        {
            //_pmu(pi);
            return CommandResult::Create(PigpioError::pigif_bad_send);
        }

        const auto data_bytes = data.size(); // elements of data are uint8_t so 1 byte each

        // The original code uses a for loop here and gets an array of buffers as parameter instead of a single large buffer.
        // Since TCP is stream based this shouldn't matter but maybe it does, so watch out if you have trouble with this extended SendCommand method.
        if (this->_client.Write(data.data(), data_bytes) != data_bytes)
        {
            // _pmu(pi);
            return CommandResult::Create(PigpioError::pigif_bad_send);
        }

        if (this->_client.Read(reinterpret_cast<uint8_t *>(&cmd), sizeof(cmd)) != sizeof(cmd))
        {
            //_pmu(pi);
            return CommandResult::Create(PigpioError::pigif_bad_recv);
        }
        // if (rl) _pmu(pi);

        return cmd.Res >= 0 ? CommandResult::Create(cmd.Res)
                            : CommandResult::Create(static_cast<PigpioError>(cmd.Res));
    }

    NoNagleSyncClient PiConnection::GetConnection()
    {
        return this->_client;
    }

    PiConnection::~PiConnection()
    {
        this->Stop();
    }

} // namespace pigpio_remote