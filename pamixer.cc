
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
        ("get", "get the current volume")
        ("set", po::value<double>(&value), "set the volume")
        ("increase", po::value<double>(&value), "increase the volume")
        ("decrease", po::value<double>(&value), "decrease the volume")
        ("toggle_mute", "switch between mute and unmute")
    ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, options), vm);
    po::notify(vm);

    if (vm.count("help")) {
        cout << options << endl;
        return 0;
    }

    conflicting_options(vm, "set", "increase");
    conflicting_options(vm, "set", "decrease");
    conflicting_options(vm, "decrease", "increase");

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

    if (vm.count("toggle_mute")) {
        pulse.set_sink_mute(s, !s.mute);
    }

    if (vm.count("get")) {
        cout << s.volume_percent;
    }

    return 0;
}
