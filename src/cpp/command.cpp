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

#include "command.h"

using namespace SmartRedis;

// Command copy constructor
Command::Command(const Command& cmd)
{
    *this = cmd;
}

// Command copy assignment operator
Command& Command::operator=(const Command& cmd)
{
    // Check for self-assignment
    if (this == &cmd)
        return *this; // we have met the enemy and he is us

    make_empty();

    this->_fields.resize(cmd._fields.size());

    // copy pointer fields and put into _fields
    this->_ptr_fields = cmd._ptr_fields;
    std::vector<std::pair<char*, size_t> >::const_iterator ptr_it =
        this->_ptr_fields.begin();
    for (; ptr_it != this->_ptr_fields.end(); ptr_it++)
        this->_fields[ptr_it->second] = ptr_it->first;

    // copy _local_fields and _cmd_keys, and put the fields into _fields
    std::vector<std::pair<char*, size_t> >::const_iterator local_it =
        cmd._local_fields.begin();
    for (; local_it != cmd._local_fields.end(); local_it++) {
        // allocate memory and copy a local field
        int field_size = cmd._fields[local_it->second].size();
        char* f = (char*)std::malloc(field_size);
        if (f == NULL)
            throw std::bad_alloc();
        std::memcpy(f, local_it->first, field_size);
        this->_local_fields.push_back(
            std::pair<char*, size_t>(f, local_it->second));
        this->_fields[local_it->second] = std::string_view(f, field_size);

        if (cmd._cmd_keys.count(cmd._fields[local_it->second]) != 0) {
            // copy a command key
            this->_cmd_keys[std::string_view(f, field_size)] =
                local_it->second;
        }
    }

    return *this;
}

// Command destructor
Command::~Command()
{
    make_empty();
}

// Add a field to the Command from a string.
void Command::add_field(std::string field, bool is_key)
{
    /* Copy the field string into a char* that is stored
    locally in the Command object.  The new string is not
    null terminated because the fields vector is of type
    string_view which stores the length of the string.
    If is_key is true, the key will be added to the command
    keys.
    */

    int field_size = field.size();
    char* f = (char*)std::malloc(field_size + 1);
    if (f == NULL)
        throw std::bad_alloc();
    field.copy(f, field_size, 0);
    f[field_size] = '\0';
    this->_local_fields.push_back({f, _fields.size()});
    this->_fields.push_back(std::string_view(f, field_size));

    if (is_key) {
        this->_cmd_keys[std::string_view(f, field_size)] =
            this->_fields.size() - 1;
    }
}

// Add a field to the Command from a c-string.
void Command::add_field(const char* field, bool is_key)
{
    /* Copy the field char* into a char* that is stored
    locally in the Command object.  The new string is not
    null terminated because the fields vector is of type
    string_view which stores the length of the string.
    If is_key is true, the key will be added to the command
    keys.
    */

    int field_size = std::strlen(field);
    char* f = (char*)std::malloc(field_size);
    if (f == NULL)
        throw std::bad_alloc();
    std::memcpy(f, field, field_size);
    this->_local_fields.push_back({f, _fields.size()});
    this->_fields.push_back(std::string_view(f, field_size));

    if (is_key) {
        this->_cmd_keys[std::string_view(f, field_size)] =
            this->_fields.size()-1;
    }
}

// Add a field to the Command from a c-string without copying the data.
void Command::add_field_ptr(char* field, size_t field_size)
{
    /* This function adds a field to the fields data
    structure without copying the data.  This means
    that the memory needs to be valid when it is later
    accessed.  This function should be used for very large
    fields.  Field pointers cannot act as Command keys.
    */
    this->_ptr_fields.push_back({field, _fields.size()});
    this->_fields.push_back(std::string_view(field, field_size));
}

// Add a field to the Command from a std::string_view without copying the data
void Command::add_field_ptr(std::string_view field)
{
    /* This function adds a field to the fields data
    structure without copying the data.  This means
    that the memory needs to be valid when it is later
    accessed.  This function should be used for very large
    fields.  If is_key is true, the key will be added to the
    command keys.  Field pointers cannot act as Command keys.
    */
    this->_ptr_fields.push_back({(char*)field.data(), _fields.size()});
    this->_fields.push_back(field);
}

// Add fields to the Command from a vector of strings.
void Command::add_fields(const std::vector<std::string>& fields, bool is_key)
{
    /* Copy field strings into a char* that is stored
    locally in the Command object.  The new string is not
    null terminated because the fields vector is of type
    string_view which stores the length of the string
    */
    for (int i = 0; i < fields.size(); i++) {
        this->add_field(fields[i], is_key);
    }
}

// Get the value of the field field
std::string Command::first_field()
{
    if (this->begin() == this->end())
        throw std::runtime_error("No fields exist in the Command.");
    return std::string(this->begin()->data(), this->begin()->size());
}

// Get a string of the entire Command
std::string Command::to_string()
{
    std::string output;
    for (Command::iterator it = this->begin(); it != this->end(); it++) {
        output += " " + std::string(it->data(), it->size());
    }
    return output;
}

// Returns an iterator pointing to the first field in the Command
Command::iterator Command::begin()
{
    return this->_fields.begin();
}

// Returns a const iterator pointing to the first field in the Command
Command::const_iterator Command::cbegin()
{
    return this->_fields.cbegin();
}

// Returns an iterator pointing to the past-the-end field in the Command
Command::iterator Command::end()
{
    return this->_fields.end();
}

// Returns a const iterator pointing to the past-the-end field in the Command
Command::const_iterator Command::cend()
{
    return this->_fields.cend();
}

// Return true if the Command has keys
bool Command::has_keys()
{
    return (this->_cmd_keys.size()>0);
}

// Return a copy of all Command keys
std::vector<std::string> Command::get_keys() {
    /* This function returns a vector of key copies
    to the user.  Original keys are not returned
    to the user because memory management
    becomes complicated for std::string_view that
    may need to grow or decrease in size.
    */
    std::vector<std::string> keys;
    std::unordered_map<std::string_view,size_t>::iterator it =
        this->_cmd_keys.begin();
    for ( ; it != this->_cmd_keys.end(); it++) {
        keys.push_back(std::string(it->first.data(), it->first.length()));
    }
    return keys;
}

// Helper function for emptying the Command
void Command::make_empty()
{
    std::vector<std::pair<char*, size_t> >::iterator it =
        this->_local_fields.begin();

    for ( ; it != this->_local_fields.end(); it++) {
        if (it->first != NULL)
            free(it->first);
        *it = {NULL, 0};
    }
    this->_local_fields.clear();
    this->_ptr_fields.clear();
    this->_cmd_keys.clear();
    this->_fields.clear();
}

// Set address and port for command to be executed on
void Command::set_exec_address_port(std::string address,
                                    uint16_t port)
{
    this->_address = address;
    this->_port = port;
}

// Get address that command will be to be executed on
std::string Command::get_address()
{
    return this->_address;
}

// Get port that command will be to be executed on
uint16_t Command::get_port()
{
    return this->_port;
}

// EOF