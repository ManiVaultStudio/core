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

QIcon Colors::getIcon(const QColor& color /*= Qt::black*/) const
{
    return QIcon();
}

std::vector<std::uint32_t>& Colors::getSelectionIndices()
{
    return getSelection<Colors>()->indices;
}

void Colors::setSelectionIndices(const std::vector<std::uint32_t>& indices)
{
    getSelectionIndices() = indices;
}

bool Colors::canSelect() const
{
    return false;
}

bool Colors::canSelectAll() const
{
    return false;
}

bool Colors::canSelectNone() const
{
    return false;
}

bool Colors::canSelectInvert() const
{
    return false;
}

void Colors::selectAll()
{
}

void Colors::selectNone()
{
}

void Colors::selectInvert()
{
}

QIcon ColorDataFactory::getIcon(const QColor& color /*= Qt::black*/) const
{
    return hdps::Application::getIconFont("FontAwesome").getIcon("palette", color);
}

// =============================================================================
// Factory
// =============================================================================

hdps::plugin::RawData* ColorDataFactory::produce()
{
    return new ColorData(this);
}
