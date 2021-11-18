#include "PointsDimensionPickerAction.h"
#include "Application.h"

#include <QHBoxLayout>

using namespace hdps;

PointsDimensionPickerAction::PointsDimensionPickerAction(QObject* parent, const QString& title) :
    WidgetAction(parent),
    _points(nullptr),
    _currentDimensionAction(this, "Select dimension")
{
    setText(title);
    setIcon(Application::getIconFont("FontAwesome").getIcon("adjust"));

    _currentDimensionAction.setToolTip("Pick a dimension by selecting");

    // Show both the combobox and line edit by default and change their visibility when the dimensions change
    _currentDimensionAction.setDefaultWidgetFlags(OptionAction::ComboBox | OptionAction::LineEdit);

    // Selection changed notifier
    const auto selectionChanged = [this]() {
        emit currentDimensionIndexChanged(_currentDimensionAction.getCurrentIndex());
        emit currentDimensionNameChanged(_currentDimensionAction.getCurrentText());
    };

    // Relay current index/text changed signal from the current dimension action
    connect(&_currentDimensionAction, &OptionAction::currentIndexChanged, this, selectionChanged);
    connect(&_currentDimensionAction, &OptionAction::currentTextChanged, this, selectionChanged);
}

void PointsDimensionPickerAction::setPointsDataset(const Dataset<Points>& points)
{
    // Only proceed if we have a valid points dataset
    if (!points.isValid()) {

        // Reset options
        _currentDimensionAction.setOptions(QStringList());

        // Reset current dimension index
        setCurrentDimensionIndex(-1);

        return;
    }

    // Assign points dataset reference
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

    // And set current dimensions
    if (getNumberOfDimensions() >= 1)
        setCurrentDimensionIndex(0);
}

QStringList PointsDimensionPickerAction::getDimensionNames() const
{
    return _currentDimensionAction.getOptions();
}

std::uint32_t PointsDimensionPickerAction::getNumberOfDimensions() const
{
    return _currentDimensionAction.getNumberOfOptions();
}

std::int32_t PointsDimensionPickerAction::getCurrentDimensionIndex() const
{
    return _currentDimensionAction.getCurrentIndex();
}

QString PointsDimensionPickerAction::getCurrentDimensionName() const
{
    return _currentDimensionAction.getCurrentText();
}

void PointsDimensionPickerAction::setCurrentDimensionIndex(const std::int32_t& dimensionIndex)
{
    _currentDimensionAction.setCurrentIndex(dimensionIndex);
}

void PointsDimensionPickerAction::setCurrentDimensionName(const QString& dimensionName)
{
    _currentDimensionAction.setCurrentText(dimensionName);
}

void PointsDimensionPickerAction::setDefaultDimensionIndex(const std::int32_t& defaultDimensionIndex)
{
    _currentDimensionAction.setDefaultIndex(defaultDimensionIndex);
}

void PointsDimensionPickerAction::setDefaultDimensionName(const QString& defaultDimensionName)
{
    _currentDimensionAction.setDefaultText(defaultDimensionName);
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
        const auto showComboBox = pointsDimensionPickerAction->getCurrentDimensionAction().getNumberOfOptions() < 1000;

        // Show/hide combobox and line edit
        widget->findChild<QComboBox*>("ComboBox")->setVisible(showComboBox);
        widget->findChild<QLineEdit*>("LineEdit")->setVisible(!showComboBox);
    };

    // Update widgets when the number of options change
    connect(&pointsDimensionPickerAction->getCurrentDimensionAction(), &OptionAction::optionsChanged, updateWidgets);

    // Do an initial update at startup
    updateWidgets();
}
