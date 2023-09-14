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


#include "stream.hh"
#include <pulse/ext-stream-restore.h>
#include <cmath>
#include <cstring>
#include <iostream>



Stream::Stream(const pa_ext_stream_restore_info* info) {
	mute = info->mute == 1;
	name = info->name;
	//NULL handling as the device for the stream can point to 0x0
	if(info->device) device = info->device;

    setVolume(&(info->volume));
}




