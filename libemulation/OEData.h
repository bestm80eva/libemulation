
/**
 * libemulation
 * OEData
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Data class with reference counting
 */

#ifndef _OEDATA_H
#define _OEDATA_H

#include <vector>

#include "OETypes.h"

using namespace std;

class OEData
{
public:
	OEData();
	OEData(int size);
	OEData(const OEData &t);
	OEData &operator=(const OEData &t);
	~OEData();
	
	void retain();
	void release();
	
	char *getData();
	size_t size();
	char &operator[](const size_t pos);
	
	bool resize(int size);
	
private:
	int retainCount;
	vector<char> data;
	
	void init();
};

#endif