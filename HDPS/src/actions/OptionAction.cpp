#include "OptionAction.h"
#include "Application.h"

#include <QDebug>
#include <QHBoxLayout>
#include <QComboBox>
#include <QListView>
#include <QPushButton>

namespace hdps {

namespace gui {

OptionAction::OptionAction(QObject* parent, const QString& title /*= ""*/, const QStringList& options /*= QStringList()*/, const QString& currentOption /*= ""*/, const QString& defaultOption /*= ""*/) :
    WidgetAction(parent),
    _defaultModel(),
    _customModel(nullptr),
    _currentIndex(-1),
    _defaultIndex(0),
    _placeholderString()
{
    setText(title);
    setDefaultWidgetFlags(WidgetFlag::Default);
    initialize(options, currentOption, defaultOption);
}

void OptionAction::initialize(const QStringList& options /*= QStringList()*/, const QString& currentOption /*= ""*/, const QString& defaultOption /*= ""*/)
{
    setOptions(options);
    setCurrentText(currentOption);
    setDefaultText(defaultOption);
    notifyResettable();
}

void OptionAction::initialize(QAbstractItemModel& customModel, const QString& currentOption /*= ""*/, const QString& defaultOption /*= ""*/)
{
    setCustomModel(&customModel);
    setCurrentText(currentOption);
    setDefaultText(defaultOption);
    notifyResettable();
}

QStringList OptionAction::getOptions() const
{
    QStringList options;

    for (int rowIndex = 0; rowIndex < getModel()->rowCount(); ++rowIndex)
        options << getModel()->index(rowIndex, 0).data(Qt::DisplayRole).toString();

    return options;
}

std::uint32_t OptionAction::getNumberOfOptions() const
{
    return getModel()->rowCount();
}

bool OptionAction::hasOption(const QString& option) const
{
    return getModel()->match(getModel()->index(0, 0), Qt::DisplayRole, option).count() == 1;
}

bool OptionAction::hasOptions() const
{
    return !getOptions().isEmpty();
}

void OptionAction::setOptions(const QStringList& options)
{
    // Only proceed if the options changed
    if (_defaultModel.stringList() == options)
        return;

    // Cache the current index and text
    const auto oldCurrentIndex  = _currentIndex;
    const auto oldCurrentText   = getCurrentText();

    if (_defaultModel.rowCount() == options.count()) {

        // Number of items matches so update string list model with new items
        for (std::int32_t rowIndex = 0; rowIndex < _defaultModel.rowCount(); rowIndex++)
            _defaultModel.setData(_defaultModel.index(rowIndex, 0), options.at(rowIndex));
    }
    else {

        // Override the string in the string list model
        _defaultModel.setStringList(options);

        // Update the current index so that it respects the underlying model
        updateCurrentIndex();
    }

    // Notify the others that the model changed
    emit modelChanged();

    // Notify others that the current index and text changed
    emit currentIndexChanged(_currentIndex);
    emit currentTextChanged(getCurrentText());

    notifyResettable();
}

const QAbstractItemModel* OptionAction::getModel() const
{
    if (_customModel != nullptr)
        return _customModel;

    return &_defaultModel;
}

void OptionAction::fromVariantMap(const QVariantMap& variantMap)
{
    if (!variantMap.contains("Value"))
        return;

    setCurrentText(variantMap["Value"].toString());
}

QVariantMap OptionAction::toVariantMap() const
{
    return { { "Value", getCurrentText() } };
}

void OptionAction::updateCurrentIndex()
{
    if (getModel()->rowCount() < 1) {
        _currentIndex = -1;
    } else {
        if (_currentIndex >= getModel()->rowCount())
            _currentIndex = getModel()->rowCount() - 1;
    }
}

void OptionAction::setCustomModel(QAbstractItemModel* itemModel)
{
    if (itemModel == _customModel)
        return;

    _customModel = itemModel;

    // Notify others that the custom model and the current model changed
    emit customModelChanged(_customModel);
    emit modelChanged();

    // Possibly adjust the current index when the layout of the custom model changes
    if (_customModel)
        connect(_customModel, &QAbstractItemModel::layoutChanged, this, &OptionAction::updateCurrentIndex);

    // Update the current index so that it respects the underlying model
    updateCurrentIndex();

    // Notify others that the current index and text changed
    emit currentIndexChanged(_currentIndex);
    emit currentTextChanged(getCurrentText());
}

bool OptionAction::hasCustomModel() const
{
    return _customModel != nullptr;
}

std::int32_t OptionAction::getCurrentIndex() const
{
    return _currentIndex;
}

void OptionAction::setCurrentIndex(const std::int32_t& currentIndex)
{
    if (currentIndex == _currentIndex)
        return;

    _currentIndex = currentIndex;

    // Update the current index so that it respects the underlying model
    updateCurrentIndex();

    // Notify others that the current index and text changed
    emit currentIndexChanged(_currentIndex);
    emit currentTextChanged(getCurrentText());

    notifyResettable();
}

std::int32_t OptionAction::getDefaultIndex() const
{
    return _defaultIndex;
}

void OptionAction::setDefaultIndex(const std::int32_t& defaultIndex)
{
    if (defaultIndex == _defaultIndex)
        return;

    _defaultIndex = defaultIndex;

    // Notify others that the default index changed
    emit defaultIndexChanged(_defaultIndex);

    notifyResettable();
}

QString OptionAction::getDefaultText() const
{
    return getOptions()[_defaultIndex];
}

void OptionAction::setDefaultText(const QString& defaultText)
{
    _defaultIndex = getOptions().indexOf(defaultText);
}

QString OptionAction::getPlaceholderString() const
{
    return _placeholderString;
}

void OptionAction::setPlaceHolderString(const QString& placeholderString)
{
    if (placeholderString == _placeholderString)
        return;

    _placeholderString = placeholderString;

    emit placeholderStringChanged(_placeholderString);
}

QString OptionAction::getCurrentText() const
{
    if (_currentIndex < 0)
        return "";

    auto options = getOptions();

    if (_currentIndex >= options.count())
        return "";

    return getOptions()[_currentIndex];
}

void OptionAction::setCurrentText(const QString& currentText)
{
    if (hasOption(currentText))
        _currentIndex = getModel()->match(getModel()->index(0, 0), Qt::DisplayRole, currentText).first().row();

    emit currentTextChanged(getCurrentText());
    emit currentIndexChanged(_currentIndex);

    notifyResettable();
}

bool OptionAction::hasSelection() const
{
    return _currentIndex >= 0;
}

OptionAction::ComboBoxWidget::ComboBoxWidget(QWidget* parent, OptionAction* optionAction) :
    QComboBox(parent)
{
    setObjectName("ComboBox");
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    // Update tooltips
    const auto updateToolTip = [this, optionAction]() -> void {
        setToolTip(optionAction->hasOptions() ? QString("%1: %2").arg(optionAction->toolTip(), optionAction->getCurrentText()) : optionAction->toolTip());
    };

    // Assign option action model to combobox
    const auto updateComboBoxModel = [this, optionAction, updateToolTip]() -> void {
        
        // Prevent signals from being emitted
        QSignalBlocker comboBoxSignalBlocker(this);

        // Disconnect from any previous list model
        if (model())
            disconnect(model(), &QAbstractItemModel::layoutChanged, this, nullptr);

        // Update model and enable/disable based on the row count
        setModel(const_cast<QAbstractItemModel*>(optionAction->getModel()));

        // Enabled/disable the combobox depending on the number of items
        connect(model(), &QAbstractItemModel::layoutChanged, this, [this, optionAction](const QList<QPersistentModelIndex>& parents = QList<QPersistentModelIndex>(), QAbstractItemModel::LayoutChangeHint hint = QAbstractItemModel::NoLayoutChangeHint) {
            setEnabled(optionAction->getNumberOfOptions() >= 2);
        });

        updateToolTip();
    };

    // Assign model when changed in the option action
    connect(optionAction, &OptionAction::modelChanged, this, updateComboBoxModel);

    // Update combobox selection when the action changes
    const auto updateComboBoxSelection = [this, optionAction]() -> void {

        // Only proceed if the current text changed
        if (optionAction->getCurrentText() == currentText())
            return;

        // Prevent signals from being emitted
        QSignalBlocker comboBoxSignalBlocker(this);

        setCurrentText(optionAction->getCurrentText());
    };

    // Update the combobox selection and tooltip when the option action selection changes
    connect(optionAction, &OptionAction::currentTextChanged, this, [this, updateComboBoxSelection, updateToolTip](const QString& currentText) {

        // Prevent signals from being emitted
        QSignalBlocker comboBoxSignalBlocker(this);

        updateComboBoxSelection();
        updateToolTip();
    });

    // Update the option action when the combobox selection changes
    connect(this, qOverload<int>(&QComboBox::currentIndexChanged), this, [this, optionAction](const int& currentIndex) {
        optionAction->setCurrentIndex(currentIndex);
    });

    // Update the place holder text in the combobox
    const auto updatePlaceHolderText = [this, optionAction]() -> void {
        setPlaceholderText(optionAction->getPlaceholderString());
    };

    // Update the combobox placeholder string when the action placeholder string changes
    connect(optionAction, &OptionAction::placeholderStringChanged, this, updatePlaceHolderText);

    // Perform initial updates
    updateComboBoxModel();
    updateComboBoxSelection();
    updateToolTip();
    updatePlaceHolderText();
}

OptionAction::LineEditWidget::LineEditWidget(QWidget* parent, OptionAction* optionAction) :
    QLineEdit(parent),
    _optionAction(optionAction),
    _completer()
{
    setObjectName("LineEdit");
    setCompleter(&_completer);

    _completer.setCaseSensitivity(Qt::CaseInsensitive);
    _completer.setFilterMode(Qt::MatchContains);
    _completer.setCompletionColumn(0);
    _completer.setCompletionMode(QCompleter::PopupCompletion);

    // Update completer model
    const auto updateCompleterModel = [this, optionAction]() {
        _completer.setModel(const_cast<QAbstractItemModel*>(optionAction->getModel()));
    };

    // Update completer model when the option action model changes
    connect(optionAction, &OptionAction::modelChanged, this, updateCompleterModel);

    // Update line edit text when the current option changes
    const auto updateText = [this, optionAction]() -> void {

        // Prevent signals from being emitted
        QSignalBlocker lineEditBlocker(this);

        // Update text
        setText(optionAction->getCurrentText());
    };

    // Update line edit text when the current option changes
    connect(optionAction, &OptionAction::currentTextChanged, this, updateText);

    // Update option action selection when the line edit editing finished
    connect(this, &QLineEdit::editingFinished, this, [this, optionAction]() {
        optionAction->setCurrentText(text());
    });

    // Update option action selection when auto completion activated
    connect(&_completer, QOverload<const QString&>::of(&QCompleter::activated), [this, optionAction](const QString& text) {
        optionAction->setCurrentText(text);
    });

    // Update option action selection when a auto completion item is highlighted
    connect(&_completer, QOverload<const QString&>::of(&QCompleter::highlighted), [this, optionAction](const QString& text) {
        optionAction->setCurrentText(text);
    });

    // Do an initial update of the completer model and update the line edit text
    updateCompleterModel();
    updateText();
}

OptionAction::ButtonsWidget::ButtonsWidget(QWidget* parent, OptionAction* optionAction, const Qt::Orientation& orientation) :
    QWidget(parent)
{
    setObjectName("Buttons");

    // Create horizontal layout
    QLayout* layout = nullptr;

    switch (orientation)
    {
        case Qt::Horizontal:
            layout = new QHBoxLayout();
            break;

        case Qt::Vertical:
            layout = new QVBoxLayout();
            break;

        default:
            break;
    }

    layout->setMargin(0);

    std::int32_t optionIndex = 0;

    // Add push button for each option
    for (const auto& option : optionAction->getOptions()) {

        // Create push button for the option
        auto optionPushButton = new QPushButton(option);

        // Set tooltip
        optionPushButton->setToolTip(optionAction->text() + ": " + option);

        // Add the push button to the layout
        layout->addWidget(optionPushButton);

        // Update the current index when the option push button is clicked
        connect(optionPushButton, &QPushButton::clicked, this, [optionAction, optionIndex]() {
            optionAction->setCurrentIndex(optionIndex);
        });

        optionIndex++;
    }

    // Apply the layout
    setLayout(layout);
}

QWidget* OptionAction::getWidget(QWidget* parent, const std::int32_t& widgetFlags)
{
    auto widget = new WidgetActionWidget(parent, this);
    auto layout = new QHBoxLayout();

    layout->setMargin(0);

    if (widgetFlags & WidgetFlag::ComboBox)
        layout->addWidget(new OptionAction::ComboBoxWidget(parent, this));

    if (widgetFlags & WidgetFlag::LineEdit)
        layout->addWidget(new OptionAction::LineEditWidget(parent, this));

    if (widgetFlags & WidgetFlag::HorizontalButtons)
        layout->addWidget(new ButtonsWidget(parent, this, Qt::Horizontal));

    if (widgetFlags & WidgetFlag::VerticalButtons)
        layout->addWidget(new ButtonsWidget(parent, this, Qt::Vertical));

    widget->setLayout(layout);

    return widget;
}



}
}
