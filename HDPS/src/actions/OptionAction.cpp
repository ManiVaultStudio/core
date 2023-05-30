#include "OptionAction.h"
#include "Application.h"

#include "util/Serialization.h"

#include <QDebug>
#include <QHBoxLayout>
#include <QComboBox>
#include <QListView>
#include <QStylePainter>

using namespace hdps::util;

namespace hdps::gui {

OptionAction::OptionAction(QObject* parent, const QString& title /*= ""*/, const QStringList& options /*= QStringList()*/, const QString& currentOption /*= ""*/, const QString& defaultOption /*= ""*/) :
    WidgetAction(parent, title),
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
}

void OptionAction::initialize(QAbstractItemModel& customModel, const QString& currentOption /*= ""*/, const QString& defaultOption /*= ""*/)
{
    setCustomModel(&customModel);
    setCurrentText(currentOption);
    setDefaultText(defaultOption);
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

    const auto oldCurrentIndex  = _currentIndex;
    const auto oldCurrentText   = getCurrentText();

    if (_defaultModel.rowCount() == options.count()) {
        for (std::int32_t rowIndex = 0; rowIndex < _defaultModel.rowCount(); rowIndex++)
            _defaultModel.setData(_defaultModel.index(rowIndex, 0), options.at(rowIndex));
    }
    else {
        _defaultModel.setStringList(options);

        updateCurrentIndex();
    }

    emit modelChanged();

    emit currentIndexChanged(_currentIndex);
    emit currentTextChanged(getCurrentText());
}

const QAbstractItemModel* OptionAction::getModel() const
{
    if (_customModel != nullptr)
        return _customModel;

    return &_defaultModel;
}

void OptionAction::connectToPublicAction(WidgetAction* publicAction, bool recursive)
{
    auto publicOptionAction = dynamic_cast<OptionAction*>(publicAction);

    Q_ASSERT(publicOptionAction != nullptr);

    const auto currentTextChanged = [this, publicOptionAction](const QString& currentText) -> void {
        if (currentText.isEmpty())
            return;

        publicOptionAction->setCurrentText(currentText);
    };

    connect(this, &OptionAction::currentTextChanged, publicOptionAction, currentTextChanged);
    
    connect(publicOptionAction, &OptionAction::currentTextChanged, this, [this, publicOptionAction, currentTextChanged](const QString& currentText) -> void {
        disconnect(this, &OptionAction::currentTextChanged, publicOptionAction, nullptr);
        {
            if (hasOption(currentText))
                setCurrentText(currentText);
            else
                setCurrentIndex(-1);
        }
        connect(this, &OptionAction::currentTextChanged, publicOptionAction, currentTextChanged);
    });

    const auto updatePublicOptions = [this, publicOptionAction]() -> void {
        auto publicOptions = publicOptionAction->getOptions();

        publicOptions << getOptions();

        publicOptions.removeDuplicates();

        publicOptionAction->setOptions(publicOptions);
    };

    updatePublicOptions();

    connect(this, &OptionAction::modelChanged, this, updatePublicOptions);

    setCurrentText(publicOptionAction->getCurrentText());

    WidgetAction::connectToPublicAction(publicAction, recursive);
}

void OptionAction::disconnectFromPublicAction(bool recursive)
{
    if (!isConnected())
        return;

    auto publicOptionAction = dynamic_cast<OptionAction*>(getPublicAction());

    Q_ASSERT(publicOptionAction != nullptr);

    if (publicOptionAction == nullptr)
        return;

    disconnect(this, &OptionAction::currentTextChanged, publicOptionAction, nullptr);
    disconnect(this, &OptionAction::modelChanged, this, nullptr);
    disconnect(publicOptionAction, &OptionAction::currentTextChanged, this, nullptr);

    WidgetAction::disconnectFromPublicAction(recursive);
}

void OptionAction::fromVariantMap(const QVariantMap& variantMap)
{
    WidgetAction::fromVariantMap(variantMap);

    variantMapMustContain(variantMap, "Value");
    variantMapMustContain(variantMap, "IsPublic");

    if (variantMap["IsPublic"].toBool())
        setOptions(variantMap["Options"].toStringList());

    setCurrentText(variantMap["Value"].toString());
}

QVariantMap OptionAction::toVariantMap() const
{
    auto variantMap = WidgetAction::toVariantMap();

    variantMap.insert({
        { "Value", getCurrentText() }
    });

    if (isPublic()) {
        variantMap.insert({
            { "Options", getOptions() }
        });
    }

    return variantMap;
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

    emit customModelChanged(_customModel);
    emit modelChanged();

    if (_customModel)
        connect(_customModel, &QAbstractItemModel::layoutChanged, this, &OptionAction::updateCurrentIndex);

    updateCurrentIndex();

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

    updateCurrentIndex();

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
    if (getOptions().isEmpty())
        return "";

    if (_defaultIndex < 0)
        return "";

    if (_defaultIndex >= getOptions().count())
        return "";

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
    if (currentText == getCurrentText())
        return;

    _currentIndex = hasOption(currentText) ? getModel()->match(getModel()->index(0, 0), Qt::DisplayRole, currentText).first().row() : -1;

    emit currentTextChanged(getCurrentText());
    emit currentIndexChanged(_currentIndex);
}

bool OptionAction::hasSelection() const
{
    return _currentIndex >= 0;
}

OptionAction::ComboBoxWidget::ComboBoxWidget(QWidget* parent, OptionAction* optionAction) :
    QComboBox(parent),
    _optionAction(optionAction)
{
    setObjectName("ComboBox");
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    const auto updateToolTip = [this, optionAction]() -> void {
        setToolTip(optionAction->hasOptions() ? QString("%1: %2").arg(optionAction->toolTip(), optionAction->getCurrentText()) : optionAction->toolTip());
    };

    const auto updateReadOnlyAndSelection = [this, optionAction]() -> void {
        if (optionAction->getCurrentIndex() < 0)
            setEnabled(optionAction->getNumberOfOptions() >= 1);
        else
            setEnabled(optionAction->getNumberOfOptions() >= 2);

        setCurrentText(optionAction->getCurrentText());
    };

    const auto updateComboBoxModel = [this, optionAction, updateToolTip, updateReadOnlyAndSelection]() -> void {
        QSignalBlocker comboBoxSignalBlocker(this);

        if (model()) {
            disconnect(model(), &QAbstractItemModel::layoutChanged, this, nullptr);
            disconnect(model(), &QAbstractItemModel::rowsInserted, this, nullptr);
            disconnect(model(), &QAbstractItemModel::rowsRemoved, this, nullptr);
        }

        setModel(const_cast<QAbstractItemModel*>(optionAction->getModel()));

        connect(model(), &QAbstractItemModel::layoutChanged, this, updateReadOnlyAndSelection);
        connect(model(), &QAbstractItemModel::rowsInserted, this, updateReadOnlyAndSelection);
        connect(model(), &QAbstractItemModel::rowsRemoved, this, updateReadOnlyAndSelection);

        updateToolTip();

        updateReadOnlyAndSelection();
    };

    connect(optionAction, &OptionAction::modelChanged, this, updateComboBoxModel);

    const auto updateComboBoxSelection = [this, optionAction]() -> void {
        if (optionAction->getCurrentText() == currentText())
            return;

        QSignalBlocker comboBoxSignalBlocker(this);

        setCurrentText(optionAction->getCurrentText());
    };

    connect(optionAction, &OptionAction::currentTextChanged, this, [this, updateComboBoxSelection, updateToolTip](const QString& currentText) {
        QSignalBlocker comboBoxSignalBlocker(this);

        updateComboBoxSelection();
        updateToolTip();

        update();
    });

    connect(optionAction, &OptionAction::placeholderStringChanged, this, [this]() -> void {
        update();
    });

    connect(this, qOverload<int>(&QComboBox::activated), this, [this, optionAction](const int& currentIndex) {
        optionAction->setCurrentIndex(currentIndex);
    });

    updateReadOnlyAndSelection();
    updateComboBoxModel();
    updateComboBoxSelection();
    updateToolTip();
}

void OptionAction::ComboBoxWidget::paintEvent(QPaintEvent* paintEvent)
{
    auto painter = QSharedPointer<QStylePainter>::create(this);

    painter->setPen(palette().color(QPalette::Text));

    auto styleOptionComboBox = QStyleOptionComboBox();

    initStyleOption(&styleOptionComboBox);

    painter->drawComplexControl(QStyle::CC_ComboBox, styleOptionComboBox);

    if (_optionAction->getCurrentIndex() < 0) {
        styleOptionComboBox.palette.setBrush(QPalette::ButtonText, styleOptionComboBox.palette.brush(QPalette::ButtonText).color().lighter());

        if (!_optionAction->getPlaceholderString().isEmpty())
            styleOptionComboBox.currentText = _optionAction->getPlaceholderString();
    }

    painter->drawControl(QStyle::CE_ComboBoxLabel, styleOptionComboBox);
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

    const auto updateCompleterModel = [this, optionAction]() {
        _completer.setModel(const_cast<QAbstractItemModel*>(optionAction->getModel()));
    };

    connect(optionAction, &OptionAction::modelChanged, this, updateCompleterModel);

    const auto updateText = [this, optionAction]() -> void {
        QSignalBlocker lineEditBlocker(this);

        setText(optionAction->getCurrentText());
    };

    connect(optionAction, &OptionAction::currentTextChanged, this, updateText);

    connect(this, &QLineEdit::editingFinished, this, [this, optionAction]() {
        optionAction->setCurrentText(text());
    });

    connect(&_completer, QOverload<const QString&>::of(&QCompleter::activated), [this, optionAction](const QString& text) {
        optionAction->setCurrentText(text);
    });

    connect(&_completer, QOverload<const QString&>::of(&QCompleter::highlighted), [this, optionAction](const QString& text) {
        optionAction->setCurrentText(text);
    });

    updateCompleterModel();
    updateText();
}

OptionAction::ButtonsWidget::ButtonsWidget(QWidget* parent, OptionAction* optionAction, const Qt::Orientation& orientation) :
    QWidget(parent)
{
    setObjectName("Buttons");

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

    layout->setContentsMargins(0, 0, 0, 0);

    std::int32_t optionIndex = 0;

    for (const auto& option : optionAction->getOptions()) {
        auto optionPushButton = new QPushButton(option);

        optionPushButton->setToolTip(optionAction->text() + ": " + option);

        layout->addWidget(optionPushButton);

        connect(optionPushButton, &QPushButton::clicked, this, [optionAction, optionIndex]() {
            optionAction->setCurrentIndex(optionIndex);
        });

        optionIndex++;
    }

    setLayout(layout);
}

QWidget* OptionAction::getWidget(QWidget* parent, const std::int32_t& widgetFlags)
{
    auto widget = new WidgetActionWidget(parent, this);
    auto layout = new QHBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);

    if (widgetFlags & WidgetFlag::ComboBox)
        layout->addWidget(new OptionAction::ComboBoxWidget(parent, this));

    if (widgetFlags & WidgetFlag::LineEdit)
        layout->addWidget(new OptionAction::LineEditWidget(parent, this));

    if (widgetFlags & WidgetFlag::HorizontalButtons)
        layout->addWidget(new ButtonsWidget(parent, this, Qt::Horizontal));

    if (widgetFlags & WidgetFlag::VerticalButtons)
        layout->addWidget(new ButtonsWidget(parent, this, Qt::Vertical));

    if (widgetFlags & WidgetFlag::Clearable) {
        auto clearSelectionAction = new TriggerAction(parent, "Clear");

        clearSelectionAction->setIcon(Application::getIconFont("FontAwesome").getIcon("times"));
        clearSelectionAction->setToolTip("Clear the current selection");

        connect(clearSelectionAction, &TriggerAction::triggered, this, [this]() -> void {
            setCurrentIndex(-1);
        });

        const auto updateReadOnly = [this, clearSelectionAction]() -> void {
            clearSelectionAction->setEnabled(getCurrentIndex() >= 0);
        };

        updateReadOnly();

        connect(this, &OptionAction::currentIndexChanged, this, updateReadOnly);

        layout->addWidget(clearSelectionAction->createWidget(widget, TriggerAction::Icon));
    }

    widget->setLayout(layout);

    return widget;
}

}
