// SPDX-License-Identifier: LGPL-3.0-or-later

#include "PointDataStorageRecommendationDialog.h"

#include <Application.h>
#include <actions/OptionAction.h>
#include <actions/OptionsAction.h>
#include <actions/StringAction.h>
#include <actions/ToggleAction.h>
#include <actions/VerticalGroupAction.h>
#include <util/StyledIcon.h>

#include <QDialogButtonBox>
#include <QTextEdit>
#include <QVBoxLayout>

namespace mv::pointdata::storage
{

class PointDataStorageRecommendationDialog::Private
{
public:
    Private(PointDataStorageRecommendationDialog* dialog, const std::uint64_t rows, const std::uint64_t columns, const std::size_t elementSize) :
        settingsGroupAction(dialog, "Storage recommendation"),
        rowAccessAction(&settingsGroupAction, "Row access", false),
        rowAccessPatternAction(&settingsGroupAction, "Row pattern", { "Random", "Contiguous" }, { "Random" }),
        rowAccessFrequencyAction(&settingsGroupAction, "Row frequency", { "Incidental", "Frequent" }, "Incidental"),
        columnAccessAction(&settingsGroupAction, "Column access", false),
        columnAccessPatternAction(&settingsGroupAction, "Column pattern", { "Random", "Contiguous" }, { "Random" }),
        columnAccessFrequencyAction(&settingsGroupAction, "Column frequency", { "Incidental", "Frequent" }, "Incidental"),
        pointAccessAction(&settingsGroupAction, "Point access", false),
        pointAccessFrequencyAction(&settingsGroupAction, "Point frequency", { "Incidental", "Frequent" }, "Incidental"),
        memoryPreferenceAction(&settingsGroupAction, "Memory preference", { "Balanced", "Low memory" }, "Balanced"),
        recommendationAction(&settingsGroupAction, "Recommendation"),
        rows(rows),
        columns(columns),
        elementSize(elementSize)
    {
        settingsGroupAction.setShowLabels(true);
        settingsGroupAction.setDefaultWidgetFlag(mv::gui::GroupAction::NoMargins);
        rowAccessAction.setDefaultWidgetFlags(mv::gui::ToggleAction::CheckBox);
        rowAccessPatternAction.setDefaultWidgetFlags(mv::gui::OptionsAction::ComboBox);
        rowAccessFrequencyAction.setDefaultWidgetFlags(mv::gui::OptionAction::ComboBox);
        columnAccessAction.setDefaultWidgetFlags(mv::gui::ToggleAction::CheckBox);
        columnAccessPatternAction.setDefaultWidgetFlags(mv::gui::OptionsAction::ComboBox);
        columnAccessFrequencyAction.setDefaultWidgetFlags(mv::gui::OptionAction::ComboBox);
        pointAccessAction.setDefaultWidgetFlags(mv::gui::ToggleAction::CheckBox);
        pointAccessFrequencyAction.setDefaultWidgetFlags(mv::gui::OptionAction::ComboBox);
        memoryPreferenceAction.setDefaultWidgetFlags(mv::gui::OptionAction::ComboBox);
        recommendationAction.setDefaultWidgetFlags(mv::gui::StringAction::TextEdit);
        recommendationAction.setEnabled(false);

        settingsGroupAction.addAction(&rowAccessAction);
        settingsGroupAction.addAction(&rowAccessPatternAction);
        settingsGroupAction.addAction(&rowAccessFrequencyAction);
        settingsGroupAction.addAction(&columnAccessAction);
        settingsGroupAction.addAction(&columnAccessPatternAction);
        settingsGroupAction.addAction(&columnAccessFrequencyAction);
        settingsGroupAction.addAction(&pointAccessAction);
        settingsGroupAction.addAction(&pointAccessFrequencyAction);
        settingsGroupAction.addAction(&memoryPreferenceAction);
        settingsGroupAction.addAction(&recommendationAction, -1, [](mv::gui::WidgetAction*, QWidget* widget) {
            if (auto* textEdit = widget->findChild<QTextEdit*>(QStringLiteral("TextEdit")))
                textEdit->setFixedHeight(180);
        });

        const auto update = [this] { updateActions(); updateRecommendation(); };
        QObject::connect(&rowAccessAction, &mv::gui::ToggleAction::toggled, dialog, update);
        QObject::connect(&rowAccessPatternAction, &mv::gui::OptionsAction::selectedOptionsChanged, dialog, [this] { updateActions(); updateRecommendation(); });
        QObject::connect(&rowAccessFrequencyAction, &mv::gui::OptionAction::currentIndexChanged, dialog, [this] { updateActions(); updateRecommendation(); });
        QObject::connect(&columnAccessAction, &mv::gui::ToggleAction::toggled, dialog, update);
        QObject::connect(&columnAccessPatternAction, &mv::gui::OptionsAction::selectedOptionsChanged, dialog, [this] { updateActions(); updateRecommendation(); });
        QObject::connect(&columnAccessFrequencyAction, &mv::gui::OptionAction::currentIndexChanged, dialog, [this] { updateActions(); updateRecommendation(); });
        QObject::connect(&pointAccessAction, &mv::gui::ToggleAction::toggled, dialog, update);
        QObject::connect(&pointAccessFrequencyAction, &mv::gui::OptionAction::currentIndexChanged, dialog, [this] { updateActions(); updateRecommendation(); });
        QObject::connect(&memoryPreferenceAction, &mv::gui::OptionAction::currentIndexChanged, dialog, [this] { updateRecommendation(); });
        QObject::connect(dialog, &QDialog::accepted, dialog, [this] { saveSettings(); });
        loadSettings();
        updateActions();
        updateRecommendation();
    }

    void updateActions()
    {
        rowAccessPatternAction.setEnabled(rowAccessAction.isChecked());
        rowAccessFrequencyAction.setEnabled(rowAccessAction.isChecked());
        columnAccessPatternAction.setEnabled(columnAccessAction.isChecked());
        columnAccessFrequencyAction.setEnabled(columnAccessAction.isChecked());
        pointAccessFrequencyAction.setEnabled(pointAccessAction.isChecked());

        const auto balancedRecommendation = recommendationFor(StorageMemoryPreference::Balanced);
        const auto lowMemoryRecommendation = recommendationFor(StorageMemoryPreference::Low);
        const auto geometryDiffers = balancedRecommendation.geometry.rowTileSize != lowMemoryRecommendation.geometry.rowTileSize || balancedRecommendation.geometry.columnTileSize != lowMemoryRecommendation.geometry.columnTileSize;
        const auto cacheCapacityDiffers = balancedRecommendation.cacheCapacity != lowMemoryRecommendation.cacheCapacity;
        memoryPreferenceAction.setEnabled(geometryDiffers || cacheCapacityDiffers);
    }

    TiledStorageRecommendation recommendationFor(const StorageMemoryPreference memoryPreference) const
    {
        const auto rowAccess = rowAccessAction.isChecked() ? static_cast<StorageAxisAccess>((rowAccessPatternAction.getSelectedOptions().contains(QStringLiteral("Random")) ? static_cast<int>(StorageAxisAccess::Random) : 0) | (rowAccessPatternAction.getSelectedOptions().contains(QStringLiteral("Contiguous")) ? static_cast<int>(StorageAxisAccess::Contiguous) : 0)) : StorageAxisAccess::None;
        const auto rowFrequency = rowAccessAction.isChecked() ? static_cast<StorageAccessFrequency>(rowAccessFrequencyAction.getCurrentIndex()) : StorageAccessFrequency::Incidental;
        const auto columnAccess = columnAccessAction.isChecked() ? static_cast<StorageAxisAccess>((columnAccessPatternAction.getSelectedOptions().contains(QStringLiteral("Random")) ? static_cast<int>(StorageAxisAccess::Random) : 0) | (columnAccessPatternAction.getSelectedOptions().contains(QStringLiteral("Contiguous")) ? static_cast<int>(StorageAxisAccess::Contiguous) : 0)) : StorageAxisAccess::None;
        const auto columnFrequency = columnAccessAction.isChecked() ? static_cast<StorageAccessFrequency>(columnAccessFrequencyAction.getCurrentIndex()) : StorageAccessFrequency::Incidental;
        const auto pointFrequency = pointAccessAction.isChecked() ? static_cast<StorageAccessFrequency>(pointAccessFrequencyAction.getCurrentIndex()) : StorageAccessFrequency::Incidental;
        return recommendTiledStorage(rows, columns, elementSize, rowAccess, rowFrequency, columnAccess, columnFrequency, pointAccessAction.isChecked(), pointFrequency, memoryPreference);
    }

    void updateRecommendation()
    {
        const auto memoryPreference = memoryPreferenceAction.getCurrentIndex() == 1 ? StorageMemoryPreference::Low : StorageMemoryPreference::Balanced;
        recommendation = recommendationFor(memoryPreference);

        recommendationAction.setString(QStringLiteral("Tile geometry: %1 rows x %2 columns\nCache capacity: %3 tiles\nPacked tiles: yes\nSuper-tile: %4 tile rows x %5 tile columns\n\nThese are preliminary heuristics. Review the generated values and validate them with the benchmark.")
            .arg(recommendation.geometry.rowTileSize)
            .arg(recommendation.geometry.columnTileSize)
            .arg(recommendation.cacheCapacity)
            .arg(recommendation.superTileRows)
            .arg(recommendation.superTileColumns));
    }

    void saveSettings() const
    {
        auto* application = Application::current();

        application->setSetting("Developer/PointDataStorageRecommendationWizard/RowAccessEnabled", rowAccessAction.isChecked());
        application->setSetting("Developer/PointDataStorageRecommendationWizard/RowAccess", rowAccessPatternAction.getSelectedOptions());
        application->setSetting("Developer/PointDataStorageRecommendationWizard/RowAccessFrequency", rowAccessFrequencyAction.getCurrentText());
        application->setSetting("Developer/PointDataStorageRecommendationWizard/ColumnAccessEnabled", columnAccessAction.isChecked());
        application->setSetting("Developer/PointDataStorageRecommendationWizard/ColumnAccess", columnAccessPatternAction.getSelectedOptions());
        application->setSetting("Developer/PointDataStorageRecommendationWizard/ColumnAccessFrequency", columnAccessFrequencyAction.getCurrentText());
        application->setSetting("Developer/PointDataStorageRecommendationWizard/PointAccessEnabled", pointAccessAction.isChecked());
        application->setSetting("Developer/PointDataStorageRecommendationWizard/PointAccessFrequency", pointAccessFrequencyAction.getCurrentText());
        application->setSetting("Developer/PointDataStorageRecommendationWizard/MemoryPreference", memoryPreferenceAction.getCurrentText());
    }

    void loadSettings()
    {
        auto* application = Application::current();

        rowAccessAction.setChecked(application->getSetting("Developer/PointDataStorageRecommendationWizard/RowAccessEnabled", rowAccessAction.isChecked()).toBool());
        rowAccessPatternAction.setSelectedOptions(application->getSetting("Developer/PointDataStorageRecommendationWizard/RowAccess", rowAccessPatternAction.getSelectedOptions()).toStringList());
        rowAccessFrequencyAction.setCurrentText(application->getSetting("Developer/PointDataStorageRecommendationWizard/RowAccessFrequency", rowAccessFrequencyAction.getCurrentText()).toString());
        columnAccessAction.setChecked(application->getSetting("Developer/PointDataStorageRecommendationWizard/ColumnAccessEnabled", columnAccessAction.isChecked()).toBool());
        columnAccessPatternAction.setSelectedOptions(application->getSetting("Developer/PointDataStorageRecommendationWizard/ColumnAccess", columnAccessPatternAction.getSelectedOptions()).toStringList());
        columnAccessFrequencyAction.setCurrentText(application->getSetting("Developer/PointDataStorageRecommendationWizard/ColumnAccessFrequency", columnAccessFrequencyAction.getCurrentText()).toString());
        pointAccessAction.setChecked(application->getSetting("Developer/PointDataStorageRecommendationWizard/PointAccessEnabled", pointAccessAction.isChecked()).toBool());
        pointAccessFrequencyAction.setCurrentText(application->getSetting("Developer/PointDataStorageRecommendationWizard/PointAccessFrequency", pointAccessFrequencyAction.getCurrentText()).toString());
        memoryPreferenceAction.setCurrentText(application->getSetting("Developer/PointDataStorageRecommendationWizard/MemoryPreference", memoryPreferenceAction.getCurrentText()).toString());
    }

    mv::gui::VerticalGroupAction settingsGroupAction;
    mv::gui::ToggleAction       rowAccessAction;
    mv::gui::OptionsAction      rowAccessPatternAction;
    mv::gui::OptionAction       rowAccessFrequencyAction;
    mv::gui::ToggleAction       columnAccessAction;
    mv::gui::OptionsAction      columnAccessPatternAction;
    mv::gui::OptionAction       columnAccessFrequencyAction;
    mv::gui::ToggleAction       pointAccessAction;
    mv::gui::OptionAction       pointAccessFrequencyAction;
    mv::gui::OptionAction       memoryPreferenceAction;
    mv::gui::StringAction       recommendationAction;
    std::uint64_t                rows;
    std::uint64_t                columns;
    std::size_t                  elementSize;
    TiledStorageRecommendation   recommendation;
};

PointDataStorageRecommendationDialog::PointDataStorageRecommendationDialog(QWidget* parent, const std::uint64_t rows, const std::uint64_t columns, const std::size_t elementSize) :
    QDialog(parent),
    _private(new Private(this, rows, columns, elementSize))
{
    setWindowTitle(tr("Storage settings wizard"));
    setWindowIcon(mv::util::StyledIcon("lightbulb"));
    resize(560, 500);

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    auto* layout = new QVBoxLayout(this);
    layout->addWidget(_private->settingsGroupAction.createWidget(this));
    layout->addWidget(buttons);

    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

const TiledStorageRecommendation& PointDataStorageRecommendationDialog::recommendation() const
{
    return _private->recommendation;
}

QVariantMap PointDataStorageRecommendationDialog::profile() const
{
    return {
        { QStringLiteral("row_access_enabled"), _private->rowAccessAction.isChecked() },
        { QStringLiteral("row_access"), _private->rowAccessAction.isChecked() ? _private->rowAccessPatternAction.getSelectedOptions().join(QStringLiteral(", ")) : QStringLiteral("Disabled") },
        { QStringLiteral("row_access_frequency"), _private->rowAccessAction.isChecked() ? _private->rowAccessFrequencyAction.getCurrentText() : QStringLiteral("Disabled") },
        { QStringLiteral("column_access_enabled"), _private->columnAccessAction.isChecked() },
        { QStringLiteral("column_access"), _private->columnAccessAction.isChecked() ? _private->columnAccessPatternAction.getSelectedOptions().join(QStringLiteral(", ")) : QStringLiteral("Disabled") },
        { QStringLiteral("column_access_frequency"), _private->columnAccessAction.isChecked() ? _private->columnAccessFrequencyAction.getCurrentText() : QStringLiteral("Disabled") },
        { QStringLiteral("point_access_enabled"), _private->pointAccessAction.isChecked() },
        { QStringLiteral("point_access_frequency"), _private->pointAccessAction.isChecked() ? _private->pointAccessFrequencyAction.getCurrentText() : QStringLiteral("Disabled") },
        { QStringLiteral("memory_preference"), _private->memoryPreferenceAction.getCurrentText() },
        { QStringLiteral("recommended_tile_rows"), QVariant::fromValue(_private->recommendation.geometry.rowTileSize) },
        { QStringLiteral("recommended_tile_columns"), QVariant::fromValue(_private->recommendation.geometry.columnTileSize) },
        { QStringLiteral("recommended_cache_capacity"), QVariant::fromValue(_private->recommendation.cacheCapacity) },
        { QStringLiteral("recommended_packed_tiles"), true },
        { QStringLiteral("recommended_super_tile_rows"), QVariant::fromValue(_private->recommendation.superTileRows) },
        { QStringLiteral("recommended_super_tile_columns"), QVariant::fromValue(_private->recommendation.superTileColumns) },
        { QStringLiteral("recommendation_version"), 4 }
    };
}

PointDataStorageRecommendationDialog::~PointDataStorageRecommendationDialog() = default;

}
