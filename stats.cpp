#include "stats.h"
#include "segment.h"

#include <iostream>
#include <math.h>

extern unsigned int no_segments;
extern double segment_time;

using namespace std;

Stats::Stats(vector<Segment> & responses): m_responses(responses)
{
    P = vector<double>(no_segments, 0.0);
    U = vector<double>(no_segments, 0.0);
}

void Stats::setDelay(double delay_time, unsigned int segment_number)
{
    P.at(segment_number - 1) = delay_time;
    m_total_freeze_time += delay_time;
    m_freezes++;
}

double Stats::computeOverallQoE()
{
    double loss_quality_change = 0.0;
    double loss_delay = exp(GAMMA * P.at(0) / segment_time);
    double gain_quality = m_responses.at(0).m_quality;
    U.at(0) = gain_quality - loss_delay;
    m_gain_quality += gain_quality;
    m_loss_delay += loss_delay;

    for(unsigned int i = 1; i < no_segments; i++){
        loss_quality_change =  BETA * fabs(m_responses.at(i).m_quality - m_responses.at(i-1).m_quality);
        loss_delay = exp(GAMMA * P.at(i) / segment_time);
        gain_quality = m_responses.at(i).m_quality;
        U.at(i) =  gain_quality - loss_quality_change - loss_delay;
        m_loss_delay += loss_delay;
        m_loss_quality_change += loss_quality_change;
        m_gain_quality += gain_quality;
    }
    double qoe = 0;
    for (double v: U)
        qoe += v;
    return qoe;
}

string Stats::toString()
{
    string s = "overall QoE: " + to_string(computeOverallQoE()) + "\n";
    s += "total freeze time: " + to_string(m_total_freeze_time) + " (sec)\n";
    s += "number of freezes: " + to_string(m_freezes) + " (might include freeze due to prefetching) \n";
    s += "initial delay: " + to_string(P.at(0)) + " (sec)\n";
    s += "loss delay: (-) " + to_string(m_loss_delay) + "\n";
    s += "loss quality change: (-) " + to_string(m_loss_quality_change) + "\n";
    s += "quality gain: (+) " + to_string(m_gain_quality) + "\n";
    s += "time left to play after download: " + to_string(m_time_left) + " (sec)\n";

    unsigned int sum = 0;
    for(Segment & seg: m_responses)
        sum += seg.m_coding_level;

    s += "avarage coding level: " +to_string((double) sum / m_responses.size()) + "\n\n";

    return s;
}

void Stats::setTimeLeft(double time)
{
    m_time_left = time;
}
