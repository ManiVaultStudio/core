#pragma once

#include "pointdata_export.h"

#include <qwidget>
#include "PointData.h"

class QComboBox;

class POINTDATA_EXPORT PointDataStorageSelectionWidget : public QWidget
{
    Q_OBJECT

    void updateDataTypeOptions(const QString & selectedStorageType);
public:
    explicit PointDataStorageSelectionWidget(QWidget* parent = nullptr);
    ~PointDataStorageSelectionWidget();

    PointData::StorageConfiguration selected() const;

private:
    QComboBox* _storageSelection;
    QComboBox* _dataTypeSelection;

};
