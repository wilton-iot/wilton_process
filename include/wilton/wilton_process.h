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
 * File:   wilton_process.h
 * Author: alex
 *
 * Created on October 17, 2017, 8:57 PM
 */

#ifndef WILTON_PROCESS_H
#define WILTON_PROCESS_H

#include "wilton/wilton.h"

#ifdef __cplusplus
extern "C" {
#endif

char* wilton_process_spawn(
        const char* executable,
        int executable_len,
        const char* args_list_json,
        int args_list_json_len,
        const char* output_file,
        int output_file_len,
        const char* directory,
        int directory_len,
        int await_exit,
        int* pid_out);

char* wilton_process_spawn_shell(
        const char* command,
        int command_len,
        int* code_out);

char* wilton_process_current_pid(
        int* pid_out);

char* wilton_process_kill_by_pid(
        int pid,
        char** msg_out,
        int* msg_len_out);

#ifdef __cplusplus
}
#endif

#endif /* WILTON_PROCESS_H */

