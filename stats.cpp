#include "stats.h"
#include "segment.h"

#include <iostream>
#include <math.h>


float Stats::beta = 0.5;
float Stats::gamma = 0.5;

extern unsigned int no_segments;
extern float segment_time;

Stats::Stats(std::vector<Segment> & responses): m_responses(responses)
{
    P = std::vector<float>(no_segments, 0.0);
    U = std::vector<float>(no_segments, 0.0);
}

void Stats::updatePhi(float delay_time, int segment_number)
{
    P.at(segment_number - 1) = delay_time;
    m_total_freeze_time += delay_time;
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
    s += "initial delay: " + std::to_string(P.at(0)) + " (sec)\n";
    s += "loss delay: " + std::to_string(m_loss_delay) + "\n";
    s += "loss quality change: " + std::to_string(m_loss_quality_change) + "\n";
    s += "quality gain: " + std::to_string(m_gain_quality) + "\n";
    return s;
}
