// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ThemeIconEngine.h"
#include "NamedIcon.h"

#include <QDebug>
#include <QPainter>

namespace mv::util
{

ThemeIconEngine::ThemeIconEngine(NamedIcon& namedIcon) :
    _namedIcon(namedIcon)
{
}

ThemeIconEngine::ThemeIconEngine(const ThemeIconEngine& other) :
    ThemeIconEngine(other._namedIcon)
{
}

void ThemeIconEngine::paint(QPainter* painter, const QRect& rect, QIcon::Mode mode, QIcon::State state)
{
}

QPixmap ThemeIconEngine::pixmap(const QSize& size, QIcon::Mode mode, QIcon::State state)
{
    const auto pixmap = _namedIcon.getPixmap();

    if (!pixmap.isNull()) {
        const auto recoloredPixmap  = recolorPixmap(pixmap, qApp->palette().color(QPalette::ColorGroup::Normal, _namedIcon.getColorRoleForCurrentTheme()));
        const auto recoloredIcon    = QIcon(recoloredPixmap);

        return recoloredIcon.pixmap(size, mode, state);
    }

	return {};
}

QIconEngine* ThemeIconEngine::clone() const
{
    return new ThemeIconEngine(*this);
}

QPixmap ThemeIconEngine::recolorPixmap(const QPixmap& pixmap, const QColor& color)
{
    auto image = pixmap.toImage();

    QPainter painter(&image);

    painter.setCompositionMode(QPainter::CompositionMode_SourceAtop);
    painter.fillRect(image.rect(), color);
    painter.end();

    return { QPixmap::fromImage(image) };
}

}
