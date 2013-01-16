#include <string>
#include <iostream>
#include <sys/types.h>
#include <dirent.h>
#include <vector>
#include "db.h"
#include "pointset.h"
#include "emd.h"

using namespace std;

#define DIR_SEP "/"
#define EMDDB_INDEX "emdindex.csv"

int getDir (string dir, vector<string> &files) {
    DIR *dp;
    struct dirent *dirp;
    if((dp  = opendir(dir.c_str())) == NULL) {
        return -1;
    }
    while ((dirp = readdir(dp)) != NULL) {
        files.push_back(string(dirp->d_name));
    }
    closedir(dp);
    return 0;
}

int main( int argc, char** argv ) {
  
  if (argc == 3) {
	  string firstParam(argv[1]);
	  if (firstParam.compare("-builddb") == 0) {
		string dir(argv[2]);
		vector<string> files = vector<string>();
		if (getDir(dir,files) == 0) {
			EmdDB emddb;
			string auxDir = dir + DIR_SEP;
			if (emddb.create(auxDir + EMDDB_INDEX) == 0) {
				for (unsigned int i = 0;i < files.size();i++) {
					signature_t dbImageSignature;
					generate_signature(dir + DIR_SEP + files[i], dbImageSignature);
					entry_t entry;
					entry.filename = files[i];
					entry.signature = dbImageSignature;
					emddb.addEntry(entry);
				}
				emddb.close();
			}
			else {
				cout << "Unable to create the DB file. Please, check the permissions." << endl;
			}
		}
		else {
			cout << "Unable to read the contents of the directory. Please, check the path." << endl;
		}
	  }
	  else {
		  signature_t queryImageSignature;
		  if (generate_signature(firstParam, queryImageSignature) == 0) {
			  
			  //waitKey(0);
			  EmdDB db;
			  string db_path(argv[2]);
			  db.load(db_path + DIR_SEP + EMDDB_INDEX);

		  }
		  else {
			  cout << "Unable to generate the signature for this image. Please, check the path." << endl;
		  }
	  }
  }
  else {
	  cout << "Incorrect call. The format should be:" << endl;
	  cout << "chsemd -builddb path-to-directory-with-images" << endl;
	  cout << "chsemd path-to-query-image path-to-db-directory" << endl;
  }

  return 0;
}
