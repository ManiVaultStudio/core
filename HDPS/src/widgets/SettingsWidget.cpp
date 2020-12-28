#include "SettingsWidget.h"
#include "AccordionSection.h"

#include <QVariant>

namespace hdps
{

namespace gui
{

void SettingsWidget::setTitle(const QString& title)
{
    if (title == getTitle())
        return;

    setProperty(qPrintable(AccordionSection::TITLE_PROPERTY_NAME), title);
}

QString SettingsWidget::getTitle() const
{
    return property(qPrintable(AccordionSection::TITLE_PROPERTY_NAME)).toString();
}

void SettingsWidget::setSubtitle(const QString& subtitle)
{
    if (subtitle == getSubtitle())
        return;

    setProperty(qPrintable(AccordionSection::SUBTITLE_PROPERTY_NAME), subtitle);
}

QString SettingsWidget::getSubtitle() const
{
    return property(qPrintable(AccordionSection::SUBTITLE_PROPERTY_NAME)).toString();
}

void SettingsWidget::setIcon(const QIcon& icon)
{
    setProperty(qPrintable(AccordionSection::ICON_PROPERTY_NAME), icon);
}

QIcon SettingsWidget::getIcon() const
{
    return property(qPrintable(AccordionSection::ICON_PROPERTY_NAME)).value<QIcon>();
}

}
}