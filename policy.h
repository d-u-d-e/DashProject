#ifndef POLICY_H
#define POLICY_H

#include <vector>

class Segment;
class Stats;
class Request;
class Downloader;

class Policy
{
private:
    int m_current_down_quality = 0;
    Stats & m_stats;
    std::vector<Segment> & m_responses;
    Downloader & m_downloader;

    void preFetch(int coding_level);

public:
    static const unsigned short k = 5;
    Policy(Stats & s, std::vector<Segment> & responses, Downloader & d):
        m_stats(s), m_responses(responses), m_downloader(d){
        preFetch(m_current_down_quality);
    }

    Request getRequest();

};

#endif // POLICY_H
