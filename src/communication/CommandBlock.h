#ifndef PIGPIO_REMOTE_SRC_COMMUNICATION_COMMANDBLOCK_H
#define PIGPIO_REMOTE_SRC_COMMUNICATION_COMMANDBLOCK_H

namespace pigpio_remote
{
   namespace communication
   {

      struct CommandBlock
      {
         uint32_t Cmd;
         uint32_t P1;
         uint32_t P2;
         union
         {
            uint32_t P3;
            uint32_t ExtLen;
            int32_t Res;
         };
      };

   } // namespace communication

} // namespace pigpio_remote

#endif // PIGPIO_REMOTE_SRC_COMMUNICATION_COMMANDBLOCK_H