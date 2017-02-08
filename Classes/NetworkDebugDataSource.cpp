#include "NetworkDebugDataSource.h"

void NetworkDebugDataSource::createEntry(std::string str, NetworkDebugType type)
{
	_debugData.push_back(std::make_pair(type, std::to_string(_debugData.size()) + ": " + str));
}

Size NetworkDebugDataSource::cellSizeForTable(TableView * table)
{
	return Size(200.0f, 20.0f);
}

TableViewCell * NetworkDebugDataSource::tableCellAtIndex(TableView * table, ssize_t idx)
{

	TableViewCell* viewCell = table->dequeueCell();

	std::string str;
	Color3B col;
	if (idx >= _debugData.size() || idx < 0)
	{
		str = "NULL";
		col = Color3B::WHITE;
	}
	else
	{
		str = _debugData.at(idx).second;
		switch (_debugData.at(idx).first)
		{
		case NET_LOG:
			col = Color3B::ORANGE;
			break;
		case NET_ERR:
			col = Color3B::RED;
			break;
		default:
			break;
		}
	}


	if (viewCell)
	{
		LabelTTF* label = (LabelTTF*)viewCell->getChildByTag(1);
		label->setString(str);
		label->setFontFillColor(col);
	}
	else
	{
		viewCell = TableViewCell::create();
		LabelTTF* label = LabelTTF::create(str, "Ariel", 20.0f);
		
		label->setPosition(Vec2::ZERO);
		label->setAnchorPoint(Vec2::ZERO);
		label->setTag(1);

		label->setString(str);
		label->setFontFillColor(col);

		viewCell->addChild(label);
	}

	return viewCell;
}

ssize_t NetworkDebugDataSource::numberOfCellsInTableView(TableView * table)
{
	return _debugData.size();
}
