// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "NamedIcon.h"
#include "Application.h"

#include <QDebug>

namespace mv::util
{
NamedIcon::NamedIcon(const QString& iconName, const QString& iconFontName, const Version& iconFontVersion) :
    _iconName(iconName),
    _iconFontName(iconFontName),
    _iconFontVersion(iconFontVersion)
{
    connect(&_themeWatcher, &ThemeWatcher::paletteChanged, this, &NamedIcon::updateIcon);

    updateIcon();
}

NamedIcon NamedIcon::fromFontAwesome(const QString& iconName, const Version& version /*= { 6, 5 }*/)
{
    return NamedIcon(iconName, "FontAwesome", version);
}

void NamedIcon::updateIcon()
{
    const auto textColor = qApp->palette().text().color();

    static_cast<QIcon&>(*this) = Application::getIconFont(_iconFontName, _iconFontVersion.getMajor(), _iconFontVersion.getMinor()).getIcon(_iconName, textColor);

	//QIcon::addPixmap(QPixmap());  // Trick to trigger repaint
}

}