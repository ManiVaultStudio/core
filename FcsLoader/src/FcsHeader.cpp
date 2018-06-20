#include "FcsHeader.h"

#include <sstream>

FcsHeader::FcsHeader(FcsHeader* copy) :
    _numEvents(0)
{
    if (copy != nullptr)
    {
        setFileName(copy->getFileName());
        setFcsVersion(copy->getFcsVersion());

        setNumEvents(copy->getNumEvents());

        _markerList[0] = *(copy->markerList(0));
        _markerList[1] = *(copy->markerList(1));

        createVariables(copy->getNumVariables());
        for (int i = 0; i < _variables.size(); i++)
        {
            _variables[i] = *(copy->getVariable(i));
        }

        setStartTime(copy->getStartTime());
        setStopTime(copy->getStopTime());
        setCytometry(copy->getCytometry());
        setDate(copy->getDate());
        setByteOrder(copy->getByteOrder());
        setDataType(copy->getDataType());
        setSystem(copy->getSystem());
        setProject(copy->getProject());
        setExperiment(copy->getExperiment());
        setCells(copy->getCells());
        setCreator(copy->getCreator());
    }
}

void FcsHeader::initDerivedData()
{
    for (int i = 0; i < _variables.size(); i++)
    {
        _markerList[0].push_back(_variables[i].name[0]);
        _markerList[1].push_back(_variables[i].name[1]);
    }
}

FcsHeader::~FcsHeader()
{

}

std::vector<FcsVariable>* FcsHeader::createVariables(int numVars)
{
    _variables.resize(numVars);

    return &_variables;
}

std::string FcsHeader::printHeader()
{
    std::stringstream structuredHeaderString;
    structuredHeaderString << _fileName << "\n";
    structuredHeaderString << _fcsVersion << "\n\n";

    if (_numEvents < 0) structuredHeaderString << "different numbers of Events\n";
    else structuredHeaderString << _numEvents << " Events\n";
    structuredHeaderString << _variables.size() << " Variables\n\n";

    if (_startTime.size() > 0) structuredHeaderString << "Start Time: " << _startTime << "\n";
    if (_stopTime.size() > 0) structuredHeaderString << "Stop Time: " << _stopTime << "\n";
    if (_date.size() > 0) structuredHeaderString << "Date: " << _date << "\n\n";

    if (_byteOrder.size() > 0) structuredHeaderString << "Byte Order: " << _byteOrder << "\n";
    if (_dataType.size() > 0) structuredHeaderString << "Datatype: " << _dataType << "\n\n";

    if (_cytometry.size() > 0) structuredHeaderString << "Cytometry: " << _cytometry << "\n";
    if (_system.size() > 0) structuredHeaderString << "System: " << _system << "\n";
    if (_project.size() > 0) structuredHeaderString << "Project: " << _project << "\n";
    if (_experiment.size() > 0) structuredHeaderString << "Experiment: " << _experiment << "\n";
    if (_cells.size() > 0) structuredHeaderString << "Cells: " << _cells << "\n\n";

    if (_creator.size() > 0) structuredHeaderString << "Creator: " << _creator << "\n\n\n";

    if (_variables.size() > 0) structuredHeaderString << "========== Variables ==========\n";

    for (int i = 0; i < _variables.size(); i++)
    {
        structuredHeaderString << "\n--" << _variables[i].name[0] << " // " << _variables[i].name[1] << " --\n";
        structuredHeaderString << "Range: " << _variables[i].range << "\n";
        structuredHeaderString << _variables[i].numBits << " bits\n";
        structuredHeaderString << "Exponent: " << _variables[i].exponent << "\n";
    }

    return structuredHeaderString.str();
}

std::vector<std::string>* FcsHeader::markerList(int which)
{
    return &_markerList[which];
}

std::string FcsHeader::getFileName()
{
    return _fileName;
}

void FcsHeader::setFileName(std::string fileName)
{
    _fileName = fileName;
}

int FcsHeader::getNumEvents()
{
    return _numEvents;
}

void FcsHeader::setNumEvents(int numEvents)
{
    _numEvents = numEvents;
}

int FcsHeader::getNumVariables()
{
    return _variables.size();
}

FcsVariable* FcsHeader::getVariable(int index)
{
    FcsVariable* var = nullptr;
    if (index >= 0 && index < _variables.size())
    {
        var = &(_variables[index]);
    }
    return var;
}

FcsVariable* FcsHeader::getVariable(std::string name)
{
    FcsVariable* var = nullptr;
    int index = getVariableIndex(name);
    if (index >= 0)
    {
        var = &(_variables[index]);
    }
    return var;
}

int FcsHeader::getVariableIndex(std::string name)
{
    int idx = -1;

    for (int index = 0; index < _variables.size(); index++)
    {
        if (_variables[index].name[0] == name)
        {
            idx = index;
            break;
        }
    }

    return idx;
}

std::string FcsHeader::getVariableName(int index)
{
    std::string name = std::string();

    if (index >= 0 && index < _variables.size())
    {
        name = _variables[index].name[0];
    }

    return name;
}

std::vector<FcsVariable>* FcsHeader::getVariables()
{
    return &_variables;
}

void FcsHeader::setVariable(int index, FcsVariable variable)
{
    if (index >= 0 && index < _variables.size())
    {
        _variables[index] = variable;

        _markerList[0][index] = variable.name[0];
        _markerList[1][index] = variable.name[1];
    }
}

void FcsHeader::addDimension(std::string name)
{
    FcsVariable variable;

    variable.name[0] = name;
    variable.name[1] = name;
    variable.range = 0;
    variable.numBits = 32;
    variable.exponent = 1;

    _variables.push_back(variable);

    _markerList[0].push_back(name);
    _markerList[1].push_back(name);
}

std::string FcsHeader::getStartTime()
{
    return _startTime;
}

std::string FcsHeader::getStopTime()
{
    return _stopTime;
}

std::string FcsHeader::getFcsVersion()
{
    return _fcsVersion;
}

std::string FcsHeader::getCytometry()
{
    return _cytometry;
}

std::string FcsHeader::getDate()
{
    return _date;
}

std::string FcsHeader::getByteOrder()
{
    return _byteOrder;
}

std::string FcsHeader::getDataType()
{
    return _dataType;
}

std::string FcsHeader::getSystem()
{
    return _system;
}

std::string FcsHeader::getProject()
{
    return _project;
}

std::string FcsHeader::getExperiment()
{
    return _experiment;
}

std::string FcsHeader::getCells()
{
    return _cells;
}

std::string FcsHeader::getCreator()
{
    return _creator;
}

void FcsHeader::setStartTime(std::string startTime)
{
    _startTime = startTime;
}

void FcsHeader::setStopTime(std::string stopTime)
{
    _stopTime = stopTime;
}

void FcsHeader::setFcsVersion(std::string fcsVersion)
{
    _fcsVersion = fcsVersion;
}

void FcsHeader::setCytometry(std::string cytometry)
{
    _cytometry = cytometry;
}

void FcsHeader::setDate(std::string date)
{
    _date = date;
}

void FcsHeader::setByteOrder(std::string byteOrder)
{
    _byteOrder = byteOrder;
}

void FcsHeader::setDataType(std::string dataType)
{
    _dataType = dataType;
}

void FcsHeader::setSystem(std::string system)
{
    _system = system;
}

void FcsHeader::setProject(std::string project)
{
    _project = project;
}

void FcsHeader::setExperiment(std::string experiment)
{
    _experiment = experiment;
}

void FcsHeader::setCells(std::string cells)
{
    _cells = cells;
}

void FcsHeader::setCreator(std::string creator)
{
    _creator = creator;
}
