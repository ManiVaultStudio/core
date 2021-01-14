#include "SelectionSettingsWidget.h"
#include "SelectionSettingsPopupWidget.h"
#include "ScatterplotPlugin.h"
#include "Application.h"

#include "ui_SelectionSettingsWidget.h"

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
    _typeComboBox(new QComboBox()),
    _modifierAddPushButton(new QPushButton()),
    _modifierRemovePushButton(new QPushButton()),
    _radiusLabel(new QLabel("Radius:")),
    _radiusDoubleSpinBox(new QDoubleSpinBox()),
    _radiusSlider(new QSlider()),
    _selectLabel(new QLabel("Select:")),
    _clearSelectionPushButton(new QPushButton("None")),
    _selectAllPushButton(new QPushButton("All")),
    _invertSelectionPushButton(new QPushButton("Invert")),
    _notifyDuringSelectionCheckBox(new QCheckBox("Notify during selection"))
{
    auto& fontAwesome = hdps::Application::getIconFont("FontAwesome");

    _typeComboBox->addItems(QStringList(PixelSelectionTool::types.keys()));
}

void SelectionSettingsWidget::initialize(const ScatterplotPlugin& plugin)
{
    /*
    //resize(QSize(10, 10));

    setFixedHeight(32);
    setFixedWidth(32);

    auto& fontAwesome = hdps::Application::getIconFont("FontAwesome");

    _ui->popupPushButton->setStyleSheet("text-align: center");
    _ui->popupPushButton->setFont(fontAwesome.getFont(8));
    _ui->popupPushButton->setText(fontAwesome.getIconCharacter("mouse-pointer"));

    const auto showSelectionSettingsPopupWidget = [this, &plugin](QWidget* parent) {
        auto windowLevelWidget = new SelectionSettingsPopupWidget(parent);

        windowLevelWidget->initialize(plugin);
        windowLevelWidget->show();
    };

    QObject::connect(_ui->popupPushButton, &QPushButton::clicked, [this, &plugin, showSelectionSettingsPopupWidget]() {
        showSelectionSettingsPopupWidget(_ui->popupPushButton);
    });
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

            /*
            stateLayout->addWidget(_sizeLabel, 0, 0);
            stateLayout->addWidget(_sizeDoubleSpinBox, 0, 1);
            stateLayout->addWidget(_sizeSlider, 0, 2);

            stateLayout->addWidget(_opacityLabel, 1, 0);
            stateLayout->addWidget(_opacityDoubleSpinBox, 1, 1);
            stateLayout->addWidget(_opacitySlider, 1, 2);
            */

            break;
        }

        case State::Compact:
        {
            auto stateLayout = new QHBoxLayout();

            applyLayout(stateLayout);

            stateLayout->addWidget(_typeComboBox);
            stateLayout->addWidget(_modifierAddPushButton);
            stateLayout->addWidget(_modifierRemovePushButton);

            /*
            stateLayout->addWidget(_sizeLabel);
            stateLayout->addWidget(_sizeDoubleSpinBox);
            stateLayout->addWidget(_sizeSlider);

            stateLayout->addWidget(_opacityLabel);
            stateLayout->addWidget(_opacityDoubleSpinBox);
            stateLayout->addWidget(_opacitySlider);
            */

            break;
        }

        case State::Full:
        {
            auto stateLayout = new QHBoxLayout();

            applyLayout(stateLayout);

            /*
            stateLayout->addWidget(_sizeLabel);
            stateLayout->addWidget(_sizeDoubleSpinBox);
            stateLayout->addWidget(_sizeSlider);

            stateLayout->addWidget(_opacityLabel);
            stateLayout->addWidget(_opacityDoubleSpinBox);
            stateLayout->addWidget(_opacitySlider);
            */

            break;
        }

        default:
            break;
    }

    /*
    _sizeDoubleSpinBox->setVisible(_state != WidgetStateMixin::State::Compact);
    _opacityDoubleSpinBox->setVisible(_state != WidgetStateMixin::State::Compact);

    _sizeSlider->setFixedWidth(_state == WidgetStateMixin::State::Popup ? 100 : 40);
    _opacitySlider->setFixedWidth(_state == WidgetStateMixin::State::Popup ? 100 : 40);

    _sizeLabel->setText(sizeLabelText);
    _opacityLabel->setText(opacityLabelText);
    */
}