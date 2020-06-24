#include "request.h"
#include "stats.h"

extern double buf_size;

Downloader::Downloader(Stats & s, std::vector<unsigned int> & bitrates):
    m_stats(s), B(bitrates){}

double Downloader::get(Request & r, double at_time)
{
    r.m_send_time = at_time;

    unsigned int current_bitrate;

    if(m_number < B.size())
        current_bitrate = B.at(m_number++);
    else
        current_bitrate = B.back(); //should we reach end of bitrate file

    double down_time = (double) r.m_seg.m_size / current_bitrate;
    r.m_received_time = r.m_send_time + down_time;

    R.push_back(r);

    //update stats
    m_stats.m_overall_avg_bitrate =
            ((m_number - 1) * m_stats.m_overall_avg_bitrate + current_bitrate) / m_number;

    double sum_recent_bitrates = 0;
    unsigned int size = std::min(m_number, 4U);
    for(unsigned int i = 1; i <= size; i++)
        sum_recent_bitrates += B.at(m_number - i);
    m_stats.m_avg_bitrate = sum_recent_bitrates / size;

    m_stats.m_previous_bitrate = m_stats.m_current_bitrate;
    m_stats.m_current_bitrate = current_bitrate;

    return down_time;
}

double Downloader::estimateDownTime(unsigned int size)
{
    return size / m_stats.m_avg_bitrate;
}

unsigned int Downloader::m_number = 0;
