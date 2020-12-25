#include "../include/pigpio-remote/I2cDevice.h"

#include <array>
#include "communication/Command.h"

namespace pigpio_remote
{

    using namespace communication;
    using namespace internal;

    I2cOpenError I2cDevice::Open(const uint8_t address, const unsigned int bus)
    {
        if (this->_handle != INVALID_HANDLE)
            return {PigpioError::PI_I2C_CONNECTION_OPENED_ALREADY};

        uint32_t i2c_flags = 0;
        const std::array<uint32_t, 3> parameters = {bus, address, sizeof(i2c_flags)};
        const std::vector<uint8_t> data(&i2c_flags, &i2c_flags + sizeof(i2c_flags));

        auto result = this->_connection.SendCommand(Command::PI_CMD_I2CO, parameters, data);

        if (result.Error == PigpioError::PI_OK)
        {
            this->_address = address;
            this->_bus = bus;
            this->_handle = result.Result;
        }

        return {result.Error};
    }

    I2cCloseError I2cDevice::Close()
    {
        if (this->_handle == INVALID_HANDLE)
            return {PigpioError::PI_BAD_HANDLE};

        auto result = this->_connection.SendCommand(Command::PI_CMD_I2CC, this->_handle);

        if (result.Error == PigpioError::PI_OK)
        {
            this->_handle = INVALID_HANDLE;
            this->_bus = INVALID_BUS;
            this->_address = INVALID_ADDRESS;
        }

        return {result.Error};
    }

    I2cWriteError I2cDevice::WriteQuick(const bool bit)
    {
        if (this->_handle == INVALID_HANDLE)
            return {PigpioError::PI_BAD_HANDLE};

        auto result = this->_connection.SendCommand(Command::PI_CMD_I2CWQ, this->_handle, static_cast<uint32_t>(bit));
        return {result.Error};
    }

    I2cWriteError I2cDevice::WriteByte(const uint8_t byte)
    {
        if (this->_handle == INVALID_HANDLE)
            return {PigpioError::PI_BAD_HANDLE};

        auto result = this->_connection.SendCommand(Command::PI_CMD_I2CWS, this->_handle, byte);
        return {result.Error};
    }

    PigpioResult<uint8_t, I2cReadError> I2cDevice::ReadByte()
    {
        if (this->_handle == INVALID_HANDLE)
            return {PigpioError::PI_BAD_HANDLE};

        auto result = this->_connection.SendCommand(Command::PI_CMD_I2CRS, this->_handle);
        return MakeResult<uint8_t, I2cReadError>(result);
    }

    I2cWriteError I2cDevice::WriteByteData(const uint8_t i2c_reg, const uint8_t data)
    {
        if (this->_handle == INVALID_HANDLE)
            return {PigpioError::PI_BAD_HANDLE};
        
        const uint32_t data_32 = data;
        const std::array<uint32_t, 3> parameters = {static_cast<uint32_t>(this->_handle), i2c_reg, sizeof(data_32)};
        const std::vector<uint8_t> buffer(&data_32, &data_32 + sizeof(data_32));

        auto result = this->_connection.SendCommand(Command::PI_CMD_I2CWB, parameters, buffer);
        return {result.Error};
    }

    PigpioResult<uint8_t, I2cReadError> I2cDevice::ReadByteData(const uint8_t i2c_reg)
    {
        if (this->_handle == INVALID_HANDLE)
            return {PigpioError::PI_BAD_HANDLE};

        auto result = this->_connection.SendCommand(Command::PI_CMD_I2CRB, this->_handle, i2c_reg);
        return MakeResult<uint8_t, I2cReadError>(result);
    }

    I2cWriteError I2cDevice::WriteWordData(const uint8_t i2c_reg, const uint16_t data)
    {
        if (this->_handle == INVALID_HANDLE)
            return {PigpioError::PI_BAD_HANDLE};

        const uint32_t data_32 = data;
        const std::array<uint32_t, 3> parameters = {static_cast<uint32_t>(this->_handle), i2c_reg, sizeof(data_32)};
        const std::vector<uint8_t> buffer(&data_32, &data_32 + sizeof(data_32));

        auto result = this->_connection.SendCommand(Command::PI_CMD_I2CWW, parameters, buffer);
        return {result.Error};
    }

    PigpioResult<uint16_t, I2cReadError> I2cDevice::ReadWordData(const uint8_t i2c_reg)
    {
        if (this->_handle == INVALID_HANDLE)
            return {PigpioError::PI_BAD_HANDLE};

        auto result = this->_connection.SendCommand(Command::PI_CMD_I2CRW, this->_handle, i2c_reg);
        return MakeResult<uint16_t, I2cReadError>(result);
    }

    PigpioResult<uint16_t, I2cReadError> I2cDevice::ProcessCall(const uint8_t i2c_reg, const uint16_t data_word)
    {
        if (this->_handle == INVALID_HANDLE)
            return {PigpioError::PI_BAD_HANDLE};

        const uint32_t data_32 = data_word;
        const std::array<uint32_t, 3> parameters = {static_cast<uint32_t>(this->_handle), i2c_reg, sizeof(data_32)};
        const std::vector<uint8_t> buffer(&data_32, &data_32 + sizeof(data_32));

        auto result = this->_connection.SendCommand(Command::PI_CMD_I2CPC, parameters, buffer);
        return MakeResult<uint16_t, I2cReadError>(result);
    }

    I2cWriteError I2cDevice::WriteBlockData(const uint8_t i2c_reg, const std::vector<uint8_t> &data)
    {
        if (this->_handle == INVALID_HANDLE)
            return {PigpioError::PI_BAD_HANDLE};

        constexpr uint32_t MAX_DATA_TO_TRANSFER = 32;

        if (data.size() > MAX_DATA_TO_TRANSFER)
        {
            return {PigpioError::PI_I2C_WRITE_FAILED};
        }

        const std::array<uint32_t, 3> parameters = {static_cast<uint32_t>(this->_handle), i2c_reg, static_cast<uint32_t>(data.size())};

        auto result = this->_connection.SendCommand(Command::PI_CMD_I2CWK, parameters, data);
        return {result.Error};
    }

    PigpioResult<std::vector<uint8_t>, I2cReadError> I2cDevice::ReadBlockData(const uint8_t i2c_reg)
    {
        if (this->_handle == INVALID_HANDLE)
            return {PigpioError::PI_BAD_HANDLE};

        //_pml(pi);
        auto result = this->_connection.SendCommand(Command::PI_CMD_I2CRK, this->_handle, i2c_reg);

        if (result.Error == PigpioError::PI_OK)
        {
            constexpr uint32_t MAX_DATA_TO_RETURN = 32;
            auto data = this->_connection.ReceiveExtraData(result.Result, MAX_DATA_TO_RETURN);

            //_pmu(pi);
            return {data};
        }

        //_pmu(pi);
        return {result.Error};
    }

    PigpioResult<std::vector<uint8_t>, I2cReadError> I2cDevice::BlockProcessCall(const uint8_t i2c_reg, const std::vector<uint8_t> &data)
    {
        if (this->_handle == INVALID_HANDLE)
            return {PigpioError::PI_BAD_HANDLE};

        constexpr uint32_t MAX_DATA_TO_TRANSFER = 32;

        if (data.size() > MAX_DATA_TO_TRANSFER)
        {
            return {PigpioError::PI_I2C_READ_FAILED};
        }

        const std::array<uint32_t, 3> parameters = {static_cast<uint32_t>(this->_handle), i2c_reg, static_cast<uint32_t>(data.size())};

        //_pml(pi);
        auto result = this->_connection.SendCommand(Command::PI_CMD_I2CPK, parameters, data);

        if (result.Error == PigpioError::PI_OK)
        {
            auto data = this->_connection.ReceiveExtraData(result.Result, MAX_DATA_TO_TRANSFER);

            //_pmu(pi);
            return {data};
        }

        //_pmu(pi);
        return {result.Error};
    }

    I2cWriteError I2cDevice::WriteI2cBlockData(const uint8_t i2c_reg, const std::vector<uint8_t> &data)
    {
        if (this->_handle == INVALID_HANDLE)
            return {PigpioError::PI_BAD_HANDLE};

        constexpr uint32_t MAX_DATA_TO_TRANSFER = 32;

        if (data.size() > MAX_DATA_TO_TRANSFER)
        {
            return {PigpioError::PI_I2C_WRITE_FAILED};
        }

        const std::array<uint32_t, 3> parameters = {static_cast<uint32_t>(this->_handle), i2c_reg, static_cast<uint32_t>(data.size())};

        auto result = this->_connection.SendCommand(Command::PI_CMD_I2CWI, parameters, data);
        return {result.Error};
    }

    PigpioResult<std::vector<uint8_t>, I2cReadError> I2cDevice::ReadI2cBlockData(const uint8_t i2c_reg, const uint8_t count)
    {
        if (this->_handle == INVALID_HANDLE)
            return {PigpioError::PI_BAD_HANDLE};

        constexpr uint32_t MAX_DATA_TO_TRANSFER = 32;

        if (count > MAX_DATA_TO_TRANSFER)
        {
            return {PigpioError::PI_I2C_READ_FAILED};
        }

        const uint32_t count_32 = count;
        const std::array<uint32_t, 3> parameters = {static_cast<uint32_t>(this->_handle), i2c_reg, sizeof(count_32)};
        const std::vector<uint8_t> buffer(&count_32, &count_32 + sizeof(count_32));

        //_pml(pi);
        auto result = this->_connection.SendCommand(Command::PI_CMD_I2CRI, parameters, buffer);

        if (result.Error == PigpioError::PI_OK)
        {
            auto data = this->_connection.ReceiveExtraData(result.Result, count);

            //_pmu(pi);
            return {data};
        }

        //_pmu(pi);
        return {result.Error};
    }

    I2cWriteError I2cDevice::Write(const std::vector<uint8_t> &data)
    {
        if (this->_handle == INVALID_HANDLE)
            return {PigpioError::PI_BAD_HANDLE};

        constexpr uint32_t NO_REGISTER = 0;
        const std::array<uint32_t, 3> parameters = {static_cast<uint32_t>(this->_handle), NO_REGISTER, static_cast<uint32_t>(data.size())};

        auto result = this->_connection.SendCommand(Command::PI_CMD_I2CWD, parameters, data);
        return {result.Error};
    }

    PigpioResult<std::vector<uint8_t>, I2cReadError> I2cDevice::Read(unsigned int count)
    {
        if (this->_handle == INVALID_HANDLE)
            return {PigpioError::PI_BAD_HANDLE};

        const uint32_t count_32 = count;

        //_pml(pi);
        auto result = this->_connection.SendCommand(Command::PI_CMD_I2CRD, this->_handle, count);

        if (result.Error == PigpioError::PI_OK)
        {
            auto data = this->_connection.ReceiveExtraData(result.Result, count);

            //_pmu(pi);
            return {data};
        }

        //_pmu(pi);
        return {result.Error};
    }

    PigpioResult<std::vector<uint8_t>, I2cZipError> I2cDevice::Zip(const std::vector<uint8_t> &input_sequence)
    {
        if (this->_handle == INVALID_HANDLE)
            return {PigpioError::PI_BAD_HANDLE};

        constexpr uint32_t NO_REGISTER = 0;
        const std::array<uint32_t, 3> parameters = {static_cast<uint32_t>(this->_handle), NO_REGISTER, static_cast<uint32_t>(input_sequence.size())};

        //_pml(pi);
        auto result = this->_connection.SendCommand(Command::PI_CMD_I2CZ, parameters, input_sequence);

        if (result.Error == PigpioError::PI_OK)
        {
            auto data = this->_connection.ReceiveExtraData(result.Result, result.Result);

            //_pmu(pi);
            return {data};;
        }

        //_pmu(pi);
        return {result.Error};
    }

} // namespace pigpio_remote