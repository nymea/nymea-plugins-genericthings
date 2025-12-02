// SPDX-License-Identifier: GPL-3.0-or-later

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*
* Copyright (C) 2013 - 2024, nymea GmbH
* Copyright (C) 2024 - 2025, chargebyte austria GmbH
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

#include "integrationplugingenericthing.h"
#include "plugininfo.h"

#include <QDebug>
#include <QtMath>

IntegrationPluginGenericThing::IntegrationPluginGenericThing()
{

}

void IntegrationPluginGenericThing::setupThing(ThingSetupInfo *info)
{
    info->finish(Thing::ThingErrorNoError);
}

void IntegrationPluginGenericThing::executeAction(ThingActionInfo *info)
{
    Thing *thing = info->thing();
    Action action = info->action();

    if (thing->thingClassId() == genericThingClassId) {
        if (action.actionTypeId() == genericAnalogInputActionTypeId) {
            thing->setStateValue(genericAnalogInputStateTypeId, action.paramValue(genericAnalogInputActionAnalogInputParamTypeId));
            info->finish(Thing::ThingErrorNoError);
            return;
        }
        if (action.actionTypeId() == genericAnalogOutputActionTypeId) {
            thing->setStateValue(genericAnalogOutputStateTypeId, action.paramValue(genericAnalogOutputActionAnalogOutputParamTypeId));
            info->finish(Thing::ThingErrorNoError);
            return;
        }
        if (action.actionTypeId() == genericDigitalInputActionTypeId) {
            thing->setStateValue(genericDigitalInputStateTypeId, action.paramValue(genericDigitalInputActionDigitalInputParamTypeId));
            info->finish(Thing::ThingErrorNoError);
            return;
        }
        if (action.actionTypeId() == genericDigitalOutputActionTypeId) {
            thing->setStateValue(genericDigitalOutputStateTypeId, action.paramValue(genericDigitalOutputActionDigitalOutputParamTypeId));
            info->finish(Thing::ThingErrorNoError);
            return;
        }
        if (action.actionTypeId() == genericStringStateActionTypeId) {
            thing->setStateValue(genericStringStateStateTypeId, action.paramValue(genericStringStateActionStringStateParamTypeId));
            info->finish(Thing::ThingErrorNoError);
            return;
        }
    } else {
        Q_ASSERT_X(false, "executeAction", QString("Unhandled thingClassId: %1").arg(thing->thingClassId().toString()).toUtf8());
    }
}
