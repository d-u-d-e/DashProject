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
    virtual Request getRequest() = 0;
    double preFetch(unsigned short coding_level, unsigned int number = DEFAULT_PREFETCH);

    class policy_done: public std::exception{

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
    const unsigned short k;
    Policy2(Stats & s, std::vector<Segment> & responses, Downloader & d, MPD & mpd, unsigned short k_param):
        BasePolicy(s, responses, d, mpd), k(k_param){}
    Request getRequest() override;
};

#endif // POLICY_H


class Policy3: public BasePolicy{

private:
    unsigned short dynamic_k;
    unsigned short init_k;
public:
    Policy3(Stats & s, std::vector<Segment> & responses, Downloader & d, MPD & mpd, unsigned short k_param):
        BasePolicy(s, responses, d, mpd), init_k(k_param){dynamic_k = init_k;}

    Request getRequest() override;
};
