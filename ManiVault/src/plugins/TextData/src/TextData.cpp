// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "TextData.h"

#include <Application.h>

#include "InfoAction.h"

#include <QtCore>
#include <QtDebug>

Q_PLUGIN_METADATA(IID "studio.manivault.TextData")

TextData::~TextData(void)
{
}

void TextData::init()
{
}

Dataset<DatasetImpl> TextData::createDataSet(const QString& guid /*= ""*/) const
{
    return Dataset<DatasetImpl>(new Text(getName(), true, guid));
}

QIcon TextDataFactory::getIcon(const QColor& color /*= Qt::black*/) const
{
    return Application::getIconFont("FontAwesome").getIcon("font", color);
}

QUrl TextDataFactory::getReadmeMarkdownUrl() const
{
#ifdef ON_LEARNING_CENTER_FEATURE_BRANCH
    return QUrl("https://raw.githubusercontent.com/ManiVaultStudio/core/feature/learning_center/ManiVault/src/plugins/TextData/README.md");
#else
    return QUrl("https://raw.githubusercontent.com/ManiVaultStudio/core/master/ManiVault/src/plugins/TextData/README.md");
#endif
}

QUrl TextDataFactory::getRespositoryUrl() const
{
    return QUrl("https://github.com/ManiVaultStudio/core");
}

mv::plugin::RawData* TextDataFactory::produce()
{
    return new TextData(this);
}

void Text::init()
{
    DatasetImpl::init();

    _infoAction = new InfoAction(this, *this);
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

/* -------------------------------------------------------------------------- */
/*                               Action getters                               */
/* -------------------------------------------------------------------------- */

InfoAction& Text::getInfoAction()
{
    return *_infoAction;
}
