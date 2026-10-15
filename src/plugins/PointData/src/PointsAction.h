// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "pointdata_export.h"

#include <Dataset.h>
#include <actions/GroupAction.h>

class Points;

/**
 * @brief Point-data action section containing the interactive points view.
 *
 * PointsAction embeds the reusable PointsViewWidget in the PointData action
 * hierarchy. This makes the lazy table and facade-access controls available
 * next to the regular point-data information actions, while keeping the
 * widget implementation independent from the Points View plugin wrapper.
 *
 * The action owns only the action-widget shell. Each action widget owns its
 * PointsViewWidget through the normal Qt parent-child relationship. The view
 * remains read-only with respect to point values and uses the selected data
 * hierarchy item as its data source.
 *
 * @author Thomas Kroes
 */
class POINTDATA_EXPORT PointsAction final : public mv::gui::GroupAction
{
    Q_OBJECT

protected:
    /** Widget implementation that embeds the reusable PointsViewWidget. */
    class POINTDATA_EXPORT Widget final : public mv::gui::WidgetActionWidget
    {
    public:
        /** Constructs the action widget and its embedded points view. */
        Widget(QWidget* parent, PointsAction* pointsAction);
    };

    /** Creates the custom widget representation of this action. */
    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override;

public:
    /** Constructs the point-data action section. */
    PointsAction(QObject* parent, const mv::Dataset<Points>& points);

    /** Returns the point-data source displayed by the embedded widget. */
    const mv::Dataset<Points>& getPoints() const;

private:
    mv::Dataset<Points> _points; /**< Point-data source displayed by the action. */
};
