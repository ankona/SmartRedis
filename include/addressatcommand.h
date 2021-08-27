/*
 * BSD 2-Clause License
 *
 * Copyright (c) 2021, Hewlett Packard Enterprise
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef SMARTREDIS_ADDRESSATCOMMAND_H
#define SMARTREDIS_ADDRESSATCOMMAND_H

#include "nonkeyedcommand.h"

///@file

namespace SmartRedis {

class RedisServer;

/*!
*   \brief The AddressAtCommand class constructs non-keyed Client
*          commands that address the user given db node(s).
*          This is a subclass of the NonKeyedCommand class.
*/
class AddressAtCommand : public NonKeyedCommand
{
    public:
        /*!
        *   \brief Run this Command on the RedisServer.
        *   \param server A pointer to the RedisServer
        */
        virtual CommandReply run_me(RedisServer* server);

        /*!
        *   \brief Deep copy operator
        *   \details This method creates a new derived
        *            type Command and returns a Command*
        *            pointer.  The new derived type is
        *            allocated on the heap.  Contents
        *            are copied using the copy assignment
        *            operator for the derived type. This is meant
        *            to provide functionality to deep
        *            copy a Command.
        *   \returns A pointer to dynamically allocated
        *            derived type cast to parent Command
        *            type.
        */
        virtual Command* clone();

        /*!
        *   \brief Returns host of database node
        *   \param address The address of the database node
        *   \returns The host of the database node
        */
        inline std::string parse_host(std::string address)
        {
            return address.substr(0, address.find(":"));
        }

        /*!
        *   \brief Returns port of database node
        *   \param address The address of the database node
        *   \returns The port of the database node
        */
        inline uint64_t parse_port(std::string address)
        {
            return std::stoul(address.substr(address.find(":") + 1),
                              nullptr, 0);;
        }

};

} //namespace SmartRedis

#endif //ADDRESSATCOMMAND