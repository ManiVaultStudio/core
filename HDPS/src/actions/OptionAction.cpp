#include "OptionAction.h"
#include "Application.h"

#include <QDebug>
#include <QHBoxLayout>
#include <QComboBox>

namespace hdps {

namespace gui {

OptionAction::OptionAction(QObject* parent, const QString& title /*= ""*/) :
    WidgetAction(parent),
    _options(),
    _currentIndex(-1),
    _defaultIndex(0),
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
}

bool OptionAction::canReset() const
{
    return _currentIndex != _defaultIndex;
}

void OptionAction::reset()
{
    setCurrentIndex(_defaultIndex);
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

OptionAction::Widget::Widget(QWidget* parent, OptionAction* optionAction, const bool& resettable /*= true*/) :
    WidgetAction::Widget(parent, optionAction)
{
    auto layout = new QHBoxLayout();

    auto comboBox = new QComboBox();

    layout->setMargin(0);
    layout->addWidget(comboBox);

    setLayout(layout);

    const auto updateToolTip = [this, optionAction, comboBox]() -> void {
        comboBox->setToolTip(optionAction->hasOptions() ? QString("%1: %2").arg(optionAction->toolTip(), optionAction->getCurrentText()) : optionAction->toolTip());
    };

    const auto populateComboBox = [this, optionAction, updateToolTip, comboBox]() -> void {
        QSignalBlocker comboBoxSignalBlocker(comboBox);

        const auto options = optionAction->getOptions();

        comboBox->clear();
        comboBox->addItems(options);
        comboBox->setEnabled(!options.isEmpty());

        updateToolTip();
    };

    const auto connected = connect(optionAction, &OptionAction::optionsChanged, this, [this, populateComboBox](const QStringList& options) {
        populateComboBox();
    });

    const auto updateComboBoxSelection = [this, optionAction, comboBox]() -> void {
        if (optionAction->getCurrentIndex() == comboBox->currentIndex())
            return;
        
        comboBox->setCurrentIndex(optionAction->getCurrentIndex());
    };

    connect(optionAction, &OptionAction::currentIndexChanged, this, [this, updateComboBoxSelection](const std::int32_t& currentIndex) {
        updateComboBoxSelection();
    });

    connect(optionAction, &OptionAction::currentTextChanged, this, [this, updateComboBoxSelection, updateToolTip](const QString& currentText) {
        updateComboBoxSelection();
        updateToolTip();
    });

    connect(comboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, [this, optionAction](const int& currentIndex) {
        optionAction->setCurrentIndex(currentIndex);
    });

    if (resettable) {
        auto resetPushButton = new QPushButton();

        resetPushButton->setIcon(hdps::Application::getIconFont("FontAwesome").getIcon("undo"));
        resetPushButton->setToolTip(QString("Reset %1").arg(optionAction->text()));

        layout->addWidget(resetPushButton);

        connect(resetPushButton, &QPushButton::clicked, this, [this, optionAction]() {
            optionAction->reset();
        });

        const auto onUpdateCurrentIndex = [this, optionAction, resetPushButton]() -> void {
            resetPushButton->setEnabled(optionAction->canReset());
        };

        connect(optionAction, &OptionAction::currentIndexChanged, this, [this, onUpdateCurrentIndex](const QColor& color) {
            onUpdateCurrentIndex();
        });

        onUpdateCurrentIndex();
    }

    populateComboBox();
    updateComboBoxSelection();
    updateToolTip();
}

}
}