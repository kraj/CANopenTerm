/** @file dbc.c
 *
 *  A versatile software tool to analyse and configure CANopen devices.
 *
 *  Copyright (c) 2024, Michael Fitzmayer. All rights reserved.
 *  SPDX-License-Identifier: MIT
 *
 **/

#include "core.h"
#include "dbc.h"
#include "lauxlib.h"
#include "lua.h"
#include "os.h"

static uint64 extract_raw_signal(uint64 can_frame, uint8 start_bit, uint8 length, endian_t endianness);
static void   parse_message_line(char *line, message_t *message);
static void   parse_signal_line(char *line, signal_t *signal);
static bool_t starts_with(const char *str, const char *prefix);
static char*  trim_whitespace(char *str);

const char *dbc_decode(dbc_t *dbc, uint32 can_id, uint64 data)
{
    static char result[4096] = { 0 };
    int         pos          = 0;
    int         i;

    if (NULL == dbc)
    {
        return "";
    }

    for (i = 0; i < dbc->message_count; ++i)
    {
        if (dbc->messages[i].id == can_id)
        {
            message_t* msg = &dbc->messages[i];
            int        n   = os_snprintf(result + pos, sizeof(result) - pos, "%s\n", msg->name);
            int        j;

            if (n > 0)
            {
                pos += n;
            }

            for (j = 0; j < msg->signal_count; ++j)
            {
                signal_t* signal    = &msg->signals[j];
                uint64    raw_value = extract_raw_signal(data, signal->start_bit, signal->length, signal->endianness);
                double    value     = (raw_value * signal->scale) + signal->offset;
                int       k;

                n = os_snprintf(result + pos, sizeof(result) - pos, "  %s", signal->name);
                if (n > 0)
                {
                    pos += n;
                }

                for (k = 0; k <= 35 - os_strlen(signal->name); ++k)
                {
                    n = os_snprintf(result + pos, sizeof(result) - pos, " ");
                    if (n > 0)
                    {
                        pos += n;
                    }
                }

                n = os_snprintf(result + pos, sizeof(result) - pos, ": %f %s\n", value, signal->unit);
                if (n > 0)
                {
                    pos += n;
                }
            }
            result[sizeof(result) - 1] = '\0';
            break;
        }
    }

    if (0 == pos)
    {
        return "";
    }

    return result;
}


void dbc_deinit(dbc_t* dbc)
{
    int i, j;

    if (NULL == dbc)
    {
        return;
    }

    for (i = 0; i < dbc->message_count; ++i)
    {
        message_t* msg = &dbc->messages[i];

        if (msg->name)
        {
            os_free(msg->name);
            msg->name = NULL; // Set to NULL after freeing
        }

        if (msg->transmitter)
        {
            os_free(msg->transmitter);
            msg->transmitter = NULL; // Set to NULL after freeing
        }

        for (j = 0; j < msg->signal_count; ++j)
        {
            signal_t* sig = &msg->signals[j];

            if (sig->name)
            {
                os_free(sig->name);
                sig->name = NULL; // Set to NULL after freeing
            }
            if (sig->unit)
            {
                os_free(sig->unit);
                sig->unit = NULL;
            }
            if (sig->receiver)
            {
                os_free(sig->receiver);
                sig->receiver = NULL;
            }
        }

        if (msg->signals)
        {
            os_free(msg->signals);
            msg->signals = NULL;
        }
    }

    if (dbc->messages)
    {
        os_free(dbc->messages);
        dbc->messages = NULL;
    }

    dbc->message_count = 0;
}

status_t dbc_init(dbc_t **dbc)
{
    *dbc = os_calloc(1, sizeof(dbc_t));
    if (NULL == *dbc)
    {
        return OS_MEMORY_ALLOCATION_ERROR;
    }
    else
    {
        return ALL_OK;
    }
}

status_t dbc_load(dbc_t* dbc, const char *filename)
{
    char       line[1024]      = { 0 };
    FILE_t*    file            = os_fopen(filename, "r");
    message_t* current_message = NULL;

    if (NULL == file)
    {
        return OS_FILE_NOT_FOUND;
    }

    if (NULL == dbc)
    {
        os_fclose(file);
        return OS_INVALID_ARGUMENT;
    }

    while (os_fgets(line, sizeof(line), file) != NULL)
    {
        char *trimmed_line = trim_whitespace(line);

        if (starts_with(trimmed_line, "BO_ "))
        {
            message_t* temp = os_realloc(dbc->messages, sizeof(message_t) * (dbc->message_count + 1));
            if (NULL == temp)
            {
                os_fclose(file);
                return OS_MEMORY_ALLOCATION_ERROR;
            }

            dbc->messages   = temp;
            current_message = &dbc->messages[dbc->message_count];
            os_memset(current_message, 0, sizeof(message_t));

            if (current_message != NULL)
            {
                dbc->message_count += 1;
                parse_message_line(trimmed_line, current_message);
            }
        }
        else if (starts_with(trimmed_line, "SG_ "))
        {
            if (current_message != NULL)
            {
                signal_t* current_signal;
                current_message->signals = realloc(current_message->signals, sizeof(signal_t) * (current_message->signal_count + 1));
                current_message->signal_count += 1;
                current_signal = &current_message->signals[current_message->signal_count - 1];
                parse_signal_line(trimmed_line, current_signal);
            }
        }
    }

    os_fclose(file);
    return ALL_OK;
}

void dbc_print(const dbc_t *dbc)
{
    int i, j;

    if (NULL == dbc)
    {
        return;
    }

    os_printf("DBC File contains %d messages\n", dbc->message_count);
    for (i = 0; i < dbc->message_count; ++i)
    {
        const message_t *msg = &dbc->messages[i];
        os_printf("Message %d: ID=%u, Name=%s, DLC=%u, Transmitter=%s\n",
            i + 1, msg->id, msg->name, msg->dlc, msg->transmitter);
        os_printf("  Contains %d signals\n", msg->signal_count);
        for (j = 0; j < msg->signal_count; ++j)
        {
            const signal_t *sig = &msg->signals[j];
            os_printf("  Signal %d: Name=%s, StartBit=%d, Length=%d, Endianness=%d, Scale=%.6f, Offset=%.2f, Min=%.2f, Max=%.2f, Unit=%s, Receiver=%s\n",
                j + 1, sig->name, sig->start_bit, sig->length, sig->endianness, sig->scale, sig->offset, sig->min_value, sig->max_value, sig->unit, sig->receiver);
        }
    }
}

int lua_dbc_decode(lua_State *L)
{
    int    can_id     = luaL_checkinteger(L, 1);
    uint32 data_d0_d3 = lua_tointeger(L, 2);
    uint32 data_d4_d7 = lua_tointeger(L, 3);
    uint64 data = ((uint64)data_d0_d3 << 32) | data_d4_d7;

    /* Tbd. */

    return 0;
}

void lua_register_dbc_command(core_t *core)
{
    lua_pushcfunction(core->L, lua_dbc_decode);
    lua_setglobal(core->L, "lua_dbc_decode");
}

static uint64 extract_raw_signal(uint64 can_frame, uint8 start_bit, uint8 length, endian_t endianness)
{
    uint64 mask = (1ULL << length) - 1;
    uint64 raw_value;

    if (ENDIANNESS_MOTOROLA == endianness)
    {
        int bit_pos = 64 - (start_bit + length);
        raw_value = (can_frame >> bit_pos) & mask;
    }
    else
    {
        raw_value = (can_frame >> start_bit) & mask;
    }

    return raw_value;
}

static void parse_message_line(char *line, message_t *message)
{
    char* token;
    char* rest = line;

    os_strtokr(rest, " ", &rest);

    token                = os_strtokr(rest, " ", &rest);
    message->id          = os_strtoul(token, NULL, 10);
    token                = os_strtokr(rest, ":", &rest);
    message->name        = os_strdup(token);
    token                = os_strtokr(rest, " ", &rest);
    message->dlc         = os_atoi(token);
    message->transmitter = os_strdup(trim_whitespace(rest));
}

static void parse_signal_line(char *line, signal_t *signal)
{
    char* token;
    char* rest = line;

    signal->name      = NULL;
    signal->unit      = NULL;
    signal->receiver  = NULL;
    signal->start_bit = 0;
    signal->length    = 0;
    signal->scale     = 1.0;
    signal->offset    = 0.0;
    signal->min_value = 0.0;
    signal->max_value = 0.0;
    signal->endianness = 0;

    os_strtokr(rest, " ", &rest);

    token = os_strtokr(rest, " ", &rest);
    if (token != NULL)
    {
        signal->name = os_strdup(token);
    }

    token = os_strtokr(rest, "|", &rest);
    if (token != NULL)
    {
        signal->start_bit = os_atoi(token);
        token = os_strtokr(rest, "@", &rest);
        if (token != NULL)
        {
            signal->length = os_atoi(token);
            if (*rest != '\0')
            {
                signal->endianness = os_atoi(rest);
                rest++;
            }
        }
    }

    token = os_strtokr(rest, "(", &rest);
    if (token != NULL)
    {
        token = os_strtokr(rest, ",", &rest);
        if (token != NULL)
        {
            signal->scale = os_atof(token);
            token         = os_strtokr(rest, ")", &rest);
            if (token != NULL)
            {
                signal->offset = os_atof(token);
            }
        }
    }

    token = os_strtokr(rest, "[", &rest);
    if (token != NULL)
    {
        token = os_strtokr(rest, "|", &rest);
        if (token != NULL)
        {
            signal->min_value = os_atof(token);
            token             = os_strtokr(rest, "]", &rest);
            if (token != NULL)
            {
                signal->max_value = os_atof(token);
            }
        }
    }

    os_strtokr(rest, "\"", &rest);
    if (rest[0] == '\"')
    {
        signal->unit = os_strdup("");
        rest++;
    }
    else
    {
        char* end = os_strtokr(rest, "\"", &rest);
        if (end != NULL)
        {
            signal->unit = os_strdup(end);
        }
        else
        {
            signal->unit = os_strdup("");
        }
    }

    while (*rest == ' ')
    {
        rest++;
    }

    token = os_strtokr(rest, " ", &rest);
    if (token != NULL)
    {
        signal->receiver = os_strdup(token);
    }
    else
    {
        signal->receiver = os_strdup("");
    }
}

static bool_t starts_with(const char *str, const char *prefix)
{
    bool_t status;
    size_t len_prefix = os_strlen(prefix);
    size_t len_str    = os_strlen(str);

    if (len_str < len_prefix)
    {
        status = IS_FALSE;
    }
    else
    {
        status = (bool_t)(os_strncmp(prefix, str, len_prefix) == 0);
    }

    return status;
}

static char* trim_whitespace(char *str)
{
    char *end;

    if (NULL == str)
    {
        return "";
    }

    while (os_isspace((unsigned char)*str))
    {
        str++;
    }

    if (0  == *str)
    {
        return str;
    }

    end = str + os_strlen(str) - 1;
    while ((end > str) && (os_isspace((unsigned char)*end)))
    {
        end--;
    }

    *(end + 1) = 0;

    return str;
}
