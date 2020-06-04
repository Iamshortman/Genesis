#pragma once

#include <string>

#include <vector>
#include <set>
#include <map>
#include <unordered_map>
#include <queue>

#define has_value(list, value) (list.find(value) != list.end())

namespace Genesis
{
	typedef double TimeStep;

	//Puts these std classes in the Genesis namespace
	using std::string;
	using std::vector;
	using std::set;
	using std::map;
	using std::unordered_map;
	using std::queue;
};
