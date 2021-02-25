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
        Widget(QWidget* parent, DoubleAction* floatAction);

    private:
        QHBoxLayout     _layout;
        QDoubleSpinBox  _spinBox;
        //QPushButton     _resetPushButton;
        QSlider         _slider;
    };

public:
    DoubleAction(QObject * parent, const QString& title, const double& minimum = DEFAULT_MIN_VALUE, const double& maximum = DEFAULT_MAX_VALUE, const double& value = 0.0, const std::int32_t& decimals = DEFAULT_DECIMALS);

    QWidget* createWidget(QWidget* parent) override;

    double getValue() const;
    void setValue(const double& value);

    float getMinimum() const;
    void setMinimum(const double& minimum);

    float getMaximum() const;
    void setMaximum(const double& maximum);

    void setRange(const double& min, const double& max);

    QString getSuffix() const;
    void setSuffix(const QString& suffix);

    std::int32_t getDecimals() const;
    void setDecimals(const std::int32_t& decimals);

    bool getUpdateDuringDrag() const;
    void setUpdateDuringDrag(const bool& updateDuringDrag);

signals:
    void valueChanged(const double& value);
    void minimumChanged(const double& minimum);
    void maximumChanged(const double& maximum);
    void suffixChanged(const QString& suffix);
    void decimalsChanged(const std::int32_t& decimals);

protected:
    double          _value;
    double          _minimum;
    double          _maximum;
    QString         _suffix;
    std::int32_t    _decimals;
    bool            _updateDuringDrag;

    static constexpr int SLIDER_MULTIPLIER = 1000;
    static constexpr double DEFAULT_MIN_VALUE = 0.0;
    static constexpr double DEFAULT_MAX_VALUE = 100.0;
    static constexpr double DEFAULT_VALUE = 0.0;
    static constexpr int DEFAULT_DECIMALS = 1;
};

}
}