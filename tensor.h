#include <vector>
#include <iostream>
#include <tuple>
#include <memory>

#ifndef PROGETTOTORSELLO_TENSOR_H
#define PROGETTOTORSELLO_TENSOR_H

template <class T, int D>
class Tensor;

template <class T, int R=0>
class Tensor {
private:
    int dimensions_[R];
    int strides_[R];
    //dubbio
    std::shared_ptr<std::vector<T>> array_;
    int start_ptr_;
    int end_ptr_;

    friend class Tensor<T, R+1>;

    /*
     * eccezioni index
     *
     * calculatePosition
     * */

    inline int calculatePosition(const int tupla[], const int dim){
        int position=start_ptr_;
        for (int i = 0; i < dim; ++i) {
            position+=strides_[i]*tupla[i];
        }
        return position;
    }

    //il costruttore
    Tensor(
            const std::shared_ptr<std::vector<T>>& array,
            int dimensions[],
            int strides[],
            const int start_ptr,
            const int end_ptr
            ): array_ (array),
            start_ptr_(start_ptr),
            end_ptr_(end_ptr)

    {
        for (int i=0; i < R; ++i){
            dimensions_[i]=dimensions[i];
            strides_[i]=strides[i];
        }
    }
public:
    //default contsructor
    Tensor() : Tensor(std::vector<int>(R,1)){}

    template<typename ...Ints>
    Tensor(Ints... DIM) : Tensor(std::vector<int>({DIM...})){}

    Tensor(const std::vector<int>& dimensions){
        if(dimensions.size()!=R){
            throw std::invalid_argument("Number of dimensions different from "+ std::to_string(R));
        }
        int n_elem=1;
        for (int i = R-1; i >= 0; --i) {
            if(dimensions[i]<=0)
                throw std::invalid_argument("Dimensions value is equal to 0");
            dimensions_[i]=dimensions[i];
            strides_[i]=n_elem;
            n_elem*=dimensions_[i];
        }
        array_ = std::make_shared<std::vector<T>>(n_elem);
        start_ptr_=0;
        end_ptr_=n_elem;
    }

    //sharing
    Tensor(const Tensor& tensor)=default;

    //move
    Tensor(Tensor&& tensor)=default;

    //destructor
    ~Tensor() = default;

    //sharing
    Tensor& operator=(const Tensor& tensor)=default;

    //move
    Tensor& operator=(Tensor&& tensor)=default;

    //copy

    Tensor copy(){
        int n_elem=1;
        int n_strides_[R];
        for (int i = R-1; i >= 0 ; --i) {
            n_strides_[i]=n_elem;
            n_elem*=dimensions_[i];
        }
        std::shared_ptr<std::vector<T>> n_array = std::make_shared<std::vector<T>>(n_elem);
        Tensor<T,R> copy_t_(n_array, dimensions_, n_strides_, 0, dimensions_[0]*strides_[0]);
        const auto j = copy_t_.start_ptr_;
        //da controllare e rifare senza iterator
        /*//copy da pensare
        for(auto i = copy_t_.start_ptr_; i < copy_t_.end_ptr_; ++i){
            copy_t_.setArray(i, j, R);
            ++j;
        }*/
        return copy_t_;
    }

    //get element
    template<typename ...Ints>
    inline T get(Ints... INDEXES) const{
        int indexes[] = {INDEXES...};
        return getInArray(indexes, (int) sizeof...(INDEXES));
    }

    //get element x 2
    //check existence
    inline T getInArray(const int indexes[],const int dim) {
        return array_->at(calculatePosition(indexes,dim));
    }

    //set
    template<typename ...Ints>
    inline void set(const T& value, Ints... INDEXES){
        int index[]={INDEXES...};
        setArray(value, index,(int)sizeof...(INDEXES));
    }

    //set
    inline void setArray(const T& value, const int indexes[], const int dim){
        //errori
        array_->at(calculatePosition(indexes,dim))=value;
        //std::cout << "elem: " << array_->at(calculatePosition(indexes,dim)) << std::endl;
    }


    //slicing
    inline Tensor<T, R-1> slicing(const int index, const int val){
        //try catch

        if(R-1==0){
            return Tensor <T,R-1>(array_,(int *)NULL,(int *)NULL, start_ptr_+val*strides_[index],(int)NULL);
        }else{
            int n_dimensions_[R-1];
            int n_strides_[R-1];
            for (int i = 0; i < R; ++i) {
                if(i<index){
                    n_dimensions_[i]=dimensions_[i];
                    n_strides_[i]=strides_[i];
                }else if(i>index){
                    n_dimensions_[i-1]=dimensions_[i];
                    n_strides_[i-1]=strides_[i];
                }
            }
            int n_start_ptr=start_ptr_+val*strides_[index];
            int n_end_ptr=n_start_ptr+n_dimensions_[0]*n_strides_[0];
            return Tensor<T,R-1>(array_,n_dimensions_,n_strides_,n_start_ptr,n_end_ptr);
        }
    }

    Tensor<T,R-1> operator[](const int val) {
        return slicing(0,val);
    }
    void printTensor(const std::string name){
        if(array_ == nullptr){
            std::cout << "Empty Tensor Variable!!!" << std::endl;
        } else{
            std::cout << "start: " << start_ptr_ << std::endl;
            std::cout << "end: " << end_ptr_<< std::endl;
            for (int i=0; array_->size()>i;i++) {
                std::cout << "elem: " << array_->at(i)<< std::endl;
            }

        }
        std::cout << std::endl;
    }
};

//tensor con rate 0
template <class T>
class Tensor <T,0>{
private:

    std::shared_ptr<std::vector<T>> array_;
    int start_ptr_;
    int end_ptr_;

    friend class Tensor<T,1>;
    template <class T1, int ... D>

    Tensor(const std::shared_ptr<std::vector<T>> array, const int start_ptr):
    array_(array),
    start_ptr_(start_ptr){}

    //il costruttore
    Tensor(
            const std::shared_ptr<std::vector<T>>& array,
            int dimensions[],
            int strides[],
            const int start_ptr,
            const int end_ptr
    ): array_ (array),
       start_ptr_(start_ptr) {}


public:

    //rank 0
    Tensor(){
        array_ = std::make_shared<std::vector<T>>(1);
    }

    //1 element
    Tensor(const T elem):Tensor(){
        array_->at(0)=elem;
    }

    //sharing
    Tensor(const Tensor& tensor)=default;

    //move
    Tensor(Tensor&& tensor)=default;

    //destructor
    ~Tensor() = default;

    //sharing
    Tensor& operator=(const Tensor& tensor)=default;

    //move
    Tensor& operator=(Tensor&& tensor)=default;

    //assignement???
    Tensor& operator=(const T value){
        array_->at(start_ptr_)=value;
        return this;
    }

    Tensor<T> copy(){
        Tensor <T,0> n_tensor_;
        n_tensor_.set(this->get());
    }

    inline T get() const{
        //controlli
        return array_->at(start_ptr_);
    }

    inline void set(const T value){
        if(array_ == nullptr){
            throw std::out_of_range("error 1");
        }
        array_->at(start_ptr_)=value;
    }
};



#endif //PROGETTOTORSELLO_TENSOR_H
