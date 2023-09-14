/*
 * Copyright (C) 2022 m4sc
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


#include "sink-input.hh"
#include <pulse/ext-stream-restore.h>
#include <cmath>
#include <cstring>
#include <iostream>



SinkInput::SinkInput(const pa_sink_input_info* info) {

	name   = info->name;
	index = info->index;
    mute   = info->mute == 1;
    client = info->client;
    sink = info->sink;

    setVolume(&(info->volume));
}





