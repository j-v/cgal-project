#include <string>
#include <iostream>

#include <sys/types.h>
#include <dirent.h>
#include <vector>

#include "pointset.h"
#include "emd.h"

using namespace std;

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
  //EmdDB emddb;
  //if (1 == emddb.create("path"))...
  
  if (argc == 3) {
	  string firstParam(argv[1]);
	  if (firstParam.compare("-builddb") == 0) {
		string dir = argv[2];
		vector<string> files = vector<string>();
		if (getDir(dir,files) == 0) {
			for (unsigned int i = 0;i < files.size();i++) {
				cout << files[i] << endl;
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
