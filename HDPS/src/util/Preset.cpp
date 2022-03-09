#include "Preset.h"

namespace hdps {

namespace util {

Preset::Preset(const QString& name, const QString& description, const PresetType& presetType /*= PresetType::Normal*/) :
    _name(name),
    _description(description),
    _presetType(presetType)
{
}

QString Preset::getName() const
{
    return _name;
}

QString Preset::getDescription() const
{
    return _description;
}

QIcon Preset::getIcon() const
{
    switch (_presetType)
    {
        case PresetType::Normal:
            return Application::getIconFont("FontAwesome").getIcon("sliders-h");

        case PresetType::Global:
            return Application::getIconFont("FontAwesome").getIcon("globe-europe");

        case PresetType::FactoryDefault:
            return Application::getIconFont("FontAwesome").getIcon("industry");

        default:
            break;
    }

    return QIcon();
}

bool Preset::isDivergent() const
{
    return _isDivergent;
}

void Preset::isDivergent(bool isDivergent)
{
    _isDivergent = isDivergent;
}

}
}
