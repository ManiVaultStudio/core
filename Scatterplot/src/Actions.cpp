#include "Actions.h"

#include "ScatterplotWidget.h"

ActionButton::ActionButton(QWidget *parent) :
    QPushButton(parent)
{
}

void ActionButton::setAction(QAction *action) {

    // if I've got already an action associated to the button
    // remove all connections
    if (actionOwner && actionOwner != action) {
        disconnect(actionOwner, &QAction::changed,
            this, &ActionButton::updateButtonStatusFromAction);
        disconnect(this, &ActionButton::clicked,
            actionOwner, &QAction::trigger);
    }
    // store the action
    actionOwner = action;
    // configure the button
    updateButtonStatusFromAction();
    // connect the action and the button
    // so that when the action is changed the
    // button is changed too!
    connect(action, &QAction::changed,
        this, &ActionButton::updateButtonStatusFromAction);
    // connect the button to the slot that forwards the
    // signal to the action
    connect(this, &ActionButton::clicked,
        actionOwner, &QAction::trigger);
}

void ActionButton::updateButtonStatusFromAction() {

    if (!actionOwner)
        return;

    setText(actionOwner->text());
    setStatusTip(actionOwner->statusTip());
    setToolTip(actionOwner->toolTip());
    setIcon(actionOwner->icon());
    setEnabled(actionOwner->isEnabled());
    setCheckable(actionOwner->isCheckable());
    setChecked(actionOwner->isChecked());
}

FloatAction::Widget::Widget(QWidget* widget, FloatAction* floatAction) :
    QWidget(widget),
    _layout(),
    _label(QString("%1:").arg(floatAction->text())),
    _spinBox(),
    _slider(Qt::Horizontal)
{
    _label.setFixedWidth(80);

    _layout.setMargin(0);
    _layout.addWidget(&_label);
    _layout.addWidget(&_spinBox);
    _layout.addWidget(&_slider);

    setLayout(&_layout);

    connect(floatAction, &FloatAction::changed, this, [this]() {
    });
}

FloatAction::FloatAction(QObject * parent, const QString& title /*= ""*/) :
    QWidgetAction(parent)
{
    setText(title);
}

QWidget* FloatAction::createWidget(QWidget* parent)
{
    auto widget = new Widget(parent, this);

    widget->show();

    return widget;
}

OptionAction::Widget::Widget(QWidget* widget, OptionAction* optionAction) :
    QWidget(widget),
    _layout(),
    _label(QString("%1:").arg(optionAction->text())),
    _comboBox()
{
    _label.setFixedWidth(80);

    _layout.setMargin(0);
    _layout.addWidget(&_label);
    _layout.addWidget(&_comboBox);

    setLayout(&_layout);
}

OptionAction::OptionAction(QObject* parent, const QString& title /*= ""*/) :
    QWidgetAction(parent)
{
    setText(title);
}

QWidget* OptionAction::createWidget(QWidget* parent)
{
    auto widget = new Widget(parent, this);

    widget->show();

    return widget;
}

RenderModeW::RenderModeW(ScatterplotWidget* scatterplotWidget) :
    QWidget(),
    _scatterplotWidget(scatterplotWidget),
    _layout(),
    _toolBar(),
    _toolButton()
{
    _layout.setMargin(0);
    _layout.addWidget(&_toolBar);

    _toolBar.setIconSize(QSize(16, 16));
    _toolBar.addAction(&scatterplotWidget->getScatterplotAction());
    _toolBar.addAction(&scatterplotWidget->getDensityPlotAction());
    _toolBar.addAction(&scatterplotWidget->getContourPlotAction());
    _toolBar.addWidget(&_toolButton);

    _toolButton.setToolButtonStyle(Qt::ToolButtonIconOnly);
    _toolButton.setPopupMode(QToolButton::InstantPopup);

    _toolButton.setPopupMode(QToolButton::InstantPopup);
    _toolButton.addAction(&scatterplotWidget->getRenderModePopupAction());

    setLayout(&_layout);
}

PlotSettingsW::PlotSettingsW(ScatterplotWidget* scatterplotWidget) :
    QWidget(),
    _scatterplotWidget(scatterplotWidget),
    _layout(),
    _toolBar(),
    _toolButton()
{
    _layout.setMargin(0);
    _layout.addWidget(&_toolBar);

    _toolBar.setIconSize(QSize(16, 16));
    _toolBar.addAction(&scatterplotWidget->getPointSizeAction());
    _toolBar.addAction(&scatterplotWidget->getPointOpacityAction());
    _toolBar.addAction(&scatterplotWidget->getSigmaAction());
    _toolBar.addWidget(&_toolButton);

    _toolButton.setPopupMode(QToolButton::InstantPopup);
    _toolButton.addAction(&scatterplotWidget->getPlotPopupAction());

    setLayout(&_layout);
}

PopupWidget::PopupWidget(QWidget* parent, const QString& title) :
    QWidget(parent),
    _mainLayout(),
    _groupBox(title)
{
    _mainLayout.setMargin(4);
    _mainLayout.addWidget(&_groupBox);

    setLayout(&_mainLayout);
}

void PopupWidget::setContentLayout(QLayout* layout)
{
    _groupBox.setLayout(layout);
}

PlotPopupWidget::PlotPopupWidget(QWidget* parent, ScatterplotWidget* scatterplotWidget) :
    PopupWidget(parent, "Plot"),
    _layout()
{
    const auto renderMode = scatterplotWidget->getRenderMode();

    switch (renderMode)
    {
        case ScatterplotWidget::SCATTERPLOT:
            _layout.addWidget(scatterplotWidget->getPointSizeAction().createWidget(this));
            _layout.addWidget(scatterplotWidget->getPointOpacityAction().createWidget(this));
            break;

        case ScatterplotWidget::DENSITY:
        case ScatterplotWidget::LANDSCAPE:
            _layout.addWidget(scatterplotWidget->getSigmaAction().createWidget(this));
            break;

        default:
            break;
    }

    setContentLayout(&_layout);
}


RenderModePopupWidget::RenderModePopupWidget(QWidget* parent, ScatterplotWidget* scatterplotWidget) :
    PopupWidget(parent, "Render mode"),
    _layout(),
    _scatterPlotToolButton(),
    _densityPlotToolButton(),
    _contourPlotToolButton()
{
    _layout.addWidget(&_scatterPlotToolButton);
    _layout.addWidget(&_densityPlotToolButton);
    _layout.addWidget(&_contourPlotToolButton);
    
    _scatterPlotToolButton.setAction(&scatterplotWidget->getScatterplotAction());
    _densityPlotToolButton.setAction(&scatterplotWidget->getDensityPlotAction());
    _contourPlotToolButton.setAction(&scatterplotWidget->getContourPlotAction());

    setContentLayout(&_layout);
}