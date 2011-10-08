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

int
main(int argc, char* argv[])
{
    string sink;
    double value;

    po::options_description options("Allowed options");
    options.add_options()
        ("help,h", "help message")
        ("sink,s", po::value(&sink), "choose a different sink than the default")
        ("get-volume", "get the current volume")
        ("set-volume", po::value<double>(&value), "set the volume")
        ("increase", po::value<double>(&value), "increase the volume")
        ("decrease", po::value<double>(&value), "decrease the volume")
        ("toggle-mute", "switch between mute and unmute")
        ("mute", "set mute")
        ("unmute", "unset mute")
        ("get-mute", "unset mute")
        ("list-sinks", "list the sinks")
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

    Pulseaudio pulse("pamixer");
    Sink s = pulse.get_default_sink();
    if (vm.count("sink")) {
        s = pulse.get_sink(sink);
    }

    if (vm.count("set")) {
        pulse.set_sink_volume(s, value);
        s = pulse.get_sink(s.index);
    } else if (vm.count("increase")) {
        pulse.set_sink_volume(s, s.volume_percent + value);
        s = pulse.get_sink(s.index);
    } else if (vm.count("decrease")) {
        pulse.set_sink_volume(s, s.volume_percent - value);
        s = pulse.get_sink(s.index);
    }

    if (vm.count("toggle-mute")) {
        pulse.set_sink_mute(s, !s.mute);
    } else if (vm.count("mute")) {
        pulse.set_sink_mute(s, true);
    } else if (vm.count("unmute")) {
        pulse.set_sink_mute(s, false);
    }

    int ret = 0;
    if (vm.count("get-volume")) {
        cout << s.volume_percent;
        ret = (s.volume_percent > 0 ? 0 : 1);
    } else if (vm.count("get-mute")) {
        cout << boolalpha << s.mute;
        ret = (s.mute ? 0 : 1);
    } else if (vm.count("list-sinks")) {
        list<Sink> sinks = pulse.get_sinks();
        list<Sink>::iterator it;
        for (it = sinks.begin(); it != sinks.end(); ++it) {
            cout << it->index << " " << it->name << " " << it->description << endl;
        }
    }

    return ret;
}
