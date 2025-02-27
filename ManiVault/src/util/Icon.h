// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include "util/Badge.h" 

#include <QIcon>

namespace mv::gui {

static const QList<QSize> defaultIconPixmapSizes = QList({QSize(64, 64), QSize(32, 32), QSize(24, 24), QSize(16, 16), QSize(8, 8) });

/**
 * Convenience function to create icon from a \p pixmap and for \p pixmapSizes
 * @param pixmap Source pixmap
 * @param pixmapSizes Pixmap size in the icon
 * @return Icon
 */
CORE_EXPORT QIcon createIcon(const QPixmap& pixmap, const QList<QSize>& pixmapSizes = defaultIconPixmapSizes);

/**
 * Convenience function to overlay \p icon pixmap(s) with an overlay \p pixmap
 * @param icon Source icon
 * @param overlay Overlay pixmap
 * @return Overlay icon
 */
CORE_EXPORT QIcon createOverlayIcon(const QIcon& icon, const QPixmap& overlay);

/**
 * Draws \p iconB on top of \p iconA
 * @param iconA Icon A
 * @param iconB Icon B (overlays iconA)
 * @return Combined icon
 */
CORE_EXPORT QIcon combineIcons(const QIcon& iconA, const QIcon& iconB);

/**
 * Convenience function to create a plugin icon, which consists of maximum of four characters laid out in a grid
 * @param characters Characters
 * @return Plugin icon
 */
CORE_EXPORT QIcon createPluginIcon(const QString& characters);

/**
 * Combines two (or more) icons into a horizontal icon
 * @param icons Vector of input icons (need to be the same height)
 * @return Combined icon
 */
CORE_EXPORT QIcon combineIconsHorizontally(const QVector<QIcon>& icons);

/**
 * Creates a pixmap with a number badge from \p number, \p badgeBackgroundColor and \p foregroundColor
 * @param number Integral to display in the badge
 * @param backgroundColor Color of the badge background
 * @param foregroundColor Color of the badge foreground (text)
 * @return Number badge pixmap
 */
CORE_EXPORT QPixmap createNumberBadgeOverlayPixmap(std::uint32_t number /*= 0*/, const QColor& backgroundColor /*= Qt::red*/, const QColor& foregroundColor /*= Qt::white*/);

/**
 * Creates a pixmap with a number badge from \p widgetActionBadge
 * @param widgetActionBadge Widget action badge configuration
 * @return Number badge pixmap
 */
CORE_EXPORT QPixmap createNumberBadgeOverlayPixmap(const util::Badge& widgetActionBadge);

/**
 * Overlays \p icon with a \p number badge in \p color and with \p alignment
 * @param icon Icon to overlay
 * @param widgetActionBadge Widget action badge configuration
 * @param iconMargin Icon margin
 * @return Icon with badge overlay
 */
CORE_EXPORT QIcon createIconWithNumberBadgeOverlay(const QIcon& icon, const util::Badge& widgetActionBadge, std::uint32_t iconMargin = 0);

}