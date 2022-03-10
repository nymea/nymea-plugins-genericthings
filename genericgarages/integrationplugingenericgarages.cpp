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

#include "integrationplugingenericgarages.h"
#include "plugininfo.h"

#include <QDebug>
#include <QtMath>

IntegrationPluginGenericGarages::IntegrationPluginGenericGarages()
{

}

void IntegrationPluginGenericGarages::setupThing(ThingSetupInfo *info)
{
    Thing *thing = info->thing();

    if (thing->thingClassId() == extendedStatefulGaragedoorThingClassId) {
        uint openingDuration = thing->setting(extendedStatefulGaragedoorSettingsOpeningDurationParamTypeId).toUInt();
        QTimer* timer = new QTimer(this);
        timer->setInterval(openingDuration/100.00); // closing timer / 100 to update on every percent
        m_statefulGaragePercentageTimer.insert(thing, timer);
        connect(thing, &Thing::settingChanged, thing, [timer] (const ParamTypeId &paramTypeId, const QVariant &value) {
            if (paramTypeId == extendedStatefulGaragedoorSettingsOpeningDurationParamTypeId) {
                timer->setInterval(value.toUInt()/100.00);
            }
        });
        connect(timer, &QTimer::timeout, this, [thing, timer, this] {
            uint currentPercentage = thing->stateValue(extendedStatefulGaragedoorPercentageStateTypeId).toUInt();
            uint targetPercentage = m_statefulGarageTargetPercentage.value(thing);

            if (currentPercentage < targetPercentage) {
                currentPercentage++;
                thing->setStateValue(extendedStatefulGaragedoorPercentageStateTypeId, currentPercentage);
                thing->setStateValue(extendedStatefulGaragedoorStateStateTypeId, "closing");
                thing->setStateValue(extendedStatefulGaragedoorMovingStateTypeId, true);
                thing->setStateValue(extendedStatefulGaragedoorOpeningOutputStateTypeId, false);
                thing->setStateValue(extendedStatefulGaragedoorClosingOutputStateTypeId, true);

            } else if (currentPercentage > targetPercentage) {
                currentPercentage--;
                thing->setStateValue(extendedStatefulGaragedoorPercentageStateTypeId, currentPercentage);
                thing->setStateValue(extendedStatefulGaragedoorStateStateTypeId, "opening");
                thing->setStateValue(extendedStatefulGaragedoorMovingStateTypeId, true);
                thing->setStateValue(extendedStatefulGaragedoorOpeningOutputStateTypeId, true);
                thing->setStateValue(extendedStatefulGaragedoorClosingOutputStateTypeId, false);

            }

            if (currentPercentage == targetPercentage){
                QString state = currentPercentage == 100 ? "open" : currentPercentage == 0 ? "closed" : "intermediate";
                thing->setStateValue(extendedStatefulGaragedoorStateStateTypeId, state);
                thing->setStateValue(extendedStatefulGaragedoorMovingStateTypeId, false);
                thing->setStateValue(extendedStatefulGaragedoorOpeningOutputStateTypeId, false);
                thing->setStateValue(extendedStatefulGaragedoorClosingOutputStateTypeId, false);
                qCDebug(dcGenericGarages()) << "Stopping garage timer";
                timer->stop();
            }
        });
    }

    // Fall trough, if not already finished and returned...
    info->finish(Thing::ThingErrorNoError);
}

void IntegrationPluginGenericGarages::executeAction(ThingActionInfo *info)
{
    Thing *thing = info->thing();
    Action action = info->action();

    if (thing->thingClassId() == impulseGaragedooorThingClassId) {
        if (action.actionTypeId() == impulseGaragedooorTriggerImpulseActionTypeId) {
            uint duration = thing->setting(impulseGaragedooorSettingsImpulseDurationParamTypeId).toUInt();
            thing->setStateValue(impulseGaragedooorImpulseStateTypeId, true);
            QTimer::singleShot(duration, thing, [thing](){
                thing->setStateValue(impulseGaragedooorImpulseStateTypeId, false);
            });
            info->finish(Thing::ThingErrorNoError);
            return;
        }
        Q_ASSERT_X(false, "executeAction", QString("Unhandled actionTypeId: %1").arg(action.actionTypeId().toString()).toUtf8());
    } else if (thing->thingClassId() == simpleGaragedoorThingClassId) {
        if (action.actionTypeId() == simpleGaragedoorOpenActionTypeId) {
            thing->setStateValue(simpleGaragedoorClosingOutputStateTypeId, false);
            thing->setStateValue(simpleGaragedoorOpeningOutputStateTypeId, true);
            info->finish(Thing::ThingErrorNoError);
            return;
        }
        if (action.actionTypeId() == simpleGaragedoorCloseActionTypeId) {
            thing->setStateValue(simpleGaragedoorOpeningOutputStateTypeId, false);
            thing->setStateValue(simpleGaragedoorClosingOutputStateTypeId, true);
            info->finish(Thing::ThingErrorNoError);
            return;
        }
        if (action.actionTypeId() == simpleGaragedoorStopActionTypeId) {
            thing->setStateValue(simpleGaragedoorClosingOutputStateTypeId, false);
            thing->setStateValue(simpleGaragedoorOpeningOutputStateTypeId, false);
            info->finish(Thing::ThingErrorNoError);
            return;
        }
        Q_ASSERT_X(false, "executeAction", QString("Unhandled actionTypeId: %1").arg(action.actionTypeId().toString()).toUtf8());
    } else if (thing->thingClassId() == extendedStatefulGaragedoorThingClassId) {
        if (action.actionTypeId() == extendedStatefulGaragedoorOpenActionTypeId) {
            m_statefulGarageTargetPercentage[thing] = 0;
            m_statefulGaragePercentageTimer[thing]->start();
            info->finish(Thing::ThingErrorNoError);
            return;
        }
        if (action.actionTypeId() == extendedStatefulGaragedoorCloseActionTypeId) {
            m_statefulGarageTargetPercentage[thing] = 100;
            m_statefulGaragePercentageTimer[thing]->start();
            info->finish(Thing::ThingErrorNoError);
            return;
        }
        if (action.actionTypeId() == extendedStatefulGaragedoorStopActionTypeId) {
            m_statefulGarageTargetPercentage[thing] = thing->stateValue(extendedStatefulGaragedoorPercentageStateTypeId).toUInt();
            info->finish(Thing::ThingErrorNoError);
            return;
        }

        Q_ASSERT_X(false, "executeAction", QString("Unhandled actionTypeId: %1").arg(action.actionTypeId().toString()).toUtf8());

    } else {
        Q_ASSERT_X(false, "executeAction", QString("Unhandled thingClassId: %1").arg(thing->thingClassId().toString()).toUtf8());
    }
}
