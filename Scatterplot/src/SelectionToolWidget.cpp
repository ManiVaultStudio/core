#include "SelectionToolWidget.h"
#include "ScatterplotPlugin.h"
#include "Application.h"

#include "ui_SelectionToolWidget.h"

#include <QDebug>

SelectionToolWidget::SelectionToolWidget(ScatterplotPlugin* scatterplotPlugin) :
    QWidget(scatterplotPlugin),
    _scatterplotPlugin(scatterplotPlugin),
	_ui{ std::make_unique<Ui::SelectionToolWidget>() }
{
    _ui->setupUi(this);

    auto& fontAwesome = hdps::Application::getIconFont("FontAwesome");

    _ui->modifierAddPushButton->setFont(fontAwesome.getFont(8));
    _ui->modifierAddPushButton->setText(fontAwesome.getIconCharacter("plus"));
    _ui->modifierRemovePushButton->setFont(fontAwesome.getFont(8));
    _ui->modifierRemovePushButton->setText(fontAwesome.getIconCharacter("minus"));
    _ui->radiusSpinBox->setMinimum(PixelSelectionTool::RADIUS_MIN);
    _ui->radiusSpinBox->setMaximum(PixelSelectionTool::RADIUS_MAX);
    _ui->radiusSlider->setMinimum(PixelSelectionTool::RADIUS_MIN * 1000.0f);
    _ui->radiusSlider->setMaximum(PixelSelectionTool::RADIUS_MAX * 1000.0f);

    _ui->typeComboBox->addItems(QStringList(PixelSelectionTool::types.keys()));

    QObject::connect(_ui->typeComboBox, &QComboBox::currentTextChanged, [this](QString currentText) {
        getPixelSelectionTool().setType(PixelSelectionTool::getTypeEnum(currentText));
    });

    QObject::connect(_ui->modifierAddPushButton, &QPushButton::toggled, [this](bool checked) {
        getPixelSelectionTool().setModifier(checked ? PixelSelectionTool::Modifier::Add : PixelSelectionTool::Modifier::Replace);
    });

    QObject::connect(_ui->modifierRemovePushButton, &QPushButton::toggled, [this](bool checked) {
        getPixelSelectionTool().setModifier(checked ? PixelSelectionTool::Modifier::Remove : PixelSelectionTool::Modifier::Replace);
    });

    QObject::connect(_ui->radiusSpinBox, qOverload<double>(&QDoubleSpinBox::valueChanged), [this](double value) {
        getPixelSelectionTool().setRadius(static_cast<float>(value));
    });

    QObject::connect(_ui->radiusSlider, qOverload<int>(&QSlider::valueChanged), [this](int value) {
        getPixelSelectionTool().setRadius(PixelSelectionTool::RADIUS_MIN + ((static_cast<float>(value) - _ui->radiusSlider->minimum()) / 1000.0f));
    });

    QObject::connect(_ui->selectAllPushButton, &QPushButton::clicked, [this]() {
        _scatterplotPlugin->selectAll();
    });

    QObject::connect(_ui->clearSelectionPushButton, &QPushButton::clicked, [this]() {
        _scatterplotPlugin->clearSelection();
    });

    QObject::connect(_ui->invertSelectionPushButton, &QPushButton::clicked, [this]() {
        _scatterplotPlugin->invertSelection();
    });

    QObject::connect(_ui->notifyDuringSelectionCheckBox, &QCheckBox::toggled, [this](bool checked) {
        getPixelSelectionTool().setNotifyDuringSelection(checked);
    });

    const auto updateTypeUI = [this]() {
        const auto canSelect = _scatterplotPlugin->canSelect();

        _ui->typeLabel->setEnabled(canSelect);
        _ui->typeComboBox->setEnabled(canSelect);
    };

    const auto updateModifierUI = [this]() {
        const auto canSelect = _scatterplotPlugin->canSelect();

        switch (getPixelSelectionTool().getModifier())
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

    const auto updateRadiusUI = [this]() {
        auto radiusEnabled = false;

        if (_scatterplotPlugin->canSelect()) {
            switch (getPixelSelectionTool().getType())
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

    QObject::connect(&getPixelSelectionTool(), &PixelSelectionTool::typeChanged, [this, updateRadiusUI](const PixelSelectionTool::Type& type) {
        _ui->typeComboBox->setCurrentText(PixelSelectionTool::getTypeName(type));

        updateRadiusUI();
    });

    QObject::connect(&getPixelSelectionTool(), &PixelSelectionTool::modifierChanged, [this, updateModifierUI](const PixelSelectionTool::Modifier& modifier) {
        updateModifierUI();
    });

    QObject::connect(&getPixelSelectionTool(), &PixelSelectionTool::radiusChanged, [this](const float& radius) {
        _ui->radiusSpinBox->setValue(getPixelSelectionTool().getRadius());
        _ui->radiusSlider->setValue(getPixelSelectionTool().getRadius() * 1000.0f);
    });

    QObject::connect(&getPixelSelectionTool(), &PixelSelectionTool::notifyDuringSelectionChanged, [this](const bool& notifyDuringSelection) {
        _ui->notifyDuringSelectionCheckBox->setChecked(notifyDuringSelection);
    });

    QObject::connect(_scatterplotPlugin, &ScatterplotPlugin::currentDatasetChanged, [this](const QString& currentDataset) {
        _ui->selectionGroupBox->setEnabled(!currentDataset.isEmpty());
    });

    QObject::connect(_scatterplotPlugin, qOverload<>(&ScatterplotPlugin::selectionChanged), [this, updateTypeUI, updateModifierUI, updateRadiusUI]() {
        updateTypeUI();
        updateModifierUI();
        updateRadiusUI();

        _ui->selectAllPushButton->setEnabled(_scatterplotPlugin->canSelectAll());
        _ui->clearSelectionPushButton->setEnabled(_scatterplotPlugin->canClearSelection());
        _ui->invertSelectionPushButton->setEnabled(_scatterplotPlugin->canInvertSelection());
    });
    
    getPixelSelectionTool().setChanged();
}

PixelSelectionTool& SelectionToolWidget::getPixelSelectionTool()
{
    Q_ASSERT(_scatterplotPlugin != nullptr);

    return _scatterplotPlugin->getSelectionTool();
}