#include "mpd.h"

MPD::MPD()
{

}

void MPD::addSegmentMetaData(Segment s)
{
    if(s.m_number > metadata.size())
        metadata.push_back(vector({s}));
    else
        metadata.at(s.m_number - 1).push_back(s);
}
