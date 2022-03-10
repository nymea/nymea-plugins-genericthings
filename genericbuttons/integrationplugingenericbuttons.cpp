/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*
* Copyright 2013 - 2020, nymea GmbH
* Contact: contact@nymea.io
*
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

#include "integrationplugingenericbuttons.h"
#include "plugininfo.h"

#include <QDebug>

IntegrationPluginGenericButtons::IntegrationPluginGenericButtons()
{
}

void IntegrationPluginGenericButtons::setupThing(ThingSetupInfo *info)
{
    info->finish(Thing::ThingErrorNoError);
}

void IntegrationPluginGenericButtons::executeAction(ThingActionInfo *info)
{
    Thing *thing = info->thing();
    Action action = info->action();

    if (thing->thingClassId() == buttonThingClassId) {
        if (action.actionTypeId() == buttonPressActionTypeId) {
            thing->emitEvent(buttonPressedEventTypeId);
            info->finish(Thing::ThingErrorNoError);
            return;
        }
    }

    if (thing->thingClassId() == virtualButtonThingClassId) {
        if (action.actionTypeId() == virtualButtonPressActionTypeId) {
            thing->emitEvent(virtualButtonPressedEventTypeId);
            info->finish(Thing::ThingErrorNoError);
            return;
        }
    }

    if (thing->thingClassId() == powerSwitchThingClassId) {
        if (action.actionTypeId() == powerSwitchPowerActionTypeId) {
            thing->setStateValue(powerSwitchPowerStateTypeId, action.param(powerSwitchPowerActionPowerParamTypeId).value());
            thing->emitEvent(powerSwitchPressedEventTypeId, {
                                 Param(powerSwitchPowerActionPowerParamTypeId,
                                 action.paramValue(powerSwitchPowerActionPowerParamTypeId).toBool())
                             });
            info->finish(Thing::ThingErrorNoError);
            return;
        }
    }

    if (thing->thingClassId() == virtualPowerSwitchThingClassId) {
        if (action.actionTypeId() == virtualPowerSwitchPowerActionTypeId) {
            thing->setStateValue(virtualPowerSwitchPowerStateTypeId, action.param(virtualPowerSwitchPowerActionPowerParamTypeId).value());
            thing->emitEvent(virtualPowerSwitchPressedEventTypeId, {
                                 Param(virtualPowerSwitchPowerActionPowerParamTypeId,
                                 action.paramValue(virtualPowerSwitchPowerActionPowerParamTypeId).toBool())
                             });
            info->finish(Thing::ThingErrorNoError);
            return;
        }
    }

    Q_ASSERT_X(false, "executeAction", QString("Unhandled actionTypeId: %1").arg(action.actionTypeId().toString()).toUtf8());
}
