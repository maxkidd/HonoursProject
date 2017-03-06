#ifndef _NETWORKDEBUGDATASOURCE_H_
#define _NETWORKDEBUGDATASOURCE_H_

#include "cocos2d.h"
#include "cocos-ext.h"

#include <vector>
#include <list>

using namespace cocos2d;
using namespace cocos2d::extension;

enum NetworkDebugType
{
	NET_LOG,
	NET_ERR
};

class NetworkDebugDataSource : public TableViewDataSource
{
private:
	int _numCells = 0;

	std::deque<std::pair<NetworkDebugType,std::string>> _debugData;
public:
	void createEntry(std::string str, NetworkDebugType type);

	Size cellSizeForTable(TableView *table);
	TableViewCell* tableCellAtIndex(TableView *table, ssize_t idx); // Override
	ssize_t numberOfCellsInTableView(TableView *table); // Override
};

#endif