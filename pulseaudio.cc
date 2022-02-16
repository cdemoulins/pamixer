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


#include "pulseaudio.hh"
#include <cmath>
#include <unistd.h>

// Fix issue #7
#ifndef UINT32_MAX
#include <limits>
#define UINT32_MAX std::numeric_limits<uint32_t>::max()
#endif



void
Pulseaudio::iterate(pa_operation* op) {
    while (pa_operation_get_state(op) == PA_OPERATION_RUNNING) {
        pa_mainloop_iterate(mainloop, 1, &retval);
        usleep(100);
    }
}

Pulseaudio::Pulseaudio(std::string client_name) {
    mainloop = pa_mainloop_new();
    mainloop_api = pa_mainloop_get_api(mainloop);
    context = pa_context_new(mainloop_api, client_name.c_str());
    pa_context_set_state_callback(context, &state_cb, this);

    state = CONNECTING;
    if (pa_context_connect(context, NULL, PA_CONTEXT_NOFLAGS, NULL) < 0)
        throw "Connection error\n";
    while (state == CONNECTING) {
        if (pa_mainloop_iterate(mainloop, 1, &retval) < 0)
            throw "Mainloop error\n";
    }
    if (state == ERROR)
        throw "Connection error\n";
}

Pulseaudio::~Pulseaudio() {
    pa_context_set_state_callback(context, NULL, NULL);
    if (state == CONNECTED)
        pa_context_disconnect(context);
    pa_context_unref(context);
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

    if (sinks.empty()) throw "The sink doesn't exist\n";
    return *(sinks.begin());
}

Device
Pulseaudio::get_sink(std::string name) {
    std::list<Device> sinks;
    pa_operation* op = pa_context_get_sink_info_by_name(context, name.c_str(), &sink_list_cb, &sinks);
    iterate(op);
    pa_operation_unref(op);

    if (sinks.empty()) throw "The sink doesn't exist\n";
    return *(sinks.begin());
}

Device
Pulseaudio::get_source(uint32_t index) {
    std::list<Device> sources;
    pa_operation* op = pa_context_get_source_info_by_index(context, index, &source_list_cb, &sources);
    iterate(op);
    pa_operation_unref(op);

    if (sources.empty()) throw "The source doesn't exist\n";
    return *(sources.begin());
}

Device
Pulseaudio::get_source(std::string name) {
    std::list<Device> sources;
    pa_operation* op = pa_context_get_source_info_by_name(context, name.c_str(), &source_list_cb, &sources);
    iterate(op);
    pa_operation_unref(op);

    if (sources.empty()) throw "The source doesn't exist\n";
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
Pulseaudio::set_volume(Device& device, pa_volume_t new_volume) {
    if (new_volume > PA_VOLUME_MAX) {
        new_volume = PA_VOLUME_MAX;
    }
    pa_cvolume* new_cvolume = pa_cvolume_set(&device.volume, device.volume.channels, new_volume);
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
