#include "request.h"
#include "stats.h"

extern double buf_size;

Downloader::Downloader(Stats & s, std::vector<unsigned int> & bitrates):
    m_stats(s), B(bitrates){}


/*
    @param at_time: the time at which the request r is made
    @return: the time to download the entire segment specified by the request
*/
double Downloader::get(Request & r, double at_time)
{
    r.m_send_time = at_time;

    unsigned int current_bitrate;

    if(m_number < B.size())
        current_bitrate = B.at(m_number++);
    else
        current_bitrate = B.back(); //should we reach the end of the bitrates file

    double down_time = (double) r.m_seg.m_size / current_bitrate;
    r.m_received_time = r.m_send_time + down_time;

    R.push_back(r);

    //update stats
    //this updates the overall mean
    m_stats.m_overall_avg_bitrate =
            ((m_number - 1) * m_stats.m_overall_avg_bitrate + current_bitrate) / m_number;

    //this updates the recent bitrate, based on at most 4 previous values
    double sum_recent_bitrates = 0;
    unsigned int size = std::min(m_number, 4U);
    for(unsigned int i = 1; i <= size; i++)
        sum_recent_bitrates += B.at(m_number - i);
    m_stats.m_avg_bitrate = sum_recent_bitrates / size;

    m_stats.m_current_bitrate = current_bitrate; //saves last bitrate known

    return down_time;
}

double Downloader::estimateDownTime(unsigned int size)
{
    return size / m_stats.m_avg_bitrate;
}

unsigned int Downloader::m_number = 0;
