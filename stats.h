#ifndef STATS_H
#define STATS_H

#include <vector>
#include <string>

class Segment;

class Stats{

private:
    std::vector<double> P;
    std::vector<double> U;
    static constexpr double BETA = 0.5;
    static constexpr double GAMMA = 0.5;
    std::vector<Segment> & m_responses;
    double m_total_freeze_time = 0.0;
    double m_loss_delay = 0.0;
    double m_loss_quality_change = 0.0;
    double m_gain_quality = 0.0;
    double m_time_left = 0.0;
    unsigned int m_freezes = 0;

    double computeOverallQoE();

public:

    Stats(std::vector<Segment> & responses);

    void setDelay(double delay_time, unsigned int segment_number);
    std::string toString();
    void setTimeLeft(double time);

};


#endif // STATS_H
