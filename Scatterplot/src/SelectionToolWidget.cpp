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

    _ui->selectionTypeComboBox->addItems(QStringList(SelectionTool::types.keys()));

    QObject::connect(_ui->selectionTypeComboBox, &QComboBox::currentTextChanged, [this, &selectionTool](QString currentText) {
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

    QObject::connect(&_selectionTool, &SelectionTool::typeChanged, [this](const SelectionTool::Type& type) {
        _ui->selectionTypeComboBox->setCurrentText(SelectionTool::getTypeName(type));

        auto radiusEnabled = false;

        switch (type)
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

        _ui->radiusLabel->setEnabled(radiusEnabled);
        _ui->radiusSpinBox->setEnabled(radiusEnabled);
        _ui->radiusSlider->setEnabled(radiusEnabled);
    });

    QObject::connect(&_selectionTool, &SelectionTool::modifierChanged, [this](const SelectionTool::Modifier& modifier) {
        switch (modifier)
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
        _ui->selectionTypeComboBox->setCurrentText(SelectionTool::getModifierName(modifier));
    });

    QObject::connect(&_selectionTool, &SelectionTool::radiusChanged, [this, &selectionTool](const float& radius) {
        _ui->radiusSpinBox->setValue(selectionTool.getRadius());
        _ui->radiusSlider->setValue(selectionTool.getRadius() * 1000.0f);
    });

    QObject::connect(&_selectionTool, &SelectionTool::selectionChanged, [this, &selectionTool](const std::int32_t& selectionSize, const std::int32_t& numPoints) {
        _ui->selectAllPushButton->setEnabled(numPoints == -1 ? false : selectionSize != numPoints);
        _ui->clearSelectionPushButton->setEnabled(numPoints == -1 ? false : selectionSize >= 1);
        _ui->invertSelectionPushButton->setEnabled(numPoints >= 0);
    });

    _selectionTool.setChanged();
}