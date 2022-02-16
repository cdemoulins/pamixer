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

#include <config.hh>
#include "pulseaudio.hh"
#include "device.hh"

#include <cxxopts.hpp>

#include <cmath>
#include <list>
#include <string>
#include <iostream>
using namespace std;

#include <pulse/pulseaudio.h>



void conflicting_options(const cxxopts::ParseResult& result, const char* opt1, const char* opt2);
Device get_selected_device(Pulseaudio& pulse, const cxxopts::ParseResult& result, const string& sink_name, const string& source_name);
int gammaCorrection(int i, double gamma, int delta);
int main(int argc, char* argv[]);



/* Function used to check that 'opt1' and 'opt2' are not specified
   at the same time. */
void conflicting_options(const cxxopts::ParseResult& result, const char* opt1, const char* opt2) {
    if (result.count(opt1) && !result[opt1].has_default()
        && result.count(opt2) && !result[opt2].has_default()) {

        throw logic_error(string("Conflicting options '") + opt1 + "' and '" + opt2 + "'.");
    }
}

Device get_selected_device(Pulseaudio& pulse, const cxxopts::ParseResult& result, const string& sink_name, const string& source_name) {
    Device device = pulse.get_default_sink();
    if (result.count("sink")) {
        device = pulse.get_sink(sink_name);
    } else if (result.count("default-source")) {
        device = pulse.get_default_source();
    } else if (result.count("source")) {
        device = pulse.get_source(source_name);
    }
    return device;
}

string device_state_to_string(Device device) {
    string state;
    switch(device.state) {
        case DEVICE_RUNNING: state = string("Running"); break;
        case DEVICE_IDLE: state = string("Idle"); break;
        case DEVICE_SUSPENDED: state = string("Suspended"); break;
        default: state = string("Invalid state");
    }
    return state;
}

pa_volume_t gammaCorrection(pa_volume_t i, double gamma, int delta) {
    double j = double(i);
    double relRelta = double(delta) / 100.0;

    j = j / PA_VOLUME_NORM;
    j = pow(j, (1.0/gamma));

    j = j + relRelta;
    if(j < 0.0) {
        j = 0.0;
    }

    j = pow(j, gamma);
    j = j * PA_VOLUME_NORM;

    return (pa_volume_t) round(j);
}

int main(int argc, char* argv[])
{
    string sink_name, source_name;
    int value, limit_value;
    double gamma;

    cxxopts::Options options("pamixer", "pulseaudio command line mixer");

    options.add_options()
        ("h,help", "help message")
        ("v,version", "print version info")
        ("sink", "choose a different sink than the default", cxxopts::value<string>(sink_name))
        ("source", "choose a different source than the default", cxxopts::value<string>(source_name))
        ("default-source", "select the default source")
        ("get-volume", "get the current volume")
        ("get-volume-human", "get the current volume percentage or the string \"muted\"")
        ("set-volume", "set the volume", cxxopts::value<int>(value))
        ("i,increase", "increase the volume", cxxopts::value<int>(value))
        ("d,decrease", "decrease the volume", cxxopts::value<int>(value))
        ("t,toggle-mute", "switch between mute and unmute")
        ("m,mute", "set mute")
        ("allow-boost", "allow volume to go above 100%")
        ("set-limit", "set a limit for the volume", cxxopts::value<int>(limit_value))
        ("gamma", "increase/decrease using gamma correction e.g. 2.2", cxxopts::value<double>(gamma)->default_value("1.0"))
        ("u,unmute", "unset mute")
        ("get-mute", "display true if the volume is mute, false otherwise")
        ("list-sinks", "list the sinks")
        ("list-sources", "list the sources")
        ("get-default-sink", "print the default sink")
        ;

    try
    {
        auto result = options.parse(argc, argv);

        // FIXME: print help message in case missing options
        if (result.count("help")) {
            cout << options.help() << endl;
            return 0;
        }

        if (result.count("version")) {
            cout << VERSION << endl;
            return 0;
        }

        conflicting_options(result, "set-volume", "increase");
        conflicting_options(result, "set-volume", "decrease");
        conflicting_options(result, "decrease", "increase");
        conflicting_options(result, "toggle-mute", "mute");
        conflicting_options(result, "toggle-mute", "unmute");
        conflicting_options(result, "unmute", "mute");
        conflicting_options(result, "sink", "source");
        conflicting_options(result, "sink", "default-source");
        conflicting_options(result, "get-volume", "list-sinks");
        conflicting_options(result, "get-volume", "list-sources");
        conflicting_options(result, "get-volume", "get-volume-human");
        conflicting_options(result, "get-volume", "get-default-sink");
        conflicting_options(result, "get-volume-human", "list-sinks");
        conflicting_options(result, "get-volume-human", "list-sources");
        conflicting_options(result, "get-volume-human", "get-mute");
        conflicting_options(result, "get-volume-human", "get-default-sink");
        conflicting_options(result, "get-mute", "list-sinks");
        conflicting_options(result, "get-mute", "list-sources");
        conflicting_options(result, "get-mute", "get-default-sink");

        Pulseaudio pulse("pamixer");
        Device device = get_selected_device(pulse, result, sink_name, source_name);

        if (result.count("set-volume") || result.count("increase") || result.count("decrease")) {
            if (value < 0) {
                value = 0;
            }

            pa_volume_t new_value = 0;
            if (result.count("set-volume")) {
                new_value = round( (double)value * (double)PA_VOLUME_NORM / 100.0);
            } else if (result.count("increase")) {
                new_value = gammaCorrection(device.volume_avg, gamma,  value);
            } else if (result.count("decrease")) {
                new_value = gammaCorrection(device.volume_avg, gamma, -value);
            }

            if (!result.count("allow-boost") && new_value > PA_VOLUME_NORM) {
                new_value = PA_VOLUME_NORM;
            }

            pulse.set_volume(device, new_value);
            device = get_selected_device(pulse, result, sink_name, source_name);
        }

        if (result.count("set-limit")) {

            if (limit_value < 0 ) {
                limit_value = 0;
            }

            pa_volume_t limit = round( (double)limit_value * (double)PA_VOLUME_NORM / 100.0);
            if (device.volume_avg > limit) {
                pulse.set_volume(device, limit);
                device = get_selected_device(pulse, result, sink_name, source_name);
            }
        }

        if (result.count("toggle-mute") || result.count("mute") || result.count("unmute")) {
            if (result.count("toggle-mute")) {
                pulse.set_mute(device, !device.mute);
            } else {
                pulse.set_mute(device, result.count("mute") || !result.count("unmute"));
            }
            device = get_selected_device(pulse, result, sink_name, source_name);
        }

        if (result.count("get-volume") && result.count("get-mute")) {
            cout << boolalpha << device.mute << ' ' << device.volume_percent << '\n';
        } else if (result.count("get-volume")) {
            cout << device.volume_percent << '\n';
        } else if (result.count("get-volume-human")) {
            if (device.mute) {
                cout << "muted\n";
            } else {
                cout << device.volume_percent << "%\n";
            }
        } else if (result.count("get-mute")) {
            cout << boolalpha << device.mute << '\n';
        } else {
            if (result.count("list-sinks")) {
                cout << "Sinks:\n";
                for (const Device& sink : pulse.get_sinks()) {
                    cout << sink.index << " \""
                         << sink.name << "\" \""
                         << device_state_to_string(sink) << "\" \""
                         << sink.description << "\"\n";
                }
            }
            if (result.count("list-sources")) {
                cout << "Sources:\n";
                for (const Device& source : pulse.get_sources()) {
                    cout << source.index << " \""
                         << source.name << "\" \""
                         << device_state_to_string(source) << "\" \""
                         << source.description << "\"\n";
                }
            }
            if (result.count("get-default-sink")) {
                Device sink = pulse.get_default_sink();
                cout << "Default sink:\n";
                cout << sink.index << " \""
                     << sink.name << "\" \""
                     << sink.description << "\"\n";
            }
        }

        return 0;
    }
    catch (const char* message)
    {
        cerr << message << '\n';
        return 3;
    }
    catch (const std::exception& e)
    {
        cerr << argv[0] << ": " << e.what() << "\n\n";
        cerr << options.help() << '\n';
        return 2;
    }
}
