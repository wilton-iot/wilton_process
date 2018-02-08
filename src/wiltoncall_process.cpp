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
 * File:   wiltoncall_process.cpp
 * Author: alex
 *
 * Created on October 17, 2017, 10:03 PM
 */

#include "wilton/wilton_process.h"

#include "staticlib/config.hpp"
#include "staticlib/json.hpp"
#include "staticlib/utils.hpp"

#include "wilton/support/buffer.hpp"
#include "wilton/support/exception.hpp"
#include "wilton/support/registrar.hpp"

namespace wilton {
namespace process {

support::buffer spawn(sl::io::span<const char> data) {
    // json parse
    auto json = sl::json::load(data);
    auto rexecutable = std::ref(sl::utils::empty_string());
    auto args = std::string();
    auto routfile = std::ref(sl::utils::empty_string());
    bool await_exit = false;
    for (const sl::json::field& fi : json.as_object()) {
        auto& name = fi.name();
        if ("executable" == name) {
            rexecutable = fi.as_string_nonempty_or_throw(name);
        } else if ("args" == name) {
            args = fi.val().dumps();
        } else if ("outputFile" == name) {
            routfile = fi.as_string_nonempty_or_throw(name);
        } else if ("awaitExit" == name) {
            await_exit = fi.as_bool_or_throw(name);
        } else {
            throw support::exception(TRACEMSG("Unknown data field: [" + name + "]"));
        }
    }
    if (rexecutable.get().empty()) throw support::exception(TRACEMSG(
            "Required parameter 'executable' not specified"));
    if (args.empty()) throw support::exception(TRACEMSG(
            "Required parameter 'args' not specified"));
    if (routfile.get().empty()) throw support::exception(TRACEMSG(
            "Required parameter 'outFile' not specified"));    
    const std::string& executable = rexecutable.get();
    const std::string& outfile = routfile.get();
    // call wilton
    int pid = -1;
    char* err = wilton_process_spawn(executable.c_str(), static_cast<int>(executable.length()),
            args.c_str(), static_cast<int>(args.length()),
            outfile.c_str(), static_cast<int>(outfile.length()),
            await_exit ? 1 : 0,
            std::addressof(pid));
    if (nullptr != err) {
        support::throw_wilton_error(err, TRACEMSG(err));
    }
    return support::make_json_buffer(sl::json::value(pid));
}

} // namespace
}

extern "C" char* wilton_module_init() {
    try {
        wilton::support::register_wiltoncall("process_spawn", wilton::process::spawn);
        return nullptr;
    } catch (const std::exception& e) {
        return wilton::support::alloc_copy(TRACEMSG(e.what() + "\nException raised"));
    }
}
