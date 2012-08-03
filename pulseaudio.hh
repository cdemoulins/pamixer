#pragma once

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

#include <iostream>
#include <string>
#include <list>
#include <cmath>

#include <pulse/pulseaudio.h>

enum state {
    CONNECTING,
    CONNECTED,
    ERROR
};
typedef enum state state_t;

enum device_type {
    SOURCE,
    SINK
};
typedef enum device_type device_type_t;

void state_cb(pa_context* context, void* raw);
void sink_list_cb(pa_context *c, const pa_sink_info *i, int eol, void *userdata);
void source_list_cb(pa_context *c, const pa_source_info *i, int eol, void *userdata);
void server_info_cb(pa_context* context, const pa_server_info* i, void* raw);
void success_cb(pa_context* context, int success, void* raw);

double round(double value) {
    return (value > 0.0) ? floor(value + 0.5) : ceil(value - 0.5);
}

int percent(pa_cvolume& volume) {
    return (int) round(((double) pa_cvolume_avg(&volume) * 100.) / PA_VOLUME_NORM);
}


/*
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

    Device(const pa_source_info* i) {
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

    Device(const pa_sink_info* i) {
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
};

class ServerInfo {
public:
    std::string default_source_name;
    std::string default_sink_name;
};

/*
 * Class to manipulate the pulseaudio server using the asynchronous C library.
 * When the constructor is called, a connection is established to a local pulseaudio server.
 * If the connection fail an exception is raised.
 */
class Pulseaudio {
private:
    pa_mainloop* mainloop;
    pa_mainloop_api* mainloop_api;
    pa_context* context;
    int retval;

    void iterate(pa_operation* op) {
        while (pa_operation_get_state(op) == PA_OPERATION_RUNNING) {
            pa_mainloop_iterate(mainloop, 1, &retval);
        }
    }

public:
    state_t state;

    /*
     * Initialize the connection to a local pulseaudio
     * @param client_name
     */
    Pulseaudio(std::string client_name) {
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

    /*
     * Properly disconnect and free all the resources
     */
    ~Pulseaudio() {
        if (state == CONNECTED)
            pa_context_disconnect(context);
        pa_mainloop_free(mainloop);
    }

    /*
     * @return list of the available sinks
     */
    std::list<Device> get_sinks() {
        std::list<Device> sinks;
        pa_operation* op = pa_context_get_sink_info_list(context, &sink_list_cb, &sinks);
        iterate(op);
        pa_operation_unref(op);

        return sinks;
    }

    /*
     * @return list of the available sources
     */
    std::list<Device> get_sources() {
        std::list<Device> sources;
        pa_operation* op = pa_context_get_source_info_list(context, &source_list_cb, &sources);
        iterate(op);
        pa_operation_unref(op);

        return sources;
    }

    /*
     * Get a specific sink
     * @param index index of the sink
     */
    Device get_sink(uint32_t index) {
        std::list<Device> sinks;
        pa_operation* op = pa_context_get_sink_info_by_index(context, index, &sink_list_cb, &sinks);
        iterate(op);
        pa_operation_unref(op);

        if (sinks.empty()) throw "The sink doesn't exit\n";
        return *(sinks.begin());
    }

    /*
     * Get a specific sink
     * @param name name of the requested sink
     */
    Device get_sink(std::string name) {
        std::list<Device> sinks;
        pa_operation* op = pa_context_get_sink_info_by_name(context, name.c_str(), &sink_list_cb, &sinks);
        iterate(op);
        pa_operation_unref(op);

        if (sinks.empty()) throw "The sink doesn't exit\n";
        return *(sinks.begin());
    }

    /*
     * Get a specific source
     * @param index index of the source
     */
    Device get_source(uint32_t index) {
        std::list<Device> sources;
        pa_operation* op = pa_context_get_source_info_by_index(context, index, &source_list_cb, &sources);
        iterate(op);
        pa_operation_unref(op);

        if (sources.empty()) throw "The source doesn't exit\n";
        return *(sources.begin());
    }

    /*
     * Get a specific source
     * @param name name of the requested source
     */
    Device get_source(std::string name) {
        std::list<Device> sources;
        pa_operation* op = pa_context_get_source_info_by_name(context, name.c_str(), &source_list_cb, &sources);
        iterate(op);
        pa_operation_unref(op);

        if (sources.empty()) throw "The source doesn't exit\n";
        return *(sources.begin());
    }

    /*
     * Get the default sink
     */
    Device get_default_sink() {
        ServerInfo info;
        pa_operation* op = pa_context_get_server_info(context, &server_info_cb, &info);
        iterate(op);
        pa_operation_unref(op);

        return get_sink(info.default_sink_name);
    }

    /*
     * Get the default source
     */
    Device get_default_source() {
        ServerInfo info;
        pa_operation* op = pa_context_get_server_info(context, &server_info_cb, &info);
        iterate(op);
        pa_operation_unref(op);

        return get_source(info.default_source_name);
    }

    /*
     * Set the volume to a new value for the specified device
     * @param device
     * @param new_volume
     */
    void set_volume(Device& device, int new_volume) {
        pa_cvolume* new_cvolume = pa_cvolume_set(&device.volume, device.volume.channels, (pa_volume_t) round(fmax(((double)new_volume * PA_VOLUME_NORM) / 100, 0)));
        pa_operation* op;
        if (device.type == SINK)
            op = pa_context_set_sink_volume_by_index(context, device.index, new_cvolume, success_cb, NULL);
        else
            op = pa_context_set_source_volume_by_index(context, device.index, new_cvolume, success_cb, NULL);
        iterate(op);
        pa_operation_unref(op);
    }

    /*
     * Change the mute state of a device
     * @param device
     * @param mute
     */
    void set_mute(Device& device, bool mute) {
        pa_operation* op;
        if (device.type == SINK)
            op = pa_context_set_sink_mute_by_index(context, device.index, (int) mute, success_cb, NULL);
        else
            op = pa_context_set_source_mute_by_index(context, device.index, (int) mute, success_cb, NULL);
        iterate(op);
        pa_operation_unref(op);
    }
};


/////////////////////////////////////////////////
// Callback functions

void state_cb(pa_context* context, void* raw) {
    Pulseaudio* pulse = (Pulseaudio*) raw;
    switch(pa_context_get_state(context))
    {
        case PA_CONTEXT_READY:
            pulse->state = CONNECTED;
            break;
        case PA_CONTEXT_FAILED:
            pulse->state = ERROR;
            break;
        case PA_CONTEXT_UNCONNECTED:
        case PA_CONTEXT_AUTHORIZING:
        case PA_CONTEXT_SETTING_NAME:
        case PA_CONTEXT_CONNECTING:
        case PA_CONTEXT_TERMINATED:
            break;
    }
}

void sink_list_cb(pa_context *c, const pa_sink_info *i, int eol, void *raw) {
    if (eol != 0) return;

    std::list<Device>* sinks = (std::list<Device>*) raw;
    Device s(i);
    sinks->push_back(s);
}

void source_list_cb(pa_context *c, const pa_source_info *i, int eol, void *raw) {
    if (eol != 0) return;

    std::list<Device>* sources = (std::list<Device>*) raw;
    Device s(i);
    sources->push_back(s);
}

void server_info_cb(pa_context* context, const pa_server_info* i, void* raw) {
    ServerInfo* info = (ServerInfo*) raw;
    info->default_sink_name = i->default_sink_name;
    info->default_source_name = i->default_source_name;
}

void success_cb(pa_context* context, int success, void* raw) {
}

/////////////////////////////////////////////////
