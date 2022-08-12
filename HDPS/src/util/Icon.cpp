#include "Icon.h"

#include <QPainter>
#include <QApplication>
#include <QPalette>

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

QIcon createPluginIcon(const QString& characters)
{
    const auto margin       = 8;
    const auto pixmapSize   = QSize(256, 256);
    const auto pixmapRect   = QRect(QPoint(), pixmapSize).marginsRemoved(QMargins(margin, margin, margin, margin));
    const auto halfSize     = pixmapRect.size() / 2;

    QPixmap pixmap(pixmapSize);

    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);

    painter.setRenderHint(QPainter::Antialiasing);

    const auto textColor = QApplication::palette().text().color();

    painter.setPen(QPen(textColor, 1, Qt::SolidLine, Qt::SquareCap, Qt::SvgMiterJoin));

    auto font = QFont("Arial", 100);

    font.setBold(true);

    painter.setFont(font);

    const auto textOption = QTextOption(Qt::AlignCenter);

    if (characters.length() >= 1)
        painter.drawText(QRect(pixmapRect.topLeft(), halfSize), characters[0], textOption);

    if (characters.length() >= 2)
        painter.drawText(QRect(QPoint(halfSize.width(), pixmapRect.top()), halfSize), characters[1], textOption);

    if (characters.length() >= 3)
        painter.drawText(QRect(QPoint(pixmapRect.left(), halfSize.height()), halfSize), characters[2], textOption);

    if (characters.length() >= 4)
        painter.drawText(QRect(QPoint(halfSize.width(), halfSize.height()), halfSize), characters[3], textOption);

    return createIcon(pixmap);
}

QIcon combineIconsHorizontally(const QVector<QIcon>& icons)
{
    QSet<QSize> sizes;

    auto combinedWidth = 0u;

    for (const auto& icon : icons) {
        const auto largestSize = icon.availableSizes().first();

        sizes.insert(largestSize);

        combinedWidth += largestSize.width();
    }
        
    if (sizes.count() >= 2)
        return QIcon();

    const auto iconSize = sizes.values().first();

    QPixmap pixmap(QSize(combinedWidth, iconSize.height()));

    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);

    painter.setRenderHint(QPainter::Antialiasing);

    auto offset = 0u;

    for (const auto& icon : icons) {
        painter.drawPixmap(offset, 0, icon.pixmap(iconSize));
        offset += iconSize.width();
    }

    return createIcon(pixmap);
}

}
}
