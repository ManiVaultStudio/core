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

    /**
     * Construct from \p namedIcon
     * @param namedIcon Reference to named icon
     */
    ThemeIconEngine(NamedIcon& namedIcon);

    /**
     * Copy construct from \p other
     * @param other Other theme icon engine to copy from
     */
    ThemeIconEngine(const ThemeIconEngine& other);

    /** No need for custom destructor */
    ~ThemeIconEngine() override = default;

    /**
     * Paint the icon
     * @param painter Pointer to painter
     * @param rect Rectangle to paint
     * @param mode Mode of the icon
     * @param state State of the icon
     */
    void paint(QPainter* painter, const QRect& rect, QIcon::Mode mode, QIcon::State state) override;

    /**
     * Get pixmap
     * @param size Size of the pixmap
     * @param mode Mode of the icon
     * @param state State of the icon
     * @return Pixmap
     */
    QPixmap pixmap(const QSize& size, QIcon::Mode mode, QIcon::State state) override;

    /**
     * Clone the theme icon engine
     * @return Pointer to cloned theme icon engine
     */
    QIconEngine* clone() const override;

private:

    /**
     * Re-color \p pixmap with \p color
     * @param pixmap Pixmap to re-color
     * @param color New color
     * @return Re-colored pixmap
     */
    static QPixmap recolorPixmap(const QPixmap& pixmap, const QColor& color);

private:
    NamedIcon&   _namedIcon;    /** Reference to named icon */
};


}
