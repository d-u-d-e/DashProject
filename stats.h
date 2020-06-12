#ifndef STATS_H
#define STATS_H

#include <vector>
#include <string>

class Segment;

class Stats{

private:
    std::vector<float> P;
    std::vector<float> U;
    static constexpr float BETA = 0.5;
    static constexpr float GAMMA = 0.5;
    std::vector<Segment> & m_responses;
    float m_total_freeze_time = 0.0;
    float m_loss_delay = 0.0;
    float m_loss_quality_change = 0.0;
    float m_gain_quality = 0.0;
    float m_time_left = 0.0;
    unsigned int m_freezes = 0;

    double computeOverallQoE();

public:

    Stats(std::vector<Segment> & responses);

    void setDelay(float delay_time, int segment_number);
    std::string toString();
    void setTimeLeft(float time);

};


#endif // STATS_H
