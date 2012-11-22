======================================
pamixer: pulseaudio command line mixer
======================================

pamixer is like amixer but for pulseaudio. It can control the volume levels of the sinks.

Also, this project can provide you a small C++ library to control pulseaudio.


Features
--------

* Get the current volume of the default sink, the default source or a selected one by his id
* Set the volume for the default sink, the default source or any other device
* List the sinks
* List the sources
* Increase / Decrease the volume for a device
* Mute or unmute a device

Dependencies
------------

* libpulse
* boost-program_options

Installation
------------

* From source:

    * Get the source::

        git clone https://cdemoulins@github.com/cdemoulins/pamixer.git

    * Compile::

        make

    * And use it, no install command for now::

        ./pamixer --help

        Allowed options:
          -h [ --help ]         help message
          -s [ --sink ] arg     choose a different sink than the default
          --source arg          choose a different source than the default
          --default-source      select the default source
          --get-volume          get the current volume
          --set-volume arg      set the volume
          --increase arg        increase the volume
          --decrease arg        decrease the volume
          --toggle-mute         switch between mute and unmute
          --mute                set mute
          --unmute              unset mute
          --get-mute            display true if the volume is mute, false otherwise
          --allow-boost         allow volume to go above 100%
          --list-sinks          list the sinks
          --list-sources        list the sources

