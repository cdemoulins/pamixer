#ifndef DEVICE_H
#define DEVICE_H

/*
 * Copyright (C) 2011 Clément Démoulins <clement@archivel.fr>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <basic-info.hh>
#include <pulse/pulseaudio.h>
#include <string>


enum device_type {
    SOURCE,
    SINK
};
typedef enum device_type device_type_t;

enum device_state {
    DEVICE_INVALID_STATE,
    DEVICE_RUNNING,
    DEVICE_IDLE,
    DEVICE_SUSPENDED
};
typedef enum device_state device_state_t;

/**
 * Class to store device (sink or source) related informations
 *
 * @see pa_sink_info
 * @see pa_source_info
 */
class Device : public BasicInfo{
public:
    uint32_t index;
    device_type_t type;
    std::string description;
    device_state_t state;

    Device(const pa_source_info* i);
    Device(const pa_sink_info* i);
};

#endif
