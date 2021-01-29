#pragma once

#include "graphics/Vector3f.h"

#include <QWidget>

class ScatterplotPlugin;

namespace hdps {
    namespace gui {
        class ResponsiveToolBar;
    }
}

class SettingsWidget : public QWidget
{
    Q_OBJECT
public:
    struct object {
        template <class T>
        object(T t)
            : someFunction([t = std::move(t)]() { return t.someFunction(); })
        { }

        std::function<bool()> someFunction;
    };
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
    hdps::Vector3f                  _baseColor;                     /** Base color */
    hdps::Vector3f                  _selectionColor;                /** Selection color */

    static const hdps::Vector3f DEFAULT_BASE_COLOR;
    static const hdps::Vector3f DEFAULT_SELECTION_COLOR;
};