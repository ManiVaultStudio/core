#pragma once

#include "DropWidget.h"
#include "DataType.h"

#include <QWidget>
#include <QLabel>

class QMimeData;

class ScatterplotPlugin;

namespace hdps
{

namespace gui
{

class DropDataWidget : public DropWidget
{
public:
    class DropRegionWidget : public QWidget {

    public:
        DropRegionWidget(QWidget* parent = nullptr);

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
    DropDataWidget(QWidget* parent, ScatterplotPlugin* scatterplotPlugin);

    void initialize(GetDropRegionsFunction getDropRegions);

private:
    void removeAllDropRegionWidgets();

protected:
    ScatterplotPlugin*          _scatterplotPlugin;
    GetDropRegionsFunction      _getDropRegionsFunction;
};

}
}