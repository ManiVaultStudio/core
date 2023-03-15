#include "DecimalRangeAction.h"
#include "Application.h"

#include <QDebug>
#include <QHBoxLayout>

namespace hdps::gui {

#if (__cplusplus < 201703L)   // definition needed for pre C++17 gcc and clang
    constexpr float  DecimalRangeAction::INIT_LIMIT_MIN;
    constexpr float  DecimalRangeAction::INIT_LIMIT_MAX;
    constexpr float  DecimalRangeAction::INIT_RANGE_MIN;
    constexpr float  DecimalRangeAction::INIT_RANGE_MAX;
    constexpr float  DecimalRangeAction::INIT_NUMBER_OF_DECIMALS;
#endif

DecimalRangeAction::DecimalRangeAction(QObject* parent, const QString& title, const float& limitMin /*= INIT_LIMIT_MIN*/, const float& limitMax /*= INIT_LIMIT_MAX*/, const float& rangeMin /*= INIT_RANGE_MIN*/, const float& rangeMax /*= INIT_RANGE_MAX*/, std::int32_t numberOfDecimals /*= INIT_NUMBER_OF_DECIMALS*/) :
    NumericalRangeAction(parent, title, limitMin, limitMax, rangeMin, rangeMax)
{
    getRangeMinAction().setNumberOfDecimals(numberOfDecimals);
    getRangeMaxAction().setNumberOfDecimals(numberOfDecimals);

    connect(&getRangeMinAction(), &DecimalAction::valueChanged, this, [this](const float& value) -> void {
        if (value >= _rangeMaxAction.getValue())
            _rangeMaxAction.setValue(value);

        emit rangeChanged(_rangeMinAction.getValue(), _rangeMaxAction.getValue());
    });

    connect(&getRangeMaxAction(), &DecimalAction::valueChanged, this, [this](const float& value) -> void {
        if (value <= _rangeMinAction.getValue())
            _rangeMinAction.setValue(value);

        emit rangeChanged(_rangeMinAction.getValue(), _rangeMaxAction.getValue());
    });
}

DecimalRangeAction::DecimalRangeWidget::DecimalRangeWidget(QWidget* parent, DecimalRangeAction* decimalRangeAction, const std::int32_t& widgetFlags /*= 0*/) :
    WidgetActionWidget(parent, decimalRangeAction, widgetFlags)
{
    if (widgetFlags & PopupLayout) {
        auto layout = new QHBoxLayout();

        layout->addWidget(decimalRangeAction->getRangeMinAction().createWidget(this, DecimalAction::SpinBox));
        layout->addWidget(decimalRangeAction->getRangeMaxAction().createWidget(this, DecimalAction::SpinBox));

        setPopupLayout(layout);
    }
    else {
        auto layout = new QHBoxLayout();

        layout->setContentsMargins(0, 0, 0, 0);

        layout->addWidget(decimalRangeAction->getRangeMinAction().createWidget(this, DecimalAction::SpinBox));
        layout->addWidget(decimalRangeAction->getRangeMaxAction().createWidget(this, DecimalAction::SpinBox));

        setLayout(layout);
    }
}

}
