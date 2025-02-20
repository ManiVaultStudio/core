// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ColorData.h"
#include "Application.h"

#include <QtCore>
#include <QtDebug>

Q_PLUGIN_METADATA(IID "studio.manivault.ColorData")

using namespace mv;

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

Dataset<DatasetImpl> ColorData::createDataSet(const QString& guid /*= ""*/) const
{
    return Dataset<DatasetImpl>(new Colors(getName(), true, guid));
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
    return mv::StyledIcon("palette", color);
}

QUrl ColorDataFactory::getReadmeMarkdownUrl() const 
{
#ifdef ON_LEARNING_CENTER_FEATURE_BRANCH
    return QUrl("https://raw.githubusercontent.com/ManiVaultStudio/core/feature/learning_center/ManiVault/src/plugins/ColorData/README.md");
#else
    return QUrl("https://raw.githubusercontent.com/ManiVaultStudio/core/master/ManiVault/src/plugins/ColorData/README.md");
#endif
}

QUrl ColorDataFactory::getRepositoryUrl() const
{
    return QUrl("https://github.com/ManiVaultStudio/core");
}

mv::plugin::RawData* ColorDataFactory::produce()
{
    return new ColorData(this);
}
