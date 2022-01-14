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


#include "device.hh"

#include <cmath>


Device::Device(const pa_source_info* info) {
    type            = SOURCE;
    index           = info->index;
    name            = info->name;
    description     = info->description;
    mute            = info->mute == 1;
    switch(info->state) {
        case PA_SOURCE_RUNNING: state = DEVICE_RUNNING; break;
        case PA_SOURCE_IDLE: state = DEVICE_IDLE; break;
        case PA_SOURCE_SUSPENDED: state = DEVICE_SUSPENDED; break;
        default: state = DEVICE_INVALID_STATE;
    }
    setVolume(&(info->volume));
}


Device::Device(const pa_sink_info* info) {
    type            = SINK;
    index           = info->index;
    name            = info->name;
    description     = info->description;
    mute            = info->mute == 1;
    switch(info->state) {
        case PA_SINK_RUNNING: state = DEVICE_RUNNING; break;
        case PA_SINK_IDLE: state = DEVICE_IDLE; break;
        case PA_SINK_SUSPENDED: state = DEVICE_SUSPENDED; break;
        default: state = DEVICE_INVALID_STATE;
    }
    setVolume(&(info->volume));
}


void
Device::setVolume(const pa_cvolume* v) {
    volume         = *v;
    volume_avg     = pa_cvolume_avg(v);
    volume_percent = (int) round( (double)volume_avg * 100. / PA_VOLUME_NORM );
}
