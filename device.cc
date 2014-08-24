#include "device.hh"

#include <cmath>


Device::Device(const pa_source_info* info) {
    type            = SOURCE;
    index           = info->index;
    name            = info->name;
    description     = info->description;
    mute            = info->mute == 1;
    setVolume(&(info->volume));
}


Device::Device(const pa_sink_info* info) {
    type            = SINK;
    index           = info->index;
    name            = info->name;
    description     = info->description;
    mute            = info->mute == 1;
    setVolume(&(info->volume));
}


void
Device::setVolume(const pa_cvolume* v) {
    volume         = *v;
    volume_avg     = pa_cvolume_avg(v);
    volume_percent = (int) round( (double)volume_avg * 100. / PA_VOLUME_NORM );
}
