#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>
#include <iomanip>

#include "stats.h"
#include "policy.h"
#include "request.h"
#include "mpd.h"

using namespace std;

double buf_size = 0;
double segment_time = 0.5;
double time_played = 0;
unsigned int no_segments;
double current_time = 0.0;

bool readFiles(Downloader & d, MPD & m){
    ifstream mpd("MPD.txt");
    ifstream channel("channel.txt");

    if(!mpd.is_open() || !channel.is_open())
        return false;

   int seg, size;
   double quality;
   unsigned short coding_lev = 0;
   unsigned int prev_seg = 1;

   while(mpd >> seg >> size >> quality){
       if(seg != prev_seg) coding_lev = 0;
        m.addSegmentMetaData(Segment(seg, coding_lev++, quality, size));
        prev_seg = seg;
   }

   int bitrate;
   while(channel >> bitrate)
       B.push_back(abs(bitrate));

   d.setBitrates(B);
   no_segments = m.size();
   return true;
}

int main()
{
    vector<Segment> responses;
    Stats s(responses);
    Downloader downloader(s, B);
    MPD mpd;

    if(!readFiles(downloader, mpd)){
        cout << "cannot open files, exit..." << endl;
        exit(1);
    }

    Policy3 p(s, responses, downloader, 40);

    double media_time = no_segments * segment_time;
    current_time = p.preFetch(0, 5);

    while(time_played < media_time){

        try {
             Request r = p.getRequest();
             double down_time = downloader.get(r, current_time);

             Segment received = r.getSegment();

             if(buf_size < down_time){
                 double freeze = down_time - buf_size;
                 time_played += buf_size;
                 cout << "delay before playing seg: " << round(time_played / segment_time) + 1 << " " << "for secs " << freeze << "; down_time: " <<
                         down_time << ", buf_size: " << buf_size << endl;
                 //using round to prevent truncating because time_played is slightly affected by approx errors
                 s.setDelay(freeze, round(time_played / segment_time) + 1);
                 buf_size = 0;
             }
             else{
                 buf_size -= down_time;
                 time_played += down_time;
             }

             current_time += down_time;

             if(r.getType() == Request::new_segment){ //policy decided for a segment which has been downloaded for the first time
                 buf_size += segment_time;
                 responses.push_back(received);
             } //we already have this segment, but policy decided to download a new one with a better quality
             else if(time_played <= received.m_number * segment_time){ //better quality segment and we can play it
                responses.at(received.m_number - 1) = received;
             }

        } catch (exception e) { //policy has finished, so we can play the whole media
            s.setTimeLeft(buf_size);
            time_played += buf_size + 0.1; //get rid of approx errors
            buf_size = 0;
        }
    }

    cout << endl << s.toString();

    ofstream out;
    out.open("policy.txt");


    for(Request & r: downloader.getRequests()){
        Segment s = r.getSegment();
        out << setprecision(6) << fixed; //microseconds
        out << left << setw(12) << r.getSendTime() << setw(3) << !r.m_is_media_buffering << setw(7) << s.m_number
            <<  s.m_coding_level << endl;
    }
}
