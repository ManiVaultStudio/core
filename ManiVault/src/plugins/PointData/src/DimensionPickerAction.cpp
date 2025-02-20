// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "DimensionPickerAction.h"
#include "Application.h"

#include <QHBoxLayout>

using namespace mv;

DimensionPickerAction::DimensionPickerAction(QObject* parent, const QString& title) :
    WidgetAction(parent, title),
    _points(nullptr),
    _currentDimensionAction(this, "Select dimension"),
    _searchThreshold(DEFAULT_SEARCH_THRESHOLD)
{
    setText(title);
    setIconByName("adjust");

    _currentDimensionAction.setToolTip("Current dimension");

    // Selection changed notifier
    const auto selectionChanged = [this]() {
        emit currentDimensionIndexChanged(_currentDimensionAction.getCurrentIndex());
    };

    // Relay current index/text changed signal from the current dimension action
    connect(&_currentDimensionAction, &OptionAction::currentIndexChanged, this, selectionChanged);
}

void DimensionPickerAction::setPointsDataset(const Dataset<Points>& points)
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
    if (getNumberOfDimensions() >= 1) {
        setCurrentDimensionIndex(0);
    }
}

QStringList DimensionPickerAction::getDimensionNames() const
{
    return _currentDimensionAction.getOptions();
}

std::uint32_t DimensionPickerAction::getNumberOfDimensions() const
{
    return _currentDimensionAction.getNumberOfOptions();
}

std::int32_t DimensionPickerAction::getCurrentDimensionIndex() const
{
    return _currentDimensionAction.getCurrentIndex();
}

QString DimensionPickerAction::getCurrentDimensionName() const
{
    return _currentDimensionAction.getCurrentText();
}

void DimensionPickerAction::setCurrentDimensionIndex(const std::int32_t& dimensionIndex)
{
    _currentDimensionAction.setCurrentIndex(dimensionIndex);
}

void DimensionPickerAction::setCurrentDimensionName(const QString& dimensionName)
{
    _currentDimensionAction.setCurrentText(dimensionName);
}

std::uint32_t DimensionPickerAction::getSearchThreshold() const
{
    return _searchThreshold;
}

void DimensionPickerAction::setSearchThreshold(const std::uint32_t& searchThreshold)
{
    _searchThreshold = searchThreshold;
}

bool DimensionPickerAction::maySearch() const
{
    return _currentDimensionAction.getNumberOfOptions() >= _searchThreshold;
}

void DimensionPickerAction::connectToPublicAction(WidgetAction* publicAction, bool recursive)
{
    auto publicDimensionPickerAction = dynamic_cast<DimensionPickerAction*>(publicAction);

    Q_ASSERT(publicDimensionPickerAction != nullptr);

    if (publicDimensionPickerAction == nullptr)
        return;

    if (recursive) {
        actions().connectPrivateActionToPublicAction(&_currentDimensionAction, &publicDimensionPickerAction->getCurrentDimensionAction(), recursive);
    }

    WidgetAction::connectToPublicAction(publicAction, recursive);
}

void DimensionPickerAction::disconnectFromPublicAction(bool recursive)
{
    if (!isConnected())
        return;

    if (recursive) {
        actions().disconnectPrivateActionFromPublicAction(&_currentDimensionAction, recursive);
    }

    WidgetAction::disconnectFromPublicAction(recursive);
}

void DimensionPickerAction::fromVariantMap(const QVariantMap& variantMap)
{
    WidgetAction::fromVariantMap(variantMap);

    if (variantMap.contains("DatasetID")) {
        const auto datasetID = variantMap["DatasetID"].toString();

        if (!datasetID.isEmpty()) {
            auto dataset = mv::data().getDataset(datasetID);

            if (dataset.isValid())
                setPointsDataset(Dataset<Points>(dataset));
        }
    }

    _currentDimensionAction.fromParentVariantMap(variantMap);
}

QVariantMap DimensionPickerAction::toVariantMap() const
{
    auto variantMap = WidgetAction::toVariantMap();

    _currentDimensionAction.insertIntoVariantMap(variantMap);

    const auto datasetId = _points.isValid() ? _points->getId() : "";

    variantMap.insert({
        { "DatasetID", datasetId }
    });

    return variantMap;
}

DimensionPickerAction::Widget::Widget(QWidget* parent, DimensionPickerAction* dimensionPickerAction) :
    WidgetActionWidget(parent, dimensionPickerAction)
{
    auto layout = new QHBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);

    // Create widgets
    auto comboBoxWidget = dimensionPickerAction->getCurrentDimensionAction().createWidget(this, OptionAction::ComboBox);
    auto lineEditWidget = dimensionPickerAction->getCurrentDimensionAction().createWidget(this, OptionAction::LineEdit);

    // Gets search icon to decorate the line edit
    const auto searchIcon = StyledIcon("magnifying-glass");

    // Add the icon to the line edit
    lineEditWidget->findChild<QLineEdit*>("LineEdit")->addAction(searchIcon, QLineEdit::TrailingPosition);

    // Add widgets to layout
    layout->addWidget(comboBoxWidget);
    layout->addWidget(lineEditWidget);

    // Update widgets visibility
    const auto updateWidgetsVisibility = [dimensionPickerAction, comboBoxWidget, lineEditWidget]() {
        
        // Get whether the option action may be searched
        const auto maySearch = dimensionPickerAction->maySearch();

        // Update visibility
        comboBoxWidget->setVisible(!maySearch);
        lineEditWidget->setVisible(maySearch);
    };

    // Update widgets visibility when the dimensions change
    connect(&dimensionPickerAction->getCurrentDimensionAction(), &OptionAction::modelChanged, this, updateWidgetsVisibility);

    // Initial update of widgets visibility
    updateWidgetsVisibility();

    // Apply layout
    setLayout(layout);
}
