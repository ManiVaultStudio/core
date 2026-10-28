// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <PointData/pointdata_export.h>
#include <actions/WidgetAction.h>
#include <util/SeverityLevel.h>

#include <QList>
#include <QPointer>

namespace mv::gui
{

/**
 * @brief Displays a severity-coded observation for tiled-storage settings.
 *
 * The action stores its severity and description independently of its widgets.
 * Existing widgets are updated when the observation changes, and widgets
 * created later display the latest state. The parent action owns this action;
 * QWidgetAction manages the lifetime of its created widgets.
 *
 * @author T. Kroes
 */
class POINTDATA_EXPORT TiledStorageObservationAction : public WidgetAction
{
public:
    /**
     * Constructs an observation action with a fixed category heading.
     * @param parent Owning action or QObject.
     * @param category Heading shown beside the severity.
     */
    TiledStorageObservationAction(QObject* parent, const QString& category);

    /**
     * Updates the stored state and every currently live widget.
     * @param severity Severity used for the icon, label, and tint.
     * @param description User-facing explanation of the observation.
     */
    void setObservation(util::SeverityLevel severity, const QString& description);

protected:
    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override;

private:
    void updateWidget(QWidget* widget) const;

    util::SeverityLevel      _severity = util::SeverityLevel::Info;
    QString                  _description;
    QList<QPointer<QWidget>> _widgets;
};

}
