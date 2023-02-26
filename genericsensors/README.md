# Generic sensors

This plugin allows to create generic sensors in nymea. Such things don't have any
functionality on their own, but allow to create a visually appealing experience when
using general purpose input and outputs, such as GPIOs or MQTT.

## Set-up by connecting Generic inputs

* Set up a sensor with generic inputs. For instance from a GPIO or an I2C device.
* Set up the desired generc thing
* Use the generic things settings to connect it to the device in the Generic Input/Outputs section.
* Use the min/max settings to calibrate the sensor.

## Set-up by using scripting

* Set up a thing that obtains sensor data in one way or another. For example via the
mqtt client , the HTTP client or the Modbus commander plugin.
* Set up the desired generic thing.
* Create a script obtaining the input value and writing it to the generic sensor thing.

## Examples

### A soil moisture sensor on an analog input GPIO

First, set up the GPIO to receive the analog GPIO values from the sensor. Normally
such an analog input GPIO will dispatch values from 0V to e.g. 3.3V, depending on the
hardware. An input value of 0V normally refers to a soil moisture value of 0%, while
an input value of 3.3V would refer to the maximum, that is 100% soil moisture. Once
those input values are received in nymea, set up a generic soil moisture thing and
go to its settings. In there you'll find the Input/Output connections section which
allows to map this generic soil moisture sensor to the according GPIO input. Once
this connection has been made, the generic soil moisture thing will represent the
input from the GPIO in a nice sensor view.
Some sensors might require calibration in order to display correct values. For instance
a sensor may not support the full range from 0% to 100% and might output
a value of 0V on a minimum soil moisture of e.g. 10% or might only be able to
measure the soil moisture up to, say, 90%. Normally the sensor hardware would give
that information in the data sheet or user guide. If this is the case, the appropriate
input range can also be defined in the generic moisture sensor page.



### A temperature sensor from MQTT
First decide on the MQTT broker to be used. The possibility is to use the nymea
integrated MQTT broker or an external one.

If using the nymea internal mqtt broker, make sure it is enabled in the System Settings
and, if authentication is enabled for the server, a valid client policy is in place.
Then configure the device to publish values to nymea. Then create an "Internal mqtt client"
thing and subscribe to an appropriate topic filter for the device's publishing topic.

If instead the device publishes values to another mqtt broker, create a new "MQTT client"
thing in nymea and provide connection information to the external MQTT broker.

Once a MQTT client thing is set up, create the desired generic sensor thing.

Then create a nymea script which parses the MQTT message and writes the value to the generic
sensor.

```
ThingEvent {
    thingId: "..." // the mqtt client's id
    eventName": "triggered"
    onTriggered: {
        console.log("Publish received from MQTT", JSON.stringify(params))
        temperatureState.value = JSON.parse(params["data"])["temperature"]
    }
}

ThingState {
    id: temperatureState
    thingId: "..." // the generic temperature sensor
    stateName": "temperature"
}
```
