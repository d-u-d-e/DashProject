#include "policy.h"
#include "request.h"
#include "stats.h"

#include <math.h>
#include <iostream>

using namespace std;

extern double buf_size;
extern double segment_time;
extern unsigned int no_segments;
extern double time_played;
extern double current_time;

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
    m_current_coding_level = coding_level;
    return time;
}

//*************** POLICY 1 ***************

Request Policy1::getRequest()
{
    unsigned int next_segment = m_responses.size() + 1;

    if(next_segment <= no_segments){ //more segments to download
        Request r(Segment(next_segment, 3), Request::new_segment);
        r.m_is_media_buffering = (buf_size == 0);
        return r;
    }
    else //all done
       throw exception();
}

//*************** END POLICY 1 ***************

//*************** POLICY 2 ***************

Request Policy2::getRequest()
{
    if(buf_size < k * segment_time)
        m_current_coding_level = min(4, m_current_coding_level + 1);
    else
        m_current_coding_level = max(0, m_current_coding_level - 1);

    unsigned int next_segment = m_responses.size() + 1;
    if(next_segment <= no_segments){ //more segments to download
        Request r(Segment(next_segment, m_current_coding_level), Request::new_segment);
        r.m_is_media_buffering = (buf_size == 0);
        return r;
    }
    else //all done
       throw exception();
}

//*************** END POLICY 2 ***************

//*************** POLICY 3 ***************


Request Policy3::getRequest()
{
    unsigned int next_seg = m_responses.size() + 1;
    if(next_seg <= no_segments){ //more segments to download

        unsigned int current_seg = time_played / segment_time + 1;

        unsigned int candidate = current_seg + 1;


        while(m_responses.size() >= candidate){
            if(m_responses.at(candidate - 1).m_coding_level > 0){
            unsigned int size = m_responses.at(candidate - 1).m_size;
            double down_est = m_downloader.estimateDownTime(size);
            if((time_played + down_est) / segment_time + 2 <= candidate)
        }

        unsigned int size = m_responses.at(next_seg - 1).m_size;
        //rough estimation of next download time


        //just in case, we stay safe and go ahead 1 segment, but we don't have to worry if it takes much time,
        //main.cpp checks if this segment is valid before playing it
        unsigned int first_to_replace = (time_played + down_est) / segment_time + 2;


        Request r(Segment(next_seg, m_current_coding_level), Request::new_segment);
        r.m_is_media_buffering = (buf_size == 0);
        return r;
    }
    else{ //all segments downloaded

        //start from the end and replace (if we have time) each segment with the best quality

        next_seg = m_responses.size();
        unsigned int current_seg = time_played / segment_time + 1;

        while(next_seg > current_seg && m_responses.at(next_seg - 1).m_coding_level == 0)
            next_seg--;

        unsigned int size = m_responses.at(next_seg - 1).m_size;
        //rough estimation of next download time
        double down_est = m_downloader.estimateDownTime(size);

        //just in case, we stay safe and go ahead 1 segment, but we don't have to worry if it takes much time,
        //main.cpp checks if this segment is valid before playing it
        unsigned int first_to_replace = (time_played + down_est) / segment_time + 2;

        if(next_seg >= first_to_replace){
            Request r(Segment(next_seg, 0), Request::better_segment);
            r.m_is_media_buffering = (buf_size == 0);
            return r;
        }
        else{
            //all done, no need to download new segments,
            //either we don't have much time left to play (or download requires too much time)
            //or all segments are already at highest quality
            throw exception();
        }
    }
}

//*************** END POLICY 3 ***************

