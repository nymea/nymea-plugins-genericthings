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

#include "integrationplugingenericheatingcooling.h"
#include "plugininfo.h"

#include <QDebug>
#include <QtMath>

IntegrationPluginGenericHeatingCooling::IntegrationPluginGenericHeatingCooling()
{

}

void IntegrationPluginGenericHeatingCooling::setupThing(ThingSetupInfo *info)
{
    Thing *thing = info->thing();

    if (thing->thingClassId() == thermostatThingClassId) {
        thermostatCheckPowerOutputState(thing); // check the initial values
        connect(thing, &Thing::settingChanged, thing, [this, thing] (const ParamTypeId &paramTypeId, const QVariant &value) {
            Q_UNUSED(value)
            if (paramTypeId == thermostatSettingsTemperatureDifferenceParamTypeId) {
                thermostatCheckPowerOutputState(thing);
            }
            if (paramTypeId == thermostatSettingsMinTargetTemperatureParamTypeId) {
                thing->setStateMinValue(thermostatTargetTemperatureStateTypeId, value);
            }
            if (paramTypeId == thermostatSettingsMaxTargetTemperatureParamTypeId) {
                thing->setStateMaxValue(thermostatTargetTemperatureStateTypeId, value);
            }
        });
    } else if (thing->thingClassId() == sgReadyThingClassId) {
        bool relay1 = thing->stateValue(sgReadyRelay1StateTypeId).toBool();
        bool relay2 = thing->stateValue(sgReadyRelay2StateTypeId).toBool();
        QString operatingModeString = sgReadyOperatingMode(relay1, relay2);
        thing->setStateValue(sgReadySgReadyModeStateTypeId, operatingModeString);
        thing->setStateValue(sgReadyOperatingModeDescriptionStateTypeId, sgReadyOperatingModeDescription(operatingModeString));
    }

    info->finish(Thing::ThingErrorNoError);
}

void IntegrationPluginGenericHeatingCooling::executeAction(ThingActionInfo *info)
{
    Thing *thing = info->thing();
    Action action = info->action();

    if (thing->thingClassId() == heatingThingClassId) {
        if (action.actionTypeId() == heatingPowerActionTypeId) {
            thing->setStateValue(heatingPowerStateTypeId, action.param(heatingPowerActionPowerParamTypeId).value());
            return info->finish(Thing::ThingErrorNoError);
        } else {
            Q_ASSERT_X(false, "executeAction", QString("Unhandled actionTypeId: %1").arg(action.actionTypeId().toString()).toUtf8());
        }
    } else if (thing->thingClassId() == ventilationThingClassId) {
        if (action.actionTypeId() == ventilationPowerActionTypeId) {
            thing->setStateValue(ventilationPowerStateTypeId, action.param(ventilationPowerActionPowerParamTypeId).value());
            info->finish(Thing::ThingErrorNoError);
            return;
        } else {
            Q_ASSERT_X(false, "executeAction", QString("Unhandled actionTypeId: %1").arg(action.actionTypeId().toString()).toUtf8());
        }
    } else if (thing->thingClassId() == ventilationFlowThingClassId) {
        if (action.actionTypeId() == ventilationFlowPowerActionTypeId) {
            thing->setStateValue(ventilationFlowPowerStateTypeId, action.param(ventilationFlowPowerActionPowerParamTypeId).value());
            info->finish(Thing::ThingErrorNoError);
            return;
        } else if (action.actionTypeId() == ventilationFlowFlowRateActionTypeId) {
            thing->setStateValue(ventilationFlowFlowRateStateTypeId, action.param(ventilationFlowFlowRateActionFlowRateParamTypeId).value());
            info->finish(Thing::ThingErrorNoError);
            return;
        } else {
            Q_ASSERT_X(false, "executeAction", QString("Unhandled actionTypeId: %1").arg(action.actionTypeId().toString()).toUtf8());
        }

    } else if (thing->thingClassId() == thermostatThingClassId) {
        if (action.actionTypeId() == thermostatTemperatureActionTypeId) {
            thing->setStateValue(thermostatTemperatureStateTypeId, action.param(thermostatTemperatureActionTemperatureParamTypeId).value());
        } else if (action.actionTypeId() == thermostatTargetTemperatureActionTypeId) {
            double minSetting = thing->setting(thermostatSettingsMinTargetTemperatureParamTypeId).toDouble();
            double maxSetting = thing->setting(thermostatSettingsMaxTargetTemperatureParamTypeId).toDouble();
            double newTemp = action.param(thermostatTargetTemperatureActionTargetTemperatureParamTypeId).value().toDouble();
            newTemp = qMax(newTemp, minSetting);
            newTemp = qMin(newTemp, maxSetting);
            thing->setStateValue(thermostatTargetTemperatureStateTypeId, newTemp);
        } else if (action.actionTypeId() == thermostatHeatingOnActionTypeId) {
            thing->setStateValue(thermostatHeatingOnStateTypeId, action.param(thermostatHeatingOnActionHeatingOnParamTypeId).value());
        } else if (action.actionTypeId() == thermostatCoolingOnActionTypeId) {
            thing->setStateValue(thermostatCoolingOnStateTypeId, action.param(thermostatCoolingOnActionCoolingOnParamTypeId).value());
        } else {
            Q_ASSERT_X(false, "executeAction", QString("Unhandled actionTypeId: %1").arg(action.actionTypeId().toString()).toUtf8());
        }
        thermostatCheckPowerOutputState(thing);
        info->finish(Thing::ThingErrorNoError);
        return;

    } else if (thing->thingClassId() == heatingThingClassId) {
        if (action.actionTypeId() == heatingPowerActionTypeId) {
            thing->setStateValue(heatingPowerStateTypeId, action.paramValue(heatingPowerActionPowerParamTypeId).toBool());
        } else {
            Q_ASSERT_X(false, "executeAction", QString("Unhandled actionTypeId: %1").arg(action.actionTypeId().toString()).toUtf8());
        }
        info->finish(Thing::ThingErrorNoError);
        return;

    } else if (thing->thingClassId() == coolingThingClassId) {
        if (action.actionTypeId() == coolingPowerActionTypeId) {
            thing->setStateValue(coolingPowerStateTypeId, action.paramValue(coolingPowerActionPowerParamTypeId).toBool());
        } else {
            Q_ASSERT_X(false, "executeAction", QString("Unhandled actionTypeId: %1").arg(action.actionTypeId().toString()).toUtf8());
        }
        info->finish(Thing::ThingErrorNoError);
        return;

    } else if (thing->thingClassId() == sgReadyThingClassId) {
        if (action.actionTypeId() == sgReadyRelay1ActionTypeId) {
            thing->setStateValue(sgReadyRelay1StateTypeId, action.param(sgReadyRelay1ActionRelay1ParamTypeId).value());
            QString operatingMode = sgReadyOperatingMode(thing->stateValue(sgReadyRelay1StateTypeId).toBool(), thing->stateValue(sgReadyRelay2StateTypeId).toBool());
            thing->setStateValue(sgReadySgReadyModeStateTypeId, operatingMode);
            thing->setStateValue(sgReadyOperatingModeDescriptionStateTypeId, sgReadyOperatingModeDescription(operatingMode));
            info->finish(Thing::ThingErrorNoError);
            return;
        }
        if (action.actionTypeId() == sgReadyRelay2ActionTypeId) {
            thing->setStateValue(sgReadyRelay2StateTypeId, action.param(sgReadyRelay2ActionRelay2ParamTypeId).value());
            QString operatingMode = sgReadyOperatingMode(thing->stateValue(sgReadyRelay1StateTypeId).toBool(), thing->stateValue(sgReadyRelay2StateTypeId).toBool());
            thing->setStateValue(sgReadySgReadyModeStateTypeId, operatingMode);
            thing->setStateValue(sgReadyOperatingModeDescriptionStateTypeId, sgReadyOperatingModeDescription(operatingMode));
            info->finish(Thing::ThingErrorNoError);
            return;
        }
        if (action.actionTypeId() == sgReadySgReadyModeActionTypeId) {
            QString operatingMode = action.paramValue(sgReadySgReadyModeActionSgReadyModeParamTypeId).toString();
            thing->setStateValue(sgReadySgReadyModeStateTypeId, operatingMode);
            thing->setStateValue(sgReadyOperatingModeDescriptionStateTypeId, sgReadyOperatingModeDescription(operatingMode));
            if (operatingMode == "Off") {
                thing->setStateValue(sgReadyRelay1StateTypeId, true);
                thing->setStateValue(sgReadyRelay2StateTypeId, false);
            } else if (operatingMode == "Low") {
                thing->setStateValue(sgReadyRelay1StateTypeId, false);
                thing->setStateValue(sgReadyRelay2StateTypeId, false);
            } else if (operatingMode == "Standard") {
                thing->setStateValue(sgReadyRelay1StateTypeId, false);
                thing->setStateValue(sgReadyRelay2StateTypeId, true);
            } else if (operatingMode == "High") {
                thing->setStateValue(sgReadyRelay1StateTypeId, true);
                thing->setStateValue(sgReadyRelay2StateTypeId, true);
            }
            info->finish(Thing::ThingErrorNoError);
            return;
        }
        Q_ASSERT_X(false, "executeAction", QString("Unhandled actionTypeId: %1").arg(action.actionTypeId().toString()).toUtf8());
    } else if (thing->thingClassId() == simpleHeatPumpThingClassId) {
        if (action.actionTypeId() == simpleHeatPumpPowerActionTypeId) {
            thing->setStateValue(simpleHeatPumpPowerStateTypeId, action.paramValue(simpleHeatPumpPowerActionPowerParamTypeId).toBool());
            info->finish(Thing::ThingErrorNoError);
        }
    } else {
        Q_ASSERT_X(false, "executeAction", QString("Unhandled thingClassId: %1").arg(thing->thingClassId().toString()).toUtf8());
    }
}

void IntegrationPluginGenericHeatingCooling::thermostatCheckPowerOutputState(Thing *thing)
{
    bool autoControl = thing->setting(thermostatSettingsAutoControlParamTypeId).toBool();
    if (!autoControl) {
        return;
    }
    double targetTemperature = thing->stateValue(thermostatTargetTemperatureStateTypeId).toDouble();
    double actualTemperature = thing->stateValue(thermostatTemperatureStateTypeId).toDouble();
    double temperatureDifference = thing->setting(thermostatSettingsTemperatureDifferenceParamTypeId).toDouble();
    if (actualTemperature <= (targetTemperature-temperatureDifference)) {
        thing->setStateValue(thermostatHeatingOnStateTypeId, true);
    } else if (actualTemperature >= targetTemperature) {
        thing->setStateValue(thermostatHeatingOnStateTypeId, false);
    }
    if (actualTemperature >= (targetTemperature+temperatureDifference)) {
        thing->setStateValue(thermostatCoolingOnStateTypeId, true);
    } else if (actualTemperature <= targetTemperature) {
        thing->setStateValue(thermostatCoolingOnStateTypeId, false);
    }
}

QString IntegrationPluginGenericHeatingCooling::sgReadyOperatingModeDescription(const QString &operatingModeString)
{
    if (operatingModeString == "Off") {
        return "Stop heating.";
    } else if (operatingModeString == "Low") {
        return "Normal mode, with partial heat storage filling.";
    } else if (operatingModeString == "Standard") {
        return "Increased room and heat storage temperature.";
    } else if (operatingModeString == "High") {
        return "Start heating.";
    }
    return QString("Unknown operating mode %1").arg(operatingModeString);
}

QString IntegrationPluginGenericHeatingCooling::sgReadyOperatingMode(bool relay1, bool relay2)
{
    if (relay1 && !relay2) {
        /*
          * Operating state 1 (Relay state: 1: 0):
          * This operating state is downward compatible with the often fixed times
          * activated EVU lock and includes a maximum of 2 hours of "hard" lock time.
        */
        return "Off";
    } else if (!relay1 && !relay2) {
        /*
         * Operating state 2 (Relay state: 0: 0):
         * In this circuit, the heat pump runs in energy-efficient normal mode
         * with partial heat storage filling for the maximum two-hour EVU lock.
        */
        return "Low";
    } else if (!relay1 && relay2) {
        /*
         * Operating state 3 (Relay state: 0: 1):
         * In this operating state, the heat pump within the controller runs in amplified mode
         * Operation for space heating and hot water preparation. It's not one
         * definitive start-up command, but a switch-on recommendation according to the current increase.
        */
        return "Standard";
    } else {
        /*
         * Operating state 4 (Relay state 1: 1):
         * This is a definitive start-up command, insofar as this is possible within the framework of the rule settings.
         * For this operating state, different control models must be set on the controller for different tariff and usage models:
         * Variant 1: The heat pump (compressor) is actively switched on.
         * Variant 2: The heat pump (compressor and electrical auxiliary heating) is actively switched on, optional: higher temperature in the heat storage
        */
        return "High";
    }
}
