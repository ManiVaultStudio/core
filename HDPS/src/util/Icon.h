#pragma once

#include <QString>
#include <QFont>
#include <QIcon>
#include <QMap>
#include <QDebug>

namespace hdps {

namespace gui {

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

}

}