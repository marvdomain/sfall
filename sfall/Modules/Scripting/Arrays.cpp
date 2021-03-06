#include <set>
#include <algorithm>

#include "..\ScriptExtender.h"
#include "..\Scripting\ScriptValue.h"
#include "..\Scripting\OpcodeContext.h"

#include "Arrays.h"

namespace sfall
{
namespace script
{

/*
	GLOBAL variable for arrays
*/
DWORD arraysBehavior = 1; // 0 - backward compatible with pre-3.4, 1 - permanent arrays don't get stored in savegames by default
// arrays map: arrayId => arrayVar
ArraysMap arrays;
// auto-incremented ID
DWORD nextarrayid = 1;
// temp arrays: set of arrayId
std::set<DWORD> tempArrays;
// saved arrays: arrayKey => arrayId
ArrayKeysMap savedArrays;

// special array ID for array expressions
DWORD stackArrayId;

static char get_all_arrays_special_key[] = "...all_arrays...";

sArrayElement::sArrayElement() : len(0), intVal(0), type(DataType::NONE)
{

}

sArrayElement::sArrayElement( DWORD _val, DataType _dataType ) : len(0), type(_dataType), intVal(_val)
{
	if (DataType::STR == _dataType)
		len = strlen((char*)_val);
}

sArrayElement::sArrayElement(const long& other)
{
	set(other);
}

void sArrayElement::clear()
{
	if (type == DataType::STR && strVal)
		delete[] strVal;
}

void sArrayElement::setByType( DWORD val, DataType dataType )
{
	switch(dataType) {
	case DataType::STR:
		set((char*)val);
		break;
	case DataType::INT:
		set(*(long*)&val);
		break;
	case DataType::FLOAT:
		set(*(float*)&val);
		break;
	}
}

void sArrayElement::set( long val )
{
	clear();
	type = DataType::INT;
	intVal = val;
}

void sArrayElement::set( float val )
{
	clear();
	type = DataType::FLOAT;
	floatVal = val;
}

void sArrayElement::set( const char* val, int _len /*= -1*/ )
{
	clear();
	type = DataType::STR;
	if (_len == -1)
		_len = strlen(val);
	if (_len > ARRAY_MAX_STRING-1) // memory safety
		_len = ARRAY_MAX_STRING-1;
	len = _len + 1;
	strVal = new char[len];
	memcpy(strVal, val, _len);
	strVal[_len] = '\0';
}

void sArrayElement::unset()
{
	clear();
	type = DataType::NONE;
	len = intVal = 0;
}

DWORD sArrayElement::getHashStatic(DWORD value, DataType type) {
	switch (type) {
	case DataType::STR:
		const char* str;
		str = (const char*)value;
		int i;
		DWORD res;
		for (i=0, res=0; str[i]!='\0'; i++) {
			res = ((res << 5) + res) + str[i];
		}
		return res;
	case DataType::INT:
	case DataType::FLOAT:
		return value;
	default:
		return 0;
	}
}

bool sArrayElement::operator<( const sArrayElement &el ) const
{
	if (type < el.type) {
		return true;
	} else if (type == el.type) {
		switch (type) {
		case DataType::STR:
			return strcmp(strVal, el.strVal) < 0;
		case DataType::FLOAT:
			return floatVal < el.floatVal;
		case DataType::INT:
		default:
			return intVal < el.intVal;
		}
	} else {
		return false;
	}
}

bool sArrayElement_EqualFunc::operator()( const sArrayElement &elA, const sArrayElement &elB ) const
{
	if (elA.type != elB.type)
		return false;
	switch (elA.type) {
	case DataType::STR:
		return strcmp(elA.strVal, elB.strVal) == 0;
	case DataType::INT:
	case DataType::FLOAT:
		return elA.intVal == elB.intVal;
	default:
		return true;
	}
}

void sArrayVar::clearRange( int from, int to /*= -1*/ )
{
	if (to == -1)
		to = val.size();
	std::vector<sArrayElement>::iterator it, itTo = val.begin() + to;
	for (it = val.begin() + from; it < itTo; ++it) {
		it->clear();
	}
}

void SaveArrayElement(sArrayElement* el, HANDLE h)
{
	DWORD unused;
	WriteFile(h, &el->type, 4, &unused, 0);
	if (el->type == DataType::STR) {
		WriteFile(h, &el->len, 4, &unused, 0);
		WriteFile(h, el->strVal, el->len, &unused, 0);
	} else {
		WriteFile(h, &el->intVal, 4, &unused, 0);
	}
}

void LoadArrayElement(sArrayElement* el, HANDLE h)
{
	DWORD unused;
	ReadFile(h, &el->type, 4, &unused, 0);
	if (el->type == DataType::STR) {
		ReadFile(h, &el->len, 4, &unused, 0);
		if (el->len > 0) {
			el->strVal = new char[el->len];
			ReadFile(h, el->strVal, el->len, &unused, 0);
		} else
			el->strVal = nullptr;
	} else {
		ReadFile(h, &el->intVal, 4, &unused, 0);
	}
}

void LoadArraysOld(HANDLE h) {
	dlogr("Loading arrays (old fmt)", DL_MAIN);
	DWORD count, unused, id, j;
	ReadFile(h, &count, 4, &unused, 0);
	if(unused!=4) return;
	sArrayVarOld var;
	sArrayVar varN;
	for(DWORD i=0;i<count;i++) {
		ReadFile(h, &id, 4, &unused, 0);
		ReadFile(h, &var, 8, &unused, 0);
		var.types=new DWORD[var.len];
		var.data=new char[var.len*var.datalen];
		ReadFile(h, var.types, 4*var.len, &unused, 0);
		ReadFile(h, var.data, var.len*var.datalen, &unused, 0);
		varN.flags = 0;
		varN.val.resize(var.len);
		for (j=0; j<var.len; j++) {
			switch (var.types[j]) {
			case DataType::INT:
				varN.val[j].set(*(long*)(&var.data[var.datalen*j]));
				break;
			case DataType::FLOAT:
				varN.val[j].set(*(float*)(&var.data[var.datalen*j]));
				break;
			case DataType::STR:
				varN.val[j].set(&var.data[var.datalen*j], var.datalen - 1);
				break;
			}
		}
		delete[] var.types;
		delete[] var.data;
		varN.key = sArrayElement(id, DataType::INT);
		arrays.insert(array_pair(id, varN));
		savedArrays[varN.key] = id;
	}
}

void LoadArrays(HANDLE h) {
	DWORD count, elCount, unused, j;
	LoadArraysOld(h);
	dlogr("Loading arrays (new fmt)", DL_MAIN);
	ReadFile(h, &count, 4, &unused, 0);
	if (unused != 4) return;
	sArrayVar arrayVar;
	nextarrayid = 1;
	for (DWORD i=0; i<count; i++) {
		LoadArrayElement(&arrayVar.key, h);
		if (static_cast<long>(arrayVar.key.type) > 4 || arrayVar.key.intVal == 0) { // partial compatibility with 3.4
			arrayVar.key.intVal = static_cast<long>(arrayVar.key.type);
			arrayVar.key.type = DataType::INT;
		}
		ReadFile(h, &arrayVar.flags, 4, &unused, 0);
		ReadFile(h, &elCount, 4, &unused, 0); // actual number of elements: keys+values
		bool isAssoc = arrayVar.isAssoc();
		arrayVar.val.resize(elCount);
		for (j=0; j<elCount; j++) { // normal and associative arrays stored and loaded equally
			LoadArrayElement(&arrayVar.val[j], h);
			if (isAssoc && (j % 2) == 0) { // only difference is that keyHash is filled with appropriate indexes
				arrayVar.keyHash[arrayVar.val[j]] = j;
			}
		}
		while (arrays.find(nextarrayid) != arrays.end()) nextarrayid++;
		if (nextarrayid == 0) nextarrayid++;
		arrays.insert(array_pair(nextarrayid, arrayVar));
		savedArrays[arrayVar.key] = nextarrayid++;
		arrayVar.keyHash.clear();
	}
}

void SaveArrays(HANDLE h) {
	DWORD count, elCount, unused;
	array_itr arIt;
	count = 0;
	WriteFile(h, &count, 4, &unused, 0); // this is for backward compatibility with 3.3-
	ArrayKeysMap::iterator it = savedArrays.begin();
	while (it != savedArrays.end()) {
		arIt = arrays.find(it->second);
		if (arIt == arrays.end()) {
			savedArrays.erase(it++);
		} else {
			++count;
			++it;
		}
	}
	WriteFile(h, &count, 4, &unused, 0);
	for (it = savedArrays.begin(); it != savedArrays.end(); ++it) {
		arIt = arrays.find(it->second);
		if (arIt != arrays.end()) {
			sArrayVar &var = arrays[it->second];
			SaveArrayElement(&var.key, h);
			WriteFile(h, &var.flags, 4, &unused, 0);
			elCount = var.val.size();
			WriteFile(h, &elCount, 4, &unused, 0);
			for (std::vector<sArrayElement>::iterator it = var.val.begin(); it != var.val.end(); ++it) {
				SaveArrayElement(&(*it), h);
			}
		}
	}
}

int GetNumArrays() {
	return arrays.size();
}

void GetArrays(int* _arrays) {
	int pos=0;
	array_citr itr=arrays.begin();
	while(itr!=arrays.end()) {
		_arrays[pos++]=itr->first;
		_arrays[pos++]=itr->second.size();
		_arrays[pos++]=itr->second.flags;
		itr++;
	}
}

// those too are not really used yet in FalloutClient (AFAIK) -- phobos2077
void DEGetArray(int id, DWORD* types, void* data) {
	//memcpy(types, arrays[id].types, arrays[id].len*4);
	//memcpy(data, arrays[id].data, arrays[id].len*arrays[id].datalen);
}

void DESetArray(int id, const DWORD* types, const void* data) {
	//if(types) memcpy(arrays[id].types, types, arrays[id].len*4);
	//memcpy(arrays[id].data, data, arrays[id].len*arrays[id].datalen);
}


/*
	Array manipulation functions for script operators
	TODO: move somewhere else
*/

DWORD _stdcall CreateArray(int len, DWORD nothing) {
	sArrayVar var;
	if (len < 0) var.flags |= ARRAYFLAG_ASSOC;
	else if (len > ARRAY_MAX_SIZE) len = ARRAY_MAX_SIZE; // safecheck
	if (!var.isAssoc()) {
		var.val.resize(len);
	}
	while(arrays.find(nextarrayid)!=arrays.end()) nextarrayid++;
	if (nextarrayid == 0) nextarrayid++;
	if (arraysBehavior == 0) {
		var.key = sArrayElement(nextarrayid, DataType::INT);
		savedArrays[var.key] = nextarrayid;
	}
	stackArrayId = nextarrayid;
	arrays[nextarrayid]=var;
	return nextarrayid++;
}

DWORD _stdcall TempArray(DWORD len, DWORD nothing) {
	DWORD id=CreateArray(len, nothing);
	tempArrays.insert(id);
	return id;
}

void _stdcall FreeArray(DWORD id) {
	array_itr it = arrays.find(id);
	if (it != arrays.end()) {
		savedArrays.erase(it->second.key);
		it->second.clear();
		arrays.erase(id);
	}
}

ScriptValue _stdcall GetArrayKey(DWORD id, int index) {
	if (arrays.find(id) == arrays.end() || index < -1 || index > arrays[id].size()) {
		return ScriptValue(0);
	}
	if (index == -1) { // special index to indicate if array is associative
		return ScriptValue(arrays[id].isAssoc());
	}
	if (arrays[id].isAssoc()) {
		index *= 2;
		// for associative array - return key at the specified index
		switch(arrays[id].val[index].type) {
		case DataType::INT:
			return ScriptValue(arrays[id].val[index].intVal);
		case DataType::FLOAT:
			return ScriptValue(arrays[id].val[index].floatVal);
		case DataType::STR:
			return ScriptValue(arrays[id].val[index].strVal);
		case DataType::NONE:
		default:
			return ScriptValue(0);
		}
	} else {
		// just return index for normal array..
		return ScriptValue(index);
	}
}

ScriptValue _stdcall GetArray(DWORD id, const ScriptValue& key) {
	if (arrays.find(id) == arrays.end()) {
		return 0;
	}
	int el;
	sArrayVar &arr = arrays[id];
	if (arr.isAssoc()) {
		ArrayKeysMap::iterator it = arr.keyHash.find(sArrayElement(key.rawValue(), key.type()));
		if (it != arr.keyHash.end())
			el = it->second + 1;
		else
			return 0;
	} else {
		el = key.asInt();
		// check for invalid index
		if (el < 0 || el >= arr.size()) {
			return 0;
		}
	}
	switch (arr.val[el].type) {
	case DataType::NONE:  return ScriptValue();
	case DataType::INT:
		return ScriptValue(arr.val[el].intVal);
	case DataType::FLOAT:
		return ScriptValue(arr.val[el].floatVal);
	case DataType::STR:
		return ScriptValue(arr.val[el].strVal);
	}
	return ScriptValue(0);
}

void _stdcall SetArray(DWORD id, const ScriptValue& key, const ScriptValue& val, bool allowUnset) {
	if (arrays.find(id) == arrays.end()) {
		return;
	}
	int el;
	sArrayVar &arr = arrays[id];
	if (arrays[id].isAssoc()) {
		sArrayElement sEl(key.rawValue(), key.type());
		ArrayKeysMap::iterator elIter = arr.keyHash.find(sEl);
		el = (elIter != arr.keyHash.end()) 
			? elIter->second 
			: -1;
		if (val.isInt() && val.asInt() == 0 && allowUnset) {
			// after assigning zero to a key, no need to store it, because "get_array" returns 0 for non-existent keys: try unset
			if (el >= 0) {
				// remove from hashtable
				arr.keyHash.erase(elIter);
				// shift all keyHash references
				for (elIter = arr.keyHash.begin(); elIter != arr.keyHash.end(); ++elIter) {
					if (elIter->second >= (DWORD)el)
						elIter->second -= 2;
				}
				// remove key=>value pair from vector
				arr.clearRange(el, el + 2);
				arr.val.erase(arr.val.begin() + el, arr.val.begin() + (el + 2));
			}
		} else {
			if (el == -1) {
				// size check
				if (arr.size() >= ARRAY_MAX_SIZE) return;
				// add pair
				el = arr.val.size();
				arr.val.resize(el + 2);
				arr.val[el].setByType(key.rawValue(), key.type()); // copy data
				arr.keyHash[arr.val[el]] = el;
			}
			arr.val[el+1].setByType(val.rawValue(), val.type());
		}
	} else {
		// only update normal array if key is an integer and within array size
		el = key.asInt();
		if (key.isInt() && arr.size() > el) {
			arr.val[el].setByType(val.rawValue(), val.type());
		}
	}
}

int _stdcall LenArray(DWORD id) {
	if(arrays.find(id)==arrays.end()) return -1;
	else return arrays[id].size();
}

void _stdcall ResizeArray(DWORD id, int newlen) {
	if (arrays.find(id) == arrays.end() || arrays[id].size() == newlen) return;
	sArrayVar &arr = arrays[id];
	if (arr.isAssoc()) {
		// only allow to reduce number of elements (adding range of elements is meaningless for maps)
		if (newlen < arrays[id].size()) {
			ArrayKeysMap::iterator itHash;
			std::vector<sArrayElement>::iterator itVal;
			int actualLen = newlen*2;
			for (itVal = arr.val.begin() + actualLen; itVal != arr.val.end(); itVal += 2) {
				if ((itHash = arr.keyHash.find(*itVal)) != arr.keyHash.end())
					arr.keyHash.erase(itHash);
			}
			arr.clearRange(actualLen);
			arr.val.resize(actualLen);
		}
		return;
	}
	if (newlen >= 0) { // actual resize
		if (newlen > ARRAY_MAX_SIZE) // safety
			newlen = ARRAY_MAX_SIZE;
		if (newlen < arr.size())
			arr.clearRange(newlen);
		arr.val.resize(newlen);
	} else { // special functions for lists...
		switch (newlen) {
		case ARRAY_ACTION_SORT: // sort ascending
			std::sort(arr.val.begin(), arr.val.end());
			break;
		case ARRAY_ACTION_RSORT: // sort descending
			std::sort(arr.val.rbegin(), arr.val.rend());
			break;
		case ARRAY_ACTION_REVERSE: // reverse elements
			std::reverse(arr.val.rbegin(), arr.val.rend());
			break;
		case ARRAY_ACTION_SHUFFLE: // shuffle elements
			std::random_shuffle(arr.val.rbegin(), arr.val.rend());
			break;
		}
	}
}

void _stdcall FixArray(DWORD id) {
	tempArrays.erase(id);
}

ScriptValue _stdcall ScanArray(DWORD id, const ScriptValue& val) {
	if (arrays.find(id) == arrays.end()) {
		return ScriptValue(-1);
	}
	char step = arrays[id].isAssoc() ? 2 : 1;
	for (size_t i = 0; i < arrays[id].val.size(); i += step) {
		sArrayElement &el = arrays[id].val[i + step - 1];
		if (el.type == val.type()) {
			 if ((!val.isString() && static_cast<DWORD>(el.intVal) == val.rawValue()) ||
				 (val.isString() && strcmp(el.strVal, val.asString()) == 0)) {
				 if (arrays[id].isAssoc()) { // return key instead of index for associative arrays
					 return ScriptValue(
						 static_cast<DataType>(arrays[id].val[i].type), 
						 static_cast<DWORD>(arrays[id].val[i].intVal)
					 );
				 } else {
					 return ScriptValue(static_cast<int>(i));
				 }
			 }
		}
	}
	return ScriptValue(-1);
}

DWORD _stdcall LoadArray(const ScriptValue& key) {
	if (!key.isInt() || key.asInt() != 0) { // returns arrayId by it's key (ignoring int(0) because it is used to "unsave" array)
		sArrayElement keyEl = sArrayElement(key.rawValue(), key.type());
		if (keyEl.type == DataType::STR && strcmp(keyEl.strVal, get_all_arrays_special_key) == 0) { // this is a special case to get temp array containing all saved arrays
			DWORD tmpArrId = TempArray(savedArrays.size(), 0);
			if (tmpArrId > 0) {
				std::vector<sArrayElement>::iterator elIt;
				ArrayKeysMap::iterator it;
				sArrayVar &tmpArr = arrays[tmpArrId];
				for (it = savedArrays.begin(), elIt = tmpArr.val.begin(); it != savedArrays.end(); ++it, ++elIt) {
					elIt->set(it->first);
				}
				std::sort(tmpArr.val.begin(), tmpArr.val.end()); // sort in ascending order
				return tmpArrId;
			}
		} else {
			ArrayKeysMap::iterator it = savedArrays.find(keyEl);
			if (it != savedArrays.end()) {
				return it->second;
			}
		}
	}
	return 0; // not found
}

void _stdcall SaveArray(const ScriptValue& key, DWORD id) {
	array_itr it = arrays.find(id), it2;
	if (it != arrays.end()) {
		if (!key.isInt() || key.asInt() != 0) {
			// make array permanent
			FixArray(it->first);
			// if another array is saved under the same key, clear it
			ArrayKeysMap::iterator sIt = savedArrays.find(sArrayElement(key.rawValue(), key.type()));
			if (sIt != savedArrays.end() && sIt->second != id && (it2 = arrays.find(sIt->second)) != arrays.end()) {
				it2->second.key.unset();
			}
			it->second.key.setByType(key.rawValue(), key.type());
			savedArrays[it->second.key] = id;
		} else { // int(0) is used to "unsave" array without destroying it
			int num = savedArrays.erase(it->second.key);
			it->second.key.unset();
		}
	}
}

/*
	Sets element to array created in last CreateArray call (used for array expressions)
	For normal arrays, this method should be called in strict linear index order:
	(create_array(0, 0) + arrayexpr(0, 20) + arrayexpr(1, 25) + arrayexpr(2, 777) + ...)

	For assoc arrays order doesn't matter:
	(create_array(0, 1) + arrayexpr("key1", "val1") + arrayexpr("key2", 25) + arrayexpr(3, 6.1242) + ...)

	Should always return 0!
*/
DWORD _stdcall StackArray(const ScriptValue& key, const ScriptValue& val) {
	DWORD id = stackArrayId;
	if (id == 0 || arrays.find(id) == arrays.end()) {
		return 0;
	}
	if (!arrays[id].isAssoc()) {
		// automatically resize array to fit one new element
		ResizeArray(id, arrays[id].size() + 1);
	}
	SetArray(id, key, val, 0);
	return 0;
}

}
}
