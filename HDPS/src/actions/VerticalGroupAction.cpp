#include "VerticalGroupAction.h"
#include "WidgetActionLabel.h"

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
    //_layout->setContentsMargins(10, 10, 10, 10);

    if (widgetFlags & PopupLayout)
        setPopupLayout(&_layout);
    else {
        setLayout(&_layout);
    }

    const auto reset = [this]() -> void {
        if (_verticalGroupAction->getShowLabels()) {
            switch (_verticalGroupAction->getLabelSizingType())
            {
                case LabelSizingType::Auto:
                {
                    _layout.setColumnStretch(1, 1);
                    break;
                }

                case LabelSizingType::Percentage:
                {
                    _layout.setColumnStretch(0, _verticalGroupAction->getLabelWidthPercentage());
                    _layout.setColumnStretch(1, 100 - _verticalGroupAction->getLabelWidthPercentage());
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

        for (auto widgetAction : _verticalGroupAction->getActions()) {
            const auto numRows = _layout.rowCount();

            if (_verticalGroupAction->getShowLabels() && !widgetAction->isConfigurationFlagSet(WidgetAction::ConfigurationFlag::NoLabelInGroup)) {
                auto labelWidget = dynamic_cast<WidgetActionLabel*>(widgetAction->createLabelWidget(this, WidgetActionLabel::ColonAfterName));

                switch (_verticalGroupAction->getLabelSizingType())
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
                        labelWidget->setFixedWidth(_verticalGroupAction->getLabelWidthFixed());
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

    connect(_verticalGroupAction, &GroupAction::actionsChanged, this, reset);
    connect(_verticalGroupAction, &GroupAction::showLabelsChanged, this, reset);
    connect(_verticalGroupAction, &VerticalGroupAction::labelSizingTypeChanged, this, reset);
    connect(_verticalGroupAction, &VerticalGroupAction::labelWidthPercentageChanged, this, reset);
    connect(_verticalGroupAction, &VerticalGroupAction::labelWidthFixedChanged, this, reset);

    reset();
}

}
