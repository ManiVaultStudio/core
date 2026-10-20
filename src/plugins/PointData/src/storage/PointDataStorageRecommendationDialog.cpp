// SPDX-License-Identifier: LGPL-3.0-or-later

#include "PointDataStorageRecommendationDialog.h"

#include <actions/OptionAction.h>
#include <actions/StringAction.h>
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
        rowAccessAction(&settingsGroupAction, "Row access", { "None", "Contiguous", "Random" }, "None"),
        columnAccessAction(&settingsGroupAction, "Column access", { "None", "Contiguous", "Random" }, "None"),
        pointAccessFrequencyAction(&settingsGroupAction, "Point access", { "Incidental", "Frequent" }, "Incidental"),
        memoryPreferenceAction(&settingsGroupAction, "Memory preference", { "Balanced", "Low memory" }, "Balanced"),
        recommendationAction(&settingsGroupAction, "Recommendation"),
        rows(rows),
        columns(columns),
        elementSize(elementSize)
    {
        settingsGroupAction.setShowLabels(true);
        settingsGroupAction.setDefaultWidgetFlag(mv::gui::GroupAction::NoMargins);
        rowAccessAction.setDefaultWidgetFlags(mv::gui::OptionAction::ComboBox);
        columnAccessAction.setDefaultWidgetFlags(mv::gui::OptionAction::ComboBox);
        pointAccessFrequencyAction.setDefaultWidgetFlags(mv::gui::OptionAction::ComboBox);
        memoryPreferenceAction.setDefaultWidgetFlags(mv::gui::OptionAction::ComboBox);
        recommendationAction.setDefaultWidgetFlags(mv::gui::StringAction::TextEdit);
        recommendationAction.setEnabled(false);

        settingsGroupAction.addAction(&rowAccessAction);
        settingsGroupAction.addAction(&columnAccessAction);
        settingsGroupAction.addAction(&pointAccessFrequencyAction);
        settingsGroupAction.addAction(&memoryPreferenceAction);
        settingsGroupAction.addAction(&recommendationAction, -1, [](mv::gui::WidgetAction*, QWidget* widget) {
            if (auto* textEdit = widget->findChild<QTextEdit*>(QStringLiteral("TextEdit")))
                textEdit->setFixedHeight(180);
        });

        QObject::connect(&rowAccessAction, &mv::gui::OptionAction::currentIndexChanged, dialog, [this] { updateRecommendation(); });
        QObject::connect(&columnAccessAction, &mv::gui::OptionAction::currentIndexChanged, dialog, [this] { updateRecommendation(); });
        QObject::connect(&pointAccessFrequencyAction, &mv::gui::OptionAction::currentIndexChanged, dialog, [this] { updateRecommendation(); });
        QObject::connect(&memoryPreferenceAction, &mv::gui::OptionAction::currentIndexChanged, dialog, [this] { updateRecommendation(); });
        updateRecommendation();
    }

    void updateRecommendation()
    {
        const auto rowAccess = static_cast<StorageAxisAccess>(rowAccessAction.getCurrentIndex());
        const auto columnAccess = static_cast<StorageAxisAccess>(columnAccessAction.getCurrentIndex());
        const auto pointAccessFrequency = static_cast<StoragePointAccessFrequency>(pointAccessFrequencyAction.getCurrentIndex());
        const auto memoryPreference = memoryPreferenceAction.getCurrentIndex() == 1 ? StorageMemoryPreference::Low : StorageMemoryPreference::Balanced;
        recommendation = recommendTiledStorage(rows, columns, elementSize, rowAccess, columnAccess, pointAccessFrequency, memoryPreference);

        recommendationAction.setString(QStringLiteral("Tile geometry: %1 x %2\nCache capacity: %3 tiles\nPacked tiles: yes\nSuper-tile: %4 x %5 tiles\n\nThese are preliminary heuristics. Review the generated values and validate them with the benchmark.")
            .arg(recommendation.geometry.rowTileSize)
            .arg(recommendation.geometry.columnTileSize)
            .arg(recommendation.cacheCapacity)
            .arg(recommendation.superTileRows)
            .arg(recommendation.superTileColumns));
    }

    mv::gui::VerticalGroupAction settingsGroupAction;
    mv::gui::OptionAction       rowAccessAction;
    mv::gui::OptionAction       columnAccessAction;
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
    resize(560, 360);

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
        { QStringLiteral("row_access"), _private->rowAccessAction.getCurrentText() },
        { QStringLiteral("column_access"), _private->columnAccessAction.getCurrentText() },
        { QStringLiteral("point_access_frequency"), _private->pointAccessFrequencyAction.getCurrentText() },
        { QStringLiteral("memory_preference"), _private->memoryPreferenceAction.getCurrentText() },
        { QStringLiteral("recommended_tile_rows"), QVariant::fromValue(_private->recommendation.geometry.rowTileSize) },
        { QStringLiteral("recommended_tile_columns"), QVariant::fromValue(_private->recommendation.geometry.columnTileSize) },
        { QStringLiteral("recommended_cache_capacity"), QVariant::fromValue(_private->recommendation.cacheCapacity) },
        { QStringLiteral("recommended_packed_tiles"), true },
        { QStringLiteral("recommended_super_tile_rows"), QVariant::fromValue(_private->recommendation.superTileRows) },
        { QStringLiteral("recommended_super_tile_columns"), QVariant::fromValue(_private->recommendation.superTileColumns) },
        { QStringLiteral("recommendation_version"), 1 }
    };
}

PointDataStorageRecommendationDialog::~PointDataStorageRecommendationDialog() = default;

}
