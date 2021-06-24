#include "OptionAction.h"
#include "Application.h"

#include <QDebug>
#include <QHBoxLayout>
#include <QComboBox>
#include <QPushButton>

namespace hdps {

namespace gui {

OptionAction::OptionAction(QObject* parent, const QString& title /*= ""*/) :
    WidgetAction(parent),
    _options(),
    _model(nullptr),
    _currentIndex(-1),
    _defaultIndex(0)
{
    setText(title);
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

    if (_currentIndex >= options.count())
        setCurrentIndex(0);
}

QAbstractListModel* OptionAction::getModel()
{
    return _model;
}

void OptionAction::setModel(QAbstractListModel* listModel)
{
    if (listModel == _model)
        return;

    _model = listModel;

    emit modelChanged(_model);
}

bool OptionAction::hasModel() const
{
    return _model != nullptr;
}

std::int32_t OptionAction::getCurrentIndex() const
{
    return _currentIndex;
}

void OptionAction::setCurrentIndex(const std::int32_t& currentIndex)
{
    if (currentIndex == _currentIndex)
        return;

    if (!hasModel() && currentIndex >= static_cast<std::int32_t>(_options.count()))
        return;

    _currentIndex = currentIndex;
    
    emit currentIndexChanged(_currentIndex);
    emit currentTextChanged(getCurrentText());
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

void OptionAction::clearOptions()
{
    _options.clear();
}

QString OptionAction::getCurrentText() const
{
    if (_currentIndex < 0)
        return "";

    if (hasModel())
        return _model->data(_model->index(_currentIndex, 0), Qt::DisplayRole).toString();
    else
        return _options[_currentIndex];
}

void OptionAction::setCurrentText(const QString& currentText)
{
    if (!_options.contains(currentText))
        return;

    if (hasModel()) {
        const auto matches = _model->match(_model->index(0), Qt::DisplayRole, currentText, Qt::MatchFlag::MatchExactly);

        if (!matches.isEmpty())
            _currentIndex = matches.first().row();
    } else {
        if (_options.contains(currentText))
            _currentIndex = _options.indexOf(currentText);
        else
            _currentIndex = 0;
    }

    emit currentTextChanged(getCurrentText());
    emit currentIndexChanged(_currentIndex);
}

bool OptionAction::hasSelection() const
{
    return _currentIndex >= 0;
}

OptionAction::Widget::Widget(QWidget* parent, OptionAction* optionAction) :
    WidgetAction::Widget(parent, optionAction, Widget::State::Standard),
    _layout(new QHBoxLayout()),
    _comboBox(new QComboBox()),
    _resetPushButton(new QPushButton())
{
    //comboBox->setSizeAdjustPolicy(QComboBox::SizeAdjustPolicy::AdjustToContents);

    _layout->setMargin(0);
    _layout->addWidget(_comboBox, 1);

    setLayout(_layout);

    const auto updateToolTip = [this, optionAction]() -> void {
        _comboBox->setToolTip(optionAction->hasOptions() ? QString("%1: %2").arg(optionAction->toolTip(), optionAction->getCurrentText()) : optionAction->toolTip());
    };

    const auto populateComboBox = [this, optionAction, updateToolTip]() -> void {
        QSignalBlocker comboBoxSignalBlocker(_comboBox);

        _comboBox->clear();

        if (optionAction->hasModel()) {
            _comboBox->setModel(optionAction->getModel());
        }
        else {
            const auto options = optionAction->getOptions();
            
            _comboBox->addItems(options);
            _comboBox->setEnabled(!options.isEmpty());
        }
        
        //comboBox->adjustSize();

        updateToolTip();
    };

    connect(optionAction, &OptionAction::optionsChanged, this, [this, populateComboBox](const QStringList& options) {
        populateComboBox();
    });

    connect(optionAction, &OptionAction::modelChanged, this, [this, populateComboBox](QAbstractListModel* listModel) {
        populateComboBox();
    });

    const auto updateComboBoxSelection = [this, optionAction]() -> void {
        if (optionAction->getCurrentText() == _comboBox->currentText())
            return;
        
        QSignalBlocker comboBoxSignalBlocker(_comboBox);
        
        _comboBox->setCurrentText(optionAction->getCurrentText());
    };

    connect(optionAction, &OptionAction::currentIndexChanged, this, [this, updateComboBoxSelection](const std::int32_t& currentIndex) {
        updateComboBoxSelection();
    });

    connect(optionAction, &OptionAction::currentTextChanged, this, [this, updateComboBoxSelection, updateToolTip](const QString& currentText) {
        updateComboBoxSelection();
        updateToolTip();
    });

    connect(_comboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, [this, optionAction](const int& currentIndex) {
        optionAction->setCurrentIndex(currentIndex);
    });

    _resetPushButton->setVisible(false);
    _resetPushButton->setIcon(hdps::Application::getIconFont("FontAwesome").getIcon("undo"));
    _resetPushButton->setToolTip(QString("Reset %1").arg(optionAction->text()));

    _layout->addWidget(_resetPushButton);

    connect(_resetPushButton, &QPushButton::clicked, this, [this, optionAction]() {
        optionAction->reset();
    });

    const auto onUpdateCurrentIndex = [this, optionAction]() -> void {
        _resetPushButton->setEnabled(optionAction->canReset());
    };

    connect(optionAction, &OptionAction::currentIndexChanged, this, [this, onUpdateCurrentIndex](const QColor& color) {
        onUpdateCurrentIndex();
    });

    onUpdateCurrentIndex();
    populateComboBox();
    updateComboBoxSelection();
    updateToolTip();
}

}
}