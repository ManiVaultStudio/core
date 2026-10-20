// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "pointdata_export.h"
#include "../view/PointsViewWidget.h"

#include <Dataset.h>
#include <actions/WidgetAction.h>

#include <QPointer>

class Points;

/**
 * @brief Embeds the reusable point-data view in an action hierarchy.
 *
 * PointsViewAction provides the point-data table and its associated access
 * controls as one self-contained action widget. It exists so that PointsAction
 * can remain a regular group action and arrange the storage-backend action and
 * the view through the standard action system. The action does not own the
 * dataset; it stores the dataset handle and passes it to the embedded widget.
 *
 * @author Thomas Kroes
 */
class POINTDATA_EXPORT PointsViewAction final : public mv::gui::WidgetAction
{
    Q_OBJECT

protected:
    /** Widget implementation that owns the embedded point-data view. */
    class POINTDATA_EXPORT Widget final : public mv::gui::WidgetActionWidget
    {
    public:
        /** Constructs the action widget and its embedded point-data view. */
        Widget(QWidget* parent, PointsViewAction* pointsViewAction);
    };

    /** Creates the custom widget representation of this action. */
    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override;

public:
    /**
     * Constructs a point-data view action for points.
     *
     * The dataset handle is retained by the action and passed to each widget
     * instance. The action does not own the dataset implementation.
     */
    PointsViewAction(QObject* parent, const mv::Dataset<Points>& points);

    /** Returns the point-data source displayed by the embedded view. */
    const mv::Dataset<Points>& getPoints() const;

    /** Returns the current settings of the embedded point-data view. */
    PointsViewWidget::ViewState getViewState() const;

private:
    mv::Dataset<Points>       _points;           /**< Point-data source displayed by the view. */
    QPointer<PointsViewWidget> _pointsViewWidget; /**< Embedded point-data view, when its widget exists. */
};
