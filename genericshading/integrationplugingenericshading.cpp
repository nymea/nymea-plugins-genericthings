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

#include "integrationplugingenericshading.h"
#include "plugininfo.h"

#include <QDebug>

IntegrationPluginGenericShading::IntegrationPluginGenericShading()
{

}

void IntegrationPluginGenericShading::setupThing(ThingSetupInfo *info)
{
    Thing *thing = info->thing();

    if (thing->thingClassId() == extendedBlindThingClassId) {
        uint closingDuration = thing->setting(extendedBlindSettingsClosingDurationParamTypeId).toUInt();
        QTimer* timer = new QTimer(this);
        timer->setInterval(closingDuration/100.00); // closing timer / 100 to update on every percent
        m_extendedBlindPercentageTimer.insert(thing, timer);
        connect(thing, &Thing::settingChanged, thing, [timer] (const ParamTypeId &paramTypeId, const QVariant &value) {
            if (paramTypeId == extendedBlindSettingsClosingDurationParamTypeId) {
                timer->setInterval(value.toUInt()/100.00);
            }
        });
        connect(timer, &QTimer::timeout, this, [thing, this] {
            uint currentPercentage = thing->stateValue(extendedBlindPercentageStateTypeId).toUInt();

            if (thing->stateValue(extendedBlindStatusStateTypeId).toString() == "Closing") {

                if (currentPercentage == 100) {
                    setBlindState(BlindStateStopped, thing);
                    qCDebug(dcGenericShading()) << "Extended blind is closed, stopping timer";
                } else {
                    currentPercentage++;
                    thing->setStateValue(extendedBlindPercentageStateTypeId, currentPercentage);
                }
            } else if (thing->stateValue(extendedBlindStatusStateTypeId).toString() == "Opening") {

                if (currentPercentage == 0) {
                    setBlindState(BlindStateStopped, thing);
                    qCDebug(dcGenericShading()) << "Extended blind is opened, stopping timer";
                } else {
                    currentPercentage--;
                    thing->setStateValue(extendedBlindPercentageStateTypeId, currentPercentage);
                }
            } else {
                setBlindState(BlindStateStopped, thing);
            }

            if (m_extendedBlindPercentageTimer.contains(thing)) {
                uint targetPercentage = m_extendedBlindTargetPercentage.value(thing);
                if (targetPercentage == currentPercentage) {
                    qCDebug(dcGenericShading()) << "Extended blind has reached target percentage, stopping timer";
                    setBlindState(BlindStateStopped, thing);
                }
            }
        });
    } else if (thing->thingClassId() == extendedAwningThingClassId) {
            uint closingDuration = thing->setting(extendedAwningSettingsClosingDurationParamTypeId).toUInt();
            QTimer* timer = new QTimer(this);
            timer->setInterval(closingDuration/100.00); // closing timer / 100 to update on every percent
            m_extendedAwningPercentageTimer.insert(thing, timer);
            connect(thing, &Thing::settingChanged, thing, [timer] (const ParamTypeId &paramTypeId, const QVariant &value) {
                if (paramTypeId == extendedAwningSettingsClosingDurationParamTypeId) {
                    timer->setInterval(value.toUInt()/100.00);
                }
            });
            connect(timer, &QTimer::timeout, this, [thing, this] {
                uint currentPercentage = thing->stateValue(extendedAwningPercentageStateTypeId).toUInt();

                if (thing->stateValue(extendedAwningStatusStateTypeId).toString() == "Closing") {

                    if (currentPercentage == 100) {
                        setBlindState(BlindStateStopped, thing);
                        qCDebug(dcGenericShading()) << "Extended awning is closed, stopping timer";
                    } else {
                        currentPercentage++;
                        thing->setStateValue(extendedAwningPercentageStateTypeId, currentPercentage);
                    }
                } else if (thing->stateValue(extendedAwningStatusStateTypeId).toString() == "Opening") {

                    if (currentPercentage == 0) {
                        setBlindState(BlindStateStopped, thing);
                        qCDebug(dcGenericShading()) << "Extended awning is opened, stopping timer";
                    } else {
                        currentPercentage--;
                        thing->setStateValue(extendedAwningPercentageStateTypeId, currentPercentage);
                    }
                } else {
                    setBlindState(BlindStateStopped, thing);
                }

                if (m_extendedAwningPercentageTimer.contains(thing)) {
                    uint targetPercentage = m_extendedAwningTargetPercentage.value(thing);
                    if (targetPercentage == currentPercentage) {
                        qCDebug(dcGenericShading()) << "Extended awning has reached target percentage, stopping timer";
                        setBlindState(BlindStateStopped, thing);
                    }
                }
            });
    } else if (info->thing()->thingClassId() == venetianBlindThingClassId) {
        uint closingTime = thing->setting(venetianBlindSettingsClosingDurationParamTypeId).toUInt();
        uint angleTime = thing->setting(venetianBlindSettingsAngleTimeParamTypeId).toUInt();
        if (closingTime < angleTime) {
            return info->finish(Thing::ThingErrorSetupFailed, tr("Invalid closing or angle time"));
        }
        QTimer* closingTimer = new QTimer(this);
        closingTimer->setInterval(closingTime/100.00); // closing timer / 100 to update on every percent
        m_extendedBlindPercentageTimer.insert(thing, closingTimer);

        connect(closingTimer, &QTimer::timeout, thing, [thing, this] {
            uint currentPercentage = thing->stateValue(venetianBlindPercentageStateTypeId).toUInt();

            if (thing->stateValue(venetianBlindStatusStateTypeId).toString() == "Closing") {

                if (currentPercentage == 100) {
                    setBlindState(BlindStateStopped, thing);
                    qCDebug(dcGenericShading()) << "Venetian blind is closed, stopping timer";
                } else if (currentPercentage > 100) {
                    currentPercentage = 100;
                    setBlindState(BlindStateStopped, thing);
                    qCWarning(dcGenericShading()) << "Venetian blind overshoot 100 percent";
                } else {
                    currentPercentage++;
                    thing->setStateValue(venetianBlindPercentageStateTypeId, currentPercentage);
                }
            } else if (thing->stateValue(venetianBlindStatusStateTypeId).toString() == "Opening") {

                if (currentPercentage == 0) {
                    setBlindState(BlindStateStopped, thing);
                    qCDebug(dcGenericShading()) << "Venetian blind is opened, stopping timer";
                } else {
                    currentPercentage--;
                    thing->setStateValue(venetianBlindPercentageStateTypeId, currentPercentage);
                }
            } else {
                setBlindState(BlindStateStopped, thing);
            }

            if (m_extendedBlindPercentageTimer.contains(thing)) {
                uint targetPercentage = m_extendedBlindTargetPercentage.value(thing);
                if (targetPercentage == currentPercentage) {
                    qCDebug(dcGenericShading()) << "Venetian blind has reached target percentage, stopping timer";
                    setBlindState(BlindStateStopped, thing);
                }
            }
        });

        QTimer* angleTimer = new QTimer(this);
        angleTimer->setInterval(angleTime/180.00); // -90 to 90 degree -> 180 degree total
        m_venetianBlindAngleTimer.insert(thing, angleTimer);
        connect(thing, &Thing::settingChanged, thing, [closingTimer, angleTimer] (const ParamTypeId &paramTypeId, const QVariant &value) {
            if (paramTypeId == venetianBlindSettingsClosingDurationParamTypeId) {
                closingTimer->setInterval(value.toUInt()/100.00);
            } else if (paramTypeId == venetianBlindSettingsAngleTimeParamTypeId) {
                angleTimer->setInterval(value.toUInt()/180.00);
            }
        });
        connect(angleTimer, &QTimer::timeout, thing, [thing, this] {
            int currentAngle = thing->stateValue(venetianBlindAngleStateTypeId).toInt();
            if (thing->stateValue(venetianBlindStatusStateTypeId).toString() == "Closing") {

                if (currentAngle < 90) {
                    currentAngle++;
                } else if (currentAngle == 90) {
                    m_venetianBlindAngleTimer.value(thing)->stop();
                } else if (currentAngle > 90) {
                    currentAngle = 90;
                    m_venetianBlindAngleTimer.value(thing)->stop();
                    qCWarning(dcGenericShading()) << "Venetian blind overshoot angle boundaries";
                }
                thing->setStateValue(venetianBlindAngleStateTypeId, currentAngle);
            } else if (thing->stateValue(venetianBlindStatusStateTypeId).toString() == "Opening") {

                if (currentAngle > -90) {
                    currentAngle--;
                } else if (currentAngle == -90) {
                    m_venetianBlindAngleTimer.value(thing)->stop();
                } else if (currentAngle < -90) {
                    currentAngle = -90;
                    m_venetianBlindAngleTimer.value(thing)->stop();
                    qCWarning(dcGenericShading()) << "Venetian blind overshoot angle boundaries";
                }
                thing->setStateValue(venetianBlindAngleStateTypeId, currentAngle);
            }

            if (m_venetianBlindTargetAngle.contains(thing)) {
                int targetAngle = m_venetianBlindTargetAngle.value(thing);
                if (targetAngle == currentAngle) {
                    qCDebug(dcGenericShading()) << "Venetian blind has reached target angle, stopping timer";
                    setBlindState(BlindStateStopped, thing);
                }
            }
        });
    }

    // Fall trough, if not already finished and returned...
    info->finish(Thing::ThingErrorNoError);
}

void IntegrationPluginGenericShading::executeAction(ThingActionInfo *info)
{
    Thing *thing = info->thing();
    Action action = info->action();

    if (thing->thingClassId() == awningThingClassId) {
        if (action.actionTypeId() == awningOpenActionTypeId) {
            thing->setStateValue(awningStatusStateTypeId, "Opening");
            thing->setStateValue(awningClosingOutputStateTypeId, false);
            thing->setStateValue(awningOpeningOutputStateTypeId, true);
            return info->finish(Thing::ThingErrorNoError);
        } else if (action.actionTypeId() == awningStopActionTypeId) {
            thing->setStateValue(awningStatusStateTypeId, "Stopped");
            thing->setStateValue(awningOpeningOutputStateTypeId, false);
            thing->setStateValue(awningClosingOutputStateTypeId, false);
            return info->finish(Thing::ThingErrorNoError);
        } else if (action.actionTypeId() == awningCloseActionTypeId) {
            thing->setStateValue(awningStatusStateTypeId, "Closing");
            thing->setStateValue(awningOpeningOutputStateTypeId, false);
            thing->setStateValue(awningClosingOutputStateTypeId, true);
            return info->finish(Thing::ThingErrorNoError);
        } else if (action.actionTypeId() == awningOpeningOutputActionTypeId) {
            bool on = action.param(awningOpeningOutputActionOpeningOutputParamTypeId).value().toBool();
            thing->setStateValue(awningOpeningOutputStateTypeId, on);
            if (on) {
                thing->setStateValue(awningStatusStateTypeId, "Opening");
                thing->setStateValue(awningClosingOutputStateTypeId, false);
            } else {
                thing->setStateValue(awningStatusStateTypeId, "Stopped");
            }
            info->finish(Thing::ThingErrorNoError);
        } else if (action.actionTypeId() == awningClosingOutputActionTypeId) {
            bool on = action.param(awningClosingOutputActionClosingOutputParamTypeId).value().toBool();
            thing->setStateValue(awningClosingOutputStateTypeId, on);
            if (on) {
                thing->setStateValue(awningStatusStateTypeId, "Closing");
                thing->setStateValue(awningOpeningOutputStateTypeId, false);
            } else {
                thing->setStateValue(awningStatusStateTypeId, "Stopped");
            }
            info->finish(Thing::ThingErrorNoError);
        } else {
            Q_ASSERT_X(false, "executeAction", QString("Unhandled actionTypeId: %1").arg(action.actionTypeId().toString()).toUtf8());
        }

    } else if (thing->thingClassId() == extendedAwningThingClassId) {
        if (action.actionTypeId() == extendedAwningOpenActionTypeId) {
            setBlindState(BlindStateOpening, thing);
            return info->finish(Thing::ThingErrorNoError);
        } else if (action.actionTypeId() == extendedAwningStopActionTypeId) {
            setBlindState(BlindStateStopped, thing);
            return info->finish(Thing::ThingErrorNoError);
        } else if (action.actionTypeId() == extendedAwningCloseActionTypeId) {
            setBlindState(BlindStateClosing, thing);
            return info->finish(Thing::ThingErrorNoError);
        } else if (action.actionTypeId() == extendedAwningOpeningOutputActionTypeId) {
            bool on = action.param(extendedAwningOpeningOutputActionOpeningOutputParamTypeId).value().toBool();
            thing->setStateValue(extendedAwningOpeningOutputStateTypeId, on);
            if (on) {
                setBlindState(BlindStateOpening, thing);
            } else {
                setBlindState(BlindStateStopped, thing);
            }
            info->finish(Thing::ThingErrorNoError);
        } else if (action.actionTypeId() == extendedAwningClosingOutputActionTypeId) {
            bool on = action.param(extendedAwningClosingOutputActionClosingOutputParamTypeId).value().toBool();
            thing->setStateValue(extendedAwningClosingOutputStateTypeId, on);
            if (on) {
                setBlindState(BlindStateClosing, thing);
            } else {
                setBlindState(BlindStateStopped, thing);
            }
            info->finish(Thing::ThingErrorNoError);
        } else if (action.actionTypeId() == extendedAwningPercentageActionTypeId) {
            moveBlindToPercentage(action, thing);
        } else {
            Q_ASSERT_X(false, "executeAction", QString("Unhandled actionTypeId: %1").arg(action.actionTypeId().toString()).toUtf8());
        }
    } else if (thing->thingClassId() == blindThingClassId ) {
        if (action.actionTypeId() == blindOpenActionTypeId) {
            thing->setStateValue(blindStatusStateTypeId, "Opening");
            thing->setStateValue(blindClosingOutputStateTypeId, false);
            thing->setStateValue(blindOpeningOutputStateTypeId, true);
            return info->finish(Thing::ThingErrorNoError);
        } else if (action.actionTypeId() == blindStopActionTypeId) {
            thing->setStateValue(blindStatusStateTypeId, "Stopped");
            thing->setStateValue(blindOpeningOutputStateTypeId, false);
            thing->setStateValue(blindClosingOutputStateTypeId, false);
            return info->finish(Thing::ThingErrorNoError);
        } else if (action.actionTypeId() == blindCloseActionTypeId) {
            thing->setStateValue(blindStatusStateTypeId, "Closing");
            thing->setStateValue(blindOpeningOutputStateTypeId, false);
            thing->setStateValue(blindClosingOutputStateTypeId, true);
            return info->finish(Thing::ThingErrorNoError);
        } else if (action.actionTypeId() == blindOpeningOutputActionTypeId) {
            bool on = action.param(blindOpeningOutputActionOpeningOutputParamTypeId).value().toBool();
            thing->setStateValue(blindOpeningOutputStateTypeId, on);
            if (on) {
                thing->setStateValue(blindStatusStateTypeId, "Opening");
                thing->setStateValue(blindClosingOutputStateTypeId, false);
            } else {
                thing->setStateValue(blindStatusStateTypeId, "Stopped");
            }
            info->finish(Thing::ThingErrorNoError);
        } else if (action.actionTypeId() == blindClosingOutputActionTypeId) {
            bool on = action.param(blindClosingOutputActionClosingOutputParamTypeId).value().toBool();
            thing->setStateValue(blindClosingOutputStateTypeId, on);
            if (on) {
                thing->setStateValue(blindStatusStateTypeId, "Closing");
                thing->setStateValue(blindOpeningOutputStateTypeId, false);
            } else {
                thing->setStateValue(blindStatusStateTypeId, "Stopped");
            }
            info->finish(Thing::ThingErrorNoError);
        } else {
            Q_ASSERT_X(false, "executeAction", QString("Unhandled actionTypeId: %1").arg(action.actionTypeId().toString()).toUtf8());
        }
    } else if (thing->thingClassId() == extendedBlindThingClassId) {

        if (action.actionTypeId() == extendedBlindOpenActionTypeId) {
            setBlindState(BlindStateOpening, thing);
            return info->finish(Thing::ThingErrorNoError);
        } else if (action.actionTypeId() == extendedBlindStopActionTypeId) {
            setBlindState(BlindStateStopped, thing);
            return info->finish(Thing::ThingErrorNoError);
        } else if (action.actionTypeId() == extendedBlindCloseActionTypeId) {
            setBlindState(BlindStateClosing, thing);
            return info->finish(Thing::ThingErrorNoError);
        } else if (action.actionTypeId() == extendedBlindOpeningOutputActionTypeId) {
            bool on = action.param(extendedBlindOpeningOutputActionOpeningOutputParamTypeId).value().toBool();
            thing->setStateValue(extendedBlindOpeningOutputStateTypeId, on);
            if (on) {
                setBlindState(BlindStateOpening, thing);
            } else {
                setBlindState(BlindStateStopped, thing);
            }
            info->finish(Thing::ThingErrorNoError);
        } else if (action.actionTypeId() == extendedBlindClosingOutputActionTypeId) {
            bool on = action.param(extendedBlindClosingOutputActionClosingOutputParamTypeId).value().toBool();
            thing->setStateValue(extendedBlindClosingOutputStateTypeId, on);
            if (on) {
                setBlindState(BlindStateClosing, thing);
            } else {
                setBlindState(BlindStateStopped, thing);
            }
            info->finish(Thing::ThingErrorNoError);
        } else if (action.actionTypeId() == extendedBlindPercentageActionTypeId) {
            moveBlindToPercentage(action, thing);
        } else {
            Q_ASSERT_X(false, "executeAction", QString("Unhandled actionTypeId: %1").arg(action.actionTypeId().toString()).toUtf8());
        }
    } else if (thing->thingClassId() == venetianBlindThingClassId) {
        if (action.actionTypeId() == venetianBlindOpenActionTypeId) {
            setBlindState(BlindStateOpening, thing);
            return info->finish(Thing::ThingErrorNoError);
        } else if (action.actionTypeId() == venetianBlindStopActionTypeId) {
            setBlindState(BlindStateStopped, thing);
            return info->finish(Thing::ThingErrorNoError);
        } else if (action.actionTypeId() == venetianBlindCloseActionTypeId) {
            setBlindState(BlindStateClosing, thing);
            return info->finish(Thing::ThingErrorNoError);
        } else if (action.actionTypeId() == venetianBlindOpeningOutputActionTypeId) {
            bool on = action.param(venetianBlindOpeningOutputActionOpeningOutputParamTypeId).value().toBool();
            thing->setStateValue(venetianBlindOpeningOutputStateTypeId, on);
            if (on) {
                setBlindState(BlindStateOpening, thing);
            } else {
                setBlindState(BlindStateStopped, thing);
            }
            info->finish(Thing::ThingErrorNoError);
        } else if (action.actionTypeId() == venetianBlindClosingOutputActionTypeId) {
            bool on = action.param(venetianBlindClosingOutputActionClosingOutputParamTypeId).value().toBool();
            thing->setStateValue(venetianBlindClosingOutputStateTypeId, on);
            if (on) {
                setBlindState(BlindStateClosing, thing);
            } else {
                setBlindState(BlindStateStopped, thing);
            }
            info->finish(Thing::ThingErrorNoError);
        } else if (action.actionTypeId() == venetianBlindPercentageActionTypeId) {
            moveBlindToPercentage(action, thing);
            info->finish(Thing::ThingErrorNoError);
        } else if (action.actionTypeId() == venetianBlindAngleActionTypeId) {
            moveBlindToAngle(action, thing);
            info->finish(Thing::ThingErrorNoError);
        } else {
            Q_ASSERT_X(false, "executeAction", QString("Unhandled actionTypeId: %1").arg(action.actionTypeId().toString()).toUtf8());
        }
    } else if (thing->thingClassId() == shutterThingClassId) {
        if (action.actionTypeId() == shutterOpenActionTypeId) {
            thing->setStateValue(shutterStatusStateTypeId, "Opening");
            thing->setStateValue(shutterClosingOutputStateTypeId, false);
            thing->setStateValue(shutterOpeningOutputStateTypeId, true);
            return info->finish(Thing::ThingErrorNoError);
        } else if (action.actionTypeId() == shutterStopActionTypeId) {
            thing->setStateValue(shutterStatusStateTypeId, "Stopped");
            thing->setStateValue(shutterOpeningOutputStateTypeId, false);
            thing->setStateValue(shutterClosingOutputStateTypeId, false);
            return info->finish(Thing::ThingErrorNoError);
        } else if (action.actionTypeId() == shutterCloseActionTypeId) {
            thing->setStateValue(shutterStatusStateTypeId, "Closing");
            thing->setStateValue(shutterOpeningOutputStateTypeId, false);
            thing->setStateValue(shutterClosingOutputStateTypeId, true);
            return info->finish(Thing::ThingErrorNoError);
        } else if (action.actionTypeId() == shutterOpeningOutputActionTypeId) {
            bool on = action.param(shutterOpeningOutputActionOpeningOutputParamTypeId).value().toBool();
            thing->setStateValue(shutterOpeningOutputStateTypeId, on);
            if (on) {
                thing->setStateValue(shutterStatusStateTypeId, "Opening");
                thing->setStateValue(shutterClosingOutputStateTypeId, false);
            } else {
                thing->setStateValue(shutterStatusStateTypeId, "Stopped");
            }
            info->finish(Thing::ThingErrorNoError);
        } else if (action.actionTypeId() == shutterClosingOutputActionTypeId) {
            bool on = action.param(shutterClosingOutputActionClosingOutputParamTypeId).value().toBool();
            thing->setStateValue(shutterClosingOutputStateTypeId, on);
            if (on) {
                thing->setStateValue(shutterStatusStateTypeId, "Closing");
                thing->setStateValue(shutterOpeningOutputStateTypeId, false);
            } else {
                thing->setStateValue(shutterStatusStateTypeId, "Stopped");
            }
            info->finish(Thing::ThingErrorNoError);
        } else {
            Q_ASSERT_X(false, "executeAction", QString("Unhandled actionTypeId: %1").arg(action.actionTypeId().toString()).toUtf8());
        }
    } else {
        Q_ASSERT_X(false, "executeAction", QString("Unhandled thingClassId: %1").arg(thing->thingClassId().toString()).toUtf8());
    }
}

void IntegrationPluginGenericShading::thingRemoved(Thing *thing)
{
    if (thing->thingClassId() == extendedBlindThingClassId) {
        m_extendedBlindPercentageTimer.take(thing)->deleteLater();
        m_extendedBlindTargetPercentage.remove(thing);
    } else if (thing->thingClassId() == venetianBlindThingClassId) {
        m_extendedBlindPercentageTimer.take(thing)->deleteLater();
        m_extendedBlindTargetPercentage.remove(thing);
        m_venetianBlindAngleTimer.take(thing)->deleteLater();
        m_venetianBlindTargetAngle.remove(thing);
    }
}

double IntegrationPluginGenericShading::mapDoubleValue(double value, double fromMin, double fromMax, double toMin, double toMax)
{
    double percent = (value - fromMin) / (fromMax - fromMin);
    double toValue = toMin + (toMax - toMin) * percent;
    return toValue;
}

void IntegrationPluginGenericShading::setBlindState(IntegrationPluginGenericShading::BlindState state, Thing *thing)
{
    //If an ongoing "to percentage" actions is beeing executed, it is now overruled by another action
    m_extendedBlindTargetPercentage.remove(thing);

    if (thing->thingClassId() == extendedBlindThingClassId) {
        switch (state) {
        case BlindStateOpening:
            thing->setStateValue(extendedBlindStatusStateTypeId, "Opening");
            thing->setStateValue(extendedBlindClosingOutputStateTypeId, false);
            thing->setStateValue(extendedBlindOpeningOutputStateTypeId, true);
            thing->setStateValue(extendedBlindMovingStateTypeId, true);
            m_extendedBlindPercentageTimer.value(thing)->start();
            break;
        case BlindStateClosing:
            thing->setStateValue(extendedBlindStatusStateTypeId, "Closing");
            thing->setStateValue(extendedBlindClosingOutputStateTypeId, true);
            thing->setStateValue(extendedBlindOpeningOutputStateTypeId, false);
            thing->setStateValue(extendedBlindMovingStateTypeId, true);
            m_extendedBlindPercentageTimer.value(thing)->start();
            break;
        case BlindStateStopped:
            thing->setStateValue(extendedBlindStatusStateTypeId, "Stopped");
            thing->setStateValue(extendedBlindClosingOutputStateTypeId, false);
            thing->setStateValue(extendedBlindOpeningOutputStateTypeId, false);
            thing->setStateValue(extendedBlindMovingStateTypeId, false);
            m_extendedBlindPercentageTimer.value(thing)->stop();
            break;
        }
    } else if (thing->thingClassId() == extendedAwningThingClassId) {
        switch (state) {
        case BlindStateOpening:
            thing->setStateValue(extendedAwningStatusStateTypeId, "Opening");
            thing->setStateValue(extendedAwningClosingOutputStateTypeId, false);
            thing->setStateValue(extendedAwningOpeningOutputStateTypeId, true);
            thing->setStateValue(extendedAwningMovingStateTypeId, true);
            m_extendedAwningPercentageTimer.value(thing)->start();
            break;
        case BlindStateClosing:
            thing->setStateValue(extendedAwningStatusStateTypeId, "Closing");
            thing->setStateValue(extendedAwningClosingOutputStateTypeId, true);
            thing->setStateValue(extendedAwningOpeningOutputStateTypeId, false);
            thing->setStateValue(extendedAwningMovingStateTypeId, true);
            m_extendedAwningPercentageTimer.value(thing)->start();
            break;
        case BlindStateStopped:
            thing->setStateValue(extendedAwningStatusStateTypeId, "Stopped");
            thing->setStateValue(extendedAwningClosingOutputStateTypeId, false);
            thing->setStateValue(extendedAwningOpeningOutputStateTypeId, false);
            thing->setStateValue(extendedAwningMovingStateTypeId, false);
            m_extendedAwningPercentageTimer.value(thing)->stop();
            break;
        }

    } else if (thing->thingClassId() == venetianBlindThingClassId) {
        m_venetianBlindTargetAngle.remove(thing);
        switch (state) {
        case BlindStateOpening:
            thing->setStateValue(venetianBlindStatusStateTypeId, "Opening");
            thing->setStateValue(venetianBlindClosingOutputStateTypeId, false);
            thing->setStateValue(venetianBlindOpeningOutputStateTypeId, true);
            thing->setStateValue(venetianBlindMovingStateTypeId, true);
            m_extendedBlindPercentageTimer.value(thing)->start();
            m_venetianBlindAngleTimer.value(thing)->start();
            break;
        case BlindStateClosing:
            thing->setStateValue(venetianBlindStatusStateTypeId, "Closing");
            thing->setStateValue(venetianBlindClosingOutputStateTypeId, true);
            thing->setStateValue(venetianBlindOpeningOutputStateTypeId, false);
            thing->setStateValue(venetianBlindMovingStateTypeId, true);
            m_extendedBlindPercentageTimer.value(thing)->start();
            m_venetianBlindAngleTimer.value(thing)->start();
            break;
        case BlindStateStopped:
            thing->setStateValue(venetianBlindStatusStateTypeId, "Stopped");
            thing->setStateValue(venetianBlindClosingOutputStateTypeId, false);
            thing->setStateValue(venetianBlindOpeningOutputStateTypeId, false);
            thing->setStateValue(venetianBlindMovingStateTypeId, false);
            m_extendedBlindPercentageTimer.value(thing)->stop();
            m_venetianBlindAngleTimer.value(thing)->stop();
            break;
        }
    }
}

void IntegrationPluginGenericShading::moveBlindToPercentage(Action action, Thing *thing)
{
    if (thing->thingClassId() == extendedBlindThingClassId) {
        uint targetPercentage = action.param(extendedBlindPercentageActionPercentageParamTypeId).value().toUInt();
        uint currentPercentage = thing->stateValue(extendedBlindPercentageStateTypeId).toUInt();
        // 100% indicates the device is fully closed
        if (targetPercentage == currentPercentage) {
            qCDebug(dcGenericShading()) << "Extended blind is already at given percentage" << targetPercentage;
        } else if (targetPercentage > currentPercentage) {
            setBlindState(BlindStateClosing, thing);
            m_extendedBlindTargetPercentage.insert(thing, targetPercentage);
        } else if (targetPercentage < currentPercentage) {
            setBlindState(BlindStateOpening, thing);
            m_extendedBlindTargetPercentage.insert(thing, targetPercentage);
        } else {
            setBlindState(BlindStateStopped, thing);
        }
    } else if (thing->thingClassId() == extendedAwningThingClassId) {
        uint targetPercentage = action.param(extendedAwningPercentageActionPercentageParamTypeId).value().toUInt();
        uint currentPercentage = thing->stateValue(extendedAwningPercentageStateTypeId).toUInt();
        // 100% indicates the device is fully closed
        if (targetPercentage == currentPercentage) {
            qCDebug(dcGenericShading()) << "Extended awningis already at given percentage" << targetPercentage;
        } else if (targetPercentage > currentPercentage) {
            setBlindState(BlindStateClosing, thing);
            m_extendedAwningTargetPercentage.insert(thing, targetPercentage);
        } else if (targetPercentage < currentPercentage) {
            setBlindState(BlindStateOpening, thing);
            m_extendedAwningTargetPercentage.insert(thing, targetPercentage);
        } else {
            setBlindState(BlindStateStopped, thing);
        }

    } else if (thing->thingClassId() == venetianBlindThingClassId) {
        uint targetPercentage = action.param(venetianBlindPercentageActionPercentageParamTypeId).value().toUInt();
        uint currentPercentage = thing->stateValue(venetianBlindPercentageStateTypeId).toUInt();
        qCDebug(dcGenericShading()) << "Moving venetian blind to percentage" << targetPercentage << "Current percentage:" << currentPercentage;
        // 100% indicates the device is fully closed
        if (targetPercentage == currentPercentage) {
            qCDebug(dcGenericShading()) << "Extended blind is already at given percentage" << targetPercentage;
        } else if (targetPercentage > currentPercentage) {
            setBlindState(BlindStateClosing, thing);
            m_extendedBlindTargetPercentage.insert(thing, targetPercentage);
        } else if (targetPercentage < currentPercentage) {
            setBlindState(BlindStateOpening, thing);
            m_extendedBlindTargetPercentage.insert(thing, targetPercentage);
        } else {
            setBlindState(BlindStateStopped, thing);
        }
    } else {
        qCDebug(dcGenericShading()) << "Move to percentage doesn't support this thingClass";
    }
}

void IntegrationPluginGenericShading::moveBlindToAngle(Action action, Thing *thing)
{
    if (thing->thingClassId() == venetianBlindThingClassId) {
        if (action.actionTypeId() == venetianBlindAngleActionTypeId) {
            //NOTE moving percentage affects the angle but the angle doesnt affect the percentage
            // opening -> -90
            // closing -> +90
            int targetAngle = action.param(venetianBlindAngleActionAngleParamTypeId).value().toInt();
            int currentAngle = thing->stateValue(venetianBlindAngleStateTypeId).toInt();
            if (targetAngle == currentAngle) {
                qCDebug(dcGenericShading()) << "Venetian blind is already at given angle" << targetAngle;
            } else if (targetAngle > currentAngle) {
                setBlindState(BlindStateClosing, thing);
                m_venetianBlindTargetAngle.insert(thing, targetAngle);
            } else if (targetAngle < currentAngle) {
                setBlindState(BlindStateOpening, thing);
                m_venetianBlindTargetAngle.insert(thing, targetAngle);
            } else {
                setBlindState(BlindStateStopped, thing);
            }
        }
    } else {
        qCDebug(dcGenericShading()) << "Move to angle doesn't support this thingClass";
    }
}
