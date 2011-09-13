
#include "pulseaudio.hh"
#include <list>
#include <iostream>
#include <string>

#include <pulse/pulseaudio.h>

int
main(void)
{
    Pulseaudio pulse("pamixer");
    Sink s = pulse.get_default_sink();
    pulse.set_sink_volume(s, 0.01);

    return 0;
}
