#include <string>
#include <iostream>
#include <sys/types.h>
#include <dirent.h>
#include <vector>
#include <utility>
#include <algorithm>

#include <highgui.h>

#include "db.h"
#include "pointset.h"
#include "emd.h"

using namespace std;

#define DIR_SEP "/"
#define EMDDB_INDEX "emdindex.csv"

typedef pair<entry_t, float> score_pair;

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

float euclid_dist2(feature_t *f1, feature_t *f2)
{
	float dx = f2->X - f1->X;
	float dy = f2->Y - f1->Y;
	return sqrt(dx*dx + dy*dy);
}

struct score_sort_pred {
	bool operator()(const score_pair &left, const score_pair &right) {
        return left.second < right.second;
    }
};

int main( int argc, char** argv ) {
  
  if (argc == 3) {
	  string firstParam(argv[1]);
	  if (firstParam.compare("-builddb") == 0) { // BUILD IMAGE DATABASE
		string dir(argv[2]);
		vector<string> files = vector<string>();
		if (getDir(dir,files) == 0) {
			EmdDB emddb;
			string auxDir = dir + DIR_SEP;
			if (emddb.create(auxDir + EMDDB_INDEX) == 0) {
				for (unsigned int i = 0;i < files.size();i++) {
					signature_t dbImageSignature;
					if (0 == generate_signature(dir + DIR_SEP + files[i], dbImageSignature))
					{
						entry_t entry;
						entry.filename = files[i];
						entry.signature = dbImageSignature;
						emddb.addEntry(entry);
						cout << "Added signature for file: " << files[i] << endl;
					}
					else
					{
						cout << "Skipped adding signature for file: " << files[i] << endl;
					}
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
		  // QUERY IMAGE DATABASE
		  signature_t queryImageSignature;
		  cout << "Generating image signature...'" << endl;
		  if (generate_signature(firstParam, queryImageSignature) == 0) {
			  
			  
			  // Load database
			  EmdDB db;
			  string db_path(argv[2]);
			  db.load(db_path + DIR_SEP + EMDDB_INDEX);

			  // Query every image in database
			  cout << "Querying database..." << endl;
			  vector<score_pair> scores;
			  for (int i=0; i<db.numEntries; i++) {
				entry_t e = db.getEntry(i);				
				float emd_dist = emd(&queryImageSignature, &e.signature, euclid_dist2, NULL, NULL);
				score_pair p = make_pair(e, emd_dist);
				scores.push_back(p);
			  }

			  // Sort scores
			  sort(scores.begin(), scores.end(), score_sort_pred());

			  // Output N top scores
			  int N = 5;
			  for (int i=0; i<N; i++) {
				  cout << i << ". " << scores[i].first.filename << " " << scores[i].second << endl;
				  //show image
				  string filename =  scores[i].first.filename;
				  cv::Mat im = cv::imread(db_path + DIR_SEP + filename);
				  cv::namedWindow(filename);
				  cv::imshow(filename, im);
			  }
			  cv::waitKey(0);
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
