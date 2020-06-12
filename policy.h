#ifndef POLICY_H
#define POLICY_H

#include <vector>

class Segment;
class Stats;
class Request;
class Downloader;


class BasePolicy{
protected:
    int m_current_down_quality = 0;
    Stats & m_stats;
    std::vector<Segment> & m_responses;
    Downloader & m_downloader;

public:
    BasePolicy(Stats & s, std::vector<Segment> & responses, Downloader & d):
        m_stats(s), m_responses(responses), m_downloader(d){

    }
    virtual Request getRequest() = 0;
};

class Policy: public BasePolicy
{
public:
    static const unsigned short DEFAULT_PREFETCH = 5;
    const unsigned short k;
    Policy(Stats & s, std::vector<Segment> & responses, Downloader & d, unsigned int k_param = DEFAULT_PREFETCH):
        BasePolicy(s, responses, d), k(k_param){
    }
    Request getRequest() override;
    float preFetch(int coding_level);
};

#endif // POLICY_H
