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


#include "basic-info.hh"
#include <pulse/ext-stream-restore.h>
#include <cmath>
#include <cstring>
#include <iostream>


void
BasicInfo::setVolume(const pa_cvolume* v) {
    volume         = *v;
    volume_avg     = pa_cvolume_avg(v);
    volume_percent = (int) round( (double)volume_avg * 100. / PA_VOLUME_NORM );
}



