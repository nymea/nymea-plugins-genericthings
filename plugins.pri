TEMPLATE = lib
CONFIG += plugin

QMAKE_CXXFLAGS += -Werror -std=c++11 -g
QMAKE_LFLAGS += -std=c++11

CONFIG += link_pkgconfig
PKGCONFIG += nymea

HEADERS += $${OUT_PWD}/plugininfo.h \
           $${OUT_PWD}/extern-plugininfo.h

# Make the device plugin json file visible in the Qt Creator
OTHER_FILES += $$PWD/$${TARGET}/deviceplugin"$$TARGET".json

# NOTE: if the code includes "plugininfo.h", it would fail if we only give it a compiler for $$OUT_PWD/plugininfo.h
# Let's add a dummy target with the plugininfo.h file without any path to allow the developer to just include it like that.

# Create plugininfo file
plugininfo.target = $$OUT_PWD/plugininfo.h
extern-plugininfo.target = $$OUT_PWD/extern-plugininfo.h
plugininfo_dummy.target = plugininfo.h
extern-plugininfo_dummy.target = extern-plugininfo.h
plugininfo.depends = FORCE
plugininfo.commands = nymea-plugininfocompiler \
                      $$PWD/$${TARGET}/deviceplugin"$$TARGET".json \
                      --output $$OUT_PWD/plugininfo.h \
                      --extern $$OUT_PWD/extern-plugininfo.h
plugininfo_dummy.commands = $$plugininfo.commands
extern-plugininfo.depends = plugininfo
QMAKE_EXTRA_TARGETS += plugininfo plugininfo_dummy extern-plugininfo extern-plugininfo_dummy

# Clean up autogenerated plugin info files
plugininfo_clean.commands = rm -fv $$OUT_PWD/plugininfo.h $$OUT_PWD/extern-plugininfo.h
clean.depends = plugininfo_clean
QMAKE_EXTRA_TARGETS += clean plugininfo_clean

# Install translation files
TRANSLATIONS *= $$files($${PWD}/$${TARGET}/translations/*ts, true)
lupdate.depends = FORCE
lupdate.depends += plugininfo.h
lupdate.commands = lupdate -recursive -no-obsolete $$PWD/"$$TARGET"/"$$TARGET".pro;
QMAKE_EXTRA_TARGETS += lupdate

translations.path = /usr/share/nymea/translations
translations.files = $$[QT_SOURCE_TREE]/translations/*.qm
TRANSLATIONS += $$files($$[QT_SOURCE_TREE]/translations/*.ts, true)

# Install plugin
target.path = $$[QT_INSTALL_LIBS]/nymea/plugins/
INSTALLS += target translations

