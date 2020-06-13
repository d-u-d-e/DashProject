#include "request.h"
#include "stats.h"

Downloader::Downloader(Stats & s, std::vector<unsigned int> & bitrates):
    m_stats(s), B(bitrates){}

double Downloader::get(Request & r, double at_time)
{
    r.m_send_time = at_time;
    R.push_back(r);
    unsigned int current_bitrate;

    if(m_number < B.size())
        current_bitrate = B.at(m_number++);
    else
        current_bitrate = B.back(); //should we reach end of bitrate file

    m_stats.m_average_bitrate =
            ((m_number - 1) * m_stats.m_average_bitrate + current_bitrate) / m_number;

    double sum_recent_bitrates = 0;
    unsigned int size =std::min((int) B.size(), 5);
    for(unsigned int i = size; i > 0; i--)
        sum_recent_bitrates += B.at(i - 1);

    m_stats.m_current_bitrate = sum_recent_bitrates / size;

    double down_time = (double) r.m_seg.m_size / current_bitrate;
    r.m_received_time = r.m_send_time + down_time;
    return down_time;
}

unsigned int Downloader::m_number = 0;
