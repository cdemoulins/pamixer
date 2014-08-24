#include "pulseaudio.hh"
#include <cmath>




void
Pulseaudio::iterate(pa_operation* op) {
    while (pa_operation_get_state(op) == PA_OPERATION_RUNNING) {
        pa_mainloop_iterate(mainloop, 1, &retval);
    }
}

Pulseaudio::Pulseaudio(std::string client_name) {
    mainloop = pa_mainloop_new();
    mainloop_api = pa_mainloop_get_api(mainloop);
    context = pa_context_new(mainloop_api, client_name.c_str());
    pa_context_set_state_callback(context, &state_cb, this);

    state = CONNECTING;
    pa_context_connect(context, NULL, PA_CONTEXT_NOFLAGS, NULL);
    while (state == CONNECTING) {
        pa_mainloop_iterate(mainloop, 1, &retval);
    }
    if (state == ERROR) {
        throw "Connection error\n";
    }
}

Pulseaudio::~Pulseaudio() {
    if (state == CONNECTED)
        pa_context_disconnect(context);
    pa_mainloop_free(mainloop);
}

std::list<Device>
Pulseaudio::get_sinks() {
    std::list<Device> sinks;
    pa_operation* op = pa_context_get_sink_info_list(context, &sink_list_cb, &sinks);
    iterate(op);
    pa_operation_unref(op);

    return sinks;
}

std::list<Device>
Pulseaudio::get_sources() {
    std::list<Device> sources;
    pa_operation* op = pa_context_get_source_info_list(context, &source_list_cb, &sources);
    iterate(op);
    pa_operation_unref(op);

    return sources;
}

Device
Pulseaudio::get_sink(uint32_t index) {
    std::list<Device> sinks;
    pa_operation* op = pa_context_get_sink_info_by_index(context, index, &sink_list_cb, &sinks);
    iterate(op);
    pa_operation_unref(op);

    if (sinks.empty()) throw "The sink doesn't exit\n";
    return *(sinks.begin());
}

Device
Pulseaudio::get_sink(std::string name) {
    std::list<Device> sinks;
    pa_operation* op = pa_context_get_sink_info_by_name(context, name.c_str(), &sink_list_cb, &sinks);
    iterate(op);
    pa_operation_unref(op);

    if (sinks.empty()) throw "The sink doesn't exit\n";
    return *(sinks.begin());
}

Device
Pulseaudio::get_source(uint32_t index) {
    std::list<Device> sources;
    pa_operation* op = pa_context_get_source_info_by_index(context, index, &source_list_cb, &sources);
    iterate(op);
    pa_operation_unref(op);

    if (sources.empty()) throw "The source doesn't exit\n";
    return *(sources.begin());
}

Device
Pulseaudio::get_source(std::string name) {
    std::list<Device> sources;
    pa_operation* op = pa_context_get_source_info_by_name(context, name.c_str(), &source_list_cb, &sources);
    iterate(op);
    pa_operation_unref(op);

    if (sources.empty()) throw "The source doesn't exit\n";
    return *(sources.begin());
}

Device
Pulseaudio::get_default_sink() {
    ServerInfo info;
    pa_operation* op = pa_context_get_server_info(context, &server_info_cb, &info);
    iterate(op);
    pa_operation_unref(op);

    return get_sink(info.default_sink_name);
}

Device
Pulseaudio::get_default_source() {
    ServerInfo info;
    pa_operation* op = pa_context_get_server_info(context, &server_info_cb, &info);
    iterate(op);
    pa_operation_unref(op);

    return get_source(info.default_source_name);
}

void
Pulseaudio::set_volume(Device& device, int new_volume) {
    pa_cvolume* new_cvolume = pa_cvolume_set(&device.volume, device.volume.channels, (pa_volume_t) round(fmax(((double)new_volume * PA_VOLUME_NORM) / 100, 0)));
    pa_operation* op;
    if (device.type == SINK)
        op = pa_context_set_sink_volume_by_index(context, device.index, new_cvolume, success_cb, NULL);
    else
        op = pa_context_set_source_volume_by_index(context, device.index, new_cvolume, success_cb, NULL);
    iterate(op);
    pa_operation_unref(op);
}

void
Pulseaudio::set_mute(Device& device, bool mute) {
    pa_operation* op;
    if (device.type == SINK)
        op = pa_context_set_sink_mute_by_index(context, device.index, (int) mute, success_cb, NULL);
    else
        op = pa_context_set_source_mute_by_index(context, device.index, (int) mute, success_cb, NULL);
    iterate(op);
    pa_operation_unref(op);
}