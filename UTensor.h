#include <vector>
#include <iostream>
#include <tuple>
#include <memory>
#include "Tensor.h"

#ifndef PROGETTOTORSELLO_UTENSOR_H
#define PROGETTOTORSELLO_UTENSOR_H

namespace UTensor {


    template<class T>
    class UTensor {

    public:
        UTensor() : UTensor(std::vector<int>({})) {}

        template<typename  ...Ints>
        UTensor(Ints... DIM):UTensor(std::vector<int>({DIM...})) {}

        UTensor(const std::vector<int> &dimensions) :
                rank_(dimensions.size()),
                dimensions_(std::vector<int>(dimensions)),
                strides_(std::vector<int>(rank_)) {
            int n_elem = 1;
            for (int i = rank_ - 1; i >= 0; --i) {
                if (dimensions.at(i) <= 0)
                    throw std::invalid_argument("Dimensions value is equal to 0");
                strides_.at(i) = n_elem;
                n_elem *= dimensions_.at(i);
            }
            array_ = std::make_shared<std::vector<T>>(n_elem);
            start_ptr_ = 0;
            if (rank_ == 0) {
                end_ptr_ = 0;
            } else {
                end_ptr_ = n_elem;
            }
        }

        UTensor(const UTensor &ten) = default;

        UTensor(UTensor &&ten) = default;

        ~UTensor() = default;

        UTensor &operator=(const UTensor &ten) = default;

        UTensor &operator=(UTensor &&ten) = default;

        //copy?

        template<typename ...Ints>
        inline T get(Ints... INDEXES) const {
            int idxs[] = {INDEXES...};
            return getWithArray(idxs, (int) sizeof...(INDEXES));
        }

        inline T getInArray(const int indexes[], const int dim) const {
            try {
                checkExistence(indexes, dim);
            }
            catch (std::invalid_argument e) {
                throw e;
            }
            catch (std::out_of_range e) {
                throw e;
            }
            return array_->at(calculatePosition(indexes, dim));
        }

        inline T get(const std::vector<int> &indexes) const {
            try {
                checkExistence(indexes);
            }
            catch (std::invalid_argument e) {
                throw e;
            }
            catch (std::out_of_range e) {
                throw e;
            }
            return array_->at(calculatePosition(indexes));
        }

        template<typename ...Ints>
        inline T operator()(Ints... INDEXES) {
            return get(INDEXES...);
        }

        template<typename ...Ints>
        inline void set(const T &value, Ints... INDEXES) {
            int idxs[] = {INDEXES...};
            setWithArray(value, idxs, (int) sizeof...(INDEXES));
        }

        inline void setInArray(const T &value, const int indexes[], const int dim) {
            try {
                checkExistence(indexes, dim);
            }
            catch (std::invalid_argument e) {
                throw e;
            }
            catch (std::out_of_range e) {
                throw e;
            }
            array_->at(calculatePosition(indexes, dim)) = value;
        }

        inline void set(const T &value, const std::vector<int> &indexes) {
            try {
                checkExistence(indexes);
            }
            catch (std::invalid_argument e) {
                throw e;
            }
            catch (std::out_of_range e) {
                throw e;
            }
            array_->at(calculatePosition(indexes)) = value;
        }

        inline UTensor slicing(const int idx, const int val) {
            try {
                checkIndex(idx, val);
            }
            catch (std::out_of_range e) {
                throw (e);
            }
            std::vector<int> new_dimensions_(dimensions_);
            new_dimensions_.erase(new_dimensions_.begin() + idx);
            std::vector<int> new_strides_(strides_);
            new_strides_.erase(new_strides_.begin() + idx);
            std::shared_ptr<std::vector<T>> new_array_ = array_;
            int new_start_pointer_ = start_ptr_ + val * strides_.at(idx);
            int new_end_pointer_;
            if (new_dimensions_.size() == 0) {
                new_end_pointer_ = new_start_pointer_;
            } else {
                new_end_pointer_ = new_start_pointer_ + new_dimensions_.at(0) * new_strides_.at(0);
            }
            UTensor new_tensor(new_array_, new_dimensions_, new_strides_, new_start_pointer_, new_end_pointer_);
            return new_tensor;
        }

        UTensor operator[](const int val) {
            try {
                return slicing(0, val);
            }
            catch (std::out_of_range e) {
                throw e;
            }
        }

        inline UTensor slicing(const std::vector<std::pair<int, int>> &idx_val) {
            std::vector<std::pair<int, int>> idx_val_tmp(idx_val);
            UTensor tmp(*this);
            for (int i = 0; i < idx_val_tmp.size(); ++i) {
                tmp = tmp.slicing(idx_val_tmp[i].first, idx_val_tmp[i].second);
                for (int j = i + 1; j < idx_val_tmp.size(); ++j) {
                    if (idx_val_tmp[j].first > idx_val_tmp[i].first) {
                        idx_val_tmp[j].first--;
                    }
                }
            }
            return tmp;
        }


    private:
        int rank_;
        std::vector<int> dimensions_;
        std::vector<int> strides_;
        std::shared_ptr<std::vector<T>> array_;
        int start_ptr_;
        int end_ptr_;

        template<class T1, int R>
        friend class Tensor;

        inline int calculatePosition(const std::vector<int> &tupla) const {
            int position = start_ptr_;
            for (int i = 0; i < rank_; ++i) {
                position += strides_.at(i) * tupla.at(i);
            }
            return position;
        }

        inline int calculatePosition(const int tupla[], const int dim) const {
            int position = start_ptr_;
            for (int i = 0; i < dim; ++i) {
                position += strides_.at(i) * tupla[i];
            }
            return position;
        }

        inline void checkExistence(const std::vector<int> &indexes) const {
            if (indexes.size() != rank_)
                throw std::invalid_argument("Indexes number not equal to rank");
            for (int i = 0; i < rank_; ++i) {
                if (indexes.at(i) < 0 || indexes.at(i) >= dimensions_.at(i)) {
                    throw std::out_of_range("Indexes out of range");
                }
            }
        }

        inline void checkExistence(const int indexes[], const int dim) const {
            if (dim != rank_)
                throw std::invalid_argument("Indexes number not equal to rank");
            for (int i = 0; i < dim; ++i) {
                if (indexes[i] < 0 || indexes[i] >= dimensions_.at(i)) {
                    throw std::out_of_range("Indexes out of range");
                }
            }
        }

        inline void checkIndex(const int idx, const int val) const {
            if (idx < 0 || idx >= rank_ || val < 0 || val >= dimensions_.at(idx)) {
                throw std::out_of_range("Indexes out of range");
            }
        }

        inline void checkIndex(const int idx) const {
            if (idx < 0 || idx >= rank_) {
                throw std::out_of_range("Indexes out of range. No possible operations.");
            }
        }

    };
}
#endif //PROGETTOTORSELLO_UTENSOR_H
