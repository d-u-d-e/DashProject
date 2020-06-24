#ifndef MPD_H
#define MPD_H

#include <vector>
#include  "segment.h"

using namespace std;

class MPD
{
    vector<vector<Segment>> metadata;
public:
    MPD();
    unsigned int size(){return metadata.size();}
    Segment & getSegment(unsigned int number, unsigned short coding_level){
        return metadata.at(number).at(coding_level);
    }
    void addSegmentMetaData(Segment s);
};

#endif // MPD_H
