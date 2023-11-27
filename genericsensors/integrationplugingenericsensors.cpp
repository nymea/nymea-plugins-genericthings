/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*
*
* Copyright 2013 - 2020, nymea GmbH
* Contact: contact@nymea.io

* This file is part of nymea.
* This project including source code and documentation is protected by
* copyright law, and remains the property of nymea GmbH. All rights, including
* reproduction, publication, editing and translation, are reserved. The use of
* this project is subject to the terms of a license agreement to be concluded
* with nymea GmbH in accordance with the terms of use of nymea GmbH, available
* under https://nymea.io/license
*
* GNU Lesser General Public License Usage
* Alternatively, this project may be redistributed and/or modified under the
* terms of the GNU Lesser General Public License as published by the Free
* Software Foundation; version 3. This project is distributed in the hope that
* it will be useful, but WITHOUT ANY WARRANTY; without even the implied
* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this project. If not, see <https://www.gnu.org/licenses/>.
*
* For any further details and any questions please contact us under
* contact@nymea.io or see our FAQ/Licensing Information on
* https://nymea.io/license/faq
*
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "integrationplugingenericsensors.h"
#include "plugininfo.h"

#include <QDebug>
#include <QtMath>

IntegrationPluginGenericSensors::IntegrationPluginGenericSensors()
{

}

void IntegrationPluginGenericSensors::setupThing(ThingSetupInfo *info)
{
    Thing *thing = info->thing();

    if (thing->thingClassId() == temperatureSensorThingClassId) {
        connect(thing, &Thing::settingChanged, thing, [thing](const ParamTypeId &settingTypeId, const QVariant &value){
            if (settingTypeId == temperatureSensorSettingsMinTempParamTypeId) {
                thing->setStateMinValue(temperatureSensorTemperatureStateTypeId, value);
            }
            if (settingTypeId == temperatureSensorSettingsMaxTempParamTypeId) {
                thing->setStateMaxValue(temperatureSensorTemperatureStateTypeId, value);
            }
        });
    }
    if (thing->thingClassId() == orpSensorThingClassId) {
        connect(thing, &Thing::settingChanged, thing, [thing](const ParamTypeId &settingTypeId, const QVariant &value){
            if (settingTypeId == orpSensorSettingsMinORPParamTypeId) {
                thing->setStateMinValue(orpSensorOrpStateTypeId, value);
            }
            if (settingTypeId == orpSensorSettingsMaxORPParamTypeId) {
                thing->setStateMaxValue(orpSensorOrpStateTypeId, value);
            }
        });
    }
    if (thing->thingClassId() == pressureSensorThingClassId) {
        connect(thing, &Thing::settingChanged, thing, [thing](const ParamTypeId &settingTypeId, const QVariant &value){
            if (settingTypeId == pressureSensorSettingsMinPressureParamTypeId) {
                thing->setStateMinValue(pressureSensorPressureStateTypeId, value);
            }
            if (settingTypeId == pressureSensorSettingsMaxPressureParamTypeId) {
                thing->setStateMaxValue(pressureSensorPressureStateTypeId, value);
            }
        });
    }
    if (thing->thingClassId() == coSensorThingClassId) {
        connect(thing, &Thing::settingChanged, thing, [thing](const ParamTypeId &settingTypeId, const QVariant &value){
            if (settingTypeId == coSensorSettingsMinCOParamTypeId) {
                thing->setStateMinValue(coSensorCoStateTypeId, value);
            }
            if (settingTypeId == coSensorSettingsMaxCOParamTypeId) {
                thing->setStateMaxValue(coSensorCoStateTypeId, value);
            }
        });
    }
    if (thing->thingClassId() == co2SensorThingClassId) {
        connect(thing, &Thing::settingChanged, thing, [thing](const ParamTypeId &settingTypeId, const QVariant &value){
            if (settingTypeId == co2SensorSettingsMinCO2ParamTypeId) {
                thing->setStateMinValue(co2SensorCo2StateTypeId, value);
            }
            if (settingTypeId == co2SensorSettingsMaxCO2ParamTypeId) {
                thing->setStateMaxValue(co2SensorCo2StateTypeId, value);
            }
        });
    }
    if (thing->thingClassId() == gasSensorThingClassId) {
        connect(thing, &Thing::settingChanged, thing, [thing](const ParamTypeId &settingTypeId, const QVariant &value){
            if (settingTypeId == gasSensorSettingsMinValueParamTypeId) {
                thing->setStateMinValue(gasSensorGasLevelStateTypeId, value);
            }
            if (settingTypeId == gasSensorSettingsMaxValueParamTypeId) {
                thing->setStateMaxValue(gasSensorGasLevelStateTypeId, value);
            }
        });
    }
    if (thing->thingClassId() == lightSensorThingClassId) {
        connect(thing, &Thing::settingChanged, thing, [thing](const ParamTypeId &settingTypeId, const QVariant &value){
            if (settingTypeId == lightSensorSettingsMinLuxParamTypeId) {
                thing->setStateMinValue(lightSensorLightIntensityStateTypeId, value);
            }
            if (settingTypeId == lightSensorSettingsMaxLuxParamTypeId) {
                thing->setStateMaxValue(lightSensorLightIntensityStateTypeId, value);
            }
        });
    }
    if (thing->thingClassId() == waterLevelSensorThingClassId) {
        connect(thing, &Thing::settingChanged, thing, [ thing](const ParamTypeId &settingTypeId, const QVariant &value){
            if (settingTypeId == waterLevelSensorSettingsCapacityParamTypeId) {
                double capacity = value.toDouble();
                double input = thing->stateValue(waterLevelSensorInputStateTypeId).toDouble();
                double minInputValue = thing->setting(waterLevelSensorSettingsMinInputValueParamTypeId).toDouble();
                double maxInputValue = thing->setting(waterLevelSensorSettingsMaxInputValueParamTypeId).toDouble();
                double normalizedInput = (input - minInputValue) / (maxInputValue - minInputValue);
                double waterLevel = normalizedInput * capacity;
                thing->setStateMaxValue(waterLevelSensorWaterLevelStateTypeId, capacity);
                thing->setStateValue(waterLevelSensorWaterLevelStateTypeId, waterLevel);
            }
        });
    }

    info->finish(Thing::ThingErrorNoError);
}

void IntegrationPluginGenericSensors::executeAction(ThingActionInfo *info)
{
    Thing *thing = info->thing();
    Action action = info->action();

    if (thing->thingClassId() == temperatureSensorThingClassId) {
        if (action.actionTypeId() == temperatureSensorTemperatureActionTypeId) {
            double value = info->action().param(temperatureSensorTemperatureActionTemperatureParamTypeId).value().toDouble();
            thing->setStateValue(temperatureSensorTemperatureStateTypeId, value);
            info->finish(Thing::ThingErrorNoError);
            return;
        } else if (action.actionTypeId() == temperatureSensorInputActionTypeId) {
            double value = info->action().param(temperatureSensorInputActionInputParamTypeId).value().toDouble();
            thing->setStateValue(temperatureSensorInputStateTypeId, value);
            double inputMin = info->thing()->setting(temperatureSensorSettingsMinInputParamTypeId).toDouble();
            double inputMax = info->thing()->setting(temperatureSensorSettingsMaxInputParamTypeId).toDouble();
            double tempMin = info->thing()->setting(temperatureSensorSettingsMinTempParamTypeId).toDouble();
            double tempMax = info->thing()->setting(temperatureSensorSettingsMaxTempParamTypeId).toDouble();
            double newValue = mapDoubleValue(value, inputMin, inputMax, tempMin, tempMax);
            double roundingFactor = qPow(10, info->thing()->setting(temperatureSensorSettingsAccuracyParamTypeId).toInt());
            newValue = qRound(newValue * roundingFactor) / roundingFactor;
            thing->setStateValue(temperatureSensorTemperatureStateTypeId, newValue);
            info->finish(Thing::ThingErrorNoError);
            return;
        } else {
            Q_ASSERT_X(false, "executeAction", QString("Unhandled actionTypeId: %1").arg(action.actionTypeId().toString()).toUtf8());
        }
    } else if (thing->thingClassId() == humiditySensorThingClassId) {
        if (action.actionTypeId() == humiditySensorHumidityActionTypeId) {
            double value = info->action().param(humiditySensorHumidityActionHumidityParamTypeId).value().toDouble();
            thing->setStateValue(humiditySensorHumidityStateTypeId, value);
            info->finish(Thing::ThingErrorNoError);
            return;
        } else if (action.actionTypeId() == humiditySensorInputActionTypeId) {
            double value = info->action().param(humiditySensorInputActionInputParamTypeId).value().toDouble();
            thing->setStateValue(humiditySensorInputStateTypeId, value);
            double inputMin = info->thing()->setting(humiditySensorSettingsMinInputParamTypeId).toDouble();
            double inputMax = info->thing()->setting(humiditySensorSettingsMaxInputParamTypeId).toDouble();
            double newValue = mapDoubleValue(value, inputMin, inputMax, 0, 100);
            double roundingFactor = qPow(10, info->thing()->setting(humiditySensorSettingsAccuracyParamTypeId).toInt());
            newValue = qRound(newValue * roundingFactor) / roundingFactor;
            thing->setStateValue(humiditySensorHumidityStateTypeId, newValue);
            info->finish(Thing::ThingErrorNoError);
            return;
        } else {
            Q_ASSERT_X(false, "executeAction", QString("Unhandled actionTypeId: %1").arg(action.actionTypeId().toString()).toUtf8());
        }
    } else if (thing->thingClassId() == moistureSensorThingClassId) {
        if (action.actionTypeId() == moistureSensorMoistureActionTypeId) {
            double value = info->action().param(moistureSensorInputActionInputParamTypeId).value().toDouble();
            thing->setStateValue(moistureSensorMoistureStateTypeId, value);
            info->finish(Thing::ThingErrorNoError);
            return;
        } else if (action.actionTypeId() == moistureSensorInputActionTypeId) {
            double value = info->action().param(moistureSensorInputActionInputParamTypeId).value().toDouble();
            thing->setStateValue(moistureSensorInputStateTypeId, value);
            double inputMin = info->thing()->setting(moistureSensorSettingsMinInputParamTypeId).toDouble();
            double inputMax = info->thing()->setting(moistureSensorSettingsMaxInputParamTypeId).toDouble();
            double newValue = mapDoubleValue(value, inputMin, inputMax, 0, 100);
            double roundingFactor = qPow(10, info->thing()->setting(moistureSensorSettingsAccuracyParamTypeId).toInt());
            newValue = qRound(newValue * roundingFactor) / roundingFactor;
            thing->setStateValue(moistureSensorMoistureStateTypeId, newValue);
            info->finish(Thing::ThingErrorNoError);
            return;
        } else {
            Q_ASSERT_X(false, "executeAction", QString("Unhandled actionTypeId: %1").arg(action.actionTypeId().toString()).toUtf8());
        }
    } else if (thing->thingClassId() == phSensorThingClassId) {
        if (action.actionTypeId() == phSensorPhActionTypeId) {
            double value = info->action().paramValue(phSensorInputActionInputParamTypeId).toDouble();
            thing->setStateValue(phSensorPhStateTypeId, value);
            info->finish(Thing::ThingErrorNoError);
            return;
        } else if (action.actionTypeId() == phSensorInputActionTypeId) {
            double value = info->action().paramValue(phSensorInputActionInputParamTypeId).toDouble();
            thing->setStateValue(phSensorInputStateTypeId, value);
            double min = info->thing()->setting(phSensorSettingsMinInputParamTypeId).toDouble();
            double max = info->thing()->setting(phSensorSettingsMaxInputParamTypeId).toDouble();
            double newValue = mapDoubleValue(value, min, max, 0, 14);
            double roundingFactor = qPow(10, info->thing()->setting(phSensorSettingsAccuracyParamTypeId).toInt());
            newValue = qRound(newValue * roundingFactor) / roundingFactor;
            thing->setStateValue(phSensorPhStateTypeId, newValue);
            info->finish(Thing::ThingErrorNoError);
            return;
        }

    } else if (thing->thingClassId() == orpSensorThingClassId) {
        if (action.actionTypeId() == orpSensorOrpActionTypeId) {
            double value = info->action().paramValue(orpSensorInputActionInputParamTypeId).toDouble();
            thing->setStateValue(orpSensorOrpStateTypeId, value);
            info->finish(Thing::ThingErrorNoError);
            return;
        } else if (action.actionTypeId() == orpSensorInputActionTypeId) {
            double value = info->action().paramValue(orpSensorInputActionInputParamTypeId).toDouble();
            thing->setStateValue(orpSensorInputStateTypeId, value);
            double minInput = info->thing()->setting(orpSensorSettingsMinInputParamTypeId).toDouble();
            double maxInput = info->thing()->setting(orpSensorSettingsMaxInputParamTypeId).toDouble();
            double minOrp = info->thing()->setting(orpSensorSettingsMinORPParamTypeId).toDouble();
            double maxOrp = info->thing()->setting(orpSensorSettingsMaxORPParamTypeId).toDouble();
            double newValue = mapDoubleValue(value, minInput, maxInput, minOrp, maxOrp);
            double roundingFactor = qPow(10, info->thing()->setting(orpSensorSettingsAccuracyParamTypeId).toInt());
            newValue = qRound(newValue * roundingFactor) / roundingFactor;
            thing->setStateValue(orpSensorOrpStateTypeId, newValue);
            info->finish(Thing::ThingErrorNoError);
            return;
        }
    } else if (thing->thingClassId() == o2SensorThingClassId) {
        if (action.actionTypeId() == o2SensorO2saturationActionTypeId) {
            double value = info->action().paramValue(o2SensorO2saturationActionO2saturationParamTypeId).toDouble();
            thing->setStateValue(o2SensorO2saturationStateTypeId, value);
            info->finish(Thing::ThingErrorNoError);
            return;
        } else if (action.actionTypeId() == o2SensorInputActionTypeId) {
            double value = info->action().paramValue(o2SensorInputActionInputParamTypeId).toDouble();
            thing->setStateValue(o2SensorInputStateTypeId, value);
            double minInput = info->thing()->setting(o2SensorSettingsMinInputParamTypeId).toDouble();
            double maxInput = info->thing()->setting(o2SensorSettingsMaxInputParamTypeId).toDouble();
            double newValue = mapDoubleValue(value, minInput, maxInput, 0, 100);
            double roundingFactor = qPow(10, info->thing()->setting(o2SensorSettingsAccuracyParamTypeId).toInt());
            newValue = qRound(newValue * roundingFactor) / roundingFactor;
            thing->setStateValue(o2SensorO2saturationStateTypeId, newValue);
            info->finish(Thing::ThingErrorNoError);
            return;
        }
    } else if (thing->thingClassId() == pressureSensorThingClassId) {
        if (action.actionTypeId() == pressureSensorPressureActionTypeId) {
            double value = info->action().param(pressureSensorPressureActionPressureParamTypeId).value().toDouble();
            thing->setStateValue(pressureSensorPressureStateTypeId, value);
            info->finish(Thing::ThingErrorNoError);
            return;
        } else if (action.actionTypeId() == pressureSensorInputActionTypeId) {
            double value = info->action().param(pressureSensorInputActionInputParamTypeId).value().toDouble();
            thing->setStateValue(pressureSensorInputStateTypeId, value);
            double minInput = info->thing()->setting(pressureSensorSettingsMinInputParamTypeId).toDouble();
            double maxInput = info->thing()->setting(pressureSensorSettingsMaxInputParamTypeId).toDouble();
            double minPressure = info->thing()->setting(pressureSensorSettingsMinPressureParamTypeId).toDouble();
            double maxPressure = info->thing()->setting(pressureSensorSettingsMaxPressureParamTypeId).toDouble();
            double newValue = mapDoubleValue(value, minInput, maxInput, minPressure, maxPressure);
            double roundingFactor = qPow(10, info->thing()->setting(pressureSensorSettingsAccuracyParamTypeId).toInt());
            newValue = qRound(newValue * roundingFactor) / roundingFactor;
            thing->setStateValue(pressureSensorPressureStateTypeId, newValue);
            info->finish(Thing::ThingErrorNoError);
            return;
        } else {
            Q_ASSERT_X(false, "executeAction", QString("Unhandled actionTypeId: %1").arg(action.actionTypeId().toString()).toUtf8());
        }
    } else if (thing->thingClassId() == coSensorThingClassId) {
        if (action.actionTypeId() == coSensorCoActionTypeId) {
            double value = info->action().param(coSensorCoActionCoParamTypeId).value().toDouble();
            thing->setStateValue(coSensorCoStateTypeId, value);
            info->finish(Thing::ThingErrorNoError);
            return;
        } else if (action.actionTypeId() == coSensorInputActionTypeId) {
            double value = info->action().param(coSensorInputActionInputParamTypeId).value().toDouble();
            thing->setStateValue(coSensorInputStateTypeId, value);
            double minInput = info->thing()->setting(coSensorSettingsMinInputParamTypeId).toDouble();
            double maxInput = info->thing()->setting(coSensorSettingsMaxInputParamTypeId).toDouble();
            double minCo = info->thing()->setting(coSensorSettingsMinCOParamTypeId).toDouble();
            double maxCo = info->thing()->setting(coSensorSettingsMaxCOParamTypeId).toDouble();
            double newValue = mapDoubleValue(value, minInput, maxInput, minCo, maxCo);
            double roundingFactor = qPow(10, info->thing()->setting(coSensorSettingsAccuracyParamTypeId).toInt());
            newValue = qRound(newValue * roundingFactor) / roundingFactor;
            thing->setStateValue(coSensorCoStateTypeId, newValue);
            info->finish(Thing::ThingErrorNoError);
            return;
        } else {
            Q_ASSERT_X(false, "executeAction", QString("Unhandled actionTypeId: %1").arg(action.actionTypeId().toString()).toUtf8());
        }
    } else if (thing->thingClassId() == co2SensorThingClassId) {
        if (action.actionTypeId() == co2SensorCo2ActionTypeId) {
            double value = info->action().param(co2SensorCo2ActionCo2ParamTypeId).value().toDouble();
            thing->setStateValue(co2SensorCo2StateTypeId, value);
            info->finish(Thing::ThingErrorNoError);
            return;
        } else if (action.actionTypeId() == co2SensorInputActionTypeId) {
            double value = info->action().param(co2SensorInputActionInputParamTypeId).value().toDouble();
            thing->setStateValue(co2SensorInputStateTypeId, value);
            double minInput = info->thing()->setting(co2SensorSettingsMinInputParamTypeId).toDouble();
            double maxInput = info->thing()->setting(co2SensorSettingsMaxInputParamTypeId).toDouble();
            double minCo2 = info->thing()->setting(co2SensorSettingsMinCO2ParamTypeId).toDouble();
            double maxCo2 = info->thing()->setting(co2SensorSettingsMaxCO2ParamTypeId).toDouble();
            double newValue = mapDoubleValue(value, minInput, maxInput, minCo2, maxCo2);
            double roundingFactor = qPow(10, info->thing()->setting(co2SensorSettingsAccuracyParamTypeId).toInt());
            newValue = qRound(newValue * roundingFactor) / roundingFactor;
            thing->setStateValue(co2SensorCo2StateTypeId, newValue);
            info->finish(Thing::ThingErrorNoError);
            return;
        } else {
            Q_ASSERT_X(false, "executeAction", QString("Unhandled actionTypeId: %1").arg(action.actionTypeId().toString()).toUtf8());
        }
    } else if (thing->thingClassId() == gasSensorThingClassId) {
        if (action.actionTypeId() == gasSensorGasLevelActionTypeId) {
            double value = info->action().param(gasSensorGasLevelActionGasLevelParamTypeId).value().toDouble();
            thing->setStateValue(gasSensorGasLevelStateTypeId, value);
            info->finish(Thing::ThingErrorNoError);
            return;
        } else if (action.actionTypeId() == gasSensorInputActionTypeId) {
            double value = info->action().param(gasSensorInputActionInputParamTypeId).value().toDouble();
            thing->setStateValue(gasSensorInputStateTypeId, value);
            double minInput = info->thing()->setting(gasSensorSettingsMinInputParamTypeId).toDouble();
            double maxInput = info->thing()->setting(gasSensorSettingsMaxInputParamTypeId).toDouble();
            double minValue = info->thing()->setting(gasSensorSettingsMinValueParamTypeId).toDouble();
            double maxValue = info->thing()->setting(gasSensorSettingsMaxValueParamTypeId).toDouble();
            double newValue = mapDoubleValue(value, minInput, maxInput, minValue, maxValue);
            double roundingFactor = qPow(10, info->thing()->setting(gasSensorSettingsAccuracyParamTypeId).toInt());
            newValue = qRound(newValue * roundingFactor) / roundingFactor;
            thing->setStateValue(gasSensorGasLevelStateTypeId, newValue);
            info->finish(Thing::ThingErrorNoError);
            return;
        } else {
            Q_ASSERT_X(false, "executeAction", QString("Unhandled actionTypeId: %1").arg(action.actionTypeId().toString()).toUtf8());
        }
    } else if (thing->thingClassId() == lightSensorThingClassId) {
        if (action.actionTypeId() == lightSensorLightIntensityStateTypeId) {
            double value = info->action().param(lightSensorLightIntensityStateTypeId).value().toDouble();
            thing->setStateValue(lightSensorLightIntensityStateTypeId, value);
            info->finish(Thing::ThingErrorNoError);
            return;
        } else if (action.actionTypeId() == lightSensorInputActionTypeId) {
            double value = info->action().param(lightSensorInputActionInputParamTypeId).value().toDouble();
            thing->setStateValue(lightSensorInputStateTypeId, value);
            double minInput = info->thing()->setting(lightSensorSettingsMinInputParamTypeId).toDouble();
            double maxInput = info->thing()->setting(lightSensorSettingsMaxInputParamTypeId).toDouble();
            double minValue = info->thing()->setting(lightSensorSettingsMinLuxParamTypeId).toDouble();
            double maxValue = info->thing()->setting(lightSensorSettingsMaxLuxParamTypeId).toDouble();
            double newValue = mapDoubleValue(value, minInput, maxInput, minValue, maxValue);
            double roundingFactor = qPow(10, info->thing()->setting(lightSensorSettingsAccuracyParamTypeId).toInt());
            newValue = qRound(newValue * roundingFactor) / roundingFactor;
            thing->setStateValue(lightSensorLightIntensityStateTypeId, newValue);
            info->finish(Thing::ThingErrorNoError);
            return;
        } else {
            Q_ASSERT_X(false, "executeAction", QString("Unhandled actionTypeId: %1").arg(action.actionTypeId().toString()).toUtf8());
        }
    } else if (thing->thingClassId() == doorSensorThingClassId) {
        if (action.actionTypeId() == doorSensorClosedActionTypeId) {
            bool closed = action.paramValue(doorSensorClosedActionClosedParamTypeId).toBool();
            qCDebug(dcGenericSensors()) << "Door sensor is now" << (closed ? "open" : "closed");
            thing->setStateValue(doorSensorClosedStateTypeId, closed);
            info->finish(Thing::ThingErrorNoError);
            return;
        }
    } else if (thing->thingClassId() == waterSensorThingClassId) {
        if (action.actionTypeId() == waterSensorWaterDetectedActionTypeId) {
            bool waterDetected = action.paramValue(waterSensorWaterDetectedActionWaterDetectedParamTypeId).toBool();
            qCDebug(dcGenericSensors()).nospace() << "Water sensor is " << (waterDetected ? "" : "not") << "detecting water";
            thing->setStateValue(waterSensorWaterDetectedStateTypeId, waterDetected);
            info->finish(Thing::ThingErrorNoError);
            return;
        }
    } else if (thing->thingClassId() == waterLevelSensorThingClassId) {
        if (action.actionTypeId() == waterLevelSensorInputActionTypeId) {
            double capacity = thing->setting(waterLevelSensorSettingsCapacityParamTypeId).toDouble();
            double input = action.paramValue(waterLevelSensorInputActionInputParamTypeId).toDouble();
            double minInputValue = thing->setting(waterLevelSensorSettingsMinInputValueParamTypeId).toDouble();
            double maxInputValue = thing->setting(waterLevelSensorSettingsMaxInputValueParamTypeId).toDouble();
            double normalizedInput = (input - minInputValue) / (maxInputValue - minInputValue);
            double waterLevel = normalizedInput * capacity;
            thing->setStateValue(waterLevelSensorWaterLevelStateTypeId, waterLevel);
            info->finish(Thing::ThingErrorNoError);
            return;
        }
    } else if (thing->thingClassId() == presenceSensorThingClassId) {
        if (action.actionTypeId() == presenceSensorIsPresentActionTypeId) {
            bool isPresent = action.paramValue(presenceSensorIsPresentActionIsPresentParamTypeId).toBool();
            qCDebug(dcGenericSensors()) << "Presence sensor is now" << (isPresent ? "present" : "not present");
            thing->setStateValue(presenceSensorIsPresentStateTypeId, isPresent);
            if (isPresent) {
                thing->setStateValue(presenceSensorLastSeenTimeStateTypeId, QDateTime::currentDateTime().toMSecsSinceEpoch() / 1000);
            }
            info->finish(Thing::ThingErrorNoError);
            return;
        }
    } else if (thing->thingClassId() == fireSensorThingClassId) {
        if (action.actionTypeId() == fireSensorFireDetectedActionTypeId) {
            bool fire = action.paramValue(fireSensorFireDetectedActionFireDetectedParamTypeId).toBool();
            qCDebug(dcGenericSensors()) << "Fire sensor state:" << (fire ? "fire" : "no fire");
            thing->setStateValue(fireSensorFireDetectedStateTypeId, fire);
            info->finish(Thing::ThingErrorNoError);
            return;
        }
    } else if (thing->thingClassId() == vibrationSensorThingClassId) {
        if (action.actionTypeId() == vibrationSensorTriggerVibrationAlertActionTypeId) {
            thing->emitEvent(vibrationSensorVibrationDetectedEventTypeId);
        }
    } else {
        Q_ASSERT_X(false, "executeAction", QString("Unhandled thingClassId: %1").arg(thing->thingClassId().toString()).toUtf8());
    }
}

double IntegrationPluginGenericSensors::mapDoubleValue(double value, double fromMin, double fromMax, double toMin, double toMax)
{
    double percent = (value - fromMin) / (fromMax - fromMin);
    double toValue = toMin + (toMax - toMin) * percent;
    return toValue;
}
