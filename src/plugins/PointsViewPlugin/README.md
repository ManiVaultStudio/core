# Points View Plugin

View plugin for inspecting dense point data through the storage-neutral point-data access API.

## Purpose

Points View is a diagnostic and exploratory view for large point datasets. It displays a table without materializing the complete dataset and uses the point-data facade to retrieve only the requested rows, columns, or ranges.

The plugin is intended to help developers and users verify tiled point-data storage, selective tile loading, and cache behavior with real datasets.

## Usage

1. Select a Points dataset in the data hierarchy.
2. Open **Points View** from the View menu.
3. Browse the visible table. The standalone view follows the current data-hierarchy selection.
4. Use the row and column filters to display a selected subset.
5. Expand **Facade access** to try individual row, column, range, and ID-list queries.

Points View is a regular, multi-instance view plugin. Multiple views can be opened at the same time, and each view can be opened as a floating window from the embedded Points section in Data Properties.

## Data Properties integration

Point datasets expose a **Points** section in the Data Properties plugin. This section contains the compact table view and the facade controls for the owning dataset. It does not follow global data-hierarchy selection changes.

Use **Open in Points View** below the embedded table to open a separate floating Points View instance for the same dataset. The standalone view provides the additional storage and cache controls.

## Point-data access examples

Examples of the access API are implemented in:

- `src/plugins/PointData/src/PointsViewWidget.cpp`
  - `fetchVisibleRange()` demonstrates visible-range table access.
  - `getRow()`, `getRowRange()`, and `getRows()` demonstrate row access.
  - `getColumn()`, `getColumnRange()`, and `getColumns()` demonstrate column access.
  - `getColumnsForRows()` demonstrates columns selected for arbitrary rows.
  - `getRange()` demonstrates rectangular range access.

The reusable table and widget implementation is provided by the PointData plugin in:

- `src/plugins/PointData/src/PointsTableModel.cpp`
- `src/plugins/PointData/src/PointsViewWidget.cpp`

The Points View plugin itself is a thin wrapper around the exported `PointsViewWidget`.
