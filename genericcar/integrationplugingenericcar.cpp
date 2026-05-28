// SPDX-License-Identifier: GPL-3.0-or-later

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*
* Copyright (C) 2013 - 2024, nymea GmbH
* Copyright (C) 2024 - 2026, chargebyte austria GmbH
*
* This file is part of nymea-plugins-genericthings.
*
* nymea-plugins-genericthings is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* nymea-plugins-genericthings is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with nymea-plugins-genericthings. If not, see <https://www.gnu.org/licenses/>.
*
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "integrationplugingenericcar.h"
#include "plugininfo.h"

IntegrationPluginGenericCar::IntegrationPluginGenericCar() {}

void IntegrationPluginGenericCar::setupThing(ThingSetupInfo *info)
{
    Thing *thing = info->thing();

    // Keep EV capability states aligned with the persisted generic car settings.
    connect(thing, &Thing::settingChanged, this, [thing](const ParamTypeId &paramTypeId, const QVariant &value) {
        if (paramTypeId == carSettingsCapacityParamTypeId) {
            thing->setStateValue(carCapacityStateTypeId, value);
        } else if (paramTypeId == carSettingsChargingInterfacesParamTypeId) {
            thing->setStateValue(carChargingInterfacesStateTypeId, value);
        } else if (paramTypeId == carSettingsAcMinChargingCurrentParamTypeId) {
            qCDebug(dcGenericCar()) << "Car minimum AC charging current settings changed" << value.toUInt() << "A";
            thing->setStateValue(carAcMinChargingCurrentStateTypeId, value);
        } else if (paramTypeId == carSettingsAcMaxChargingCurrentParamTypeId) {
            qCDebug(dcGenericCar()) << "Car maximum AC charging current settings changed" << value.toUInt() << "A";
            thing->setStateValue(carAcMaxChargingCurrentStateTypeId, value);
        } else if (paramTypeId == carSettingsAcPhaseCountParamTypeId) {
            qCDebug(dcGenericCar()) << "Car AC phase count settings changed" << value.toUInt();
            thing->setStateValue(carAcPhaseCountStateTypeId, value);
        }
    });

    // Migration from earlier versions (pre 1.3) which had the capacity setting as a writable state.
    thing->setSettingValue(carSettingsCapacityParamTypeId, thing->stateValue(carCapacityStateTypeId));

    // Existing configured generic cars were always AC charged through a wallbox.
    if (thing->setting(carSettingsChargingInterfacesParamTypeId).toString().isEmpty()) {
        thing->setSettingValue(carSettingsChargingInterfacesParamTypeId, QStringLiteral("ac"));
    }

    // Finish the setup
    info->finish(Thing::ThingErrorNoError);

    // Set the initial EV capability states from the persisted settings.
    thing->setStateValue(carCapacityStateTypeId, thing->setting(carSettingsCapacityParamTypeId));
    thing->setStateValue(carChargingInterfacesStateTypeId, thing->setting(carSettingsChargingInterfacesParamTypeId));
    thing->setStateValue(carAcMinChargingCurrentStateTypeId, thing->setting(carSettingsAcMinChargingCurrentParamTypeId));
    thing->setStateValue(carAcMaxChargingCurrentStateTypeId, thing->setting(carSettingsAcMaxChargingCurrentParamTypeId));
    thing->setStateValue(carAcPhaseCountStateTypeId, thing->setting(carSettingsAcPhaseCountParamTypeId));
}

void IntegrationPluginGenericCar::executeAction(ThingActionInfo *info)
{
    Thing *thing = info->thing();
    Action action = info->action();

    if (action.actionTypeId() == carBatteryLevelActionTypeId) {
        thing->setStateValue(carBatteryLevelStateTypeId, action.paramValue(carBatteryLevelActionBatteryLevelParamTypeId));
        thing->setStateValue(carBatteryCriticalStateTypeId, action.paramValue(carBatteryLevelActionBatteryLevelParamTypeId).toInt() < 10);
        info->finish(Thing::ThingErrorNoError);
    } else {
        Q_ASSERT_X(false, "executeAction", QString("Unhandled action: %1").arg(action.actionTypeId().toString()).toUtf8());
    }
}
