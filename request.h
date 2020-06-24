#ifndef REQUEST_H
#define REQUEST_H

#include "segment.h"
#include <vector>

class Downloader;
class Stats;
class Request{

friend Downloader;
public:
    enum RequestType{new_segment, better_segment};

    Request(Segment s, RequestType t):
         m_type(t), m_seg(s){}
    Segment & getSegment(){return m_seg;}
    RequestType getType(){return m_type;}
    double getSendTime(){return m_send_time;}
    double getDownTime(){return m_received_time - m_send_time;}

    bool m_is_media_buffering;

private:
    RequestType m_type;
    Segment m_seg;
    double m_received_time = 0;
    double m_send_time = 0;
};

class Downloader{
private:
    static unsigned int m_number;
    Stats & m_stats;
    std::vector<unsigned int> B;
    std::vector<Request> R;
public:
    Downloader(Stats & s, std::vector<unsigned int> & bitrates);
    double get(Request & r, double at_time);
    std::vector<Request> & getRequests(){return R;}
    double estimateDownTime(unsigned int size);
    void setBitrates(std::vector<unsigned int> & bitrates){B = bitrates;}
};

#endif // REQUEST_H
