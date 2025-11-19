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

#ifndef INTEGRATIONPLUGINVIRTUALBUTTONS_H
#define INTEGRATIONPLUGINVIRTUALBUTTONS_H

#include "integrations/integrationplugin.h"

#include "extern-plugininfo.h"

class IntegrationPluginGenericButtons : public IntegrationPlugin
{
    Q_OBJECT

    Q_PLUGIN_METADATA(IID "io.nymea.IntegrationPlugin" FILE "integrationplugingenericbuttons.json")
    Q_INTERFACES(IntegrationPlugin)

public:
    explicit IntegrationPluginGenericButtons();
    void setupThing(ThingSetupInfo *info) override;

public slots:
    void executeAction(ThingActionInfo *info) override;

};

#endif // INTEGRATIONPLUGINVIRTUALBUTTONS_H
