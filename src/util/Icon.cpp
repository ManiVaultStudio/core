// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "Icon.h"

#include <QPainter>
#include <QPalette>
#include <QSet>

using namespace mv::util;

namespace mv::gui {

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

QIcon combineIcons(const QIcon& iconA, const QIcon& iconB)
{
    auto pixmapA = iconA.pixmap(defaultIconPixmapSizes.first());
    auto pixmapB = iconB.pixmap(defaultIconPixmapSizes.first());

    QPainter painter(&pixmapA);

    painter.drawPixmap(0, 0, pixmapB);

    return createIcon(pixmapA);
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

    painter.setPen(QPen(Qt::black, 1, Qt::SolidLine, Qt::SquareCap, Qt::SvgMiterJoin));

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

QPixmap createNumberBadgeOverlayPixmap(std::uint32_t number /*= 0*/, const QColor& backgroundColor /*= Qt::red*/, const QColor& foregroundColor /*= Qt::white*/)
{
    const auto badgeSize        = QSize(100, 100);
    const auto badgeRectangle   = QRectF(QPoint(0, 0), badgeSize);

    QPixmap badgePixmap(badgeSize);

    badgePixmap.fill(Qt::transparent);

    QPainter badgePixmapPainter(&badgePixmap);

    badgePixmapPainter.setRenderHint(QPainter::Antialiasing);
    badgePixmapPainter.setRenderHint(QPainter::Antialiasing);
    badgePixmapPainter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    badgePixmapPainter.setRenderHint(QPainter::LosslessImageRendering, true);

    badgePixmapPainter.setPen(QPen(backgroundColor, 100, Qt::SolidLine, Qt::RoundCap));
    badgePixmapPainter.drawPoint(QPoint(50, 50));

    badgePixmapPainter.setPen(QPen(foregroundColor));

    if (number < 100) {
        badgePixmapPainter.setFont(QFont("Arial", number < 10 ? 52 : 40, 900));
        badgePixmapPainter.drawText(badgeRectangle, QString::number(number), QTextOption(Qt::AlignCenter));
    }
    else {
        badgePixmapPainter.setPen(QPen(foregroundColor, 15, Qt::SolidLine, Qt::RoundCap));

        badgePixmapPainter.drawPoint(QPoint(25, 50));
        badgePixmapPainter.drawPoint(QPoint(50, 50));
        badgePixmapPainter.drawPoint(QPoint(75, 50));
    }

    return badgePixmap;
}

QPixmap createNumberBadgeOverlayPixmap(const Badge& widgetActionBadge)
{
    return createNumberBadgeOverlayPixmap(widgetActionBadge.getNumber(), widgetActionBadge.getBackgroundColor(), widgetActionBadge.getForegroundColor());
}

QIcon createIconWithNumberBadgeOverlay(const QIcon& icon, const Badge& widgetActionBadge, std::uint32_t iconMargin /*= 0*/)
{
    if (icon.availableSizes().isEmpty())
        return {};

    const auto iconPixmapSize       = icon.availableSizes().first() + QSize(2 * iconMargin, 2 * iconMargin);
    const auto iconPixmapRectangle  = QRectF(QPoint(0, 0), iconPixmapSize);
    const auto badgePixmap          = widgetActionBadge.getPixmap();
    const auto badgeRectangle       = QRectF(QPoint(0, 0), badgePixmap.size());

    QPixmap iconPixmap(iconPixmapSize);

    iconPixmap.fill(Qt::transparent);

    QPainter iconPixmapPainter(&iconPixmap);

    iconPixmapPainter.setRenderHint(QPainter::Antialiasing);
    iconPixmapPainter.drawPixmap(QRectF(QPoint(iconMargin, iconMargin), iconPixmapSize), icon.pixmap(iconPixmapSize), iconPixmapRectangle);

    if (widgetActionBadge.getEnabled()) {
        const auto badgeScale           = widgetActionBadge.getScale();
        const auto scaledIconPixmapSize = badgeScale * iconPixmapSize;
        const auto badgeAlignment       = widgetActionBadge.getAlignment();

        QPoint origin;

        if (badgeAlignment & Qt::AlignTop)
            origin.setY(0);

        if (badgeAlignment & Qt::AlignVCenter)
            origin.setY((0.5 * iconPixmapSize.height()) - ((0.5 * badgeScale) * iconPixmapSize.height()));

        if (badgeAlignment & Qt::AlignBottom)
            origin.setY((1.0f - badgeScale) * iconPixmapSize.height());

        if (badgeAlignment & Qt::AlignLeft)
            origin.setX(0);

        if (badgeAlignment & Qt::AlignHCenter)
            origin.setX((0.5 * iconPixmapSize.width()) - ((0.5 * badgeScale) * iconPixmapSize.width()));

        if (badgeAlignment & Qt::AlignRight)
            origin.setX((1.0f - badgeScale) * iconPixmapSize.width());

        iconPixmapPainter.drawPixmap(QRectF(origin, scaledIconPixmapSize), badgePixmap, badgeRectangle);
    }

    return createIcon(iconPixmap);
}

}
