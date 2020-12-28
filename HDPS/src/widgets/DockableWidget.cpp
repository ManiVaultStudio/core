#include "DockableWidget.h"
#include "../Application.h"

#include <QVariant>

namespace hdps
{

namespace gui
{

const QString DockableWidget::TITLE_PROPERTY_NAME       = "Title";
const QString DockableWidget::SUBTITLE_PROPERTY_NAME    = "Subtitle";
const QString DockableWidget::ICON_PROPERTY_NAME        = "Icon";

DockableWidget::DockableWidget(const DockingLocation& location /*= Location::Left*/, QWidget* parent /*= nullptr*/) :
    QWidget(parent),
    _dockingLocation(location)
{
}

DockableWidget::~DockableWidget()
{
}

void DockableWidget::setTitle(const QString& title)
{
    if (title == getTitle())
        return;

    setProperty(qPrintable(DockableWidget::TITLE_PROPERTY_NAME), title);
}

QString DockableWidget::getTitle() const
{
    const auto title = property(qPrintable(DockableWidget::TITLE_PROPERTY_NAME));

    auto defaultToWindowTitle = false;

    if (!title.isValid())
        defaultToWindowTitle = true;

    if (title.toString().isEmpty())
        defaultToWindowTitle = true;

    if (defaultToWindowTitle)
        return windowTitle();

    return title.toString();
}

void DockableWidget::setSubtitle(const QString& subtitle)
{
    if (subtitle == getSubtitle())
        return;

    setProperty(qPrintable(DockableWidget::SUBTITLE_PROPERTY_NAME), subtitle);
}

QString DockableWidget::getSubtitle() const
{
    return property(qPrintable(DockableWidget::SUBTITLE_PROPERTY_NAME)).toString();
}

void DockableWidget::setIcon(const QIcon& icon)
{
    setProperty(qPrintable(DockableWidget::ICON_PROPERTY_NAME), icon);
}

QIcon DockableWidget::getIcon() const
{
    const auto icon = property(qPrintable(DockableWidget::ICON_PROPERTY_NAME));

    if (!icon.isValid())
        return windowIcon();

    return icon.value<QIcon>();
}

hdps::gui::DockableWidget::DockingLocation DockableWidget::getDockingLocation() const
{
    return _dockingLocation;
}

void DockableWidget::setDockingLocation(const DockingLocation& location)
{
    _dockingLocation = location;
}

}
}