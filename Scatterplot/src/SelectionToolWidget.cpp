#include "SelectionToolWidget.h"
#include "ScatterplotPlugin.h"
#include "Application.h"

#include "ui_SelectionToolWidget.h"

#include <QDebug>
#include <QShortcut>

SelectionToolWidget::SelectionToolWidget(QWidget* parent, SelectionTool& selectionTool) :
    QWidget(parent),
    _selectionTool(selectionTool),
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

    QObject::connect(_ui->typeComboBox, &QComboBox::currentTextChanged, [this, &selectionTool](QString currentText) {
        selectionTool.setType(SelectionTool::getTypeEnum(currentText));
    });

    QObject::connect(_ui->modifierAddPushButton, &QPushButton::toggled, [this, &selectionTool](bool checked) {
        selectionTool.setModifier(checked ? SelectionTool::Modifier::Add : SelectionTool::Modifier::Replace);
    });

    QObject::connect(_ui->modifierRemovePushButton, &QPushButton::toggled, [this, &selectionTool](bool checked) {
        selectionTool.setModifier(checked ? SelectionTool::Modifier::Remove : SelectionTool::Modifier::Replace);
    });

    QObject::connect(_ui->radiusSpinBox, qOverload<double>(&QDoubleSpinBox::valueChanged), [this, &selectionTool](double value) {
        selectionTool.setRadius(static_cast<float>(value));
    });

    QObject::connect(_ui->radiusSlider, qOverload<int>(&QSlider::valueChanged), [this, &selectionTool](int value) {
        selectionTool.setRadius(SelectionTool::RADIUS_MIN + ((static_cast<float>(value) - _ui->radiusSlider->minimum()) / 1000.0f));
    });

    QObject::connect(_ui->selectAllPushButton, &QPushButton::clicked, [this, &selectionTool]() {
        selectionTool.selectAll();
    });

    QObject::connect(_ui->clearSelectionPushButton, &QPushButton::clicked, [this, &selectionTool]() {
        selectionTool.clearSelection();
    });

    QObject::connect(_ui->invertSelectionPushButton, &QPushButton::clicked, [this, &selectionTool]() {
        selectionTool.invertSelection();
    });

    const auto updateTypeUI = [this]() {
        const auto canSelect = _selectionTool.canSelect();

        _ui->typeLabel->setEnabled(canSelect);
        _ui->typeComboBox->setEnabled(canSelect);
    };

    const auto updateModifierUI = [this]() {
        const auto canSelect = _selectionTool.canSelect();

        switch (_selectionTool.getModifier())
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

        if (_selectionTool.canSelect()) {
            switch (_selectionTool.getType())
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

    QObject::connect(&_selectionTool, &SelectionTool::typeChanged, [this, updateRadiusUI](const SelectionTool::Type& type) {
        _ui->typeComboBox->setCurrentText(SelectionTool::getTypeName(type));

        updateRadiusUI();
    });

    QObject::connect(&_selectionTool, &SelectionTool::modifierChanged, [this, updateModifierUI](const SelectionTool::Modifier& modifier) {
        updateModifierUI();
    });

    QObject::connect(&_selectionTool, &SelectionTool::radiusChanged, [this](const float& radius) {
        _ui->radiusSpinBox->setValue(_selectionTool.getRadius());
        _ui->radiusSlider->setValue(_selectionTool.getRadius() * 1000.0f);
    });

    /*
    QObject::connect(&_selectionTool, &SelectionTool::selectionChanged, [this, updateTypeUI, updateModifierUI, updateRadiusUI](const std::int32_t& numSelectedPoints, const std::int32_t& numPoints) {
        updateTypeUI();
        updateModifierUI();
        updateRadiusUI();

        _ui->selectAllPushButton->setEnabled(numPoints == -1 ? false : numSelectedPoints != numPoints);
        _ui->clearSelectionPushButton->setEnabled(numPoints == -1 ? false : numSelectedPoints >= 1);
        _ui->invertSelectionPushButton->setEnabled(numPoints >= 0);
    });
    */

    _selectionTool.setChanged();
}