#include "emd.h"
#include <string>

class EmdDB
{
public:
	// dummy constructor, must call load or create after using
	EmdDB() : numEntries(0) { }; 

	void addEntry(const signature_t & entry);

	signature_t getEntry(unsigned int i);

	// Load DB from path. Return 0 on success, other value otherwise
	int load(std::string path);

	// Create new DB in path. Return 0 on success, other value otherwise
	int create(std::string path);
	
	// Close the DB, flushing all written data to disk and performing clean-up
	void close();

	unsigned int numEntries;
};