/** @file run_unit_tests.c
 *
 *  A versatile software tool to analyse and configure CANopen devices.
 *
 *  Copyright (c) 2024, Michael Fitzmayer. All rights reserved.
 *  SPDX-License-Identifier: MIT
 *
 **/

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include "cmocka.h"
#include "test_dict.h"
#include "test_nmt.h"
#include "test_os.h"
#include "test_scripts.h"
#include "test_sdo.h"

int main(void)
{
    const struct CMUnitTest tests[] =
    {
        cmocka_unit_test(test_dict_lookup),
        cmocka_unit_test(test_has_lua_extension),
        cmocka_unit_test(test_nmt_send_command),
        cmocka_unit_test(test_nmt_print_help),
        cmocka_unit_test(test_os_calloc),
        cmocka_unit_test(test_os_free),
        cmocka_unit_test(test_os_isdigit),
        cmocka_unit_test(test_os_isprint),
        cmocka_unit_test(test_os_itoa),
        cmocka_unit_test(test_os_memcpy),
        cmocka_unit_test(test_os_memmove),
        cmocka_unit_test(test_os_memset),
        cmocka_unit_test(test_os_realloc),
        cmocka_unit_test(test_os_snprintf),
        cmocka_unit_test(test_os_strchr),
        cmocka_unit_test(test_os_strcmp),
        cmocka_unit_test(test_os_strdup),
        cmocka_unit_test(test_os_strlcat),
        cmocka_unit_test(test_os_strlen),
        cmocka_unit_test(test_os_strncmp),
        cmocka_unit_test(test_os_strrchr),
        cmocka_unit_test(test_os_strtokr),
        cmocka_unit_test(test_os_strtol),
        cmocka_unit_test(test_os_strtoull),
        cmocka_unit_test(test_os_vsnprintf),
        cmocka_unit_test(test_sdo_lookup_abort_code),
        cmocka_unit_test(test_uint8),
        cmocka_unit_test(test_uint16),
        cmocka_unit_test(test_uint32),
        cmocka_unit_test(test_uint64),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
