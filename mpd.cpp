#include "mpd.h"

MPD::MPD()
{

}

void MPD::addSegmentMetaData(Segment s)
{
    if(s.m_number > metadata.size())
        metadata.push_back(std::vector<Segment>({s}));
    else
        metadata.at(s.m_number - 1).push_back(s);
}
