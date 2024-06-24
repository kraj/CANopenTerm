/** @file command.c
 *
 *  A versatile software tool to analyse and configure CANopen devices.
 *
 *  Copyright (c) 2024, Michael Fitzmayer. All rights reserved.
 *  SPDX-License-Identifier: MIT
 *
 **/

#include <stdio.h>
#include "SDL.h"
#include "can.h"
#include "core.h"
#include "command.h"
#include "nmt_client.h"
#include "pdo.h"
#include "printf.h"
#include "scripts.h"
#include "sdo_client.h"
#include "table.h"

#ifdef _WIN32
#  define CLEAR_CMD "cls"
#elif defined(unix) || defined(__unix__) || defined(__unix)
#  define CLEAR_CMD "clear"
#else
#  define CLEAR_CMD ""
#endif

static void     convert_token_to_uint(char* token, Uint32* result);
static void     convert_token_to_uint64(char* token, Uint64* result);
static void     print_usage_information(SDL_bool show_all);
static SDL_bool is_numeric(const char* str);

void parse_command(char* input, core_t* core)
{
    int    index;
    char*  delim        = " \n";
    char*  context;
    char*  token        = NULL;
    char*  input_savptr = input;
    Uint32 command;

    token = SDL_strtokr(input_savptr, delim, &input_savptr);

    if (NULL == token)
    {
        return;
    }
    else if ((input[1] != ' ') && (input[1] != '\0'))
    {
        run_script(token, core);
        return;
    }
#ifndef __linux__
    else if (0 == SDL_strncmp(token, "b", 1))
    {
        token = SDL_strtokr(input_savptr, delim, &input_savptr);
        if (NULL == token)
        {
            can_print_baud_rate_help(core);
            return;
        }

        convert_token_to_uint(token, &command);

        if (command > 13)
        {
            can_print_baud_rate_help(core);
            return;
        }

        can_set_baud_rate(command, core);
        return;
    }
#endif
    else if (0 == SDL_strncmp(token, "c", 1))
    {
        if (0 != system(CLEAR_CMD))
        {
            c_log(LOG_WARNING, "Could not clear screen");
        }
    }
    else if (0 == SDL_strncmp(token, "q", 1))
    {
        core->is_running = SDL_FALSE;
    }
    else if (0 == SDL_strncmp(token, "h", 1))
    {
        print_usage_information(SDL_TRUE);
    }
    else if (0 == SDL_strncmp(token, "n", 1))
    {
        Uint32 node_id;
        Uint32 command;
        size_t token_len;

        token = SDL_strtokr(input_savptr, delim, &input_savptr);
        if (NULL == token)
        {
            print_usage_information(SDL_FALSE);
            return;
        }

        convert_token_to_uint(token, &node_id);

        token = SDL_strtokr(input_savptr, delim, &input_savptr);
        if (NULL == token)
        {
            nmt_print_help(TERM_OUTPUT);
            return;
        }

        token_len = SDL_strlen(token);

        if (0 == SDL_strncmp(token, "op", token_len))
        {
            command = NMT_OPERATIONAL;
        }
        else if (0 == SDL_strncmp(token, "stop", token_len))
        {
            command = NMT_STOP;
        }
        else if (0 == SDL_strncmp(token, "preop", token_len))
        {
            command = NMT_PRE_OPERATIONAL;
        }
        else if (0 == SDL_strncmp(token, "reset", token_len))
        {
            command = NMT_RESET_NODE;
        }
        else
        {
            convert_token_to_uint(token, &command);
        }

        nmt_send_command((Uint16)node_id, (Uint8)command, TERM_OUTPUT, NULL);
    }
    else if (0 == SDL_strncmp(token, "l", 1))
    {
        list_scripts();
    }
    else if (0 == SDL_strncmp(token, "p", 1))
    {
        Uint32 can_id;

        token = SDL_strtokr(input_savptr, delim, &input_savptr);
        if (NULL == token)
        {
            print_usage_information(SDL_FALSE);
            return;
        }
        else if (0 == SDL_strncmp(token, "add", 3))
        {
            Uint32 event_time_ms;
            Uint32 length;
            Uint64 data;

            token = SDL_strtokr(input_savptr, delim, &input_savptr);
            if (NULL == token)
            {
                print_usage_information(SDL_FALSE);
                return;
            }

            convert_token_to_uint(token, &can_id);

            token = SDL_strtokr(input_savptr, delim, &input_savptr);
            if (NULL == token)
            {
                print_usage_information(SDL_FALSE);
                return;
            }

            convert_token_to_uint(token, &event_time_ms);

            token = SDL_strtokr(input_savptr, delim, &input_savptr);
            if (NULL == token)
            {
                print_usage_information(SDL_FALSE);
                return;
            }

            convert_token_to_uint(token, &length);

            token = SDL_strtokr(input_savptr, delim, &input_savptr);
            if (NULL == token)
            {
                print_usage_information(SDL_FALSE);
                return;
            }

            convert_token_to_uint64(token, &data);

            if (SDL_FALSE == pdo_is_id_valid(can_id))
            {
                pdo_print_help();
                return;
            }

            if (SDL_FALSE == is_can_initialised(core))
            {
                c_log(LOG_WARNING, "Could not add PDO: CAN not initialised");
                return;
            }

            pdo_add((Uint16)can_id, event_time_ms, length, data, TERM_OUTPUT);
        }
        else if (0 == SDL_strncmp(token, "del", 3))
        {
            token = SDL_strtokr(input_savptr, delim, &input_savptr);
            if (NULL == token)
            {
                print_usage_information(SDL_FALSE);
                return;
            }

            convert_token_to_uint(token, &can_id);

            if (SDL_FALSE == pdo_is_id_valid(can_id))
            {
                pdo_print_help();
                return;
            }

            if (SDL_FALSE == is_can_initialised(core))
            {
                c_log(LOG_WARNING, "Could not delete PDO: CAN not initialised");
                return;
            }

            pdo_del((Uint16)can_id, TERM_OUTPUT);
        }
        else
        {
            print_usage_information(SDL_FALSE);
            return;
        }
    }
    else if (0 == SDL_strncmp(token, "r", 1))
    {
        can_message_t sdo_response = { 0 };
        Uint32        node_id;
        Uint32        sdo_index;
        Uint32        sub_index    = 0;

        token = SDL_strtokr(input_savptr, delim, &input_savptr);
        if (NULL == token)
        {
            print_usage_information(SDL_FALSE);
            return;
        }

        convert_token_to_uint(token, &node_id);

        token = SDL_strtokr(input_savptr, delim, &input_savptr);
        if (NULL == token)
        {
            print_usage_information(SDL_FALSE);
            return;
        }

        convert_token_to_uint(token, &sdo_index);

        token = SDL_strtokr(input_savptr, delim, &input_savptr);
        if (NULL != token)
        {
            convert_token_to_uint(token, &sub_index);
        }

        sdo_read(&sdo_response, TERM_OUTPUT, node_id, sdo_index, sub_index, NULL);
    }
    else if (0 == SDL_strncmp(token, "w", 1))
    {
        can_message_t sdo_response    = { 0 };
        Uint32        node_id;
        Uint32        sdo_index;
        Uint32        sub_index;
        Uint32        sdo_data_length = 0;
        Uint32        sdo_data        = 0;
        sdo_state_t   sdo_state       = IS_WRITE_EXPEDITED;

        token = SDL_strtokr(input_savptr, delim, &input_savptr);
        if (token == NULL)
        {
            print_usage_information(SDL_FALSE);
            return;
        }
        convert_token_to_uint(token, &node_id);

        token = SDL_strtokr(input_savptr, delim, &input_savptr);
        if (token == NULL)
        {
            print_usage_information(SDL_FALSE);
            return;
        }
        convert_token_to_uint(token, &sdo_index);

        token = SDL_strtokr(input_savptr, delim, &input_savptr);
        if (token == NULL)
        {
            print_usage_information(SDL_FALSE);
            return;
        }
        convert_token_to_uint(token, &sub_index);

        token = SDL_strtokr(input_savptr, delim, &input_savptr);
        if (token != NULL)
        {
            char buffer[256] = { 0 };

            if (is_numeric(token))
            {
                convert_token_to_uint(token, &sdo_data_length);

                token = SDL_strtokr(input_savptr, delim, &input_savptr);
                if (token != NULL)
                {
                    convert_token_to_uint(token, &sdo_data);
                }

                SDL_memcpy(buffer, &sdo_data, sizeof(Uint32));
            }
            else
            {
                size_t len;

                SDL_strlcpy(buffer, token, sizeof(buffer));
                len = SDL_strlen(buffer);
                sdo_state = IS_WRITE_SEGMENTED;
                token = SDL_strtokr(NULL, delim, &input_savptr);

                while (token != NULL)
                {
                    SDL_strlcat(buffer, " ", sizeof(buffer));
                    SDL_strlcat(buffer, token, sizeof(buffer));
                    len = SDL_strlen(buffer);
                    token = SDL_strtokr(NULL, delim, &input_savptr);
                }

                if (buffer[0] == '"' && buffer[len - 1] == '"')
                {
                    buffer[len - 1] = '\0';
                    SDL_memmove(buffer, buffer + 1, len - 1);
                }

                sdo_data_length = SDL_strlen(buffer);
            }

            if (sdo_data_length > 0)
            {
                if (IS_WRITE_EXPEDITED == sdo_state)
                {
                    sdo_write(&sdo_response, TERM_OUTPUT, node_id, sdo_index, sub_index, sdo_data_length, (void*)buffer, NULL);
                }
                else
                {
                    sdo_write_segmented(&sdo_response, TERM_OUTPUT, node_id, sdo_index, sub_index, sdo_data_length, (void*)buffer, NULL);
                }
            }
            else
            {
                print_usage_information(SDL_FALSE);
                return;
            }
        }
        else
        {
            print_usage_information(SDL_FALSE);
            return;
        }
    }
    else if (0 == SDL_strncmp(token, "s", 1))
    {
        token = SDL_strtokr(input_savptr, delim, &input_savptr);
        if (NULL == token)
        {
            print_usage_information(SDL_FALSE);
            return;
        }
        run_script(token, core);
    }
    else
    {
        print_usage_information(SDL_FALSE);
    }
}


static void convert_token_to_uint(char* token, Uint32* result)
{
    if (('0' == token[0]) && ('x' == token[1]))
    {
        *result = (Uint32)SDL_strtol(token, NULL, 16);
    }
    else
    {
        *result = (Uint32)SDL_strtol(token, NULL, 10);
    }
}

static void convert_token_to_uint64(char* token, Uint64* result)
{
    if (('0' == token[0]) && ('x' == token[1]))
    {
        *result = SDL_strtoull(token, NULL, 16);
    }
    else
    {
        *result = SDL_strtoull(token, NULL, 10);
    }
}

static void print_usage_information(SDL_bool show_all)
{
    table_t table = { DARK_CYAN, DARK_WHITE, 3, 45, 14 };

    table_print_header(&table);
    table_print_row("CMD", "Parameter(s)",                                  "Function",     &table);
    table_print_divider(&table);
    table_print_row(" h ", " ",                                             "Show full help", &table);

    if (SDL_TRUE == show_all)
    {
#ifndef __linux__
        table_print_row(" b ", "(command)",                                 "Set baud rate",  &table);
#endif
        table_print_row(" c ", " ",                                         "Clear output", &table);
        table_print_row(" l ", " ",                                         "List scripts", &table);
        table_print_row("(s)", "[script_identifier](.lua)",                 "Run script",   &table);
    }

    table_print_row(" n ", "[node_id] [command or alias]",                  "NMT command", &table);
    table_print_row(" r ", "[node_id] [index] (sub_index)",                 "Read SDO",    &table);
    table_print_row(" w ", "[node_id] [index] [sub_index] [length] (data)", "Write SDO",   &table);
    table_print_row(" w ", "[node_id] [index] [sub_index] [\"data\"]",      "Write SDO",   &table);
    table_print_row(" p ", "add [can_id] [event_time_ms] [length] [data]",  "Add TPDO",    &table);
    table_print_row(" p ", "del [can_id]",                                  "Remove TPDO", &table);
    table_print_row(" q ", " ",                                             "Quit",        &table);
    table_print_footer(&table);
}

static SDL_bool is_numeric(const char* str)
{
    if ((NULL == str) || ('\0' == *str))
    {
        return SDL_FALSE;
    }

    while (*str)
    {
        if (!isdigit(*str))
        {
            return SDL_FALSE;
        }
        str += 1;
    }

    return SDL_TRUE;
}
