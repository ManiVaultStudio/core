#include "DimensionPickerWidget.h"
#include "ScatterplotPlugin.h"

#include "ui_DimensionPickerWidget.h"

#include <QStringListModel>

namespace
{
    QStringListModel& createStringListModel(const unsigned int numDimensions, const std::vector<QString>& names, QObject& parent)
    {
        QStringList stringList;

        stringList.reserve(static_cast<int>(numDimensions));

        if (numDimensions == names.size())
        {
            for (const auto& name : names)
            {
                stringList.append(name);
            }
        }
        else
        {
            for (unsigned int i = 0; i < numDimensions; ++i)
            {
                stringList.append(QString::fromLatin1("Dim %1").arg(i));
            }
        }

        auto* stringListModel = new QStringListModel(stringList, &parent);
        return *stringListModel;
    }
}

DimensionPickerWidget::DimensionPickerWidget(const ScatterplotPlugin& plugin) :
    QWidget(),
    _ui{ std::make_unique<Ui::DimensionPickerWidget>() }
{
    _ui->setupUi(this);

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
                _ui->colorSettingsStackedWidget->setCurrentIndex(0);
                break;

            case 1:
                _ui->colorSettingsStackedWidget->setCurrentIndex(1);
                break;
        }
    });

    QObject::connect(_ui->colorDimensionComboBox, qOverload<int>(&QComboBox::currentIndexChanged), [&plugin](int index) {
        const_cast<ScatterplotPlugin&>(plugin).cDimPicked(index);
    });
}

void DimensionPickerWidget::setDimensions(unsigned int numDimensions, const std::vector<QString>& names)
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

void DimensionPickerWidget::setScalarDimensions(unsigned int numDimensions, const std::vector<QString>& names)
{
    auto& stringListModel = createStringListModel(numDimensions, names, *this);

    QSignalBlocker signalBlocker(_ui->colorDimensionComboBox);

    _ui->colorDimensionComboBox->setModel(&stringListModel);
}

int DimensionPickerWidget::getDimensionX()
{
    return _ui->xDimensionComboBox->currentIndex();
}

int DimensionPickerWidget::getDimensionY()
{
    return _ui->yDimensionComboBox->currentIndex();
}