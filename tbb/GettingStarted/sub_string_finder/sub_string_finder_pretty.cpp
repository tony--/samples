/*
    ==============================================================

    SAMPLE SOURCE CODE - SUBJECT TO THE TERMS OF SAMPLE CODE LICENSE AGREEMENT,
    http://software.intel.com/en-us/articles/intel-sample-source-code-license-agreement/

    Copyright (C) Intel Corporation

    THIS FILE IS PROVIDED "AS IS" WITH NO WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT
    NOT LIMITED TO ANY IMPLIED WARRANTY OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
    PURPOSE, NON-INFRINGEMENT OF INTELLECTUAL PROPERTY RIGHTS.

    =============================================================
*/

#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <algorithm>    //std::max

#include "tbb/parallel_for.h"
#include "tbb/blocked_range.h"

static const std::size_t N = 9;

class SubStringFinder {
    const std::string &str;
    std::vector<std::size_t> &max_array;
    std::vector<std::size_t> &pos_array;
public:
    void operator() ( const tbb::blocked_range<std::size_t>& r ) const {
        for (std::size_t i = r.begin(); i != r.end(); ++i) {
            std::size_t max_size = 0, max_pos = 0;
            for (std::size_t j = 0; j < str.size(); ++j) {
                if (j != i) {
                    std::size_t limit = str.size()-(std::max)(i,j);
                    for (std::size_t k = 0; k < limit; ++k) {
                        if (str[i + k] != str[j + k])
                            break;
                        if (k+1 > max_size) {
                            max_size = k+1;
                            max_pos = j;
                        }
                    }
                }
            }
            max_array[i] = max_size;
            pos_array[i] = max_pos;
        }
    }

    SubStringFinder( const std::string &s, std::vector<std::size_t> &m, std::vector<std::size_t> &p ) :
        str(s), max_array(m), pos_array(p) { }
};

int main() {
    using namespace tbb;

    std::string str[N] = { std::string("a"), std::string("b") };
    for (std::size_t i = 2; i < N; ++i)
        str[i] = str[i-1]+str[i-2];
    std::string &to_scan = str[N-1];
    const std::size_t num_elem = to_scan.size();
    std::cout << "String to scan: " << to_scan << std::endl;

    std::vector<std::size_t> max( num_elem );
    std::vector<std::size_t> pos( num_elem );

    parallel_for( blocked_range<std::size_t>( 0, num_elem, 100 ),
            SubStringFinder( to_scan, max, pos ) );

    for (std::size_t i = 0; i < num_elem; ++i) {
        for (std::size_t j = 0; j < num_elem; ++j) {
            if (j >= i && j < i + max[i])
                std::cout << "_";
            else
                std::cout << " ";
        }
        std::cout << std::endl << to_scan << std::endl;

        for (std::size_t j = 0; j < num_elem; ++j) {
            if (j >= pos[i] && j < pos[i] + max[i])
                std::cout << "*";
            else
                std::cout << " ";
        }
        std::cout << std::endl;
    }

    return 0;
}

