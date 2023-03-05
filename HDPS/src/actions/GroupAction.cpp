#include "GroupAction.h"
#include "WidgetActionLabel.h"

#include <QDebug>
#include <QHBoxLayout>

namespace hdps::gui {

const std::uint32_t GroupAction::globalLabelWidthPercentage = 35;
const std::uint32_t GroupAction::globalLabelWidthFixed      = 200;

GroupAction::GroupAction(QObject* parent, const QString& title, const bool& expanded /*= false*/) :
    WidgetAction(parent),
    _expanded(expanded),
    _readOnly(false),
    _actions(),
    _showLabels(true),
    _labelSizingType(LabelSizingType::Percentage),
    _labelWidthPercentage(GroupAction::globalLabelWidthPercentage),
    _labelWidthFixed(GroupAction::globalLabelWidthFixed)
{
    setText(title);
    setDefaultWidgetFlags(GroupAction::Vertical);
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

void GroupAction::addAction(const WidgetAction* action)
{
    Q_ASSERT(action != nullptr);

    if (action == nullptr)
        return;

    _actions << action;

    sortActions();

    QList<std::int32_t> configurationFlagsRequireUpdate{
        static_cast<std::int32_t>(WidgetAction::ConfigurationFlag::NoLabelInGroup),
        static_cast<std::int32_t>(WidgetAction::ConfigurationFlag::AlwaysCollapsed)
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

void GroupAction::removeAction(const WidgetAction* action)
{
    Q_ASSERT(action != nullptr);

    if (action == nullptr)
        return;

    if (!_actions.contains(action))
        return;

    if (_actions.contains(action))
        _actions.removeOne(action);

    disconnect(action, &WidgetAction::configurationFlagToggled, this, nullptr);
    disconnect(action, &WidgetAction::sortIndexChanged, this, nullptr);
}

ConstWidgetActions GroupAction::getActions()
{
    return _actions;
}

void GroupAction::setActions(const ConstWidgetActions& actions)
{
    _actions = actions;

    emit actionsChanged(_actions);
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

    _labelSizingType        = LabelSizingType::Percentage;
    _labelWidthPercentage   = labelWidthPercentage;

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
    auto widget = new WidgetActionWidget(parent, this);
    auto layout = new QHBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);

    if (widgetFlags & WidgetFlag::Vertical)
        layout->addWidget(new GroupAction::VerticalWidget(parent, this, widgetFlags));

    if (widgetFlags & WidgetFlag::Horizontal)
        layout->addWidget(new GroupAction::HorizontalWidget(parent, this, widgetFlags));

    widget->setLayout(layout);

    return widget;
}

GroupAction::VerticalWidget::VerticalWidget(QWidget* parent, GroupAction* groupAction, const std::int32_t& widgetFlags) :
    WidgetActionWidget(parent, groupAction, widgetFlags),
    _groupAction(groupAction)
{
    //_layout->setContentsMargins(10, 10, 10, 10);

    if (widgetFlags & PopupLayout)
        setPopupLayout(&_layout);
    else {
        setLayout(&_layout);
    }

    const auto reset = [this]() -> void {
        if (_groupAction->getShowLabels()) {
            switch (_groupAction->getLabelSizingType())
            {
                case LabelSizingType::Auto:
                {
                    _layout.setColumnStretch(1, 1);
                    break;
                }

                case LabelSizingType::Percentage:
                {
                    _layout.setColumnStretch(0, _groupAction->getLabelWidthPercentage());
                    _layout.setColumnStretch(1, 100 - _groupAction->getLabelWidthPercentage());
                    break;
                }

                default:
                    break;
            }

        }
        else {
            _layout.setColumnStretch(0, 0);
            _layout.setColumnStretch(1, 1);
        }

        QLayoutItem* layoutItem;

        while ((layoutItem = _layout.takeAt(0)) != NULL)
        {
            delete layoutItem->widget();
            delete layoutItem;
        }

        for (auto widgetAction : _groupAction->getActions()) {
            const auto numRows = _layout.rowCount();

            if (_groupAction->getShowLabels() && !widgetAction->isConfigurationFlagSet(WidgetAction::ConfigurationFlag::NoLabelInGroup)) {
                auto labelWidget = dynamic_cast<WidgetActionLabel*>(widgetAction->createLabelWidget(this, WidgetActionLabel::ColonAfterName));

                switch (_groupAction->getLabelSizingType())
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
                        labelWidget->setFixedWidth(_groupAction->getLabelWidthFixed());
                        break;
                    }

                    default:
                        break;
                }

                _layout.addWidget(labelWidget, numRows, 0);
            }

            auto actionWidget = const_cast<WidgetAction*>(widgetAction)->createWidget(this);

            _layout.addWidget(actionWidget, numRows, 1);

            if (widgetAction->getStretch() >= 0)
                _layout.setRowStretch(numRows, widgetAction->getStretch());

            //_layout->setAlignment(actionWidget, Qt::AlignLeft);
        }
    };

    connect(_groupAction, &GroupAction::actionsChanged, this, reset);
    connect(_groupAction, &GroupAction::showLabelsChanged, this, reset);
    connect(_groupAction, &GroupAction::labelSizingTypeChanged, this, reset);
    connect(_groupAction, &GroupAction::labelWidthPercentageChanged, this, reset);
    connect(_groupAction, &GroupAction::labelWidthFixedChanged, this, reset);

    reset();
}

GroupAction::HorizontalWidget::HorizontalWidget(QWidget* parent, GroupAction* groupAction, const std::int32_t& widgetFlags) :
    WidgetActionWidget(parent, groupAction, widgetFlags),
    _groupAction(groupAction)
{
    auto layout = new QHBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);

    const auto updateLayout = [&]() -> void {
        QLayoutItem* layoutItem;

        while ((layoutItem = layout->takeAt(0)) != nullptr) {
            delete layoutItem->widget();
            delete layoutItem;
        }

        for (auto action : _groupAction->getActions()) {
            if (_groupAction->getShowLabels() && !action->isConfigurationFlagSet(WidgetAction::ConfigurationFlag::NoLabelInGroup))
                layout->addWidget(action->createLabelWidget(this));

            if (action->isConfigurationFlagSet(WidgetAction::ConfigurationFlag::AlwaysCollapsed))
                layout->addWidget(const_cast<WidgetAction*>(action)->createCollapsedWidget(this));
            else
                layout->addWidget(const_cast<WidgetAction*>(action)->createWidget(this), action->getStretch());
        }
    };

    updateLayout();

    connect(_groupAction, &GroupAction::actionsChanged, this, updateLayout);

    setLayout(layout);
}

}
