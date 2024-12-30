#include <string>
#include "BufferManager.h"

//IndexManager.h - manages all indexes in the database
class IndexManager {
    //handles index creation, deletion, and maintenance
    void createIndex(const string& tableName, const string& columnName);
    void dropIndex(const string& indexName);
    void updateIndex(const string& indexName, int key, RecordPtr newLocation);
};
