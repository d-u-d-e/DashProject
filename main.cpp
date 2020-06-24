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

//global variables

double buf_size = 0;
double segment_time = 0.5;
double time_played = 0;
unsigned int no_segments;
double current_time = 0.0;

//an array used to keep track of bitrates
static vector<unsigned int> B;

bool readFiles(MPD & m){
    ifstream mpd("MPD.txt");
    ifstream channel("channel.txt");

    if(!mpd.is_open() || !channel.is_open())
        return false;

   unsigned int seg, size;
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

   no_segments = m.size();
   return true;
}

int main()
{
    MPD mpd;
    if(!readFiles(mpd)){
        cout << "cannot open files, exit..." << endl;
        exit(1);
    }

    vector<Segment> responses;
    Stats s(responses);
    Downloader downloader(s, B);
    Policy3 p(s, responses, downloader, mpd, 40);

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
                 buf_size = 0;
                 unsigned int seg = round(time_played / segment_time) + 1;
                 if(seg <= no_segments){
                     cout << "delay before playing seg: " << seg << ", " << "freeze time: " << freeze << endl;
                     s.setDelay(freeze, round(time_played / segment_time) + 1);
                 }
             }
             else{
                 buf_size -= down_time;
                 time_played += down_time;
                 if(received.m_number == no_segments) s.setTimeLeft(buf_size);
             }

             current_time += down_time;

             if(r.getType() == Request::new_segment){ //policy decided for a segment which has been downloaded for the first time
                 buf_size += segment_time;
                 responses.push_back(received);
             } //we already have this segment, but policy decided to download a new one with a better quality
             else if(time_played <= received.m_number * segment_time){ //we can play it
                 responses.at(received.m_number - 1) = received; //so we update the corresponding response
             }

        } catch (BasePolicy::policy_done e) { //policy has finished, play the rest
            time_played += buf_size + 0.1; //+ 0.1 to get rid of approx errors
            buf_size = 0;
        }
    }

    cout << endl << s.toString();

    ofstream out;
    out.open("policy.txt");
    //ouput is formatted in a readable manner
    for(Request & r: downloader.getRequests()){
        Segment s = r.getSegment();
        out << setprecision(6) << fixed; //microseconds
        out << left << setw(12) << r.getSendTime() << setw(3) << !r.m_is_media_buffering << setw(7) << s.m_number
            <<  s.m_coding_level << endl;
    }
}
