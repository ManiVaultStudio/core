#include "OptionAction.h"
#include "Application.h"

#include <QDebug>
#include <QHBoxLayout>
#include <QComboBox>
#include <QPushButton>

namespace hdps {

namespace gui {

OptionAction::OptionAction(QObject* parent, const QString& title /*= ""*/, const QStringList& options /*= QStringList()*/, const QString& currentOption /*= ""*/, const QString& defaultOption /*= ""*/) :
    WidgetAction(parent),
    _defaultModel(),
    _customListModel(nullptr),
    _currentIndex(-1),
    _defaultIndex(0)
{
    setText(title);
    setOptions(options);
    setCurrentText(defaultOption);
    setDefaultText(defaultOption);
}

QStringList OptionAction::getOptions() const
{
    QStringList options;

    for (int rowIndex = 0; rowIndex < getModel()->rowCount(); ++rowIndex)
        options << getModel()->index(rowIndex, 0).data(Qt::DisplayRole).toString();

    return options;
}

bool OptionAction::hasOptions() const
{
    return !getOptions().isEmpty();
}

void OptionAction::setOptions(const QStringList& options)
{
    _defaultModel.setStringList(options);

    emit optionsChanged(getOptions());
}

const QAbstractListModel* OptionAction::getModel() const
{
    if (_customListModel != nullptr)
        return _customListModel;

    return &_defaultModel;
}

void OptionAction::setCustomListModel(QAbstractListModel* customListModel)
{
    if (customListModel == _customListModel)
        return;

    _customListModel = customListModel;

    emit customListModelChanged(_customListModel);
}

bool OptionAction::hasCustomListModel() const
{
    return _customListModel != nullptr;
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

QString OptionAction::getDefaultText() const
{
    return getOptions()[_defaultIndex];
}

void OptionAction::setDefaultText(const QString& defaultText)
{
    _defaultIndex = getOptions().indexOf(defaultText);
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
    if (_currentIndex < 0)
        return "";

    return getOptions()[_currentIndex];
}

void OptionAction::setCurrentText(const QString& currentText)
{
    const auto options = getOptions();

    if (!options.contains(currentText))
        return;

    _currentIndex = options.indexOf(currentText);

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
    _layout->setMargin(0);
    _layout->addWidget(_comboBox, 1);

    setLayout(_layout);

    const auto updateToolTip = [this, optionAction]() -> void {
        _comboBox->setToolTip(optionAction->hasOptions() ? QString("%1: %2").arg(optionAction->toolTip(), optionAction->getCurrentText()) : optionAction->toolTip());
    };

    const auto populateComboBox = [this, optionAction, updateToolTip]() -> void {
        QSignalBlocker comboBoxSignalBlocker(_comboBox);

        _comboBox->setModel(new QStringListModel());
        _comboBox->setModel(const_cast<QAbstractListModel*>(optionAction->getModel()));
        _comboBox->setEnabled(!optionAction->getOptions().isEmpty());

        updateToolTip();
    };

    connect(optionAction, &OptionAction::optionsChanged, this, [this, populateComboBox](const QStringList& options) {
        populateComboBox();
    });

    const auto updateComboBoxSelection = [this, optionAction]() -> void {
        if (optionAction->getCurrentText() == _comboBox->currentText())
            return;
        
        QSignalBlocker comboBoxSignalBlocker(_comboBox);
        
        _comboBox->setCurrentText(optionAction->getCurrentText());
    };

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