#ifndef REQUEST_H
#define REQUEST_H

#include "segment.h"
#include <vector>

class Downloader;
class Request{

friend Downloader;
public:
    enum RequestType{new_segment, better_segment};

    Request(Segment s, RequestType t):
         m_type(t), m_seg(s){}
    Segment & getSegment(){return m_seg;}
    RequestType getType(){return m_type;}
    double getSendTime(){return m_send_time;}

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
    std::vector<unsigned int> & B;
    std::vector<Request> R;
public:
    Downloader(std::vector<unsigned int> & bitrates);
    double get(Request & r, double at_time);
    std::vector<Request> & getRequests(){return R;}
};

#endif // REQUEST_H
