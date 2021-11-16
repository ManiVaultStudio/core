#include "PointsDimensionPickerAction.h"
#include "Application.h"

#include <QHBoxLayout>

using namespace hdps;

PointsDimensionPickerAction::PointsDimensionPickerAction(QObject* parent) :
    WidgetAction(parent),
    _points(nullptr),
    _dimensionSelectAction(this, "Select dimension"),
    _dimensionSearchAction(this, "Search dimension")
{
    setText("Points dimension picker");
    setIcon(Application::getIconFont("FontAwesome").getIcon("adjust"));

    _dimensionSelectAction.setToolTip("Pick a dimension by selecting");
    _dimensionSearchAction.setToolTip("Pick a dimension by searching");

    _dimensionSearchAction.setIcon(Application::getIconFont("FontAwesome").getIcon("search"));
}

void PointsDimensionPickerAction::setPointsDataset(const DatasetRef<Points>& points)
{
    _points = points;

    // Dimension names options for the select action
    QStringList options;

    if (_points->getNumDimensions() == _points->getDimensionNames().size()) {

        // Get dimension name from points data
        for (const auto& dimensionName : _points->getDimensionNames())
            options << dimensionName;
    }
    else {

        // Create dimension names
        for (int dimensionIndex = 0; dimensionIndex < _points->getNumDimensions(); dimensionIndex++)
            options << QString("Dim %1").arg(QString::number(dimensionIndex));
    }

    // Assign options
    _dimensionSelectAction.setOptions(options);
}

QStringList PointsDimensionPickerAction::getDimensionNames() const
{
    return _dimensionSelectAction.getOptions();
}

void PointsDimensionPickerAction::setCurrentDimensionName(const QString& dimensionName)
{
    _dimensionSelectAction.setCurrentText(dimensionName);
}

PointsDimensionPickerAction::Widget::Widget(QWidget* parent, PointsDimensionPickerAction* pointsDimensionPickerAction) :
    WidgetActionWidget(parent, pointsDimensionPickerAction)
{
    auto layout = new QHBoxLayout();

    layout->setMargin(0);
    layout->setSpacing(3);

    layout->addWidget(pointsDimensionPickerAction->getDimensionSelectAction().createWidget(this));
    layout->addWidget(pointsDimensionPickerAction->getDimensionSearchAction().createCollapsedWidget(this));

    setLayout(layout);
}
