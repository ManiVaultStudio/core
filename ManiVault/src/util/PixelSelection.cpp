// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "PixelSelection.h"

#include <QDebug>
#include <QApplication>
#include <QPalette>
#include <QIcon>
#include <QPainter>
#include <QPainterPath>
#include <QtMath>

namespace mv::util {

QIcon getPixelSelectionTypeIcon(const PixelSelectionType& selectionType)
{
    const auto margin               = 5;
    const auto pixmapSize           = QSize(100, 100);
    const auto pixmapRectDeflated   = QRect(QPoint(), pixmapSize).marginsRemoved(QMargins(margin, margin, margin, margin));

    // Create pixmap
    QPixmap pixmap(pixmapSize);

    // Fill with a transparent background
    pixmap.fill(Qt::transparent);

    // Create a painter to draw in the pixmap
    QPainter painter(&pixmap);

    // Enable anti-aliasing
    painter.setRenderHint(QPainter::Antialiasing);

    // Get the text color from the application
    const auto textColor = QApplication::palette().text().color();

    // Configure painter
    painter.setPen(QPen(textColor, 10, Qt::DotLine, Qt::SquareCap, Qt::SvgMiterJoin));

    switch (selectionType)
    {
        case PixelSelectionType::Rectangle:
        {
            painter.drawRect(pixmapRectDeflated);
            break;
        }
        case PixelSelectionType::Line:
        {
            // Draw a line from the top-left to the bottom-right of the deflated rectangle
            painter.drawLine(pixmapRectDeflated.topLeft(), pixmapRectDeflated.bottomRight());
            break;
        }
        case PixelSelectionType::Brush:
        {
            painter.drawEllipse(pixmapRectDeflated.center(), 45, 45);
            break;
        }

        case PixelSelectionType::Lasso:
        {
            QVector<QPoint> polygonPoints;

            polygonPoints << QPoint(5, 8);
            polygonPoints << QPoint(80, 28);
            polygonPoints << QPoint(92, 90);
            polygonPoints << QPoint(45, 60);
            polygonPoints << QPoint(10, 80);

            polygonPoints << polygonPoints[0];
            polygonPoints << polygonPoints[1];

            QPainterPath testCurve;

            QVector<QVector<QPoint>> curves;

            for (int pointIndex = 1; pointIndex < polygonPoints.count() - 1; pointIndex++) {
                const auto pPrevious    = polygonPoints[pointIndex - 1];
                const auto p            = polygonPoints[pointIndex];
                const auto pNext        = polygonPoints[pointIndex + 1];
                const auto pC0          = pPrevious + ((p - pPrevious) / 2);
                const auto pC1          = p + ((pNext - p) / 2);

                curves << QVector<QPoint>({ pC0, p, pC1 });
            }

            testCurve.moveTo(curves.first().first());

            for (auto curve : curves)
                testCurve.cubicTo(curve[0], curve[1], curve[2]);

            painter.drawPath(testCurve);

            break;
        }

        case PixelSelectionType::Polygon:
        {
            QVector<QPoint> points;

            points << QPoint(10, 10);
            points << QPoint(90, 45);
            points << QPoint(25, 90);

            painter.drawPolygon(points);

            break;
        }

        case PixelSelectionType::Sample:
        {
            painter.setPen(QPen(textColor, 10, Qt::SolidLine, Qt::SquareCap, Qt::SvgMiterJoin));

            // Radii and center of the cross hair
            const auto r1       = 5.0f, r2 = 25.0f, r3 = 50.0f - margin;
            const auto center   = QPoint(50, 50);

            // Draw four orthogonal lines
            for (int step = 0; step < 4; step++) {

                // Compute angle in radians
                const auto angle = qDegreesToRadians(step * 90.0f);

                // Draw inner line
                painter.drawPolyline(QVector<QPoint>({
                    center + QPoint(0, 0),
                    center + QPoint(r1 * sin(angle), r1 * cos(angle))
                    }));

                // Draw outer line
                painter.drawPolyline(QVector<QPoint>({
                    center + QPoint(r2 * sin(angle), r2 * cos(angle)),
                    center + QPoint(r3 * sin(angle), r3 * cos(angle))
                    }));
            }

            break;
        }

        case PixelSelectionType::ROI:
        {
            // Draw outside rectangle
            painter.setPen(QPen(textColor, 10, Qt::SolidLine, Qt::SquareCap, Qt::SvgMiterJoin));
            painter.drawRect(pixmapRectDeflated);

            // Draw inside rectangle filled
            painter.setPen(Qt::NoPen);
            painter.setBrush(textColor);

            const auto roiMargin = 5 * margin;

            painter.drawRect(pixmapRectDeflated.marginsRemoved(QMargins(roiMargin, roiMargin, roiMargin, roiMargin)));

            break;
        }

        default:
            break;
    }

    return QIcon(pixmap);
}

PixelSelectionTypeModel::PixelSelectionTypeModel(QObject* parent /*= nullptr*/) :
    QAbstractListModel(parent),
    _pixelSelectionTypes()
{
}

void PixelSelectionTypeModel::setPixelSelectionTypes(const PixelSelectionTypes& pixelSelectionTypes)
{
    if (pixelSelectionTypes == _pixelSelectionTypes)
        return;

    _pixelSelectionTypes = pixelSelectionTypes;
}

int PixelSelectionTypeModel::rowCount(const QModelIndex& parent /*= QModelIndex()*/) const
{
    return _pixelSelectionTypes.count();
}

int PixelSelectionTypeModel::columnCount(const QModelIndex& parent /*= QModelIndex()*/) const
{
    return 1;
}

QVariant PixelSelectionTypeModel::data(const QModelIndex& index, int role) const
{
    // Get column and line type
    const auto column   = index.column();
    const auto lineType = _pixelSelectionTypes[index.row()];

    switch (role)
    {
        // Icon representation
        //case Qt::DecorationRole:
        //    return getPixelSelectionTypeIcon(lineType);

        // String representation
        case Qt::DisplayRole:
            return getPixelSelectionTypeName(lineType);

        default:
            break;
    }

    return QVariant();
}

}
