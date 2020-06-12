#include "request.h"
#include "stats.h"

Downloader::Downloader(std::vector<unsigned int> & bitrates): B(bitrates){}

float Downloader::get(Request & r, float at_time)
{
    r.m_send_time = at_time;
    R.push_back(r);
    float down_time = (float) r.m_seg.m_size / B.at(m_number++);
    r.m_received_time = r.m_send_time + down_time;
    return down_time;
}

int Downloader::m_number = 0;
