#include "PointDataPhantomGenerator.h"

#include "../PointData.h"
#include "../storage/PointDataStorageRecommendation.h"
#include "../storage/PointDataStorageRecommendationDialog.h"
#include "../storage/TiledPointsStorageBackend.h"
#include "../storage/TiledStorageAsymmetryAction.h"
#include "../storage/TiledStorageGeometry.h"

#include <Application.h>
#include <CoreInterface.h>
#include <DeveloperActionRegistry.h>
#include <actions/IntegralAction.h>
#include <actions/OptionAction.h>
#include <actions/PresetsAction.h>
#include <actions/StringAction.h>
#include <actions/ToggleAction.h>
#include <actions/VerticalGroupAction.h>
#include <util/StyledIcon.h>

#include <QDialog>
#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QPushButton>
#include <QTextEdit>
#include <QVariantMap>
#include <QVBoxLayout>

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstring>
#include <limits>
#include <random>
#include <stdexcept>
#include <vector>

namespace mv::pointdata::developer
{
namespace
{
enum class Population
{
    Deterministic,
    RandomFloat,
    RandomZeroOne,
    Zero
};

enum class DimensionNameType
{
    Dim,
    LoremIpsum
};

struct PhantomConfiguration
{
    std::uint64_t rows            = 0;
    std::uint64_t columns         = 0;
    std::size_t   rowTileSize      = 0;
    std::size_t   columnTileSize   = 0;
    std::size_t   cacheCapacity    = 0;
    std::size_t   superTileRows    = 1;
    std::size_t   superTileColumns = 1;
};

std::vector<std::size_t> parsePositiveList(const QString& text, const QString& label)
{
    std::vector<std::size_t> values;

    for (const auto& part : text.split(',', Qt::SkipEmptyParts))
    {
        bool ok = false;
        const auto value = part.trimmed().toULongLong(&ok);

        if (!ok || value == 0 || value > static_cast<qulonglong>(std::numeric_limits<std::int32_t>::max()))
            throw std::invalid_argument(QString("%1 must contain positive 32-bit integer values separated by commas").arg(label).toStdString());

        if (std::find(values.cbegin(), values.cend(), static_cast<std::size_t>(value)) == values.cend())
            values.push_back(static_cast<std::size_t>(value));
    }

    if (values.empty())
        throw std::invalid_argument(QString("%1 must contain at least one value").arg(label).toStdString());

    return values;
}

std::vector<QString> makeDimensionNames(const DimensionNameType type, const std::uint64_t columns)
{
    static const std::array<const char*, 16> loremWords {
        "lorem", "ipsum", "dolor", "sit", "amet", "consectetur", "adipiscing", "elit",
        "sed", "do", "eiusmod", "tempor", "incididunt", "ut", "labore", "magna"
    };

    std::vector<QString> names;
    names.reserve(static_cast<std::size_t>(columns));

    for (std::uint64_t column = 0; column < columns; ++column)
    {
        if (type == DimensionNameType::Dim)
        {
            names.emplace_back(QStringLiteral("DIM_%1").arg(column + 1));
            continue;
        }

        auto value = column;
        QString name;

        do
        {
            const auto word = loremWords[static_cast<std::size_t>(value % loremWords.size())];
            name = QString::fromLatin1(word) + (name.isEmpty() ? QString() : QStringLiteral(" ") + name);
            value /= loremWords.size();
        }
        while (value > 0);

        names.emplace_back(std::move(name));
    }

    return names;
}

QString formatBytes(const std::uint64_t bytes)
{
    static constexpr const char* suffixes[] = { "B", "KiB", "MiB", "GiB", "TiB" };
    double value       = static_cast<double>(bytes);
    std::size_t suffix = 0;
    while (value >= 1024.0 && suffix < 4)
    {
        value /= 1024.0;
        ++suffix;
    }
    return QString::number(value, 'f', suffix == 0 ? 0 : 2) + " " + suffixes[suffix];
}

std::size_t elementSizeForType(const Points::ElementTypeSpecifier type)
{
    switch (type)
    {
        case Points::ElementTypeSpecifier::float32:
        case Points::ElementTypeSpecifier::int32:
        case Points::ElementTypeSpecifier::uint32:
            return sizeof(std::int32_t);

        case Points::ElementTypeSpecifier::bfloat16:
        case Points::ElementTypeSpecifier::int16:
        case Points::ElementTypeSpecifier::uint16:
            return sizeof(std::int16_t);

        case Points::ElementTypeSpecifier::int8:
        case Points::ElementTypeSpecifier::uint8:
            return sizeof(std::int8_t);

        default:
            return sizeof(float);
    }
}

template <typename T>
void writeValue(char* destination, const std::size_t offset, const T value)
{
    std::memcpy(destination + offset, &value, sizeof(T));
}

QByteArray makeTile(const Points::ElementTypeSpecifier type, const Population population, const std::uint64_t rowOffset, const std::uint64_t columnOffset, const std::uint64_t rows, const std::uint64_t columns)
{
    const auto elementSize = [&] {
        switch (type)
        {
            case Points::ElementTypeSpecifier::float32:
                return sizeof(float);

            case Points::ElementTypeSpecifier::bfloat16:
                return std::size_t(2);

            case Points::ElementTypeSpecifier::int32:
            case Points::ElementTypeSpecifier::uint32:
                return sizeof(std::int32_t);

            case Points::ElementTypeSpecifier::int16:
            case Points::ElementTypeSpecifier::uint16:
                return sizeof(std::int16_t);

            case Points::ElementTypeSpecifier::int8:
            case Points::ElementTypeSpecifier::uint8:
                return sizeof(std::int8_t);
        }
        return sizeof(float);
    }();

    QByteArray bytes(static_cast<qsizetype>(rows * columns * elementSize), Qt::Uninitialized);
    std::mt19937_64 random(rowOffset * 0x9e3779b97f4a7c15ULL + columnOffset + 17);
    std::uniform_real_distribution<float> unit(0.0f, 1.0f);

    for (std::uint64_t row = 0; row < rows; ++row)
    {
        for (std::uint64_t column = 0; column < columns; ++column)
        {
            const auto linearIndex   = row * columns + column;
            const auto offset        = static_cast<std::size_t>(linearIndex * elementSize);
            const auto deterministic = static_cast<float>((rowOffset + row) % 10000) / 10000.0f
                + static_cast<float>((columnOffset + column) % 1000) / 1000.0f;
            const auto value         = population == Population::Zero ? 0.0f
                : population == Population::RandomZeroOne ? (random() & 1ULL ? 1.0f : 0.0f)
                : population == Population::RandomFloat ? unit(random)
                : deterministic;

            switch (type)
            {
                case Points::ElementTypeSpecifier::float32:
                    writeValue(bytes.data(), offset, value);
                    break;

                case Points::ElementTypeSpecifier::bfloat16:
                {
                    std::uint32_t bits = 0;
                    std::memcpy(&bits, &value, sizeof(bits));
                    const auto truncated = static_cast<std::uint16_t>(bits >> 16);
                    writeValue(bytes.data(), offset, truncated);
                    break;
                }

                case Points::ElementTypeSpecifier::int32:
                    writeValue(bytes.data(), offset, static_cast<std::int32_t>(value * 1000.0f));
                    break;

                case Points::ElementTypeSpecifier::uint32:
                    writeValue(bytes.data(), offset, static_cast<std::uint32_t>(std::max(0.0f, value * 1000.0f)));
                    break;

                case Points::ElementTypeSpecifier::int16:
                    writeValue(bytes.data(), offset, static_cast<std::int16_t>(value * 1000.0f));
                    break;

                case Points::ElementTypeSpecifier::uint16:
                    writeValue(bytes.data(), offset, static_cast<std::uint16_t>(std::max(0.0f, value * 1000.0f)));
                    break;

                case Points::ElementTypeSpecifier::int8:
                    writeValue(bytes.data(), offset, static_cast<std::int8_t>(value * 100.0f));
                    break;

                case Points::ElementTypeSpecifier::uint8:
                    writeValue(bytes.data(), offset, static_cast<std::uint8_t>(std::max(0.0f, value * 100.0f)));
                    break;
            }
        }
    }
    return bytes;
}

float makeValue(const Population population, const std::uint64_t row, const std::uint64_t column, std::mt19937_64& random, std::uniform_real_distribution<float>& unit)
{
    const auto deterministic = static_cast<float>(row % 10000) / 10000.0f + static_cast<float>(column % 1000) / 1000.0f;

    return population == Population::Zero ? 0.0f
        : population == Population::RandomZeroOne ? (random() & 1ULL ? 1.0f : 0.0f)
        : population == Population::RandomFloat ? unit(random)
        : deterministic;
}

template <typename T>
T convertValue(const float value)
{
    return static_cast<T>(value);
}

template <>
biovault::bfloat16_t convertValue(const float value)
{
    std::uint32_t bits = 0;
    std::memcpy(&bits, &value, sizeof(bits));

    const auto truncated = static_cast<std::uint16_t>(bits >> 16);
    biovault::bfloat16_t result;
    std::memcpy(&result, &truncated, sizeof(truncated));
    return result;
}

template <typename T>
std::vector<T> makeEagerData(const Population population, const std::uint64_t rows, const std::uint64_t columns)
{
    std::vector<T> values(static_cast<std::size_t>(rows * columns));
    std::mt19937_64 random(17);
    std::uniform_real_distribution<float> unit(0.0f, 1.0f);

    for (std::uint64_t row = 0; row < rows; ++row)
    {
        for (std::uint64_t column = 0; column < columns; ++column)
            values[static_cast<std::size_t>(row * columns + column)] = convertValue<T>(makeValue(population, row, column, random, unit));
    }

    return values;
}

void populateEager(Points& points, const Points::ElementTypeSpecifier type, const Population population, const std::uint64_t rows, const std::uint64_t columns)
{
    switch (type)
    {
        case Points::ElementTypeSpecifier::float32:
            points.setData(makeEagerData<float>(population, rows, columns), columns);
            break;

        case Points::ElementTypeSpecifier::bfloat16:
            points.setData(makeEagerData<biovault::bfloat16_t>(population, rows, columns), columns);
            break;

        case Points::ElementTypeSpecifier::int32:
            points.setData(makeEagerData<std::int32_t>(population, rows, columns), columns);
            break;

        case Points::ElementTypeSpecifier::uint32:
            points.setData(makeEagerData<std::uint32_t>(population, rows, columns), columns);
            break;

        case Points::ElementTypeSpecifier::int16:
            points.setData(makeEagerData<std::int16_t>(population, rows, columns), columns);
            break;

        case Points::ElementTypeSpecifier::uint16:
            points.setData(makeEagerData<std::uint16_t>(population, rows, columns), columns);
            break;

        case Points::ElementTypeSpecifier::int8:
            points.setData(makeEagerData<std::int8_t>(population, rows, columns), columns);
            break;

        case Points::ElementTypeSpecifier::uint8:
            points.setData(makeEagerData<std::uint8_t>(population, rows, columns), columns);
            break;
    }
}

class PhantomSettingsAction final : public mv::gui::VerticalGroupAction
{
public:
    PhantomSettingsAction(QObject* parent, const QString& title) :
        mv::gui::VerticalGroupAction(parent, title)
    {
    }

    void fromVariantMap(const QVariantMap& variantMap) override
    {
        mv::gui::VerticalGroupAction::fromVariantMap(variantMap);

        for (auto* action : getActions())
            action->fromParentVariantMap(variantMap, true);
    }

    QVariantMap toVariantMap() const override
    {
        auto variantMap = mv::gui::VerticalGroupAction::toVariantMap();

        for (const auto* action : const_cast<PhantomSettingsAction*>(this)->getConstActions())
            action->insertIntoVariantMap(variantMap);

        return variantMap;
    }
};

/**
 * @brief Collects settings for a developer-generated phantom dataset.
 *
 * The dialog keeps dataset dimensions, element representation, population
 * strategy, dimension-name style, storage backend, tile geometry, and cache capacity together so the
 * developer menu can create repeatable large-data scenarios without embedding
 * configuration in the generator callback. It supports both fully materialized
 * eager data and generator-backed tiled data. Dataset dimensions and, for tiled
 * data, tile dimensions, cache capacity, and packed-tile geometry accept
 * comma-separated lists; the generator creates the Cartesian product of those
 * values.
 *
 * @author T. Kroes
 */
class PhantomDialog final : public QDialog
{
public:
    explicit PhantomDialog(QWidget* parent) :
        QDialog(parent),
        _settingsGroupAction(this, "Phantom points"),
        _descriptionAction(&_settingsGroupAction, "Description", "Enter one value or a comma-separated list for dataset size, tile geometry, cache capacity, or packed-tile geometry. Every combination of the supplied values creates a separate phantom dataset."),
        _nameAction(&_settingsGroupAction, "Dataset name", "Phantom points"),
        _nameOverrideAction(&_settingsGroupAction, "Override generated name", false),
        _rowsAction(&_settingsGroupAction, "Rows", "1000000"),
        _columnsAction(&_settingsGroupAction, "Dimensions", "32"),
        _dimensionNameAction(&_settingsGroupAction, "Dimension names", { "DIM_<index>", "Lorem ipsum" }, "DIM_<index>"),
        _typeAction(&_settingsGroupAction, "Element type"),
        _populationAction(&_settingsGroupAction, "Population", { "Deterministic", "Random floats", "Random zeros and ones", "All zeros" }, "Deterministic"),
        _storageBackendAction(&_settingsGroupAction, "Storage backend", { "Eager", "Tiled" }, "Tiled"),
        _tileAsymmetryAction(&_settingsGroupAction),
        _tileSizeAction(&_settingsGroupAction, "Tile size (MiB)", 1, 1024, 1),
        _rowTileSizeAction(&_settingsGroupAction, "Tile heights", "256"),
        _columnTileSizeAction(&_settingsGroupAction, "Tile widths", "8"),
        _cacheCapacityAction(&_settingsGroupAction, "Cache tiles", "8"),
        _packedTilesAction(&_settingsGroupAction, "Packed tiles", false),
        _superTileRowsAction(&_settingsGroupAction, "Super-tile rows", "1"),
        _superTileColumnsAction(&_settingsGroupAction, "Super-tile columns", "1"),
        _estimateAction(&_settingsGroupAction, "Estimate"),
        _presetsAction(&_settingsGroupAction, &_settingsGroupAction, "Developer/PhantomPointGenerator/Presets", "Phantom points", mv::util::StyledIcon("prescription"))
    {
        setWindowIcon(mv::util::StyledIcon("table"));
        setWindowTitle(tr("Create phantom points dataset"));

        QStringList typeNames;

        for (const auto* typeName : Points::getElementTypeNames())
            typeNames.push_back(QString::fromLatin1(typeName));

        _typeAction.initialize(typeNames, typeNames.first());

        _settingsGroupAction.setShowLabels(true);
        _settingsGroupAction.setDefaultWidgetFlag(mv::gui::GroupAction::NoMargins);
        _descriptionAction.setDefaultWidgetFlags(mv::gui::StringAction::TextEdit);
        _descriptionAction.setEnabled(false);
        _nameAction.setDefaultWidgetFlags(mv::gui::StringAction::LineEdit);
        _nameOverrideAction.setDefaultWidgetFlags(mv::gui::ToggleAction::CheckBox);
        _rowsAction.setDefaultWidgetFlags(mv::gui::StringAction::LineEdit);
        _columnsAction.setDefaultWidgetFlags(mv::gui::StringAction::LineEdit);
        _dimensionNameAction.setDefaultWidgetFlags(mv::gui::OptionAction::ComboBox);
        _typeAction.setDefaultWidgetFlags(mv::gui::OptionAction::ComboBox);
        _populationAction.setDefaultWidgetFlags(mv::gui::OptionAction::ComboBox);
        _storageBackendAction.setDefaultWidgetFlags(mv::gui::OptionAction::ComboBox);
        _tileSizeAction.setDefaultWidgetFlags(mv::gui::IntegralAction::SpinBox);
        _rowTileSizeAction.setDefaultWidgetFlags(mv::gui::StringAction::LineEdit);
        _columnTileSizeAction.setDefaultWidgetFlags(mv::gui::StringAction::LineEdit);
        _cacheCapacityAction.setDefaultWidgetFlags(mv::gui::StringAction::LineEdit);
        _packedTilesAction.setDefaultWidgetFlags(mv::gui::ToggleAction::CheckBox);
        _superTileRowsAction.setDefaultWidgetFlags(mv::gui::StringAction::LineEdit);
        _superTileColumnsAction.setDefaultWidgetFlags(mv::gui::StringAction::LineEdit);
        _estimateAction.setDefaultWidgetFlags(mv::gui::StringAction::TextEdit);
        _estimateAction.setEnabled(false);
        _presetsAction.setSaveDefaultPresetActionVisible(false);

        _descriptionAction.setToolTip("Summary of the generator settings. The controls below accept single values or comma-separated lists; lists create the Cartesian product of all supplied values.");
        _nameAction.setToolTip("Dataset name. When automatic naming is enabled, this field is generated from the dataset size, type, storage backend, tile geometry, cache, and packing settings.");
        _nameOverrideAction.setToolTip("Use a custom dataset name instead of the generated name. Naming does not affect storage or benchmark performance.");
        _rowsAction.setToolTip("Number of rows in each generated dataset. Larger datasets increase disk usage, creation time, and the amount of data that can be touched during scans; comma-separated values create multiple sizes.");
        _columnsAction.setToolTip("Number of dimensions, or columns, in each generated dataset. More columns increase the raw footprint and can make column access and wide row access more expensive; comma-separated values create multiple sizes.");
        _dimensionNameAction.setToolTip("How dimensions are named. This affects labels and metadata, but has no meaningful effect on numeric access performance.");
        _typeAction.setToolTip("Element type stored for every point value. Smaller types reduce disk and memory traffic; larger or more precise types increase storage, I/O, and decoding work.");
        _populationAction.setToolTip("How values are generated. Deterministic, random, and zero-filled data are useful for different tests; the choice may affect compression ratio and therefore disk footprint and I/O time.");
        _storageBackendAction.setToolTip("Storage representation. Eager storage keeps the complete dataset materialized, while Tiled storage loads bounded tiles on demand and is the relevant choice for tile and cache experiments.");
        _tileAsymmetryAction.setToolTip("Balances tile shape between row-oriented and column-oriented access. Row-oriented tiles can accelerate contiguous row access, while column-oriented tiles can accelerate contiguous column access; the wrong orientation can increase tile reads.");
        _tileSizeAction.setToolTip("Automatic tile-geometry budget used to calculate row and column tile sizes. Larger budgets reduce the number of tiles but increase the amount of data read and decoded per miss; changing this policy recalculates the tile dimensions below.");
        _rowTileSizeAction.setToolTip("Number of dataset rows in each tile. Larger values help contiguous row scans but make random access fetch more unrelated data; comma-separated values create one dataset for every combination.");
        _columnTileSizeAction.setToolTip("Number of dataset columns in each tile. Larger values help contiguous column access but increase the data fetched for individual values or narrow column requests; comma-separated values create one dataset for every combination.");
        _cacheCapacityAction.setToolTip("Maximum number of decoded tiles retained in memory. A larger cache can reduce repeated disk reads for local or repeated access, but uses more memory; comma-separated values create one dataset for every combination.");
        _packedTilesAction.setToolTip("Pack neighboring tiles into shared container files. Packing can reduce file-open and filesystem overhead, while individual tile access still requires locating and decoding the requested tile.");
        _superTileRowsAction.setToolTip("Number of tile rows in each packed-tile container. Larger values can reduce metadata and file-operation overhead, but may increase container reads and packing work; comma-separated values create one dataset for every combination.");
        _superTileColumnsAction.setToolTip("Number of tile columns in each packed-tile container. Larger values can reduce metadata and file-operation overhead, but may increase container reads and packing work; comma-separated values create one dataset for every combination.");
        _estimateAction.setToolTip("Estimated raw footprint, tile count, cache footprint, and index size. These are planning estimates; actual runtime depends on compression, cache state, filesystem behavior, and access pattern.");

        _settingsGroupAction.addAction(&_descriptionAction, -1, [](mv::gui::WidgetAction*, QWidget* widget) {
            if (auto* textEdit = widget->findChild<QTextEdit*>(QStringLiteral("TextEdit")))
                textEdit->setFixedHeight(56);
        });
        _settingsGroupAction.addAction(&_nameAction);
        _settingsGroupAction.addAction(&_nameOverrideAction);
        _settingsGroupAction.addAction(&_rowsAction);
        _settingsGroupAction.addAction(&_columnsAction);
        _settingsGroupAction.addAction(&_dimensionNameAction);
        _settingsGroupAction.addAction(&_typeAction);
        _settingsGroupAction.addAction(&_populationAction);
        _settingsGroupAction.addAction(&_storageBackendAction);
        _settingsGroupAction.addAction(&_tileAsymmetryAction);
        _settingsGroupAction.addAction(&_tileSizeAction);
        _settingsGroupAction.addAction(&_rowTileSizeAction);
        _settingsGroupAction.addAction(&_columnTileSizeAction);
        _settingsGroupAction.addAction(&_cacheCapacityAction);
        _settingsGroupAction.addAction(&_packedTilesAction);
        _settingsGroupAction.addAction(&_superTileRowsAction);
        _settingsGroupAction.addAction(&_superTileColumnsAction);
        _settingsGroupAction.addAction(&_estimateAction);

        auto* layout = new QVBoxLayout(this);
        layout->addWidget(_settingsGroupAction.createWidget(this));
        setMinimumWidth(720);

        auto* presetsButton = new QPushButton(tr("Presets"), this);
        presetsButton->setMenu(_presetsAction.getMenu(presetsButton));

        auto* wizardButton = new QPushButton(tr("Wizard"), this);
        wizardButton->setIcon(mv::util::StyledIcon("lightbulb"));
        _wizardButton = wizardButton;

        auto* createButton = new QPushButton(tr("Create"), this);
        _createButton = createButton;

        auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
        _okButton = buttons->button(QDialogButtonBox::Ok);
        auto* buttonLayout = new QHBoxLayout();
        buttonLayout->addWidget(presetsButton);
        buttonLayout->addWidget(wizardButton);
        buttonLayout->addStretch(1);
        buttonLayout->addWidget(createButton);
        buttonLayout->addWidget(buttons);
        layout->addLayout(buttonLayout);

        connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
        connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

        connect(&_rowsAction, &mv::gui::StringAction::stringChanged, this, [this] { updateName(); updateEstimate(); });
        connect(&_columnsAction, &mv::gui::StringAction::stringChanged, this, [this] { updateName(); updateEstimate(); });
        connect(&_typeAction, &mv::gui::OptionAction::currentIndexChanged, this, [this] { updateTileGeometryFromPolicy(); });
        connect(&_populationAction, &mv::gui::OptionAction::currentIndexChanged, this, [this] { updateName(); });
        connect(&_nameOverrideAction, &mv::gui::ToggleAction::toggled, this, [this] { updateName(); });
        connect(&_storageBackendAction, &mv::gui::OptionAction::currentIndexChanged, this, [this] { updateActions(); });
        connect(&_tileAsymmetryAction.getAsymmetryAction(), &mv::gui::IntegralAction::valueChanged, this, [this] { updateTileGeometryFromPolicy(); });
        connect(&_tileSizeAction, &mv::gui::IntegralAction::valueChanged, this, [this] { updateTileGeometryFromPolicy(); });
        connect(&_rowTileSizeAction, &mv::gui::StringAction::stringChanged, this, [this] { updateEstimate(); });
        connect(&_columnTileSizeAction, &mv::gui::StringAction::stringChanged, this, [this] { updateEstimate(); });
        connect(&_cacheCapacityAction, &mv::gui::StringAction::stringChanged, this, [this] { updateEstimate(); });
        connect(&_packedTilesAction, &mv::gui::ToggleAction::toggled, this, [this] { updateActions(); });
        connect(&_superTileRowsAction, &mv::gui::StringAction::stringChanged, this, [this] { updateEstimate(); });
        connect(&_superTileColumnsAction, &mv::gui::StringAction::stringChanged, this, [this] { updateEstimate(); });
        connect(wizardButton, &QPushButton::clicked, this, [this] { showStorageWizard(); });

        loadSettings();
        updateActions();
    }

    QString name() const
    {
        const auto name = _nameAction.getString().trimmed();
        return name.isEmpty() ? QStringLiteral("Phantom points") : name;
    }

    QPushButton* createButton() const { return _createButton; }

    std::vector<std::size_t> rowCounts() const { return parsePositiveList(_rowsAction.getString(), "Rows"); }

    std::vector<std::size_t> columnCounts() const { return parsePositiveList(_columnsAction.getString(), "Dimensions"); }

    std::uint64_t rows() const { return static_cast<std::uint64_t>(rowCounts().front()); }

    std::uint64_t columns() const { return static_cast<std::uint64_t>(columnCounts().front()); }

    std::vector<std::size_t> rowTileSizes() const { return parsePositiveList(_rowTileSizeAction.getString(), "Tile heights"); }

    std::vector<std::size_t> columnTileSizes() const { return parsePositiveList(_columnTileSizeAction.getString(), "Tile widths"); }

    std::vector<std::size_t> cacheCapacities() const { return parsePositiveList(_cacheCapacityAction.getString(), "Cache tiles"); }

    bool packedTiles() const { return _packedTilesAction.isChecked(); }

    std::vector<std::size_t> superTileRows() const { return parsePositiveList(_superTileRowsAction.getString(), "Super-tile rows"); }

    std::vector<std::size_t> superTileColumns() const { return parsePositiveList(_superTileColumnsAction.getString(), "Super-tile columns"); }

    bool isTiled() const { return _storageBackendAction.getCurrentText() == "Tiled"; }

    Points::ElementTypeSpecifier type() const { return Points::elementTypeSpecifier(_typeAction.getCurrentText()); }

    Population population() const { return static_cast<Population>(_populationAction.getCurrentIndex()); }

    DimensionNameType dimensionNameType() const { return static_cast<DimensionNameType>(_dimensionNameAction.getCurrentIndex()); }

    std::vector<PhantomConfiguration> configurations() const
    {
        const auto rowCountValues        = this->rowCounts();
        const auto columnCountValues     = this->columnCounts();
        const auto rowTileSizeValues     = isTiled() ? this->rowTileSizes() : std::vector<std::size_t> { 0 };
        const auto columnTileSizeValues  = isTiled() ? this->columnTileSizes() : std::vector<std::size_t> { 0 };
        const auto cacheCapacityValues   = isTiled() ? this->cacheCapacities() : std::vector<std::size_t> { 0 };
        const auto superTileRowValues    = isTiled() && packedTiles() ? this->superTileRows() : std::vector<std::size_t> { 1 };
        const auto superTileColumnValues = isTiled() && packedTiles() ? this->superTileColumns() : std::vector<std::size_t> { 1 };
        std::vector<PhantomConfiguration> result;

        result.reserve(rowCountValues.size() * columnCountValues.size() * rowTileSizeValues.size() * columnTileSizeValues.size() * cacheCapacityValues.size() * superTileRowValues.size() * superTileColumnValues.size());

        for (const auto rowCount : rowCountValues)
        {
            for (const auto columnCount : columnCountValues)
            {
                for (const auto rowTileSize : rowTileSizeValues)
                {
                    for (const auto columnTileSize : columnTileSizeValues)
                    {
                        for (const auto cacheCapacity : cacheCapacityValues)
                        {
                            for (const auto superTileRow : superTileRowValues)
                            {
                                for (const auto superTileColumn : superTileColumnValues)
                                    result.push_back({ rowCount, columnCount, rowTileSize, columnTileSize, cacheCapacity, superTileRow, superTileColumn });
                            }
                        }
                    }
                }
            }
        }

        return result;
    }

    QVariantMap storageRecommendationProfile() const { return _storageRecommendationProfile; }

    QString generatedName(const PhantomConfiguration& configuration) const
    {
        auto generatedName = QStringLiteral("Phantom (size: %1x%2, type: %3, storage: %4")
            .arg(configuration.rows)
            .arg(configuration.columns)
            .arg(_typeAction.getCurrentText())
            .arg(_storageBackendAction.getCurrentText());

        if (isTiled())
        {
            generatedName += QStringLiteral(", tilesize: %1x%2, cache: %3")
                .arg(configuration.rowTileSize)
                .arg(configuration.columnTileSize)
                .arg(configuration.cacheCapacity);

            if (packedTiles())
                generatedName += QStringLiteral(", packed %1x%2 tiles")
                    .arg(configuration.superTileRows)
                    .arg(configuration.superTileColumns);
        }

        generatedName += QLatin1Char(')');
        return generatedName;
    }

    QString datasetName(const PhantomConfiguration& configuration, const std::size_t index, const std::size_t count) const
    {
        if (!_nameOverrideAction.isChecked())
            return generatedName(configuration);

        const auto baseName = name();
        return count > 1 ? QStringLiteral("%1 (%2/%3)").arg(baseName).arg(index + 1).arg(count) : baseName;
    }

    void saveSettings() const
    {
        auto* application = Application::current();

        application->setSetting("Developer/PhantomPointGenerator/Name", _nameAction.getString());
        application->setSetting("Developer/PhantomPointGenerator/NameOverride", _nameOverrideAction.isChecked());
        application->setSetting("Developer/PhantomPointGenerator/Rows", _rowsAction.getString());
        application->setSetting("Developer/PhantomPointGenerator/Columns", _columnsAction.getString());
        application->setSetting("Developer/PhantomPointGenerator/DimensionNames", _dimensionNameAction.getCurrentText());
        application->setSetting("Developer/PhantomPointGenerator/ElementType", _typeAction.getCurrentText());
        application->setSetting("Developer/PhantomPointGenerator/Population", _populationAction.getCurrentText());
        application->setSetting("Developer/PhantomPointGenerator/StorageBackend", _storageBackendAction.getCurrentText());
        application->setSetting("Developer/PhantomPointGenerator/TileAsymmetry", _tileAsymmetryAction.getAsymmetryAction().getValue());
        application->setSetting("Developer/PhantomPointGenerator/TileSizeMiB", _tileSizeAction.getValue());
        application->setSetting("Developer/PhantomPointGenerator/RowTileSizes", _rowTileSizeAction.getString());
        application->setSetting("Developer/PhantomPointGenerator/ColumnTileSizes", _columnTileSizeAction.getString());
        application->setSetting("Developer/PhantomPointGenerator/CacheCapacities", _cacheCapacityAction.getString());
        application->setSetting("Developer/PhantomPointGenerator/PackedTiles", _packedTilesAction.isChecked());
        application->setSetting("Developer/PhantomPointGenerator/SuperTileRows", _superTileRowsAction.getString());
        application->setSetting("Developer/PhantomPointGenerator/SuperTileColumns", _superTileColumnsAction.getString());
    }

private:
    void loadSettings()
    {
        auto* application = Application::current();

        _nameAction.setString(application->getSetting("Developer/PhantomPointGenerator/Name", _nameAction.getString()).toString());
        _nameOverrideAction.setChecked(application->getSetting("Developer/PhantomPointGenerator/NameOverride", _nameOverrideAction.isChecked()).toBool());
        _rowsAction.setString(application->getSetting("Developer/PhantomPointGenerator/Rows", _rowsAction.getString()).toString());
        _columnsAction.setString(application->getSetting("Developer/PhantomPointGenerator/Columns", _columnsAction.getString()).toString());
        _dimensionNameAction.setCurrentText(application->getSetting("Developer/PhantomPointGenerator/DimensionNames", _dimensionNameAction.getCurrentText()).toString());
        _typeAction.setCurrentText(application->getSetting("Developer/PhantomPointGenerator/ElementType", _typeAction.getCurrentText()).toString());
        _populationAction.setCurrentText(application->getSetting("Developer/PhantomPointGenerator/Population", _populationAction.getCurrentText()).toString());
        _storageBackendAction.setCurrentText(application->getSetting("Developer/PhantomPointGenerator/StorageBackend", _storageBackendAction.getCurrentText()).toString());
        const auto asymmetry = application->getSetting("Developer/PhantomPointGenerator/TileAsymmetry", application->getSetting("Developer/PhantomPointGenerator/TileDominance", _tileAsymmetryAction.getAsymmetryAction().getValue())).toInt();
        _tileAsymmetryAction.getAsymmetryAction().setValue(asymmetry);
        _tileSizeAction.setValue(application->getSetting("Developer/PhantomPointGenerator/TileSizeMiB", _tileSizeAction.getValue()).toInt());
        _rowTileSizeAction.setString(application->getSetting("Developer/PhantomPointGenerator/RowTileSizes", application->getSetting("Developer/PhantomPointGenerator/RowTileSize", _rowTileSizeAction.getString())).toString());
        _columnTileSizeAction.setString(application->getSetting("Developer/PhantomPointGenerator/ColumnTileSizes", application->getSetting("Developer/PhantomPointGenerator/ColumnTileSize", _columnTileSizeAction.getString())).toString());
        _cacheCapacityAction.setString(application->getSetting("Developer/PhantomPointGenerator/CacheCapacities", application->getSetting("Developer/PhantomPointGenerator/CacheCapacity", _cacheCapacityAction.getString())).toString());
        _packedTilesAction.setChecked(application->getSetting("Developer/PhantomPointGenerator/PackedTiles", _packedTilesAction.isChecked()).toBool());
        _superTileRowsAction.setString(application->getSetting("Developer/PhantomPointGenerator/SuperTileRows", _superTileRowsAction.getString()).toString());
        _superTileColumnsAction.setString(application->getSetting("Developer/PhantomPointGenerator/SuperTileColumns", _superTileColumnsAction.getString()).toString());
    }

    void updateActions()
    {
        updateName();
        const auto tiled = isTiled();
        _tileAsymmetryAction.setEnabled(tiled);
        _tileSizeAction.setEnabled(tiled);
        _rowTileSizeAction.setEnabled(tiled);
        _columnTileSizeAction.setEnabled(tiled);
        _cacheCapacityAction.setEnabled(tiled);
        _packedTilesAction.setEnabled(tiled);
        _superTileRowsAction.setEnabled(tiled && packedTiles());
        _superTileColumnsAction.setEnabled(tiled && packedTiles());
        updateEstimate();
    }

    void showStorageWizard()
    {
        const auto configurations = this->configurations();

        if (configurations.size() != 1)
            return;

        storage::PointDataStorageRecommendationDialog dialog(this, configurations.front().rows, configurations.front().columns, elementSizeForType(type()));

        if (dialog.exec() != QDialog::Accepted)
            return;

        const auto& recommendation = dialog.recommendation();
        _storageRecommendationProfile = dialog.profile();
        _storageBackendAction.setCurrentText(QStringLiteral("Tiled"));
        _rowTileSizeAction.setString(QString::number(recommendation.geometry.rowTileSize));
        _columnTileSizeAction.setString(QString::number(recommendation.geometry.columnTileSize));
        _cacheCapacityAction.setString(QString::number(recommendation.cacheCapacity));
        _packedTilesAction.setChecked(true);
        _superTileRowsAction.setString(QString::number(recommendation.superTileRows));
        _superTileColumnsAction.setString(QString::number(recommendation.superTileColumns));
        updateActions();
    }

    void updateTileGeometryFromPolicy()
    {
        if (_rowsAction.getString().contains(',') || _columnsAction.getString().contains(','))
        {
            updateEstimate();
            return;
        }

        try
        {
            const auto geometry = storage::calculateTiledStorageGeometry(rows(), columns(), elementSizeForType(type()), static_cast<std::uint64_t>(_tileSizeAction.getValue()) * 1024 * 1024, _tileAsymmetryAction.getAsymmetryAction().getValue());
            _rowTileSizeAction.setString(QString::number(geometry.rowTileSize));
            _columnTileSizeAction.setString(QString::number(geometry.columnTileSize));
            _cacheCapacityAction.setString(QString::number(storage::calculateTiledStorageCacheCapacity(rows(), columns(), geometry)));
            updateName();
        }
        catch (const std::exception&)
        {
            updateEstimate();
        }
    }

    void updateName()
    {
        _nameAction.setEnabled(_nameOverrideAction.isChecked());

        if (_nameOverrideAction.isChecked())
            return;

        if (!isTiled())
        {
            _nameAction.setString(QStringLiteral("Phantom (size: %1x%2, type: %3, storage: %4)")
                .arg(_rowsAction.getString().trimmed())
                .arg(_columnsAction.getString().trimmed())
                .arg(_typeAction.getCurrentText())
                .arg(_storageBackendAction.getCurrentText()));
            return;
        }

        _nameAction.setString(QStringLiteral("Phantom (size: %1x%2, type: %3, storage: %4, tilesize: %5x%6, cache: %7%8)")
            .arg(_rowsAction.getString().trimmed())
            .arg(_columnsAction.getString().trimmed())
            .arg(_typeAction.getCurrentText())
            .arg(_storageBackendAction.getCurrentText())
            .arg(_rowTileSizeAction.getString().trimmed())
            .arg(_columnTileSizeAction.getString().trimmed())
            .arg(_cacheCapacityAction.getString().trimmed())
            .arg(packedTiles() ? QStringLiteral(", packed %1x%2 tiles").arg(_superTileRowsAction.getString().trimmed()).arg(_superTileColumnsAction.getString().trimmed()) : QString()));
    }

    void updateEstimate()
    {
        updateName();

        try
        {
            const auto configurations = this->configurations();

            if (configurations.empty())
                throw std::invalid_argument("No valid phantom configurations");

            _createButton->setEnabled(true);
            _wizardButton->setEnabled(configurations.size() == 1);
            _createButton->setText(QStringLiteral("Create (%1)").arg(configurations.size()));
            _okButton->setText(QStringLiteral("OK (%1)").arg(configurations.size()));

            if (!isTiled())
            {
                _estimateAction.setString(QString("Storage backend: Eager\nConfigurations: %1\nFirst size: %2x%3\nRaw size: %4")
                    .arg(configurations.size())
                    .arg(configurations.front().rows)
                    .arg(configurations.front().columns)
                    .arg(formatBytes(configurations.front().rows * configurations.front().columns * elementSizeForType(type()))));
                return;
            }

            const auto type        = Points::elementTypeSpecifier(_typeAction.getCurrentText());
            const auto elementSize = elementSizeForType(type);
            const auto& configuration = configurations.front();
            const storage::TiledStorageGeometry geometry { configuration.rowTileSize, configuration.columnTileSize };
            const auto estimate = PointData::estimateTiledMemory(rows(), columns(), elementSize, geometry.rowTileSize, geometry.columnTileSize, configuration.cacheCapacity);

            _estimateAction.setString(QString("Storage backend: Tiled\nConfigurations: %1\nRaw size: %2\nFirst tile: %3x%4\nLargest tile: %5\nEstimated cache: %6\nTiles: %7\nIndex estimate: %8")
                .arg(configurations.size())
                .arg(formatBytes(estimate.rawDataBytes))
                .arg(configuration.rowTileSize)
                .arg(configuration.columnTileSize)
                .arg(formatBytes(estimate.largestTileBytes))
                .arg(formatBytes(estimate.cachedTileBytes))
                .arg(estimate.tileCount)
                .arg(formatBytes(estimate.indexBytes)));
        }
        catch (const std::exception& exception)
        {
            _createButton->setEnabled(false);
            _wizardButton->setEnabled(false);
            _createButton->setText(QStringLiteral("Create"));
            _okButton->setText(QStringLiteral("OK"));
            _estimateAction.setString(QStringLiteral("Invalid settings: %1").arg(QString::fromUtf8(exception.what())));
        }
    }

    PhantomSettingsAction                _settingsGroupAction;
    mv::gui::StringAction                _descriptionAction;
    mv::gui::StringAction                _nameAction;
    mv::gui::ToggleAction                _nameOverrideAction;
    mv::gui::StringAction                _rowsAction;
    mv::gui::StringAction                _columnsAction;
    mv::gui::OptionAction                _dimensionNameAction;
    mv::gui::OptionAction                _typeAction;
    mv::gui::OptionAction                _populationAction;
    mv::gui::OptionAction                _storageBackendAction;
    mv::gui::TiledStorageAsymmetryAction _tileAsymmetryAction;
    mv::gui::IntegralAction              _tileSizeAction;
    mv::gui::StringAction                _rowTileSizeAction;
    mv::gui::StringAction                _columnTileSizeAction;
    mv::gui::StringAction                _cacheCapacityAction;
    mv::gui::ToggleAction                _packedTilesAction;
    mv::gui::StringAction                _superTileRowsAction;
    mv::gui::StringAction                _superTileColumnsAction;
    mv::gui::StringAction                _estimateAction;
    mv::gui::PresetsAction               _presetsAction;
    QPushButton*                          _createButton = nullptr;
    QPushButton*                          _okButton = nullptr;
    QPushButton*                          _wizardButton = nullptr;
    QVariantMap                            _storageRecommendationProfile;
};
}

void PointDataPhantomGenerator::populateMenu(QMenu& menu, QWidget* owner)
{
    auto* action = menu.addAction(mv::util::StyledIcon("table"), QObject::tr("Phantom points…"));
    action->setToolTip(QObject::tr("Create a synthetic eager or tiled points dataset"));
    QObject::connect(action, &QAction::triggered, owner, [owner] {
        PhantomDialog dialog(owner);
        const auto createDatasets = [&dialog] {
            dialog.saveSettings();

            const auto configurations = dialog.configurations();
            const auto type           = dialog.type();
            const auto population     = dialog.population();
            const auto recommendationProfile = configurations.size() == 1 ? dialog.storageRecommendationProfile() : QVariantMap();

            for (std::size_t index = 0; index < configurations.size(); ++index)
            {
                const auto& configuration = configurations[index];
                const auto datasetName = dialog.datasetName(configuration, index, configurations.size());
                auto dataset = mv::data().createDataset<Points>("Points", datasetName);

                if (!recommendationProfile.isEmpty())
                    dataset.getDataset()->setProperty(QStringLiteral("PointDataStorageRecommendation"), QVariant::fromValue(recommendationProfile));

                if (dialog.isTiled())
                {
                    auto creation = std::make_shared<storage::TiledStorageBackendCreationOptions>();
                    creation->rows              = configuration.rows;
                    creation->columns           = configuration.columns;
                    creation->elementSize       = elementSizeForType(type);
                    creation->elementTypeIndex  = static_cast<std::uint32_t>(type);
                    creation->rowTileSize       = configuration.rowTileSize;
                    creation->columnTileSize    = configuration.columnTileSize;
                    creation->cacheCapacity    = configuration.cacheCapacity;
                    creation->packedTiles      = dialog.packedTiles();
                    creation->superTileRows    = configuration.superTileRows;
                    creation->superTileColumns = configuration.superTileColumns;
                    creation->generator        = [type, population](const std::uint64_t rowOffset, const std::uint64_t columnOffset, const std::uint64_t rows, const std::uint64_t columns) {
                        return makeTile(type, population, rowOffset, columnOffset, rows, columns);
                    };

                    dataset->convertToStorageBackend(Points::StorageBackendType::Tiled, creation);
                }
                else
                    populateEager(*dataset, type, population, configuration.rows, configuration.columns);

                dataset->setDimensionNames(makeDimensionNames(dialog.dimensionNameType(), configuration.columns));
                mv::events().notifyDatasetDataChanged(dataset);
            }

            mv::help().addNotification(QObject::tr("Phantom points datasets created"),
                QObject::tr("Created %1 %2 dataset%3.")
                    .arg(configurations.size())
                    .arg(dialog.isTiled() ? QObject::tr("tiled") : QObject::tr("eager"))
                    .arg(configurations.size() == 1 ? QString() : QStringLiteral("s")), mv::util::StyledIcon("check"));
        };

        QObject::connect(dialog.createButton(), &QPushButton::clicked, owner, [&dialog, createDatasets, owner] {
            try
            {
                createDatasets();
            }
            catch (const std::exception& exception)
            {
                QMessageBox::critical(owner, QObject::tr("Phantom dataset creation failed"), QString::fromUtf8(exception.what()));
            }
        });

        if (dialog.exec() != QDialog::Accepted)
            return;

        try
        {
            createDatasets();
        }
        catch (const std::exception& exception)
        {
            QMessageBox::critical(owner, QObject::tr("Phantom dataset creation failed"), QString::fromUtf8(exception.what()));
        }
    });
}

namespace
{
const bool registered = [] {
    mv::developer::registerMenuPopulator(QStringLiteral("PointData.DataSynthesis.PhantomGenerator"),
        [](QMenu& menu, QWidget* owner) { PointDataPhantomGenerator::populateMenu(menu, owner); });
    return true;
}();
}
}
