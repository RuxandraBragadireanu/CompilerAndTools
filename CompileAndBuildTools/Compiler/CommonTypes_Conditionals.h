#ifndef COMMON_TYPES_CONDITIONALS_H
#define COMMON_TYPES_CONDITIONALS_H

enum SymbolArrivedResult
{
	SAR_NOT_NEEDED = 0,
	SAR_NEEDED,
	SAR_NEEDED_AND_FINAL,
};

#include <set>
typedef std::set<std::string> ExpectedConditionInputs;
typedef ExpectedConditionInputs::iterator ExpectedConditionInputsIter;

#endif