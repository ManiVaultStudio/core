#pragma once

#include "widgets/PopupPushButton.h"

#include <QAction>
#include <QWidgetAction>
#include <QMenu>
#include <QComboBox>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QSlider>
#include <QToolBar>
#include <QToolButton>
#include <QEvent>

class ScatterplotWidget;

class ActionButton : public QPushButton {
    Q_OBJECT

private:

    QAction *actionOwner = nullptr;

public:
    explicit ActionButton(QWidget *parent = nullptr);
    explicit ActionButton(QAction *action);
    void setAction(QAction *action);

public slots:
    void updateButtonStatusFromAction();
};

class PopupWidget : public QWidget {
public:
    PopupWidget(QWidget* parent, const QString& title);

    void setContentLayout(QLayout* layout);

private:
    QVBoxLayout     _mainLayout;
    QGroupBox       _groupBox;
};

class ActionWidget : public QWidget
{
public:
    ActionWidget(QWidget* parent, QAction* action) :
        QWidget(parent),
        _action(action)
    {
        setEnabled(action->isEnabled());
        action->installEventFilter(this);
    }

    bool childOfMenu() const {
        return dynamic_cast<QMenu*>(parent());
    }

    bool childOfToolbar() const {
        return dynamic_cast<QToolBar*>(parent());
    }

    bool childOfWidget() const {
        return childOfMenu() || childOfToolbar();
    }

    bool childOfPopupWidget() const {
        return dynamic_cast<PopupWidget*>(parent());
    }

    bool eventFilter(QObject* object, QEvent* event) {
        auto widget = dynamic_cast<QWidget*>(object);

        if (widget != nullptr) {
            switch (event->type())
            {
                case QEvent::EnabledChange:
                    setEnabled(_action->isEnabled());
                    break;

                default:
                    break;
            }
        }

        return QObject::eventFilter(object, event);
    }

protected:
    QAction*  _action;
};

class DoubleAction : public QWidgetAction
{
    Q_OBJECT

public:
    class Widget : public ActionWidget {
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

class OptionAction : public QWidgetAction
{
    Q_OBJECT

public:
    class Widget : public ActionWidget {
    public:
        Widget(QWidget* widget, OptionAction* optionAction);

    private:
        QHBoxLayout     _layout;
        QLabel          _label;
        QComboBox       _comboBox;
    };

public:
    OptionAction(QObject* parent, const QString& title = "");

    QWidget* createWidget(QWidget* parent) override;
};

template<typename WidgetType>
class CustomAction : public QWidgetAction
{
public:
    CustomAction(ScatterplotWidget* scatterplotWidget) :
        QWidgetAction(scatterplotWidget),
        _scatterplotWidget(scatterplotWidget)
    {
    }

    QWidget* createWidget(QWidget* parent) override {
        return new WidgetType(parent, _scatterplotWidget);
    }

protected:
    ScatterplotWidget*  _scatterplotWidget;
};

class PositionAction : public QWidgetAction
{
    Q_OBJECT

public:
    class Widget : public ActionWidget {
    public:
        Widget(QWidget* parent, PositionAction* positionAction);

    private:
        QHBoxLayout         _layout;
        QToolBar            _toolBar;
        QToolButton         _toolButton;
        PopupWidget         _popupWidget;
        QWidgetAction       _popupWidgetAction;
    };

public:
    PositionAction(QObject* parent, OptionAction* xDimensionAction, OptionAction* yDimensionAction);

    QWidget* createWidget(QWidget* parent) override {
        return new Widget(parent, this);
    }

protected:
    OptionAction*       _xDimensionAction;
    OptionAction*       _yDimensionAction;

    friend class Widget;
};

class RenderModeAction : public QWidgetAction
{
    Q_OBJECT

public:
    class Widget : public ActionWidget {
    public:
        Widget(QWidget* parent, RenderModeAction* renderModeAction);

    private:
        QHBoxLayout         _layout;
        QToolBar            _toolBar;
        QToolButton         _toolButton;
        PopupWidget         _popupWidget;
        QWidgetAction       _popupWidgetAction;
    };

public:
    RenderModeAction(ScatterplotWidget* scatterplotWidget);

    QWidget* createWidget(QWidget* parent) override {
        return new Widget(parent, this);
    }

    QMenu* getContextMenu();

protected:
    QAction     _scatterPlotAction;
    QAction     _densityPlotAction;
    QAction     _contourPlotAction;

    friend class Widget;
};

class PlotAction : public QWidgetAction
{
    Q_OBJECT

public:
    class Widget : public ActionWidget {
    public:
        Widget(QWidget* parent, PlotAction* plotAction);

    private:
        QHBoxLayout         _layout;
        QToolBar            _toolBar;
        QToolButton         _toolButton;
        PopupWidget         _popupWidget;
        QWidgetAction       _popupWidgetAction;
    };

public:
    PlotAction(ScatterplotWidget* scatterplotWidget);

    QWidget* createWidget(QWidget* parent) override {
        return new Widget(parent, this);
    }

    QMenu* getContextMenu();

protected:
    ScatterplotWidget*  _scatterplotWidget;
    DoubleAction        _pointSizeAction;
    DoubleAction        _pointOpacityAction;
    DoubleAction        _sigmaAction;

    friend class Widget;
};

class SelectionAction : public QWidgetAction
{
    Q_OBJECT

public:
    class Widget : public ActionWidget {
    public:
        Widget(QWidget* parent, SelectionAction* selectionAction);

    private:
        QHBoxLayout         _layout;
        QToolBar            _toolBar;
        QToolButton         _toolButton;
        PopupWidget         _popupWidget;
        QWidgetAction       _popupWidgetAction;
    };

public:
    SelectionAction(QObject* parent);

    QWidget* createWidget(QWidget* parent) override {
        return new Widget(parent, this);
    }

    QMenu* getContextMenu();

protected:
    OptionAction    _typeAction;
    QAction         _brushAction;
    QAction         _lassoAction;
    QAction         _polygonAction;
    QAction         _rectangleAction;
    QAction         _modifierAddAction;
    QAction         _modifierRemoveAction;
    DoubleAction    _brushRadiusAction;
    QAction         _clearSelectionAction;
    QAction         _selectAllAction;
    QAction         _invertSelectionAction;
    QAction         _notifyDuringSelectionAction;

    friend class Widget;
};