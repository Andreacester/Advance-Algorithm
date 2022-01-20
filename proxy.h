#include <vector>
#include "label.h"

#ifndef PROGETTOTORSELLO_PROXY_H
#define PROGETTOTORSELLO_PROXY_H


template<typename T, typename L>
class proxy_tensor;

template<typename T, typename L>
class proxy_sum_tensor;

template<typename T, typename L>
class proxy_label_tensor;

template<typename T, typename L>
class proxy_tensor{
    friend class proxy_sum_tensor<T,L>;
    friend class proxy_label_tensor<T,L>;

public:
    virtual operator proxy_label_tensor<T, L>() = 0;

private:
    std::vector<label<L>> list_positions;

    proxy_tensor(std::vector<label<L>> labels) : list_positions(labels){
            if(labels.size()>static_cast<size_t>(0)) throw std::exception();
    }

    proxy_tensor(){

    }

    bool is_last_position(const std::vector<size_t> *point, const std::vector<size_t> *dimensions){
        bool last=true;
        for (size_t i = static_cast<size_t>(0); i < point->size(); ++i) {
            if(point->at(i)== std::numeric_limits<size_t>::max() || point->at(i) < dimensions->at(i) - 1) {
                last=false;
                break;
            }
        }
        return last;
    }

    void next(std::vector<size_t> *pointer, std::vector<size_t> *origin){
        int current = 0;
        do {
            if (++pointer->at(current) == origin->at(current)) {
                pointer->at(current) = static_cast<size_t>(0);
                ++current;
            }
            else {
                break;
            }
        } while (current < pointer->size());
    }

};




#endif //PROGETTOTORSELLO_PROXY_H
