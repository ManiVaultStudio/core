// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ThemeIconEngine.h"
#include "NamedIcon.h"

#include <QDebug>

namespace mv::util
{

ThemeIconEngine::ThemeIconEngine(NamedIcon& namedIcon) :
    QIconEngine(),
    _namedIcon(namedIcon)
{
}

ThemeIconEngine::ThemeIconEngine(const ThemeIconEngine& other) :
    ThemeIconEngine(other._namedIcon)
{
}

void ThemeIconEngine::paint(QPainter* painter, const QRect& rect, QIcon::Mode mode, QIcon::State state)
{
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->drawPixmap(rect, _namedIcon.getIconPixmap(qApp->palette().text().color()));
}

QPixmap ThemeIconEngine::pixmap(const QSize& size, QIcon::Mode mode, QIcon::State state)
{
    const auto pixmap = _namedIcon.getIconPixmap(qApp->palette().text().color());

    if (pixmap.isNull())
        return {};
    
    return pixmap.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
}

QIconEngine* ThemeIconEngine::clone() const
{
    return new ThemeIconEngine(*this);
}

}
