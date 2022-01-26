#include <vector>
#include "label.h"
#include "tensor.h"
#include <vector>
#include <iostream>
#include <tuple>
#include <memory>
#include "UTensor.h"

#ifndef PROGETTOTORSELLO_PROXY_H
#define PROGETTOTORSELLO_PROXY_H

/**
 * declaration of every template necessary for the proxy
 */

/**
 * declaration for the base proxy
 * @tparam T
 * @tparam L
 */
template<typename T, int D, typename L>
class proxy_tensor;

template<typename T,int D,  typename L>
class proxy_op_tensor;

/**
 * declaration for the Product class
 * @tparam T
 * @tparam L
 */
template<typename T, int D, typename L>
class proxy_prod_tensor;

/**
 * declaration for the Sum class used to sum Tensor
 * @tparam T
 * @tparam L
 */
template<typename T, int D, typename L>
class proxy_sum_tensor;

/**
 * declaration to create a Proxy Label Tensor from a Label Tensor
 * @tparam T
 * @tparam L
 */
template<typename T ,int D, typename L>
class proxy_label_tensor;

/**
 * creation for the class proxy_tensor
 * @tparam T
 * @tparam L
 */
template<typename T, int D, typename L>
class proxy_tensor{
    friend class proxy_op_tensor<T, D, L>;
    friend class proxy_label_tensor<T,D,L>;

public:
    virtual operator proxy_label_tensor<T,D,L>() = 0;



private:

    std::vector<label<L>> positions_list;

    proxy_tensor(std::vector<label<L>> labels) : positions_list(labels) {
        assert(labels.size() > static_cast<size_t>(0));
    }

    proxy_tensor() {
    }



};

template <typename T, int D, typename L>
class proxy_label_tensor : public proxy_tensor<T,D,L>{
    friend class proxy_tensor<T,D, L>;
    friend class proxy_op_tensor<T,D,L>;

public:
    proxy_label_tensor(Tensor::Tensor<T,D> info, std::initializer_list<L> names) : t_data(info), proxy_tensor<T,D,L>::proxy_tensor(labels_from_names(info, names)) {
        assert(info.get_rank() == names.size());
    }

    proxy_sum_tensor<T,D,L> operator+(const proxy_sum_tensor<T,D,L> &other) {
        /* return other + *this; */
        other.positions_list.push_back(*this);
        return other;
    }

    proxy_sum_tensor<T,D,L> operator+(const proxy_label_tensor<T,D,L> &other) {
        return proxy_sum_tensor<T,D,L>(*this, other);
    }

    proxy_prod_tensor<T,D,L> operator*(const proxy_prod_tensor<T,D,L> &other) {
        /* return other * *this; */
        other.positions_list.push_back(*this);
        return other;
    }

    proxy_prod_tensor<T,D,L> operator*(const proxy_label_tensor<T,D,L> &other) {
        return proxy_prod_tensor<T,D,L>(*this, other);
    }

    operator Tensor::Tensor<T,D> () {
        bool to_implode = false;
        auto dead = proxy_tensor<T,D,L>::positions_list.end();
        for (auto current_label = proxy_tensor<T,D,L>::positions_list.begin(); current_label < dead; ++current_label) {
            if (current_label->positions.size() > 1)
                to_implode = true;
        }
        if (!to_implode)
            return t_data;
        else {
            // There is a trace to compute
            return static_cast<Tensor::Tensor<T,D>>(static_cast<proxy_label_tensor<T,D,L>>(proxy_prod_tensor<T,D,L>(*this)));
        }
    }

    operator proxy_label_tensor<T,D,L>() {
        return *this;
    }

private:

    proxy_label_tensor(Tensor::Tensor<T,D> info, std::vector<label<L>> labels) : t_data(info), proxy_tensor<T,D, L>::proxy_tensor(copy_labels(info, labels)) {
        assert(info.get_rank() == labels.size());
    }

    static std::vector<label<L>> labels_from_names(Tensor::Tensor<T,D> &info, std::initializer_list<L> &names) {
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

    static std::vector<label<L>> copy_labels(Tensor::Tensor<T> &info, std::vector<label<L>> &labels) {
        std::vector<label<L>> result;
        typename std::vector<label<L>>::iterator iter = labels.begin();
        for (size_t index = 0; index < labels.size(); ++index) {
            result.push_back(label<L>(iter[index].name, info.get_dimension(index), index));
        }

        return result;
    }



    Tensor::Tensor<T,D> t_data;
};

template <typename T,int D, typename L>
class proxy_op_tensor: public proxy_tensor<T,D,L> {
    friend class proxy_tensor<T,D, L>;
    friend class proxy_label_tensor<T,D, L>;
    friend class proxy_sum_tensor<T,D, L>;
    friend class proxy_prod_tensor<T,D, L>;

public:

    operator Tensor::Tensor<T,D> () {
        return static_cast<Tensor::Tensor<T,D>>(static_cast<proxy_label_tensor<T,D,L>>(*this));
    }

    virtual operator proxy_label_tensor<T,D,L>() = 0;

private:
    std::vector<label<L>> unique_labels;
    std::vector<label<L>> common_labels;
    std::vector<proxy_label_tensor<T,D,L>> positions_list;

    proxy_op_tensor(proxy_label_tensor<T,D,L> st, proxy_label_tensor<T,D,L> nd) {
        positions_list.push_back(st);
        positions_list.push_back(nd);
    }

    proxy_op_tensor() {}

    void divide_labels() {
        typename std::vector<proxy_label_tensor<T,D,L>>::iterator current, succ, dead = positions_list.end();
        for (current = positions_list.begin(); current < dead; ++current) {
            for (label<L> &l: current->positions_list) {
                if (l.find_label(&common_labels) == std::numeric_limits<size_t>::max()) {
                    proxy_tensor<T,D,L>::positions_list.push_back(l);

                    for (succ = current + 1; succ < dead; ++succ) {
                        if (l.find_label(&succ->proxy_tensor<T,D,L>::positions_list) != std::numeric_limits<size_t>::max()) {
                            common_labels.push_back(l);
                            break;
                        }
                    }
                    if (succ == dead) {
                        unique_labels.push_back(l);
                    }
                }
            }
        }
    }
    bool is_label_in_all_positions(label<L> *l) {
        for (proxy_label_tensor<T,D,L> &proxy_tensor : positions_list) {
            if (l->find_label(&proxy_tensor.positions_list) == std::numeric_limits<size_t>::max())
                return false;
        }
        return true;
    }
    virtual proxy_label_tensor<T,D,L> eval() {};
};

template<typename T, int D, typename L>
class proxy_prod_tensor : public proxy_op_tensor<T,D,L> {

    friend class proxy_tensor<T,D,L>;
    friend class proxy_op_tensor<T,D,L>;
    friend class proxy_sum_tensor<T,D,L>;
    friend class proxy_label_tensor<T,D,L>;

public:
    proxy_sum_tensor<T,D,L>operator+(const proxy_label_tensor<T,D,L> &other) {
        return proxy_sum_tensor<T,D,L>(*this, other);
    }

    proxy_sum_tensor<T,D,L> operator+(const proxy_prod_tensor<T,D,L> &other) {
        return proxy_sum_tensor<T,D,L>(*this, other);
    }

    proxy_prod_tensor<T,D,L> operator*(const proxy_label_tensor<T,D,L> &other) {
        proxy_op_tensor<T,D,L>::positions_list.push_back(other);
        return *this;
    }

    proxy_prod_tensor<T,D,L> operator*(const proxy_prod_tensor<T,D,L> &other) {
        proxy_op_tensor<T,D,L>::positions_list.insert(
                proxy_op_tensor<T,D,L>::positions_list.end(),
                other.proxy_op_tensor<T,D,L>::positions_list.begin(),
                other.proxy_op_tensor<T,D,L>::positions_list.end());
        return *this;
    }

private:
    proxy_prod_tensor(proxy_label_tensor<T,D,L> st, proxy_label_tensor<T,D,L> nd) : proxy_op_tensor<T,D,L>::proxy_op_tensor(st, nd) {
    }

    proxy_prod_tensor(proxy_label_tensor<T,D,L> st) : proxy_op_tensor<T,D,L>::proxy_op_tensor() {
        proxy_op_tensor<T,D,L>::positions_list.push_back(st);
    }

    proxy_label_tensor<T,D,L> eval() override {
        proxy_op_tensor<T,D,L>::divide_labels();
        std::vector<size_t> pointer(proxy_op_tensor<T,D,L>::unique_labels.size());

            // There is at least one unique label in the labeled positions list and it isn't from only one tensor
            std::vector<size_t> unique_labels_size = label<L>::get_labels_sizes(&(proxy_op_tensor<T,D,L>::unique_labels));
            std::vector<size_t> common_labels_size = label<L>::get_labels_sizes(&(proxy_op_tensor<T,D,L>::common_labels));
            Tensor::Tensor<T,D> computed_tensor(unique_labels_size);
            --pointer[0];
            int current_dim;
            while (!proxy_tensor<T,D,L>::last_visitable_point(&pointer, &unique_labels_size)) {
                // Going trough the final tensor positions
                proxy_tensor<T,D,L>::next_position(&pointer, &unique_labels_size);
                std::vector<size_t> common_label_pointer(proxy_op_tensor<T,D,L>::common_labels.size());
                --common_label_pointer[0];

                while (!proxy_tensor<T,D,L>::last_visitable_point(&common_label_pointer, &common_labels_size)) {
                    // Going trough the common labels like they were a tensor pointer
                    proxy_tensor<T,D,L>::next_position(&common_label_pointer, &common_labels_size);
                    T product = 1;
                    for (proxy_label_tensor<T,D,L> &proxy_tensor_ptr : proxy_op_tensor<T,D,L>::positions_list) {
                        // Going trough the proxy_tensor
                        product *= proxy_tensor_ptr.data.at(proxy_tensor_ptr.get_position(&common_label_pointer,
                                                                                          &(proxy_op_tensor<T,D,L>::common_labels),
                                                                                          &pointer,
                                                                                          &(proxy_op_tensor<T,D,L>::unique_labels)));
                    }
                    computed_tensor.at(pointer) += product;
                }
            }
            return proxy_label_tensor<T,D,L>(computed_tensor, proxy_op_tensor<T,D,L>::unique_labels);
    }

    operator proxy_label_tensor<T,D,L> () override {
        return eval();
    }

};

template<typename T, int D, typename L>
class proxy_sum_tensor : public proxy_op_tensor<T,D,L> {

    friend class proxy_tensor<T, D, L>;
    friend class proxy_op_tensor<T, D, L>;
    friend class proxy_label_tensor<T, D, L>;
    friend class proxy_prod_tensor<T, D, L>;

public:
    proxy_sum_tensor<T,D,L> operator+(const proxy_label_tensor<T,D,L> &other) {
        proxy_op_tensor<T,D,L>::positions_list.push_back(other);
        return *this;
    }

    proxy_sum_tensor<T,D,L> operator+(const proxy_prod_tensor<T,D,L> &other) {
        prod_positions_list.push_back(other);
        return *this;
    }

    proxy_sum_tensor<T,D,L> operator+(const proxy_sum_tensor<T,D,L> &other) {
        proxy_op_tensor<T,D,L>::positions_list.insert(
                proxy_op_tensor<T,D,L>::positions_list.end(),
                other.proxy_op_tensor<T,D,L>::positions_list.begin(),
                other.proxy_op_tensor<T,D,L>::positions_list.end());

        prod_positions_list.insert(
                prod_positions_list.end(),
                other.prod_positions_list.begin(),
                other.prod_positions_list.end());
        return *this;
    }

    operator proxy_label_tensor<T,D,L> () override {
        return eval();
    }

private:

    std::vector<proxy_prod_tensor<T, D, L>> prod_positions_list;

    proxy_sum_tensor(proxy_label_tensor<T,D,L> st, proxy_label_tensor<T,D,L> nd) : proxy_op_tensor<T,D,L>::proxy_op_tensor(st, nd) {
    }

    proxy_sum_tensor(proxy_prod_tensor<T,D,L> st, proxy_label_tensor<T,D,L> nd) : proxy_op_tensor<T,D,L>::proxy_op_tensor() {
        prod_positions_list.push_back(st);
        proxy_op_tensor<T,D,L>::positions_list.push_back(nd);
    }

    proxy_sum_tensor(proxy_label_tensor<T,D,L> st, proxy_prod_tensor<T,D,L> nd) : proxy_sum_tensor(nd, st) {}

    proxy_label_tensor<T,D,L> eval() override {
        for (proxy_prod_tensor<T,D,L> proxy_tensor : prod_positions_list) {
            proxy_op_tensor<T,D,L>::positions_list.push_back(static_cast<proxy_label_tensor<T,D,L>>(proxy_tensor));
        }
        proxy_op_tensor<T,D,L>::divide_labels();

        assert((proxy_op_tensor<T,D,L>::unique_labels.size() == static_cast<size_t> (0)));
        for (label<L> &l : proxy_op_tensor<T,D,L>::common_labels) {
            assert((proxy_op_tensor<T,D,L>::is_label_in_all_positions(&l)));
        }

        std::vector<size_t> labels_size = label<L>::get_labels_sizes(&(proxy_op_tensor<T,D,L>::common_labels));
        std::vector<size_t> pointer(proxy_op_tensor<T,D,L>::common_labels.size());
        --pointer[0];
        Tensor::Tensor<T,D> computed_tensor(labels_size);
        while (!proxy_tensor<T,D,L>::last_visitable_point(&pointer, &labels_size)) {
            proxy_tensor<T,D,L>::next_position(&pointer, &labels_size);
            for (proxy_label_tensor<T,D,L> &proxy_tensor_ptr : proxy_op_tensor<T,D,L>::positions_list) {
                computed_tensor.at(pointer) += proxy_tensor_ptr.data.at(
                        proxy_tensor_ptr.get_position(&pointer, &(proxy_op_tensor<T,D,L>::common_labels)));
            }
        }
        return proxy_label_tensor<T,D,L>(computed_tensor, proxy_op_tensor<T,D,L>::common_labels);
    }

};
#endif //PROGETTOTORSELLO_PROXY_H
