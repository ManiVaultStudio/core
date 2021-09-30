#include "DecimalRangeAction.h"
#include "Application.h"

#include <QDebug>
#include <QHBoxLayout>

namespace hdps {

namespace gui {

DecimalRangeAction::DecimalRangeAction(QObject* parent, const QString& title /*= ""*/, const float& limitMin /*= INIT_LIMIT_MIN*/, const float& limitMax /*= INIT_LIMIT_MAX*/, const float& rangeMin /*= INIT_RANGE_MIN*/, const float& rangeMax /*= INIT_RANGE_MAX*/, const float& defaultRangeMin /*= INIT_DEFAULT_RANGE_MIN*/, const float& defaultRangeMax /*= INIT_DEFAULT_RANGE_MAX*/) :
    WidgetAction(parent),
    _rangeMinAction(this, "Minimum"),
    _rangeMaxAction(this, "Maximum")
{
    setMayReset(true);

    connect(&_rangeMinAction, &DecimalAction::valueChanged, this, [this](const float& value) -> void {
        if (value >= _rangeMaxAction.getValue())
            _rangeMaxAction.setValue(value);

        emit rangeChanged(_rangeMinAction.getValue(), _rangeMaxAction.getValue());

        setResettable(_rangeMinAction.isResettable() || _rangeMaxAction.isResettable());
    });

    connect(&_rangeMaxAction, &DecimalAction::valueChanged, this, [this](const float& value) -> void {
        if (value <= _rangeMinAction.getValue())
            _rangeMinAction.setValue(value);

        emit rangeChanged(_rangeMinAction.getValue(), _rangeMaxAction.getValue());

        setResettable(_rangeMinAction.isResettable() || _rangeMaxAction.isResettable());
    });

    initialize(limitMin, limitMax, rangeMin, rangeMax);
}

void DecimalRangeAction::initialize(const float& limitMin /*= INIT_LIMIT_MIN*/, const float& limitMax /*= INIT_LIMIT_MAX*/, const float& rangeMin /*= INIT_RANGE_MIN*/, const float& rangeMax /*= INIT_RANGE_MAX*/, const float& defaultRangeMin /*= INIT_DEFAULT_RANGE_MIN*/, const float& defaultRangeMax /*= INIT_DEFAULT_RANGE_MAX*/)
{
    _rangeMinAction.initialize(limitMin, limitMax, rangeMin, defaultRangeMin);
    _rangeMaxAction.initialize(limitMin, limitMax, rangeMax, defaultRangeMax);
}

float DecimalRangeAction::getMinimum() const
{
    return _rangeMinAction.getValue();
}

float DecimalRangeAction::getMaximum() const
{
    return _rangeMaxAction.getValue();
}

void DecimalRangeAction::setRange(const float& minimum, const float& maximum)
{
    Q_ASSERT(minimum < maximum);

    _rangeMinAction.initialize(minimum, maximum, minimum, minimum);
    _rangeMaxAction.initialize(minimum, maximum, maximum, maximum);
}

bool DecimalRangeAction::isResettable() const
{
    return _rangeMinAction.isResettable() || _rangeMaxAction.isResettable();
}

void DecimalRangeAction::reset()
{
    _rangeMinAction.reset();
    _rangeMaxAction.reset();
}

DecimalRangeAction::DecimalRangeWidget::DecimalRangeWidget(QWidget* parent, DecimalRangeAction* decimalRangeAction, const WidgetActionWidget::State& state) :
    WidgetActionWidget(parent, decimalRangeAction, state)
{
    auto layout = new QHBoxLayout();

    auto rangeMinSpinBoxWidget  = decimalRangeAction->getRangeMinAction().createSpinBoxWidget(this);
    auto rangeMinSliderWidget   = decimalRangeAction->getRangeMinAction().createSliderWidget(this);
    auto rangeMaxSpinBoxWidget  = decimalRangeAction->getRangeMaxAction().createSpinBoxWidget(this);
    auto rangeMaxSliderWidget   = decimalRangeAction->getRangeMaxAction().createSliderWidget(this);

    switch (state)
    {
        case WidgetActionWidget::State::Standard:
        {
            auto layout = new QHBoxLayout();

            layout->setMargin(0);
            layout->addWidget(rangeMinSpinBoxWidget);
            layout->addWidget(rangeMinSliderWidget);
            layout->addWidget(rangeMaxSpinBoxWidget);
            layout->addWidget(rangeMaxSpinBoxWidget);

            setLayout(layout);
            break;
        }

        case WidgetActionWidget::State::Popup:
        {
            auto layout = new QGridLayout();

            layout->addWidget(rangeMinSpinBoxWidget, 0, 0);
            layout->addWidget(rangeMinSliderWidget, 0, 1);
            layout->addWidget(rangeMaxSpinBoxWidget, 1, 0);
            layout->addWidget(rangeMaxSpinBoxWidget, 1, 1);

            setPopupLayout(layout);
            break;
        }

        default:
            break;
    }
}

}
}
