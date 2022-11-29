#ifndef SINKINPUT_H
#define SINKINPUT_H

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


#include <pulse/pulseaudio.h>
#include <pulse/ext-stream-restore.h>
#include <string>


/**
 * Class to store streams and used devices
 *
 * @see pa_ext_stream_restore_info
 */
class SinkInput {
public:
	uint32_t index;
    std::string name;
    uint32_t client;
    uint32_t sink;
    bool mute;

    SinkInput(const pa_sink_input_info* i);
};

#endif
