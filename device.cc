#include "device.hh"

#include <cmath>

Device::Device(const pa_source_info* i) {
    index = i->index;
    type = SOURCE;
    name = i->name;
    description = i->description;
    volume.channels = i->volume.channels;
    int n;
    for (n = 0; n < volume.channels; ++n)
        volume.values[n] = i->volume.values[n];
    volume_percent = percent(volume);

    mute = i->mute == 1;
}

Device::Device(const pa_sink_info* i) {
    index = i->index;
    type = SINK;
    name = i->name;
    description = i->description;
    volume.channels = i->volume.channels;
    int n;
    for (n = 0; n < volume.channels; ++n)
        volume.values[n] = i->volume.values[n];
    volume_percent = percent(volume);

    mute = i->mute == 1;
}


}

int
Device::percent(pa_cvolume& volume) {
    return (int) round(((double) pa_cvolume_avg(&volume) * 100.) / PA_VOLUME_NORM);
}