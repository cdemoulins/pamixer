#ifndef STREAM_H
#define STREAM_H

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

#include <basic-info.hh>
#include <pulse/pulseaudio.h>
#include <pulse/ext-stream-restore.h>
#include <string>


/**
 * Class to store streams and used devices
 *
 * @see pa_ext_stream_restore_info
 */
class Stream : public BasicInfo{
public:
	/** The sink source of the stream when it was last seen */
    std::string device;
    Stream(const pa_ext_stream_restore_info* i);    
};

#endif
