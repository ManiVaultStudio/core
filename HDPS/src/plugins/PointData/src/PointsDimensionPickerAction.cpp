#include "PointsDimensionPickerAction.h"
#include "Application.h"

#include <QHBoxLayout>

using namespace hdps;

PointsDimensionPickerAction::PointsDimensionPickerAction(QObject* parent) :
    WidgetAction(parent),
    _points(nullptr),
    _currentDimensionAction(this, "Select dimension")
{
    setText("Points dimension picker");
    setIcon(Application::getIconFont("FontAwesome").getIcon("adjust"));

    _currentDimensionAction.setToolTip("Pick a dimension by selecting");
    _currentDimensionAction.setDefaultWidgetFlags(OptionAction::ComboBox | OptionAction::LineEdit);
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
        for (std::uint32_t dimensionIndex = 0; dimensionIndex < _points->getNumDimensions(); dimensionIndex++)
            options << QString("Dim %1").arg(QString::number(dimensionIndex));
    }

    // Assign options
    _currentDimensionAction.setOptions(options);
}

QStringList PointsDimensionPickerAction::getDimensionNames() const
{
    return _currentDimensionAction.getOptions();
}

void PointsDimensionPickerAction::setCurrentDimensionName(const QString& dimensionName)
{
    _currentDimensionAction.setCurrentText(dimensionName);
}

PointsDimensionPickerAction::Widget::Widget(QWidget* parent, PointsDimensionPickerAction* pointsDimensionPickerAction) :
    WidgetActionWidget(parent, pointsDimensionPickerAction)
{
    auto layout = new QHBoxLayout();

    layout->setMargin(0);

    auto widget = pointsDimensionPickerAction->getCurrentDimensionAction().createWidget(this);

    layout->addWidget(widget);

    setLayout(layout);

    // Update visibility of the combobox and line edit depending on the number of options
    const auto updateWidgets = [this, pointsDimensionPickerAction, widget]() {

        // Determine whether to show the combobox or the line edit
        const auto showComboBox = pointsDimensionPickerAction->getCurrentDimensionAction().getNumberOfOptions() < 5;

        // Show/hide combobox and line edit
        widget->findChild<QComboBox*>("ComboBox")->setVisible(showComboBox);
        widget->findChild<QLineEdit*>("LineEdit")->setVisible(!showComboBox);
    };

    // Update widgets when the number of options change
    connect(&pointsDimensionPickerAction->getCurrentDimensionAction(), &OptionAction::optionsChanged, updateWidgets);

    // Do an initial update at startup
    updateWidgets();
}
