#include "TextData.h"

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

hdps::DataSet* TextData::createDataSet() const
{
    return new Text(_core, getName());
}

// =============================================================================
// Factory
// =============================================================================

RawData* TextDataFactory::produce()
{
    return new TextData();
}

QIcon Text::getIcon() const
{
    return QIcon();
}
