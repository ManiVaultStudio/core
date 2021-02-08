#include "Actions.h"
#include "Application.h"

#include "ScatterplotWidget.h"

ActionButton::ActionButton(QWidget *parent) :
    QPushButton(parent)
{
}

ActionButton::ActionButton(QAction *action) :
    QPushButton()
{
    setAction(action);
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

DoubleAction::Widget::Widget(QWidget* parent, DoubleAction* doubleAction) :
    ActionWidget(parent, doubleAction),
    _layout(),
    _label(QString("%1:").arg(doubleAction->text())),
    _spinBox(),
    _slider(Qt::Horizontal)
{
    setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred));

    if (childOfWidget()) {
        _label.setFixedWidth(80);
    }

    if (childOfMenu()) {
        _label.setFixedWidth(80);
        _spinBox.setFixedWidth(60);
        _slider.setFixedWidth(100);
    }
    
    if (childOfToolbar()) {
        _spinBox.setFixedWidth(60);
        _slider.setFixedWidth(60);
    }

    _layout.setMargin(0);
    _layout.addWidget(&_label);
    _layout.addWidget(&_spinBox);
    _layout.addWidget(&_slider);

    setLayout(&_layout);

    //_label.setVisible(childOfMenu());

    const auto setToolTips = [this, doubleAction]() {
        _label.setToolTip(doubleAction->text());

        const auto toolTip = QString("%1: %2%3").arg(doubleAction->text(), QString::number(doubleAction->getValue(), 'f', doubleAction->getDecimals()), doubleAction->getSuffix());

        _spinBox.setToolTip(toolTip);
        _slider.setToolTip(toolTip);
    };

    connect(&_spinBox, qOverload<double>(&QDoubleSpinBox::valueChanged), this, [this, doubleAction](double value) {
        doubleAction->setValue(value);
    });

    connect(&_slider, &QSlider::valueChanged, this, [this, doubleAction](int value) {
        doubleAction->setValue(static_cast<double>(value) / static_cast<double>(SLIDER_MULTIPLIER));
    });

    const auto updateValue = [this, doubleAction, setToolTips]() {
        QSignalBlocker spinBoxBlocker(&_spinBox), sliderBlocker(&_slider);

        const auto value = doubleAction->getValue();

        if (value != _spinBox.value())
            _spinBox.setValue(value);

        if (value * SLIDER_MULTIPLIER != _slider.value())
            _slider.setValue(value * static_cast<double>(SLIDER_MULTIPLIER));

        setToolTips();
    };

    connect(doubleAction, &DoubleAction::valueChanged, this, [this, doubleAction, updateValue](const double& value) {
        updateValue();
    });

    const auto updateValueRange = [this, doubleAction]() {
        QSignalBlocker spinBoxBlocker(&_spinBox), sliderBlocker(&_slider);

        const auto minValue = doubleAction->getMinValue();

        _spinBox.setMinimum(minValue);
        _slider.setMinimum(minValue * SLIDER_MULTIPLIER);

        const auto maxValue = doubleAction->getMaxValue();

        _spinBox.setMaximum(maxValue);
        _slider.setMaximum(maxValue * SLIDER_MULTIPLIER);
    };

    connect(doubleAction, &DoubleAction::minValueChanged, this, [this, doubleAction, updateValueRange](const double& minValue) {
        updateValueRange();
    });

    connect(doubleAction, &DoubleAction::maxValueChanged, this, [this, doubleAction, updateValueRange](const double& maxValue) {
        updateValueRange();
    });

    const auto updateSuffix = [this, doubleAction, setToolTips]() {
        QSignalBlocker spinBoxBlocker(&_spinBox);

        _spinBox.setSuffix(doubleAction->getSuffix());

        setToolTips();
    };
    
    connect(doubleAction, &DoubleAction::suffixChanged, this, [this, doubleAction, updateSuffix](const QString& suffix) {
        updateSuffix();
    });

    const auto updateDecimals = [this, doubleAction]() {
        QSignalBlocker spinBoxBlocker(&_spinBox);

        _spinBox.setDecimals(doubleAction->getDecimals());
    };

    connect(doubleAction, &DoubleAction::decimalsChanged, this, [this, doubleAction, updateDecimals](const std::int32_t& decimals) {
        updateDecimals();
    });

    updateValueRange();
    updateValue();
    updateSuffix();
    updateDecimals();
    setToolTips();
}

DoubleAction::DoubleAction(QObject * parent, const QString& title /*= ""*/) :
    QWidgetAction(parent),
    _value(0.0f)
{
    setText(title);
    setMinValue(DEFAULT_MIN_VALUE);
    setMaxValue(DEFAULT_MAX_VALUE);
    setDecimals(DEFAULT_DECIMALS);
}

QWidget* DoubleAction::createWidget(QWidget* parent)
{
    if (dynamic_cast<QMenu*>(parent))
        qDebug() << "Parent is a menu";

    if (dynamic_cast<QToolBar*>(parent))
        qDebug() << "Parent is a toolbar";

    /*
    if (dynamic_cast<QWidget*>(parent))
        qDebug() << "Parent is a widget";
    */

    auto widget = new Widget(parent, this);

    widget->show();

    return widget;
}

OptionAction::Widget::Widget(QWidget* widget, OptionAction* optionAction) :
    ActionWidget(widget, optionAction),
    _layout(),
    _label(QString("%1:").arg(optionAction->text())),
    _comboBox()
{
    _label.setFixedWidth(80);

    _layout.setMargin(0);
    _layout.addWidget(&_label);
    _layout.addWidget(&_comboBox);

    //_label.setVisible(childOfMenu());

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

PositionAction::Widget::Widget(QWidget* parent, PositionAction* positionAction) :
    ActionWidget(parent, positionAction),
    _layout(),
    _toolBar(),
    _toolButton(),
    _popupWidget(this, "Position"),
    _popupWidgetAction(this)
{
    _layout.addWidget(&_toolBar);

    _toolBar.addAction(positionAction->_xDimensionAction);
    _toolBar.addAction(positionAction->_yDimensionAction);
    _toolBar.addWidget(&_toolButton);

    auto popupLayout = new QVBoxLayout();

    popupLayout->addWidget(positionAction->_xDimensionAction->createWidget(this));
    popupLayout->addWidget(positionAction->_yDimensionAction->createWidget(this));

    _popupWidget.setContentLayout(popupLayout);

    _popupWidgetAction.setDefaultWidget(&_popupWidget);

    _toolButton.setPopupMode(QToolButton::InstantPopup);
    _toolButton.addAction(&_popupWidgetAction);

    setLayout(&_layout);
}

PositionAction::PositionAction(QObject* parent, OptionAction* xDimensionAction, OptionAction* yDimensionAction) :
    QWidgetAction(parent),
    _xDimensionAction(xDimensionAction),
    _yDimensionAction(yDimensionAction)
{
}













PlotAction::PlotAction(ScatterplotWidget* scatterplotWidget) :
    QWidgetAction(scatterplotWidget),
    _scatterplotWidget(scatterplotWidget),
    _pointSizeAction(this, "Point size"),
    _pointOpacityAction(this, "Point opacity"),
    _sigmaAction(this, "Sigma")
{
    _pointSizeAction.setDisabled(true);
    _pointSizeAction.setSuffix("px");
    _pointOpacityAction.setSuffix("%");

    const auto updateRenderMode = [this, scatterplotWidget]() {
        const auto renderMode = scatterplotWidget->getRenderMode();

        _pointSizeAction.setVisible(renderMode == ScatterplotWidget::SCATTERPLOT);
        _pointOpacityAction.setVisible(renderMode == ScatterplotWidget::SCATTERPLOT);
        _sigmaAction.setVisible(renderMode != ScatterplotWidget::SCATTERPLOT);
    };

    connect(scatterplotWidget, &ScatterplotWidget::renderModeChanged, this, [this, updateRenderMode](const ScatterplotWidget::RenderMode& renderMode) {
        updateRenderMode();
    });

    updateRenderMode();
}

QMenu* PlotAction::getContextMenu()
{
    auto menu = new QMenu("Plot");

    const auto renderMode = _scatterplotWidget->getRenderMode();

    switch (renderMode)
    {
        case ScatterplotWidget::RenderMode::SCATTERPLOT:
            menu->addAction(&_pointSizeAction);
            menu->addAction(&_pointOpacityAction);
            break;

        case ScatterplotWidget::RenderMode::DENSITY:
        case ScatterplotWidget::RenderMode::LANDSCAPE:
            menu->addAction(&_sigmaAction);
            break;

        default:
            break;
    }

    return menu;
}

PlotAction::Widget::Widget(QWidget* parent, PlotAction* plotAction) :
    ActionWidget(parent, plotAction),
    _layout(),
    _toolBar(),
    _toolButton(),
    _popupWidget(this, "Plot"),
    _popupWidgetAction(this)
{
    _layout.addWidget(&_toolBar);

    _toolBar.addAction(&plotAction->_pointSizeAction);
    _toolBar.addAction(&plotAction->_pointOpacityAction);
    _toolBar.addAction(&plotAction->_sigmaAction);

    _popupWidgetAction.setDefaultWidget(&_popupWidget);

    _toolButton.setPopupMode(QToolButton::InstantPopup);
    _toolButton.addAction(&_popupWidgetAction);

    setLayout(&_layout);
}






































RenderModeAction::RenderModeAction(ScatterplotWidget* scatterplotWidget) :
    QWidgetAction(scatterplotWidget),
    _scatterPlotAction("Scatter plot"),
    _densityPlotAction("Density plot"),
    _contourPlotAction("Contour plot")
{
    _scatterPlotAction.setCheckable(true);
    _densityPlotAction.setCheckable(true);
    _contourPlotAction.setCheckable(true);

    _scatterPlotAction.setShortcut(QKeySequence("S"));
    _densityPlotAction.setShortcut(QKeySequence("D"));
    _contourPlotAction.setShortcut(QKeySequence("C"));

    _scatterPlotAction.setToolTip("Set render mode to scatter plot");
    _densityPlotAction.setToolTip("Set render mode to density plot");
    _contourPlotAction.setToolTip("Set render mode to contour plot");

    const auto& fontAwesome = Application::getIconFont("FontAwesome");

    _scatterPlotAction.setIcon(fontAwesome.getIcon("braille"));
    _densityPlotAction.setIcon(fontAwesome.getIcon("cloud"));
    _contourPlotAction.setIcon(fontAwesome.getIcon("mountain"));

    connect(&_scatterPlotAction, &QAction::triggered, this, [this, scatterplotWidget]() {
        scatterplotWidget->setRenderMode(ScatterplotWidget::RenderMode::SCATTERPLOT);
    });

    connect(&_densityPlotAction, &QAction::triggered, this, [this, scatterplotWidget]() {
        scatterplotWidget->setRenderMode(ScatterplotWidget::RenderMode::DENSITY);
    });

    connect(&_contourPlotAction, &QAction::triggered, this, [this, scatterplotWidget]() {
        scatterplotWidget->setRenderMode(ScatterplotWidget::RenderMode::LANDSCAPE);
    });

    const auto updateButtons = [this, scatterplotWidget]() {
        const auto renderMode = scatterplotWidget->getRenderMode();

        QSignalBlocker scatterPlotActionBlocker(&_scatterPlotAction), densityPlotAction(&_densityPlotAction), contourPlotAction(&_contourPlotAction);

        _scatterPlotAction.setChecked(renderMode == ScatterplotWidget::RenderMode::SCATTERPLOT);
        _densityPlotAction.setChecked(renderMode == ScatterplotWidget::RenderMode::DENSITY);
        _contourPlotAction.setChecked(renderMode == ScatterplotWidget::RenderMode::LANDSCAPE);
    };

    connect(scatterplotWidget, &ScatterplotWidget::renderModeChanged, this, [this, updateButtons](const ScatterplotWidget::RenderMode& renderMode) {
        updateButtons();
    });

    updateButtons();
}

QMenu* RenderModeAction::getContextMenu()
{
    auto menu = new QMenu("Render mode");

    menu->addAction(&_scatterPlotAction);
    menu->addAction(&_densityPlotAction);
    menu->addAction(&_contourPlotAction);

    return menu;
}

RenderModeAction::Widget::Widget(QWidget* parent, RenderModeAction* renderModeAction) :
    ActionWidget(parent, renderModeAction),
    _layout(),
    _toolBar(),
    _toolButton(),
    _popupWidget(this, "Render mode"),
    _popupWidgetAction(this)
{
    _layout.addWidget(&_toolBar);

    _toolBar.addAction(&renderModeAction->_scatterPlotAction);
    _toolBar.addAction(&renderModeAction->_densityPlotAction);
    _toolBar.addAction(&renderModeAction->_contourPlotAction);

    _popupWidgetAction.setDefaultWidget(&_popupWidget);

    _toolButton.setPopupMode(QToolButton::InstantPopup);
    _toolButton.addAction(&_popupWidgetAction);

    setLayout(&_layout);
}

SelectionAction::SelectionAction(QObject* parent) :
    QWidgetAction(parent),
    _typeAction(this, "Type"),
    _brushAction("Brush"),
    _lassoAction("Lasso"),
    _polygonAction("Polygon"),
    _rectangleAction("Rectangle"),
    _modifierAddAction("+"),
    _modifierRemoveAction("-"),
    _brushRadiusAction(this, "Brush radius"),
    _clearSelectionAction("Select none"),
    _selectAllAction("Select all"),
    _invertSelectionAction("Invert selection"),
    _notifyDuringSelectionAction("Notify during selection")
{
    _brushAction.setShortcut(QKeySequence("B"));
    _lassoAction.setShortcut(QKeySequence("L"));
    _polygonAction.setShortcut(QKeySequence("P"));
    _rectangleAction.setShortcut(QKeySequence("R"));
    _clearSelectionAction.setShortcut(QKeySequence("E"));
    _selectAllAction.setShortcut(QKeySequence("A"));
    _invertSelectionAction.setShortcut(QKeySequence("I"));
    _notifyDuringSelectionAction.setShortcut(QKeySequence("U"));

    _brushAction.setCheckable(true);
    _lassoAction.setCheckable(true);
    _polygonAction.setCheckable(true);
    _rectangleAction.setCheckable(true);
    _notifyDuringSelectionAction.setCheckable(true);
}

QMenu* SelectionAction::getContextMenu()
{
    auto menu = new QMenu("Selection");

    menu->addAction(&_brushAction);
    menu->addAction(&_lassoAction);
    menu->addAction(&_polygonAction);
    menu->addAction(&_rectangleAction);

    menu->addSeparator();

    menu->addAction(&_clearSelectionAction);
    menu->addAction(&_selectAllAction);
    menu->addAction(&_invertSelectionAction);

    menu->addSeparator();
    
    menu->addAction(&_notifyDuringSelectionAction);

    return menu;
}

SelectionAction::Widget::Widget(QWidget* parent, SelectionAction* selectionAction) :
    ActionWidget(parent, selectionAction),
    _layout(),
    _toolBar(),
    _toolButton(),
    _popupWidget(this, "Selection"),
    _popupWidgetAction(this)
{
    _layout.addWidget(&_toolBar);

    _toolBar.addAction(&selectionAction->_typeAction);
    _toolBar.addAction(&selectionAction->_modifierAddAction);
    _toolBar.addAction(&selectionAction->_modifierRemoveAction);
    _toolBar.addAction(&selectionAction->_brushRadiusAction);
    _toolBar.addAction(&selectionAction->_clearSelectionAction);
    _toolBar.addAction(&selectionAction->_selectAllAction);
    _toolBar.addAction(&selectionAction->_invertSelectionAction);
    _toolBar.addAction(&selectionAction->_notifyDuringSelectionAction);
    _toolBar.addWidget(&_toolButton);

    auto popupLayout = new QGridLayout();

    auto typeWidget = new QWidget();

    auto typeWidgetLayout = new QHBoxLayout();

    typeWidgetLayout->addWidget(selectionAction->_typeAction.createWidget(this));
    typeWidgetLayout->addWidget(new ActionButton(&selectionAction->_modifierAddAction));
    typeWidgetLayout->addWidget(new ActionButton(&selectionAction->_modifierRemoveAction));

    typeWidget->setLayout(typeWidgetLayout);

    popupLayout->addWidget(new QLabel("Type:"), 0, 0);
    popupLayout->addWidget(typeWidget, 0, 1);

    popupLayout->addWidget(new QLabel("Brush radius:"), 1, 0);
    popupLayout->addWidget(selectionAction->_brushRadiusAction.createWidget(this), 1, 1);

    auto selectWidget = new QWidget();

    auto selectWidgetLayout = new QHBoxLayout();

    selectWidgetLayout->addWidget(new ActionButton(&selectionAction->_clearSelectionAction));
    selectWidgetLayout->addWidget(new ActionButton(&selectionAction->_selectAllAction));
    selectWidgetLayout->addWidget(new ActionButton(&selectionAction->_invertSelectionAction));

    selectWidget->setLayout(selectWidgetLayout);

    popupLayout->addWidget(new QLabel("Select:"), 2, 0);
    popupLayout->addWidget(selectWidget, 2, 1);

    popupLayout->addWidget(selectWidget, 3, 1);

    _popupWidget.setContentLayout(popupLayout);

    _popupWidgetAction.setDefaultWidget(&_popupWidget);

    _toolButton.setPopupMode(QToolButton::InstantPopup);
    _toolButton.addAction(&_popupWidgetAction);

    setLayout(&_layout);
}