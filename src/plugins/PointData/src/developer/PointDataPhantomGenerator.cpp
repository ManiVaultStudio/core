#include "PointDataPhantomGenerator.h"

#include "../PointData.h"
#include "../storage/TiledStorageAsymmetryAction.h"
#include "../storage/TiledStorageGeometry.h"

#include <Application.h>
#include <CoreInterface.h>
#include <DeveloperActionRegistry.h>
#include <actions/IntegralAction.h>
#include <actions/OptionAction.h>
#include <actions/StringAction.h>
#include <actions/VerticalGroupAction.h>
#include <util/StyledIcon.h>

#include <QDialog>
#include <QDialogButtonBox>
#include <QVBoxLayout>

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <limits>
#include <random>

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

/**
 * @brief Collects settings for a developer-generated phantom dataset.
 *
 * The dialog keeps dataset dimensions, element representation, population
 * strategy, storage backend, tile asymmetry, and cache capacity together so the
 * developer menu can create repeatable large-data scenarios without embedding
 * configuration in the generator callback. It supports both fully materialized
 * eager data and generator-backed tiled data. For tiled data, the row and
 * column tile sizes are optimized from the data shape and selected asymmetry.
 *
 * @author T. Kroes
 */
class PhantomDialog final : public QDialog
{
public:
    explicit PhantomDialog(QWidget* parent) :
        QDialog(parent),
        _settingsGroupAction(this, "Phantom points"),
        _nameAction(&_settingsGroupAction, "Dataset name", "Phantom points"),
        _rowsAction(&_settingsGroupAction, "Rows", 1, std::numeric_limits<std::int32_t>::max(), 1000000),
        _columnsAction(&_settingsGroupAction, "Dimensions", 1, 100000, 32),
        _typeAction(&_settingsGroupAction, "Element type"),
        _populationAction(&_settingsGroupAction, "Population", { "Deterministic", "Random floats", "Random zeros and ones", "All zeros" }, "Deterministic"),
        _storageBackendAction(&_settingsGroupAction, "Storage backend", { "Eager", "Tiled" }, "Tiled"),
        _tileAsymmetryAction(&_settingsGroupAction),
        _tileSizeAction(&_settingsGroupAction, "Tile size (MiB)", 1, 1024, 1),
        _tileGeometryAction(&_settingsGroupAction, "Tile geometry"),
        _cacheCapacityAction(&_settingsGroupAction, "Cache tiles", 1, 100000, 8),
        _estimateAction(&_settingsGroupAction, "Estimate")
    {
        setWindowTitle(tr("Create phantom points dataset"));

        QStringList typeNames;

        for (const auto* typeName : Points::getElementTypeNames())
            typeNames.push_back(QString::fromLatin1(typeName));

        _typeAction.initialize(typeNames, typeNames.first());

        _settingsGroupAction.setShowLabels(true);
        _nameAction.setDefaultWidgetFlags(mv::gui::StringAction::LineEdit);
        _rowsAction.setDefaultWidgetFlags(mv::gui::IntegralAction::SpinBox);
        _columnsAction.setDefaultWidgetFlags(mv::gui::IntegralAction::SpinBox);
        _typeAction.setDefaultWidgetFlags(mv::gui::OptionAction::ComboBox);
        _populationAction.setDefaultWidgetFlags(mv::gui::OptionAction::ComboBox);
        _storageBackendAction.setDefaultWidgetFlags(mv::gui::OptionAction::ComboBox);
        _tileSizeAction.setDefaultWidgetFlags(mv::gui::IntegralAction::SpinBox);
        _tileGeometryAction.setDefaultWidgetFlags(mv::gui::StringAction::TextEdit);
        _tileGeometryAction.setEnabled(false);
        _cacheCapacityAction.setDefaultWidgetFlags(mv::gui::IntegralAction::SpinBox);
        _estimateAction.setDefaultWidgetFlags(mv::gui::StringAction::TextEdit);
        _estimateAction.setEnabled(false);

        _settingsGroupAction.addAction(&_nameAction);
        _settingsGroupAction.addAction(&_rowsAction);
        _settingsGroupAction.addAction(&_columnsAction);
        _settingsGroupAction.addAction(&_typeAction);
        _settingsGroupAction.addAction(&_populationAction);
        _settingsGroupAction.addAction(&_storageBackendAction);
        _settingsGroupAction.addAction(&_tileAsymmetryAction);
        _settingsGroupAction.addAction(&_tileSizeAction);
        _settingsGroupAction.addAction(&_tileGeometryAction);
        _settingsGroupAction.addAction(&_cacheCapacityAction);
        _settingsGroupAction.addAction(&_estimateAction);

        auto* layout = new QVBoxLayout(this);
        layout->addWidget(_settingsGroupAction.createWidget(this));

        auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
        layout->addWidget(buttons);

        connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
        connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

        connect(&_rowsAction, &mv::gui::IntegralAction::valueChanged, this, [this] { updateTileGeometry(); });
        connect(&_columnsAction, &mv::gui::IntegralAction::valueChanged, this, [this] { updateTileGeometry(); });
        connect(&_typeAction, &mv::gui::OptionAction::currentIndexChanged, this, [this] { updateTileGeometry(); });
        connect(&_storageBackendAction, &mv::gui::OptionAction::currentIndexChanged, this, [this] { updateActions(); });
        connect(&_tileAsymmetryAction.getAsymmetryAction(), &mv::gui::IntegralAction::valueChanged, this, [this] { updateTileGeometry(); });
        connect(&_tileSizeAction, &mv::gui::IntegralAction::valueChanged, this, [this] { updateTileGeometry(); });
        connect(&_cacheCapacityAction, &mv::gui::IntegralAction::valueChanged, this, [this] { updateEstimate(); });

        loadSettings();
        updateActions();
    }

    QString name() const
    {
        const auto name = _nameAction.getString().trimmed();
        return name.isEmpty() ? QStringLiteral("Phantom points") : name;
    }

    std::uint64_t rows() const { return static_cast<std::uint64_t>(_rowsAction.getValue()); }

    std::uint64_t columns() const { return static_cast<std::uint64_t>(_columnsAction.getValue()); }

    std::size_t rowTileSize() const { return storage::calculateTiledStorageGeometry(rows(), columns(), elementSizeForType(type()), tileSizeBytes(), _tileAsymmetryAction.getAsymmetryAction().getValue()).rowTileSize; }

    std::size_t columnTileSize() const { return storage::calculateTiledStorageGeometry(rows(), columns(), elementSizeForType(type()), tileSizeBytes(), _tileAsymmetryAction.getAsymmetryAction().getValue()).columnTileSize; }

    std::uint64_t tileSizeBytes() const { return static_cast<std::uint64_t>(_tileSizeAction.getValue()) * 1024 * 1024; }

    std::size_t cacheCapacity() const { return static_cast<std::size_t>(_cacheCapacityAction.getValue()); }

    bool isTiled() const { return _storageBackendAction.getCurrentText() == "Tiled"; }

    Points::ElementTypeSpecifier type() const { return Points::elementTypeSpecifier(_typeAction.getCurrentText()); }

    Population population() const { return static_cast<Population>(_populationAction.getCurrentIndex()); }

    void saveSettings() const
    {
        auto* application = Application::current();

        application->setSetting("Developer/PhantomPointGenerator/Name", _nameAction.getString());
        application->setSetting("Developer/PhantomPointGenerator/Rows", _rowsAction.getValue());
        application->setSetting("Developer/PhantomPointGenerator/Columns", _columnsAction.getValue());
        application->setSetting("Developer/PhantomPointGenerator/ElementType", _typeAction.getCurrentText());
        application->setSetting("Developer/PhantomPointGenerator/Population", _populationAction.getCurrentText());
        application->setSetting("Developer/PhantomPointGenerator/StorageBackend", _storageBackendAction.getCurrentText());
        application->setSetting("Developer/PhantomPointGenerator/TileAsymmetry", _tileAsymmetryAction.getAsymmetryAction().getValue());
        application->setSetting("Developer/PhantomPointGenerator/TileSizeMiB", _tileSizeAction.getValue());
        application->setSetting("Developer/PhantomPointGenerator/CacheCapacity", _cacheCapacityAction.getValue());
    }

private:
    void loadSettings()
    {
        auto* application = Application::current();

        _nameAction.setString(application->getSetting("Developer/PhantomPointGenerator/Name", _nameAction.getString()).toString());
        _rowsAction.setValue(application->getSetting("Developer/PhantomPointGenerator/Rows", _rowsAction.getValue()).toInt());
        _columnsAction.setValue(application->getSetting("Developer/PhantomPointGenerator/Columns", _columnsAction.getValue()).toInt());
        _typeAction.setCurrentText(application->getSetting("Developer/PhantomPointGenerator/ElementType", _typeAction.getCurrentText()).toString());
        _populationAction.setCurrentText(application->getSetting("Developer/PhantomPointGenerator/Population", _populationAction.getCurrentText()).toString());
        _storageBackendAction.setCurrentText(application->getSetting("Developer/PhantomPointGenerator/StorageBackend", _storageBackendAction.getCurrentText()).toString());
        const auto asymmetry = application->getSetting("Developer/PhantomPointGenerator/TileAsymmetry", application->getSetting("Developer/PhantomPointGenerator/TileDominance", _tileAsymmetryAction.getAsymmetryAction().getValue())).toInt();
        _tileAsymmetryAction.getAsymmetryAction().setValue(asymmetry);
        _tileSizeAction.setValue(application->getSetting("Developer/PhantomPointGenerator/TileSizeMiB", _tileSizeAction.getValue()).toInt());
        _cacheCapacityAction.setValue(application->getSetting("Developer/PhantomPointGenerator/CacheCapacity", _cacheCapacityAction.getValue()).toInt());
    }

    void updateActions()
    {
        const auto tiled = isTiled();
        _tileAsymmetryAction.setEnabled(tiled);
        _tileGeometryAction.setEnabled(tiled);
        _cacheCapacityAction.setEnabled(tiled);
        updateEstimate();
    }

    void updateTileGeometry()
    {
        const auto geometry = storage::calculateTiledStorageGeometry(rows(), columns(), elementSizeForType(type()), tileSizeBytes(), _tileAsymmetryAction.getAsymmetryAction().getValue());
        _cacheCapacityAction.setValue(static_cast<std::int32_t>(storage::calculateTiledStorageCacheCapacity(rows(), columns(), geometry)));
        updateEstimate();
    }

    void updateEstimate()
    {
        const auto type        = Points::elementTypeSpecifier(_typeAction.getCurrentText());
        const auto elementSize = elementSizeForType(type);

        const auto geometry = storage::calculateTiledStorageGeometry(rows(), columns(), elementSize, tileSizeBytes(), _tileAsymmetryAction.getAsymmetryAction().getValue());
        const auto estimate = Points::estimateTiledMemory(rows(), columns(), elementSize, geometry.rowTileSize, geometry.columnTileSize, cacheCapacity());

        _tileGeometryAction.setString(QString("Rows per tile: %1\nColumns per tile: %2")
            .arg(geometry.rowTileSize)
            .arg(geometry.columnTileSize));

        _estimateAction.setString(QString("Storage backend: %1\nRaw size: %2\nLargest tile: %3\nEstimated cache: %4\nTiles: %5\nIndex estimate: %6")
            .arg(isTiled() ? "Tiled" : "Eager")
            .arg(formatBytes(estimate.rawDataBytes))
            .arg(formatBytes(estimate.largestTileBytes))
            .arg(formatBytes(estimate.cachedTileBytes))
            .arg(estimate.tileCount)
            .arg(formatBytes(estimate.indexBytes)));
    }

    mv::gui::VerticalGroupAction         _settingsGroupAction;
    mv::gui::StringAction                _nameAction;
    mv::gui::IntegralAction              _rowsAction;
    mv::gui::IntegralAction              _columnsAction;
    mv::gui::OptionAction                _typeAction;
    mv::gui::OptionAction                _populationAction;
    mv::gui::OptionAction                _storageBackendAction;
    mv::gui::TiledStorageAsymmetryAction _tileAsymmetryAction;
    mv::gui::IntegralAction              _tileSizeAction;
    mv::gui::StringAction                 _tileGeometryAction;
    mv::gui::IntegralAction               _cacheCapacityAction;
    mv::gui::StringAction                 _estimateAction;
};
}

void PointDataPhantomGenerator::populateMenu(QMenu& menu, QWidget* owner)
{
    auto* action = menu.addAction(mv::util::StyledIcon("table"), QObject::tr("Phantom points…"));
    action->setToolTip(QObject::tr("Create a synthetic eager or tiled points dataset"));
    QObject::connect(action, &QAction::triggered, owner, [owner] {
        PhantomDialog dialog(owner);
        if (dialog.exec() != QDialog::Accepted)
            return;

        dialog.saveSettings();

        const auto type = dialog.type();
        const auto population = dialog.population();
        auto dataset = mv::data().createDataset<Points>("Points", dialog.name());

        if (dialog.isTiled())
        {
            dataset->makeTiledFromGenerator(dialog.rows(), dialog.columns(), type, dialog.rowTileSize(), dialog.columnTileSize(), [type, population](const std::uint64_t rowOffset, const std::uint64_t columnOffset, const std::uint64_t rows, const std::uint64_t columns) {
                return makeTile(type, population, rowOffset, columnOffset, rows, columns);
            });
            dataset->setTiledCacheCapacity(dialog.cacheCapacity());
        }
        else
            populateEager(*dataset, type, population, dialog.rows(), dialog.columns());

        mv::help().addNotification(QObject::tr("Phantom points dataset created"),
            QObject::tr("%1 was created with %2 storage and has a raw logical size of %3.")
                .arg(dialog.name())
                .arg(dialog.isTiled() ? QObject::tr("tiled") : QObject::tr("eager"))
                .arg(formatBytes(dataset->getRawDataSize())), mv::util::StyledIcon("check"));
    });
}

namespace
{
const bool registered = [] {
    mv::developer::registerMenuPopulator(QStringLiteral("PointData.PhantomGenerator"),
        [](QMenu& menu, QWidget* owner) { PointDataPhantomGenerator::populateMenu(menu, owner); });
    return true;
}();
}
}
