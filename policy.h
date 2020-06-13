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
    std::vector<Segment> & m_responses;
    Downloader & m_downloader;

public:
    BasePolicy(std::vector<Segment> & responses, Downloader & d):
        m_responses(responses), m_downloader(d){

    }
    virtual Request getRequest() = 0;
};

class Policy1: public BasePolicy
{
private:
    Stats & m_stats;

public:
    static const unsigned short DEFAULT_PREFETCH = 5;
    const unsigned short k;
    Policy1(Stats & s, std::vector<Segment> & responses, Downloader & d, unsigned short k_param = DEFAULT_PREFETCH):
        BasePolicy(responses, d), m_stats(s), k(k_param){
    }
    Request getRequest() override;
    double preFetch(unsigned short coding_level);
};

#endif // POLICY_H

class Policy2: public BasePolicy{

public:
    Policy2(std::vector<Segment> & responses, Downloader & d): BasePolicy(responses, d){}
    Request getRequest() override;
};
