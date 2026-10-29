// SPDX-License-Identifier: LGPL-3.0-or-later

#include "StorageBackendObservationAction.h"

#include <util/StyledIcon.h>

#include <QColor>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>

namespace mv::gui
{

namespace
{

QColor getSeverityColor(const util::SeverityLevel severity)
{
    switch (severity)
    {
        case util::SeverityLevel::Warning: return QColor(196, 150, 20);
        case util::SeverityLevel::Error: return QColor(196, 32, 32);
        case util::SeverityLevel::Fatal: return QColor(150, 0, 0);
        default: return QColor(80, 130, 190);
    }
}

}

StorageBackendObservationAction::StorageBackendObservationAction(QObject* parent, const QString& category) :
    WidgetAction(parent, category)
{
    setConfigurationFlag(ConfigurationFlag::NoLabelInGroup);
}

void StorageBackendObservationAction::setObservation(const util::SeverityLevel severity, const QString& description)
{
    _severity = severity;
    _description = description;

    for (const auto& widget : _widgets)
        if (widget)
            updateWidget(widget);
}

QWidget* StorageBackendObservationAction::getWidget(QWidget* parent, const std::int32_t&)
{
    auto* row = new QFrame(parent);
    row->setObjectName(QStringLiteral("StorageBackendObservationRow"));

    auto* layout = new QHBoxLayout(row);
    layout->setContentsMargins(8, 6, 8, 6);
    layout->setSpacing(8);

    auto* icon = new QLabel(row);
    icon->setObjectName(QStringLiteral("SeverityIcon"));
    icon->setFixedSize(20, 20);
    icon->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    layout->addWidget(icon);

    auto* textLayoutWidget = new QWidget(row);
    auto* textLayout = new QVBoxLayout(textLayoutWidget);
    textLayout->setContentsMargins(0, 0, 0, 0);
    textLayout->setSpacing(3);

    auto* headingLayoutWidget = new QWidget(textLayoutWidget);
    auto* headingLayout = new QHBoxLayout(headingLayoutWidget);
    headingLayout->setContentsMargins(0, 0, 0, 0);
    headingLayout->setSpacing(6);

    auto* severityLabel = new QLabel(headingLayoutWidget);
    severityLabel->setObjectName(QStringLiteral("SeverityLabel"));
    headingLayout->addWidget(severityLabel);

    auto* categoryLabel = new QLabel(text(), headingLayoutWidget);
    categoryLabel->setObjectName(QStringLiteral("CategoryLabel"));
    headingLayout->addWidget(categoryLabel);
    headingLayout->addStretch(1);
    textLayout->addWidget(headingLayoutWidget);

    auto* descriptionLabel = new QLabel(textLayoutWidget);
    descriptionLabel->setObjectName(QStringLiteral("DescriptionLabel"));
    descriptionLabel->setWordWrap(true);
    textLayout->addWidget(descriptionLabel);
    layout->addWidget(textLayoutWidget, 1);

    _widgets.append(row);
    updateWidget(row);
    return row;
}

void StorageBackendObservationAction::updateWidget(QWidget* widget) const
{
    const auto color = getSeverityColor(_severity);
    const auto severityName = util::getSeverityLevelName(_severity);
    auto* icon = widget->findChild<QLabel*>(QStringLiteral("SeverityIcon"));
    auto* severityLabel = widget->findChild<QLabel*>(QStringLiteral("SeverityLabel"));
    auto* descriptionLabel = widget->findChild<QLabel*>(QStringLiteral("DescriptionLabel"));

    icon->setPixmap(util::getSeverityLevelIcon(_severity).pixmap(18, 18));
    icon->setAccessibleName(severityName);
    severityLabel->setText(severityName);
    severityLabel->setStyleSheet(QStringLiteral("color: %1; font-weight: bold;").arg(color.name()));
    descriptionLabel->setText(_description);

    const auto tint = QStringLiteral("rgba(%1, %2, %3, 24)").arg(color.red()).arg(color.green()).arg(color.blue());
    widget->setStyleSheet(QStringLiteral("QFrame#StorageBackendObservationRow { background-color: %1; border-left: 3px solid %2; border-radius: 3px; }")
        .arg(tint, color.name()));
    widget->setAccessibleDescription(QStringLiteral("%1: %2").arg(severityName, _description));
}

}
