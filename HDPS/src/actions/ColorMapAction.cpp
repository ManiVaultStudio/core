#include "ColorMapAction.h"

namespace hdps {

namespace gui {

ColorMapAction::ColorMapAction(QObject* parent, const QString& title /*= ""*/, const QString& colorMap /*= ""*/, const QString& defaultColorMap /*= ""*/) :
    OptionAction(parent, title, QStringList(), colorMap, defaultColorMap)
{
    setText(title);
    initialize(colorMap, defaultColorMap);
}

void ColorMapAction::initialize(const QString& colorMap /*= ""*/, const QString& defaultColorMap /*= ""*/)
{
    setCurrentText(colorMap);
    setDefaultText(defaultColorMap);
}

QString ColorMapAction::getColorMap() const
{
    return OptionAction::getCurrentText();
}

void ColorMapAction::setColorMap(const QString& colorMap)
{
    Q_ASSERT(!colorMap.isEmpty());

    OptionAction::setCurrentText(colorMap);

}

QString ColorMapAction::getDefaultColorMap() const
{
    return OptionAction::getDefaultText();
}

void ColorMapAction::setDefaultColorMap(const QString& defaultColorMap)
{
    Q_ASSERT(!defaultColorMap.isEmpty());

    OptionAction::setDefaultText(defaultColorMap);
}

}
}
