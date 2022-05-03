#include "OptionsAction.h"
#include "Application.h"

#include <QDebug>
#include <QHBoxLayout>
#include <QStylePainter>
#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

namespace hdps {

namespace gui {

OptionsAction::OptionsAction(QObject* parent, const QString& title /*= ""*/, const QStringList& options /*= QStringList()*/, const QStringList& selectedOptions /*= QStringList()*/) :
    WidgetAction(parent),
    _optionsModel(),
    _selectionAction(*this),
    _fileAction(*this)
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
    if (selectedOptions == getSelectedOptions())
        return;

    QSignalBlocker optionsModelBlocker(&_optionsModel);

    for (int i = 0; i < _optionsModel.rowCount(); i++)
        _optionsModel.item(i, 0)->setData(Qt::Unchecked, Qt::CheckStateRole);

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
    auto publicOptionsAction = dynamic_cast<OptionsAction*>(publicAction);

    Q_ASSERT(publicOptionsAction != nullptr);

    connect(this, &OptionsAction::selectedOptionsChanged, publicOptionsAction, &OptionsAction::setSelectedOptions);
    connect(publicOptionsAction, &OptionsAction::selectedOptionsChanged, this, &OptionsAction::setSelectedOptions);

    setSelectedOptions(publicOptionsAction->getSelectedOptions());

    WidgetAction::connectToPublicAction(publicAction);
}

void OptionsAction::disconnectFromPublicAction()
{
    auto publicOptionsAction = dynamic_cast<OptionsAction*>(_publicAction);

    Q_ASSERT(publicOptionsAction != nullptr);

    disconnect(this, &OptionsAction::selectedOptionsChanged, publicOptionsAction, &OptionsAction::setSelectedOptions);
    disconnect(publicOptionsAction, &OptionsAction::selectedOptionsChanged, this, &OptionsAction::setSelectedOptions);

    WidgetAction::disconnectFromPublicAction();
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
    layout->setSpacing(3);

    if (widgetFlags & WidgetFlag::ComboBox)
        layout->addWidget(new OptionsAction::ComboBoxWidget(parent, this));

    //if (widgetFlags & WidgetFlag::ListView)
    //    layout->addWidget(new OptionAction::ListViewWidget(parent, this));

    if (widgetFlags & WidgetFlag::Selection)
        layout->addWidget(_selectionAction.createCollapsedWidget(parent));

    if (widgetFlags & WidgetFlag::File)
        layout->addWidget(_fileAction.createCollapsedWidget(parent));

    widget->setLayout(layout);

    return widget;
}

OptionsAction::SelectionAction::SelectionAction(OptionsAction& optionsAction) :
    WidgetAction(&optionsAction),
    _optionsAction(optionsAction),
    _selectAllAction(this, "All"),
    _clearSelectionAction(this, "Clear"),
    _invertSelectionAction(this, "Invert")
{
    auto& fontAwesome = Application::getIconFont("FontAwesome");

    setText("Selection");
    setToolTip("Change selection");
    setIcon(fontAwesome.getIcon("mouse-pointer"));

    const auto updateReadOnly = [this]() -> void {
        _selectAllAction.setEnabled(_optionsAction.getSelectedOptions().count() < _optionsAction.getOptions().count());
        _clearSelectionAction.setEnabled(_optionsAction.getSelectedOptions().count() >= 1);
    };

    connect(&_optionsAction, &OptionsAction::selectedOptionsChanged, this, updateReadOnly);

    connect(&_selectAllAction, &TriggerAction::triggered, this, [this]() -> void {
        _optionsAction.setSelectedOptions(_optionsAction.getOptions());
    });

    connect(&_clearSelectionAction, &TriggerAction::triggered, this, [this]() -> void {
        _optionsAction.setSelectedOptions(QStringList());
    });

    connect(&_invertSelectionAction, &TriggerAction::triggered, this, [this]() -> void {
        auto invertedSelectedOptions = _optionsAction.getOptions();

        for (const auto& selectedOption : _optionsAction.getSelectedOptions())
            invertedSelectedOptions.removeOne(selectedOption);

        _optionsAction.setSelectedOptions(invertedSelectedOptions);
    });
}

OptionsAction::SelectionAction::Widget::Widget(QWidget* parent, SelectionAction* selectionAction) :
    WidgetActionWidget(parent, selectionAction)
{
    auto layout = new QHBoxLayout();

    layout->addWidget(selectionAction->getSelectAllAction().createWidget(this));
    layout->addWidget(selectionAction->getClearSelectionAction().createWidget(this));
    layout->addWidget(selectionAction->getInvertSelectionAction().createWidget(this));

    setPopupLayout(layout);
}

OptionsAction::FileAction::FileAction(OptionsAction& optionsAction) :
    WidgetAction(&optionsAction),
    _optionsAction(optionsAction),
    _loadSelectionAction(this, "Load selection"),
    _saveSelectionAction(this, "Save selection")
{
    auto& fontAwesome = Application::getIconFont("FontAwesome");

    setText("File");
    setToolTip("Load/save selection");
    setIcon(fontAwesome.getIcon("file"));

    const auto updateReadOnly = [this]() -> void {
        _loadSelectionAction.setEnabled(_optionsAction.getOptions().count() >= 1);
        _saveSelectionAction.setEnabled(_optionsAction.getOptions().count() >= 1);
    };

    connect(&_optionsAction, &OptionsAction::selectedOptionsChanged, this, updateReadOnly);

    connect(&_loadSelectionAction, &TriggerAction::triggered, this, [this]() -> void {
        QFileDialog fileDialog;

        fileDialog.setWindowTitle("Open selection from file");
        fileDialog.setWindowIcon(Application::getIconFont("FontAwesome").getIcon("folder-open"));
        fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
        fileDialog.setFileMode(QFileDialog::ExistingFile);
        fileDialog.setNameFilters({ "Selection files (*.json)" });
        fileDialog.setDefaultSuffix(".json");
        //fileDialog.setDirectory(ApplicationgetSetting("Projects/WorkingDirectory", QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation)).toString());

        if (fileDialog.exec() == 0)
            return;

        if (fileDialog.selectedFiles().count() != 1)
            throw std::runtime_error("Only one file may be selected");

        QFile jsonFile(fileDialog.selectedFiles().first());

        jsonFile.open(QFile::ReadOnly);

        const auto jsonDocument = QJsonDocument::fromJson(jsonFile.readAll());
        const auto variantMap = jsonDocument.toVariant().toMap();
        const auto selection = variantMap["selection"].toStringList();

        _optionsAction.setSelectedOptions(selection);

        _loadSelectionAction.setEnabled(false);
    });

    connect(&_saveSelectionAction, &TriggerAction::triggered, this, [this]() -> void {
        QFileDialog fileDialog;

        fileDialog.setWindowTitle("Save selection to file");
        fileDialog.setWindowIcon(Application::getIconFont("FontAwesome").getIcon("save"));
        fileDialog.setAcceptMode(QFileDialog::AcceptSave);
        fileDialog.setNameFilters({ "Selection files (*.json)" });
        fileDialog.setDefaultSuffix(".json");
        fileDialog.setOption(QFileDialog::DontUseNativeDialog, true);
        //fileDialog.setDirectory(getSetting("Projects/WorkingDirectory", QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation)).toString());

        fileDialog.exec();

        if (fileDialog.selectedFiles().count() != 1)
            throw std::runtime_error("Only one file may be selected");

        QJsonDocument jsonDocument;

        QJsonObject jsonObject;

        jsonObject.insert("selection", QJsonArray::fromStringList(_optionsAction.getSelectedOptions()));

        jsonDocument.setObject(jsonObject);

        QFile jsonFile(fileDialog.selectedFiles().first());

        jsonFile.open(QFile::WriteOnly);
        jsonFile.write(jsonDocument.toJson());

        _saveSelectionAction.setEnabled(false);
    });

    updateReadOnly();
}

OptionsAction::FileAction::Widget::Widget(QWidget* parent, FileAction* fileAction) :
    WidgetActionWidget(parent, fileAction)
{
    auto layout = new QHBoxLayout();

    layout->addWidget(fileAction->getLoadSelectionAction().createWidget(this));
    layout->addWidget(fileAction->getSaveSelectionAction().createWidget(this));

    setPopupLayout(layout);
}

}
}
