#ifndef __SENDCOMMANDRESULT_H__
#define __SENDCOMMANDRESULT_H__

#include "../include/pigpio-remote/PigpioError.h"

namespace pigpio_remote
{
    namespace internal
    {

        struct CommandResult
        {
            int Result;
            PigpioError Error;

            static CommandResult Create(int result)
            {
                CommandResult res;
                res.Error = PigpioError::PI_OK;
                res.Result = result;
                return res;
            }

            static CommandResult Create(PigpioError error)
            {
                CommandResult res;
                res.Error = error;
                return res;
            }
        };

        template <typename ResultT, typename ErrorT>
        PigpioResult<ResultT, ErrorT> MakeResult(CommandResult result)
        {
            if (result.Error == PigpioError::PI_OK)
            {
                return PigpioResult<ResultT, ErrorT>(static_cast<ResultT>(result.Result));
            }

            return PigpioResult<ResultT, ErrorT>(result.Error);
        }

    } // namespace internal

} // namespace pigpio_remote

#endif // __SENDCOMMANDRESULT_H__
