#include "TextData.h"

#include <QtCore>
#include <QtDebug>

Q_PLUGIN_METADATA(IID "hdps.TextData")

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

hdps::plugin::RawData* TextDataFactory::produce()
{
    return new TextData(this);
}

QIcon Text::getIcon() const
{
    return QIcon();
}
