#include "AdvancedSelectionSettingsWidget.h"
#include "ScatterplotPlugin.h"
#include "Application.h"

#include "ui_AdvancedSelectionSettingsWidget.h"

#include <QDebug>
#include <QDoubleSpinBox>
#include <QCheckBox>

AdvancedSelectionSettingsWidget::AdvancedSelectionSettingsWidget(QWidget* parent /*= nullptr*/) :
    QWidget(parent),
	_ui{ std::make_unique<Ui::AdvancedSelectionSettingsWidget>() }
{
    _ui->setupUi(this);
}

void AdvancedSelectionSettingsWidget::initialize(const ScatterplotPlugin& plugin)
{
    auto& fontAwesome = hdps::Application::getIconFont("FontAwesome");

    _ui->modifierAddPushButton->setFont(fontAwesome.getFont(6));
    _ui->modifierAddPushButton->setText(fontAwesome.getIconCharacter("plus"));
    _ui->modifierRemovePushButton->setFont(fontAwesome.getFont(6));
    _ui->modifierRemovePushButton->setText(fontAwesome.getIconCharacter("minus"));
    _ui->radiusSpinBox->setMinimum(PixelSelectionTool::RADIUS_MIN);
    _ui->radiusSpinBox->setMaximum(PixelSelectionTool::RADIUS_MAX);
    _ui->radiusSlider->setMinimum(PixelSelectionTool::RADIUS_MIN * 1000.0f);
    _ui->radiusSlider->setMaximum(PixelSelectionTool::RADIUS_MAX * 1000.0f);

    auto& scatterplotPlugin = const_cast<ScatterplotPlugin&>(plugin);
    auto& pixelSelectionTool = scatterplotPlugin.getSelectionTool();

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

    QObject::connect(&scatterplotPlugin, qOverload<>(&ScatterplotPlugin::selectionChanged), [this, &scatterplotPlugin, updateModifierUI, updateRadiusUI]() {
        updateModifierUI();
        updateRadiusUI();

        _ui->selectAllPushButton->setEnabled(scatterplotPlugin.canSelectAll());
        _ui->clearSelectionPushButton->setEnabled(scatterplotPlugin.canClearSelection());
        _ui->invertSelectionPushButton->setEnabled(scatterplotPlugin.canInvertSelection());
    });
    
    pixelSelectionTool.setChanged();
}