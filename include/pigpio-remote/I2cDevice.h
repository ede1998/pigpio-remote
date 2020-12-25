#ifndef PIGPIO_REMOTE_INCLUDE_PIGPIO_REMOTE_I2CDEVICE_H
#define PIGPIO_REMOTE_INCLUDE_PIGPIO_REMOTE_I2CDEVICE_H

#include "PiConnection.h"
#include "PigpioError.h"
#include <limits>

namespace pigpio_remote
{
    using I2cOpenError = PigpioErrorView<PigpioError::PI_BAD_I2C_BUS,
                                         PigpioError::PI_BAD_I2C_ADDR,
                                         PigpioError::PI_BAD_FLAGS,
                                         PigpioError::PI_NO_HANDLE,
                                         PigpioError::PI_I2C_OPEN_FAILED,
                                         PigpioError::PI_I2C_CONNECTION_OPENED_ALREADY>;

    using I2cCloseError = PigpioErrorView<PigpioError::PI_BAD_HANDLE>;

    using I2cWriteError = PigpioErrorView<PigpioError::PI_BAD_PARAM,
                                          PigpioError::PI_BAD_HANDLE,
                                          PigpioError::PI_I2C_WRITE_FAILED>;

    using I2cReadError = PigpioErrorView<PigpioError::PI_BAD_HANDLE, PigpioError::PI_I2C_READ_FAILED>;

    using I2cZipError = PigpioErrorView<PigpioError::PI_BAD_HANDLE,
                                        PigpioError::PI_BAD_POINTER,
                                        PigpioError::PI_BAD_I2C_CMD,
                                        PigpioError::PI_BAD_I2C_RLEN,
                                        PigpioError::PI_BAD_I2C_WLEN,
                                        PigpioError::PI_BAD_I2C_SEG>;

    /**
    * @brief Represents a device on an I2C bus.
    *
    * To make it easier to understand, what a method actually sends,
    * the low level transactions are shown at the end of the function
    * description in this class. The following abbreviations are used.
    * 
    * ```
    * S       (1 bit) : Start bit
    * P       (1 bit) : Stop bit
    * Rd/Wr   (1 bit) : Read/Write bit. Rd equals 1, Wr equals 0.
    * A, NA   (1 bit) : Accept and not accept bit.
    *
    * Addr    (7 bits): I2C 7 bit address.
    * i2c_reg (8 bits): A byte which often selects a register.
    * Data    (8 bits): A data byte.
    * Count   (8 bits): A byte defining the length of a block operation.
    *
    * [..]: Data sent by the device.
    * ```
    */
    class I2cDevice
    {
    private:
        int _handle = INVALID_HANDLE;
        uint8_t _address = INVALID_ADDRESS;
        unsigned int _bus = INVALID_BUS;
        PiConnection& _connection;

        static constexpr int INVALID_HANDLE = -1;
    public:

        /**
         * @brief Adress that is set when the I2cDevice is constructed.
         * 
         * 3 is currently "reserved for future purposes", so it shouldn't be used
         * for now.
         */
        static constexpr uint8_t INVALID_ADDRESS = 3;
    
        /**
         * @brief Bus number that is set when the bus for the I2cDevice has not yet been set.
         */
        static constexpr unsigned int INVALID_BUS = std::numeric_limits<unsigned int>::max();

        /**
         * @brief Get the current device address. If none is set, \p I2cDevice::INVALID_ADDRESS is returned.
         * 
         * @return uint8_t device address
         */
        uint8_t GetAddress() const { return this->_address; }

        /**
         * @brief Get the device's bus number. If none is set, \p I2cDevice::INVALID_BUS is returned.
         * 
         * @return unsigned int bus number
         */
        unsigned int GetBus() const { return this->_bus; }

        /**
         * @brief Destroy the I2cDevice object and close the connection if any is open.
         */
        ~I2cDevice() { this->Close(); }

        /**
         * @brief Construct a new I2cDevice object using a PiConnection
         * 
         * @param connection connection to use
         */
        explicit I2cDevice(PiConnection &connection) : _connection(connection) {}

        /**
         * @brief Opens a connection to the specified I2C address.
         *
         * This method does not send any data on the I2C bus.
         *
         * Physically buses 0 and 1 are available on the Pi.
         * Higher numbered buses will be available if a kernel supported bus multiplexor is being used.
         * The GPIO used are given in the following table.
         *
         * Bus   | SDA | SCL
         * :----:|:---:|:---:
         * I2C 0 |  0  |  1
         * I2C 1 |  2  |  3
         *
         * @param address the I2C address of the device
         * @param bus the bus to use
         * @return I2cOpenError
         */
        I2cOpenError Open(uint8_t address, unsigned int bus = 1);

        /**
         * @brief Closes the connection to the I2C device.
         *
         * This method does not send any data on the I2C bus.
         *
         * @return I2cCloseError
         */
        I2cCloseError Close();

        /**
         * @brief This sends a single bit (in the Rd/Wr bit) to the device.
         *
         * I2C data: `S Addr bit [A] P`
         * @param bit bit to send
         * @return I2cWriteError
         */
        I2cWriteError WriteQuick(bool bit);

        /**
         * @brief This sends a single byte to the device.
         *
         * I2C data: `S Addr Wr [A] byte [A] P`
         * @param byte data to send
         * @return I2cWriteError
         */
        I2cWriteError WriteByte(uint8_t byte);

        /**
         * @brief This reads a single byte from the device.
         *
         * I2C data: `S Addr Rd [A] [Data] NA P`
         * @return PigpioResult<uint8_t, I2cReadError> read data byte or error code
         */
        PigpioResult<uint8_t, I2cReadError> ReadByte();

        /**
         * @brief This writes a single byte to the specified register of the device.
         *
         * I2C data: `S Addr Wr [A] i2c_reg [A] data [A] P`
         * @param i2c_reg register to write in
         * @param data data byte to write
         * @return I2cWriteError
         */
        I2cWriteError WriteByteData(uint8_t i2c_reg, uint8_t data);

        /**
         * @brief This reads a single byte from the specified register of the device.
         *
         * I2C data: `S Addr Wr [A] i2c_reg [A] S Addr Rd [A] [Data] NA P`
         * @param i2c_reg register to read from
         * @return PigpioResult<uint8_t, I2cReadError> read data byte or error code
         */
        PigpioResult<uint8_t, I2cReadError> ReadByteData(uint8_t i2c_reg);

        /**
         * @brief This writes a single 16 bit word to the specified register of the device.
         *
         * I2C data: `S Addr Wr [A] i2c_reg [A] data_Low [A] data_High [A] P`
         * @param i2c_reg register to write in
         * @param data data word to write
         * @return I2cWriteError
         */
        I2cWriteError WriteWordData(uint8_t i2c_reg, uint16_t data);

        /**
         * @brief This reads a single 16 bit word from the specified register of the device.
         *
         * I2C data: `S Addr Wr [A] i2c_reg [A] S Addr Rd [A] [DataLow] A [DataHigh] NA P`
         * @param i2c_reg register to read from
         * @return PigpioResult<uint16_t, I2cReadError> read data word or error code
         */
        PigpioResult<uint16_t, I2cReadError> ReadWordData(uint8_t i2c_reg);

        /**
         * @brief This writes 16 bits of data to the specified register of the device and reads 16 bits of data in return.
         *
         * I2C data: `S Addr Wr [A] i2c_reg [A] data_word_Low [A] data_word_High [A] S Addr Rd [A] [DataLow] A [DataHigh] NA P`
         * @param i2c_reg register to write the data in
         * @return PigpioResult<uint16_t, I2cReadError> read data word or error code
         */
        PigpioResult<uint16_t, I2cReadError> ProcessCall(uint8_t i2c_reg, uint16_t data_word);

        /**
         * @brief This writes up to 32 bytes to the specified register of the device.
         *
         * This method differs from WriteI2cBlockData() because with this method the number of
         * bytes to write is sent to the device beforehand.
         *
         * I2C data: `S Addr Wr [A] i2c_reg [A] count [A] data0 [A] data1 [A] ... [A] datan [A] P`
         * @param i2c_reg register to write the data in
         * @param data the data to write
         * @return I2cWriteError
         */
        I2cWriteError WriteBlockData(uint8_t i2c_reg, const std::vector<uint8_t> &data);

        /**
         * @brief This reads a block of up to 32 bytes from the specified register of the device.
         *
         * This method differs from ReadI2cBlockData() because with this method the number of
         * bytes to read is set by the device and sent over I2C before the transmission starts.
         *
         * I2C data: `S Addr Wr [A] i2c_reg [A] S Addr Rd [A] [Count] A [buf0] A [buf1] A ... A [bufn] NA P`
         * @param i2c_reg register to read from
         * @return PigpioResult<std::vector<uint8_t>, I2cReadError> read data or error code
         */
        PigpioResult<std::vector<uint8_t>, I2cReadError> ReadBlockData(uint8_t i2c_reg);

        /**
         * @brief This writes the bytes from data to the specified register of the device and reads a device
         * specified number of bytes of data in return.
         *
         * The smbus 2.0 documentation states that a minimum of 1 byte may be sent and a minimum of 1 byte may be received.
         * The total number of bytes sent/received must be 32 or less.
         *
         * I2C data: `S Addr Wr [A] i2c_reg [A] count [A] buf0 [A] ... S Addr Rd [A] [Count] A [Data] ... A P`
         * @param i2c_reg register to write to
         * @param data data to write
         * @return PigpioResult<std::vector<uint8_t>, I2cReadError> received data or error code
         */
        PigpioResult<std::vector<uint8_t>, I2cReadError> BlockProcessCall(uint8_t i2c_reg, const std::vector<uint8_t> &data);

        /**
         * @brief This writes 1 to 32 bytes to the specified register of the device.
         *
         * This method differs from WriteBlockData() because with this method the number of
         * bytes to write is not sent to the device beforehand.
         *
         * I2C data: `S Addr Wr [A] i2c_reg [A] data0 [A] data1 [A] ... [A] datan [A] P`
         * @param i2c_reg register to write to
         * @param data data to write
         * @return I2cWriteError
         */
        I2cWriteError WriteI2cBlockData(uint8_t i2c_reg, const std::vector<uint8_t> &data);

        /**
         * @brief This reads count bytes from the specified register of the device. The count may be 1-32.
         *
         * This method differs from ReadBlockData() because with this method the number of
         * bytes to read is not set by the device and not sent on the I2C bus.
         *
         *
         * I2C data: `S Addr Wr [A] i2c_reg [A] S Addr Rd [A] [buf0] A [buf1] A ... A [bufn] NA P`
         * @param i2c_reg register to read from
         * @param count number of bytes to read, must be between [1-32]
         * @return PigpioResult<std::vector<uint8_t>, I2cReadError> received data or error code
         */
        PigpioResult<std::vector<uint8_t>, I2cReadError> ReadI2cBlockData(uint8_t i2c_reg, uint8_t count);

        /**
         * @brief This writes the bytes in \p data to the raw device.
         *
         * I2C data: `S Addr Wr [A] data0 [A] data1 [A] ... [A] datan [A] P`
         * @param data data to write
         * @return I2cWriteError
         */
        I2cWriteError Write(const std::vector<uint8_t> &data);

        /**
         * @brief This reads count bytes from the raw device.
         *
         * I2C data: `S Addr Rd [A] [data0] A [data1] A ... A [datan] NA P`
         * @param count number of bytes to read
         * @return PigpioResult<std::vector<uint8_t>, I2cReadError> read bytes or error code
         */
        PigpioResult<std::vector<uint8_t>, I2cReadError> Read(unsigned int count);

        /**
         * @brief This function executes a sequence of I2C operations.
         * The operations to be performed are specified by the contents of
         * \p input_sequence which contains the concatenated command codes
         * and associated data.
         * 
         * The following command codes are supported:
         * 
         * Name     | Cmd & Data | Meaning
         * :-------:|:----------:|:------------------------------
         * End      |     0      | No more commands
         * Escape   |     1      | Next P is two bytes
         * On       |     2      | Switch combined flag on
         * Off      |     3      | Switch combined flag off
         * Address  |     4 P    | Set I2C address to P
         * Flags    |  5 lsb msb | Set I2C flags to lsb + (msb << 8)
         * Read     |     6 P    | Read P bytes of data
         * Write    |   7 P ...  | Write P bytes of data
         * 
         * 
         * The address, read, and write commands take a parameter P.
         * Normally P is one byte (0-255). If the command is preceded by the Escape
         * command then P is two bytes (0-65535, least significant byte first).
         * 
         * The address defaults to that associated with the handle. The flags default
         * to 0. The address and flags maintain their previous value until updated.
         * 
         * The returned I2C data is stored in the return value.
         * 
         * Example
         * 
         * ```
         * Set address 0x53, write 0x32, read 6 bytes
         * Set address 0x1E, write 0x03, read 6 bytes
         * Set address 0x68, write 0x1B, read 8 bytes
         * End
         * 
         * 0x04 0x53   0x07 0x01 0x32   0x06 0x06
         * 0x04 0x1E   0x07 0x01 0x03   0x06 0x06
         * 0x04 0x68   0x07 0x01 0x1B   0x06 0x08
         * 0x00
         * ```
         * 
         * @param input_sequence program to determine what to send on the I2C bus
         * @return PigpioResult<std::vector<uint8_t>, I2cZipError> data received over the I2C bus
         */
        PigpioResult<std::vector<uint8_t>, I2cZipError> Zip(const std::vector<uint8_t> &input_sequence);
    };

} // namespace pigpio_remote

#endif // PIGPIO_REMOTE_INCLUDE_PIGPIO_REMOTE_I2CDEVICE_H