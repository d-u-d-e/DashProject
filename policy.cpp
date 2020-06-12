#include "policy.h"
#include "request.h"
#include "stats.h"

using namespace std;

extern float buf_size;
extern float segment_time;
extern unsigned int no_segments;

void Policy::preFetch(int coding_level)
{
    float time = 0;
    for(int i = 1; i <= k; i++){
        Request r(Segment(i, coding_level), Request::new_segment);
        time += m_downloader.get(r);
        m_responses.push_back(r.getSegment());
    }
    m_stats.updatePhi(time, 1);
    buf_size += k * segment_time;
}

Request Policy::getRequest()
{
    if(buf_size < k * segment_time)
        current_down_quality = max(0, current_down_quality - 1);
    else
        current_down_quality = min(4, current_down_quality + 1);
    if(m_responses.size() < vector<Segment>::size_type(no_segments))
        return Request(Segment(m_responses.size() + 1, current_down_quality), Request::new_segment);
    else
       throw exception();
}

unsigned short Policy::k = 10;
