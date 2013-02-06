#include "db.h"
#include <stdlib.h>
#include <sstream>

using namespace std;

void
EmdDB::addEntry(const entry_t & entry)
{
	entries.push_back(entry);
	numEntries++;

	if (numEntries > 1)
		(*f_out) << endl;

	(*f_out) << entry.filename << "," << entry.signature.n;
	for (int i = 0; i < entry.signature.n; i++)
	{
		(*f_out) << "," << entry.signature.Features[i].X 
			<< "," << entry.signature.Features[i].Y
			<< "," << entry.signature.Weights[i];
	}

}

entry_t
EmdDB::getEntry(unsigned int i)
{
	return entries[i];
}

int EmdDB::load(std::string path)
{
	string line;
	ifstream f(path.c_str()); // csv file to open

	if (f.is_open())
	{
		// Read CSV
		while (f.good())
		{
			getline(f, line);

			if (line.compare("") == 0)
				break;
			
			// parse csv line, putting values into a vector
			stringstream ss(line);
			string word = "";
			vector<string> values;
			while (getline(ss, word, ','))
			{
				values.push_back(word);
			}
			// read values from the vector and populate the entries vector
			if (values.size() < 2)
			{
				printf("Not enough values in CSV line \n");
				f.close();
				return -1; // Failure
			}
			else
			{
				string filename = values[0];
				int numPoints = atoi(values[1].c_str());
				signature_t s;
				s.Features = new feature_t[numPoints];
				s.Weights =  new float[numPoints];
				s.n = numPoints;
				if (values.size() < 2 + 3*numPoints)
				{
					printf("Not enough values in CSV line \n");
					f.close();
					return -1; // Failure
				}
				else
				{
					// read feature (X,Y) and weight values
					int i;
					for (i=0; i < numPoints; i++)
					{
						int v_index = 2 + (i * 3);
						int x = atoi(values[v_index].c_str());
						int y = atoi(values[v_index+1].c_str());
						float w = atof(values[v_index+2].c_str());
						feature_t f = {x, y};
						s.Features[i] = f;
						s.Weights[i] = w;
					}
				}

				// combine signature with filename to make entry
				entry_t entry;
				entry.signature = s;
				entry.filename = filename;
				entries.push_back(entry); // entry added to vector
			}

		}
		f.close();
		numEntries = entries.size();
	}
	else
	{
		printf("Unable to open file: %s \n", path.c_str());
		return -1; // Failure
	}
	
	this->path = path;
	return 0; // Success
}

int EmdDB::create(std::string path)
{
	// TODO complete implementation
	f_out = new ofstream(path.c_str());
	if (!(f_out->is_open()))
	{
		printf("Unable to create file: %s \n", path.c_str());
		return 1; // FAIL
	}

	this->path = path;
	return 0; // Success
}

void EmdDB::close()
{
	// flush buffers
	if (f_out != NULL)
	{
		f_out->flush();
		f_out->close();
	}
	delete f_out;
	f_out = NULL;

	// clean up memory of signatures
	for (int i=0; i<entries.size(); i++)
	{
		// TODO
		//releaseSignatureMemory();
		
	}
}

string EmdDB::getPath()
{
	return path;
}