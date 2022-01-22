#include <vector>
#include <iostream>
#include <tuple>
#include <memory>

#ifndef PROGETTOTORSELLO_TENSOR_H
#define PROGETTOTORSELLO_TENSOR_H
namespace Tensor {

    template<class T, int D>
    class Tensor;

/**
 * Tensor with Rank specified
 * @tparam T
 * @tparam R
 */
    template<class T, int R = 0>
    class Tensor {

    public:
        /**
         * default and public constructor
         * call the other constructor to build the Tensor
         */
        Tensor() : Tensor(std::vector<int>(R, 1)) {}

        /**
         *
         * @tparam Ints parameter need to be int and we don't specify the number
         * @param DIM dimension
         */
        template<typename ...Ints>
        Tensor(Ints... DIM) : Tensor(std::vector<int>({DIM...})) {}

        /**
         *
         * @param dimensions dimensions of the tensor we wanr to create
         */
        Tensor(const std::vector<int> &dimensions) {
            if (dimensions.size() != R) {
                throw std::invalid_argument("Number of dimensions different from " + std::to_string(R));
            }
            int n_elem = 1;
            for (int i = R - 1; i >= 0; --i) {
                if (dimensions[i] <= 0)
                    throw std::invalid_argument("Dimensions value is equal to 0");
                dimensions_[i] = dimensions[i];
                strides_[i] = n_elem;
                n_elem *= dimensions_[i];
            }
            array_ = std::make_shared<std::vector<T>>(n_elem);
            start_ptr_ = 0;
            end_ptr_ = n_elem;
        }

        //sharing
        Tensor(const Tensor &tensor) = default;

        //move
        Tensor(Tensor &&tensor) = default;

        /**
         * default destructor for the Tensor
         */
        ~Tensor() = default;

        //sharing
        Tensor &operator=(const Tensor &tensor) = default;

        //move
        Tensor &operator=(Tensor &&tensor) = default;

        //copy

        Tensor copy() {
            int n_elem = 1;
            int n_strides_[R];
            for (int i = R - 1; i >= 0; --i) {
                n_strides_[i] = n_elem;
                n_elem *= dimensions_[i];
            }
            std::shared_ptr<std::vector<T>> n_array = std::make_shared<std::vector<T>>(n_elem);
            Tensor<T, R> copy_t_(n_array, dimensions_, n_strides_, 0, dimensions_[0] * strides_[0]);
            const auto j = copy_t_.start_ptr_;
            //da controllare e rifare senza iterator
            /*//copy da pensare
            for(auto i = copy_t_.start_ptr_; i < copy_t_.end_ptr_; ++i){
                copy_t_.setArray(i, j, R);
                ++j;
            }*/
            return copy_t_;
        }

        /**
         * get an element from the tensor
         * @tparam Ints
         * @param INDEXES
         * @return return the element calling the getInArray function
         */
        template<typename ...Ints>
        inline T get(Ints... INDEXES) const {
            int indexes[] = {INDEXES...};
            return getInArray(indexes, (int) sizeof...(INDEXES));
        }

        //get element x 2
        //check existence
        /**
         *
         * @param indexes
         * @param dim
         * @return element of the array calculating the position
         */
        inline T getInArray(const int indexes[], const int dim) {
            return array_->at(calculatePosition(indexes, dim));
        }

        /**
         * set an element of the Tensor calling the setArray function
         * @tparam Ints
         * @param value
         * @param INDEXES
         */
        template<typename ...Ints>
        inline void set(const T &value, Ints... INDEXES) {
            int index[] = {INDEXES...};
            setArray(value, index, (int) sizeof...(INDEXES));
        }

        /**
         * set the element on the array with the value
         * @param value
         * @param indexes
         * @param dim
         */
        inline void setArray(const T &value, const int indexes[], const int dim) {
            //errori
            array_->at(calculatePosition(indexes, dim)) = value;
            //std::cout << "elem: " << array_->at(calculatePosition(indexes,dim)) << std::endl;
        }


        /**
         * produce a lower rank Tensor with the same data as before
         * @param index
         * @param val
         * @return the tensor with a Rank-1
         */
        inline Tensor<T, R - 1> slicing(const int index, const int val) {
            //try catch

            if (R - 1 == 0) {
                return Tensor<T, R - 1>(array_, (int *) NULL, (int *) NULL, start_ptr_ + val * strides_[index],
                                        (int) NULL);
            } else {
                int n_dimensions_[R - 1];
                int n_strides_[R - 1];
                for (int i = 0; i < R; ++i) {
                    if (i < index) {
                        n_dimensions_[i] = dimensions_[i];
                        n_strides_[i] = strides_[i];
                    } else if (i > index) {
                        n_dimensions_[i - 1] = dimensions_[i];
                        n_strides_[i - 1] = strides_[i];
                    }
                }
                int n_start_ptr = start_ptr_ + val * strides_[index];
                int n_end_ptr = n_start_ptr + n_dimensions_[0] * n_strides_[0];
                return Tensor<T, R - 1>(array_, n_dimensions_, n_strides_, n_start_ptr, n_end_ptr);
            }
        }

        Tensor<T, R - 1> operator[](const int val) {
            return slicing(0, val);
        }

        /**
         * function used to debug tensor
         * @param name
         */
        void printTensor(const std::string name) {
            if (array_ == nullptr) {
                std::cout << "Empty Tensor Variable!!!" << std::endl;
            } else {
                std::cout << "start: " << start_ptr_ << std::endl;
                std::cout << "end: " << end_ptr_ << std::endl;
                for (int i = 0; array_->size() > i; i++) {
                    std::cout << "elem: " << array_->at(i) << std::endl;
                }

            }
            std::cout << std::endl;
        }

        constexpr size_t get_rank() const { return R; }

    private:
        int dimensions_[R];
        int strides_[R];
        //dubbio
        std::shared_ptr<std::vector<T>> array_;
        int start_ptr_;
        int end_ptr_;

        friend class Tensor<T, R + 1>;

        /**
         * calculatePosition
         * @param tupla
         * @param dim dimension
         * @return position return the calculate position in the tensor
         */

        inline int calculatePosition(const int tupla[], const int dim) {
            if (dim <= 0) throw std::out_of_range("Out of range");
            int position = start_ptr_;
            for (int i = 0; i < dim; ++i) {
                position += strides_[i] * tupla[i];
            }
            return position;
        }

        /**
         * private constructur
         * inizialize the Tensor with the parameter given in input
         * @param array shared array containg the tensor
         * @param dimensions dimension of the tensor
         * @param strides strides of the tensor
         * @param start_ptr starting pointer
         * @param end_ptr end pointer
         */
        Tensor(
                const std::shared_ptr<std::vector<T>> &array,
                int dimensions[],
                int strides[],
                const int start_ptr,
                const int end_ptr
        ) : array_(array),
            start_ptr_(start_ptr),
            end_ptr_(end_ptr) {
            for (int i = 0; i < R; ++i) {
                dimensions_[i] = dimensions[i];
                strides_[i] = strides[i];
            }
        }

    };

/**
 * tensor con Rank 0
 * @tparam T
 */
    template<class T>
    class Tensor<T, 0> {
    public:

        /**
         * public constructor with rank 0
         * the tensor will be crate shared
         */
        Tensor() {
            array_ = std::make_shared<std::vector<T>>(1);
        }

        //1 element
        Tensor(const T elem) : Tensor() {
            array_->at(0) = elem;
        }

        /**
         * used for sharing a Tensor
         * @param tensor
         */
        Tensor(const Tensor &tensor) = default;

        /**
         * move a tensor
         * @param tensor
         */
        Tensor(Tensor &&tensor) = default;

        /**
         * we use the default destructor for the Tensor
         */
        ~Tensor() = default;

        /**
        * used for sharing a Tensor
        * @param tensor
        */
        Tensor &operator=(const Tensor &tensor) = default;

        /**
         * move a tensor
         * @param tensor
         */
        Tensor &operator=(Tensor &&tensor) = default;

        /**
         *
         * @param value value to be assigned
         * @return tensor with value assigned
         */
        Tensor &operator=(const T value) {
            array_->at(start_ptr_) = value;
            return this;
        }

        /**
         * copy a Tensor to an another tensor
         */
        Tensor<T> copy() {
            Tensor<T, 0> n_tensor_;
            n_tensor_.set(this->get());
        }

        /**
         * get an element from the Tensor
         * @return the element of the Tensor
         * to do: control check about the bounds
         */
        inline T get() const {
            return array_->at(start_ptr_);
        }

        /**
         * set an element of the Tensor with a value:
         * @param value
         */
        inline void set(const T value) {
            if (array_ == nullptr) {
                throw std::out_of_range("Out of range exception");
            }
            array_->at(start_ptr_) = value;
        }
        constexpr size_t get_rank() const { return 0; }

    private:

        /**
         * used to create an shared Tensor
         */
        std::shared_ptr<std::vector<T>> array_;
        int start_ptr_;
        int end_ptr_;

        friend class Tensor<T, 1>;

        template<class T1, int ... D>
        /**
         * private constructor
         * @tparam T1 Tensor
         * @tparam D dimensions
         * @param array Aarray of Tensor
         * @param start_ptr start pointer
         */
        Tensor(const std::shared_ptr<std::vector<T>> array, const int start_ptr):
                array_(array),
                start_ptr_(start_ptr) {}

        /**
         * private constructor for the tensor
         */
        Tensor(
                const std::shared_ptr<std::vector<T>> &array,
                int dimensions[],
                int strides[],
                const int start_ptr,
                const int end_ptr
        ) : array_(array),
            start_ptr_(start_ptr) {}


    };
}


#endif //PROGETTOTORSELLO_TENSOR_H
