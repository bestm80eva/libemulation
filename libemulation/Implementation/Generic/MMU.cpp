

/**
 * libemulation
 * MMU
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a generic MMU
 */

#include <iostream>
#include <cctype>
#include <algorithm>

#include "MMU.h"

MMU::MMU()
{
    addressDecoder = NULL;
}

bool MMU::setValue(string name, string value)
{
	if (name.substr(0, 3) == "map")
		conf[name.substr(3)] = value;
	else
		return false;
	
	return true;
}

bool MMU::setRef(string name, OEComponent *ref)
{
    if (name == "addressDecoder")
        addressDecoder = ref;
	else if (name.substr(0, 3) == "ref")
		this->ref[name.substr(3)] = ref;
	else
		return false;
	
	return true;
}

bool MMU::init()
{
    if (!addressDecoder)
    {
        logMessage("addressDecoder not connected");
        
        return false;
    }
    
	for (MMUConf::iterator i = conf.begin();
		 i != conf.end();
		 i++)
	{
		if (!ref.count(i->first))
		{
			logMessage("invalid address range '" + i->first + "'");
            
			return false;
		}
        
 		if (!mapRef(ref[i->first], i->second))
			return false;
	}
    
    updateMaps();
    
	return true;
}

bool MMU::postMessage(OEComponent *sender, int message, void *data)
{
	switch(message)
	{
		case MMU_MAP:
            if (!addMemoryMap((MemoryMap *) data))
                return false;
            
            updateMaps();
            
			return true;
            
        case MMU_UNMAP:
            if (!removeMemoryMap((MemoryMap *) data))
                return false;
            
            updateMaps();
            
			return true;
	}
	
	return false;
}

bool MMU::getMemoryMap(MemoryMap& decoderMap,
                       OEComponent *component,
                       string value)
{
	decoderMap.component = component;
	
    value = strtolower(value);
    
	decoderMap.read = (value.find_first_of('r') != string::npos);
	decoderMap.write = (value.find_first_of('w') != string::npos);
    
	if (!decoderMap.read && !decoderMap.write)
		decoderMap.read = decoderMap.write = true;
	
    value = strfilter(value, "0123456789abcdef-x");
	
    vector<string> items = strsplit(value, '-');
    
    if (items.size() == 1)
    {
        if (items[0] == "")
            return false;
        
        decoderMap.startAddress = decoderMap.endAddress = getUInt(items[0]);
    }
    else if (items.size() == 2)
    {
        if ((items[0] == "") || (items[1] == ""))
            return false;
        
        decoderMap.startAddress = getUInt(items[0]);
        decoderMap.endAddress = getUInt(items[1]);
    }
    else
        return false;
    
	return true;
}

bool MMU::mapRef(OEComponent *component, string conf)
{
    vector<string> confItem = strsplit(conf, ',');
    
    for (vector<string>::iterator i = confItem.begin();
         i != confItem.end();
         i++)
    {
		MemoryMap memoryMap;
        
		if (!getMemoryMap(memoryMap, component, *i))
		{
			logMessage("invalid map '" + conf + "'");
            
			return false;
		}
		
        addMemoryMap(&memoryMap);
    };
    
    updateMaps();
    
    return true;
}

bool MMU::addMemoryMap(MemoryMap *value)
{
    for (MMUMaps::iterator i = memoryMaps.begin();
         i != memoryMaps.end();
         i++)
    {
        if ((i->component == value->component) &&
            (i->startAddress == value->startAddress) &&
            (i->endAddress == value->endAddress) &&
            (i->read == value->read) &&
            (i->write == value->write))
            return false;
    }
    
    memoryMaps.push_back(*value);
    
    return true;
}

bool MMU::removeMemoryMap(MemoryMap *value)
{
    for (MMUMaps::iterator i = memoryMaps.begin();
         i != memoryMaps.end();
         i++)
    {
        if ((i->component == value->component) &&
            (i->startAddress == value->startAddress) &&
            (i->endAddress == value->endAddress) &&
            (i->read == value->read) &&
            (i->write == value->write))
        {
            memoryMaps.erase(i);
            
            return true;
        }
    }
    
    return false;
}

void MMU::updateMaps()
{
    if (!addressDecoder)
        return;
    
//    addressDecoder->postMessage(this, ADDRESSDECODER_CLEAR, NULL);
    
    // To-Do: Update
    
    for (MMUMaps::iterator i = memoryMaps.begin();
         i != memoryMaps.end();
         i++)
        addressDecoder->postMessage(this, ADDRESSDECODER_MAP, &(*i));
}
