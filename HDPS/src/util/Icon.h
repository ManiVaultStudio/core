// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "actions/WidgetActionBadge.h" 

#include <QString>
#include <QFont>
#include <QIcon>
#include <QMap>
#include <QDebug>

namespace mv::gui {

static const QList<QSize> defaultIconPixmapSizes = QList({QSize(64, 64), QSize(32, 32), QSize(24, 24), QSize(16, 16), QSize(8, 8) });

/**
 * Convenience function to create icon from a \p pixmap and for \p pixmapSizes
 * @param pixmap Source pixmap
 * @param pixmapSizes Pixmap size in the icon
 * @return Icon
 */
QIcon createIcon(const QPixmap& pixmap, const QList<QSize>& pixmapSizes = defaultIconPixmapSizes);

/**
 * Convenience function to overlay \p icon pixmap(s) with an overlay \p pixmap
 * @param icon Source icon
 * @param overlay Overlay pixmap
 * @return Overlay icon
 */
QIcon createOverlayIcon(const QIcon& icon, const QPixmap& overlay);

/**
 * Convenience function to create a plugin icon, which consists of maximum of four characters laid out in a grid
 * @param characters Characters
 * @param color Icon color
 * @return Plugin icon
 */
QIcon createPluginIcon(const QString& characters, const QColor& color);

/**
 * Combines two (or more) icons into a horizontal icon
 * @param icons Vector of input icons (need to be the same height)
 * @return Combined icon
 */
QIcon combineIconsHorizontally(const QVector<QIcon>& icons);

/**
 * Creates a pixmap with a number badge from \p number, \p badgeBackgroundColor and \p foregroundColor
 * @param number Integral to display in the badge
 * @param backgroundColor Color of the badge background
 * @param foregroundColor Color of the badge foreground (text)
 * @return Number badge pixmap
 */
QPixmap createNumberBadgeOverlayPixmap(std::uint32_t number /*= 0*/, const QColor& backgroundColor /*= Qt::red*/, const QColor& foregroundColor /*= Qt::white*/);

/**
 * Creates a pixmap with a number badge from \p widgetActionBadge
 * @param widgetActionBadge Widget action badge configuration
 * @return Number badge pixmap
 */
QPixmap createNumberBadgeOverlayPixmap(const WidgetActionBadge& widgetActionBadge);

/**
 * Overlays \p icon with a \p number badge in \p color and with \p alignment
 * @param icon Icon to overlay
 * @param iconMargin Icon margin
 * @param number Integral to display in the badge
 * @param badgeBackgroundColor Color of the badge background
 * @param badgeForegroundColor Color of the badge foreground
 * @param badgeAlignment Icon to overlay
 * @param badgeScale Scale of the icon [0, 1]
 * @return Icon with badge overlay
 */
QIcon createIconWithNumberBadgeOverlay(const QIcon& icon, std::uint32_t iconMargin = 0, std::uint32_t badgeNumber = 0, const QColor& badgeBackgroundColor = Qt::red, const QColor& badgeForegroundColor = Qt::white, Qt::Alignment badgeAlignment = Qt::AlignTop | Qt::AlignRight, float badgeScale = 0.5f);

}