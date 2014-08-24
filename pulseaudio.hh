#ifndef PULSEAUDIO_H
#define PULSEAUDIO_H


#include <pulse/pulseaudio.h>
#include <string>
#include <list>
#include "device.hh"


class ServerInfo {
public:
    std::string default_source_name;
    std::string default_sink_name;
};


enum state {
    CONNECTING,
    CONNECTED,
    ERROR
};
typedef enum state state_t;

void state_cb(pa_context* context, void* raw);
void sink_list_cb(pa_context *c, const pa_sink_info *i, int eol, void *userdata);
void source_list_cb(pa_context *c, const pa_source_info *i, int eol, void *userdata);
void server_info_cb(pa_context* context, const pa_server_info* i, void* raw);
void success_cb(pa_context* context, int success, void* raw);


/**
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

    void iterate(pa_operation* op);

public:
    state_t state;

    /**
     * Initialize the connection to a local pulseaudio
     * @param client_name
     */
    Pulseaudio(std::string client_name);

    /**
     * Properly disconnect and free all the resources
     */
    ~Pulseaudio();

    /**
     * @return list of the available sinks
     */
    std::list<Device> get_sinks();

    /**
     * @return list of the available sources
     */
    std::list<Device> get_sources();

    /**
     * Get a specific sink
     * @param index index of the sink
     */
    Device get_sink(uint32_t index);

    /**
     * Get a specific sink
     * @param name name of the requested sink
     */
    Device get_sink(std::string name);

    /**
     * Get a specific source
     * @param index index of the source
     */
    Device get_source(uint32_t index);

    /**
     * Get a specific source
     * @param name name of the requested source
     */
    Device get_source(std::string name);

    /**
     * Get the default sink
     */
    Device get_default_sink();

    /**
     * Get the default source
     */
    Device get_default_source();

    /**
     * Set the volume to a new value for the specified device
     * @param device
     * @param new_volume
     */
    void set_volume(Device& device, int new_volume);

    /**
     * Change the mute state of a device
     * @param device
     * @param mute
     */
    void set_mute(Device& device, bool mute);
};

#endif
