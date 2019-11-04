#include "CsvLoader.h"

#include "PointData.h"
#include "Set.h"

#include <QtCore>
#include <QtDebug>

#include <QFileDialog>
#include <vector>
#include <QInputDialog>

Q_PLUGIN_METADATA(IID "nl.tudelft.CsvLoader")

// =============================================================================
// View
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

    std::vector<float> data;
    int numDimensions = 1;

    QTextStream in(&file);
    while (!in.atEnd())
    {
        QString line = in.readLine();
        QStringList tokens = line.split(",");

        numDimensions = tokens.size();
        for (int i = 0; i < tokens.size(); i++)
        {
            if (tokens[i].isEmpty())
                continue;
            float f = tokens[i].toFloat();

            data.push_back(f);
        }
    }

    bool ok;
    QString dataSetName = QInputDialog::getText(nullptr, "Add New Dataset",
        "Dataset name:", QLineEdit::Normal, "DataSet", &ok);

    if (ok && !dataSetName.isEmpty()) {
        QString name = _core->addData("Points", dataSetName);
        const IndexSet& set = dynamic_cast<const IndexSet&>(_core->requestSet(name));
        PointsPlugin& points = set.getData();
        
        points.setData(data.data(), data.size() / numDimensions, numDimensions);

        qDebug() << "Number of dimensions: " << points.getNumDimensions();

        _core->notifyDataAdded(name);

        qDebug() << "CSV file loaded. Num data points: " << points.getNumPoints();
    }
}

// =============================================================================
// Factory
// =============================================================================

LoaderPlugin* CsvLoaderFactory::produce()
{
    return new CsvLoader();
}
