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

private:
    RequestType m_type;
    Segment m_seg;
    float m_arrived_at = 0;
    float m_requested_at = 0;
};

class Downloader{
private:
    static int number;
    std::vector<unsigned int> & B;
    std::vector<Request> R;
public:
    Downloader(std::vector<unsigned int> & bitrates);
    float get(Request & r);

};

#endif // REQUEST_H
