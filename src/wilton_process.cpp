/*
 * Copyright 2017, alex at staticlibs.net
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/* 
 * File:   wilton_process.cpp
 * Author: alex
 *
 * Created on October 17, 2017, 10:03 PM
 */

#include "wilton/wilton_process.h"

#include <cstdint>
#include <string>
#include <vector>

#include "staticlib/config.hpp"
#include "staticlib/utils.hpp"
#include "staticlib/json.hpp"

#include "wilton/support/alloc.hpp"
#include "wilton/support/buffer.hpp"
#include "wilton/support/logging.hpp"

namespace { // anonymous

const std::string logger = std::string("wilton.process");

} // namespace

char* wilton_process_spawn(const char* executable, int executable_len,
        const char* args_list_json, int args_list_json_len,
        const char* output_file, int output_file_len,
        const char* directory, int directory_len,
        int await_exit, int* pid_out) /* noexcept */ {
    if (nullptr == executable) return wilton::support::alloc_copy(TRACEMSG("Null 'executable' parameter specified"));
    if (!sl::support::is_uint16_positive(executable_len)) return wilton::support::alloc_copy(TRACEMSG(
            "Invalid 'executable_len' parameter specified: [" + sl::support::to_string(executable_len) + "]"));
    if (nullptr == args_list_json) return wilton::support::alloc_copy(TRACEMSG("Null 'args_list_json' parameter specified"));
    if (!sl::support::is_uint16_positive(args_list_json_len)) return wilton::support::alloc_copy(TRACEMSG(
            "Invalid 'args_list_json_len' parameter specified: [" + sl::support::to_string(args_list_json_len) + "]"));
    if (nullptr == output_file) return wilton::support::alloc_copy(TRACEMSG("Null 'output_file' parameter specified"));
    if (!sl::support::is_uint16_positive(output_file_len)) return wilton::support::alloc_copy(TRACEMSG(
            "Invalid 'output_file_len' parameter specified: [" + sl::support::to_string(output_file_len) + "]"));
    if (nullptr == directory) return wilton::support::alloc_copy(TRACEMSG("Null 'directory' parameter specified"));
    if (!sl::support::is_uint16(directory_len)) return wilton::support::alloc_copy(TRACEMSG(
            "Invalid 'directory_len' parameter specified: [" + sl::support::to_string(directory_len) + "]"));
    if (nullptr == pid_out) return wilton::support::alloc_copy(TRACEMSG("Null 'pid_out' parameter specified"));
    try {
        auto executable_str = std::string(executable, static_cast<uint16_t>(executable_len));
        auto args = std::vector<std::string>();
        auto args_json = sl::json::load({args_list_json, args_list_json_len});
        for (auto& el : args_json.as_array_or_throw("args_list_json")) {
            auto& st = el.as_string_or_throw("args_list_json");
            args.push_back(st);
        }
        auto outfile = std::string(output_file, static_cast<uint16_t>(output_file_len));
        auto dir = std::string(directory, static_cast<uint16_t>(directory_len));
        wilton::support::log_debug(logger, "Spawning process,"
                " executable: [" + executable_str + "]," +
                " args: [" + std::string(args_list_json, args_list_json_len) + "]" +
                " output_file: [" + outfile + "]" +
                " directory: [" + dir + "]" +
                " await exit: [" + sl::support::to_string_bool(0 != await_exit) + "] ...");
        // call utils
        int pid = 0;
        if (0 != await_exit) {
            pid = sl::utils::exec_and_wait(executable_str, args, outfile, dir);
        } else {
            pid = sl::utils::exec_async(executable_str, args, outfile, dir);
        }
        wilton::support::log_debug(logger, "Process spawn complete,result: [" + sl::support::to_string(pid) +"]");
        *pid_out = pid;
        return nullptr;
    } catch (const std::exception& e) {
        return wilton::support::alloc_copy(TRACEMSG(e.what() + "\nException raised"));
    }
}

char* wilton_process_spawn_shell(const char* command, int command_len, int* code_out) /* noexcept */ {
    if (nullptr == command) return wilton::support::alloc_copy(TRACEMSG("Null 'command' parameter specified"));
    if (!sl::support::is_uint16_positive(command_len)) return wilton::support::alloc_copy(TRACEMSG(
            "Invalid 'command_len' parameter specified: [" + sl::support::to_string(command_len) + "]"));
    if (nullptr == code_out) return wilton::support::alloc_copy(TRACEMSG("Null 'code_out' parameter specified"));
    try {
        auto command_str = std::string(command, static_cast<int>(command_len));
        wilton::support::log_debug(logger, "Spawning shell process, command: [" + command_str + "] ...");
        int code = sl::utils::shell_exec_and_wait(command_str);
        wilton::support::log_debug(logger, "Shell spawn complete,result: [" + sl::support::to_string(code) +"]");
        *code_out = code;
        return nullptr;
    } catch (const std::exception& e) {
        return wilton::support::alloc_copy(TRACEMSG(e.what() + "\nException raised"));
    }
}

char* wilton_process_current_pid(int* pid_out) /* noexcept */ {
    if (nullptr == pid_out) return wilton::support::alloc_copy(TRACEMSG("Null 'pid_out' parameter specified"));
    try {
        wilton::support::log_debug(logger, "Obtaining PID of the current process ...");
        int pid = sl::utils::current_process_pid();
        wilton::support::log_debug(logger, "PID obtained, value: [" + sl::support::to_string(pid) +"]");
        *pid_out = pid;
        return nullptr;
    } catch (const std::exception& e) {
        return wilton::support::alloc_copy(TRACEMSG(e.what() + "\nException raised"));
    }
}

char* wilton_process_kill_by_pid(
        int pid,
        char** msg_out,
        int* msg_len_out) /* noexcept */ {
    if (!sl::support::is_uint32_positive(pid)) return wilton::support::alloc_copy(TRACEMSG(
            "Invalid 'pid' parameter specified: [" + sl::support::to_string(pid) + "]"));
    if (nullptr == msg_out) return wilton::support::alloc_copy(TRACEMSG("Null 'msg_out' parameter specified"));
    if (nullptr == msg_len_out) return wilton::support::alloc_copy(TRACEMSG("Null 'msg_len_out' parameter specified"));
    try {
        wilton::support::log_debug(logger, "Is due to kill process, pid: [" + sl::support::to_string(pid) + "] ...");
        auto err = sl::utils::kill_process(pid);
        wilton::support::log_debug(logger, "Process kill performed, result: [" + err + "]");
        auto buf = wilton::support::make_string_buffer(err);
        *msg_out = buf.data();
        *msg_len_out = buf.size_int();
        return nullptr;
    } catch (const std::exception& e) {
        return wilton::support::alloc_copy(TRACEMSG(e.what() + "\nException raised"));
    }
}
