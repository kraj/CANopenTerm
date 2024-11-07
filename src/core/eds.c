/** @file eds.c
 *
 *  A versatile software tool to analyse and configure CANopen devices.
 *
 *  Copyright (c) 2024, Michael Fitzmayer. All rights reserved.
 *  SPDX-License-Identifier: MIT
 *
 **/

#include <string.h>
#include <dirent.h>
#include "eds.h"
#include "ini.h"
#include "table.h"

void list_eds(void)
{
    status_t status;
    table_t  table = { DARK_CYAN, DARK_WHITE, 3, 25, 1 };
    DIR_t*   d;
    int      script_no = 1;

    struct dirent_t* dir;

    status = table_init(&table, 1024);
    if (ALL_OK != status)
    {
        return;
    }

    d = os_opendir("eds");
    if (d)
    {
        table_print_header(&table);
        table_print_row("No.", "File name", "-", &table);
        table_print_divider(&table);

        while ((dir = os_readdir(d)) != NULL)
        {
            if (os_strstr(dir->d_name, ".eds") != NULL)
            {
                char script_no_str[4];
                os_snprintf(script_no_str, 4, "%3d", script_no);

                table_print_row(script_no_str, dir->d_name, "-", &table);
                script_no++;
            }
        }
        os_closedir(d);
    }
    else
    {
        os_log(LOG_WARNING, "Could not open eds directory.");
    }

    table_print_footer(&table);
    table_flush(&table);
}

void validate_eds(const char* name, core_t* core)
{
    (void)name;
    (void)core;
    /* tbd. */
}
