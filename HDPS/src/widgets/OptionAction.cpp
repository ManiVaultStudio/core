#include "OptionAction.h"

namespace hdps {

namespace gui {

OptionAction::Widget::Widget(QWidget* parent, OptionAction* optionAction) :
    WidgetAction::Widget(parent, optionAction),
    _layout(),
    _comboBox()
{
    _layout.setMargin(0);
    _layout.addWidget(&_comboBox);

    setLayout(&_layout);

    if (childOfMenu())
        _comboBox.setFixedWidth(120);

    const auto populateComboBox = [this, optionAction]() {
        QSignalBlocker comboBoxSignalBlocker(&_comboBox);

        const auto options = optionAction->getOptions();

        _comboBox.addItems(options);
        _comboBox.setEnabled(!options.isEmpty());
    };

    connect(optionAction, &OptionAction::optionsChanged, this, [this, populateComboBox](const QStringList& options) {
        populateComboBox();
    });

    const auto updateComboBoxSelection = [this, optionAction]() {
        QSignalBlocker comboBoxSignalBlocker(&_comboBox);
        _comboBox.setCurrentIndex(optionAction->getCurrentIndex());
    };

    connect(optionAction, &OptionAction::currentIndexChanged, this, [this, updateComboBoxSelection](const std::int32_t& currentIndex) {
        updateComboBoxSelection();
    });

    connect(optionAction, &OptionAction::currentTextChanged, this, [this, updateComboBoxSelection](const QString& currentText) {
        updateComboBoxSelection();
    });

    connect(&_comboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, [this, optionAction](const int& currentIndex) {
        optionAction->setCurrentIndex(currentIndex);
    });

    populateComboBox();
    updateComboBoxSelection();
}

OptionAction::OptionAction(QObject* parent, const QString& title /*= ""*/) :
    WidgetAction(parent)
{
    setText(title);
}

QWidget* OptionAction::createWidget(QWidget* parent)
{
    auto widget = new Widget(parent, this);

    widget->show();

    return widget;
}

QStringList OptionAction::getOptions() const
{
    return _options;
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
    if (currentIndex == _currentIndex)
        return;

    _currentIndex = currentIndex;
    
    emit currentIndexChanged(_currentIndex);

    if (_currentIndex < _options.count())
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

}
}