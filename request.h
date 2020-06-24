#ifndef REQUEST_H
#define REQUEST_H

#include "segment.h"
#include <vector>

class Stats;

class Request{

friend class Downloader;

public:
    //two types of request: for a new segment never ever downloaded, or for a better segment (higher quality)
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
    static unsigned int m_number; //in order to advance to the next row of bitrates
    Stats & m_stats;
    std::vector<unsigned int> & B; //this holds the bitrates, as in channels.txt
    std::vector<Request> R; //this holds the requests
public:
    Downloader(Stats & s, std::vector<unsigned int> & bitrates);
    double get(Request & r, double at_time);
    std::vector<Request> & getRequests(){return R;}
    double estimateDownTime(unsigned int size);
};


#endif // REQUEST_H
