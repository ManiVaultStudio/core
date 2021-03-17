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
        DropRegion(QObject* parent, const QString& title, const bool& isDropAllowed = true, const Dropped& dropped = Dropped());

        QString getTitle() const;
        bool isDropAllowed() const;
        void drop();

    protected:
        const QString   _title;
        const bool      _isDropAllowed;
        const Dropped   _dropped;
    };

    using DropRegions = QList<DropRegion*>;

public:
    using GetDropRegionsFunction = std::function<DropRegions(const QMimeData*)>;

public:
    class DropRegionWidget : public QWidget {
    public:

    public:
        DropRegionWidget(DropRegion* dropRegion, QWidget* parent = nullptr);

        void setActive(const bool& active);
        void activate();
        void deactivate();

        void dragEnterEvent(QDragEnterEvent* dragEnterEvent) override;

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

    void initialize(GetDropRegionsFunction getDropRegions);

private:
    void removeAllDropRegionWidgets();

protected:
    ScatterplotPlugin*          _scatterplotPlugin;
    GetDropRegionsFunction      _getDropRegionsFunction;
};