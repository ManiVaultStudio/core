#pragma once

#include "DataType.h"

#include "widgets/DataSlot.h"

#include <QWidget>
#include <QLabel>

class ScatterplotPlugin;

class DropDataTypesWidget : public QWidget
{
public:
    class DropRegionWidget : public QWidget {
    public:
        DropRegionWidget(const QString& title, QWidget* parent);

        bool eventFilter(QObject* target, QEvent* event) override;

        void setActive(const bool& active);
        void activate();
        void deactivate();

    protected:
        void setHighLight(const bool& highlight = false);

    protected:
        QWidget     _labelsWidget;
        QLabel      _iconLabel;
        QLabel      _dataTypeLabel;
    };

public:
    DropDataTypesWidget(QWidget* parent, ScatterplotPlugin* scatterplotPlugin);

    bool eventFilter(QObject* target, QEvent* event) override;

protected:
    ScatterplotPlugin*                  _scatterplotPlugin;
    QMap<QString, DropRegionWidget*>    _dropRegionWidgets;
};