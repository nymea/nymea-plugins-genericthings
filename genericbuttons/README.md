# Generic buttons

The generic buttons plugin allows to set up generic buttons (as in remote controls) and power switches
for devices that don't have their own specialized plugin but can be connected via some generic input
like GPIOs or MQTT.
The virtual buttons plugin allows to create virtual buttons. Unlike other nymea plugins they are not
meant to be used for a certain pyhsical device or service, but instead simulate a button or power switch
thing inside nymea. This may be useful to create buttons or toggle switches in nymea which are then
connected to a rule or script to trigger other behavior in the system. One example use case would be
to create a "lights off" button which is then hooked up to a rule to turn off a larger set of lights
with a single button press.
Another example would be to use the virtual switch as a condition for a scene to be active or not.

## Set-up

First, set up a thing with generic inputs or outputs. For instance from the GPIO plugin.
When done, set up the desired generic thing and use the things settings to connect
the inputs and/or outputs accordingly. For more complex inputs, like MQTT, use a
nymea script to process the MQTT payload and set the buttons events/states accordingls.

### Example: A light switch on a digital output GPIO

First, set up the GPIO to control the light. Please refer to the documentation of the
according plugin for this. Once the lights can be controlled by nymea using the raw
GPIO toggle, set up a generic light from this plugin and go to the settings of the
newly added generic light. In there, you'll find a section for Input/Output connections
which can be used to connect the power state of this generic light to the GPIO that
actually controls the light. The light can now be used like any other ready-made light
device in nymea.
