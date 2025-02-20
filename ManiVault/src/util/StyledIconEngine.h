// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

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
class CORE_EXPORT StyledIconEngine : public QObject, public QIconEngine
{
public:

    /**
     * Construct from \p namedIcon
     * @param styledIcon Reference to styled icon
     */
    StyledIconEngine(StyledIcon& styledIcon);

    /**
     * Construct from parameters
     * @param styledIcon Reference to styled icon
     * @param sha SHA of the icon
     * @param colorGroupLightTheme Color group for light theme
     * @param colorGroupDarkTheme Color group for dark theme
     * @param colorRoleLightTheme Color role for light theme
     * @param colorRoleDarkTheme Color role for dark theme
     */
    StyledIconEngine(StyledIcon& styledIcon, const QString& sha, const QPalette::ColorGroup& colorGroupLightTheme, const QPalette::ColorGroup& colorGroupDarkTheme, const QPalette::ColorRole& colorRoleLightTheme, const QPalette::ColorRole& colorRoleDarkTheme);

    /**
     * Copy construct from \p other
     * @param other Other theme icon engine to copy from
     */
    StyledIconEngine(const StyledIconEngine& other);

    /** No need for custom destructor */
    ~StyledIconEngine() override;

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

    /**
     * Get color group for current theme
     * @return Color group for current theme
     */
    QPalette::ColorGroup getColorGroupForCurrentTheme() const;

    /**
     * Get color role for current theme
     * @return Color role for current theme
     */
    QPalette::ColorRole getColorRoleForCurrentTheme() const;

private:
    StyledIcon&             _styledIcon;                /** Reference to styled icon */
    QString                 _sha;                       /** NamedIcons::icons key */
    QPalette::ColorGroup    _colorGroupLightTheme;      /** Color group for light theme */
    QPalette::ColorGroup    _colorGroupDarkTheme;       /** Color group for dark theme */
    QPalette::ColorRole     _colorRoleLightTheme;       /** Color role for light theme */
    QPalette::ColorRole     _colorRoleDarkTheme;        /** Color role for dark theme */

    friend class StyledIcon;
};


}
