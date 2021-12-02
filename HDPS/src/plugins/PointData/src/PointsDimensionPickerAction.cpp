#include "PointsDimensionPickerAction.h"
#include "Application.h"

#include <QHBoxLayout>

using namespace hdps;

PointsDimensionPickerAction::PointsDimensionPickerAction(QObject* parent, const QString& title) :
    WidgetAction(parent),
    _points(nullptr),
    _currentDimensionAction(this, "Select dimension"),
    _searchThreshold(DEFAULT_SEARCH_THRESHOLD)
{
    setText(title);
    setIcon(Application::getIconFont("FontAwesome").getIcon("adjust"));

    _currentDimensionAction.setToolTip("Current dimension");

    // Selection changed notifier
    const auto selectionChanged = [this]() {
        emit currentDimensionIndexChanged(_currentDimensionAction.getCurrentIndex());
    };

    // Relay current index/text changed signal from the current dimension action
    connect(&_currentDimensionAction, &OptionAction::currentIndexChanged, this, selectionChanged);
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

std::uint32_t PointsDimensionPickerAction::getSearchThreshold() const
{
    return _searchThreshold;
}

void PointsDimensionPickerAction::setSearchThreshold(const std::uint32_t& searchThreshold)
{
    _searchThreshold = searchThreshold;
}

bool PointsDimensionPickerAction::maySearch() const
{
    return _currentDimensionAction.getNumberOfOptions() >= _searchThreshold;
}

PointsDimensionPickerAction::Widget::Widget(QWidget* parent, PointsDimensionPickerAction* pointsDimensionPickerAction) :
    WidgetActionWidget(parent, pointsDimensionPickerAction)
{
    auto layout = new QHBoxLayout();

    layout->setMargin(0);

    // Create widgets
    auto comboBoxWidget = pointsDimensionPickerAction->getCurrentDimensionAction().createWidget(this, OptionAction::ComboBox);
    auto lineEditWidget = pointsDimensionPickerAction->getCurrentDimensionAction().createWidget(this, OptionAction::LineEdit);

    // Gets search icon to decorate the line edit
    const auto searchIcon = Application::getIconFont("FontAwesome").getIcon("search");

    // Add the icon to the line edit
    lineEditWidget->findChild<QLineEdit*>("LineEdit")->addAction(searchIcon, QLineEdit::TrailingPosition);

    // Add widgets to layout
    layout->addWidget(comboBoxWidget);
    layout->addWidget(lineEditWidget);

    // Update widgets visibility
    const auto updateWidgetsVisibility = [pointsDimensionPickerAction, comboBoxWidget, lineEditWidget]() {
        
        // Get whether the option action may be searched
        const auto maySearch = pointsDimensionPickerAction->maySearch();

        // Update visibility
        comboBoxWidget->setVisible(!maySearch);
        lineEditWidget->setVisible(maySearch);
    };

    // Update widgets visibility when the dimensions change
    connect(&pointsDimensionPickerAction->getCurrentDimensionAction(), &OptionAction::modelChanged, this, updateWidgetsVisibility);

    // Initial update of widgets visibility
    updateWidgetsVisibility();

    // Apply layout
    setLayout(layout);
}
