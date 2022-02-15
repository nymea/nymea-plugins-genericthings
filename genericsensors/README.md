# Generic sensors

This plugin allows to create generic sensors in nymea. Such things don't have any
functionality on their own, but allow to create a visually appealing experience when
using general purpose input and outputs, such as GPIOs or MQTT.

## Set-up

First, set up a sensor with generic inputs or outputs. For instance from the GPIO plugin
or the MQTT plugin. When done, set up the desired generic thing and use the things settings to connect
the inputs and/or outputs or use nymea scripts to process MQTT messages and fill in the states accordingly.

### Example: A soil moisture sensor on an analog input GPIO

First, set up the GPIO to receive the analog GPIO values from the sensor. Normally
such an analog input GPIO will dispatch values from 0V to e.g. 3.3V, depending on the
hardware. An input value of 0V normally refers to a soil moisture value of 0%, while
an input value of 3.3V would refer to the maximum, that is 100% soil moisture. Once
those input values are received in nymea, set up a generic soil moisture thing and
go to its settings. In there you'll find the Input/Output connections section which
allows to map this generic soil moisture sensure to the according GPIO input. Once
this connection has been made, the generic soil moisture thing will represent the
input from the GPIO in a nice sensor view.
Some sensors might not support the full range from 0% to 100% and might output
a value of 0V on a minimum soil moisture of e.g. 10% or might only be able to
measure the soil moisture up to, say, 90%. Normally the sensor hardware would give
that information in the data sheet or user guide. If this is the case, the appropriate
input range can also be defined in the generic moisture sensor page.

