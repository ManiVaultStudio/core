#include "TextData.h"

#include <QtCore>
#include <QtDebug>

Q_PLUGIN_METADATA(IID "hdps.TextData")

TextData::~TextData(void)
{
}

void TextData::init()
{
}

Dataset<DatasetImpl> TextData::createDataSet(const QString& guid /*= ""*/) const
{
    return Dataset<DatasetImpl>(new Text(Application::core(), getName(), guid));
}

QIcon TextDataFactory::getIcon(const QColor& color /*= Qt::black*/) const
{
    return Application::getIconFont("FontAwesome").getIcon("font", color);
}

hdps::plugin::RawData* TextDataFactory::produce()
{
    return new TextData(this);
}

QIcon Text::getIcon(const QColor& color /*= Qt::black*/) const
{
    return Application::getIconFont("FontAwesome").getIcon("font", color);
}

std::vector<std::uint32_t>& Text::getSelectionIndices()
{
    return getSelection<Text>()->indices;
}

void Text::setSelectionIndices(const std::vector<std::uint32_t>& indices)
{
}

bool Text::canSelect() const
{
    return false;
}

bool Text::canSelectAll() const
{
    return false;
}

bool Text::canSelectNone() const
{
    return false;
}

bool Text::canSelectInvert() const
{
    return false;
}

void Text::selectAll()
{
}

void Text::selectNone()
{
}

void Text::selectInvert()
{
}
