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

#include "integrationplugingenericenergy.h"
#include "plugininfo.h"

#include <QDebug>
#include <QtMath>

IntegrationPluginGenericEnergy::IntegrationPluginGenericEnergy()
{

}

void IntegrationPluginGenericEnergy::setupThing(ThingSetupInfo *info)
{
    Thing *thing = info->thing();

    if (thing->thingClassId() == smartMeterThingClassId) {

        QTimer* smartMeterTimer = new QTimer(this);
        int timeframe = thing->setting(smartMeterSettingsImpulseTimeframeParamTypeId).toInt();
        smartMeterTimer->setInterval(timeframe * 1000);
        m_smartMeterTimer.insert(thing, smartMeterTimer);
        smartMeterTimer->start();
        connect(thing, &Thing::settingChanged, smartMeterTimer, [smartMeterTimer] (const ParamTypeId &paramTypeId, const QVariant &value) {
            if (paramTypeId == smartMeterSettingsImpulseTimeframeParamTypeId) {
                smartMeterTimer->setInterval(value.toInt() * 1000);
            }
        });

        connect(smartMeterTimer, &QTimer::timeout, thing, [this, smartMeterTimer, thing] {
            double impulsePerKwh = thing->setting(smartMeterSettingsImpulsePerKwhParamTypeId).toDouble();
            int interval = smartMeterTimer->interval()/1000;
            double power = (m_pulsesPerTimeframe.value(thing)/impulsePerKwh)/(interval/3600.00); // Power = Energy/Time; Energy = Impulses/ImpPerkWh
            thing->setStateValue(smartMeterCurrentPowerStateTypeId, power*1000);
            m_pulsesPerTimeframe.insert(thing, 0);
        });
    } else if  (thing->thingClassId() == batteryThingClassId) {
        connect(thing, &Thing::settingChanged, [thing](const ParamTypeId &settingTypeId, const QVariant &value){
            if (settingTypeId == batterySettingsCriticalLevelParamTypeId) {
                int currentBatteryLevel = thing->stateValue(batteryBatteryLevelStateTypeId).toInt();
                thing->setStateValue(batteryBatteryCriticalStateTypeId, currentBatteryLevel <= value.toInt());
            }
        });
    }

    // Fall trough, if not already finished and returned...
    info->finish(Thing::ThingErrorNoError);
}

void IntegrationPluginGenericEnergy::executeAction(ThingActionInfo *info)
{
    Thing *thing = info->thing();
    Action action = info->action();

    if (thing->thingClassId() == socketThingClassId) {
        if (action.actionTypeId() == socketPowerActionTypeId) {
            thing->setStateValue(socketPowerStateTypeId, action.param(socketPowerActionPowerParamTypeId).value());
            return info->finish(Thing::ThingErrorNoError);
        } else {
            Q_ASSERT_X(false, "executeAction", QString("Unhandled actionTypeId: %1").arg(action.actionTypeId().toString()).toUtf8());
        }
    } else if (thing->thingClassId() == lightThingClassId) {
        if (action.actionTypeId() == lightPowerActionTypeId) {
            thing->setStateValue(lightPowerStateTypeId, action.param(lightPowerActionPowerParamTypeId).value());
            return info->finish(Thing::ThingErrorNoError);
        } else {
            Q_ASSERT_X(false, "executeAction", QString("Unhandled actionTypeId: %1").arg(action.actionTypeId().toString()).toUtf8());
        }
    } else if (thing->thingClassId() == smartMeterThingClassId) {
        if (action.actionTypeId() == smartMeterImpulseInputActionTypeId) {
            bool value = info->action().param(smartMeterImpulseInputActionImpulseInputParamTypeId).value().toBool();
            thing->setStateValue(smartMeterImpulseInputStateTypeId, value);
            int impulsePerKwh = info->thing()->setting(smartMeterSettingsImpulsePerKwhParamTypeId).toInt();
            if (value) {
                double currentEnergy = thing->stateValue(smartMeterTotalEnergyConsumedStateTypeId).toDouble();
                thing->setStateValue(smartMeterTotalEnergyConsumedStateTypeId ,currentEnergy + (1.00/impulsePerKwh));
                m_pulsesPerTimeframe[thing]++;
            }
            info->finish(Thing::ThingErrorNoError);
            return;
        } else {
            Q_ASSERT_X(false, "executeAction", QString("Unhandled actionTypeId: %1").arg(action.actionTypeId().toString()).toUtf8());
        }

    } else if (thing->thingClassId() == batteryThingClassId) {
        if (action.actionTypeId() == batteryBatteryLevelControlActionTypeId) {
            int value = action.paramValue(batteryBatteryLevelControlActionBatteryLevelControlParamTypeId).toInt();
            thing->setStateValue(batteryBatteryLevelStateTypeId, value);
            thing->setStateValue(batteryBatteryLevelControlStateTypeId, value);
            int criticalValue = thing->setting(batterySettingsCriticalLevelParamTypeId).toInt();
            thing->setStateValue(batteryBatteryCriticalStateTypeId, value <= criticalValue);
            info->finish(Thing::ThingErrorNoError);
            return;
        } else if (action.actionTypeId() == batteryChargingActionTypeId) {
            thing->setStateValue(batteryChargingStateTypeId, action.paramValue(batteryChargingActionChargingParamTypeId));
            info->finish(Thing::ThingErrorNoError);
            return;
        }
    } else {
        Q_ASSERT_X(false, "executeAction", QString("Unhandled thingClassId: %1").arg(thing->thingClassId().toString()).toUtf8());
    }
}

void IntegrationPluginGenericEnergy::thingRemoved(Thing *thing)
{
    if (thing->thingClassId() == smartMeterThingClassId) {
        m_pulsesPerTimeframe.remove(thing);
    } else if (thing->thingClassId() == smartMeterThingClassId) {
        m_smartMeterTimer.take(thing)->deleteLater();
        m_pulsesPerTimeframe.remove(thing);
    }
}
