#include "emd.h"
#include <string>

class EmdDB
{
public:
	void addEntry(const signature_t & entry);
	signature_t getEntry(unsigned int i);
	void close();

	unsigned int numEntries;

	static EmdDB load(std::string path);
	static EmdDB create(std::string path);
private:
	EmdDB();

};