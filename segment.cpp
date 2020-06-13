#include "segment.h"
#include <vector>

extern std::vector<double> Q;
extern std::vector<unsigned int> M;

Segment::Segment(unsigned int number, unsigned short coding_level):
    m_number(number), m_coding_level(coding_level)
{
    unsigned int pos = (number - 1) * 5 + coding_level;
    m_quality = Q.at(pos);
    m_size = M.at(pos);
}
