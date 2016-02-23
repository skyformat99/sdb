#ifndef SDB_META_H_
#define SDB_META_H_

#include <string>
#include <vector>
#include <map>
#include <set>

class Root
{
public:
	~Root();
	static Root* load(const std::string &filename);
private:
	std::map<int, std::string> _items;
	std::set<int> _del_items;

	Root();
};

#endif
