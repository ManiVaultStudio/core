#pragma once

#include "WidgetAction.h"

#include <QHBoxLayout>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QSlider>

class QWidget;

namespace hdps {

namespace gui {

/**
 * Double widget action class
 *
 * Stores a double and creates widgets to interact with it
 *
 * @author Thomas Kroes
 */
class DoubleAction : public WidgetAction
{
    Q_OBJECT

public:
    class Widget : public WidgetAction::Widget
    {
    public:
        enum Configuration {
            SpinBox = 0x0001,
            Slider  = 0x0002,
            Reset   = 0x0004,

            SpinBoxSlider   = SpinBox | Slider,
            All             = SpinBox | Slider | Reset
        };

    public:
        Widget(QWidget* parent, DoubleAction* floatAction, const Configuration& configuration = Configuration::All);

    private:
        QHBoxLayout     _layout;
        QDoubleSpinBox  _valueDoubleSpinBox;
        QSlider         _valueSlider;
        QPushButton     _resetPushButton;
    };

public:
    DoubleAction(QObject * parent, const QString& title, const double& minimum = MIN_VALUE, const double& maximum = MAX_VALUE, const double& value = VALUE, const double& defaultValue = DEFAULT_VALUE, const std::int32_t& decimals = DECIMALS);

    QWidget* createWidget(QWidget* parent) override;

    double getValue() const;
    void setValue(const double& value);

    double getDefaultValue() const;
    void setDefaultValue(const double& defaultValue);

    bool canReset() const;
    void reset();

    float getMinimum() const;
    void setMinimum(const double& minimum);

    float getMaximum() const;
    void setMaximum(const double& maximum);

    void setRange(const double& min, const double& max);

    QString getSuffix() const;
    void setSuffix(const QString& suffix);

    std::uint32_t getDecimals() const;
    void setDecimals(const std::uint32_t& decimals);

    bool getUpdateDuringDrag() const;
    void setUpdateDuringDrag(const bool& updateDuringDrag);

signals:
    void valueChanged(const double& value);
    void defaultValueChanged(const double& defaultValue);
    void minimumChanged(const double& minimum);
    void maximumChanged(const double& maximum);
    void suffixChanged(const QString& suffix);
    void decimalsChanged(const std::uint32_t& decimals);

protected:
    double          _value;
    double          _defaultValue;
    double          _minimum;
    double          _maximum;
    QString         _suffix;
    std::uint32_t   _decimals;
    bool            _updateDuringDrag;

    static constexpr int SLIDER_MULTIPLIER = 1000;
    static constexpr double MIN_VALUE = 0.0;
    static constexpr double MAX_VALUE = 100.0;
    static constexpr double VALUE = 0.0;
    static constexpr double DEFAULT_VALUE = 0.0;
    static constexpr int DECIMALS = 1;
};

}
}