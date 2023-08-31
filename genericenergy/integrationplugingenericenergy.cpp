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

    if (thing->thingClassId() == impulseSmartMeterThingClassId) {

        QTimer* smartMeterTimer = new QTimer(this);
        int timeframe = thing->setting(impulseSmartMeterSettingsImpulseTimeframeParamTypeId).toInt();
        smartMeterTimer->setInterval(timeframe * 1000);
        m_smartMeterTimer.insert(thing, smartMeterTimer);
        smartMeterTimer->start();
        connect(thing, &Thing::settingChanged, smartMeterTimer, [smartMeterTimer] (const ParamTypeId &paramTypeId, const QVariant &value) {
            if (paramTypeId == impulseSmartMeterSettingsImpulseTimeframeParamTypeId) {
                smartMeterTimer->setInterval(value.toInt() * 1000);
            }
        });

        connect(smartMeterTimer, &QTimer::timeout, thing, [this, smartMeterTimer, thing] {
            double impulsePerKwh = thing->setting(impulseSmartMeterSettingsImpulsePerKwhParamTypeId).toDouble();
            int interval = smartMeterTimer->interval()/1000;
            double power = (m_pulsesPerTimeframe.value(thing)/impulsePerKwh)/(interval/3600.00); // Power = Energy/Time; Energy = Impulses/ImpPerkWh
            thing->setStateValue(impulseSmartMeterCurrentPowerStateTypeId, power*1000);
            m_pulsesPerTimeframe.insert(thing, 0);
        });
    } else if  (thing->thingClassId() == energyStorageThingClassId) {
        connect(thing, &Thing::settingChanged, [thing](const ParamTypeId &settingTypeId, const QVariant &value){
            if (settingTypeId == energyStorageSettingsCapacityParamTypeId) {
                thing->setStateValue(energyStorageCapacityStateTypeId, value);
            }
            if (settingTypeId == energyStorageSettingsCriticalLevelParamTypeId) {
                int currentBatteryLevel = thing->stateValue(energyStorageBatteryLevelStateTypeId).toInt();
                thing->setStateValue(energyStorageBatteryCriticalStateTypeId, currentBatteryLevel <= value.toInt());
            }
        });
    } else if (thing->thingClassId() == wallboxThingClassId) {
        connect(thing, &Thing::settingChanged, [thing](const ParamTypeId &settingTypeId, const QVariant &value) {
            if (settingTypeId == wallboxSettingsMinChargingCurrentParamTypeId) {
                thing->setStateMinValue(wallboxMaxChargingCurrentStateTypeId, value);
            } else if (settingTypeId == wallboxSettingsMaxChargingCurrentParamTypeId) {
                thing->setStateMaxValue(wallboxMaxChargingCurrentStateTypeId, value);
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
    } else if (thing->thingClassId() == energyMeterThingClassId) {
        if (action.actionTypeId() == energyMeterCurrentPowerActionTypeId) {
            thing->setStateValue(energyMeterCurrentPowerStateTypeId, action.paramValue(energyMeterCurrentPowerActionCurrentPowerParamTypeId));
        } else if (action.actionTypeId() == energyMeterTotalEnergyConsumedActionTypeId) {
            thing->setStateValue(energyMeterTotalEnergyConsumedStateTypeId, action.paramValue(energyMeterTotalEnergyConsumedActionTotalEnergyConsumedParamTypeId));
        } else if (action.actionTypeId() == energyMeterTotalEnergyProducedActionTypeId) {
            thing->setStateValue(energyMeterTotalEnergyProducedStateTypeId, action.paramValue(energyMeterTotalEnergyProducedActionTotalEnergyProducedParamTypeId));
        } else if (action.actionTypeId() == energyMeterCurrentPhaseAActionTypeId) {
            thing->setStateValue(energyMeterCurrentPhaseAStateTypeId, action.paramValue(energyMeterCurrentPhaseAActionCurrentPhaseAParamTypeId));
        } else if (action.actionTypeId() == energyMeterVoltagePhaseAActionTypeId) {
            thing->setStateValue(energyMeterVoltagePhaseAStateTypeId, action.paramValue(energyMeterVoltagePhaseAActionVoltagePhaseAParamTypeId));
        }
        info->finish(Thing::ThingErrorNoError);
    } else if (thing->thingClassId() == impulseSmartMeterThingClassId) {
        if (action.actionTypeId() == impulseSmartMeterImpulseInputActionTypeId) {
            bool value = info->action().param(impulseSmartMeterImpulseInputActionImpulseInputParamTypeId).value().toBool();
            thing->setStateValue(impulseSmartMeterImpulseInputStateTypeId, value);
            int impulsePerKwh = info->thing()->setting(impulseSmartMeterSettingsImpulsePerKwhParamTypeId).toInt();
            if (value) {
                double currentEnergy = thing->stateValue(impulseSmartMeterTotalEnergyConsumedStateTypeId).toDouble();
                thing->setStateValue(impulseSmartMeterTotalEnergyConsumedStateTypeId ,currentEnergy + (1.00/impulsePerKwh));
                m_pulsesPerTimeframe[thing]++;
            }
            info->finish(Thing::ThingErrorNoError);
            return;
        } else {
            Q_ASSERT_X(false, "executeAction", QString("Unhandled actionTypeId: %1").arg(action.actionTypeId().toString()).toUtf8());
        }

    } else if (thing->thingClassId() == smartMeterConsumerThingClassId) {
        if (action.actionTypeId() == smartMeterConsumerCurrentPowerActionTypeId) {
            thing->setStateValue(smartMeterConsumerCurrentPowerStateTypeId, action.paramValue(smartMeterConsumerCurrentPowerActionCurrentPowerParamTypeId));
            info->finish(Thing::ThingErrorNoError);
        } else if (action.actionTypeId() == smartMeterConsumerTotalEnergyConsumedActionTypeId) {
            thing->setStateValue(smartMeterConsumerTotalEnergyConsumedStateTypeId, action.paramValue(smartMeterConsumerTotalEnergyConsumedActionTotalEnergyConsumedParamTypeId));
            info->finish(Thing::ThingErrorNoError);
        } else {
            Q_ASSERT_X(false, "executeAction", QString("Unhandled actionTypeId: %1").arg(action.actionTypeId().toString()).toUtf8());
        }

    } else if (thing->thingClassId() == smartMeterProducerThingClassId) {
        if (action.actionTypeId() == smartMeterProducerCurrentPowerActionTypeId) {
            thing->setStateValue(smartMeterProducerCurrentPowerStateTypeId, action.paramValue(smartMeterProducerCurrentPowerActionCurrentPowerParamTypeId));
            info->finish(Thing::ThingErrorNoError);
        } else if (action.actionTypeId() == smartMeterProducerTotalEnergyProducedActionTypeId) {
            thing->setStateValue(smartMeterProducerTotalEnergyProducedStateTypeId, action.paramValue(smartMeterProducerTotalEnergyProducedActionTotalEnergyProducedParamTypeId));
            info->finish(Thing::ThingErrorNoError);
        } else {
            Q_ASSERT_X(false, "executeAction", QString("Unhandled actionTypeId: %1").arg(action.actionTypeId().toString()).toUtf8());
        }
    } else if (thing->thingClassId() == energyStorageThingClassId) {
        if (action.actionTypeId() == energyStorageBatteryLevelActionTypeId) {
            int value = action.paramValue(energyStorageBatteryLevelActionBatteryLevelParamTypeId).toInt();
            thing->setStateValue(energyStorageBatteryLevelStateTypeId, value);
            int criticalValue = thing->setting(energyStorageSettingsCriticalLevelParamTypeId).toInt();
            thing->setStateValue(energyStorageBatteryCriticalStateTypeId, value <= criticalValue);
            info->finish(Thing::ThingErrorNoError);
        } else if (action.actionTypeId() == energyStorageCurrentPowerActionTypeId) {
            thing->setStateValue(energyStorageCurrentPowerStateTypeId, action.paramValue(energyStorageCurrentPowerActionCurrentPowerParamTypeId));
            info->finish(Thing::ThingErrorNoError);
        } else if (action.actionTypeId() == energyStorageChargingActionTypeId) {
            thing->setStateValue(energyStorageChargingStateTypeId, action.paramValue(energyStorageChargingActionChargingParamTypeId));
            info->finish(Thing::ThingErrorNoError);
            return;
        }
    } else if (thing->thingClassId() == wallboxThingClassId) {
        if (action.actionTypeId() == wallboxChargingActionTypeId) {
            thing->setStateValue(wallboxChargingStateTypeId, action.paramValue(wallboxChargingActionChargingParamTypeId).toBool());
        } else if (action.actionTypeId() == wallboxCurrentPowerActionTypeId) {
            thing->setStateValue(wallboxCurrentPowerStateTypeId, action.paramValue(wallboxCurrentPowerActionCurrentPowerParamTypeId).toDouble());
        } else if (action.actionTypeId() == wallboxDesiredPhaseCountActionTypeId) {
            thing->setStateValue(wallboxDesiredPhaseCountStateTypeId, action.paramValue(wallboxDesiredPhaseCountActionDesiredPhaseCountParamTypeId).toUInt());
        } else if (action.actionTypeId() == wallboxMaxChargingCurrentActionTypeId) {
            thing->setStateValue(wallboxMaxChargingCurrentStateTypeId, action.paramValue(wallboxMaxChargingCurrentActionMaxChargingCurrentParamTypeId).toUInt());
        } else if (action.actionTypeId() == wallboxPhaseCountActionTypeId) {
            thing->setStateValue(wallboxPhaseCountStateTypeId, action.paramValue(wallboxPhaseCountActionPhaseCountParamTypeId).toUInt());
        } else if (action.actionTypeId() == wallboxPluggedInActionTypeId) {
            thing->setStateValue(wallboxPluggedInStateTypeId, action.paramValue(wallboxPluggedInActionPluggedInParamTypeId).toBool());
        } else if (action.actionTypeId() == wallboxPowerActionTypeId) {
            thing->setStateValue(wallboxPowerStateTypeId, action.paramValue(wallboxPowerActionPowerParamTypeId).toBool());
        } else if (action.actionTypeId() == wallboxTotalEnergyConsumedActionTypeId) {
            thing->setStateValue(wallboxTotalEnergyConsumedStateTypeId, action.paramValue(wallboxTotalEnergyConsumedActionTotalEnergyConsumedParamTypeId).toDouble());
        } else {
            Q_ASSERT_X(false, "Generic Wallbox", QString("Unhandled action: %1").arg(action.actionTypeId().toString()).toUtf8());
            info->finish(Thing::ThingErrorUnsupportedFeature);
            return;
        }
        info->finish(Thing::ThingErrorNoError);

    } else {
        Q_ASSERT_X(false, "executeAction", QString("Unhandled thingClassId: %1").arg(thing->thingClassId().toString()).toUtf8());
    }
}

void IntegrationPluginGenericEnergy::thingRemoved(Thing *thing)
{
    if (thing->thingClassId() == impulseSmartMeterThingClassId) {
        m_pulsesPerTimeframe.remove(thing);
    } else if (thing->thingClassId() == impulseSmartMeterThingClassId) {
        m_smartMeterTimer.take(thing)->deleteLater();
        m_pulsesPerTimeframe.remove(thing);
    }
}
