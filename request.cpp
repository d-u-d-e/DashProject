#include "request.h"
#include "stats.h"

Downloader::Downloader(std::vector<unsigned int> & bitrates): B(bitrates){}

double Downloader::get(Request & r, double at_time)
{
    r.m_send_time = at_time;
    R.push_back(r);
    double down_time = (double) r.m_seg.m_size / B.at(m_number++);
    r.m_received_time = r.m_send_time + down_time;
    return down_time;
}

unsigned int Downloader::m_number = 0;
