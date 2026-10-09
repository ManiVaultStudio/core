#include "util/ColorScheme.h"

#include <QDebug>

namespace {
bool require(bool condition, const char* message)
{
    if (!condition)
        qCritical().noquote() << message;
    return condition;
}
}

bool preservesColorSchemeProperties()
{
    const auto palette = QPalette(QColor("#123456"));
    mv::util::ColorScheme colorScheme(mv::util::ColorScheme::Mode::UserAdded, "Custom", "A custom scheme", palette);

    if (!require(colorScheme.getMode() == mv::util::ColorScheme::Mode::UserAdded, "color scheme mode was not preserved")
        || !require(colorScheme.getName() == "Custom", "color scheme name was not preserved")
        || !require(colorScheme.getDescription() == "A custom scheme", "color scheme description was not preserved")
        || !require(colorScheme.getPalette().color(QPalette::Window) == palette.color(QPalette::Window), "color scheme palette was not preserved"))
        return false;

    colorScheme.setMode(mv::util::ColorScheme::Mode::BuiltIn);
    colorScheme.setName("Updated");
    colorScheme.setDescription("Updated description");
    colorScheme.setPalette(QPalette(Qt::white));

    return require(colorScheme.getMode() == mv::util::ColorScheme::Mode::BuiltIn, "color scheme mode was not updated")
        && require(colorScheme.getName() == "Updated", "color scheme name was not updated")
        && require(colorScheme.getDescription() == "Updated description", "color scheme description was not updated")
        && require(colorScheme.getPalette().color(QPalette::Window) == QColor(Qt::white), "color scheme palette was not updated");
}

int main()
{
    return preservesColorSchemeProperties() ? 0 : 1;
}
