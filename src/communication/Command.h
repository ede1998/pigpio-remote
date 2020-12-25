#ifndef PIGPIO_REMOTE_SRC_COMMUNICATION_COMMAND_H
#define PIGPIO_REMOTE_SRC_COMMUNICATION_COMMAND_H

#include <cstdint>

namespace pigpio_remote
{

    namespace communication
    {
        /**
         * @brief Internal enum containing all the different commands that
         * the Pigpio daemon understands.
         */
        enum class Command : uint32_t
        {
            PI_CMD_MODES = 0,
            PI_CMD_MODEG = 1,
            PI_CMD_PUD = 2,
            PI_CMD_READ = 3,
            PI_CMD_WRITE = 4,
            PI_CMD_PWM = 5,
            PI_CMD_PRS = 6,
            PI_CMD_PFS = 7,
            PI_CMD_SERVO = 8,
            PI_CMD_WDOG = 9,
            PI_CMD_BR1 = 10,
            PI_CMD_BR2 = 11,
            PI_CMD_BC1 = 12,
            PI_CMD_BC2 = 13,
            PI_CMD_BS1 = 14,
            PI_CMD_BS2 = 15,
            PI_CMD_TICK = 16,
            PI_CMD_HWVER = 17,
            PI_CMD_NO = 18,
            PI_CMD_NB = 19,
            PI_CMD_NP = 20,
            PI_CMD_NC = 21,
            PI_CMD_PRG = 22,
            PI_CMD_PFG = 23,
            PI_CMD_PRRG = 24,
            PI_CMD_HELP = 25,
            PI_CMD_PIGPV = 26,
            PI_CMD_WVCLR = 27,
            PI_CMD_WVAG = 28,
            PI_CMD_WVAS = 29,
            PI_CMD_WVGO = 30,
            PI_CMD_WVGOR = 31,
            PI_CMD_WVBSY = 32,
            PI_CMD_WVHLT = 33,
            PI_CMD_WVSM = 34,
            PI_CMD_WVSP = 35,
            PI_CMD_WVSC = 36,
            PI_CMD_TRIG = 37,
            PI_CMD_PROC = 38,
            PI_CMD_PROCD = 39,
            PI_CMD_PROCR = 40,
            PI_CMD_PROCS = 41,
            PI_CMD_SLRO = 42,
            PI_CMD_SLR = 43,
            PI_CMD_SLRC = 44,
            PI_CMD_PROCP = 45,
            PI_CMD_MICS = 46,
            PI_CMD_MILS = 47,
            PI_CMD_PARSE = 48,
            PI_CMD_WVCRE = 49,
            PI_CMD_WVDEL = 50,
            PI_CMD_WVTX = 51,
            PI_CMD_WVTXR = 52,
            PI_CMD_WVNEW = 53,

            PI_CMD_I2CO = 54,
            PI_CMD_I2CC = 55,
            PI_CMD_I2CRD = 56,
            PI_CMD_I2CWD = 57,
            PI_CMD_I2CWQ = 58,
            PI_CMD_I2CRS = 59,
            PI_CMD_I2CWS = 60,
            PI_CMD_I2CRB = 61,
            PI_CMD_I2CWB = 62,
            PI_CMD_I2CRW = 63,
            PI_CMD_I2CWW = 64,
            PI_CMD_I2CRK = 65,
            PI_CMD_I2CWK = 66,
            PI_CMD_I2CRI = 67,
            PI_CMD_I2CWI = 68,
            PI_CMD_I2CPC = 69,
            PI_CMD_I2CPK = 70,

            PI_CMD_SPIO = 71,
            PI_CMD_SPIC = 72,
            PI_CMD_SPIR = 73,
            PI_CMD_SPIW = 74,
            PI_CMD_SPIX = 75,

            PI_CMD_SERO = 76,
            PI_CMD_SERC = 77,
            PI_CMD_SERRB = 78,
            PI_CMD_SERWB = 79,
            PI_CMD_SERR = 80,
            PI_CMD_SERW = 81,
            PI_CMD_SERDA = 82,

            PI_CMD_GDC = 83,
            PI_CMD_GPW = 84,

            PI_CMD_HC = 85,
            PI_CMD_HP = 86,

            PI_CMD_CF1 = 87,
            PI_CMD_CF2 = 88,

            PI_CMD_BI2CC = 89,
            PI_CMD_BI2CO = 90,
            PI_CMD_BI2CZ = 91,

            PI_CMD_I2CZ = 92,

            PI_CMD_WVCHA = 93,

            PI_CMD_SLRI = 94,

            PI_CMD_CGI = 95,
            PI_CMD_CSI = 96,

            PI_CMD_FG = 97,
            PI_CMD_FN = 98,

            PI_CMD_NOIB = 99,

            PI_CMD_WVTXM = 100,
            PI_CMD_WVTAT = 101,

            PI_CMD_PADS = 102,
            PI_CMD_PADG = 103,

            PI_CMD_FO = 104,
            PI_CMD_FC = 105,
            PI_CMD_FR = 106,
            PI_CMD_FW = 107,
            PI_CMD_FS = 108,
            PI_CMD_FL = 109,

            PI_CMD_SHELL = 110,

            PI_CMD_BSPIC = 111,
            PI_CMD_BSPIO = 112,
            PI_CMD_BSPIX = 113,

            PI_CMD_BSCX = 114,

            PI_CMD_EVM = 115,
            PI_CMD_EVT = 116,

            PI_CMD_PROCU = 117,
            PI_CMD_WVCAP = 118,
        };

    } // namespace communication

} // namespace pigpio_remote

#endif // PIGPIO_REMOTE_SRC_COMMUNICATION_COMMAND_H