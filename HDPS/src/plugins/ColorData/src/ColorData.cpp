#include "ColorData.h"
#include "Application.h"

#include <QtCore>
#include <QtDebug>

Q_PLUGIN_METADATA(IID "nl.lumc.ColorData")

using namespace hdps;

ColorData::~ColorData(void)
{
    
}

void ColorData::init()
{

}

uint ColorData::count()
{
    return static_cast<std::uint32_t>(_colors.size());
}

Dataset<DatasetImpl> ColorData::createDataSet() const
{
    return Dataset<DatasetImpl>(new Colors(_core, getName()));
}

// =============================================================================
// Color Data Set
// =============================================================================

QIcon Colors::getIcon() const
{
    return QIcon();
}

QIcon ColorDataFactory::getIcon() const
{
    return hdps::Application::getIconFont("FontAwesome").getIcon("palette");
}

// =============================================================================
// Factory
// =============================================================================

hdps::plugin::RawData* ColorDataFactory::produce()
{
    return new ColorData(this);
}
