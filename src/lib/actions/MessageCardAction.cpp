// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2026 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#include "MessageCardAction.h"

#include "util/StyledIcon.h"

#include <QColor>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

namespace mv::gui
{

namespace
{

QColor getSeverityColor(const util::SeverityLevel severity)
{
    switch (severity)
    {
        case util::SeverityLevel::Warning:  return QColor(196, 150, 20);
        case util::SeverityLevel::Error:    return QColor(196, 32, 32);
        case util::SeverityLevel::Fatal:    return QColor(150, 0, 0);

        default:
            return QColor(80, 130, 190);
    }
}

}

MessageCardAction::MessageCardAction(QObject* parent) :
    WidgetAction(parent, "Message")
{
    setConfigurationFlag(ConfigurationFlag::NoLabelInGroup);
}

void MessageCardAction::setMessage(const util::SeverityLevel severity, const QString& category, const QString& description, const ButtonActions& buttonActions)
{
    _severity       = severity;
    _category       = category;
    _description    = description;
    _buttonActions  = buttonActions;

    for (const auto& widget : _widgets)
        if (widget)
            updateWidget(widget);
}

QWidget* MessageCardAction::getWidget(QWidget* parent, const std::int32_t&)
{
    auto* card = new QFrame(parent);

    card->setObjectName(QStringLiteral("MessageCard"));

    auto* layout = new QHBoxLayout(card);

    layout->setContentsMargins(8, 6, 8, 6);
    layout->setSpacing(8);

    auto* icon = new QLabel(card);

    icon->setObjectName(QStringLiteral("SeverityIcon"));
    icon->setFixedSize(20, 20);
    icon->setAlignment(Qt::AlignTop | Qt::AlignHCenter);

    layout->addWidget(icon);

    auto* textWidget = new QWidget(card);
    auto* textLayout = new QVBoxLayout(textWidget);

    textLayout->setContentsMargins(0, 0, 0, 0);
    textLayout->setSpacing(3);

    auto* headingWidget = new QWidget(textWidget);
    auto* headingLayout = new QHBoxLayout(headingWidget);

    headingLayout->setContentsMargins(0, 0, 0, 0);
    headingLayout->setSpacing(6);

    auto* severityLabel = new QLabel(headingWidget);

    severityLabel->setObjectName(QStringLiteral("SeverityLabel"));
    headingLayout->addWidget(severityLabel);

    auto* categoryLabel = new QLabel(headingWidget);

    categoryLabel->setObjectName(QStringLiteral("CategoryLabel"));
    headingLayout->addWidget(categoryLabel);
    headingLayout->addStretch(1);
    textLayout->addWidget(headingWidget);

    auto* descriptionLabel = new QLabel(textWidget);

    descriptionLabel->setObjectName(QStringLiteral("DescriptionLabel"));
    descriptionLabel->setWordWrap(true);
    textLayout->addWidget(descriptionLabel);
    layout->addWidget(textWidget, 1);

    auto* actionButtonsWidget = new QWidget(card);

    actionButtonsWidget->setObjectName(QStringLiteral("ActionButtons"));

    auto* actionButtonsLayout = new QVBoxLayout(actionButtonsWidget);

    actionButtonsLayout->setContentsMargins(0, 0, 0, 0);
    actionButtonsLayout->setSpacing(4);

    layout->addWidget(actionButtonsWidget, 0, Qt::AlignVCenter);

    _widgets.append(card);
    updateWidget(card);

    return card;
}

void MessageCardAction::updateWidget(QWidget* widget)
{
    const auto color            = getSeverityColor(_severity);
    const auto severityName     = util::getSeverityLevelName(_severity);
    auto* icon                  = widget->findChild<QLabel*>(QStringLiteral("SeverityIcon"));
    auto* severityLabel         = widget->findChild<QLabel*>(QStringLiteral("SeverityLabel"));
    auto* categoryLabel         = widget->findChild<QLabel*>(QStringLiteral("CategoryLabel"));
    auto* descriptionLabel      = widget->findChild<QLabel*>(QStringLiteral("DescriptionLabel"));
    auto* actionButtonsWidget   = widget->findChild<QWidget*>(QStringLiteral("ActionButtons"));
    auto* actionButtonsLayout   = qobject_cast<QVBoxLayout*>(actionButtonsWidget->layout());

    icon->setPixmap(util::getSeverityLevelIcon(_severity).pixmap(18, 18));
    icon->setAccessibleName(severityName);

    severityLabel->setText(severityName);
    severityLabel->setStyleSheet(QStringLiteral("color: %1; font-weight: bold;").arg(color.name()));

	categoryLabel->setText(_category);
    descriptionLabel->setText(_description);

    while (actionButtonsLayout->count() > _buttonActions.size())
    {
        auto* item = actionButtonsLayout->takeAt(actionButtonsLayout->count() - 1);
        delete item->widget();
        delete item;
    }

    for (int actionIndex = 0; actionIndex < _buttonActions.size(); ++actionIndex)
    {
        auto* actionButton = actionButtonsLayout->count() > actionIndex ? qobject_cast<QPushButton*>(actionButtonsLayout->itemAt(actionIndex)->widget()) : nullptr;

            if (!actionButton)
        {
            actionButton = new QPushButton(actionButtonsWidget);
            actionButton->setObjectName(QStringLiteral("ActionButton%1").arg(actionIndex));

                connect(actionButton, &QPushButton::clicked, this, [this, actionIndex] {
                const auto trigger = _buttonActions.value(actionIndex).trigger;
                if (trigger)
                    trigger();
            });

                actionButtonsLayout->addWidget(actionButton);
        }

        actionButton->setText(_buttonActions[actionIndex].text);
    }

    actionButtonsWidget->setVisible(!_buttonActions.isEmpty());

    const auto tint = QStringLiteral("rgba(%1, %2, %3, 24)").arg(color.red()).arg(color.green()).arg(color.blue());

    widget->setStyleSheet(QStringLiteral("QFrame#MessageCard { background-color: %1; border-left: 3px solid %2; border-radius: 3px; }").arg(tint, color.name()));
    widget->setAccessibleDescription(QStringLiteral("%1 %2: %3").arg(severityName, _category, _description));
}

}
