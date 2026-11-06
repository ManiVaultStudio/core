// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2026 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#include "MessageCardAction.h"

#include "util/StyledIcon.h"

#include <QColor>
#include <QEvent>
#include <QFrame>
#include <QFontMetrics>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QResizeEvent>
#include <QToolButton>
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

class ElidedMessageLabel : public QLabel
{
public:
    explicit ElidedMessageLabel(QWidget* parent = nullptr) : QLabel(parent) {}

    void setFullText(const QString& text)
    {
        _fullText = text;
        updateElision();
    }

protected:
    void resizeEvent(QResizeEvent* event) override
    {
        QLabel::resizeEvent(event);
        updateElision();
    }

private:
    void updateElision()
    {
        QLabel::setText(fontMetrics().elidedText(_fullText, Qt::ElideRight, contentsRect().width()));
    }

    QString _fullText;
};

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
        {
            if (_displayMode == DisplayMode::Compact && !widget->property("MessageCardUserToggled").toBool())
                widget->setProperty("MessageCardExpanded", _severity != util::SeverityLevel::Info);
            updateWidget(widget);
        }
}

void MessageCardAction::setDisplayMode(const DisplayMode displayMode)
{
    if (_displayMode == displayMode)
        return;

    _displayMode = displayMode;
    for (const auto& widget : _widgets)
        if (widget)
        {
            if (_displayMode == DisplayMode::Compact && !widget->property("MessageCardUserToggled").toBool())
                widget->setProperty("MessageCardExpanded", _severity != util::SeverityLevel::Info);
            updateWidget(widget);
        }
}

MessageCardAction::DisplayMode MessageCardAction::getDisplayMode() const
{
    return _displayMode;
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
    icon->setAttribute(Qt::WA_TransparentForMouseEvents);

    layout->addWidget(icon);

    auto* textWidget = new QWidget(card);
    textWidget->setAttribute(Qt::WA_TransparentForMouseEvents);
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

    auto* compactTextLabel = new ElidedMessageLabel(textWidget);
    compactTextLabel->setObjectName(QStringLiteral("CompactTextLabel"));
    compactTextLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    textLayout->addWidget(compactTextLabel);
    layout->addWidget(textWidget, 1);

    auto* actionButtonsWidget = new QWidget(card);

    actionButtonsWidget->setObjectName(QStringLiteral("ActionButtons"));

    auto* actionButtonsLayout = new QVBoxLayout(actionButtonsWidget);

    actionButtonsLayout->setContentsMargins(0, 0, 0, 0);
    actionButtonsLayout->setSpacing(4);

    layout->addWidget(actionButtonsWidget, 0, Qt::AlignVCenter);

    auto* expandCollapseButton = new QToolButton(card);
    expandCollapseButton->setObjectName(QStringLiteral("ExpandCollapseButton"));
    expandCollapseButton->setArrowType(Qt::DownArrow);
    expandCollapseButton->setAutoRaise(true);
    expandCollapseButton->setFocusPolicy(Qt::StrongFocus);
    expandCollapseButton->setToolTip(tr("Expand message"));
    expandCollapseButton->setAccessibleName(tr("Expand message"));
    layout->addWidget(expandCollapseButton, 0, Qt::AlignVCenter);
    connect(expandCollapseButton, &QToolButton::clicked, this, [this, card] { toggleCard(card); });

    _widgets.append(card);
    card->setProperty("MessageCardExpanded", _severity != util::SeverityLevel::Info);
    card->setProperty("MessageCardUserToggled", false);
    updateWidget(card);
    for (auto* childWidget : card->findChildren<QWidget*>())
        childWidget->installEventFilter(this);
    card->installEventFilter(this);

    return card;
}

void MessageCardAction::updateWidget(QWidget* widget)
{
    const auto compact         = _displayMode == DisplayMode::Compact && !widget->property("MessageCardExpanded").toBool();
    const auto color            = getSeverityColor(_severity);
    const auto severityName     = util::getSeverityLevelName(_severity);
    auto* icon                  = widget->findChild<QLabel*>(QStringLiteral("SeverityIcon"));
    auto* severityLabel         = widget->findChild<QLabel*>(QStringLiteral("SeverityLabel"));
    auto* categoryLabel         = widget->findChild<QLabel*>(QStringLiteral("CategoryLabel"));
    auto* descriptionLabel      = widget->findChild<QLabel*>(QStringLiteral("DescriptionLabel"));
    auto* actionButtonsWidget   = widget->findChild<QWidget*>(QStringLiteral("ActionButtons"));
    auto* actionButtonsLayout   = qobject_cast<QVBoxLayout*>(actionButtonsWidget->layout());
    auto* expandCollapseButton  = widget->findChild<QToolButton*>(QStringLiteral("ExpandCollapseButton"));

    icon->setPixmap(util::getSeverityLevelIcon(_severity).pixmap(18, 18));
    icon->setAccessibleName(severityName);

    severityLabel->setText(severityName);
    severityLabel->setStyleSheet(QStringLiteral("color: %1; font-weight: bold;").arg(color.name()));
    severityLabel->setVisible(!compact);
    if (auto* headingWidget = widget->findChild<QWidget*>(QStringLiteral("SeverityLabel"))->parentWidget())
        headingWidget->setVisible(!compact);

    categoryLabel->setText(_category);
    descriptionLabel->setText(_description);
    descriptionLabel->setVisible(!compact);
    auto* compactTextLabel = static_cast<ElidedMessageLabel*>(widget->findChild<QLabel*>(QStringLiteral("CompactTextLabel")));
    compactTextLabel->setFullText(_category.isEmpty() ? _description : QStringLiteral("%1 — %2").arg(_category, _description));
    compactTextLabel->setVisible(compact);

    if (auto* cardLayout = qobject_cast<QHBoxLayout*>(widget->layout()))
    {
        cardLayout->setContentsMargins(compact ? 6 : 8, compact ? 3 : 6, compact ? 6 : 8, compact ? 3 : 6);
        icon->setAlignment(compact ? Qt::AlignCenter : Qt::AlignTop | Qt::AlignHCenter);
    }
    actionButtonsLayout->setDirection(compact ? QBoxLayout::LeftToRight : QBoxLayout::TopToBottom);

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
    expandCollapseButton->setVisible(_displayMode == DisplayMode::Compact);
    expandCollapseButton->setArrowType(widget->property("MessageCardExpanded").toBool() ? Qt::UpArrow : Qt::DownArrow);
    expandCollapseButton->setToolTip(widget->property("MessageCardExpanded").toBool() ? tr("Collapse message") : tr("Expand message"));
    expandCollapseButton->setAccessibleName(expandCollapseButton->toolTip());

    const auto tint = QStringLiteral("rgba(%1, %2, %3, 24)").arg(color.red()).arg(color.green()).arg(color.blue());

    widget->setStyleSheet(QStringLiteral("QFrame#MessageCard { background-color: %1; border-left: 3px solid %2; border-radius: 3px; } QFrame#MessageCard[expandable=\"true\"]:hover { background-color: rgba(%3, %4, %5, 40); } QToolButton:focus { border: 1px solid %2; border-radius: 3px; }")
        .arg(tint, color.name()).arg(color.red()).arg(color.green()).arg(color.blue()));
    widget->setProperty("expandable", _displayMode == DisplayMode::Compact);
    widget->setAccessibleDescription(QStringLiteral("%1 %2: %3").arg(severityName, _category, _description));
    widget->updateGeometry();
}

bool MessageCardAction::eventFilter(QObject* watched, QEvent* event)
{
    if (event->type() == QEvent::MouseButtonRelease)
    {
        auto* watchedWidget = qobject_cast<QWidget*>(watched);
        if (watchedWidget)
        {
            for (const auto& widget : _widgets)
            {
                if (!widget || (watchedWidget != widget && !widget->isAncestorOf(watchedWidget)))
                    continue;

                const auto* buttonsWidget = widget->findChild<QWidget*>(QStringLiteral("ActionButtons"));
                const auto* expandButton = widget->findChild<QToolButton*>(QStringLiteral("ExpandCollapseButton"));
                if ((!buttonsWidget || (watchedWidget != buttonsWidget && !buttonsWidget->isAncestorOf(watchedWidget))) && watchedWidget != expandButton)
                    toggleCard(widget);
                break;
            }
        }
    }

    return WidgetAction::eventFilter(watched, event);
}

void MessageCardAction::toggleCard(QWidget* widget)
{
    if (_displayMode != DisplayMode::Compact || !widget)
        return;

    widget->setProperty("MessageCardExpanded", !widget->property("MessageCardExpanded").toBool());
    widget->setProperty("MessageCardUserToggled", true);
    updateWidget(widget);
}

}
