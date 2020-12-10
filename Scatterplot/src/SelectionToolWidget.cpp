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
    _ui->radiusSpinBox->setMinimum(SelectionTool::RADIUS_MIN);
    _ui->radiusSpinBox->setMaximum(SelectionTool::RADIUS_MAX);
    _ui->radiusSlider->setMinimum(SelectionTool::RADIUS_MIN * 1000.0f);
    _ui->radiusSlider->setMaximum(SelectionTool::RADIUS_MAX * 1000.0f);

    _ui->typeComboBox->addItems(QStringList(SelectionTool::types.keys()));

    QObject::connect(_ui->typeComboBox, &QComboBox::currentTextChanged, [this](QString currentText) {
        getSelectionTool().setType(SelectionTool::getTypeEnum(currentText));
    });

    QObject::connect(_ui->modifierAddPushButton, &QPushButton::toggled, [this](bool checked) {
        getSelectionTool().setModifier(checked ? SelectionTool::Modifier::Add : SelectionTool::Modifier::Replace);
    });

    QObject::connect(_ui->modifierRemovePushButton, &QPushButton::toggled, [this](bool checked) {
        getSelectionTool().setModifier(checked ? SelectionTool::Modifier::Remove : SelectionTool::Modifier::Replace);
    });

    QObject::connect(_ui->radiusSpinBox, qOverload<double>(&QDoubleSpinBox::valueChanged), [this](double value) {
        getSelectionTool().setRadius(static_cast<float>(value));
    });

    QObject::connect(_ui->radiusSlider, qOverload<int>(&QSlider::valueChanged), [this](int value) {
        getSelectionTool().setRadius(SelectionTool::RADIUS_MIN + ((static_cast<float>(value) - _ui->radiusSlider->minimum()) / 1000.0f));
    });

    QObject::connect(_ui->selectAllPushButton, &QPushButton::clicked, [this]() {
        getSelectionTool().selectAll();
    });

    QObject::connect(_ui->clearSelectionPushButton, &QPushButton::clicked, [this]() {
        getSelectionTool().clearSelection();
    });

    QObject::connect(_ui->invertSelectionPushButton, &QPushButton::clicked, [this]() {
        getSelectionTool().invertSelection();
    });

    const auto updateTypeUI = [this]() {
        const auto canSelect = getSelectionTool().canSelect();

        _ui->typeLabel->setEnabled(canSelect);
        _ui->typeComboBox->setEnabled(canSelect);
    };

    const auto updateModifierUI = [this]() {
        const auto canSelect = getSelectionTool().canSelect();

        switch (getSelectionTool().getModifier())
        {
            case SelectionTool::Modifier::Replace:
            {
                _ui->modifierAddPushButton->setChecked(false);
                _ui->modifierRemovePushButton->setChecked(false);
                break;
            }

            case SelectionTool::Modifier::Add:
            {
                _ui->modifierAddPushButton->setChecked(true);
                _ui->modifierRemovePushButton->setChecked(false);
                break;
            }

            case SelectionTool::Modifier::Remove:
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

        if (getSelectionTool().canSelect()) {
            switch (getSelectionTool().getType())
            {
                case SelectionTool::Type::Rectangle:
                case SelectionTool::Type::Circle:
                    break;

                case SelectionTool::Type::Brush:
                {
                    radiusEnabled = true;
                    break;
                }

                case SelectionTool::Type::Lasso:
                case SelectionTool::Type::Polygon:
                    break;

                default:
                    break;
            }
        }

        _ui->radiusLabel->setEnabled(radiusEnabled);
        _ui->radiusSpinBox->setEnabled(radiusEnabled);
        _ui->radiusSlider->setEnabled(radiusEnabled);
    };

    QObject::connect(&getSelectionTool(), &SelectionTool::typeChanged, [this, updateRadiusUI](const SelectionTool::Type& type) {
        _ui->typeComboBox->setCurrentText(SelectionTool::getTypeName(type));

        updateRadiusUI();
    });

    QObject::connect(&getSelectionTool(), &SelectionTool::modifierChanged, [this, updateModifierUI](const SelectionTool::Modifier& modifier) {
        updateModifierUI();
    });

    QObject::connect(&getSelectionTool(), &SelectionTool::radiusChanged, [this](const float& radius) {
        _ui->radiusSpinBox->setValue(getSelectionTool().getRadius());
        _ui->radiusSlider->setValue(getSelectionTool().getRadius() * 1000.0f);
    });

    QObject::connect(_scatterplotPlugin, &ScatterplotPlugin::currentDatasetChanged, [this](const QString& currentDataset) {
        _ui->selectionGroupBox->setEnabled(!currentDataset.isEmpty());
    });

    QObject::connect(_scatterplotPlugin, qOverload<>(&ScatterplotPlugin::selectionChanged), [this, updateTypeUI, updateModifierUI, updateRadiusUI]() {
        updateTypeUI();
        updateModifierUI();
        updateRadiusUI();

        _ui->selectAllPushButton->setEnabled(getSelectionTool().canSelectAll());
        _ui->clearSelectionPushButton->setEnabled(getSelectionTool().canClearSelection());
        _ui->invertSelectionPushButton->setEnabled(getSelectionTool().canInvertSelection());
    });
    
    getSelectionTool().setChanged();
}

SelectionTool& SelectionToolWidget::getSelectionTool()
{
    Q_ASSERT(_scatterplotPlugin != nullptr);

    return _scatterplotPlugin->getSelectionTool();
}