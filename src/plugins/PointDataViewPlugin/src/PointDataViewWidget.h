// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "PointDataTableModel.h"

#include <QWidget>

class QEvent;
template <typename T> class QFutureWatcher;
class QLabel;
class QPushButton;
class QSpinBox;
class QTableView;

namespace mv { class Task; }

class PointDataViewWidget final : public QWidget
{
    Q_OBJECT

public:
    explicit PointDataViewWidget(QWidget* parent = nullptr, mv::Task* progressTask = nullptr);

signals:
    void tiledStorageEnabled(const QString& datasetName);

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

private slots:
    void refreshSelection();
    void fetchVisibleRange();
    void tileSelectedData();

private:
    QSpinBox* createIndexSpinBox(QWidget* parent) const;
    void setStatus(const QString& text);

    QLabel* _datasetLabel;
    QLabel* _statusLabel;
    QSpinBox* _rowTileSize;
    QSpinBox* _columnTileSize;
    QSpinBox* _cacheCapacity;
    QPushButton* _tileButton;
    QTableView* _table;
    PointDataTableModel* _model;
    mv::Task* _progressTask;
    QFutureWatcher<void>* _tileWatcher;
};
