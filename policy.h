#ifndef POLICY_H
#define POLICY_H

#include <vector>

class Segment;
class Stats;
class Request;
class Downloader;


class BasePolicy{
protected:
    static const unsigned short DEFAULT_PREFETCH = 5;
    int m_current_down_quality = 0;
    Stats & m_stats;
    std::vector<Segment> & m_responses;
    Downloader & m_downloader;

public:
    BasePolicy(Stats & s, std::vector<Segment> & responses, Downloader & d):
        m_stats(s), m_responses(responses), m_downloader(d){

    }
    virtual Request getRequest() = 0;
    double preFetch(unsigned short coding_level, unsigned int number = DEFAULT_PREFETCH);
};

class Policy1: public BasePolicy
{
public:
    const unsigned short k;
    Policy1(Stats & s, std::vector<Segment> & responses, Downloader & d, unsigned short k_param):
        BasePolicy(s, responses, d), k(k_param){
        preFetch(0, k);
    }
    Request getRequest() override;
};

#endif // POLICY_H

class Policy2: public BasePolicy{

public:
    Policy2(Stats & s, std::vector<Segment> & responses, Downloader & d): BasePolicy(s, responses, d){}
    Request getRequest() override;
};

class Policy3: public BasePolicy{
public:
    const unsigned short k;
    Policy3(Stats & s, std::vector<Segment> & responses, Downloader & d, unsigned short k_param):
        BasePolicy(s, responses, d), k(k_param){
        preFetch(0, k);
    }
    Request getRequest() override;
};
