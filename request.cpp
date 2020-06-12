#include "request.h"
#include "stats.h"

extern float time_played;

Downloader::Downloader(std::vector<unsigned int> & bitrates): B(bitrates)
{

}

float Downloader::get(Request & r)
{
    r.m_requested_at = time_played;
    R.push_back(r);
    float down_time = (float) r.m_seg.m_size / B.at(m_number++);
    r.m_arrived_at = r.m_requested_at + down_time;
    return down_time;
}

int Downloader::m_number = 0;
