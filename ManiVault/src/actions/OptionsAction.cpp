// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "OptionsAction.h"
#include "Application.h"

#include <QDebug>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QAbstractItemView>
#include <QMouseEvent>
#include <QListView>

using namespace mv::util;

namespace mv::gui {

OptionsAction::OptionsAction(QObject* parent, const QString& title, const QStringList& options /*= QStringList()*/, const QStringList& selectedOptions /*= QStringList()*/) :
    WidgetAction(parent, title),
    _optionsModel({}),
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

void OptionsAction::initialize(const QStringList& options /*= QStringList()*/, const QStringList& selectedOptions /*= QStringList()*/)
{
    setOptions(options);
    setSelectedOptions(selectedOptions);
}

QStringList OptionsAction::getOptions() const
{
    return _optionsModel.stringList();
}

const CheckableStringListModel& OptionsAction::getOptionsModel() const
{
    return _optionsModel;
}

CheckableStringListModel& OptionsAction::getOptionsModel()
{
    return _optionsModel;
}

std::uint32_t OptionsAction::getNumberOfOptions() const
{
    return _optionsModel.rowCount();
}

bool OptionsAction::hasOption(const QString& option) const
{
    return _optionsModel.match(_optionsModel.index(0, 0), Qt::DisplayRole, option).count() >= 1;
}

bool OptionsAction::hasOptions() const
{
    return _optionsModel.rowCount() >= 1;
}

void OptionsAction::setOptions(const QStringList& options, bool clearSelection /*= false*/)
{
    const auto selectedOptions = getSelectedOptions();

    _optionsModel.setStringList(options);

    for (std::int32_t rowIndex = 0; rowIndex < options.count(); ++rowIndex) {

        auto index = _optionsModel.index(rowIndex, 0, QModelIndex());

        _optionsModel.setData(index, clearSelection ? Qt::Unchecked : (selectedOptions.contains(options.at(rowIndex)) ? Qt::Checked : Qt::Unchecked), Qt::CheckStateRole);
    }

    emit optionsChanged(getOptions());
}

QStringList OptionsAction::getSelectedOptions() const
{
    return _optionsModel.getCheckedStrings();
}

QList<std::int32_t> OptionsAction::getSelectedOptionIndices() const
{
    return _optionsModel.getCheckedIndicesList();
}

bool OptionsAction::isOptionSelected(const QString& option) const
{
    const auto matches = _optionsModel.match(_optionsModel.index(0, 0), Qt::DisplayRole, option);

    if (matches.isEmpty())
        return false;

    return matches.first().data(Qt::CheckStateRole).toInt() == Qt::Checked;
}

bool OptionsAction::hasSelectedOptions() const
{
    for (std::int32_t optionIndex = 0; optionIndex < _optionsModel.rowCount(); optionIndex++)
        if (_optionsModel.index(optionIndex, 0).data(Qt::CheckStateRole).toInt() == Qt::Checked)
            return true;

    return false;
}

void OptionsAction::selectOption(const QString& option, bool unselect /*= false*/)
{
    const auto matches = _optionsModel.match(_optionsModel.index(0, 0), Qt::DisplayRole, option);

    if (!matches.isEmpty())
        _optionsModel.setData(matches.first(), unselect ? Qt::Unchecked : Qt::Checked, Qt::CheckStateRole);

    saveToSettings();
}

void OptionsAction::setSelectedOptions(const QStringList& selectedOptions)
{
    if (selectedOptions == getSelectedOptions())
        return;

    auto previousSelectedOptions = getSelectedOptions();

    QSignalBlocker optionsModelBlocker(&_optionsModel);

    auto selectionChanged = false;

    for (std::int32_t optionIndex = 0; optionIndex < _optionsModel.rowCount(); optionIndex++)
        _optionsModel.setData(_optionsModel.index(optionIndex, 0), selectedOptions.contains(_optionsModel.index(optionIndex, 0).data(Qt::EditRole).toString()) ? Qt::Checked : Qt::Unchecked, Qt::CheckStateRole);

    if (getSelectedOptions() != previousSelectedOptions)
        emit selectedOptionsChanged(getSelectedOptions());

    saveToSettings();
}

void OptionsAction::connectToPublicAction(WidgetAction* publicAction, bool recursive /*= true*/)
{
    auto publicOptionsAction = dynamic_cast<OptionsAction*>(publicAction);

    Q_ASSERT(publicOptionsAction != nullptr);

    if (publicOptionsAction == nullptr)
        return;

    const auto updatePublicOptions = [this, publicOptionsAction]() -> void {
        auto allOptions = publicOptionsAction->getOptions() + getOptions();

        allOptions.removeDuplicates();
        allOptions.sort();

        publicOptionsAction->setOptions(allOptions);
    };

    updatePublicOptions();

    const auto initializeSelectedOptions = [this, publicOptionsAction]() -> void {
        auto publicSelectedOptions = publicOptionsAction->getSelectedOptions();

        for (auto selectedOption : getSelectedOptions())
            publicSelectedOptions << selectedOption;

        setSelectedOptions(publicSelectedOptions);
    };

    initializeSelectedOptions();

    const auto initializePublicOptionsSelection = [this, publicOptionsAction]() -> void {
        auto publicSelectedOptions = publicOptionsAction->getSelectedOptions();

        for (auto selectedOption : getSelectedOptions())
            publicSelectedOptions << selectedOption;

        publicOptionsAction->setSelectedOptions(publicSelectedOptions);
    };

    initializePublicOptionsSelection();

    connect(this, &OptionsAction::selectedOptionsChanged, this, [this, publicOptionsAction]() -> void {
        auto publicSelectedOptions = publicOptionsAction->getSelectedOptions();

        for (auto option : getOptions())
            publicSelectedOptions.removeOne(option);

        for (auto selectedOption : getSelectedOptions())
            publicSelectedOptions << selectedOption;

        publicOptionsAction->setSelectedOptions(publicSelectedOptions);
    });

    connect(publicOptionsAction, &OptionsAction::selectedOptionsChanged, this, [this, publicOptionsAction, initializePublicOptionsSelection](const QStringList& selectedOptions) -> void {
        setSelectedOptions(selectedOptions);
    });

    connect(this, &OptionsAction::optionsChanged, this, updatePublicOptions);

    WidgetAction::connectToPublicAction(publicAction, recursive);
}

void OptionsAction::disconnectFromPublicAction(bool recursive)
{
    if (!isConnected())
        return;

    auto publicOptionsAction = dynamic_cast<OptionsAction*>(getPublicAction());

    Q_ASSERT(publicOptionsAction != nullptr);

    if (publicOptionsAction == nullptr)
        return;

    disconnect(this, &OptionsAction::selectedOptionsChanged, this, nullptr);
    disconnect(publicOptionsAction, &OptionsAction::selectedOptionsChanged, this, nullptr);
    disconnect(this, &OptionsAction::optionsChanged, this, nullptr);

    WidgetAction::disconnectFromPublicAction(recursive);
}

void OptionsAction::fromVariantMap(const QVariantMap& variantMap)
{
    WidgetAction::fromVariantMap(variantMap);

    variantMapMustContain(variantMap, "Value");
    variantMapMustContain(variantMap, "IsPublic");

    if (variantMap["IsPublic"].toBool())
        setOptions(variantMap["Options"].toStringList());

    setSelectedOptions(variantMap["Value"].toStringList());
}

QVariantMap OptionsAction::toVariantMap() const
{
    auto variantMap = WidgetAction::toVariantMap();

    variantMap.insert({
        { "Value", getSelectedOptions() }
    });

    if (isPublic()) {
        variantMap.insert({
            { "Options", getOptions() }
        });
    }

    return variantMap;
}

OptionsAction::ComboBoxWidget::ComboBoxWidget(QWidget* parent, OptionsAction* optionsAction, QCompleter* completer) :
    QComboBox(parent),
    _optionsAction(optionsAction),
    _view()
{
    setObjectName("ComboBox");
    setEditable(true);
    setCompleter(completer);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    setSizeAdjustPolicy(QComboBox::AdjustToContents);
    setModel(&optionsAction->getOptionsModel());
    setModelColumn(0);
    setInsertPolicy(QComboBox::NoInsert);
    setView(&_view);

    connect(optionsAction, &OptionsAction::selectedOptionsChanged, this, &ComboBoxWidget::updateCurrentText);
    connect(this, &QComboBox::activated, this, &ComboBoxWidget::updateCurrentText);
    connect(completer, QOverload<const QString&>::of(&QCompleter::activated), this, &ComboBoxWidget::updateCurrentText);
    connect(completer, QOverload<const QModelIndex&>::of(&QCompleter::activated), this, &ComboBoxWidget::updateCurrentText);

    updateCurrentText();
    
    this->installEventFilter(this);

    lineEdit()->installEventFilter(this);
}

void OptionsAction::ComboBoxWidget::updateCurrentText()
{
    const auto selectedOptions = _optionsAction->getSelectedOptions();

    QString text;

    if (selectedOptions.isEmpty())
        text = "None selected";
    else {
        if (selectedOptions.count() > 100)
            text = QString("%1 selected").arg(QString::number(selectedOptions.count()));
        else
            text = _optionsAction->getSelectedOptions().join(", ");
    }

    QFontMetrics metrics(lineEdit()->font());

    lineEdit()->setText(metrics.elidedText(text, Qt::ElideMiddle, lineEdit()->width()));
};

bool OptionsAction::ComboBoxWidget::eventFilter(QObject* target, QEvent* event)
{
    switch (event->type())
    {
        case QEvent::MouseButtonPress:
        {
            if (target == lineEdit())
                QTimer::singleShot(0, lineEdit(), &QLineEdit::selectAll);

            break;
        }

        case QEvent::Resize:
        {
            if (target == lineEdit())
                updateCurrentText();

            break;
        }

        case QEvent::FocusOut:
        {
            if (target == this)
                updateCurrentText();

            break;
        }

        default:
            break;
    }

    return QComboBox::eventFilter(target, event);
}

QWidget* OptionsAction::getWidget(QWidget* parent, const std::int32_t& widgetFlags)
{
    auto widget     = new WidgetActionWidget(parent, this, widgetFlags);
    auto layout     = new QHBoxLayout();
    auto completer  = new QCompleter();

    completer->setModel(&_optionsModel);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setFilterMode(Qt::MatchContains);
    completer->setPopup(new CheckableItemView(widget));

    if (!widget->isPopup())
        layout->setContentsMargins(0, 0, 0, 0);

    if (widgetFlags & WidgetFlag::ComboBox)
        layout->addWidget(new OptionsAction::ComboBoxWidget(parent, this, completer));

    if (widgetFlags & WidgetFlag::Selection)
        layout->addWidget(_selectionAction.createCollapsedWidget(parent));

    if (widgetFlags & WidgetFlag::File)
        layout->addWidget(_fileAction.createCollapsedWidget(parent));

    widget->setLayout(layout);

    return widget;
}

OptionsAction::SelectionAction::SelectionAction(OptionsAction& optionsAction) :
    WidgetAction(&optionsAction, "Selection"),
    _optionsAction(optionsAction),
    _selectAllAction(this, "All"),
    _clearSelectionAction(this, "Clear"),
    _invertSelectionAction(this, "Invert")
{
    auto& fontAwesome = Application::getIconFont("FontAwesome");

    setText("Selection");
    setToolTip("Change selection");
    setIconByName("mouse-pointer");

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

    setLayout(layout);
}

OptionsAction::FileAction::FileAction(OptionsAction& optionsAction) :
    WidgetAction(&optionsAction, "File"),
    _optionsAction(optionsAction),
    _loadSelectionAction(this, "Load selection"),
    _saveSelectionAction(this, "Save selection")
{
    setText("File");
    setToolTip("Load/save selection");
    setIconByName("file");

    const auto updateReadOnly = [this]() -> void {
        _loadSelectionAction.setEnabled(_optionsAction.getOptions().count() >= 1);
        _saveSelectionAction.setEnabled(_optionsAction.getOptions().count() >= 1);
    };

    connect(&_optionsAction, &OptionsAction::selectedOptionsChanged, this, updateReadOnly);

    connect(&_loadSelectionAction, &TriggerAction::triggered, this, [this]() -> void {
        auto* fileDialog = new QFileDialog();

        fileDialog->setWindowTitle("Open selection from file");
        fileDialog->setWindowIcon(Application::getIconFont("FontAwesome").getIcon("folder-open"));
        fileDialog->setAcceptMode(QFileDialog::AcceptOpen);
        fileDialog->setFileMode(QFileDialog::ExistingFile);
        fileDialog->setNameFilters({ "Selection files (*.json)" });
        fileDialog->setDefaultSuffix(".json");
        //fileDialog->setDirectory(ApplicationgetSetting("Projects/WorkingDirectory", QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation)).toString());

        connect(fileDialog, &QFileDialog::accepted, this, [this, fileDialog]() -> void {
            if (fileDialog->selectedFiles().count() != 1)
                throw std::runtime_error("Only one file may be selected");

            QFile jsonFile(fileDialog->selectedFiles().first());

            jsonFile.open(QFile::ReadOnly);

            const auto jsonDocument = QJsonDocument::fromJson(jsonFile.readAll());
            const auto variantMap = jsonDocument.toVariant().toMap();
            const auto selection = variantMap["selection"].toStringList();

            _optionsAction.setSelectedOptions(selection);

            _loadSelectionAction.setEnabled(false);
            });
        connect(fileDialog, &QFileDialog::finished, fileDialog, &QFileDialog::deleteLater);

        fileDialog->open();

    });

    connect(&_saveSelectionAction, &TriggerAction::triggered, this, [this]() -> void {
        auto* fileDialog = new QFileDialog();

        fileDialog->setWindowTitle("Save selection to file");
        fileDialog->setWindowIcon(Application::getIconFont("FontAwesome").getIcon("save"));
        fileDialog->setAcceptMode(QFileDialog::AcceptSave);
        fileDialog->setNameFilters({ "Selection files (*.json)" });
        fileDialog->setDefaultSuffix(".json");
        fileDialog->setOption(QFileDialog::DontUseNativeDialog, true);
        //fileDialog->setDirectory(getSetting("Projects/WorkingDirectory", QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation)).toString());

        connect(fileDialog, &QFileDialog::accepted, this, [this, fileDialog]() -> void {
            if (fileDialog->selectedFiles().count() != 1)
                throw std::runtime_error("Only one file may be selected");

            QJsonDocument jsonDocument;

            QJsonObject jsonObject;

            jsonObject.insert("selection", QJsonArray::fromStringList(_optionsAction.getSelectedOptions()));

            jsonDocument.setObject(jsonObject);

            QFile jsonFile(fileDialog->selectedFiles().first());

            jsonFile.open(QFile::WriteOnly);
            jsonFile.write(jsonDocument.toJson());

            _saveSelectionAction.setEnabled(false);
            });
        connect(fileDialog, &QFileDialog::finished, fileDialog, &QFileDialog::deleteLater);

        fileDialog->open();

    });

    updateReadOnly();
}

OptionsAction::FileAction::Widget::Widget(QWidget* parent, FileAction* fileAction) :
    WidgetActionWidget(parent, fileAction)
{
    auto layout = new QHBoxLayout();

    layout->addWidget(fileAction->getLoadSelectionAction().createWidget(this));
    layout->addWidget(fileAction->getSaveSelectionAction().createWidget(this));

    setLayout(layout);
}

void OptionsAction::CheckableItemView::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        const auto index = indexAt(event->pos());

        if (index.isValid()) {
            auto model = this->model();

            if (model) {
                const auto value = model->data(index, Qt::CheckStateRole);

                if (value.isValid()) {
                    model->setData(index, (static_cast<Qt::CheckState>(value.toInt()) == Qt::Unchecked) ? Qt::Checked : Qt::Unchecked, Qt::CheckStateRole);
                    event->accept();
                    
                    return;
                }
            }
        }
    }

    QAbstractItemView::mousePressEvent(event);
}

}