#include "PixelSelectionAction.h"

#include "Application.h"

#include "util/PixelSelectionTool.h"

#include <QKeyEvent>

PixelSelectionAction::PixelSelectionAction(QWidget* targetWidget, PixelSelectionTool& pixelSelectionTool) :
    GroupAction(targetWidget),
    _targetWidget(targetWidget),
    _pixelSelectionTool(pixelSelectionTool),
    _pixelSelectionTypeAction(*this),
    _pixelSelectionOverlayAction(*this),
    _pixelSelectionModifierAction(*this),
    _pixelSelectionOperationsAction(*this),
    _brushRadiusAction(this, "Brush radius", PixelSelectionTool::BRUSH_RADIUS_MIN, PixelSelectionTool::BRUSH_RADIUS_MAX, PixelSelectionTool::BRUSH_RADIUS_DEFAULT, PixelSelectionTool::BRUSH_RADIUS_DEFAULT),
    _notifyDuringSelectionAction(this, "Notify during selection")
{
    setText("Selection");
    setIcon(hdps::Application::getIconFont("FontAwesome").getIcon("mouse-pointer"));

    _notifyDuringSelectionAction.setShortcutContext(Qt::WidgetWithChildrenShortcut);

    _targetWidget->addAction(&_brushRadiusAction);
    _targetWidget->addAction(&_notifyDuringSelectionAction);

    _notifyDuringSelectionAction.setCheckable(true);
    _notifyDuringSelectionAction.setShortcut(QKeySequence("U"));

    _brushRadiusAction.setToolTip("Brush selection tool radius");
    

    const auto& fontAwesome = hdps::Application::getIconFont("FontAwesome");

    _brushRadiusAction.setSuffix("px");

    connect(&_pixelSelectionTool, &PixelSelectionTool::brushRadiusChanged, this, [this](const float& brushRadius) {
        _brushRadiusAction.setValue(brushRadius);
    });

    connect(&_brushRadiusAction, &DecimalAction::valueChanged, this, [this](const double& value) {
        _pixelSelectionTool.setBrushRadius(value);
    });

    _notifyDuringSelectionAction.setToolTip("Notify during selection or only at the end of the selection process (U)");

    const auto updateNotifyDuringSelection = [this]() -> void {
        _notifyDuringSelectionAction.setChecked(_pixelSelectionTool.isNotifyDuringSelection());
    };

    connect(&_pixelSelectionTool, &PixelSelectionTool::notifyDuringSelectionChanged, this, [this, updateNotifyDuringSelection](const bool& notifyDuringSelection) {
        updateNotifyDuringSelection();
    });

    updateNotifyDuringSelection();

    connect(&_notifyDuringSelectionAction, &QAction::toggled, [this, &pixelSelectionTool](bool toggled) {
        pixelSelectionTool.setNotifyDuringSelection(toggled);
    });

    const auto updateType = [this, &pixelSelectionTool]() {
        _brushRadiusAction.setEnabled(pixelSelectionTool.getType() == PixelSelectionTool::Type::Brush);
    };

    connect(&pixelSelectionTool, &PixelSelectionTool::typeChanged, this, updateType);

    updateType();

    _targetWidget->installEventFilter(this);
}

QWidget* PixelSelectionAction::getTargetWidget()
{
    return _targetWidget;
}

PixelSelectionTool& PixelSelectionAction::getPixelSelectionTool()
{
    return _pixelSelectionTool;
}

QMenu* PixelSelectionAction::getContextMenu()
{
    auto menu = new QMenu("Selection");

    const auto addActionToMenu = [menu](QAction* action) -> void {
        auto actionMenu = new QMenu(action->text());

        actionMenu->addAction(action);

        menu->addMenu(actionMenu);
    };

    menu->addAction(&_pixelSelectionTypeAction.getRectangleAction());
    menu->addAction(&_pixelSelectionTypeAction.getBrushAction());
    menu->addAction(&_pixelSelectionTypeAction.getLassoAction());
    menu->addAction(&_pixelSelectionTypeAction.getPolygonAction());

    menu->addSeparator();

    addActionToMenu(&_brushRadiusAction);

    menu->addSeparator();

    menu->addAction(&_pixelSelectionModifierAction.getModifierAddAction());
    menu->addAction(&_pixelSelectionModifierAction.getModifierRemoveAction());

    menu->addSeparator();

    menu->addAction(&_pixelSelectionOperationsAction.getSelectAllAction());
    menu->addAction(&_pixelSelectionOperationsAction.getClearSelectionAction());
    menu->addAction(&_pixelSelectionOperationsAction.getInvertSelectionAction());

    menu->addSeparator();

    menu->addAction(&_notifyDuringSelectionAction);

    return menu;
}

bool PixelSelectionAction::eventFilter(QObject* object, QEvent* event)
{
    const auto keyEvent = dynamic_cast<QKeyEvent*>(event);

    if (!keyEvent)
        return QObject::eventFilter(object, event);

    if (keyEvent->isAutoRepeat())
        return QObject::eventFilter(object, event);

    switch (keyEvent->type())
    {
        case QEvent::KeyPress:
        {
            switch (keyEvent->key())
            {
                case Qt::Key_Shift:
                    _pixelSelectionModifierAction.getModifierAddAction().setChecked(true);
                    break;

                case Qt::Key_Control:
                    _pixelSelectionModifierAction.getModifierRemoveAction().setChecked(true);
                    break;

                default:
                    break;
            }

            break;
        }

        case QEvent::KeyRelease:
        {
            switch (keyEvent->key())
            {
                case Qt::Key_Shift:
                    _pixelSelectionModifierAction.getModifierAddAction().setChecked(false);
                    break;

                case Qt::Key_Control:
                    _pixelSelectionModifierAction.getModifierRemoveAction().setChecked(false);
                    break;

                default:
                    break;
            }

            break;
        }

        default:
            break;
    }

    return QObject::eventFilter(object, event);
}
