#pragma once

#include <string>
#include <vector>

struct FcsVariable
{
    bool operator==(const FcsVariable& rhs)
    {
        return (name[0] == rhs.name[0]) && (name[1] == rhs.name[1]) &&
               (numBits == rhs.numBits) && (exponent == rhs.exponent);
    }

    bool operator!=(const FcsVariable& rhs)
    {
        return !(*this == rhs);
    }

    std::string name[2];
    int range;
    int numBits;
    double exponent;
};

class FcsHeader
{
public:
    FcsHeader(FcsHeader* copy = nullptr);
    ~FcsHeader();

    void initDerivedData();
    std::vector<FcsVariable>* createVariables(int numVars);
    std::string printHeader();
    std::vector<std::string>* markerList(int which);

    std::string getFileName();
    void setFileName(std::string fileName);
    
    int getNumEvents();
    void setNumEvents(int numEvents);

    int getNumVariables();
    FcsVariable* getVariable(int index);
    FcsVariable* getVariable(std::string name);
    int getVariableIndex(std::string name);
    std::string getVariableName(int index);
    std::vector<FcsVariable>* getVariables();
    
    void setVariable(int index, FcsVariable variable);
    
    void addDimension(std::string name);

    std::string getStartTime();
    std::string getStopTime();
    std::string getFcsVersion();
    std::string getCytometry();
    std::string getDate();
    std::string getByteOrder();
    std::string getDataType();
    std::string getSystem();
    std::string getProject();
    std::string getExperiment();
    std::string getCells();
    std::string getCreator();

    void setStartTime(std::string startTime);
    void setStopTime(std::string stopTime);
    void setFcsVersion(std::string fcsVersion);
    void setCytometry(std::string cytometry);
    void setDate(std::string date);
    void setByteOrder(std::string byteOrder);
    void setDataType(std::string dataType);
    void setSystem(std::string system);
    void setProject(std::string project);
    void setExperiment(std::string experiment);
    void setCells(std::string cells);
    void setCreator(std::string creator);

private:
    std::string _fileName;

    std::string _fcsVersion;

    std::vector<std::string> _markerList[2];

    int _numEvents;

    std::vector<FcsVariable> _variables;

    std::string _startTime;
    std::string _stopTime;
    std::string _cytometry;
    std::string _date;
    std::string _byteOrder;
    std::string _dataType;
    std::string _system;
    std::string _project;
    std::string _experiment;
    std::string _cells;
    std::string _creator;
};
