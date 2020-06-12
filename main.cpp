#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>


#include "stats.h"
#include "policy.h"
#include "request.h"

using namespace std;

vector<unsigned int> M;
vector<float> Q;
static vector<unsigned int> B;

float buf_size = 0;
float segment_time = 0.5;
float time_played = 0;
unsigned int no_segments;


bool readFiles(){
    ifstream mpd("MPD.txt");
    ifstream channel("channel.txt");

    if(!mpd.is_open() || !channel.is_open())
        return false;

   int seg, size;
   float quality;

   while(mpd >> seg >> size >> quality){
        M.push_back(size);
        Q.push_back(quality);
   }

   int bitrate;
   while(channel >> bitrate)
       B.push_back(abs(bitrate));

   no_segments = M.size()/5;
   return true;
}

int main()
{
    if(!readFiles()){
        cout << "cannot open files, exit..." << endl;
        exit(1);
    }

    vector<Segment> responses;
    Stats s(responses);
    Downloader downloader(B);
    Policy p(s, responses, downloader);

    float media_time = no_segments * segment_time;

    while(time_played < media_time){

        try {
             Request r = p.getRequest();
             float down_time = downloader.get(r);

             Segment received = r.getSegment();

             if(buf_size < down_time){
                 float freeze = down_time - buf_size;
                 time_played += buf_size;
                 s.updatePhi(freeze, time_played / segment_time + 1);
                 buf_size = 0;
             }
             else{
                 buf_size -= down_time;
                 time_played += down_time;
             }

             if(r.getType() == Request::new_segment){ //policy decided for a segment which has been downloaded for the first time
                 buf_size += segment_time;
                 responses.push_back(received);
             } //we already have this segment, but policy decided to download a new one with a better quality
             else if(time_played <= received.m_number * segment_time){ //better quality segment and we can play it
                responses.at(received.m_number - 1) = received;
             }

        } catch (exception e) { //policy has finished, so we can play the whole media
            time_played += buf_size;
            if(time_played < media_time)
                cout << "ERROR" << endl;
        }
    }

    cout << s.toString();

    ofstream out;
    out.open("policy.txt");

}
