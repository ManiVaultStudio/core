#include "Icon.h"

#include <QPainter>

namespace hdps {

namespace gui {

QIcon createIcon(const QPixmap& pixmap, const QList<QSize>& pixmapSizes /*= defaultIconPixmapSizes*/)
{
    QIcon icon;

    const auto addPixmap = [&icon, &pixmap](const QSize& iconSize) -> void {
        icon.addPixmap(pixmap.scaled(iconSize, Qt::AspectRatioMode::IgnoreAspectRatio, Qt::TransformationMode::SmoothTransformation));
    };

    for (const auto& pixmapSize : pixmapSizes)
        addPixmap(pixmapSize);

    return icon;
}

QIcon createOverlayIcon(const QIcon& icon, const QPixmap& overlay)
{
    auto pixmap = icon.pixmap(defaultIconPixmapSizes.first());

    QPainter painter(&pixmap);

    painter.drawPixmap(0, 0, overlay);

    return createIcon(pixmap);
}

}
}
