#include "OptionAction.h"

#include <QDebug>

namespace hdps {

namespace gui {

OptionAction::OptionAction(QObject* parent, const QString& title /*= ""*/) :
    WidgetAction(parent),
    _options(),
    _currentIndex(-1),
    _currentText()
{
    setText(title);
}

QWidget* OptionAction::createWidget(QWidget* parent)
{
    return new Widget(parent, this);
}

QStringList OptionAction::getOptions() const
{
    return _options;
}

bool OptionAction::hasOptions() const
{
    return !_options.isEmpty();
}

void OptionAction::setOptions(const QStringList& options)
{
    if (options == _options)
        return;

    _options = options;

    emit optionsChanged(_options);

    setCurrentIndex(0);
}

std::int32_t OptionAction::getCurrentIndex() const
{
    return _currentIndex;
}

void OptionAction::setCurrentIndex(const std::int32_t& currentIndex)
{
    if (currentIndex == _currentIndex || currentIndex >= static_cast<std::int32_t>(_options.count()))
        return;

    _currentIndex = currentIndex;
    
    emit currentIndexChanged(_currentIndex);

    setCurrentText(_options[_currentIndex]);
}

QString OptionAction::getCurrentText() const
{
    return _currentText;
}

void OptionAction::setCurrentText(const QString& currentText)
{
    if (currentText == _currentText)
        return;

    if (!_options.contains(currentText))
        return;

    _currentText = currentText;

    emit currentTextChanged(_currentText);

    setCurrentIndex(_options.indexOf(_currentText));
}

bool OptionAction::hasSelection() const
{
    return _currentIndex >= 0;
}

OptionAction::Widget::Widget(QWidget* parent, OptionAction* optionAction) :
    WidgetAction::Widget(parent, optionAction),
    _layout(),
    _comboBox()
{
    _layout.setMargin(0);
    _layout.addWidget(&_comboBox);

    setLayout(&_layout);

    const auto updateToolTip = [this, optionAction]() -> void {
        _comboBox.setToolTip(optionAction->hasOptions() ? QString("%1: %2").arg(optionAction->toolTip(), optionAction->getCurrentText()) : optionAction->toolTip());
    };

    const auto populateComboBox = [this, optionAction, updateToolTip]() -> void {
        QSignalBlocker comboBoxSignalBlocker(&_comboBox);

        const auto options = optionAction->getOptions();

        _comboBox.clear();
        _comboBox.addItems(options);
        _comboBox.setEnabled(!options.isEmpty());

        updateToolTip();
    };

    const auto connected = connect(optionAction, &OptionAction::optionsChanged, this, [this, populateComboBox](const QStringList& options) {
        populateComboBox();
    });

    const auto updateComboBoxSelection = [this, optionAction]() -> void {
        if (optionAction->getCurrentIndex() == _comboBox.currentIndex())
            return;
        
        _comboBox.setCurrentIndex(optionAction->getCurrentIndex());
    };

    connect(optionAction, &OptionAction::currentIndexChanged, this, [this, updateComboBoxSelection](const std::int32_t& currentIndex) {
        updateComboBoxSelection();
    });

    connect(optionAction, &OptionAction::currentTextChanged, this, [this, updateComboBoxSelection, updateToolTip](const QString& currentText) {
        updateComboBoxSelection();
        updateToolTip();
    });

    connect(&_comboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, [this, optionAction](const int& currentIndex) {
        optionAction->setCurrentIndex(currentIndex);
    });

    populateComboBox();
    updateComboBoxSelection();
    updateToolTip();
}

}
}