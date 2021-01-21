#include "SelectionSettingsWidget.h"
#include "ScatterplotPlugin.h"
#include "Application.h"

#include <QDebug>
#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QDoubleSpinBox>
#include <QSlider>
#include <QCheckBox>

using namespace hdps::gui;

SelectionSettingsWidget::SelectionSettingsWidget(QWidget* parent /*= nullptr*/) :
    QStackedWidget(parent),
    _widgetState(this),
    _popupPushButton(new PopupPushButton()),
    _widget(new QWidget()),
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
    _advancedSettingsPushButton(new QPushButton(""))
{
    initializeUI();

    connect(&_widgetState, &WidgetState::updateState, [this](const WidgetState::State& state) {
        const auto setWidgetLayout = [this](QLayout* layout) -> void {
            if (_widget->layout())
                delete _widget->layout();

            layout->setMargin(ResponsiveToolBar::LAYOUT_MARGIN);
            layout->setSpacing(ResponsiveToolBar::LAYOUT_SPACING);

            _widget->setLayout(layout);
        };

        switch (state)
        {
            case WidgetState::State::Popup:
            {
                /*
                auto layout = new QGridLayout();

                setWidgetLayout(layout);

                layout->addWidget(_typeLabel, 0, 0);
                layout->addWidget(_typeWidget, 0, 1);
                layout->addWidget(_radiusLabel, 1, 0);
                layout->addWidget(_radiusWidget, 1, 1);
                layout->addWidget(_selectLabel, 2, 0);
                layout->addWidget(_selectWidget, 2, 1);
                layout->addWidget(_notifyDuringSelectionCheckBox, 3, 1);
                */

                setCurrentWidget(_popupPushButton);
                break;
            }

            case WidgetState::State::Compact:
            case WidgetState::State::Full:
            {
                auto layout = new QHBoxLayout();

                setWidgetLayout(layout);

                layout->addWidget(_typeWidget);
                layout->addWidget(_selectWidget);
                layout->addWidget(_notifyDuringSelectionCheckBox);

                _typeLabel->setVisible(state == WidgetState::State::Popup);
                _typeWidget->setVisible(true);
                _radiusLabel->setVisible(state == WidgetState::State::Popup);
                _radiusWidget->setVisible(state == WidgetState::State::Popup);
                _selectLabel->setVisible(state == WidgetState::State::Popup);
                _selectWidget->setVisible(state != WidgetState::State::Compact);
                _notifyDuringSelectionCheckBox->setVisible(state != WidgetState::State::Compact);

                setCurrentWidget(_widget);
                break;
            }

            default:
                break;
        }
    });

    _widgetState.initialize();
}

void SelectionSettingsWidget::initializeUI()
{
    setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);

    auto& fontAwesome = hdps::Application::getIconFont("FontAwesome");

    _popupPushButton->setIcon(fontAwesome.getIcon("mouse-pointer"));

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
    _typeComboBox->setFixedWidth(70);
    //_typeComboBox->setEditable(true);
    //_typeComboBox->lineEdit()->setReadOnly(true);
    //_typeComboBox->lineEdit()->setAlignment(Qt::AlignCenter);

    _modifierAddPushButton->setToolTip("Add items to the existing selection");
    _modifierAddPushButton->setIcon(fontAwesome.getIcon("plus"));
    _modifierAddPushButton->setIconSize(QSize(10, 10));
    _modifierAddPushButton->setCheckable(true);

    _modifierRemovePushButton->setToolTip("Remove items from the existing selection");
    _modifierRemovePushButton->setIcon(fontAwesome.getIcon("minus", QSize(16, 16)));
    _modifierRemovePushButton->setIconSize(QSize(10, 10));
    _modifierRemovePushButton->setCheckable(true);

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
    _clearSelectionPushButton->setFixedWidth(50);

    _selectAllPushButton->setToolTip("Select all items");
    _selectAllPushButton->setFixedWidth(50);

    _invertSelectionPushButton->setToolTip("Invert the selection");
    _invertSelectionPushButton->setFixedWidth(50);

    _notifyDuringSelectionCheckBox->setToolTip("Whether the selection updates are published continuously or at end of the selection process");

    _advancedSettingsPushButton->setToolTip("Advanced selection settings");
    _advancedSettingsPushButton->setIconSize(ResponsiveToolBar::ICON_SIZE);
    _advancedSettingsPushButton->setIcon(fontAwesome.getIcon("ellipsis-h"));

    addWidget(_popupPushButton);
    addWidget(_widget);
}

void SelectionSettingsWidget::setScatterPlotPlugin(const ScatterplotPlugin& plugin)
{
    auto& scatterplotPlugin = const_cast<ScatterplotPlugin&>(plugin);
    auto& pixelSelectionTool = scatterplotPlugin.getSelectionTool();

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

    connect(_selectAllPushButton, &QPushButton::clicked, [this, &scatterplotPlugin]() {
        scatterplotPlugin.selectAll();
    });

    connect(_clearSelectionPushButton, &QPushButton::clicked, [this, &scatterplotPlugin]() {
        scatterplotPlugin.clearSelection();
    });

    connect(_invertSelectionPushButton, &QPushButton::clicked, [this, &scatterplotPlugin]() {
        scatterplotPlugin.invertSelection();
    });

    connect(_notifyDuringSelectionCheckBox, &QCheckBox::toggled, [this, &pixelSelectionTool](bool checked) {
        pixelSelectionTool.setNotifyDuringSelection(checked);
    });
    
    const auto updateTypeUI = [this, &scatterplotPlugin, &pixelSelectionTool]() {
        const auto canSelect = scatterplotPlugin.canSelect();

        _typeLabel->setEnabled(canSelect);
        _typeComboBox->setCurrentText(PixelSelectionTool::getTypeName(pixelSelectionTool.getType()));
    };

    const auto updateModifierUI = [this, &scatterplotPlugin, &pixelSelectionTool]() {
        const auto canSelect = scatterplotPlugin.canSelect();

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

    const auto updateRadiusUI = [this, &scatterplotPlugin, &pixelSelectionTool]() {
        auto radiusEnabled = false;

        if (scatterplotPlugin.canSelect()) {
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

    connect(&pixelSelectionTool, &PixelSelectionTool::typeChanged, [this, updateTypeUI, updateRadiusUI](const PixelSelectionTool::Type& type) {
        updateTypeUI();
        updateRadiusUI();
    });

    connect(&pixelSelectionTool, &PixelSelectionTool::modifierChanged, [this, updateModifierUI](const PixelSelectionTool::Modifier& modifier) {
        updateModifierUI();
    });

    connect(&pixelSelectionTool, &PixelSelectionTool::radiusChanged, [this, &pixelSelectionTool](const float& radius) {
        _radiusDoubleSpinBox->setValue(pixelSelectionTool.getRadius());
        _radiusSlider->setValue(pixelSelectionTool.getRadius() * 1000.0f);
    });

    connect(&pixelSelectionTool, &PixelSelectionTool::notifyDuringSelectionChanged, [this](const bool& notifyDuringSelection) {
        _notifyDuringSelectionCheckBox->setChecked(notifyDuringSelection);
    });

    const auto updateSelectionButtons = [this, &scatterplotPlugin]() {
        _selectAllPushButton->setEnabled(scatterplotPlugin.canSelectAll());
        _clearSelectionPushButton->setEnabled(scatterplotPlugin.canClearSelection());
        _invertSelectionPushButton->setEnabled(scatterplotPlugin.canInvertSelection());
    };

    connect(&scatterplotPlugin, qOverload<>(&ScatterplotPlugin::selectionChanged), [this, &scatterplotPlugin, updateModifierUI, updateRadiusUI, updateSelectionButtons]() {
        updateModifierUI();
        updateRadiusUI();
        updateSelectionButtons();
    });

    updateTypeUI();
    updateRadiusUI();
    updateSelectionButtons();

    auto scatterPlotWidget = const_cast<ScatterplotPlugin&>(plugin).getScatterplotWidget();

    const auto updateEnabled = [this, scatterPlotWidget, &pixelSelectionTool]() {
        setEnabled(scatterPlotWidget->getRenderMode() == ScatterplotWidget::RenderMode::SCATTERPLOT);
    };

    connect(scatterPlotWidget, &ScatterplotWidget::renderModeChanged, [this, updateEnabled](const ScatterplotWidget::RenderMode& renderMode) {
        updateEnabled();
    });

    updateEnabled();

    pixelSelectionTool.setChanged();
}