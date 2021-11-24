#include "OptionAction.h"
#include "Application.h"

#include <QDebug>
#include <QHBoxLayout>
#include <QComboBox>
#include <QListView>

namespace hdps {

namespace gui {

OptionAction::OptionAction(QObject* parent, const QString& title /*= ""*/, const QStringList& options /*= QStringList()*/, const QString& currentOption /*= ""*/, const QString& defaultOption /*= ""*/) :
    WidgetAction(parent),
    _defaultModel(),
    _customModel(nullptr),
    _currentIndex(-1),
    _defaultIndex(0)
{
    setText(title);
    setMayReset(true);
    setDefaultWidgetFlags(WidgetFlag::Basic);
    initialize(options, currentOption, defaultOption);
}

void OptionAction::initialize(const QStringList& options /*= QStringList()*/, const QString& currentOption /*= ""*/, const QString& defaultOption /*= ""*/)
{
    setOptions(options);
    setCurrentText(currentOption);
    setDefaultText(defaultOption);

    setResettable(isResettable());
}

void OptionAction::initialize(QAbstractItemModel& customModel, const QString& currentOption /*= ""*/, const QString& defaultOption /*= ""*/)
{
    setCustomModel(&customModel);
    setCurrentText(currentOption);
    setDefaultText(defaultOption);

    setResettable(isResettable());
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
    if (_defaultModel.stringList() == options)
        return;

    // Cache the current index
    const auto currentIndex = _currentIndex;

    // Assign the options
    _defaultModel.setStringList(options);

    // Re-assign cached current index
    _currentIndex = options.count() > 0 ? std::max(0, std::min(currentIndex, options.count() - 1)) : -1;

    // Notify the others that the options changed
    emit optionsChanged();

    setResettable(isResettable());
}

const QAbstractItemModel* OptionAction::getModel() const
{
    if (_customModel != nullptr)
        return _customModel;

    return &_defaultModel;
}

void OptionAction::setCustomModel(QAbstractItemModel* itemModel)
{
    if (itemModel == _customModel)
        return;

    _customModel = itemModel;

    emit customModelChanged(_customModel);
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

    if (currentIndex >= static_cast<std::int32_t>(getOptions().count()))
        return;

    _currentIndex = currentIndex;
    
    emit currentIndexChanged(_currentIndex);
    emit currentTextChanged(getCurrentText());
    
    setResettable(isResettable());
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

    emit defaultIndexChanged(_defaultIndex);

    setResettable(isResettable());
}

QString OptionAction::getDefaultText() const
{
    return getOptions()[_defaultIndex];
}

void OptionAction::setDefaultText(const QString& defaultText)
{
    _defaultIndex = getOptions().indexOf(defaultText);
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

    setResettable(isResettable());
}

bool OptionAction::isResettable() const
{
    return _currentIndex != _defaultIndex;
}

void OptionAction::reset()
{
    setCurrentIndex(_defaultIndex);
}

bool OptionAction::hasSelection() const
{
    return _currentIndex >= 0;
}

OptionAction::ComboBoxWidget::ComboBoxWidget(QWidget* parent, OptionAction* optionAction) :
    QComboBox(parent),
    _completer()
{
    setObjectName("ComboBox");
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setCompleter(&_completer);

    // Configure completer
    _completer.setCaseSensitivity(Qt::CaseInsensitive);
    _completer.setFilterMode(Qt::MatchContains);
    _completer.setCompletionColumn(0);
    _completer.setCompletionMode(QCompleter::PopupCompletion);

    // Update completer
    const auto updateCompleter = [this, optionAction]() -> void {
        _completer.setModel(const_cast<QAbstractItemModel*>(optionAction->getModel()));
    };

    // Update tooltips
    const auto updateToolTip = [this, optionAction]() -> void {
        setToolTip(optionAction->hasOptions() ? QString("%1: %2").arg(optionAction->toolTip(), optionAction->getCurrentText()) : optionAction->toolTip());
    };

    // Fill combobox with options from the option action
    const auto populateComboBox = [this, optionAction, updateToolTip, updateCompleter]() -> void {
        QSignalBlocker comboBoxSignalBlocker(this);

        setModel(new QStringListModel());
        setModel(const_cast<QAbstractItemModel*>(optionAction->getModel()));
        setEnabled(optionAction->getNumberOfOptions() >= 2);
        setCurrentIndex(optionAction->getCurrentIndex());

        // Update completer and tooltips
        updateCompleter();
        updateToolTip();
    };

    // Populate the combobox when the options change
    connect(optionAction, &OptionAction::optionsChanged, this, populateComboBox);

    // Update combobox selection when the action changes
    const auto updateComboBoxSelection = [this, optionAction]() -> void {
        if (optionAction->getCurrentText() == currentText())
            return;
        
        QSignalBlocker comboBoxSignalBlocker(this);
        
        setCurrentText(optionAction->getCurrentText());
    };

    // Update the combobox selection and tooltip when the option action selection changes
    connect(optionAction, &OptionAction::currentTextChanged, this, [this, updateComboBoxSelection, updateToolTip](const QString& currentText) {
        updateComboBoxSelection();
        updateToolTip();
    });

    // Update the option action when the combobox selection changes
    connect(this, qOverload<int>(&QComboBox::currentIndexChanged), this, [this, optionAction](const int& currentIndex) {
        optionAction->setCurrentIndex(currentIndex);
    });

    // Do initial updates
    updateCompleter();
    populateComboBox();
    updateComboBoxSelection();
    updateToolTip();
}

QWidget* OptionAction::getWidget(QWidget* parent, const std::int32_t& widgetFlags)
{
    auto widget = new WidgetActionWidget(parent, this);
    auto layout = new QHBoxLayout();

    layout->setMargin(0);

    if (widgetFlags & WidgetFlag::ComboBox)
        layout->addWidget(new OptionAction::ComboBoxWidget(parent, this));

    if (widgetFlags & WidgetFlag::ResetPushButton)
        layout->addWidget(createResetButton(parent));

    widget->setLayout(layout);

    return widget;
}

}
}
