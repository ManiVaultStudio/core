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
    ResponsiveToolBar::Widget("Selection"),
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
    _notifyDuringSelectionCheckBox(new QCheckBox("Notify during selection"))
{
    auto& fontAwesome = hdps::Application::getIconFont("FontAwesome");

    _typeLabel->hide();
    _typeLabel->setToolTip("Selection type");

    _typeLayout->setMargin(LAYOUT_MARGIN);
    _typeLayout->setSpacing(LAYOUT_SPACING);

    _typeLayout->addWidget(_typeComboBox);
    _typeLayout->addWidget(_modifierAddPushButton);
    _typeLayout->addWidget(_modifierRemovePushButton);
    
    _typeWidget->setLayout(_typeLayout);

    _typeComboBox->setToolTip("Choose the selection type");
    _typeComboBox->addItems(QStringList(PixelSelectionTool::types.keys()));
    _typeComboBox->setEditable(true);
    _typeComboBox->lineEdit()->setReadOnly(true);
    _typeComboBox->lineEdit()->setAlignment(Qt::AlignCenter);

    _modifierAddPushButton->setToolTip("Add items to the existing selection");
    _modifierAddPushButton->setIcon(fontAwesome.getIcon("plus"));
    _modifierAddPushButton->setIconSize(QSize(10, 10));
    _modifierAddPushButton->setCheckable(true);
    
    _modifierRemovePushButton->setToolTip("Remove items from the existing selection");
    _modifierRemovePushButton->setIcon(fontAwesome.getIcon("minus", QSize(16, 16)));
    _modifierRemovePushButton->setIconSize(QSize(10, 10));
    _modifierRemovePushButton->setCheckable(true);

    _radiusLabel->setToolTip("Brush radius");

    _radiusLayout->setMargin(LAYOUT_MARGIN);
    _radiusLayout->setSpacing(LAYOUT_SPACING);

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

    _selectLayout->setMargin(LAYOUT_MARGIN);
    _selectLayout->setSpacing(LAYOUT_SPACING);

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
}

void SelectionSettingsWidget::initialize(const ScatterplotPlugin& plugin)
{
    auto& scatterplotPlugin = const_cast<ScatterplotPlugin&>(plugin);
    auto& pixelSelectionTool = scatterplotPlugin.getSelectionTool();

    QObject::connect(_typeComboBox, &QComboBox::currentTextChanged, [this, &pixelSelectionTool](QString currentText) {
        pixelSelectionTool.setType(PixelSelectionTool::getTypeEnum(currentText));
    });

    QObject::connect(_modifierAddPushButton, &QPushButton::toggled, [this, &pixelSelectionTool](bool checked) {
        pixelSelectionTool.setModifier(checked ? PixelSelectionTool::Modifier::Add : PixelSelectionTool::Modifier::Replace);
    });

    QObject::connect(_modifierRemovePushButton, &QPushButton::toggled, [this, &pixelSelectionTool](bool checked) {
        pixelSelectionTool.setModifier(checked ? PixelSelectionTool::Modifier::Remove : PixelSelectionTool::Modifier::Replace);
    });

    QObject::connect(_radiusDoubleSpinBox, qOverload<double>(&QDoubleSpinBox::valueChanged), [this, &pixelSelectionTool](double value) {
        pixelSelectionTool.setRadius(static_cast<float>(value));
    });

    QObject::connect(_radiusSlider, qOverload<int>(&QSlider::valueChanged), [this, &pixelSelectionTool](int value) {
        pixelSelectionTool.setRadius(PixelSelectionTool::RADIUS_MIN + ((static_cast<float>(value) - _radiusSlider->minimum()) / 1000.0f));
    });

    QObject::connect(_selectAllPushButton, &QPushButton::clicked, [this, &scatterplotPlugin]() {
        scatterplotPlugin.selectAll();
    });

    QObject::connect(_clearSelectionPushButton, &QPushButton::clicked, [this, &scatterplotPlugin]() {
        scatterplotPlugin.clearSelection();
    });

    QObject::connect(_invertSelectionPushButton, &QPushButton::clicked, [this, &scatterplotPlugin]() {
        scatterplotPlugin.invertSelection();
    });

    QObject::connect(_notifyDuringSelectionCheckBox, &QCheckBox::toggled, [this, &pixelSelectionTool](bool checked) {
        pixelSelectionTool.setNotifyDuringSelection(checked);
    });
    
    const auto updateTypeUI = [this, &scatterplotPlugin, &pixelSelectionTool]() {
        const auto canSelect = scatterplotPlugin.canSelect();

        _typeLabel->setEnabled(canSelect);
        _typeComboBox->setEnabled(canSelect);
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

    QObject::connect(&pixelSelectionTool, &PixelSelectionTool::typeChanged, [this, updateTypeUI, updateRadiusUI](const PixelSelectionTool::Type& type) {
        updateTypeUI();
        updateRadiusUI();
    });

    QObject::connect(&pixelSelectionTool, &PixelSelectionTool::modifierChanged, [this, updateModifierUI](const PixelSelectionTool::Modifier& modifier) {
        updateModifierUI();
    });

    QObject::connect(&pixelSelectionTool, &PixelSelectionTool::radiusChanged, [this, &pixelSelectionTool](const float& radius) {
        _radiusDoubleSpinBox->setValue(pixelSelectionTool.getRadius());
        _radiusSlider->setValue(pixelSelectionTool.getRadius() * 1000.0f);
    });

    QObject::connect(&pixelSelectionTool, &PixelSelectionTool::notifyDuringSelectionChanged, [this](const bool& notifyDuringSelection) {
        _notifyDuringSelectionCheckBox->setChecked(notifyDuringSelection);
    });

    const auto updateSelectionButtons = [this, &scatterplotPlugin]() {
        _selectAllPushButton->setEnabled(scatterplotPlugin.canSelectAll());
        _clearSelectionPushButton->setEnabled(scatterplotPlugin.canClearSelection());
        _invertSelectionPushButton->setEnabled(scatterplotPlugin.canInvertSelection());
    };

    QObject::connect(&scatterplotPlugin, qOverload<>(&ScatterplotPlugin::selectionChanged), [this, &scatterplotPlugin, updateModifierUI, updateRadiusUI, updateSelectionButtons]() {
        updateModifierUI();
        updateRadiusUI();
        updateSelectionButtons();
    });

    updateTypeUI();
    updateRadiusUI();
    updateSelectionButtons();

    pixelSelectionTool.setChanged();
}

void SelectionSettingsWidget::updateState()
{
    /*
    if (layout()) {
        delete layout();
    }

    const auto applyLayout = [this](QLayout* stateLayout) {
        Q_ASSERT(stateLayout != nullptr);

        stateLayout->setMargin(WidgetStateMixin::LAYOUT_MARGIN);
        stateLayout->setSpacing(WidgetStateMixin::LAYOUT_SPACING);

        setLayout(stateLayout);
    };

    switch (_state)
    {
        case State::Popup:
        {
            auto stateLayout = new QGridLayout();

            applyLayout(stateLayout);

            stateLayout->addWidget(_typeLabel, 0, 0);
            stateLayout->addWidget(_typeWidget, 0, 1);

            stateLayout->addWidget(_radiusLabel, 1, 0);
            stateLayout->addWidget(_radiusWidget, 1, 1);

            stateLayout->addWidget(_selectLabel, 2, 0);
            stateLayout->addWidget(_selectWidget, 2, 1);

            stateLayout->addWidget(_notifyDuringSelectionCheckBox, 3, 1);
            
            break;
        }

        case State::Compact:
        {
            auto stateLayout = new QHBoxLayout();

            applyLayout(stateLayout);

            stateLayout->addWidget(_typeWidget);

            break;
        }

        case State::Full:
        {
            auto stateLayout = new QHBoxLayout();

            applyLayout(stateLayout);

            stateLayout->addWidget(_typeWidget);
            stateLayout->addWidget(_selectWidget);

            break;
        }

        default:
            break;
    }

    _typeLabel->setVisible(_state == WidgetStateMixin::State::Popup);
    _typeWidget->setVisible(true);
    _radiusLabel->setVisible(_state == WidgetStateMixin::State::Popup);
    _radiusWidget->setVisible(_state == WidgetStateMixin::State::Popup);
    _selectLabel->setVisible(_state == WidgetStateMixin::State::Popup);
    _selectWidget->setVisible(_state != WidgetStateMixin::State::Compact);
    _notifyDuringSelectionCheckBox->setVisible(_state == WidgetStateMixin::State::Popup);
    */
}