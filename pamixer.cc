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

#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include <list>
#include <string>
#include <iostream>
using namespace std;

#include <pulse/pulseaudio.h>

/* Function used to check that 'opt1' and 'opt2' are not specified
   at the same time. */
void conflicting_options(const po::variables_map& vm, const char* opt1, const char* opt2) {
    if (vm.count(opt1) && !vm[opt1].defaulted()
        && vm.count(opt2) && !vm[opt2].defaulted()) {

        throw logic_error(string("Conflicting options '") + opt1 + "' and '" + opt2 + "'.");
    }
}

Device get_selected_device(Pulseaudio& pulse, po::variables_map vm, string sink_name, string source_name) {
    Device device = pulse.get_default_sink();
    if (vm.count("sink")) {
        device = pulse.get_sink(sink_name);
    } else if (vm.count("default-source")) {
        device = pulse.get_default_source();
    } else if (vm.count("source")) {
        device = pulse.get_source(source_name);
    }
    return device;
}

int
main(int argc, char* argv[])
{
    string sink_name, source_name;
    int value;

    po::options_description options("Allowed options");
    options.add_options()
        ("help,h", "help message")
        ("sink,s", po::value(&sink_name), "choose a different sink than the default")
        ("source", po::value(&source_name), "choose a different source than the default")
        ("default-source", "select the default source")
        ("get-volume", "get the current volume")
        ("set-volume", po::value<int>(&value), "set the volume")
        ("increase", po::value<int>(&value), "increase the volume")
        ("decrease", po::value<int>(&value), "decrease the volume")
        ("toggle-mute", "switch between mute and unmute")
        ("mute", "set mute")
        ("unmute", "unset mute")
        ("get-mute", "display true if the volume is mute, false otherwise")
        ("list-sinks", "list the sinks")
        ("list-sources", "list the sources")
    ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, options), vm);
    po::notify(vm);

    if (vm.count("help") || vm.size() == 0) {
        cout << options << endl;
        return 0;
    }

    conflicting_options(vm, "set-volume", "increase");
    conflicting_options(vm, "set-volume", "decrease");
    conflicting_options(vm, "decrease", "increase");
    conflicting_options(vm, "toggle-mute", "mute");
    conflicting_options(vm, "toggle-mute", "unmute");
    conflicting_options(vm, "unmute", "mute");
    conflicting_options(vm, "sink", "source");
    conflicting_options(vm, "sink", "default-source");
    conflicting_options(vm, "get-volume", "get-mute");
    conflicting_options(vm, "get-volume", "list-sinks");
    conflicting_options(vm, "get-volume", "list-sources");

    Pulseaudio pulse("pamixer");
    Device device = get_selected_device(pulse, vm, sink_name, source_name);

    if (vm.count("set-volume")) {
        pulse.set_volume(device, value);
        device = get_selected_device(pulse, vm, sink_name, source_name);
    } else if (vm.count("increase")) {
        pulse.set_volume(device, device.volume_percent + value);
        device = get_selected_device(pulse, vm, sink_name, source_name);
    } else if (vm.count("decrease")) {
        pulse.set_volume(device, device.volume_percent - value);
        device = get_selected_device(pulse, vm, sink_name, source_name);
    }

    if (vm.count("toggle-mute")) {
        pulse.set_mute(device, !device.mute);
    } else if (vm.count("mute")) {
        pulse.set_mute(device, true);
    } else if (vm.count("unmute")) {
        pulse.set_mute(device, false);
    }

    int ret = 0;
    if (vm.count("get-volume")) {
        cout << device.volume_percent << flush;
        ret = (device.volume_percent > 0 ? 0 : 1);
    } else if (vm.count("get-mute")) {
        cout << boolalpha << device.mute << flush;
        ret = (device.mute ? 0 : 1);
    } else {
        if (vm.count("list-sinks")) {
            list<Device> sinks = pulse.get_sinks();
            list<Device>::iterator it;
            cout << "Sinks:" << endl;
            for (it = sinks.begin(); it != sinks.end(); ++it) {
                cout << it->index << " \"" << it->name << "\" \"" << it->description << "\"" << endl;
            }
        }
        if (vm.count("list-sources")) {
            list<Device> sources = pulse.get_sources();
            list<Device>::iterator it;
            cout << "Sources:" << endl;
            for (it = sources.begin(); it != sources.end(); ++it) {
                cout << it->index << " \"" << it->name << "\" \"" << it->description << "\"" << endl;
            }
        }
    }

    return ret;
}
