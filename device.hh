#ifndef DEVICE_H
#define DEVICE_H

#include <pulse/pulseaudio.h>
#include <string>


enum device_type {
    SOURCE,
    SINK
};
typedef enum device_type device_type_t;


/**
 * Class to store device (sink or source) related informations
 *
 * @see pa_sink_info
 * @see pa_source_info
 */
class Device {
public:
    uint32_t index;
    device_type_t type;
    std::string name;
    std::string description;
    pa_cvolume volume;
    int volume_percent;
    bool mute;

    Device(const pa_source_info* i);
    Device(const pa_sink_info* i);

private:
    int percent(pa_cvolume& volume);
};

#endif
