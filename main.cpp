#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>

using namespace std;

static float buf_size = 0;
static float segment_time = 0.5;
static float time_played = 0;
static float media_time;
static int no_segments = 0;

static vector<int> M;
static vector<int> Q;
static vector<int> B;
class Request;
static vector<Request> R;
struct Segment;
static vector<Segment> responses;

struct Segment{

    Segment(){}

    Segment(unsigned long long number, unsigned short coding_level):
        m_number(number), m_coding_level(coding_level){
        unsigned int pos = (number - 1) * 5 + coding_level;
        m_quality = Q.at(pos);
        m_size = M.at(pos);
    }

    unsigned int m_number;
    unsigned short m_coding_level;
    float m_quality;
    unsigned int m_size;
};

class Downloader;
class Request{

friend Downloader;
public:
    enum RequestType{new_segment, better_segment, null};

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
public:
    static float get(Request & r){
        r.m_requested_at = time_played;
        R.push_back(r);
        float down_time = r.m_seg.m_size / B.at(number++);
        r.m_arrived_at = r.m_requested_at + down_time;
        return down_time;
    }
};


int Downloader::number = 0;

class Policy;

class Stats{

private:
    vector<float> P;
    vector<float> U;
    static float beta;
    static float gamma;


public:

    Stats(){
        P = vector<float>(no_segments, 0.0);
        U = vector<float>(no_segments, 0.0);
    }

    void updatePhi(float delay_time, int segment_number){
        P.at(segment_number - 1) = delay_time;
    }

    double computeOverallQoE(){
        U.at(0) = responses.at(0).m_quality - exp(gamma * P.at(0) / segment_time);
        for(int i = 1; i < no_segments; i++)
            U.at(i) = responses.at(i).m_quality -
                    beta * fabs(responses.at(i).m_quality - responses.at(i-1).m_quality) -
                    exp(gamma * P.at(i) / segment_time);
        double qoe = 0;
        for (float v: U)
            qoe += v;
        return qoe;
    }

    void printInfo(){
        double qoe = computeOverallQoE();
        cout << qoe << endl;
    }

};

float Stats::beta = 0.5;
float Stats::gamma = 0.5;


class Policy
{

private:
    static unsigned short k;
    int current_down_quality = 0;
    Stats & stats;

    void preFetch(int coding_level){
        float time = 0;
        for(int i = 1; i <= k; i++){
            Request r(Segment(i, coding_level), Request::new_segment);
            time += Downloader::get(r);
            responses.push_back(r.getSegment());
        }
        stats.updatePhi(time, 1);
        buf_size += k * segment_time;
    }

public:

    Policy(Stats & s): stats(s){
        preFetch(current_down_quality);
    }

    Request getRequest(){
        if(buf_size < k * segment_time)
            current_down_quality = max(0, current_down_quality - 1);
        else
            current_down_quality = min(4, current_down_quality + 1);
        if(responses.size() < no_segments)
            return Request(Segment(responses.size() + 1, current_down_quality), Request::new_segment);
        else
           throw exception();
    }
};

unsigned short Policy::k = 5;


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
       B.push_back(bitrate);

   no_segments = M.size()/5;
   media_time = no_segments * segment_time;
   return true;
}

int main()
{
    if(!readFiles()){
        cout << "cannot open files, exit..." << endl;
        exit(1);
    }

    Stats s;
    Policy p(s);

    while(time_played < media_time){

        try {
             Request r = p.getRequest();
             float down_time = Downloader::get(r);
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

             //play done

             //it's up to the current policy to decide whether to download a new segment or a better segment

             if(r.getType() == Request::new_segment){ //policy decided for a segment never ever downloaded
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

    s.printInfo();

    ofstream out;
    out.open("policy.txt");

}
