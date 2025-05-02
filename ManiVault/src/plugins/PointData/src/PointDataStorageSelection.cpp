#include "PointDataStorageSelection.h"

#include <QComboBox.h>
#include <QGridLayout.h>
#include "PointData.h"

void PointDataStorageSelectionWidget::updateDataTypeOptions(const QString &selectedStorageType)
{
    const QMap<QString, QStringList> supportedStorageTypes = PointData::getSupportedStorageTypes();
    QString currentDataType = _dataTypeSelection->currentText();
    _dataTypeSelection->clear();
    _dataTypeSelection->addItems(supportedStorageTypes.value(selectedStorageType));
    _dataTypeSelection->setCurrentText(currentDataType);
}
PointDataStorageSelectionWidget::PointDataStorageSelectionWidget(QWidget* parent)
    : QWidget(parent)
    , _storageSelection(nullptr)
    , _dataTypeSelection(nullptr)
{
    QGridLayout* layout = new QGridLayout(this);
    setLayout(layout);

    _storageSelection = new QComboBox(this);
    _dataTypeSelection = new QComboBox(this);

    

    const QMap<QString, QStringList> supportedStorageTypes = PointData::getSupportedStorageTypes();
    for (auto [key, value] : supportedStorageTypes.asKeyValueRange())
    {
        _storageSelection->addItem(key);
    }
    
    connect(_storageSelection, &QComboBox::currentTextChanged, [=](const QString& selectedStorageType)
        {
            updateDataTypeOptions(selectedStorageType);
        }
    );

    updateDataTypeOptions(supportedStorageTypes.firstKey());

    layout->addWidget(new QLabel("Storage Type:"), 0, 0);
    layout->addWidget(_storageSelection, 0, 1);
    layout->addWidget(new QLabel("Data Type:"), 1, 0);
    layout->addWidget(_dataTypeSelection, 1, 1);
}

PointDataStorageSelectionWidget::~PointDataStorageSelectionWidget()
{ }

PointData::StorageConfiguration PointDataStorageSelectionWidget::selected() const
{
    return PointData::StorageConfiguration(_storageSelection->currentText(), _dataTypeSelection->currentText());;
}