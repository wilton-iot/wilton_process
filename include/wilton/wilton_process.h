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
        int await_exit,
        int* pid_out);

#ifdef __cplusplus
}
#endif

#endif /* WILTON_PROCESS_H */

