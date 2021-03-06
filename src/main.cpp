#include <string>
#include <iostream>
#include <fstream>
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
#define DEFAULT_TOP_N 10

typedef pair<entry_t, float> score_pair;

static ostream * log_stream;

struct score_sort_pred {
	bool operator()(const score_pair &left, const score_pair &right) {
        return left.second < right.second;
    }
};

char* getCmdOption(char ** begin, char ** end, const std::string & option)
{
    char ** itr = std::find(begin, end, option);
    if (itr != end && ++itr != end)
    {
        return *itr;
    }
    return 0;
}

bool cmdOptionExists(char** begin, char** end, const std::string& option)
{
    return std::find(begin, end, option) != end;
}

int buildDatabase(string dir, bool normalize_weights)
{
	vector<string> files = vector<string>();
	if (getDir(dir,files) == 0) {
		EmdDB emddb;
		if (emddb.create(dir + DIR_SEP + EMDDB_INDEX) == 0) {
			for (unsigned int i = 0;i < files.size();i++) {
				signature_t dbImageSignature;
				
				if (0 == generate_signature(dir + DIR_SEP + files[i], dbImageSignature))
				{
					if (normalize_weights)
						normalize_point_set(dbImageSignature);
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

int queryDatabase(EmdDB db, string queryImage, int top_n, bool normalize_weights)
{
	
	signature_t queryImageSignature;
	cout << "Generating image signature...'" << endl;
	if (generate_signature(queryImage, queryImageSignature) == 0) {		
		// TODO normalize queryImageSignature by centroid
		normalize_by_centroid(queryImageSignature);
		if (normalize_weights)
			normalize_point_set(queryImageSignature);

		// Query every image in database
		cout << "Querying database..." << endl;
		vector<score_pair> scores;
		double emd_time, total_time;
		for (int i=0; i<db.numEntries; i++) {
			int64 total_start = cvGetTickCount();
			entry_t e = db.getEntry(i);		
			normalize_by_centroid(e.signature);
			int64 emd_start = cvGetTickCount();
			float emd_dist = emd(&queryImageSignature, &e.signature, euclid_dist2, NULL, NULL);
			int64 emd_end = cvGetTickCount();
			emd_time = ((double)emd_end - (double)emd_start) / cvGetTickFrequency();
			score_pair p = make_pair(e, emd_dist);
			scores.push_back(p);
			int64 total_end = cvGetTickCount();
			total_time = ((double)total_end - (double)total_start) / cvGetTickFrequency();

			if (log_stream)
				*log_stream << db.getPath() << "," << queryImage << "," << e.filename << "," << queryImageSignature.n << "," 
					<< e.signature.n << "," << emd_time << "," << total_time << endl;
		}

		// Sort scores
		sort(scores.begin(), scores.end(), score_sort_pred());

		// get path of dir to images
		string db_path = db.getPath(); // gets path of index file
		int dir_sep_index = db_path.find_last_of(DIR_SEP);
		string db_dir = db_path.substr(0, dir_sep_index);

		// Output N top scores
		for (int i=0; i<top_n; i++) {
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

  if (argc >= 3) {
	  char * logfile = getCmdOption(argv, argv+argc, "-l");
	  log_stream = NULL;
	  if (logfile != NULL)
	  {
		  log_stream = new ofstream(logfile);
	  }
	  else
	  {
		  //log_stream = &cout;
		  log_stream = NULL;
	  }

	  char * top_n_str = getCmdOption(argv, argv+argc, "-n");
	  int top_n;
	  if (top_n_str != NULL)
		  top_n = atoi(top_n_str);
	  else
		  top_n = DEFAULT_TOP_N;
	  bool normalize_weights = cmdOptionExists(argv, argv+argc, "-N");

	  string firstParam(argv[1]);
	  if (firstParam.compare("-builddb") == 0) { // BUILD IMAGE DATABASE
		string dir(argv[2]);
		
		return buildDatabase(dir, normalize_weights);
	  }
	  else {	// QUERY IMAGE DATABASE
		// Load database
		EmdDB db;
		string db_path(argv[2]);
		db.load(db_path + DIR_SEP + EMDDB_INDEX);

		// write logfile header
		if (log_stream)
			*log_stream << "db_path,query_image,lib_image,query_points,lib_image_points,emd_time,total_time" << endl;

		// perform query
		queryDatabase(db, firstParam, top_n,normalize_weights);
	  }

	  if (logfile != NULL)
		  ((ofstream*)log_stream)->close();
  }
  else {
	  cout << "Incorrect call. The format should be:" << endl;
	  cout << "chsemd -builddb path-to-directory-with-images [-N]" << endl;
	  cout << "chsemd path-to-query-image path-to-db-directory [-l logfile] [-n number of results] [-N]" << endl;
	  cout << "Flags:" << endl;
	  cout << "N: Normalize sum of weights to 100" << endl;
	  cout << "l : Log performance data in CSV format" << endl;
	  cout << "n : Number of query results to display" << endl;
  }


  return 0;
}
