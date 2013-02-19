#include "emd.h"
#include <string>
#include <vector>

#include <stdio.h>
#include <fstream>

typedef struct
{
	std::string filename;
	signature_t signature;
} entry_t;

class EmdDB
{
public:
	// dummy constructor, must call load or create after using
	EmdDB() : numEntries(0), f_out(NULL), path("") { }; 

	void addEntry(const entry_t & entry);

	entry_t getEntry(unsigned int i);

	// Load DB from path. Return 0 on success, other value otherwise
	// Path is full path to CSV file
	int load(std::string path);

	// Create new DB in path. Return 0 on success, other value otherwise
	// Path is full path to CSV file
	int create(std::string path);
	
	// Close the DB, flushing all written data to disk and performing clean-up
	void close();

	std::string getPath();

	unsigned int numEntries;
	
private:
	std::vector<entry_t> entries;
	std::ofstream * f_out;
	std::string path;

};