# Generic garages

This plugin allows to create generic garages in nymea. Such things don't have any
functionality on their own, but allow to create a visually appealing experience when
using general purpose input and outputs, such as GPIOs.

There are three types of garages supported:

### Impulse based garages
Those garages are used for garage door drivers that have an impulse input which would toggle
opening/closing every time an impulse is sent to the input.

### Simple garage doors
Those garages usually have two inputs. One opening the garage, the other closing it.

## Stateful garage doors
The stateful garage doors thing is made for garages that support more complex interation, for example
via MQTT or a REST api. They can provide the current door position to nymea by setting the percentage
state in the generic stateful garage thing.

## Set-up

First, set up a thing with generic inputs or outputs. For instance from the GPIO plugin or
the USB relay. When done, set up the desired generic garage and use the things settings to connect
the inputs and/or outputs accordingly.

### Example: A simple garage on digital output GPIOs

First, set up two GPIO to control the garage door drivers inputs. Please refer to the documentation of the
according plugin for this. Once the garage door can be controlled by nymea using the raw
GPIO toggle, set up a generic simple garage from this plugin and go to the settings of the
newly added generic simple garage. In there, you'll find a section for Input/Output connections
which can be used to connect the power state of this generic light to the GPIOs that
actually control the garage. The garage can now be used like any other ready-made garage
device in nymea.
