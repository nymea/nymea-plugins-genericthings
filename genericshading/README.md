# Generic shading devices

This plugin allows to create generic shading devices in nymea. Such things don't have any
functionality on their own, but allow to create a visually appealing experience when
using general purpose input and outputs, such as GPIOs.

## Set-up

First, set up a thing with generic inputs or outputs. For instance from the GPIO plugin.
When done, set up the desired generic thing and use the things settings to connect
the inputs and/or outputs accordingly.

### Example: A blind on digital output GPIOs

First, set up the GPIOs to control the blind. Please refer to the documentation of the
according plugin for this. Once the blind can be controlled by nymea using the raw
GPIO toggles, set up a generic blind from this plugin and go to the settings of the
newly added generic blind. In there, you'll find a section for Input/Output connections
which can be used to connect the up/down states of this generic light to the GPIOs that
actually control the blind. The blind can now be used like any other ready-made blind
in nymea.
