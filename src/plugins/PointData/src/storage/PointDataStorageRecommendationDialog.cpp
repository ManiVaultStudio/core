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
        accessProfileAction(&settingsGroupAction, "Intended access", { "Mixed or random", "Mostly rows", "Mostly columns" }, "Mixed or random"),
        memoryPreferenceAction(&settingsGroupAction, "Memory preference", { "Balanced", "Low memory" }, "Balanced"),
        recommendationAction(&settingsGroupAction, "Recommendation"),
        rows(rows),
        columns(columns),
        elementSize(elementSize)
    {
        settingsGroupAction.setShowLabels(true);
        settingsGroupAction.setDefaultWidgetFlag(mv::gui::GroupAction::NoMargins);
        accessProfileAction.setDefaultWidgetFlags(mv::gui::OptionAction::ComboBox);
        memoryPreferenceAction.setDefaultWidgetFlags(mv::gui::OptionAction::ComboBox);
        recommendationAction.setDefaultWidgetFlags(mv::gui::StringAction::TextEdit);
        recommendationAction.setEnabled(false);

        settingsGroupAction.addAction(&accessProfileAction);
        settingsGroupAction.addAction(&memoryPreferenceAction);
        settingsGroupAction.addAction(&recommendationAction, -1, [](mv::gui::WidgetAction*, QWidget* widget) {
            if (auto* textEdit = widget->findChild<QTextEdit*>(QStringLiteral("TextEdit")))
                textEdit->setFixedHeight(180);
        });

        QObject::connect(&accessProfileAction, &mv::gui::OptionAction::currentIndexChanged, dialog, [this] { updateRecommendation(); });
        QObject::connect(&memoryPreferenceAction, &mv::gui::OptionAction::currentIndexChanged, dialog, [this] { updateRecommendation(); });
        updateRecommendation();
    }

    void updateRecommendation()
    {
        const auto accessProfile = accessProfileAction.getCurrentIndex() == 1 ? StorageAccessProfile::Rows : accessProfileAction.getCurrentIndex() == 2 ? StorageAccessProfile::Columns : StorageAccessProfile::Mixed;
        const auto memoryPreference = memoryPreferenceAction.getCurrentIndex() == 1 ? StorageMemoryPreference::Low : StorageMemoryPreference::Balanced;
        recommendation = recommendTiledStorage(rows, columns, elementSize, accessProfile, memoryPreference);

        recommendationAction.setString(QStringLiteral("Tile geometry: %1 x %2\nCache capacity: %3 tiles\nPacked tiles: yes\nSuper-tile: %4 x %5 tiles\n\nThese are preliminary heuristics. Review the generated values and validate them with the benchmark.")
            .arg(recommendation.geometry.rowTileSize)
            .arg(recommendation.geometry.columnTileSize)
            .arg(recommendation.cacheCapacity)
            .arg(recommendation.superTileRows)
            .arg(recommendation.superTileColumns));
    }

    mv::gui::VerticalGroupAction settingsGroupAction;
    mv::gui::OptionAction       accessProfileAction;
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

PointDataStorageRecommendationDialog::~PointDataStorageRecommendationDialog() = default;

}
