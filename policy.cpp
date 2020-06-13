#include "policy.h"
#include "request.h"
#include "stats.h"

using namespace std;

extern double buf_size;
extern double segment_time;
extern unsigned int no_segments;

double BasePolicy::preFetch(unsigned short coding_level, unsigned int number)
{
    double time = 0;
    for(unsigned int i = 1; i <= number; i++){
        Request r(Segment(i, coding_level), Request::new_segment);
        r.m_is_media_buffering = true;
        time += m_downloader.get(r, time);
        m_responses.push_back(r.getSegment());
    }
    m_stats.setDelay(time, 1);
    buf_size += number * segment_time;
    return time;
}

//*************** POLICY 1 ***************

Request Policy1::getRequest()
{
    if(m_responses.size() < vector<Segment>::size_type(no_segments)){
        Request r(Segment(m_responses.size() + 1, 3), Request::new_segment);
        r.m_is_media_buffering = (buf_size == 0);
        return r;
    }
    else
       throw exception();
}

//*************** END POLICY 1 ***************

//*************** POLICY 2 ***************

Request Policy2::getRequest()
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
    else
       throw exception();
}

//*************** END POLICY 2 ***************

//*************** POLICY 3 ***************

Request Policy3::getRequest()
{

}

//*************** END POLICY 3 ***************

