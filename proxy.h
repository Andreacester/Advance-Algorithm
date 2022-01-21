#include <vector>
#include "label.h"
#include "tensor.h"
#include <vector>
#include <iostream>
#include <tuple>
#include <memory>

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

template <typename T, typename L>
class proxy_label_tensor : public proxy_tensor<T,L>{
    friend class proxy_tensor<T,L>;

public:
    proxy_label_tensor(Tensor::Tensor<T> info, std::initializer_list<L> names) : data(info), proxy_tensor<T, L>::proxy_tensor(labels_from_names(info, names)) {
        if(info.get_rank() == names.size()) throw std::exception();
    }

    proxy_sum_tensor<T, L> operator+(const proxy_sum_tensor<T, L> &other) {
        /* return other + *this; */
        other.positions_list.push_back(*this);
        return other;
    }

    proxy_sum_tensor<T, L> operator+(const proxy_label_tensor<T, L> &other) {
        return proxy_sum_tensor<T, L>(*this, other);
    }

    /*proxy_prod_tensor<T, L> operator*(const proxy_prod_tensor<T, L> &other) {
         return other * *this;
        other.positions_list.push_back(*this);
        return other;
    }

    proxy_prod_tensor<T, L> operator*(const proxy_labeled_tensor<T, L> &other) {
        return proxy_prod_tensor<T, L>(*this, other);
    }*/

    operator Tensor::Tensor<T> () {
        bool to_implode = false;
        auto dead = proxy_tensor<T, L>::positions_list.end();
        for (auto current_label = proxy_tensor<T, L>::positions_list.begin(); current_label < dead; ++current_label) {
            if (current_label->positions.size() > 1)
                to_implode = true;
        }
        if (!to_implode)
            return data;
        else {
            // There is a trace to compute
            return static_cast<Tensor::Tensor<T>>(static_cast<proxy_label_tensor<T, L>>(proxy_prod_tensor<T, L>(*this)));
        }
    }

    operator proxy_label_tensor<T, L>() {
        return *this;
    }

private:
    Tensor::Tensor<T> data;

    static std::vector<label<L>> labels_from_names(Tensor::Tensor<T> &info, std::initializer_list<L> &names) {
        std::vector<label<L>> result;
        typename std::vector<label<L>>::iterator iter_result;

        size_t current_dim = static_cast<size_t>(0);

        for (const L &name : names) {
            iter_result = result.begin();
            while (iter_result != result.end()) {
                if ((*iter_result).name == name) {
                    (*iter_result).positions.push_back(current_dim);
                    break;
                }
                ++iter_result;
            }
            if (iter_result == result.end()) {
                result.push_back(label<L>(name, info.get_dimension(current_dim), current_dim));
            }
            ++current_dim;
        }
        return result;
    }

    /**
     * Copy a labels list.
     * @param info Tensor
     * @param labels List of label's objects
     * @return Vector of labels
     */
    static std::vector<label<L>> copy_labels(Tensor::Tensor<T> &info, std::vector<label<L>> &labels) {
        std::vector<label<L>> result;
        typename std::vector<label<L>>::iterator iter = labels.begin();
        for (size_t index = 0; index < labels.size(); ++index) {
            result.push_back(label<L>(iter[index].name, info.get_dimension(index), index));
        }

        return result;
    }

    /**
     * Associates the labels to the actual dimension on the tensor and moves to the specified position.
     * @throw 1 Tried to compose position with not all the labels in common label list
     * @param cm_label_pointer Positions of the common labels list
     * @param cm_labels Common labels list
     * @param [uq_label_pointer] Positions of the unique labels list
     * @param [uq_labels] Unique labels list
     * @return Vector of the position on the tensor
     */
    std::vector<size_t> get_position(std::vector<size_t> *cm_label_pointer, std::vector<label<L>> *cm_labels, std::vector<size_t> *uq_label_pointer = nullptr, std::vector<label<L>> *uq_labels = nullptr) {
        std::vector<size_t> position(data.get_rank());
        size_t order = static_cast<size_t>(0);
        for (auto label_item = proxy_tensor<T, L>::positions_list.begin(); label_item < proxy_tensor<T, L>::positions_list.end(); ++label_item) {
            size_t index = label_item->find_label(cm_labels);
            if (index != std::numeric_limits<size_t>::max()) {
                for (size_t pos = static_cast<size_t>(0); pos < label_item->positions.size(); ++pos)
                    position[label_item->positions[pos]] = cm_label_pointer->at(index);
            }
            else {
                if (uq_labels == nullptr || uq_label_pointer == nullptr) throw 1;
                size_t index = label_item->find_label(uq_labels);
                if (index != std::numeric_limits<size_t>::max()) {
                    for (size_t pos = static_cast<size_t>(0); pos < label_item->positions.size(); ++pos)
                        position[label_item->positions[pos]] = uq_label_pointer->at(index);
                }
            }
            ++order;
        }
        return position;
    }

    proxy_label_tensor(Tensor::Tensor<T> info, std::vector<label<L>> labels) : data(info), proxy_tensor<T, L>::proxy_tensor(copy_labels(info, labels)) {
        assert(info.get_rank() == labels.size());
    }
};

template <typename T, typename L>
class proxy_op_tensor: public proxy_tensor<T,L>{
    friend class proxy_tensor<T,L>;
    friend class proxy_label_tensor<T,L>;

public:
    operator Tensor::Tensor<T>(){
        return static_cast<Tensor::Tensor<T>>(static_cast<proxy_label_tensor<T, L>>(*this));
    }
private:
    std::vector<label<L>> unique_labels;
    std::vector<label<L>> common_labels;
    std::vector<proxy_label_tensor<T,L>> position_list;

    proxy_op_tensor(proxy_label_tensor<T,L> st, proxy_label_tensor<T,L> nd) {
        position_list.push_back(st);
        position_list.push_back(nd);
    }

    proxy_op_tensor() {}

    /**
     * *
     * Separating common labels form unique labels
     */
     void divide_labels(){
         typename std::vector<proxy_label_tensor<T,L>>::iterator current, succ, dead = position_list.end();
        for (current = position_list.begin(); current < dead ; ++current) {
            fopen(label<L> &l: current->position_list){
                if(l.find_label(&common_labels) == std::numeric_limits<size_t>::max()){
                    proxy_tensor<T, L>::position_list.pusg_back(l);
                    for (succ = current; succ < dead; ++succ) {
                        if(l.find_label(&succ->porxy_tensor<T,L>::postion_list) != std::numeirc_limits<size_t>::max()){
                            common_labels.push_back(l);
                            break;
                        }
                    }
                    if(succ == dead){
                        unique_labels.push_back(l);
                    }
                }
            }
        }
     }
    /**
        * check if a label is in every item of the labeles position list
        * @param l Label to search
        * @return true id the label is in all the label positions, otherwise false
        */
    bool is_label_all_in(label<L> *l){
        for (proxy_labeled_tensor<T, L> &proxy_tensor : position_list) {
            if(l->find_label(&proxy_tensor.position_list) == std::numeric_limits<size_t>::max())
                return false;
        }
        return true;
    }

    virtual proxy_labeled_tensor<T, L> eval(){};

public:
    operator Tensor::Tensor<T> (){
        return static_cast<Tensor::Tensor<T>>(static_cast<proxy_label_tensor<T,L>>(*this));
    }
    virtual operator proxy_label_tensor<T,L>() = 0;
};

#endif //PROGETTOTORSELLO_PROXY_H
