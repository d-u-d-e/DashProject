#ifndef STATS_H
#define STATS_H

#include <vector>
#include <string>

class Segment;

class Stats{

private:
    std::vector<float> P;
    std::vector<float> U;
    static float beta;
    static float gamma;
    std::vector<Segment> & m_responses;
    float m_total_freeze_time = 0.0;
    float m_loss_delay = 0.0;
    float m_loss_quality_change = 0.0;
    float m_gain_quality = 0.0;

    double computeOverallQoE();

public:

    Stats(std::vector<Segment> & responses);

    void updatePhi(float delay_time, int segment_number);
    std::string toString();

};


#endif // STATS_H
