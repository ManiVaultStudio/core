#include "SubsetSettingsWidget.h"
#include "ScatterplotPlugin.h"

#include <QPushButton>
#include <QCheckBox>

SubsetSettingsWidget::SubsetSettingsWidget(QWidget* parent /*= nullptr*/) :
    QWidget(parent),
    WidgetStateMixin("Subset"),
    _createSubsetPushButton(new QPushButton()),
    _fromSourceCheckBox(new QCheckBox())
{
    _createSubsetPushButton->setToolTip("Create a subset from the selected data points");
    _createSubsetPushButton->setFixedHeight(22);

    _fromSourceCheckBox->setToolTip("Create a subset from source or derived data");
    _fromSourceCheckBox->setText("From source");
}

void SubsetSettingsWidget::initialize(const ScatterplotPlugin& plugin)
{
    QObject::connect(&plugin, qOverload<>(&ScatterplotPlugin::selectionChanged), [this]() {
    });
}

WidgetStateMixin::State SubsetSettingsWidget::getState(const QSize& sourceWidgetSize) const
{
    const auto width = sourceWidgetSize.width();

    auto state = WidgetStateMixin::State::Popup;

    if (width >= 1000 && width < 1500)
        state = WidgetStateMixin::State::Compact;

    if (width >= 1500)
        state = WidgetStateMixin::State::Full;

    return state;
}

void SubsetSettingsWidget::updateState()
{
    if (layout())
        delete layout();

    auto stateLayout = new QHBoxLayout();

    setLayout(stateLayout);

    stateLayout->setMargin(WidgetStateMixin::LAYOUT_MARGIN);
    stateLayout->setSpacing(WidgetStateMixin::LAYOUT_SPACING);

    stateLayout->addWidget(_createSubsetPushButton);
    stateLayout->addWidget(_fromSourceCheckBox);

    switch (_state)
    {
        case State::Popup:
            _createSubsetPushButton->setText("Create");
            _fromSourceCheckBox->setText("From source dataset");
            break;

        case State::Compact:
            _createSubsetPushButton->setText("Subset");
            _fromSourceCheckBox->setText("Source");
            break;

        case State::Full:
            _createSubsetPushButton->setText("Create subset");
            _fromSourceCheckBox->setText("From source");
            break;

        default:
            break;
    }

    //_fromSourceCheckBox->setVisible(_state != WidgetStateMixin::State::Compact);
}