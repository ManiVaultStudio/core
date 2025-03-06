// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"
#include "StyledIconCommon.h"

#include <QIconEngine>
#include <QObject>
#include <QPalette>

namespace mv::util
{

class StyledIcon;

/**
 * Styled icon engine class
 *
 * Icon engine for creating theme-specific icons of the StyledIcon.
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT StyledIconEngine : public QIconEngine
{
public:

    /**
     * Construct from \p styledIconSettings
     * @param styledIconSettings Reference to styled icon settings
     */
    StyledIconEngine(const StyledIconSettings& styledIconSettings);

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
     * @param size Size of the re-colored pixmap
     * @param color New color
     * @return Re-colored pixmap
     */
    static QPixmap recolorPixmap(const QPixmap& pixmap, const QSize& size, const QColor& color);

private:
    StyledIconSettings      _iconSettings;      /** Icon settings */

    friend class StyledIcon;
};


}
