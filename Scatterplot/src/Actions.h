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

class ScatterplotWidget;

class ActionButton : public QPushButton {
    Q_OBJECT

private:

    QAction *actionOwner = nullptr;

public:
    explicit ActionButton(QWidget *parent = nullptr);
    void setAction(QAction *action);

public slots:
    void updateButtonStatusFromAction();
};

class FloatAction : public QWidgetAction
{
    Q_OBJECT

public:
    class Widget : public QWidget {
    public:
        Widget(QWidget* widget, FloatAction* floatAction);

    private:
        QHBoxLayout     _layout;
        QLabel          _label;
        QDoubleSpinBox  _spinBox;
        QSlider         _slider;
    };
public:
    FloatAction(QObject * parent, const QString& title = "");

    QWidget* createWidget(QWidget* parent) override;

signals:
    void changed(const float& value);
};

class OptionAction : public QWidgetAction
{
    Q_OBJECT

public:
    class Widget : public QWidget {
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

class PopupWidget : public QWidget {
public:
    PopupWidget(QWidget* parent, const QString& title);

    void setContentLayout(QLayout* layout);

private:
    QVBoxLayout     _mainLayout;
    QGroupBox       _groupBox;
};

class PlotPopupWidget : public PopupWidget {
public:
    PlotPopupWidget(QWidget* parent, ScatterplotWidget* scatterplotWidget);

private:
    QVBoxLayout     _layout;
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

using PlotPopupAction = CustomAction<PlotPopupWidget>;


class RenderModePopupWidget : public PopupWidget {
public:
    RenderModePopupWidget(QWidget* parent, ScatterplotWidget* scatterplotWidget);

private:
    QVBoxLayout     _layout;
    ActionButton    _scatterPlotToolButton;
    ActionButton    _densityPlotToolButton;
    ActionButton    _contourPlotToolButton;
};

using RenderModePopupAction = CustomAction<RenderModePopupWidget>;


class RenderModeW : public QWidget
{
    Q_OBJECT

public:
    RenderModeW(ScatterplotWidget* scatterplotWidget);

protected:
    ScatterplotWidget*  _scatterplotWidget;
    QHBoxLayout         _layout;
    QToolBar            _toolBar;
    QToolButton         _toolButton;
};

class PlotSettingsW : public QWidget
{
    Q_OBJECT

public:
    PlotSettingsW(ScatterplotWidget* scatterplotWidget);

protected:
    ScatterplotWidget*  _scatterplotWidget;
    QHBoxLayout         _layout;
    QToolBar            _toolBar;
    QToolButton         _toolButton;
};