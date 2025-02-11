// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include <QIconEngine>
#include <QObject>

namespace mv::util
{

class NamedIcon;

/**
 * Theme icon engine class
 *
 * Icon engine for creating theme-specific icons.
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT ThemeIconEngine : public QObject, public QIconEngine
{
    Q_OBJECT

public:
    ThemeIconEngine(NamedIcon& namedIcon);
    ThemeIconEngine(const ThemeIconEngine& other);
    ~ThemeIconEngine() override = default;

    void paint(QPainter* painter, const QRect& rect, QIcon::Mode mode, QIcon::State state) override;
    QPixmap pixmap(const QSize& size, QIcon::Mode mode, QIcon::State state) override;
    QIconEngine* clone() const override;

private:
    NamedIcon&   _namedIcon;
};


}