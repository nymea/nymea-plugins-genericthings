# Generic heating & cooling devices

This plugin allows to create generic heating and cooling devices in nymea. Such things don't have any
functionality on their own, but allow to create a visually appealing experience when
using general purpose input and outputs, such as GPIOs.

## Set-up

First, set up a thing with generic inputs or outputs. For instance from the GPIO plugin.
When done, set up the desired generic thing and use the things settings to connect
the inputs and/or outputs accordingly.

### Example: A heating rod on a digital output GPIO

First, set up the GPIO to control the heating rod. Please refer to the documentation of the
according plugin for this. Once the heating rod can be controlled by nymea using the raw
GPIO toggle, set up a generic heating thing from this plugin and go to the settings of the
newly added generic heating thing. In there, you'll find a section for Input/Output connections
which can be used to connect the power state of this generic thing to the GPIO that
actually controls the heating rod. The heating rod can now be used like any other ready-made thing
in nymea.
