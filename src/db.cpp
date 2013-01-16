#include "db.h"
#include <fstream>
#include <sstream>

using namespace std;

void
EmdDB::addEntry(const entry_t & entry)
{
	entries.push_back(entry);
	numEntries++;
}

entry_t
EmdDB::getEntry(unsigned int i)
{
	return entries[i];
}

int EmdDB::load(std::string path)
{
	// TODO complete implementation
	string line;
	ifstream f(path); // csv file to open

	if (f.is_open())
	{
		// Read CSV
		while (f.good())
		{
			getline(f, line);
			
			// parse csv line, putting values into a vector
			stringstream ss(line);
			string word = "";
			vector<string> values;
			while (getline(ss, word, ','))
			{
				values.push_back(word);
			}
			// read values from the vector and populate the entries vector

		}
		f.close();
		numEntries = entries.size();
	}
	else
	{
		printf("Unable to open file: %s", path);
		return -1; // Failure
	}
	
	return 0; // Success
}

int EmdDB::create(std::string path)
{
	// TODO complete implementation
	return 0; // Success
}

void EmdDB::close()
{
	// TODO complete implementation
}