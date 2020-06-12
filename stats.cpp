#include "stats.h"
#include "segment.h"

#include <iostream>
#include <math.h>

extern unsigned int no_segments;
extern float segment_time;

Stats::Stats(std::vector<Segment> & responses): m_responses(responses)
{
    P = std::vector<float>(no_segments, 0.0);
    U = std::vector<float>(no_segments, 0.0);
}

void Stats::setDelay(float delay_time, int segment_number)
{
    P.at(segment_number - 1) = delay_time;
    m_total_freeze_time += delay_time;
    m_freezes++;
}

double Stats::computeOverallQoE()
{
    float loss_quality_change = 0.0;
    float loss_delay = exp(gamma * P.at(0) / segment_time);
    float gain_quality = m_responses.at(0).m_quality;
    U.at(0) = gain_quality - loss_delay;
    m_gain_quality += gain_quality;
    m_loss_delay += loss_delay;

    for(unsigned int i = 1; i < no_segments; i++){
        loss_quality_change =  beta * fabs(m_responses.at(i).m_quality - m_responses.at(i-1).m_quality);
        loss_delay = exp(gamma * P.at(i) / segment_time);
        gain_quality = m_responses.at(i).m_quality;
        U.at(i) =  gain_quality - loss_quality_change - loss_delay;
        m_loss_delay += loss_delay;
        m_loss_quality_change += loss_quality_change;
        m_gain_quality += gain_quality;
    }
    double qoe = 0;
    for (float v: U)
        qoe += v;
    return qoe;
}

std::string Stats::toString()
{
    std::string s = "overall QoE: " + std::to_string(computeOverallQoE()) + "\n";
    s += "total freeze time: " + std::to_string(m_total_freeze_time) + " (sec)\n";
    s += "number of freezes: " + std::to_string(m_freezes) + " (might include freeze due to prefetching) \n";
    s += "initial delay: " + std::to_string(P.at(0)) + " (sec)\n";
    s += "loss delay: (-) " + std::to_string(m_loss_delay) + "\n";
    s += "loss quality change: (-) " + std::to_string(m_loss_quality_change) + "\n";
    s += "quality gain: (+) " + std::to_string(m_gain_quality) + "\n";
    s += "time left to play after download: " + std::to_string(m_time_left) + " (sec)\n";

    int sum = 0;
    for(Segment & seg: m_responses)
        sum += seg.m_coding_level;

    s += "avarage coding level: " + std::to_string(sum / m_responses.size()) + "\n\n";

    return s;
}

void Stats::setTimeLeft(float time)
{
    m_time_left = time;
}
