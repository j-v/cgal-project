#include "emd.h"
#include <string>

/* 
Load the image provided by imagePath and return the EMD signature in the second argument 
Returns result code: 0=success, 1=failure	
*/
int generate_signature(const std::string & imagePath, signature_t & signature);



void get_centroid(signature_t & s, double & cent_x, double & cent_y);

void normalize_by_centroid(signature_t & s);

float signature_weight_sum(signature_t &s);

void random_point_set(signature_t &s, int num_points, float weight_sum, int width, int height);

void normalize_point_set(signature_t &signature);