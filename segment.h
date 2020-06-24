#ifndef SEGMENT_H
#define SEGMENT_H

class Segment{

public:
    Segment(){}

    Segment(unsigned int number, unsigned short coding_level, double quality, unsigned int size):
    m_number(number), m_coding_level(coding_level), m_quality(quality), m_size(size){
    }

    unsigned int m_number;
    unsigned short m_coding_level;
    double m_quality;
    unsigned int m_size;
};
#endif // SEGMENT_H
