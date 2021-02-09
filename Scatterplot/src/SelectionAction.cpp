#include "SelectionAction.h"
#include "Application.h"

#include "ScatterplotPlugin.h"

#include "widgets/ActionPushButton.h"

using namespace hdps::gui;

SelectionAction::SelectionAction(ScatterplotPlugin* scatterplotPlugin) :
    PluginAction(scatterplotPlugin),
    _typeAction(this, "Type"),
    _brushAction("Brush"),
    _lassoAction("Lasso"),
    _polygonAction("Polygon"),
    _rectangleAction("Rectangle"),
    _typeActionGroup(this),
    _modifierAddAction(this),
    _modifierRemoveAction(""),
    _brushRadiusAction(this, "Brush radius"),
    _clearSelectionAction("Select none"),
    _selectAllAction("Select all"),
    _invertSelectionAction("Invert selection"),
    _notifyDuringSelectionAction("Notify during selection")
{
    const auto& fontAwesome = Application::getIconFont("FontAwesome");

    //_modifierAddAction.setAutoRepeat(false);

    _brushAction.setShortcut(QKeySequence("B"));
    _lassoAction.setShortcut(QKeySequence("L"));
    _polygonAction.setShortcut(QKeySequence("P"));
    _rectangleAction.setShortcut(QKeySequence("R"));
    _modifierAddAction.setShortcut(QKeySequence("Shift"));
    _modifierRemoveAction.setShortcut(QKeySequence(Qt::Key_Control));
    _clearSelectionAction.setShortcut(QKeySequence("E"));
    _selectAllAction.setShortcut(QKeySequence("A"));
    _invertSelectionAction.setShortcut(QKeySequence("I"));
    _notifyDuringSelectionAction.setShortcut(QKeySequence("U"));

    _brushAction.setToolTip("Select data points using a brush tool");
    _lassoAction.setToolTip("Select data points using a lasso");
    _polygonAction.setToolTip("Select data points by drawing a polygon");
    _rectangleAction.setToolTip("Select data points inside a rectangle");
    _modifierAddAction.setToolTip("Add items to the existing selection");
    _modifierRemoveAction.setToolTip("Remove items from the existing selection");
    _brushRadiusAction.setToolTip("Brush selection tool radius");
    _clearSelectionAction.setToolTip("Clears the selection");
    _selectAllAction.setToolTip("Select all data points");
    _invertSelectionAction.setToolTip("Invert the selection");
    _notifyDuringSelectionAction.setToolTip("Notify during selection or only at the end of the selection process");

    _modifierAddAction.setIcon(fontAwesome.getIcon("plus"));
    _modifierRemoveAction.setIcon(fontAwesome.getIcon("minus"));
    
    _brushAction.setCheckable(true);
    _lassoAction.setCheckable(true);
    _polygonAction.setCheckable(true);
    _rectangleAction.setCheckable(true);
    _modifierAddAction.setCheckable(true);
    _modifierRemoveAction.setCheckable(true);
    _notifyDuringSelectionAction.setCheckable(true);

    _typeActionGroup.addAction(&_brushAction);
    _typeActionGroup.addAction(&_lassoAction);
    _typeActionGroup.addAction(&_polygonAction);
    _typeActionGroup.addAction(&_rectangleAction);
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
    WidgetAction::Widget(parent, selectionAction),
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
    typeWidgetLayout->addWidget(new ActionPushButton(&selectionAction->_modifierAddAction));
    typeWidgetLayout->addWidget(new ActionPushButton(&selectionAction->_modifierRemoveAction));

    typeWidget->setLayout(typeWidgetLayout);

    popupLayout->addWidget(new QLabel("Type:"), 0, 0);
    popupLayout->addWidget(typeWidget, 0, 1);

    popupLayout->addWidget(new QLabel("Brush radius:"), 1, 0);
    popupLayout->addWidget(selectionAction->_brushRadiusAction.createWidget(this), 1, 1);

    auto selectWidget = new QWidget();

    auto selectWidgetLayout = new QHBoxLayout();

    selectWidgetLayout->addWidget(new ActionPushButton(&selectionAction->_clearSelectionAction));
    selectWidgetLayout->addWidget(new ActionPushButton(&selectionAction->_selectAllAction));
    selectWidgetLayout->addWidget(new ActionPushButton(&selectionAction->_invertSelectionAction));

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

/*
SelectionSettingsWidget::SelectionSettingsWidget(const hdps::gui::ResponsiveSectionWidget::WidgetState& widgetState, QWidget* parent ) :
ScatterplotSettingsWidget(widgetState, parent),
_typeLabel(new QLabel("Type:")),
_typeLayout(new QHBoxLayout()),
_typeWidget(new QWidget()),
_typeComboBox(new QComboBox()),
_modifierAddPushButton(new QPushButton()),
_modifierRemovePushButton(new QPushButton()),
_radiusLabel(new QLabel("Brush radius:")),
_radiusWidget(new QWidget()),
_radiusLayout(new QHBoxLayout()),
_radiusDoubleSpinBox(new QDoubleSpinBox()),
_radiusSlider(new QSlider()),
_selectLabel(new QLabel("Select:")),
_selectWidget(new QWidget()),
_selectLayout(new QHBoxLayout()),
_clearSelectionPushButton(new QPushButton("None")),
_selectAllPushButton(new QPushButton("All")),
_invertSelectionPushButton(new QPushButton("Invert")),
_notifyDuringSelectionCheckBox(new QCheckBox("Notify during selection")),
_additionalSettingsPopupPushButton(new PopupPushButton())
{
    const auto& fontAwesome = Application::getIconFont("FontAwesome");

    _typeLabel->hide();
    _typeLabel->setToolTip("Selection type");

    _typeLayout->setMargin(ResponsiveToolBar::LAYOUT_MARGIN);
    _typeLayout->setSpacing(ResponsiveToolBar::LAYOUT_SPACING);

    _typeLayout->addWidget(_typeComboBox);
    _typeLayout->addWidget(_modifierAddPushButton);
    _typeLayout->addWidget(_modifierRemovePushButton);

    _typeWidget->setLayout(_typeLayout);

    _typeComboBox->setToolTip("Choose the selection type");
    _typeComboBox->addItems(QStringList(PixelSelectionTool::types.keys()));
    _typeComboBox->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum));
    //_typeComboBox->setFixedWidth(70);
    //_typeComboBox->setEditable(true);
    //_typeComboBox->lineEdit()->setReadOnly(true);
    //_typeComboBox->lineEdit()->setAlignment(Qt::AlignCenter);

    

    _radiusLabel->setToolTip("Brush radius");

    _radiusLayout->setMargin(ResponsiveToolBar::LAYOUT_MARGIN);
    _radiusLayout->setSpacing(ResponsiveToolBar::LAYOUT_SPACING);

    _radiusLayout->addWidget(_radiusDoubleSpinBox);
    _radiusLayout->addWidget(_radiusSlider);

    _radiusWidget->setLayout(_radiusLayout);

    _radiusDoubleSpinBox->setToolTip("Brush radius");
    _radiusDoubleSpinBox->setDecimals(1);
    _radiusDoubleSpinBox->setSuffix("px");
    _radiusDoubleSpinBox->setMinimum(static_cast<double>(PixelSelectionTool::RADIUS_MIN));
    _radiusDoubleSpinBox->setMaximum(static_cast<double>(PixelSelectionTool::RADIUS_MAX));
    _radiusDoubleSpinBox->setSingleStep(5.0);

    _radiusSlider->setToolTip("Brush radius");
    _radiusSlider->setOrientation(Qt::Horizontal);
    _radiusSlider->setMinimum(static_cast<int>(1000.0 * PixelSelectionTool::RADIUS_MIN));
    _radiusSlider->setMaximum(static_cast<int>(1000.0 * PixelSelectionTool::RADIUS_MAX));
    _radiusSlider->setSingleStep(5000);

    _selectLabel->setToolTip("Select");

    _selectLayout->setMargin(ResponsiveToolBar::LAYOUT_MARGIN);
    _selectLayout->setSpacing(ResponsiveToolBar::LAYOUT_SPACING);

    _selectLayout->addWidget(_clearSelectionPushButton);
    _selectLayout->addWidget(_selectAllPushButton);
    _selectLayout->addWidget(_invertSelectionPushButton);
    _selectLayout->addStretch(1);

    _selectWidget->setLayout(_selectLayout);

    _clearSelectionPushButton->setToolTip("Removes all items from the selection");
    _selectAllPushButton->setToolTip("Select all items");
    _invertSelectionPushButton->setToolTip("Invert the selection");

    _notifyDuringSelectionCheckBox->setToolTip("Whether the selection updates are published continuously or at end of the selection process");

    _additionalSettingsPopupPushButton->setToolTip("Additional selection settings");
    _additionalSettingsPopupPushButton->setIcon(fontAwesome.getIcon("ellipsis-h"));

    if (isForm()) {
        auto layout = new QGridLayout();

        applyLayout(layout);

        layout->addWidget(_typeLabel, 0, 0);
        layout->addWidget(_typeWidget, 0, 1);
        layout->addWidget(_radiusLabel, 1, 0);
        layout->addWidget(_radiusWidget, 1, 1);
        layout->addWidget(_selectLabel, 2, 0);
        layout->addWidget(_selectWidget, 2, 1);
        layout->addWidget(_notifyDuringSelectionCheckBox, 3, 1);
    }

    if (isSequential()) {
        if (isCompact()) {
            auto layout = new QHBoxLayout();

            applyLayout(layout);

            layout->addWidget(_typeWidget);
            layout->addWidget(_additionalSettingsPopupPushButton);
        }

        if (isFull()) {
            auto layout = new QHBoxLayout();

            applyLayout(layout);

            layout->addWidget(_typeWidget);
            layout->addWidget(_selectWidget);
            layout->addWidget(_radiusWidget);
            layout->addWidget(_notifyDuringSelectionCheckBox);
        }
    }

    //setLayout(stateLayout);
}

void SelectionSettingsWidget::setScatterplotPlugin(ScatterplotPlugin* scatterplotPlugin)
{
    Q_ASSERT(scatterplotPlugin != nullptr);

    _scatterplotPlugin = scatterplotPlugin;

    auto& pixelSelectionTool = _scatterplotPlugin->getSelectionTool();

    connect(_typeComboBox, &QComboBox::currentTextChanged, [this, &pixelSelectionTool](QString currentText) {
        pixelSelectionTool.setType(PixelSelectionTool::getTypeEnum(currentText));
    });

    connect(_modifierAddPushButton, &QPushButton::toggled, [this, &pixelSelectionTool](bool checked) {
        pixelSelectionTool.setModifier(checked ? PixelSelectionTool::Modifier::Add : PixelSelectionTool::Modifier::Replace);
    });

    connect(_modifierRemovePushButton, &QPushButton::toggled, [this, &pixelSelectionTool](bool checked) {
        pixelSelectionTool.setModifier(checked ? PixelSelectionTool::Modifier::Remove : PixelSelectionTool::Modifier::Replace);
    });

    connect(_radiusDoubleSpinBox, qOverload<double>(&QDoubleSpinBox::valueChanged), [this, &pixelSelectionTool](double value) {
        pixelSelectionTool.setRadius(static_cast<float>(value));
    });

    connect(_radiusSlider, qOverload<int>(&QSlider::valueChanged), [this, &pixelSelectionTool](int value) {
        pixelSelectionTool.setRadius(PixelSelectionTool::RADIUS_MIN + ((static_cast<float>(value) - _radiusSlider->minimum()) / 1000.0f));
    });

    connect(_selectAllPushButton, &QPushButton::clicked, [this]() {
        _scatterplotPlugin->selectAll();
    });

    connect(_clearSelectionPushButton, &QPushButton::clicked, [this]() {
        _scatterplotPlugin->clearSelection();
    });

    connect(_invertSelectionPushButton, &QPushButton::clicked, [this]() {
        _scatterplotPlugin->invertSelection();
    });

    connect(_notifyDuringSelectionCheckBox, &QCheckBox::toggled, [this, &pixelSelectionTool](bool checked) {
        pixelSelectionTool.setNotifyDuringSelection(checked);
    });

    const auto updateTypeUI = [this, &pixelSelectionTool]() {
        const auto canSelect = _scatterplotPlugin->canSelect();

        _typeLabel->setEnabled(canSelect);
        _typeComboBox->setCurrentText(PixelSelectionTool::getTypeName(pixelSelectionTool.getType()));
    };

    const auto updateModifierUI = [this, &pixelSelectionTool]() {
        const auto canSelect = _scatterplotPlugin->canSelect();

        switch (pixelSelectionTool.getModifier())
        {
            case PixelSelectionTool::Modifier::Replace:
            {
                _modifierAddPushButton->setChecked(false);
                _modifierRemovePushButton->setChecked(false);
                break;
            }

            case PixelSelectionTool::Modifier::Add:
            {
                _modifierAddPushButton->setChecked(true);
                _modifierRemovePushButton->setChecked(false);
                break;
            }

            case PixelSelectionTool::Modifier::Remove:
            {
                _modifierAddPushButton->setChecked(false);
                _modifierRemovePushButton->setChecked(true);
                break;
            }

            default:
                break;
        }

        _modifierAddPushButton->setEnabled(canSelect);
        _modifierRemovePushButton->setEnabled(canSelect);
    };

    const auto updateRadiusUI = [this, &pixelSelectionTool]() {
        auto radiusEnabled = false;

        if (_scatterplotPlugin->canSelect()) {
            switch (pixelSelectionTool.getType())
            {
                case PixelSelectionTool::Type::Rectangle:
                    break;

                case PixelSelectionTool::Type::Brush:
                {
                    radiusEnabled = true;
                    break;
                }

                case PixelSelectionTool::Type::Lasso:
                case PixelSelectionTool::Type::Polygon:
                    break;

                default:
                    break;
            }
        }

        _radiusLabel->setEnabled(radiusEnabled);
        _radiusDoubleSpinBox->setEnabled(radiusEnabled);
        _radiusSlider->setEnabled(radiusEnabled);
    };

    connect(&pixelSelectionTool, &PixelSelectionTool::typeChanged, this, [this, updateTypeUI, updateRadiusUI](const PixelSelectionTool::Type& type) {
        updateTypeUI();
        updateRadiusUI();
    });

    connect(&pixelSelectionTool, &PixelSelectionTool::modifierChanged, this, [this, updateModifierUI](const PixelSelectionTool::Modifier& modifier) {
        updateModifierUI();
    });

    connect(&pixelSelectionTool, &PixelSelectionTool::radiusChanged, this, [this, &pixelSelectionTool](const float& radius) {
        _radiusDoubleSpinBox->setValue(pixelSelectionTool.getRadius());
        _radiusSlider->setValue(pixelSelectionTool.getRadius() * 1000.0f);
    });

    connect(&pixelSelectionTool, &PixelSelectionTool::notifyDuringSelectionChanged, this, [this](const bool& notifyDuringSelection) {
        _notifyDuringSelectionCheckBox->setChecked(notifyDuringSelection);
    });

    const auto updateSelectionButtons = [this]() {
        _selectAllPushButton->setEnabled(_scatterplotPlugin->canSelectAll());
        _clearSelectionPushButton->setEnabled(_scatterplotPlugin->canClearSelection());
        _invertSelectionPushButton->setEnabled(_scatterplotPlugin->canInvertSelection());
    };

    connect(_scatterplotPlugin, qOverload<>(&ScatterplotPlugin::selectionChanged), this, [this, updateModifierUI, updateRadiusUI, updateSelectionButtons]() {
        updateModifierUI();
        updateRadiusUI();
        updateSelectionButtons();
    });

    updateTypeUI();
    updateRadiusUI();
    updateSelectionButtons();

    auto scatterPlotWidget = _scatterplotPlugin->getScatterplotWidget();

    const auto updateEnabled = [this, scatterPlotWidget, &pixelSelectionTool]() {
        setEnabled(scatterPlotWidget->getRenderMode() == ScatterplotWidget::RenderMode::SCATTERPLOT);
    };

    connect(scatterPlotWidget, &ScatterplotWidget::renderModeChanged, this, [this, updateEnabled](const ScatterplotWidget::RenderMode& renderMode) {
        updateEnabled();
    });

    updateEnabled();

    pixelSelectionTool.setChanged();
}
*/