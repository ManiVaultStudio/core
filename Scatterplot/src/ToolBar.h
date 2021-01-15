#pragma once

#include "graphics/Vector3f.h"

#include "StateWidget.h"

#include <QWidget>

class ScatterplotPlugin;

namespace Ui {
    class SettingsWidget;
}

class RenderModeWidget;
class PlotSettingsWidget;
class PositionSettingsWidget;
class SubsetSettingsWidget;
class SelectionSettingsWidget;

class SettingsWidget : public QWidget
{
    Q_OBJECT

public:
    SettingsWidget(const ScatterplotPlugin& plugin);
    ~SettingsWidget() override;

    int getXDimension();
    int getYDimension();

    hdps::Vector3f getBaseColor();
    hdps::Vector3f getSelectionColor();

    void initDimOptions(const unsigned int nDim);
    void initDimOptions(const std::vector<QString>& dimNames);
    void initScalarDimOptions(const unsigned int nDim);
    void initScalarDimOptions(const std::vector<QString>& dimNames);

private:
    std::unique_ptr<Ui::SettingsWidget>     _ui;                            /** Externally loaded UI */
    StateWidget<RenderModeWidget>*          _renderModeWidget;              /** TODO */
    PlotSettingsWidget*                     _plotSettinsWidget;             /** TODO */
    StateWidget<PositionSettingsWidget>*    _positionSettingsWidget;        /** TODO */
    StateWidget<SubsetSettingsWidget>*      _subsetSettingsWidget;          /** TODO */
    StateWidget<SelectionSettingsWidget>*   _selectionSettingsWidget;       /** TODO */
    hdps::Vector3f                          _baseColor;                     /** Base color */
    hdps::Vector3f                          _selectionColor;                /** Selection color */

    static const hdps::Vector3f DEFAULT_BASE_COLOR;
    static const hdps::Vector3f DEFAULT_SELECTION_COLOR;
};