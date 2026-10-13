// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <QWidget>

class QLabel;
class QLineEdit;
class QPushButton;
class QSpinBox;
class QTableWidget;

class PointDataViewWidget final : public QWidget
{
    Q_OBJECT

public:
    explicit PointDataViewWidget(QWidget* parent = nullptr);

private slots:
    void refresh();
    void tileSelectedData();

private:
    void setStatus(const QString& text);
    QSpinBox* createIndexSpinBox(QWidget* parent) const;

    QLabel* _datasetLabel;
    QLabel* _statusLabel;
    QSpinBox* _firstRow;
    QSpinBox* _rowCount;
    QSpinBox* _firstColumn;
    QSpinBox* _columnCount;
    QSpinBox* _rowTileSize;
    QSpinBox* _columnTileSize;
    QSpinBox* _cacheCapacity;
    QPushButton* _refreshButton;
    QPushButton* _tileButton;
    QTableWidget* _table;
};
