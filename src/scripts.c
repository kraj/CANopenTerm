/** @file scripts.h
 *
 *  A versatile software tool to analyse and configure CANopen devices.
 *
 *  Copyright (c) 2022, Michael Fitzmayer. All rights reserved.
 *  SPDX-License-Identifier: MIT
 *
 **/

#include "SDL.h"
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "dirent.h"
#include "core.h"
#include "scripts.h"

void list_scripts(void)
{
    DIR *dir;

    dir = opendir("./scripts");
    if (NULL != dir)
    {
        struct dirent *ent;
        while (NULL != (ent = readdir (dir)))
        {
            switch (ent->d_type)
            {
                case DT_REG:
                    puts(ent->d_name);
                    break;
                case DT_DIR:
                case DT_LNK:
                default:
                    break;
            }
        }
        closedir (dir);
    }
}

void run_script(const char* name, core_t* core)
{
    char script_path[64] = { 0 };
    SDL_snprintf(script_path, 64, "scripts/%s", name);
    if (LUA_OK == luaL_dofile(core->L, script_path))
    {
        lua_pop(core->L, lua_gettop(core->L));
    }
    else
    {
        SDL_LogWarn(0, "Could not load script '%s'", name);
    }
}
