#include <string>
#include "pointset.h"
#include "emd.h"
using namespace std;

int main( int argc, char** argv ) {

  if (argv[1].compare("-builddb") == 0) {
	if (argc != 3) {
		printf("Call me like \"chsemd -builddb path-to-directory-with-images\"\n");
	}
	else {
		//argv[2] == PATH TO DIRECTORY
	}
  }
  else if (argc == 3) {
	  signature_t queryImageSignature;
	  if (generate_signature(argv[1], queryImageSignature) == 0) {
		  
		  //waitKey(0);
	  }
	  else {
		  printf("Unable to generate the signature for this image. Check the path.\n");
	  }
  }
  else {
	  printf("Incorrect call. The format should be:\n");
	  printf("chsemd -builddb path-to-directory-with-images\n");
	  printf("chsemd path-to-query-image path-to-db-directory\n");
  }

  return 0;
}
