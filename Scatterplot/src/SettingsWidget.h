#pragma once

#include "graphics/Vector3f.h"

#include <QWidget>

class ScatterplotPlugin;

class ResponsiveToolBar;
class RenderModeWidget;
class PointSettingsWidget;
class DensitySettingsWidget;
class PositionSettingsWidget;
class ColorSettingsWidget;
class SubsetSettingsWidget;
class SelectionSettingsWidget;

namespace hdps {
    namespace gui {
        class ResponsiveToolBar;
    }
}

class SettingsWidget : public QWidget
{
    Q_OBJECT

public:
    SettingsWidget(const ScatterplotPlugin& plugin);

    int getXDimension();
    int getYDimension();

    hdps::Vector3f getBaseColor();
    hdps::Vector3f getSelectionColor();

    void initDimOptions(const unsigned int nDim);
    void initDimOptions(const std::vector<QString>& dimNames);
    void initScalarDimOptions(const unsigned int nDim);
    void initScalarDimOptions(const std::vector<QString>& dimNames);

private:
    hdps::gui::ResponsiveToolBar*   _responsiveToolBar;             /** TODO */
    RenderModeWidget*               _renderModeWidget;              /** TODO */
    PointSettingsWidget*            _pointSettingsWidget;           /** TODO */
    DensitySettingsWidget*          _densitySettingsWidget;         /** TODO */
    PositionSettingsWidget*         _positionSettingsWidget;        /** TODO */
    ColorSettingsWidget*            _colorSettingsWidget;           /** TODO */
    SubsetSettingsWidget*           _subsetSettingsWidget;          /** TODO */
    SelectionSettingsWidget*        _selectionSettingsWidget;       /** TODO */
    hdps::Vector3f                  _baseColor;                     /** Base color */
    hdps::Vector3f                  _selectionColor;                /** Selection color */

    static const hdps::Vector3f DEFAULT_BASE_COLOR;
    static const hdps::Vector3f DEFAULT_SELECTION_COLOR;
};