# Generic lights

This plugin allows to create generic lights in nymea. Such lights don't have any
functionality on their own, but allow to create a light things which are controlled
using general purpose input and outputs, such as GPIOs, smart plugs or MQTT.

## Set-up

First, set up a thing with generic inputs or outputs. For instance from the GPIO or MQTT plugins.
When done, set up the desired generic light and use the things settings to connect
the inputs and/or outputs or nymea scripts to fill in the thing states accordingly.
