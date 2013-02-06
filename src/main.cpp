#include <string>
#include <iostream>
#include <sys/types.h>
#include <vector>
#include <utility>
#include <algorithm>

#include <highgui.h>

#include "db.h"
#include "pointset.h"
#include "emd.h"
#include "functions.h"

using namespace std;

#define DIR_SEP "/"
#define EMDDB_INDEX "emdindex.csv"

typedef pair<entry_t, float> score_pair;

struct score_sort_pred {
	bool operator()(const score_pair &left, const score_pair &right) {
        return left.second < right.second;
    }
};

int buildDatabase(string dir)
{
	vector<string> files = vector<string>();
	if (getDir(dir,files) == 0) {
		EmdDB emddb;
		if (emddb.create(dir + DIR_SEP + EMDDB_INDEX) == 0) {
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
			return 0;
		}
		else {
			cout << "Unable to create the DB file. Please, check the permissions." << endl;
			return 1;
		}
	}
	else {
		cout << "Unable to read the contents of the directory. Please, check the path." << endl;
		return 1;
	}
}

int queryDatabase(EmdDB db, string queryImage)
{
	signature_t queryImageSignature;
	cout << "Generating image signature...'" << endl;
	if (generate_signature(queryImage, queryImageSignature) == 0) {			  

		

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

		// get path of dir to images
		string db_path = db.getPath(); // gets path of index file
		int dir_sep_index = db_path.find_last_of(DIR_SEP);
		string db_dir = db_path.substr(0, dir_sep_index);

		// Output N top scores
		int N = 5;
		for (int i=0; i<N; i++) {
			cout << i << ". " << scores[i].first.filename << " " << scores[i].second << endl;
			//show image
			string filename =  scores[i].first.filename;
			
			cv::Mat im = cv::imread(db_dir + DIR_SEP + filename);
			cv::namedWindow(filename);
			cv::imshow(filename, im);
		}
		cv::waitKey(0);

		return 0;
	}
	else {
		cout << "Unable to generate the signature for this image. Please, check the path." << endl;
		return 1;
	}

}

int main( int argc, char** argv ) {
  
  if (argc == 3) {
	  string firstParam(argv[1]);
	  if (firstParam.compare("-builddb") == 0) { // BUILD IMAGE DATABASE
		string dir(argv[2]);
		return buildDatabase(dir);
	  }
	  else {	// QUERY IMAGE DATABASE
		// Load database
		EmdDB db;
		string db_path(argv[2]);
		db.load(db_path + DIR_SEP + EMDDB_INDEX);
		// perform query
		queryDatabase(db, firstParam);
	  }
  }
  else {
	  cout << "Incorrect call. The format should be:" << endl;
	  cout << "chsemd -builddb path-to-directory-with-images" << endl;
	  cout << "chsemd path-to-query-image path-to-db-directory" << endl;
  }

  return 0;
}
