#include "SettingsAction.h"

#include "ScatterplotPlugin.h"

using namespace hdps::gui;

SettingsAction::SettingsAction(ScatterplotPlugin* scatterplotPlugin) :
    PluginAction(scatterplotPlugin, "Settings"),
    _renderModeAction(scatterplotPlugin),
    _plotAction(scatterplotPlugin),
    _positionAction(scatterplotPlugin),
    _coloringAction(scatterplotPlugin),
    _subsetAction(scatterplotPlugin),
    _selectionAction(scatterplotPlugin)
{
    const auto& fontAwesome = Application::getIconFont("FontAwesome");

    _renderModeAction.setIcon(fontAwesome.getIcon("image"));
    _plotAction.setIcon(fontAwesome.getIcon("paint-brush"));
    _positionAction.setIcon(fontAwesome.getIcon("ruler-combined"));
    _coloringAction.setIcon(fontAwesome.getIcon("palette"));
    _subsetAction.setIcon(fontAwesome.getIcon("crop"));
    _selectionAction.setIcon(fontAwesome.getIcon("mouse-pointer"));

    const auto updateEnabled = [this]() {
        setEnabled(!_scatterplotPlugin->getCurrentDataset().isEmpty());
    };

    connect(scatterplotPlugin, &ScatterplotPlugin::currentDatasetChanged, this, [this, updateEnabled](const QString& datasetName) {
        updateEnabled();
    });

    updateEnabled();
}

QMenu* SettingsAction::getContextMenu()
{
    auto menu = new QMenu();

    menu->addMenu(_renderModeAction.getContextMenu());
    menu->addMenu(_plotAction.getContextMenu());
    menu->addSeparator();
    menu->addMenu(_positionAction.getContextMenu());
    menu->addMenu(_coloringAction.getContextMenu());
    menu->addSeparator();
    menu->addMenu(_subsetAction.getContextMenu());
    menu->addMenu(_selectionAction.getContextMenu());

    return menu;
}

SettingsAction::Widget::Widget(QWidget* parent, SettingsAction* settingsAction) :
    WidgetAction::Widget(parent, settingsAction),
    _layout()
{
    setAutoFillBackground(true);

    _layout.setSizeConstraint(QLayout::SetFixedSize);

    _layout.addWidget(new WidgetAction::CompactWidget(this, &settingsAction->_renderModeAction));
    //_layout.addWidget(new Spacer());
    _layout.addWidget(new PlotAction::Widget(this, &settingsAction->_plotAction));
    //_layout.addWidget(new Spacer());
    _layout.addWidget(new PositionAction::Widget(this, &settingsAction->_positionAction));
    //_layout.addWidget(new Spacer());
    _layout.addWidget(new WidgetAction::CompactWidget(this, &settingsAction->_coloringAction));
    //_layout.addWidget(new Spacer());
    _layout.addWidget(new WidgetAction::CompactWidget(this, &settingsAction->_subsetAction));
    //_layout.addWidget(new Spacer());
    _layout.addWidget(new WidgetAction::CompactWidget(this, &settingsAction->_selectionAction));
    _layout.addStretch(1);

    setLayout(&_layout);

    _layout.setMargin(4);
}

SettingsAction::Spacer::Spacer(const Type& type /*= State::Divider*/) :
    QWidget(),
    _type(Type::Divider),
    _layout(new QHBoxLayout()),
    _verticalLine(new QFrame())
{
    _verticalLine->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    _verticalLine->setFrameShape(QFrame::VLine);
    _verticalLine->setFrameShadow(QFrame::Sunken);

    _layout->setMargin(2);
    _layout->setSpacing(0);
    _layout->addWidget(_verticalLine);

    setType(type);
}

/*
ResponsiveToolBar::Spacer::Type ResponsiveToolBar::Spacer::getType(const ResponsiveSectionWidget::State& stateBefore, const ResponsiveSectionWidget::State& stateAfter)
{
    return stateBefore == ResponsiveSectionWidget::State::Collapsed && stateAfter == ResponsiveSectionWidget::State::Collapsed ? Spacer::Type::Spacer : Spacer::Type::Divider;
}

ResponsiveToolBar::Spacer::Type ResponsiveToolBar::Spacer::getType(const ResponsiveSectionWidget* sectionBefore, const ResponsiveSectionWidget* sectionAfter)
{
    return getType(sectionBefore->getState(), sectionAfter->getState());
}
*/

void SettingsAction::Spacer::setType(const Type& type)
{
    _type = type;

    setLayout(_layout);
    setFixedWidth(getWidth(_type));

    _verticalLine->setVisible(_type == Type::Divider ? true : false);
}

std::int32_t SettingsAction::Spacer::getWidth(const Type& type)
{
    switch (type)
    {
        case Type::Divider:
            return 6;

        case Type::Spacer:
            return 6;

        default:
            break;
    }

    return 0;
}