#include "ColorData.h"

#include <QtCore>
#include <QtDebug>

Q_PLUGIN_METADATA(IID "nl.lumc.ColorData")

// =============================================================================
// Data
// =============================================================================

ColorData::~ColorData(void)
{
    
}

void ColorData::init()
{

}

uint ColorData::count()
{
    return _colors.size();
}

hdps::DataSet* ColorData::createDataSet() const
{
    return new Colors(_core, getName());
}

// =============================================================================
// Color Data Set
// =============================================================================

QIcon Colors::getIcon() const
{
    return QIcon();
}

// =============================================================================
// Factory
// =============================================================================

hdps::plugin::RawData* ColorDataFactory::produce()
{
    return new ColorData(this);
}
