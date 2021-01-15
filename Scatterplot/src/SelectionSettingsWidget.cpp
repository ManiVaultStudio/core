#include "SelectionSettingsWidget.h"
#include "ScatterplotPlugin.h"
#include "Application.h"

#include <QDebug>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QDoubleSpinBox>
#include <QSlider>
#include <QCheckBox>

SelectionSettingsWidget::SelectionSettingsWidget(QWidget* parent /*= nullptr*/) :
    QWidget(parent),
    WidgetStateMixin("Selection"),
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

    _typeLayout->setMargin(0);
    _typeLayout->addWidget(_typeComboBox);
    _typeLayout->addWidget(_modifierAddPushButton);
    _typeLayout->addWidget(_modifierRemovePushButton);
    
    _typeWidget->setLayout(_typeLayout);

    _typeComboBox->setToolTip("Choose the selection type");
    _typeComboBox->setFixedHeight(20);
    _typeComboBox->addItems(QStringList(PixelSelectionTool::types.keys()));

    _modifierAddPushButton->setToolTip("Add items to the existing selection");
    _modifierAddPushButton->setFixedSize(22, 22);
    _modifierAddPushButton->setIcon(fontAwesome.getIcon("plus"));
    _modifierAddPushButton->setIconSize(QSize(10, 10));
    
    _modifierRemovePushButton->setToolTip("Remove items from the existing selection");
    _modifierRemovePushButton->setFixedSize(22, 22);
    _modifierRemovePushButton->setIcon(fontAwesome.getIcon("minus", QSize(16, 16)));
    _modifierRemovePushButton->setIconSize(QSize(10, 10));

    _radiusLabel->setToolTip("Brush radius");

    _radiusLayout->setMargin(0);
    _radiusLayout->addWidget(_radiusDoubleSpinBox);
    _radiusLayout->addWidget(_radiusSlider);

    _radiusWidget->setLayout(_radiusLayout);

    _radiusDoubleSpinBox->setToolTip("Brush radius");
    _radiusDoubleSpinBox->setDecimals(1);
    _radiusDoubleSpinBox->setSuffix("px");
    _radiusDoubleSpinBox->setMinimum(0.0);
    _radiusDoubleSpinBox->setMaximum(100.0);
    _radiusDoubleSpinBox->setSingleStep(5.0);

    _radiusSlider->setToolTip("Brush radius");
    _radiusSlider->setOrientation(Qt::Horizontal);
    _radiusSlider->setMinimum(0);
    _radiusSlider->setMaximum(100);
    _radiusSlider->setSingleStep(5);

    _selectLabel->setToolTip("Select");

    _selectLayout->setMargin(0);
    _selectLayout->addWidget(_clearSelectionPushButton);
    _selectLayout->addWidget(_selectAllPushButton);
    _selectLayout->addWidget(_invertSelectionPushButton);
    _selectLayout->addStretch(1);

    _selectWidget->setLayout(_selectLayout);

    _clearSelectionPushButton->setToolTip("Removes all items from the selection");
    _clearSelectionPushButton->setFixedHeight(22);

    _selectAllPushButton->setToolTip("Select all items");
    _selectAllPushButton->setFixedHeight(22);

    _invertSelectionPushButton->setToolTip("Invert the selection");
    _invertSelectionPushButton->setFixedHeight(22);

    _notifyDuringSelectionCheckBox->setToolTip("Whether the selection updates are published continuously or at end of the selection process");
}

void SelectionSettingsWidget::initialize(const ScatterplotPlugin& plugin)
{
    /*
     auto& fontAwesome = hdps::Application::getIconFont("FontAwesome");

    _ui->typeComboBox->addItems(QStringList(PixelSelectionTool::types.keys()));

    auto& scatterplotPlugin = const_cast<ScatterplotPlugin&>(plugin);
    auto& pixelSelectionTool = scatterplotPlugin.getSelectionTool();

    QObject::connect(_ui->typeComboBox, &QComboBox::currentTextChanged, [this, &pixelSelectionTool](QString currentText) {
        pixelSelectionTool.setType(PixelSelectionTool::getTypeEnum(currentText));
    });

    const auto updateTypeUI = [this, &scatterplotPlugin, &pixelSelectionTool]() {
        const auto canSelect = scatterplotPlugin.canSelect();

        _ui->typeLabel->setEnabled(canSelect);
        _ui->typeComboBox->setEnabled(canSelect);
        _ui->typeComboBox->setCurrentText(PixelSelectionTool::getTypeName(pixelSelectionTool.getType()));
    };

    _ui->modifierAddPushButton->setFont(fontAwesome.getFont(8));
    _ui->modifierAddPushButton->setText(fontAwesome.getIconCharacter("plus"));
    _ui->modifierRemovePushButton->setFont(fontAwesome.getFont(8));
    _ui->modifierRemovePushButton->setText(fontAwesome.getIconCharacter("minus"));
    _ui->radiusSpinBox->setMinimum(PixelSelectionTool::RADIUS_MIN);
    _ui->radiusSpinBox->setMaximum(PixelSelectionTool::RADIUS_MAX);
    _ui->radiusSlider->setMinimum(PixelSelectionTool::RADIUS_MIN * 1000.0f);
    _ui->radiusSlider->setMaximum(PixelSelectionTool::RADIUS_MAX * 1000.0f);

    QObject::connect(_ui->modifierAddPushButton, &QPushButton::toggled, [this, &pixelSelectionTool](bool checked) {
        pixelSelectionTool.setModifier(checked ? PixelSelectionTool::Modifier::Add : PixelSelectionTool::Modifier::Replace);
    });

    QObject::connect(_ui->modifierRemovePushButton, &QPushButton::toggled, [this, &pixelSelectionTool](bool checked) {
        pixelSelectionTool.setModifier(checked ? PixelSelectionTool::Modifier::Remove : PixelSelectionTool::Modifier::Replace);
    });

    QObject::connect(_ui->radiusSpinBox, qOverload<double>(&QDoubleSpinBox::valueChanged), [this, &pixelSelectionTool](double value) {
        pixelSelectionTool.setRadius(static_cast<float>(value));
    });

    QObject::connect(_ui->radiusSlider, qOverload<int>(&QSlider::valueChanged), [this, &pixelSelectionTool](int value) {
        pixelSelectionTool.setRadius(PixelSelectionTool::RADIUS_MIN + ((static_cast<float>(value) - _ui->radiusSlider->minimum()) / 1000.0f));
    });

    QObject::connect(_ui->selectAllPushButton, &QPushButton::clicked, [this, &scatterplotPlugin]() {
        scatterplotPlugin.selectAll();
    });

    QObject::connect(_ui->clearSelectionPushButton, &QPushButton::clicked, [this, &scatterplotPlugin]() {
        scatterplotPlugin.clearSelection();
    });

    QObject::connect(_ui->invertSelectionPushButton, &QPushButton::clicked, [this, &scatterplotPlugin]() {
        scatterplotPlugin.invertSelection();
    });

    QObject::connect(_ui->notifyDuringSelectionCheckBox, &QCheckBox::toggled, [this, &pixelSelectionTool](bool checked) {
        pixelSelectionTool.setNotifyDuringSelection(checked);
    });

    const auto updateModifierUI = [this, &scatterplotPlugin, &pixelSelectionTool]() {
        const auto canSelect = scatterplotPlugin.canSelect();

        switch (pixelSelectionTool.getModifier())
        {
            case PixelSelectionTool::Modifier::Replace:
            {
                _ui->modifierAddPushButton->setChecked(false);
                _ui->modifierRemovePushButton->setChecked(false);
                break;
            }

            case PixelSelectionTool::Modifier::Add:
            {
                _ui->modifierAddPushButton->setChecked(true);
                _ui->modifierRemovePushButton->setChecked(false);
                break;
            }

            case PixelSelectionTool::Modifier::Remove:
            {
                _ui->modifierAddPushButton->setChecked(false);
                _ui->modifierRemovePushButton->setChecked(true);
                break;
            }

            default:
                break;
        }

        _ui->modifierAddPushButton->setEnabled(canSelect);
        _ui->modifierRemovePushButton->setEnabled(canSelect);
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

        _ui->radiusLabel->setEnabled(radiusEnabled);
        _ui->radiusSpinBox->setEnabled(radiusEnabled);
        _ui->radiusSlider->setEnabled(radiusEnabled);
    };

    QObject::connect(&pixelSelectionTool, &PixelSelectionTool::typeChanged, [this, updateRadiusUI](const PixelSelectionTool::Type& type) {
        updateRadiusUI();
    });

    QObject::connect(&pixelSelectionTool, &PixelSelectionTool::modifierChanged, [this, updateModifierUI](const PixelSelectionTool::Modifier& modifier) {
        updateModifierUI();
    });

    QObject::connect(&pixelSelectionTool, &PixelSelectionTool::radiusChanged, [this, &pixelSelectionTool](const float& radius) {
        _ui->radiusSpinBox->setValue(pixelSelectionTool.getRadius());
        _ui->radiusSlider->setValue(pixelSelectionTool.getRadius() * 1000.0f);
    });

    QObject::connect(&pixelSelectionTool, &PixelSelectionTool::notifyDuringSelectionChanged, [this](const bool& notifyDuringSelection) {
        _ui->notifyDuringSelectionCheckBox->setChecked(notifyDuringSelection);
    });

    const auto updateSelectionButtons = [this, &scatterplotPlugin]() {
        _ui->selectAllPushButton->setEnabled(scatterplotPlugin.canSelectAll());
        _ui->clearSelectionPushButton->setEnabled(scatterplotPlugin.canClearSelection());
        _ui->invertSelectionPushButton->setEnabled(scatterplotPlugin.canInvertSelection());
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
    */
}

WidgetStateMixin::State SelectionSettingsWidget::getState(const QSize& sourceWidgetSize) const
{
    const auto width = sourceWidgetSize.width();

    auto state = WidgetStateMixin::State::Popup;

    if (width >= 1000 && width < 1500)
        state = WidgetStateMixin::State::Compact;

    if (width >= 1500)
        state = WidgetStateMixin::State::Full;

    return state;
}

void SelectionSettingsWidget::updateState()
{
    if (layout()) {
        delete layout();
    }

    const auto applyLayout = [this](QLayout* stateLayout) {
        Q_ASSERT(stateLayout != nullptr);

        stateLayout->setMargin(0);

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
}