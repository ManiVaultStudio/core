#pragma once

#include "DataType.h"

#include "widgets/DataSlot.h"

#include <QWidget>
#include <QLabel>

class QMimeData;

class ScatterplotPlugin;

class DropDataTypesWidget : public QWidget
{
public:
    class DropRegionWidget;

    class DropRegion {
    public:
        using Dropped = std::function<void(void)>;

    public:
        DropRegion(const QString& title, const bool& dropAllowed = true, const Dropped& dropped = Dropped()) :
            _title(title),
            _dropAllowed(dropAllowed),
            _dropped(dropped)
        {
        }

    protected:
        const QString   _title;
        const bool      _dropAllowed;
        const Dropped   _dropped;

        friend class DropRegionWidget;
    };

    using DropRegions = QList<DropRegion>;

public:
    using GetDropRegions = std::function<DropRegions(const QMimeData*)>;

public:
    class DropRegionWidget : public QWidget {
    public:

    public:
        DropRegionWidget(const DropRegion& dropRegion, QWidget* parent = nullptr);

        bool eventFilter(QObject* target, QEvent* event) override;

        void setActive(const bool& active);
        void activate();
        void deactivate();

        void dragEnterEvent(QDragEnterEvent* dragEnterEvent) override;
        void dragLeaveEvent(QDragLeaveEvent* dragLeaveEvent) override;
        void dropEvent(QDropEvent* dropEvent) override;

    protected:
        void setHighLight(const bool& highlight = false);

    protected:
        DropRegion  _dropRegion;
        QWidget     _labelsWidget;
        QLabel      _iconLabel;
        QLabel      _dataTypeLabel;
    };

public:
    DropDataTypesWidget(QWidget* parent, ScatterplotPlugin* scatterplotPlugin);

    bool eventFilter(QObject* target, QEvent* event) override;

    void initialize(GetDropRegions getDropRegions);

private:
    void removeAllDropRegionWidgets();

protected:
    ScatterplotPlugin*  _scatterplotPlugin;
    GetDropRegions      _getDropRegions;
    bool                _dragging;
};