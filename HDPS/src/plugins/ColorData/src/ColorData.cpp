#include "ColorData.h"

#include "IndexSet.h"

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

hdps::Set* ColorData::createSet() const
{
    return new hdps::IndexSet(_core, getName());
}

// =============================================================================
// Factory
// =============================================================================

RawData* ColorDataFactory::produce()
{
    return new ColorData();
}
