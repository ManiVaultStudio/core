#include "OptionsAction.h"
#include "Application.h"

#include <QDebug>
#include <QHBoxLayout>
#include <QStylePainter>

namespace hdps {

namespace gui {

OptionsAction::OptionsAction(QObject* parent, const QString& title /*= ""*/, const QStringList& options /*= QStringList()*/, const QStringList& selectedOptions /*= QStringList()*/) :
    WidgetAction(parent),
    _optionsModel()
{
    setText(title);
    setDefaultWidgetFlags(WidgetFlag::Default);
    initialize(options, selectedOptions);

    connect(&_optionsModel, &QStandardItemModel::dataChanged, this, [this]() -> void {
        selectedOptionsChanged(getSelectedOptions());
    });
}

QString OptionsAction::getTypeString() const
{
    return "Options";
}

void OptionsAction::initialize(const QStringList& options /*= QStringList()*/, const QStringList& selectedOptions /*= QStringList()*/)
{
    setOptions(options);
    setSelectedOptions(selectedOptions);
}

QStringList OptionsAction::getOptions() const
{
    QStringList options;

    for (int i = 0; i < _optionsModel.rowCount(); i++)
        options << _optionsModel.item(i, 0)->text();

    return options;
}

const QStandardItemModel& OptionsAction::getOptionsModel() const
{
    return _optionsModel;
}

std::uint32_t OptionsAction::getNumberOfOptions() const
{
    return _optionsModel.rowCount();
}

bool OptionsAction::hasOption(const QString& option) const
{
    return _optionsModel.match(_optionsModel.index(0, 0), Qt::DisplayRole, option).count() == 1;
}

bool OptionsAction::hasOptions() const
{
    return _optionsModel.rowCount() >= 1;
}

void OptionsAction::setOptions(const QStringList& options)
{
    _optionsModel.clear();

    for (const auto& option : options) {
        const auto row = _optionsModel.rowCount();

        auto item = new QStandardItem();

        item->setText(option);
        item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        item->setData(Qt::Unchecked, Qt::CheckStateRole);

        _optionsModel.setItem(row, 0, item);
    }

    emit optionsChanged(getOptions());
}

QStringList OptionsAction::getSelectedOptions() const
{
    QStringList selectedOptions;

    for (int i = 0; i < _optionsModel.rowCount(); i++)
        if (_optionsModel.item(i, 0)->checkState() == Qt::Checked)
            selectedOptions << _optionsModel.item(i, 0)->text();

    return selectedOptions;
}

bool OptionsAction::isOptionSelected(const QString& option) const
{
    const auto matches = _optionsModel.match(_optionsModel.index(0, 0), Qt::DisplayRole, option);

    if (matches.isEmpty())
        return false;

    return _optionsModel.item(matches.first().row())->data(Qt::CheckStateRole).toBool();
}

bool OptionsAction::hasSelectedOptions() const
{
    for (int i = 0; i < _optionsModel.rowCount(); i++)
        if (_optionsModel.item(i, 0)->checkState() == Qt::Checked)
            return true;

    return false;
}

void OptionsAction::selectOption(const QString& option, const bool& replaceSelection /*= false*/)
{
    const auto matches = _optionsModel.match(_optionsModel.index(0, 0), Qt::DisplayRole, option);

    if (!matches.isEmpty())
        _optionsModel.item(matches.first().row())->setData(Qt::Checked, Qt::CheckStateRole);
}

void OptionsAction::setSelectedOptions(const QStringList& selectedOptions)
{
    for (int i = 0; i < selectedOptions.count(); i++)
        selectOption(selectedOptions.at(i));

    emit selectedOptionsChanged(getSelectedOptions());
}

bool OptionsAction::mayPublish() const
{
    return true;
}

void OptionsAction::connectToPublicAction(WidgetAction* publicAction)
{
    /*
    auto publicOptionAction = dynamic_cast<OptionAction*>(publicAction);

    Q_ASSERT(publicOptionAction != nullptr);

    connect(this, &OptionAction::currentIndexChanged, publicOptionAction, &OptionAction::setCurrentIndex);
    connect(publicOptionAction, &OptionAction::currentIndexChanged, this, &OptionAction::setCurrentIndex);

    setCurrentIndex(publicOptionAction->getCurrentIndex());

    WidgetAction::connectToPublicAction(publicAction);
    */
}

void OptionsAction::disconnectFromPublicAction()
{
    /*
    auto publicOptionAction = dynamic_cast<OptionAction*>(_publicAction);

    Q_ASSERT(publicOptionAction != nullptr);

    disconnect(this, &OptionAction::currentIndexChanged, publicOptionAction, &OptionAction::setCurrentIndex);
    disconnect(publicOptionAction, &OptionAction::currentIndexChanged, this, &OptionAction::setCurrentIndex);

    WidgetAction::disconnectFromPublicAction();
    */
}

WidgetAction* OptionsAction::getPublicCopy() const
{
    return new OptionsAction(parent(), text(), getOptions(), getSelectedOptions());
}

void OptionsAction::fromVariantMap(const QVariantMap& variantMap)
{
}

QVariantMap OptionsAction::toVariantMap() const
{
    return {};
}

OptionsAction::ComboBoxWidget::ComboBoxWidget(QWidget* parent, OptionsAction* optionsAction) :
    QComboBox(parent),
    _optionsAction(optionsAction)
{
    setObjectName("ComboBox");
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setModel(&const_cast<QStandardItemModel&>(optionsAction->getOptionsModel()));

    const auto updateToolTip = [this, optionsAction]() -> void {
        setToolTip("Selected: " + optionsAction->getSelectedOptions().join(", "));
    };

    connect(optionsAction, &OptionsAction::selectedOptionsChanged, this, [this, updateToolTip]() -> void {
        update();
        updateToolTip();
    });

    updateToolTip();
}

void OptionsAction::ComboBoxWidget::paintEvent(QPaintEvent* paintEvent)
{
    auto painter = QSharedPointer<QStylePainter>::create(this);

    painter->setPen(palette().color(QPalette::Text));

    auto styleOptionComboBox = QStyleOptionComboBox();

    initStyleOption(&styleOptionComboBox);

    painter->drawComplexControl(QStyle::CC_ComboBox, styleOptionComboBox);

    styleOptionComboBox.palette.setBrush(QPalette::ButtonText, styleOptionComboBox.palette.brush(QPalette::ButtonText).color().lighter());

    const auto selectedOptions = _optionsAction->getSelectedOptions();

    styleOptionComboBox.currentText = selectedOptions.isEmpty() ? "None selected" : selectedOptions.join(", ");

    painter->drawControl(QStyle::CE_ComboBoxLabel, styleOptionComboBox);
}

QWidget* OptionsAction::getWidget(QWidget* parent, const std::int32_t& widgetFlags)
{
    auto widget = new WidgetActionWidget(parent, this);
    auto layout = new QHBoxLayout();

    layout->setMargin(0);

    if (widgetFlags & WidgetFlag::ComboBox)
        layout->addWidget(new OptionsAction::ComboBoxWidget(parent, this));

    //if (widgetFlags & WidgetFlag::ListView)
    //    layout->addWidget(new OptionAction::ListViewWidget(parent, this));

    widget->setLayout(layout);

    return widget;
}

}
}
