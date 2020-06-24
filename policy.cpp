#include "policy.h"
#include "request.h"
#include "stats.h"
#include "mpd.h"

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
        Request r(m_mpd.getSegment(i, coding_level), Request::new_segment);
        r.m_is_media_buffering = true;
        time += m_downloader.get(r, time);
        m_responses.push_back(r.getSegment());
    }
    m_stats.setDelay(time, 1); //delay before playout of segment 1 can start
    buf_size += number * segment_time;
    m_current_coding_level = coding_level;
    return time;
}

//*************** POLICY 1 ***************

//this simple policy requests segments with a fixed coding level, in this case 3 (range is 0-4)

Request Policy1::getRequest()
{
    unsigned int next_segment = m_responses.size() + 1;

    if(next_segment <= no_segments){ //more segments to download
        Request r(m_mpd.getSegment(next_segment, 3), Request::new_segment);
        r.m_is_media_buffering = (buf_size == 0);
        return r;
    }
    else //all done
       throw policy_done();
}

//*************** END POLICY 1 ***************

//*************** POLICY 2 ***************

//policy suggested by professor in his paper

Request Policy2::getRequest()
{
    if(buf_size < k * segment_time)
        m_current_coding_level = min(4, m_current_coding_level + 1);
    else
        m_current_coding_level = max(0, m_current_coding_level - 1);

    unsigned int next_segment = m_responses.size() + 1;
    if(next_segment <= no_segments){ //more segments to download
        Request r(m_mpd.getSegment(next_segment, m_current_coding_level), Request::new_segment);
        r.m_is_media_buffering = (buf_size == 0);
        return r;
    }
    else //all done
       throw policy_done();
}

//*************** END POLICY 2 ***************

//*************** POLICY 3 ***************

/*
    This policy is made up of two parts:
    1) if there are more segments to download:
        i)it checks if an old segment (already downloaded) can be replaced by a better segment,
        that is, one having a lower coding level. If not, then:
        ii) it looks for the next segment to download with quality depending on the current buffer size
        in a dynamic sense, because k is not fixed.
    2) otherwise it starts from the end of the queue of segments and begins to replace each one with the best segment available.
*/

Request Policy3::getRequest()
{
    unsigned int next_seg = m_responses.size() + 1;

    if(next_seg <= no_segments){ //more segments to download

        unsigned int current_seg = time_played / segment_time + 1; //current being played
        unsigned int candidate = current_seg + 1; //next to be played

        bool replace_existing = false; //a flag set to true if replace can be done in time
        unsigned short cod; //for the current coding level

        while(candidate <= m_responses.size()){ //only if it has been received
            unsigned int current_cod = m_responses.at(candidate - 1).m_coding_level; //we get its actual coding level
            //now without changing to much the current coding level (we allow at most a difference of 2 levels),
            //we try to find the first better coding level for which the download of the segment can be completed in time
            for(cod = max(0, m_current_coding_level - 2); cod < current_cod; cod++){
                unsigned int size = m_mpd.getSegment(candidate, cod).m_size;
                double down_est = m_downloader.estimateDownTime(size); //estimate down time, this is based on recent avarage bitrate
                //to be safe, we go ahead 2 segments. Note that not only we want the new segment to be played in time, but
                //at the same time we don't want our buffer to get empty, because download time might be not negligible.
                //I think it's ok to loose 2 segments of time for a better quality segment (one at the cost of two poor segments)
                if((time_played + down_est) / segment_time + 3 <= candidate && buf_size - down_est >= (dynamic_k - 2) * segment_time){
                    replace_existing = true;
                    break;
                }
            }
            if(replace_existing) break;
            candidate++; //try next segment, either this one can't be upgraded in time or it is already at highest quality
        }

        if(replace_existing){ //can replace
            m_stats.replacement(); //we add useful info to be displayed later, such as the number of replacements
            Request r(m_mpd.getSegment(candidate, cod), Request::better_segment);
            r.m_is_media_buffering = (buf_size == 0);
            return r;
        }
        else{ //cannot replace, thus we get a new segment, never ever downloaded
            double avg_bit = m_stats.getAvgBitrate(); //an average of previous bitrates (the most recent)
            unsigned int curr_bit = m_stats.getCurrentBitrate(); //this is actually the previous known bitrate
            double ratio = curr_bit / avg_bit; //is the current bitrate growing fast?
            //A better measure would be estimating this growing by comparing two averages, i.e. the average of the last
            //three bitrates with the average of the previous three again

            if(ratio >= 1.4) dynamic_k--; //these are magic numbers, for the current problem they seem reasonable
            else if(ratio <= 0.5) dynamic_k = init_k; //reset k to its starting value

            if(buf_size < dynamic_k * segment_time) //same as policy2 from now on
                m_current_coding_level = min(4, m_current_coding_level + 1);
            else
                m_current_coding_level = max(0, m_current_coding_level - 1);

            Request r(m_mpd.getSegment(next_seg, m_current_coding_level), Request::new_segment);
            r.m_is_media_buffering = (buf_size == 0);
            return r;
        }
    }
    else{ //all segments downloaded

        //start from the end and replace (if we have time) each segment with the best quality

        next_seg = m_responses.size();
        unsigned int current_seg = time_played / segment_time + 1;

        while(next_seg > current_seg && m_responses.at(next_seg - 1).m_coding_level == 0)
            next_seg--; //these segments are already at highest quality

        unsigned int size = m_responses.at(next_seg - 1).m_size;
        //rough estimation of next download time
        double down_est = m_downloader.estimateDownTime(size);

        //just in case, we stay safe and go ahead 2 segment, but we don't have to worry if it takes much time,
        //main.cpp checks if this segment is valid before playing it
        if(next_seg >= (time_played + down_est) / segment_time + 3){
            Request r(m_mpd.getSegment(next_seg, 0), Request::better_segment);
            r.m_is_media_buffering = (buf_size == 0);
            return r;
        }
        else{
            //all done, no need to download new segments,
            //either we don't have much time left to play (or download requires too much time)
            //or all segments are already at highest quality
            throw policy_done();
        }
    }
}

//*************** END POLICY 3 ***************

