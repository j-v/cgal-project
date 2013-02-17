#ifndef FUNCTIONS_H
#define FUNCTIONS_H 1

#include <string>
#include <vector>
#include <math.h>
#include <dirent.h>

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

float euclid_dist2(feature_t *f1, feature_t *f2)
{
	float dx = f2->X - f1->X;
	float dy = f2->Y - f1->Y;
	return sqrt(dx*dx + dy*dy);
}

#endif
