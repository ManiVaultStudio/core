#pragma once

#include "WidgetAction.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QSlider>

class QWidget;

namespace hdps {

namespace gui {

class DoubleAction : public WidgetAction
{
    Q_OBJECT

public:
    class Widget : public WidgetAction::Widget {
    public:
        Widget(QWidget* parent, DoubleAction* floatAction);

    private:
        QHBoxLayout     _layout;
        QLabel          _label;
        QDoubleSpinBox  _spinBox;
        QSlider         _slider;
    };
public:
    DoubleAction(QObject * parent, const QString& title = "");

    QWidget* createWidget(QWidget* parent) override;

    double getValue() const {
        return _value;
    }

    void DoubleAction::setValue(const double& value) {
        if (value == _value)
            return;

        _value = value;

        emit valueChanged(_value);
    }

    float getMinValue() const {
        return _minValue;
    }

    void setMinValue(const float& minValue) {
        if (minValue == _minValue)
            return;

        _minValue = minValue;

        emit minValueChanged(_minValue);
    }

    float getMaxValue() const {
        return _maxValue;
    }

    void setMaxValue(const float& maxValue) {
        if (maxValue == _maxValue)
            return;

        _maxValue = maxValue;

        emit maxValueChanged(_maxValue);
    }

    QString getSuffix() const {
        return _suffix;
    }

    void setSuffix(const QString& suffix) {
        if (suffix == _suffix)
            return;

        _suffix = suffix;

        emit suffixChanged(_suffix);
    }

    std::int32_t getDecimals() const {
        return _decimals;
    }

    void setDecimals(const std::int32_t& decimals) {
        if (decimals == _decimals)
            return;

        _decimals = decimals;

        emit decimalsChanged(_decimals);
    }

signals:
    void valueChanged(const double& value);
    void minValueChanged(const double& minValue);
    void maxValueChanged(const double& maxValue);
    void suffixChanged(const QString& suffix);
    void decimalsChanged(const std::int32_t& decimals);

protected:
    double          _value;
    double          _minValue;
    double          _maxValue;
    QString         _suffix;
    std::int32_t    _decimals;

    static constexpr int SLIDER_MULTIPLIER = 1000;
    static constexpr double DEFAULT_MIN_VALUE = 0.0;
    static constexpr double DEFAULT_MAX_VALUE = 100.0;
    static constexpr int DEFAULT_DECIMALS = 1;
};

}
}