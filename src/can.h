/** @file can.h
 *
 *  A versatile software tool to analyse and configure CANopen devices.
 *
 *  Copyright (c) 2022, Michael Fitzmayer. All rights reserved.
 *  SPDX-License-Identifier: MIT
 *
 **/

#ifndef CAN_H
#define CAN_H

#include "core.h"

void     can_init(core_t* core_t);
void     can_deinit(core_t* core);
SDL_bool is_can_initialised(core_t* core);

#endif /* CAN_H */
