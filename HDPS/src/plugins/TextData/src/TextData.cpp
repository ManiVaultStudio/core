#include "TextData.h"

#include "IndexSet.h"

#include <QtCore>
#include <QtDebug>

Q_PLUGIN_METADATA(IID "nl.tudelft.TextData")

// =============================================================================
// Data
// =============================================================================

TextData::~TextData(void)
{
    
}

void TextData::init()
{

}

hdps::Set* TextData::createSet() const
{
    return new hdps::IndexSet(_core, getName());
}

// =============================================================================
// Factory
// =============================================================================

RawData* TextDataFactory::produce()
{
    return new TextData();
}
