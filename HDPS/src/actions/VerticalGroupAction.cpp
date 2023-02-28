#include "VerticalGroupAction.h"

#include <QHBoxLayout>

namespace hdps::gui {

const std::uint32_t VerticalGroupAction::globalLabelWidthPercentage = 35;
const std::uint32_t VerticalGroupAction::globalLabelWidthFixed      = 200;

VerticalGroupAction::VerticalGroupAction(QObject* parent, const QString& title, const bool& expanded /*= false*/) :
    GroupAction(parent, title, expanded),
    _labelSizingType(LabelSizingType::Percentage),
    _labelWidthPercentage(VerticalGroupAction::globalLabelWidthPercentage),
    _labelWidthFixed(VerticalGroupAction::globalLabelWidthFixed)
{
}

QString VerticalGroupAction::getTypeString() const
{
    return "VerticalGroup";
}

VerticalGroupAction::LabelSizingType VerticalGroupAction::getLabelSizingType() const
{
    return _labelSizingType;
}

void VerticalGroupAction::setLabelSizingType(const LabelSizingType& labelSizingType)
{
    if (labelSizingType == _labelSizingType)
        return;

    _labelSizingType = labelSizingType;

    emit labelSizingTypeChanged(_labelSizingType);
}

std::uint32_t VerticalGroupAction::getLabelWidthPercentage() const
{
    return _labelWidthPercentage;
}

void VerticalGroupAction::setLabelWidthPercentage(std::uint32_t labelWidthPercentage)
{
    if (labelWidthPercentage == _labelWidthPercentage)
        return;

    _labelSizingType = LabelSizingType::Percentage;
    _labelWidthPercentage = labelWidthPercentage;

    emit labelWidthPercentageChanged(_labelWidthPercentage);
}

std::uint32_t VerticalGroupAction::getLabelWidthFixed() const
{
    return _labelWidthFixed;
}

void VerticalGroupAction::setLabelWidthFixed(std::uint32_t labelWidthFixed)
{
    if (labelWidthFixed == _labelWidthFixed)
        return;

    _labelSizingType = LabelSizingType::Fixed;
    _labelWidthFixed = labelWidthFixed;

    emit labelWidthFixedChanged(_labelWidthFixed);
}

VerticalGroupAction::Widget::Widget(QWidget* parent, VerticalGroupAction* verticalGroupAction, const std::int32_t& widgetFlags) :
    WidgetActionWidget(parent, verticalGroupAction, widgetFlags),
    _verticalGroupAction(verticalGroupAction)
{
    /*
    auto layout = new QHBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);

    const auto updateLayout = [&]() -> void {
        QLayoutItem* layoutItem;

        while ((layoutItem = layout->takeAt(0)) != nullptr) {
            delete layoutItem->widget();
            delete layoutItem;
        }

        for (auto action : _verticalGroupAction->getActions()) {
            if (_verticalGroupAction->getShowLabels() && !action->isConfigurationFlagSet(WidgetAction::ConfigurationFlag::NoLabelInGroup))
                layout->addWidget(action->createLabelWidget(this));

            if (action->isConfigurationFlagSet(WidgetAction::ConfigurationFlag::AlwaysCollapsed))
                layout->addWidget(const_cast<WidgetAction*>(action)->createCollapsedWidget(this));
            else
                layout->addWidget(const_cast<WidgetAction*>(action)->createWidget(this), action->getStretch());
        }
    };

    updateLayout();

    connect(_verticalGroupAction, &VerticalGroupAction::actionsChanged, this, updateLayout);

    setLayout(layout);
    */

    /*
    auto contentsMargin = _layout->contentsMargins();

    //_layout->setContentsMargins(10, 10, 10, 10);

    if (widgetFlags & PopupLayout)
        setPopupLayout(_layout);
    else {
        setLayout(_layout);
    }

    const auto reset = [this, groupAction]() -> void {
        if (groupAction->getShowLabels()) {
            switch (groupAction->getLabelSizingType())
            {
                case LabelSizingType::Auto:
                {
                    _layout->setColumnStretch(1, 1);
                    break;
                }

                case LabelSizingType::Percentage:
                {
                    _layout->setColumnStretch(0, groupAction->getLabelWidthPercentage());
                    _layout->setColumnStretch(1, 100 - groupAction->getLabelWidthPercentage());
                    break;
                }

                default:
                    break;
            }

        }
        else {
            _layout->setColumnStretch(0, 0);
            _layout->setColumnStretch(1, 1);
        }

        QLayoutItem* layoutItem;

        while ((layoutItem = layout()->takeAt(0)) != NULL)
        {
            delete layoutItem->widget();
            delete layoutItem;
        }

        for (auto widgetAction : groupAction->getSortedWidgetActions()) {
            const auto numRows          = _layout->rowCount();

            if (groupAction->getShowLabels() && !widgetAction->isConfigurationFlagSet(WidgetAction::ConfigurationFlag::NoLabelInGroup)) {
                auto labelWidget = dynamic_cast<WidgetActionLabel*>(widgetAction->createLabelWidget(this, WidgetActionLabel::ColonAfterName));

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

                _layout->addWidget(labelWidget, numRows, 0);
            }

            auto actionWidget = widgetAction->createWidget(this);

            _layout->addWidget(actionWidget, numRows, 1);

            if (widgetAction->getStretch() >= 0)
                _layout->setRowStretch(numRows, widgetAction->getStretch());

            //_layout->setAlignment(actionWidget, Qt::AlignLeft);
        }
    };

    connect(groupAction, &GroupAction::actionsChanged, this, reset);
    connect(groupAction, &GroupAction::showLabelsChanged, this, reset);
    connect(groupAction, &GroupAction::labelSizingTypeChanged, this, reset);
    connect(groupAction, &GroupAction::labelWidthPercentageChanged, this, reset);
    connect(groupAction, &GroupAction::labelWidthFixedChanged, this, reset);

    reset();
    */
}

}
