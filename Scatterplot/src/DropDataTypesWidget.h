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

    class DropRegion : public QObject {
    public:
        using Dropped = std::function<void(void)>;

    public:
        DropRegion(QObject* parent, const QString& title, const bool& dropAllowed = true, const Dropped& dropped = Dropped());

        void drop();

    protected:
        const QString   _title;
        const bool      _dropAllowed;
        const Dropped   _dropped;

        friend class DropRegionWidget;
    };

    using DropRegions = QList<DropRegion*>;

public:
    using GetDropRegions = std::function<DropRegions(const QMimeData*)>;

public:
    class DropRegionWidget : public QWidget {
    public:

    public:
        DropRegionWidget(DropRegion* dropRegion, QWidget* parent = nullptr);

        void setActive(const bool& active);
        void activate();
        void deactivate();

        void dragEnterEvent(QDragEnterEvent* dragEnterEvent) override;
        void dragLeaveEvent(QDragLeaveEvent* dragLeaveEvent) override;

        DropRegion* getDropRegion();

    protected:
        void setHighLight(const bool& highlight = false);

    protected:
        DropRegion* _dropRegion;
        QWidget     _labelsWidget;
        QLabel      _iconLabel;
        QLabel      _dataTypeLabel;

        friend class DropDataTypesWidget;
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