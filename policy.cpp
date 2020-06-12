#include "policy.h"
#include "request.h"
#include "stats.h"

using namespace std;

extern float buf_size;
extern float segment_time;
extern unsigned int no_segments;

float Policy::preFetch(int coding_level)
{
    float time = 0;
    for(unsigned int i = 1; i <= k; i++){
        Request r(Segment(i, coding_level), Request::new_segment);
        r.m_is_media_buffering = true;
        time += m_downloader.get(r, time);
        m_responses.push_back(r.getSegment());
    }
    m_stats.setDelay(time, 1);
    buf_size += k * segment_time;
    return time;
}

Request Policy::getRequest()
{
    if(buf_size < k * segment_time)
        m_current_down_quality = max(0, m_current_down_quality - 1);
    else
        m_current_down_quality = min(4, m_current_down_quality + 1);
    if(m_responses.size() < vector<Segment>::size_type(no_segments)){
        Request r(Segment(m_responses.size() + 1, m_current_down_quality), Request::new_segment);
        r.m_is_media_buffering = (buf_size == 0);
        return r;
    }
    else{
       throw exception();
    }
}

