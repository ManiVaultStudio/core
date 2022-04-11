#include "ColorMapEditorOneDimensionalAction.h"
#include "ColorMapAction.h"
#include "Application.h"

#include <QGridLayout>

namespace hdps {

namespace gui {

ColorMapEditorOneDimensionalAction::ColorMapEditorOneDimensionalAction(ColorMapAction& colorMapAction) :
    WidgetAction(&colorMapAction),
    _colorMapAction(colorMapAction)
{
    setText("Custom");
    setIcon(Application::getIconFont("FontAwesome").getIcon("chart-line"));
    setCheckable(true);
}

void ColorMapEditorOneDimensionalAction::connectToPublicAction(WidgetAction* publicAction)
{
    auto publicColorMapEditorAction = dynamic_cast<ColorMapEditorOneDimensionalAction*>(publicAction);

    Q_ASSERT(publicColorMapEditorAction != nullptr);

    WidgetAction::connectToPublicAction(publicAction);
}

void ColorMapEditorOneDimensionalAction::disconnectFromPublicAction()
{
    WidgetAction::disconnectFromPublicAction();
}

ColorMapEditorOneDimensionalAction::Widget::Widget(QWidget* parent, ColorMapEditorOneDimensionalAction* colorMapEditorOneDimensionalAction) :
    WidgetActionWidget(parent, colorMapEditorOneDimensionalAction)
{
    setAutoFillBackground(true);
    setFixedWidth(400);

    auto layout = new QGridLayout();

    setPopupLayout(layout);
}

}
}
