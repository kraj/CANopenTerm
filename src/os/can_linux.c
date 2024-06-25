/** @file can.c
 *
 *  A versatile software tool to analyse and configure CANopen devices.
 *
 *  Copyright (c) 2024, Michael Fitzmayer. All rights reserved.
 *  SPDX-License-Identifier: MIT
 *
 **/

#include <errno.h>
#include <fcntl.h>
#include <libsocketcan.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <linux/sockios.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include "can.h"
#include "core.h"
#include "os.h"

static int can_socket;

static int  can_monitor(void* core);
static char err_message[100] = { 0 };

void can_init(core_t* core)
{
    if (NULL == core)
    {
        return;
    }

    core->can_monitor_th = os_create_thread(can_monitor, "CAN monitor thread", (void*)core);
}

void can_deinit(core_t* core)
{
    if (NULL == core)
    {
        return;
    }

    core->can_status = 0;
    core->is_can_initialised = IS_FALSE;

    close(can_socket);
}

void can_quit(core_t* core)
{
    if (NULL == core)
    {
        return;
    }

    if (IS_TRUE == is_can_initialised(core))
    {
        can_deinit(core);
    }

    os_detach_thread(core->can_monitor_th);
}

uint32 can_write(can_message_t* message, disp_mode_t disp_mode, const char* comment)
{
    int    index;
    struct can_frame frame;
    long   num_bytes;

    frame.can_id  = message->id;
    frame.can_dlc = message->length;

    for (index = 0; index < 8; index += 1)
    {
        frame.data[index] = message->data[index];
    }

    num_bytes = write(can_socket, &frame, sizeof(frame));

    os_delay(1);

    if (-1 == num_bytes)
    {
        return errno;
    }
    else
    {
        return 0;
    }
}

uint32 can_read(can_message_t* message)
{
    int    index;
    struct can_frame frame;
    struct msghdr msg;
    struct iovec iov;
    char   ctrlmsg[CMSG_SPACE(sizeof(struct timeval))];
    struct cmsghdr* cmsg;
    struct timeval* tv;
    int    nbytes;

    iov.iov_base = &frame;
    iov.iov_len = sizeof(frame);

    msg.msg_name = NULL;
    msg.msg_namelen = 0;
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
    msg.msg_control = &ctrlmsg;
    msg.msg_controllen = sizeof(ctrlmsg);
    msg.msg_flags = 0;

    nbytes = recvmsg(can_socket, &msg, 0);
    if (nbytes < 0)
    {
        return nbytes;
    }

    message->id = frame.can_id;
    message->length = frame.can_dlc;

    for (index = 0; index < 8; index += 1)
    {
        message->data[index] = frame.data[index];
    }

    for (cmsg = CMSG_FIRSTHDR(&msg); cmsg != NULL; cmsg = CMSG_NXTHDR(&msg, cmsg))
    {
        if (cmsg->cmsg_level == SOL_SOCKET && cmsg->cmsg_type == SO_TIMESTAMP)
        {
            tv = (struct timeval*)CMSG_DATA(cmsg);
            message->timestamp_us = tv->tv_sec * 1000000ULL + tv->tv_usec;
            break;
        }
    }

    return 0;
}

void can_set_baud_rate(uint8 command, core_t* core)
{
    if (NULL == core)
    {
        return;
    }

    core->baud_rate = command;

    if (IS_TRUE == is_can_initialised(core))
    {
        can_deinit(core);
    }
}

void limit_node_id(uint8* node_id)
{
    if (*node_id < 0x01)
    {
        *node_id = 0x01;
    }
    else if (*node_id > 0x7f)
    {
        *node_id = 0x7f;
    }
}

int lua_can_write(lua_State* L)
{
    int           can_id      = luaL_checkinteger(L, 1);
    int           length      = luaL_checkinteger(L, 2);
    uint32        can_status;
    uint32        data_d0_d3  = lua_tointeger(L, 3);
    uint32        data_d4_d7  = lua_tointeger(L, 4);
    bool_t        show_output = lua_toboolean(L, 5);
    const char*   comment     = lua_tostring(L, 6);
    can_message_t message     = { 0 };
    disp_mode_t   disp_mode   = SILENT;

    message.id      = can_id;
    message.length  = length;
    message.data[3] = (data_d0_d3 & 0xff);
    message.data[2] = ((data_d0_d3 >> 8) & 0xff);
    message.data[1] = ((data_d0_d3 >> 16) & 0xff);
    message.data[0] = ((data_d0_d3 >> 24) & 0xff);
    message.data[7] = (data_d4_d7 & 0xff);
    message.data[6] = ((data_d4_d7 >> 8) & 0xff);
    message.data[5] = ((data_d4_d7 >> 16) & 0xff);
    message.data[4] = ((data_d4_d7 >> 24) & 0xff);

    if (IS_TRUE == show_output)
    {
        disp_mode = SCRIPT_MODE;
    }

    can_status = can_write(&message, disp_mode, comment);

    if (0 == can_status)
    {
        if (SCRIPT_MODE == disp_mode)
        {
            int  i;
            char buffer[34] = { 0 };

            if (NULL == comment)
            {
                comment = "-";
            }

            os_strlcpy(buffer, comment, 33);
            for (i = os_strlen(buffer); i < 33; ++i)
            {
                buffer[i] = ' ';
            }

            os_printf(LIGHT_BLACK, "CAN ");
            os_printf(DEFAULT_COLOR, "     0x%02X   -       -         %03u     ", can_id, length);
            os_printf(LIGHT_GREEN, "SUCC    ");
            os_printf(DARK_MAGENTA, "%s ", buffer);
            os_printf(DEFAULT_COLOR, "Write: 0x%08X%08X\n", data_d0_d3, data_d4_d7);
        }
        lua_pushboolean(L, 1);
    }
    else
    {
        can_print_error(can_id, can_get_error_message(can_status), disp_mode);
        lua_pushboolean(L, 0);
    }

    return 1;
}

int lua_can_read(lua_State* L)
{
    can_message_t message   = { 0 };
    char          buffer[9] = { 0 };
    uint32        status;
    uint32        length;

    status = can_read(&message);
    if (0 == status)
    {
        length = message.length;

        if (length > 8)
        {
            length = 8;
        }

        lua_pushinteger(L, message.id);
        lua_pushinteger(L, length);

        os_memcpy((void*)&buffer, &message.data, message.length);

        lua_pushlstring(L, (const char*)buffer, length);
        lua_pushinteger(L, message.timestamp_us);
        return 4;
    }
    else
    {
        lua_pushnil(L);
        return 1;
    }
}

void lua_register_can_commands(core_t* core)
{
    lua_pushcfunction(core->L, lua_can_write);
    lua_setglobal(core->L, "can_write");
    lua_pushcfunction(core->L, lua_can_read);
    lua_setglobal(core->L, "can_read");
}

const char* can_get_error_message(uint32 can_status)
{
#ifdef _WIN32
    if (PCAN_ERROR_OK != can_status)
    {
        CAN_GetErrorText(can_status, 0x09, err_message);
        return err_message;
    }
#else
    // Handle libsocketcan error messages if needed.
#endif
    return "Unknown CAN error or not handled";
}

void can_print_baud_rate_help(core_t* core)
{
    table_t      table = { DARK_CYAN, DARK_WHITE, 3, 13, 6 };
    char         status[14][7] = { 0 };
    unsigned int status_index = core->baud_rate;
    unsigned int index;

    if (status_index > 13)
    {
        status_index = 13;
    }

    for (index = 0; index < 14; index += 1)
    {
        if (status_index == index)
        {
            os_snprintf(status[index], 7, "Active");
        }
        else
        {
            os_snprintf(status[index], 2, " ");
        }
    }

    table_print_header(&table);
    table_print_row("CMD", "Description", "Status", &table);
    table_print_divider(&table);
    table_print_row("  0", "1 MBit/s", status[0], &table);
    table_print_row("  1", "800 kBit/s", status[1], &table);
    table_print_row("  2", "500 kBit/s", status[2], &table);
    table_print_row("  3", "250 kBit/s", status[3], &table);
    table_print_row("  4", "125 kBit/s", status[4], &table);
    table_print_row("  5", "100 kBit/s", status[5], &table);
    table_print_row("  6", "95,238 kBit/s", status[6], &table);
    table_print_row("  7", "83,333 kBit/s", status[7], &table);
    table_print_row("  8", "50 kBit/s", status[8], &table);
    table_print_row("  9", "47,619 kBit/s", status[9], &table);
    table_print_row(" 10", "33,333 kBit/s", status[10], &table);
    table_print_row(" 11", "20 kBit/s", status[11], &table);
    table_print_row(" 12", "10 kBit/s", status[12], &table);
    table_print_row(" 13", "5 kBit/s", status[13], &table);
    table_print_footer(&table);
}

bool_t is_can_initialised(core_t* core)
{
    if (NULL == core)
    {
        return IS_FALSE;
    }

    return core->is_can_initialised;
}

static int can_monitor(void* core_pt)
{
    char    err_message[100] = { 0 };
    core_t* core = core_pt;

    if (NULL == core)
    {
        return 1;
    }

    core->baud_rate = 3;

    while (IS_TRUE == core->is_running)
    {
        while (IS_FALSE == is_can_initialised(core))
        {
            struct sockaddr_can addr;
            struct ifreq ifr;
            int    buffer_size = 1024 * 1024; /* 1MB */
            int    enable_timestamp = 1;

            can_socket = socket(PF_CAN, SOCK_RAW, CAN_RAW);
            if (can_socket < 0)
            {
                os_log(LOG_ERROR, "Error while opening socket");
                return 1;
            }

            setsockopt(can_socket, SOL_SOCKET, SO_SNDBUF, &buffer_size, sizeof(buffer_size));
            setsockopt(can_socket, SOL_SOCKET, SO_TIMESTAMP, &enable_timestamp, sizeof(enable_timestamp));

            strcpy(ifr.ifr_name, core->can_interface);
            if (ioctl(can_socket, SIOCGIFINDEX, &ifr) < 0)
            {
                os_log(LOG_ERROR, "Invalid CAN interface: %s", core->can_interface);
                close(can_socket);
                core->is_can_initialised = IS_FALSE;
                return 1;
            }

            addr.can_family  = AF_CAN;
            addr.can_ifindex = ifr.ifr_ifindex;

            if (bind(can_socket, (struct sockaddr*)&addr, sizeof(addr)) < 0)
            {
                os_log(LOG_ERROR, "Error in socket bind");
                return 1;
            }

            core->is_can_initialised = IS_TRUE;
            os_log(LOG_SUCCESS, "CAN successfully initialised");
            os_print_prompt();

            os_delay(1);
            continue;
        }

        os_delay(1);
    }

    return 0;
}
