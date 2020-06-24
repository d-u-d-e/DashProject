#ifndef POLICY_H
#define POLICY_H

#include <vector>

class Segment;
class Stats;
class Request;
class Downloader;
class MPD;

class BasePolicy{
protected:
    //default number of segments to prefetch
    static const unsigned short DEFAULT_PREFETCH = 5;
    unsigned short m_current_coding_level = 4;
    Stats & m_stats;
    std::vector<Segment> & m_responses;
    Downloader & m_downloader;
    MPD & m_mpd;

public:
    BasePolicy(Stats & s, std::vector<Segment> & responses, Downloader & d, MPD & mpd):
        m_stats(s), m_responses(responses), m_downloader(d), m_mpd(mpd){

    }
    virtual Request getRequest() = 0; //virtual so that each policy overrides it
    double preFetch(unsigned short coding_level, unsigned int number = DEFAULT_PREFETCH);
    class policy_done: public std::exception{ //use to signal that getRequest() can't provide a new request

    };
};

class Policy1: public BasePolicy{

public:
    Policy1(Stats & s, std::vector<Segment> & responses, Downloader & d, MPD & mpd): BasePolicy(s, responses, d, mpd){}
    Request getRequest() override;
};

class Policy2: public BasePolicy
{
public:
    //the same k as in the policy suggested by the professor
    const unsigned short k;
    Policy2(Stats & s, std::vector<Segment> & responses, Downloader & d, MPD & mpd, unsigned short k_param):
        BasePolicy(s, responses, d, mpd), k(k_param){}
    Request getRequest() override;
};

#endif // POLICY_H


class Policy3: public BasePolicy{ //this is my policy

private:
    //the same k as in the policy suggested by the professor, but this is not fixed, hence the name
    unsigned short dynamic_k;
    //the initial value for k, chosen by the instantiator of this policy
    unsigned short init_k;
public:
    Policy3(Stats & s, std::vector<Segment> & responses, Downloader & d, MPD & mpd, unsigned short k_param):
        BasePolicy(s, responses, d, mpd), init_k(k_param){dynamic_k = init_k;}
    Request getRequest() override;
};
