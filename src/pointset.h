#include "emd.h"
#include <string>

/* 
Load the image provided by imagePath and return the EMD signature in the second argument 
Returns result code: 0=success, 1=failure	
*/
int generate_signature(const std::string & imagePath, signature_t & signature);



void get_centroid(signature_t & s, double & cent_x, double & cent_y);

void normalize_by_centroid(signature_t & s);