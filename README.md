pamixer: pulseaudio command line mixer
======================================

pamixer is like amixer but for pulseaudio. It can control the volume levels of the sinks.


Features
--------

* Get the current volume of the default sink or a selected one by his name
* Set the volume for the default sink or another
* List the sinks
* Increase / Decrease the volume
* Mute or unmute a sink

Dependencies
------------

* pulseaudio
* boost-program_options

Installation
------------

* From source:

    * Get the source::

>       git clone https://cdemoulins@github.com/cdemoulins/pamixer.git

    * Compile::

>       make

    * And use it, no install command for now::

>       ./pamixer --help

>       Allowed options:
>         -h [ --help ]         help message
>         -s [ --sink ] arg     choose a different sink than the default
>         --get                 get the current volume
>         --set arg             set the volume
>         --increase arg        increase the volume
>         --decrease arg        decrease the volume
>         --toggle-mute         switch between mute and unmute
>         --list-sinks          list the sinks

