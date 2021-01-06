#include "DimensionSettingsWidget.h"
#include "ScatterplotPlugin.h"

#include "ui_DimensionSettingsWidget.h"

DimensionSettingsWidget::DimensionSettingsWidget(QWidget* parent /*= nullptr*/) :
    QWidget(),
    _ui{ std::make_unique<Ui::DimensionSettingsWidget>() }
{
    _ui->setupUi(this);
}

void DimensionSettingsWidget::initialize(const ScatterplotPlugin& plugin)
{
    QObject::connect(_ui->xDimensionComboBox, qOverload<int>(&QComboBox::currentIndexChanged), [&plugin](int index) {
        const_cast<ScatterplotPlugin&>(plugin).xDimPicked(index);
    });

    QObject::connect(_ui->yDimensionComboBox, qOverload<int>(&QComboBox::currentIndexChanged), [&plugin](int index) {
        const_cast<ScatterplotPlugin&>(plugin).yDimPicked(index);
    });

    QObject::connect(_ui->colorOptionsComboBox, qOverload<int>(&QComboBox::currentIndexChanged), [this](int index) {
        switch (index)
        {
            case 0:
                _ui->colorDimensionStackedWidget->setCurrentIndex(0);
                break;

            case 1:
                _ui->colorDimensionStackedWidget->setCurrentIndex(1);
                break;
        }
    });

    _ui->colorDimensionStackedWidget->initialize(plugin);

    const_cast<ScatterplotPlugin&>(plugin).installEventFilter(this);
}

bool DimensionSettingsWidget::eventFilter(QObject* target, QEvent* event)
{
    if (event->type() != QEvent::Resize)
        return QWidget::eventFilter(target, event);

    const auto sourceWidgetWidth = static_cast<QResizeEvent*>(event)->size().width();

    const auto positionCompact = sourceWidgetWidth <= 800;

    _ui->positionPopupPushButton->setVisible(positionCompact);
    _ui->xDimensionLabel->setVisible(!positionCompact);
    _ui->xDimensionComboBox->setVisible(!positionCompact);
    _ui->yDimensionLabel->setVisible(!positionCompact);
    _ui->yDimensionComboBox->setVisible(!positionCompact);

    const auto colorCompact = sourceWidgetWidth <= 1000;

    _ui->colorPopupPushButton->setVisible(colorCompact);
    _ui->colorOptionsComboBox->setVisible(!colorCompact);
    _ui->colorDimensionStackedWidget->setVisible(!colorCompact);

    return QWidget::eventFilter(target, event);
}

int DimensionSettingsWidget::getDimensionX()
{
    return _ui->xDimensionComboBox->currentIndex();
}

int DimensionSettingsWidget::getDimensionY()
{
    return _ui->yDimensionComboBox->currentIndex();
}

void DimensionSettingsWidget::setDimensions(unsigned int numDimensions, const std::vector<QString>& names /*= std::vector<QString>()*/)
{
    auto& stringListModel = createStringListModel(numDimensions, names, *this);

    QSignalBlocker xDimensionComboBoxSignalBlocker(_ui->xDimensionComboBox), yDimensionComboBoxSignalBlocker(_ui->yDimensionComboBox);

    _ui->xDimensionComboBox->setModel(&stringListModel);
    _ui->yDimensionComboBox->setModel(&stringListModel);

    if (numDimensions >= 2) {
        _ui->xDimensionComboBox->setCurrentIndex(0);
        _ui->yDimensionComboBox->setCurrentIndex(1);
    }
}

void DimensionSettingsWidget::setScalarDimensions(unsigned int numDimensions, const std::vector<QString>& names /*= std::vector<QString>()*/)
{
    auto& stringListModel = createStringListModel(numDimensions, names, *this);

    auto colorByDimensionComboBox = _ui->colorDimensionStackedWidget->getColorByDimensionComboBox();

    QSignalBlocker signalBlocker(colorByDimensionComboBox);

    colorByDimensionComboBox->setModel(&stringListModel);
}