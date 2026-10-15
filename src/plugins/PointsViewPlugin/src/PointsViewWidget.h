// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "PointsTableModel.h"

#include <PointDataAccess.h>

#include <actions/StringAction.h>
#include <actions/HorizontalGroupAction.h>
#include <actions/ToggleAction.h>
#include <actions/TriggerAction.h>

#include <QWidget>

#include <cstdint>
#include <functional>
#include <vector>

class QEvent;
template <typename T> class QFutureWatcher;
class QLabel;
class QPlainTextEdit;
class QPushButton;
class QSpinBox;
class QTableView;
class QTimer;

namespace mv
{
class Task;
}

class Points;

/**
 * @brief Interactive table view for inspecting point data and tiled access.
 *
 * The widget is a diagnostic consumer of the storage-neutral PointData access
 * API. It fetches only the visible row and column window after table
 * navigation, exposes tile-cache statistics, and can enable tiled storage on
 * the selected dataset. This makes the plugin useful both as a basic point
 * data browser and as a manual test surface for lazy loading behavior.
 *
 * @author Thomas Kroes
 */
class PointsViewWidget final : public QWidget
{
    Q_OBJECT

public:
    /** Constructs the points view and optionally associates a progress task. */
    explicit PointsViewWidget(QWidget* parent = nullptr, mv::Task* progressTask = nullptr);

signals:
    /** Emitted after the selected dataset has been converted to tiled storage. */
    void tiledStorageEnabled(const QString& datasetName);

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

private slots:
    void refreshSelection();
    void scheduleVisibleRangeFetch();
    void fetchVisibleRange();
    void tileSelectedData();
    void applyFilters();
    void showLiveTable(bool live);

private:
    QSpinBox* createIndexSpinBox(QWidget* parent) const;
    void setStatus(const QString& text);
    void updateUiState();
    void updateInspector(const Points* points);
    static std::vector<std::uint64_t> parseFilter(const QString& filter, std::uint64_t maximum, QString& error);
    void displayQuery(const std::vector<std::uint64_t>& rowIds, const std::vector<std::uint64_t>& columnIds, const std::function<void(const PointDataAccessVisitor&)>& request);

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
    bool _tilingInProgress = false;
    mv::gui::HorizontalGroupAction _filterGroupAction;
    mv::gui::StringAction _rowFilterAction;
    mv::gui::StringAction _columnFilterAction;
    mv::gui::TriggerAction _applyFiltersAction;
    mv::gui::ToggleAction _liveTableAction;
};
