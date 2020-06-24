#ifndef STATS_H
#define STATS_H

#include <vector>
#include <string>

class Segment;
class Downloader;

class Stats{

friend Downloader;

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
    double m_overall_avg_bitrate = 0.0;
    double m_avg_bitrate = 0.0;
    unsigned int m_current_bitrate = 0;
    unsigned int m_replacements = 0;
    double computeOverallQoE();

public:

    Stats(std::vector<Segment> & responses);
    void setDelay(double delay_time, unsigned int segment_number);
    std::string toString();
    void setTimeLeft(double time);
    double getOverallAverageBitrate(){return m_overall_avg_bitrate;}
    double getAvgBitrate(){return m_avg_bitrate;}
    unsigned int getCurrentBitrate(){return m_current_bitrate;}
    void replacement(){m_replacements++;}
};


#endif // STATS_H
