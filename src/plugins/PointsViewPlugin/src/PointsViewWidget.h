// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "PointsTableModel.h"

#include <QWidget>

class QEvent;
template <typename T> class QFutureWatcher;
class QLabel;
class QPlainTextEdit;
class QPushButton;
class QSpinBox;
class QTableView;
class QTimer;

namespace mv { class Task; }
class PointData;

class PointsViewWidget final : public QWidget
{
    Q_OBJECT

public:
    explicit PointsViewWidget(QWidget* parent = nullptr, mv::Task* progressTask = nullptr);

signals:
    void tiledStorageEnabled(const QString& datasetName);

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

private slots:
    void refreshSelection();
    void scheduleVisibleRangeFetch();
    void fetchVisibleRange();
    void tileSelectedData();

private:
    QSpinBox* createIndexSpinBox(QWidget* parent) const;
    void setStatus(const QString& text);
    void updateInspector(const PointData* pointData);

    QLabel* _datasetLabel;
    QLabel* _inspectorLabel;
    QLabel* _statusLabel;
    QSpinBox* _rowTileSize;
    QSpinBox* _columnTileSize;
    QSpinBox* _cacheCapacity;
    QPushButton* _tileButton;
    QTableView* _table;
    QPlainTextEdit* _accessLog;
    PointsTableModel* _model;
    mv::Task* _progressTask;
    QFutureWatcher<void>* _tileWatcher;
    QTimer* _fetchTimer;
};
