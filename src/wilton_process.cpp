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
#include "wilton/support/logging.hpp"

namespace { // anonymous

const std::string LOGGER = std::string("wilton.process");

} // namespace

char* wilton_process_spawn(const char* executable, int executable_len,
        const char* args_list_json, int args_list_json_len,
        const char* output_file, int output_file_len,
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
    if (nullptr == pid_out) return wilton::support::alloc_copy(TRACEMSG("Null 'pid_out' parameter specified"));
    try {
        auto executable_str = std::string(executable, static_cast<uint16_t>(executable_len));
        auto args = std::vector<std::string>();
        auto args_json = sl::json::load({args_list_json, args_list_json_len});
        for (auto& el : args_json.as_array_or_throw("args_list_json")) {
            auto& st = el.as_string_nonempty_or_throw("args_list_json");
            args.push_back(st);
        }
        auto outfile = std::string(output_file, static_cast<uint16_t>(output_file_len));
        wilton::support::log_debug(LOGGER, "Spawning process,"
                " executable: [" + executable_str + "]," +
                " args: [" + std::string(args_list_json, args_list_json_len) + "]" +
                " output_file: [" + outfile +" ]" +
                " await exit: [" + sl::support::to_string_bool(0 != await_exit) + "] ...");
        // call utils
        int pid = 0;
        if (0 != await_exit) {
            pid = sl::utils::exec_and_wait(executable, args, outfile);
        } else {
            pid = sl::utils::exec_async(executable, args, outfile);
        }
        wilton::support::log_debug(LOGGER, "Process spawn complete,result: [" + sl::support::to_string(pid) +"]");
        *pid_out = pid;
        return nullptr;
    } catch (const std::exception& e) {
        return wilton::support::alloc_copy(TRACEMSG(e.what() + "\nException raised"));
    }
}


