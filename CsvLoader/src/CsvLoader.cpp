#include "CsvLoader.h"

#include "PointData.h"
#include "Set.h"

#include <QtCore>
#include <QtDebug>

#include <QFileDialog>
#include <QInputDialog>

Q_PLUGIN_METADATA(IID "nl.tudelft.CsvLoader")

// =============================================================================
// Loader
// =============================================================================

CsvLoader::~CsvLoader(void)
{
    
}

void CsvLoader::init()
{

}

void CsvLoader::loadData()
{
    QString fileName = QFileDialog::getOpenFileName(Q_NULLPTR, "Load File", "", "CSV Files (*.csv *)");
    
    // Don't try to load a file if the dialog was cancelled or the file name is empty
    if (fileName.isNull() || fileName.isEmpty())
        return;

    qDebug() << "Loading CSV file: " << fileName;
    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        throw DataLoadException(fileName, "File was not found at location.");

    QTextStream in(&file);
    while (!in.atEnd())
    {
        QString line = in.readLine();
        QStringList tokens = line.split(",");

        _loadedData.push_back(tokens);
    }

    // Gather some knowledge about the data from the user
    InputDialog inputDialog(nullptr);
    inputDialog.setModal(true);

    connect(&inputDialog, &InputDialog::closeDialog, this, &CsvLoader::dialogClosed);

    int inputOk = inputDialog.exec();
}

void CsvLoader::dialogClosed(QString dataSetName, bool hasHeaders)
{
    QStringList headers;
    int startIndex = 0;

    if (hasHeaders)
    {
        headers = _loadedData[0];
        startIndex = 1;
    }

    QString name = _core->addData("Points", dataSetName);
    Points& points = _core->requestData<Points>(name);

    std::vector<float> data;
    int numDimensions = 1;
    for (int i = startIndex; i < _loadedData.size(); i++)
    {
        QStringList items = _loadedData[i];
        numDimensions = items.size();
        for (int i = 0; i < items.size(); i++)
        {
            if (items[i].isEmpty())
                continue;
            float f = items[i].toFloat();

            data.push_back(f);
        }
    }

    points.setData(data.data(), data.size() / numDimensions, numDimensions);

    qDebug() << "Number of dimensions: " << points.getNumDimensions();

    _core->notifyDataAdded(name);

    qDebug() << "CSV file loaded. Num data points: " << points.getNumPoints();

    qDebug() << dataSetName << hasHeaders;
}

// =============================================================================
// Factory
// =============================================================================

LoaderPlugin* CsvLoaderFactory::produce()
{
    return new CsvLoader();
}
