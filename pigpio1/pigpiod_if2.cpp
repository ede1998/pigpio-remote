/*
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org/>
*/

/* PIGPIOD_IF2_VERSION 17 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <netif/ethernet.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>

#include "pigpio.h"

extern "C"
{
#include "command.h"
}

#include <string>
#include <vector>

#include "../NoNagleSyncClient.h"

#include "pigpiod_if2.h"

#define PI_MAX_REPORTS_PER_READ 4096

#define STACK_SIZE (256*1024)

#define MAX_PI 32

/* GLOBALS ---------------------------------------------------------------- */

std::array<bool, MAX_PI> gPiInUse{false};

static int             gPigCommand  [MAX_PI];
static int             gPigHandle   [MAX_PI];
static int             gPigNotify   [MAX_PI];

static uint32_t        gLastLevel   [MAX_PI];

static pthread_t       *gPthNotify  [MAX_PI];

/* PRIVATE ---------------------------------------------------------------- */

static void _pml(int pi) {}
static void _pmu(int pi) {}


static int pigpio_command(int pi, int command, int p1, int p2, int rl)
{
   cmdCmd_t cmd;

   if ((pi < 0) || (pi >= MAX_PI) || !gPiInUse[pi])
      return pigif_unconnected_pi;

   cmd.cmd = command;
   cmd.p1  = p1;
   cmd.p2  = p2;
   cmd.res = 0;

   // _pml(pi);

   // if (send(gPigCommand[pi], &cmd, sizeof(cmd), 0) != sizeof(cmd))
   // {
   //    _pmu(pi);
   //    return pigif_bad_send;
   // }

   // if (recv(gPigCommand[pi], &cmd, sizeof(cmd), MSG_WAITALL) != sizeof(cmd))
   // {
   //    _pmu(pi);
   //    return pigif_bad_recv;
   // }

   // if (rl) _pmu(pi);

   return cmd.res;
}

static int pigpio_notify(int pi)
{
   cmdCmd_t cmd;

   if ((pi < 0) || (pi >= MAX_PI) || !gPiInUse[pi])
      return pigif_unconnected_pi;

   cmd.cmd = PI_CMD_NOIB;
   cmd.p1  = 0;
   cmd.p2  = 0;
   cmd.res = 0;

   _pml(pi);

   // if (send(gPigNotify[pi], &cmd, sizeof(cmd), 0) != sizeof(cmd))
   // {
   //    _pmu(pi);
   //    return pigif_bad_send;
   // }

   // if (recv(gPigNotify[pi], &cmd, sizeof(cmd), MSG_WAITALL) != sizeof(cmd))
   // {
   //    _pmu(pi);
   //    return pigif_bad_recv;
   // }

   _pmu(pi);

   return cmd.res;
}

static int pigpio_command_ext
   (int pi, int command, int p1, int p2, int p3,
    int extents, gpioExtent_t *ext, int rl)
{
   int i;
   cmdCmd_t cmd;

   if ((pi < 0) || (pi >= MAX_PI) || !gPiInUse[pi])
      return pigif_unconnected_pi;

   cmd.cmd = command;
   cmd.p1  = p1;
   cmd.p2  = p2;
   cmd.p3  = p3;

   _pml(pi);

   // if (send(gPigCommand[pi], &cmd, sizeof(cmd), 0) != sizeof(cmd))
   // {
   //    _pmu(pi);
   //    return pigif_bad_send;
   // }

   // for (i=0; i<extents; i++)
   // {
   //    if (send(gPigCommand[pi], ext[i].ptr, ext[i].size, 0) != ext[i].size)
   //    {
   //       _pmu(pi);
   //       return pigif_bad_send;
   //    }
   // }

   // if (recv(gPigCommand[pi], &cmd, sizeof(cmd), MSG_WAITALL) != sizeof(cmd))
   // {
   //    _pmu(pi);
   //    return pigif_bad_recv;
   // }
   if (rl) _pmu(pi);

   return cmd.res;
}

const std::string determine_parameter(const char *parameter, const char *fallback_env_var, const std::string &fallback_default)
{
   std::string ret(parameter == nullptr ? "" : parameter);

   if (!ret.empty())
   {
      return ret;
   }

   const char *env = getenv(fallback_env_var);
   ret = env == nullptr ? "" : env;

   if (!ret.empty())
   {
      return ret;
   }

   return fallback_default;
}

static NoNagleSyncClient pigpioOpenSocket(const char *addrChar, const char *portChar)
{
   NoNagleSyncClient client;
   std::string addrStr = determine_parameter(addrChar, PI_ENVADDR, PI_DEFAULT_SOCKET_ADDR_STR);
   std::string portStr = determine_parameter(portChar, PI_ENVPORT, PI_DEFAULT_SOCKET_PORT_STR);
   auto port = static_cast<uint16_t>(atoi(portStr.c_str()));
   auto result = client.connect(addrStr.c_str(), port);

   switch (result)
   {
   case 1: // SUCCESS
      client.update_last_connection_error_code(0);
      break;
   case -2: // INVALID_SERVER
      client.update_last_connection_error_code(pigif_bad_getaddrinfo);
      break;
   default:
      client.update_last_connection_error_code(pigif_bad_connect);
      break;
   }

   return client;
}

static int recvMax(int pi, void *buf, int bufsize, int sent)
{
   /*
   Copy at most bufSize bytes from the receieved message to
   buf.  Discard the rest of the message.
   */
   uint8_t scratch[4096];
   int remaining, fetch, count;

   if (sent < bufsize) count = sent; else count = bufsize;
   
   memset(buf, 0, bufsize);
   // if (count) recv(gPigCommand[pi], buf, count, MSG_WAITALL);

   remaining = sent - count;

   while (remaining)
   {
      fetch = remaining;
      if (fetch > sizeof(scratch)) fetch = sizeof(scratch);
      // recv(gPigCommand[pi], scratch, fetch, MSG_WAITALL);
      remaining -= fetch;
   }

   return count;
}

/* PUBLIC ----------------------------------------------------------------- */

double time_time(void)
{
   struct timeval tv;
   double t;

   gettimeofday(&tv, 0);

   t = (double)tv.tv_sec + ((double)tv.tv_usec / 1E6);

   return t;
}

void time_sleep(double seconds)
{
   usleep(seconds * 1E6);
}

const char *pigpio_error(int errnum)
{
   if (errnum > -1000) return cmdErrStr(errnum);
   else
   {
      switch(errnum)
      {
         case pigif_bad_send:
            return "failed to send to pigpiod";
         case pigif_bad_recv:
            return "failed to receive from pigpiod";
         case pigif_bad_getaddrinfo:
            return "failed to find address of pigpiod";
         case pigif_bad_connect:
            return "failed to connect to pigpiod";
         case pigif_bad_socket:
            return "failed to create socket";
         case pigif_bad_noib:
            return "failed to open notification in band";
         case pigif_duplicate_callback:
            return "identical callback exists";
         case pigif_bad_malloc:
            return "failed to malloc";
         case pigif_bad_callback:
            return "bad callback parameter";
         case pigif_notify_failed:
            return "failed to create notification thread";
         case pigif_callback_not_found:
            return "callback not found";
         case pigif_unconnected_pi:
            return "not connected to Pi";
         case pigif_too_many_pis:
            return "too many connected Pis";

         default:
            return "unknown error";
      }
   }
}

unsigned pigpiod_if_version(void)
{
   return PIGPIOD_IF2_VERSION;
}

int pigpio_start(char *addrStr, char *portStr)
{
   int pi;
   int *userdata;

   for (pi=0; pi<MAX_PI; pi++)
   {
      if (!gPiInUse[pi]) break;
   }

   if (pi >= MAX_PI) return pigif_too_many_pis;

   gPiInUse[pi] = true;

   gPigCommand[pi] = pigpioOpenSocket(addrStr, portStr);

   if (gPigCommand[pi] >= 0)
   {
      gPigNotify[pi] = pigpioOpenSocket(addrStr, portStr);

      if (gPigNotify[pi] >= 0)
      {
         gPigHandle[pi] = pigpio_notify(pi);

         if (gPigHandle[pi] < 0) return pigif_bad_noib;
         else
         {
            gLastLevel[pi] = read_bank_1(pi);

            /* must be freed by pthNotifyThread */
            userdata = (int *)malloc(sizeof(*userdata));
            *userdata = pi;

            gPthNotify[pi] = 0; //start_thread(pthNotifyThread, userdata);

            if (gPthNotify[pi]) return pi;
            else                return pigif_notify_failed;

         }
      }
      else return gPigNotify[pi];
   }
   else return gPigCommand[pi];
}

void pigpio_stop(int pi)
{
   if ((pi < 0) || (pi >= MAX_PI) || !gPiInUse[pi]) return;

   if (gPthNotify[pi])
   {
      stop_thread(gPthNotify[pi]);
      gPthNotify[pi] = 0;
   }

   if (gPigCommand[pi] >= 0)
   {
      if (gPigHandle[pi] >= 0)
      {
         pigpio_command(pi, PI_CMD_NC, gPigHandle[pi], 0, 1);
         gPigHandle[pi] = -1;
      }

      close(gPigCommand[pi]);
      gPigCommand[pi] = -1;
   }

   if (gPigNotify[pi] >= 0)
   {
      close(gPigNotify[pi]);
      gPigNotify[pi] = -1;
   }

   gPiInUse[pi] = 0;
}

int set_mode(int pi, unsigned gpio, unsigned mode)
   {return pigpio_command(pi, PI_CMD_MODES, gpio, mode, 1);}

int get_mode(int pi, unsigned gpio)
   {return pigpio_command(pi, PI_CMD_MODEG, gpio, 0, 1);}

int set_pull_up_down(int pi, unsigned gpio, unsigned pud)
   {return pigpio_command(pi, PI_CMD_PUD, gpio, pud, 1);}

int gpio_read(int pi, unsigned gpio)
   {return pigpio_command(pi, PI_CMD_READ, gpio, 0, 1);}

int gpio_write(int pi, unsigned gpio, unsigned level)
   {return pigpio_command(pi, PI_CMD_WRITE, gpio, level, 1);}

int set_PWM_dutycycle(int pi, unsigned user_gpio, unsigned dutycycle)
   {return pigpio_command(pi, PI_CMD_PWM, user_gpio, dutycycle, 1);}

int get_PWM_dutycycle(int pi, unsigned user_gpio)
   {return pigpio_command(pi, PI_CMD_GDC, user_gpio, 0, 1);}

int set_PWM_range(int pi, unsigned user_gpio, unsigned range)
   {return pigpio_command(pi, PI_CMD_PRS, user_gpio, range, 1);}

int get_PWM_range(int pi, unsigned user_gpio)
   {return pigpio_command(pi, PI_CMD_PRG, user_gpio, 0, 1);}

int get_PWM_real_range(int pi, unsigned user_gpio)
   {return pigpio_command(pi, PI_CMD_PRRG, user_gpio, 0, 1);}

int set_PWM_frequency(int pi, unsigned user_gpio, unsigned frequency)
   {return pigpio_command(pi, PI_CMD_PFS, user_gpio, frequency, 1);}

int get_PWM_frequency(int pi, unsigned user_gpio)
   {return pigpio_command(pi, PI_CMD_PFG, user_gpio, 0, 1);}

int set_servo_pulsewidth(int pi, unsigned user_gpio, unsigned pulsewidth)
   {return pigpio_command(pi, PI_CMD_SERVO, user_gpio, pulsewidth, 1);}

int get_servo_pulsewidth(int pi, unsigned user_gpio)
   {return pigpio_command(pi, PI_CMD_GPW, user_gpio, 0, 1);}

int notify_open(int pi)
   {return pigpio_command(pi, PI_CMD_NO, 0, 0, 1);}

int notify_begin(int pi, unsigned handle, uint32_t bits)
   {return pigpio_command(pi, PI_CMD_NB, handle, bits, 1);}

int notify_pause(int pi, unsigned handle)
   {return pigpio_command(pi, PI_CMD_NB, handle, 0, 1);}

int notify_close(int pi, unsigned handle)
   {return pigpio_command(pi, PI_CMD_NC, handle, 0, 1);}

int set_watchdog(int pi, unsigned user_gpio, unsigned timeout)
   {return pigpio_command(pi, PI_CMD_WDOG, user_gpio, timeout, 1);}

uint32_t read_bank_1(int pi)
   {return pigpio_command(pi, PI_CMD_BR1, 0, 0, 1);}

uint32_t read_bank_2(int pi)
   {return pigpio_command(pi, PI_CMD_BR2, 0, 0, 1);}

int clear_bank_1(int pi, uint32_t levels)
   {return pigpio_command(pi, PI_CMD_BC1, levels, 0, 1);}

int clear_bank_2(int pi, uint32_t levels)
   {return pigpio_command(pi, PI_CMD_BC2, levels, 0, 1);}

int set_bank_1(int pi, uint32_t levels)
   {return pigpio_command(pi, PI_CMD_BS1, levels, 0, 1);}

int set_bank_2(int pi, uint32_t levels)
   {return pigpio_command(pi, PI_CMD_BS2, levels, 0, 1);}

int hardware_clock(int pi, unsigned gpio, unsigned frequency)
   {return pigpio_command(pi, PI_CMD_HC, gpio, frequency, 1);}

int hardware_PWM(int pi, unsigned gpio, unsigned frequency, uint32_t dutycycle)
{
   gpioExtent_t ext[1];
   
   /*
   p1=gpio
   p2=frequency
   p3=4
   ## extension ##
   uint32_t dutycycle
   */

   ext[0].size = sizeof(dutycycle);
   ext[0].ptr = &dutycycle;

   return pigpio_command_ext(
      pi, PI_CMD_HP, gpio, frequency, sizeof(dutycycle), 1, ext, 1);
}

uint32_t get_current_tick(int pi)
   {return pigpio_command(pi, PI_CMD_TICK, 0, 0, 1);}

uint32_t get_hardware_revision(int pi)
   {return pigpio_command(pi, PI_CMD_HWVER, 0, 0, 1);}

uint32_t get_pigpio_version(int pi)
   {return pigpio_command(pi, PI_CMD_PIGPV, 0, 0, 1);}

int wave_clear(int pi)
   {return pigpio_command(pi, PI_CMD_WVCLR, 0, 0, 1);}

int wave_add_new(int pi)
   {return pigpio_command(pi, PI_CMD_WVNEW, 0, 0, 1);}

int wave_add_generic(int pi, unsigned numPulses, gpioPulse_t *pulses)
{
   gpioExtent_t ext[1];
   
   /*
   p1=0
   p2=0
   p3=pulses*sizeof(gpioPulse_t)
   ## extension ##
   gpioPulse_t[] pulses
   */

   if (!numPulses) return 0;

   ext[0].size = numPulses * sizeof(gpioPulse_t);
   ext[0].ptr = pulses;

   return pigpio_command_ext(
      pi, PI_CMD_WVAG, 0, 0, ext[0].size, 1, ext, 1);
}

int wave_add_serial(
   int pi, unsigned user_gpio, unsigned baud, uint32_t databits,
   uint32_t stophalfbits, uint32_t offset,  unsigned numChar, char *str)
{
   uint8_t buf[12];
   gpioExtent_t ext[2];

   /*
   p1=user_gpio
   p2=baud
   p3=len+12
   ## extension ##
   uint32_t databits
   uint32_t stophalfbits
   uint32_t offset
   char[len] str
   */

   if (!numChar) return 0;

   memcpy(buf, &databits, 4);
   memcpy(buf+4, &stophalfbits, 4);
   memcpy(buf+8, &offset, 4);

   ext[0].size = sizeof(buf);
   ext[0].ptr = buf;

   ext[1].size = numChar;
   ext[1].ptr = str;

   return pigpio_command_ext(pi, PI_CMD_WVAS,
      user_gpio, baud, numChar+sizeof(buf), 2, ext, 1);
}

int wave_create(int pi)
   {return pigpio_command(pi, PI_CMD_WVCRE, 0, 0, 1);}

int wave_create_and_pad(int pi, int percent)
   {return pigpio_command(pi, PI_CMD_WVCAP, percent, 0, 1);}

int wave_delete(int pi, unsigned wave_id)
   {return pigpio_command(pi, PI_CMD_WVDEL, wave_id, 0, 1);}

int wave_tx_start(int pi) /* DEPRECATED */
   {return pigpio_command(pi, PI_CMD_WVGO, 0, 0, 1);}

int wave_tx_repeat(int pi) /* DEPRECATED */
   {return pigpio_command(pi, PI_CMD_WVGOR, 0, 0, 1);}

int wave_send_once(int pi, unsigned wave_id)
   {return pigpio_command(pi, PI_CMD_WVTX, wave_id, 0, 1);}

int wave_send_repeat(int pi, unsigned wave_id)
   {return pigpio_command(pi, PI_CMD_WVTXR, wave_id, 0, 1);}

int wave_send_using_mode(int pi, unsigned wave_id, unsigned mode)
   {return pigpio_command(pi, PI_CMD_WVTXM, wave_id, mode, 1);}

int wave_chain(int pi, char *buf, unsigned bufSize)
{
   gpioExtent_t ext[1];

   /*
   p1=0
   p2=0
   p3=bufSize
   ## extension ##
   char buf[bufSize]
   */

   ext[0].size = bufSize;
   ext[0].ptr = buf;

   return pigpio_command_ext
      (pi, PI_CMD_WVCHA, 0, 0, bufSize, 1, ext, 1);
}

int wave_tx_at(int pi)
   {return pigpio_command(pi, PI_CMD_WVTAT, 0, 0, 1);}

int wave_tx_busy(int pi)
   {return pigpio_command(pi, PI_CMD_WVBSY, 0, 0, 1);}

int wave_tx_stop(int pi)
   {return pigpio_command(pi, PI_CMD_WVHLT, 0, 0, 1);}

int wave_get_micros(int pi)
   {return pigpio_command(pi, PI_CMD_WVSM, 0, 0, 1);}

int wave_get_high_micros(int pi)
   {return pigpio_command(pi, PI_CMD_WVSM, 1, 0, 1);}

int wave_get_max_micros(int pi)
   {return pigpio_command(pi, PI_CMD_WVSM, 2, 0, 1);}

int wave_get_pulses(int pi)
   {return pigpio_command(pi, PI_CMD_WVSP, 0, 0, 1);}

int wave_get_high_pulses(int pi)
   {return pigpio_command(pi, PI_CMD_WVSP, 1, 0, 1);}

int wave_get_max_pulses(int pi)
   {return pigpio_command(pi, PI_CMD_WVSP, 2, 0, 1);}

int wave_get_cbs(int pi)
   {return pigpio_command(pi, PI_CMD_WVSC, 0, 0, 1);}

int wave_get_high_cbs(int pi)
   {return pigpio_command(pi, PI_CMD_WVSC, 1, 0, 1);}

int wave_get_max_cbs(int pi)
   {return pigpio_command(pi, PI_CMD_WVSC, 2, 0, 1);}

int gpio_trigger(int pi, unsigned user_gpio, unsigned pulseLen, uint32_t level)
{
   gpioExtent_t ext[1];
   
   /*
   p1=user_gpio
   p2=pulseLen 
   p3=4
   ## extension ##
   unsigned level
   */

   ext[0].size = sizeof(uint32_t);
   ext[0].ptr = &level;

   return pigpio_command_ext(
      pi, PI_CMD_TRIG, user_gpio, pulseLen, 4, 1, ext, 1);
}

int set_glitch_filter(int pi, unsigned user_gpio, unsigned steady)
   {return pigpio_command(pi, PI_CMD_FG, user_gpio, steady, 1);}

int set_noise_filter(int pi, unsigned user_gpio, unsigned steady, unsigned active)
{
   gpioExtent_t ext[1];
   
   /*
   p1=user_gpio
   p2=steady
   p3=4
   ## extension ##
   unsigned active
   */

   ext[0].size = sizeof(uint32_t);
   ext[0].ptr = &active;

   return pigpio_command_ext(
      pi, PI_CMD_FN, user_gpio, steady, 4, 1, ext, 1);
}

int store_script(int pi, char *script)
{
   unsigned len;
   gpioExtent_t ext[1];

   /*
   p1=0
   p2=0
   p3=len
   ## extension ##
   char[len] script
   */

   len = strlen(script);

   if (!len) return 0;

   ext[0].size = len;
   ext[0].ptr = script;

   return pigpio_command_ext(pi, PI_CMD_PROC, 0, 0, len, 1, ext, 1);
}

int run_script(int pi, unsigned script_id, unsigned numPar, uint32_t *param)
{
   gpioExtent_t ext[1];

   /*
   p1=script id
   p2=0
   p3=numPar * 4
   ## extension ##
   uint32_t[numPar] pars
   */

   ext[0].size = 4 * numPar;
   ext[0].ptr = param;

   return pigpio_command_ext
      (pi, PI_CMD_PROCR, script_id, 0, numPar*4, 1, ext, 1);
}

int update_script(int pi, unsigned script_id, unsigned numPar, uint32_t *param)
{
   gpioExtent_t ext[1];

   /*
   p1=script id
   p2=0
   p3=numPar * 4
   ## extension ##
   uint32_t[numPar] pars
   */

   ext[0].size = 4 * numPar;
   ext[0].ptr = param;

   return pigpio_command_ext
      (pi, PI_CMD_PROCU, script_id, 0, numPar*4, 1, ext, 1);
}

int script_status(int pi, unsigned script_id, uint32_t *param)
{
   int status;
   uint32_t p[PI_MAX_SCRIPT_PARAMS+1]; /* space for script status */

   status = pigpio_command(pi, PI_CMD_PROCP, script_id, 0, 0);

   if (status > 0)
   {
      recvMax(pi, p, sizeof(p), status);
      status = p[0];
      if (param) memcpy(param, p+1, sizeof(p)-4);
   }

   _pmu(pi);

   return status;
}

int stop_script(int pi, unsigned script_id)
   {return pigpio_command(pi, PI_CMD_PROCS, script_id, 0, 1);}

int delete_script(int pi, unsigned script_id)
   {return pigpio_command(pi, PI_CMD_PROCD, script_id, 0, 1);}

int bb_serial_read_open(int pi, unsigned user_gpio, unsigned baud, uint32_t bbBits)
{
   gpioExtent_t ext[1];
   
   /*
   p1=user_gpio
   p2=baud
   p3=4
   ## extension ##
   unsigned bbBits
   */

   ext[0].size = sizeof(uint32_t);
   ext[0].ptr = &bbBits;

   return pigpio_command_ext(
      pi, PI_CMD_SLRO, user_gpio, baud, 4, 1, ext, 1);
}

int bb_serial_read(int pi, unsigned user_gpio, void *buf, size_t bufSize)
{
   int bytes;

   bytes = pigpio_command(pi, PI_CMD_SLR, user_gpio, bufSize, 0);

   if (bytes > 0)
   {
      bytes = recvMax(pi, buf, bufSize, bytes);
   }

   _pmu(pi);

   return bytes;
}

int bb_serial_read_close(int pi, unsigned user_gpio)
   {return pigpio_command(pi, PI_CMD_SLRC, user_gpio, 0, 1);}

int bb_serial_invert(int pi, unsigned user_gpio, unsigned invert)
   {return pigpio_command(pi, PI_CMD_SLRI, user_gpio, invert, 1);}

int i2c_open(int pi, unsigned i2c_bus, unsigned i2c_addr, uint32_t i2c_flags)
{
   gpioExtent_t ext[1];

   /*
   p1=i2c_bus
   p2=i2c_addr
   p3=4
   ## extension ##
   uint32_t i2c_flags
   */

   ext[0].size = sizeof(uint32_t);
   ext[0].ptr = &i2c_flags;

   return pigpio_command_ext
      (pi, PI_CMD_I2CO, i2c_bus, i2c_addr, 4, 1, ext, 1);
}

int i2c_close(int pi, unsigned handle)
   {return pigpio_command(pi, PI_CMD_I2CC, handle, 0, 1);}

int i2c_write_quick(int pi, unsigned handle, unsigned bit)
   {return pigpio_command(pi, PI_CMD_I2CWQ, handle, bit, 1);}

int i2c_write_byte(int pi, unsigned handle, unsigned val)
   {return pigpio_command(pi, PI_CMD_I2CWS, handle, val, 1);}

int i2c_read_byte(int pi, unsigned handle)
   {return pigpio_command(pi, PI_CMD_I2CRS, handle, 0, 1);}

int i2c_write_byte_data(int pi, unsigned handle, unsigned reg, uint32_t val)
{
   gpioExtent_t ext[1];

   /*
   p1=handle
   p2=reg
   p3=4
   ## extension ##
   uint32_t val
   */

   ext[0].size = sizeof(uint32_t);
   ext[0].ptr = &val;

   return pigpio_command_ext
      (pi, PI_CMD_I2CWB, handle, reg, 4, 1, ext, 1);
}

int i2c_write_word_data(int pi, unsigned handle, unsigned reg, uint32_t val)
{
   gpioExtent_t ext[1];

   /*
   p1=handle
   p2=reg
   p3=4
   ## extension ##
   uint32_t val
   */

   ext[0].size = sizeof(uint32_t);
   ext[0].ptr = &val;

   return pigpio_command_ext
      (pi, PI_CMD_I2CWW, handle, reg, 4, 1, ext, 1);
}

int i2c_read_byte_data(int pi, unsigned handle, unsigned reg)
   {return pigpio_command(pi, PI_CMD_I2CRB, handle, reg, 1);}

int i2c_read_word_data(int pi, unsigned handle, unsigned reg)
   {return pigpio_command(pi, PI_CMD_I2CRW, handle, reg, 1);}

int i2c_process_call(int pi, unsigned handle, unsigned reg, uint32_t val)
{
   gpioExtent_t ext[1];

   /*
   p1=handle
   p2=reg
   p3=4
   ## extension ##
   uint32_t val
   */

   ext[0].size = sizeof(uint32_t);
   ext[0].ptr = &val;

   return pigpio_command_ext
      (pi, PI_CMD_I2CPC, handle, reg, 4, 1, ext, 1);
}

int i2c_write_block_data(
   int pi, unsigned handle, unsigned reg, char *buf, unsigned count)
{
   gpioExtent_t ext[1];

   /*
   p1=handle
   p2=reg
   p3=count
   ## extension ##
   char buf[count]
   */

   ext[0].size = count;
   ext[0].ptr = buf;

   return pigpio_command_ext
      (pi, PI_CMD_I2CWK, handle, reg, count, 1, ext, 1);
}

int i2c_read_block_data(int pi, unsigned handle, unsigned reg, char *buf)
{
   int bytes;

   bytes = pigpio_command(pi, PI_CMD_I2CRK, handle, reg, 0);

   if (bytes > 0)
   {
      bytes = recvMax(pi, buf, 32, bytes);
   }

   _pmu(pi);

   return bytes;
}

int i2c_block_process_call(
   int pi, unsigned handle, unsigned reg, char *buf, unsigned count)
{
   int bytes;
   gpioExtent_t ext[1];

   /*
   p1=handle
   p2=reg
   p3=count
   ## extension ##
   char buf[count]
   */

   ext[0].size = count;
   ext[0].ptr = buf;

   bytes = pigpio_command_ext
      (pi, PI_CMD_I2CPK, handle, reg, count, 1, ext, 0);

   if (bytes > 0)
   {
      bytes = recvMax(pi, buf, 32, bytes);
   }

   _pmu(pi);

   return bytes;
}

int i2c_read_i2c_block_data(
   int pi, unsigned handle, unsigned reg, char *buf, uint32_t count)
{
   int bytes;
   gpioExtent_t ext[1];

   /*
   p1=handle
   p2=reg
   p3=4
   ## extension ##
   uint32_t count
   */

   ext[0].size = sizeof(uint32_t);
   ext[0].ptr = &count;

   bytes = pigpio_command_ext
      (pi, PI_CMD_I2CRI, handle, reg, 4, 1, ext, 0);

   if (bytes > 0)
   {
      bytes = recvMax(pi, buf, count, bytes);
   }

   _pmu(pi);

   return bytes;
}


int i2c_write_i2c_block_data(
   int pi, unsigned handle, unsigned reg, char *buf, unsigned count)
{
   gpioExtent_t ext[1];

   /*
   p1=handle
   p2=reg
   p3=count
   ## extension ##
   char buf[count]
   */

   ext[0].size = count;
   ext[0].ptr = buf;

   return pigpio_command_ext
      (pi, PI_CMD_I2CWI, handle, reg, count, 1, ext, 1);
}

int i2c_read_device(int pi, unsigned handle, char *buf, unsigned count)
{
   int bytes;

   bytes = pigpio_command(pi, PI_CMD_I2CRD, handle, count, 0);

   if (bytes > 0)
   {
      bytes = recvMax(pi, buf, count, bytes);
   }

   _pmu(pi);

   return bytes;
}

int i2c_write_device(int pi, unsigned handle, char *buf, unsigned count)
{
   gpioExtent_t ext[1];

   /*
   p1=handle
   p2=0
   p3=count
   ## extension ##
   char buf[count]
   */

   ext[0].size = count;
   ext[0].ptr = buf;

   return pigpio_command_ext
      (pi, PI_CMD_I2CWD, handle, 0, count, 1, ext, 1);
}

int i2c_zip(
   int pi,
   unsigned handle,
   char    *inBuf,
   unsigned inLen,
   char    *outBuf,
   unsigned outLen)
{
   int bytes;
   gpioExtent_t ext[1];

   /*
   p1=handle
   p2=0
   p3=inLen
   ## extension ##
   char inBuf[inLen]
   */

   ext[0].size = inLen;
   ext[0].ptr = inBuf;

   bytes = pigpio_command_ext
      (pi, PI_CMD_I2CZ, handle, 0, inLen, 1, ext, 0);

   if (bytes > 0)
   {
      bytes = recvMax(pi, outBuf, outLen, bytes);
   }

   _pmu(pi);

   return bytes;
}

int bb_i2c_open(int pi, unsigned SDA, unsigned SCL, unsigned baud)
{
   gpioExtent_t ext[1];

   /*
   p1=SDA
   p2=SCL
   p3=4
   ## extension ##
   uint32_t baud
   */

   ext[0].size = sizeof(uint32_t);
   ext[0].ptr = &baud;

   return pigpio_command_ext
      (pi, PI_CMD_BI2CO, SDA, SCL, 4, 1, ext, 1);
}

int bb_i2c_close(int pi, unsigned SDA)
   {return pigpio_command(pi, PI_CMD_BI2CC, SDA, 0, 1);}

int bb_i2c_zip(
   int      pi,
   unsigned SDA,
   char    *inBuf,
   unsigned inLen,
   char    *outBuf,
   unsigned outLen)
{
   int bytes;
   gpioExtent_t ext[1];

   /*
   p1=SDA
   p2=0
   p3=inLen
   ## extension ##
   char inBuf[inLen]
   */

   ext[0].size = inLen;
   ext[0].ptr = inBuf;

   bytes = pigpio_command_ext
      (pi, PI_CMD_BI2CZ, SDA, 0, inLen, 1, ext, 0);

   if (bytes > 0)
   {
      bytes = recvMax(pi, outBuf, outLen, bytes);
   }

   _pmu(pi);

   return bytes;
}

int bb_spi_open(
   int pi,
   unsigned CS, unsigned MISO, unsigned MOSI, unsigned SCLK,
   unsigned baud, unsigned spiFlags)
{
   uint8_t buf[20];
   gpioExtent_t ext[1];

   /*
   p1=CS
   p2=0
   p3=20
   ## extension ##
   uint32_t MISO
   uint32_t MOSI
   uint32_t SCLK
   uint32_t baud
   uint32_t spiFlags
   */

   ext[0].size = 20;
   ext[0].ptr = &buf;

   memcpy(buf +  0, &MISO, 4);
   memcpy(buf +  4, &MOSI, 4);
   memcpy(buf +  8, &SCLK, 4);
   memcpy(buf + 12, &baud, 4);
   memcpy(buf + 16, &spiFlags, 4);

   return pigpio_command_ext
      (pi, PI_CMD_BSPIO, CS, 0, 20, 1, ext, 1);
}

int bb_spi_close(int pi, unsigned CS)
   {return pigpio_command(pi, PI_CMD_BSPIC, CS, 0, 1);}

int bb_spi_xfer(
   int pi,
   unsigned CS,
   char    *txBuf,
   char    *rxBuf,
   unsigned count)
{
   int bytes;
   gpioExtent_t ext[1];

   /*
   p1=CS
   p2=0
   p3=count
   ## extension ##
   char txBuf[count]
   */

   ext[0].size = count;
   ext[0].ptr = txBuf;

   bytes = pigpio_command_ext
      (pi, PI_CMD_BSPIX, CS, 0, count, 1, ext, 0);

   if (bytes > 0)
   {
      bytes = recvMax(pi, rxBuf, count, bytes);
   }

   _pmu(pi);

   return bytes;
}

int spi_open(int pi, unsigned channel, unsigned speed, uint32_t flags)
{
   gpioExtent_t ext[1];

   /*
   p1=channel
   p2=speed
   p3=4
   ## extension ##
   uint32_t flags
   */

   ext[0].size = sizeof(uint32_t);
   ext[0].ptr = &flags;

   return pigpio_command_ext
      (pi, PI_CMD_SPIO, channel, speed, 4, 1, ext, 1);
}

int spi_close(int pi, unsigned handle)
   {return pigpio_command(pi, PI_CMD_SPIC, handle, 0, 1);}

int spi_read(int pi, unsigned handle, char *buf, unsigned count)
{
   int bytes;

   bytes = pigpio_command
      (pi, PI_CMD_SPIR, handle, count, 0);

   if (bytes > 0)
   {
      bytes = recvMax(pi, buf, count, bytes);
   }

   _pmu(pi);

   return bytes;
}

int spi_write(int pi, unsigned handle, char *buf, unsigned count)
{
   gpioExtent_t ext[1];

   /*
   p1=handle
   p2=0
   p3=count
   ## extension ##
   char buf[count]
   */

   ext[0].size = count;
   ext[0].ptr = buf;

   return pigpio_command_ext
      (pi, PI_CMD_SPIW, handle, 0, count, 1, ext, 1);
}

int spi_xfer(int pi, unsigned handle, char *txBuf, char *rxBuf, unsigned count)
{
   int bytes;
   gpioExtent_t ext[1];

   /*
   p1=handle
   p2=0
   p3=count
   ## extension ##
   char buf[count]
   */

   ext[0].size = count;
   ext[0].ptr = txBuf;

   bytes = pigpio_command_ext
      (pi, PI_CMD_SPIX, handle, 0, count, 1, ext, 0);

   if (bytes > 0)
   {
      bytes = recvMax(pi, rxBuf, count, bytes);
   }

   _pmu(pi);

   return bytes;
}

int serial_open(int pi, char *dev, unsigned baud, unsigned flags)
{
   int len;
   gpioExtent_t ext[1];

   len = strlen(dev);

   /*
   p1=baud
   p2=flags
   p3=len
   ## extension ##
   char dev[len]
   */

   ext[0].size = len;
   ext[0].ptr = dev;

   return pigpio_command_ext
      (pi, PI_CMD_SERO, baud, flags, len, 1, ext, 1);
}

int serial_close(int pi, unsigned handle)
   {return pigpio_command(pi, PI_CMD_SERC, handle, 0, 1);}

int serial_write_byte(int pi, unsigned handle, unsigned val)
   {return pigpio_command(pi, PI_CMD_SERWB, handle, val, 1);}

int serial_read_byte(int pi, unsigned handle)
   {return pigpio_command(pi, PI_CMD_SERRB, handle, 0, 1);}

int serial_write(int pi, unsigned handle, char *buf, unsigned count)
{
   gpioExtent_t ext[1];

   /*
   p1=handle
   p2=0
   p3=count
   ## extension ##
   char buf[count]
   */

   ext[0].size = count;
   ext[0].ptr = buf;

   return pigpio_command_ext
      (pi, PI_CMD_SERW, handle, 0, count, 1, ext, 1);
}

int serial_read(int pi, unsigned handle, char *buf, unsigned count)
{
   int bytes;

   bytes = pigpio_command
      (pi, PI_CMD_SERR, handle, count, 0);

   if (bytes > 0)
   {
      bytes = recvMax(pi, buf, count, bytes);
   }

   _pmu(pi);

   return bytes;
}

int serial_data_available(int pi, unsigned handle)
   {return pigpio_command(pi, PI_CMD_SERDA, handle, 0, 1);}

int custom_1(int pi, unsigned arg1, unsigned arg2, char *argx, unsigned count)
{
   gpioExtent_t ext[1];

   /*
   p1=arg1
   p2=arg2
   p3=count
   ## extension ##
   char argx[count]
   */

   ext[0].size = count;
   ext[0].ptr = argx;

   return pigpio_command_ext(
      pi, PI_CMD_CF1, arg1, arg2, count, 1, ext, 1);
}


int custom_2(int pi, unsigned arg1, char *argx, unsigned count,
             char *retBuf, uint32_t retMax)
{
   int bytes;
   gpioExtent_t ext[1];

   /*
   p1=arg1
   p2=retMax
   p3=count
   ## extension ##
   char argx[count]
   */

   ext[0].size = count;
   ext[0].ptr = argx;

   bytes = pigpio_command_ext
      (pi, PI_CMD_CF2, arg1, retMax, count, 1, ext, 0);

   if (bytes > 0)
   {
      bytes = recvMax(pi, retBuf, retMax, bytes);
   }

   _pmu(pi);

   return bytes;
}

int get_pad_strength(int pi, unsigned pad)
   {return pigpio_command(pi, PI_CMD_PADG, pad, 0, 1);}

int set_pad_strength(int pi, unsigned pad, unsigned padStrength)
   {return pigpio_command(pi, PI_CMD_PADS, pad, padStrength, 1);}

int shell_(int pi, char *scriptName, char *scriptString)
{
   int ln, ls;
   gpioExtent_t ext[2];

   ln = strlen(scriptName);
   ls = strlen(scriptString);
   /*
   p1=len(scriptName)
   p2=0
   p3=len(scriptName) + len(scriptString) + 1
   ## extension ##
   char[]
   */

   ext[0].size = ln + 1; /* include null byte */
   ext[0].ptr = scriptName;

   ext[1].size = ls;
   ext[1].ptr = scriptString;

   return pigpio_command_ext
      (pi, PI_CMD_SHELL, ln, 0, ln+ls+1, 2, ext, 1);
}

int file_open(int pi, char *file, unsigned mode)
{
   int len;
   gpioExtent_t ext[1];

   len = strlen(file);

   /*
   p1=mode
   p2=0
   p3=len
   ## extension ##
   char file[len]
   */

   ext[0].size = len;
   ext[0].ptr = file;

   return pigpio_command_ext
      (pi, PI_CMD_FO, mode, 0, len, 1, ext, 1);
}

int file_close(int pi, unsigned handle)
   {return pigpio_command(pi, PI_CMD_FC, handle, 0, 1);}

int file_write(int pi, unsigned handle, char *buf, unsigned count)
{
   gpioExtent_t ext[1];

   /*
   p1=handle
   p2=0
   p3=count
   ## extension ##
   char buf[count]
   */

   ext[0].size = count;
   ext[0].ptr = buf;

   return pigpio_command_ext
      (pi, PI_CMD_FW, handle, 0, count, 1, ext, 1);
}

int file_read(int pi, unsigned handle, char *buf, unsigned count)
{
   int bytes;

   bytes = pigpio_command
      (pi, PI_CMD_FR, handle, count, 0);

   if (bytes > 0)
   {
      bytes = recvMax(pi, buf, count, bytes);
   }

   _pmu(pi);

   return bytes;
}

int file_seek(int pi, unsigned handle, int32_t seekOffset, int seekFrom)
{
   gpioExtent_t ext[1];

   /*
   p1=handle
   p2=seekOffset
   p3=4
   ## extension ##
   uint32_t seekFrom
   */

   ext[0].size = sizeof(uint32_t);
   ext[0].ptr = &seekFrom;

   return pigpio_command_ext
      (pi, PI_CMD_FS, handle, seekOffset, 4, 1, ext, 1);
}

int file_list(int pi, char *fpat,  char *buf, unsigned count)
{
   int len;
   int bytes;
   gpioExtent_t ext[1];

   len = strlen(fpat);

   /*
   p1=60000
   p2=0
   p3=len
   ## extension ##
   char fpat[len]
   */

   ext[0].size = len;
   ext[0].ptr = fpat;

   bytes = pigpio_command_ext(pi, PI_CMD_FL, 60000, 0, len, 1, ext, 0);

   if (bytes > 0)
   {
      bytes = recvMax(pi, buf, count, bytes);
   }

   _pmu(pi);

   return bytes;
}

int bsc_xfer(int pi, bsc_xfer_t *bscxfer)
{
   int bytes;
   int status;
   gpioExtent_t ext[1];

   /*
   p1=control
   p2=0
   p3=len
   ## extension ##
   char buf[len]
   */

   ext[0].size = bscxfer->txCnt;
   ext[0].ptr = bscxfer->txBuf;

   bytes = pigpio_command_ext
      (pi, PI_CMD_BSCX, bscxfer->control, 0, bscxfer->txCnt, 1, ext, 0);

   if (bytes > 0)
   {
      recvMax(pi, &status, 4, 4);
      status = ntohl(status);
      bytes -= 4;
      bytes = recvMax(pi, bscxfer->rxBuf, sizeof(bscxfer->rxBuf), bytes);
      bscxfer->rxCnt = bytes;
   }
   else
   {
      status = bytes;
   }

   _pmu(pi);

   return status;
}


int bsc_i2c(int pi, int i2c_addr, bsc_xfer_t *bscxfer)
{
   int control = 0;

   if (i2c_addr) control = (i2c_addr<<16) | 0x305;
   bscxfer->control = control;
   return bsc_xfer(pi, bscxfer);
}


int event_trigger(int pi, unsigned event)
   {return pigpio_command(pi, PI_CMD_EVM, event, 0, 1);}

