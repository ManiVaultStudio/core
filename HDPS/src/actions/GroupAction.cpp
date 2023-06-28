#include "GroupAction.h"
#include "WidgetActionLabel.h"

#include <util/Serialization.h>

#include <QDebug>
#include <QHBoxLayout>

using namespace hdps::util;

namespace hdps::gui {

const std::uint32_t GroupAction::globalLabelWidthPercentage = 35;
const std::uint32_t GroupAction::globalLabelWidthFixed      = 200;

GroupAction::GroupAction(QObject* parent, const QString& title, const bool& expanded /*= false*/, const Qt::AlignmentFlag& alignment /*= Qt::AlignmentFlag::AlignLeft*/) :
    WidgetAction(parent, title),
    _alignment(alignment),
    _expanded(expanded),
    _readOnly(false),
    _actions(),
    _showLabels(true),
    _labelSizingType(LabelSizingType::Percentage),
    _labelWidthPercentage(GroupAction::globalLabelWidthPercentage),
    _labelWidthFixed(GroupAction::globalLabelWidthFixed)
{
    setDefaultWidgetFlags(GroupAction::Vertical);
}

Qt::AlignmentFlag GroupAction::getAlignment() const
{
    return _alignment;
}

void GroupAction::setExpanded(const bool& expanded)
{
    if (expanded == _expanded)
        return;

    if (expanded)
        expand();
    else
        collapse();
}

void GroupAction::expand()
{
    if (_expanded == true)
        return;

    _expanded = true;

    emit expanded();
}

void GroupAction::collapse()
{
    if (_expanded == false)
        return;

    _expanded = false;

    emit collapsed();
}

void GroupAction::toggle()
{
    setExpanded(!isExpanded());
}

bool GroupAction::isExpanded() const
{
    return _expanded;
}

bool GroupAction::isCollapsed() const
{
    return !_expanded;
}

bool GroupAction::isReadOnly() const
{
    return _readOnly;
}

void GroupAction::setReadOnly(const bool& readOnly)
{
    if (readOnly == _readOnly)
        return;

    _readOnly = readOnly;

    emit readOnlyChanged(_readOnly);
}

bool GroupAction::getShowLabels() const
{
    return _showLabels;
}

void GroupAction::setShowLabels(bool showLabels)
{
    if (showLabels == _showLabels)
        return;

    _showLabels = showLabels;

    emit showLabelsChanged(_showLabels);
}

void GroupAction::addAction(WidgetAction* action, std::int32_t widgetFlags /*= -1*/)
{
    Q_ASSERT(action != nullptr);

    if (action == nullptr)
        return;

    _actions << action;

    _widgetFlagsMap[action] = widgetFlags;

    sortActions();

    QList<std::int32_t> configurationFlagsRequireUpdate{
        static_cast<std::int32_t>(WidgetAction::ConfigurationFlag::NoLabelInGroup),
        static_cast<std::int32_t>(WidgetAction::ConfigurationFlag::ForceCollapsedInGroup)
    };

    connect(action, &WidgetAction::configurationFlagToggled, this, [&](const WidgetAction::ConfigurationFlag& configurationFlag, bool set) -> void {
        if (!configurationFlagsRequireUpdate.contains(static_cast<std::int32_t>(configurationFlag)))
            return;

        emit actionsChanged(getActions());
    });

    connect(action, &WidgetAction::sortIndexChanged, this, [&](std::int32_t sortIndex) -> void {
        sortActions();

        emit actionsChanged(getActions());
    });

    emit actionsChanged(getActions());
}

void GroupAction::removeAction(WidgetAction* action)
{
    Q_ASSERT(action != nullptr);

    if (action == nullptr)
        return;

    if (!_actions.contains(action))
        return;

    _actions.removeOne(action);

    if (_widgetFlagsMap.contains(action))
        _widgetFlagsMap.remove(action);

    disconnect(action, &WidgetAction::configurationFlagToggled, this, nullptr);
    disconnect(action, &WidgetAction::sortIndexChanged, this, nullptr);
}

WidgetActions GroupAction::getActions()
{
    return _actions;
}

ConstWidgetActions GroupAction::getConstActions()
{
    ConstWidgetActions constWidgetActions;

    for (auto action : _actions)
        constWidgetActions << action;

    return constWidgetActions;
}

GroupAction::LabelSizingType GroupAction::getLabelSizingType() const
{
    return _labelSizingType;
}

void GroupAction::setLabelSizingType(const LabelSizingType& labelSizingType)
{
    if (labelSizingType == _labelSizingType)
        return;

    _labelSizingType = labelSizingType;

    emit labelSizingTypeChanged(_labelSizingType);
}

std::uint32_t GroupAction::getLabelWidthPercentage() const
{
    return _labelWidthPercentage;
}

void GroupAction::setLabelWidthPercentage(std::uint32_t labelWidthPercentage)
{
    if (labelWidthPercentage == _labelWidthPercentage)
        return;

    _labelSizingType = LabelSizingType::Percentage;
    _labelWidthPercentage = labelWidthPercentage;

    emit labelWidthPercentageChanged(_labelWidthPercentage);
}

std::uint32_t GroupAction::getLabelWidthFixed() const
{
    return _labelWidthFixed;
}

void GroupAction::setLabelWidthFixed(std::uint32_t labelWidthFixed)
{
    if (labelWidthFixed == _labelWidthFixed)
        return;

    _labelSizingType = LabelSizingType::Fixed;
    _labelWidthFixed = labelWidthFixed;

    emit labelWidthFixedChanged(_labelWidthFixed);
}

void GroupAction::sortActions()
{
    std::sort(_actions.begin(), _actions.end(), [](const WidgetAction* lhs, const WidgetAction* rhs) {
        return rhs->getSortIndex() > lhs->getSortIndex();
        });
}

QWidget* GroupAction::getWidget(QWidget* parent, const std::int32_t& widgetFlags)
{
    auto widget = new WidgetActionWidget(parent, this, widgetFlags);
    auto layout = new QHBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);

    auto modifiedWidgetFlags = widgetFlags & ~WidgetActionWidget::PopupLayout;

    if (widgetFlags & WidgetActionWidget::PopupLayout)
        modifiedWidgetFlags |= WithMargins;

    if (widgetFlags & WidgetFlag::Vertical)
        layout->addWidget(new GroupAction::VerticalWidget(parent, this, modifiedWidgetFlags));

    if (widgetFlags & WidgetFlag::Horizontal)
        layout->addWidget(new GroupAction::HorizontalWidget(parent, this, modifiedWidgetFlags));

    widget->setLayout(layout);

    return widget;
}

GroupAction::WidgetFlagsMap GroupAction::getWidgetFlagsMap()
{
    return _widgetFlagsMap;
}

void GroupAction::fromVariantMap(const QVariantMap& variantMap)
{
    WidgetAction::fromVariantMap(variantMap);

    variantMapMustContain(variantMap, "Expanded");

    setExpanded(variantMap["Expanded"].toBool());
}

QVariantMap GroupAction::toVariantMap() const
{
    auto variantMap = WidgetAction::toVariantMap();

    variantMap.insert({
        { "Expanded", isExpanded() }
    });

    return variantMap;
}

void GroupAction::clear()
{
    _actions.clear();

    emit actionsChanged(_actions);
}

hdps::gui::StretchAction* GroupAction::addStretch(std::int32_t stretch /*= 1*/)
{
    auto actionStretchActionId = QUuid::createUuid().toString(QUuid::WithoutBraces);

    actionStretchActionId.truncate(6);

    auto stretchAction = new StretchAction(this, QString("Stretch_%1").arg(actionStretchActionId));

    addAction(stretchAction);

    return stretchAction;
}

GroupAction::VerticalWidget::VerticalWidget(QWidget* parent, GroupAction* groupAction, const std::int32_t& widgetFlags) :
    WidgetActionWidget(parent, groupAction, widgetFlags),
    _groupAction(groupAction)
{
    auto layout = new QGridLayout();

    if (widgetFlags & NoMargins)
        layout->setContentsMargins(0, 0, 0, 0);

    const auto updateLayout = [this, layout, groupAction]() -> void {
        if (groupAction->getShowLabels()) {
            switch (groupAction->getLabelSizingType())
            {
                case LabelSizingType::Auto:
                {
                    layout->setColumnStretch(1, 1);
                    break;
                }

                case LabelSizingType::Percentage:
                {
                    layout->setColumnStretch(0, groupAction->getLabelWidthPercentage());
                    layout->setColumnStretch(1, 100 - groupAction->getLabelWidthPercentage());
                    break;
                }

                default:
                    break;
            }
        }
        else {
            layout->setColumnStretch(0, 0);
            layout->setColumnStretch(1, 1);
        }

        QLayoutItem* layoutItem;

        while ((layoutItem = layout->takeAt(0)) != nullptr) {
            delete layoutItem->widget();
            delete layoutItem;
        }

        for (auto action : groupAction->getActions()) {
            const auto numRows = layout->rowCount();

            if (groupAction->getShowLabels() && !action->isConfigurationFlagSet(WidgetAction::ConfigurationFlag::NoLabelInGroup)) {
                auto labelWidget = dynamic_cast<WidgetActionLabel*>(action->createLabelWidget(this, WidgetActionLabel::ColonAfterName));

                switch (groupAction->getLabelSizingType())
                {
                    case LabelSizingType::Auto:
                    {
                        labelWidget->setElide(false);
                        //labelWidget->setFixedWidth(groupAction->getLabelWidthFixed());
                        break;
                    }

                    case LabelSizingType::Percentage:
                    {
                        labelWidget->setElide(true);
                        break;
                    }

                    case LabelSizingType::Fixed:
                    {
                        labelWidget->setElide(true);
                        labelWidget->setFixedWidth(groupAction->getLabelWidthFixed());
                        break;
                    }

                    default:
                        break;
                }

                layout->addWidget(labelWidget, numRows, 0);
            }

            const auto widgetFlags = groupAction->getWidgetFlagsMap()[action];

            if (widgetFlags >= 0)
                layout->addWidget(action->createWidget(this, widgetFlags), numRows, 1);
            else
                layout->addWidget(action->createWidget(this), numRows, 1);

            if (action->getStretch() >= 0)
                layout->setRowStretch(numRows, action->getStretch());
        }
    };

    connect(groupAction, &GroupAction::actionsChanged, this, updateLayout);
    connect(groupAction, &GroupAction::showLabelsChanged, this, updateLayout);
    connect(groupAction, &GroupAction::labelSizingTypeChanged, this, updateLayout);
    connect(groupAction, &GroupAction::labelWidthPercentageChanged, this, updateLayout);
    connect(groupAction, &GroupAction::labelWidthFixedChanged, this, updateLayout);

    updateLayout();

    setLayout(layout);
}

GroupAction::HorizontalWidget::HorizontalWidget(QWidget* parent, GroupAction* groupAction, const std::int32_t& widgetFlags) :
    WidgetActionWidget(parent, groupAction, widgetFlags),
    _groupAction(groupAction)
{
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);

    auto layout = new QHBoxLayout();

    if (!(widgetFlags & WithMargins))
        layout->setContentsMargins(0, 0, 0, 0);

    const auto updateLayout = [this, layout, groupAction]() -> void {
        QLayoutItem* layoutItem;

        while ((layoutItem = layout->takeAt(0)) != nullptr) {
            delete layoutItem->widget();
            delete layoutItem;
        }

        for (auto action : groupAction->getActions()) {
            auto actionStretch = dynamic_cast<StretchAction*>(action);

            if (actionStretch) {
                layout->addStretch(actionStretch->getStretch());

                continue;
            }

            if (groupAction->getShowLabels() && !action->isConfigurationFlagSet(WidgetAction::ConfigurationFlag::NoLabelInGroup))
                layout->addWidget(action->createLabelWidget(this));

            if (action->isConfigurationFlagSet(WidgetAction::ConfigurationFlag::ForceCollapsedInGroup))
                layout->addWidget(const_cast<WidgetAction*>(action)->createCollapsedWidget(this));
            else {
                const auto widgetFlags = groupAction->getWidgetFlagsMap()[action];

                if (action->getStretch() >= 1) {
                    if (widgetFlags >= 0)
                        layout->addWidget(const_cast<WidgetAction*>(action)->createWidget(this, widgetFlags), action->getStretch());
                    else
                        layout->addWidget(const_cast<WidgetAction*>(action)->createWidget(this), action->getStretch());
                }
                else {
                    if (widgetFlags >= 0)
                        layout->addWidget(const_cast<WidgetAction*>(action)->createWidget(this, widgetFlags));
                    else
                        layout->addWidget(const_cast<WidgetAction*>(action)->createWidget(this));
                }

            }
        }

        updateGeometry();
    };

    updateLayout();

    connect(groupAction, &GroupAction::actionsChanged, this, updateLayout);

    setLayout(layout);
}

}