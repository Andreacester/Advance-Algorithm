
#ifndef AA_TENSOR_H
#define AA_TENSOR_H

#include <array>
#include <vector>

namespace tensor {
    //index type
    class index {
    public:
        index(char c) :c(c) {
        };

        bool operator==(const index &i) {
            return i.c == c;
        }
        bool operator!=(const index &i) {
            return i.c != c;
        }
        char getChar(){
            return c;
        }

    private:
        char c;

    };


    //main class of the tensor with specified rank
    template<typename T,int R>
    class tensor {
    public:

        tensor()=default;


        //constructor with array
        tensor(const size_t indexes[R]){
            for (int i = 0; i <R ; ++i) {
                indices[i]=indexes[i];
            }
            stride[R-1]=1;
            for (int i = R-1; i >0 ; --i) {
                stride[i-1]=stride[i]*indices[i];
            }
            data=std::make_shared<std::vector<T>>(stride[0]*indices[0]);

            start_ptr=&(data->operator[](0));
        }

        //constructor with parameter pack
        template<typename...Dims> tensor(Dims...dims) : indices({static_cast<const size_t>(dims)...}) {
            static_assert(sizeof...(dims)==R, "size mismatch");

            stride[R-1]=1UL;
            for (size_t i=R-1UL; i!=0UL; --i) {
                stride[i-1] = stride[i]*indices[i];
            }
            data = std::make_shared<std::vector<T>>(stride[0]*indices[0]);
            start_ptr = &(data->operator[](0));
        }

        //constructor with vector
        tensor(std::vector<T> indexes){
            for (int i = 0; i <R ; ++i) {
                indices[i]=indexes.at(i);
            }
            stride[R-1]=1;
            for (int i = R-1; i >0 ; --i) {
                stride[i-1]=stride[i]*indices[i];
            }
            data=std::make_shared<std::vector<T>>(stride[0]*indices[0]);

            start_ptr=&(data->operator[](0));
        }


        //direct access operator () and at()
        T& operator()(int ind[]){
            T* p=start_ptr;
            for (int i = 0; i < indices.size(); ++i) {
                p+= ind[i]*stride.at(i);
            }
            return *p;
        }
        template<typename...Dims>
        T& operator()(const Dims...dims){
            T ind[]={static_cast<const int>(dims)...};
            T* p=start_ptr;
            for (int i = 0; i < indices.size(); ++i) {
                p+= ind[i]*stride.at(i);
            }
            return *p;
        }

        template<typename...Dims>
        T& at(const Dims...dims){
            T ind[]={static_cast<const int>(dims)...};
            T* p=start_ptr;
            for (int i = 0; i < indices.size(); ++i) {
                assert(ind[i]<indices.at(i));
                p+= ind[i]*stride.at(i);
            }
            return *p;
        }

        T& at(int ind[]){
            T* p=start_ptr;
            for (int i = 0; i < indices.size(); ++i) {
                assert(ind[i]<indices.at(i));
                p+= ind[i]*stride.at(i);
            }
            return *p;
        }


        //auto set for testing
        void setData(){
            int c=1;
            for (auto i = data->begin(); i !=data->end(); ++i) {
                *i=c;
                c++;
            }
        }
        //print data for testing
        void printData(){
            int c=1;
            for (auto i = data->begin(); i !=data->end(); ++i) {
                std::cout<<"["<<c<<"]:"<<*i<<" ";
                c++;
            }
            std::cout<<std::endl;
        }
        //slice function
        tensor<T,R-1> slice(size_t index, size_t val){
            assert(index<R);
            tensor<T,R-1> ris;
            ris.data=data;
            for (int i = 0; i < index; ++i) {
                ris.stride[i]=stride[i];
                ris.indices[i]=indices[i];
            }
            for (int i = index; i < R-1; ++i) {
                ris.stride[i]=stride[i+1];
                ris.indices[i]=indices[i+1];
            }
            ris.start_ptr = start_ptr+val*stride[val];

            return ris;
        }


        std::vector<size_t> getIndices(){
            std::vector<size_t> ris;
            for (int i = 0; i < R; ++i) {
                ris.push_back(indices[i]);
            }
            return ris;
        }
        std::vector<size_t> getStride(){
            std::vector<size_t> ris;
            for (int i = 0; i < R; ++i) {
                ris.push_back(stride[i]);
            }
            return ris;
        }

        std::shared_ptr<std::vector<T>> getData(){
            return data;
        }
        T* getStart(){
            return start_ptr;
        }

        friend class tensor<T,R+1>;
    private:
        std::shared_ptr<std::vector<T>> data;
        std::array<size_t,R> indices;
        std::array<size_t,R> stride;
        T* start_ptr;
    };

    //tensor with no rank to simplify calculation
    template<typename T>
    class NRtensor {
    public:

        NRtensor()=default;

        template<int R>
        NRtensor(tensor<T,R> t){
            stride=t.getStride();
            indices=t.getIndices();
            start_ptr=t.getStart();
            rank=R;
            data=t.getData();
        }
        NRtensor(std::vector<size_t> indexes){
            rank=indexes.size();
            for (int i = 0; i <rank ; ++i) {
                indices.push_back(indexes.at(i));
                stride.push_back(0);
            }

            stride[rank-1]=1;
            for (int i = rank-1; i >0 ; --i) {
                stride[i-1]=stride.at(i)*indices.at(i);
            }
            data=std::make_shared<std::vector<T>>(stride[0]*indices[0]);

            start_ptr=&(data->operator[](0));
        }
        template<typename...Dims>
        T& operator()(const Dims...dims){
            T ind[]={static_cast<const int>(dims)...};
            T* p=start_ptr;
            for (int i = 0; i < indices.size(); ++i) {
                p+= ind[i]*stride.at(i);
            }
            return *p;
        }

        T& operator()(int ind[]){
            T* p=start_ptr;
            for (int i = 0; i < indices.size(); ++i) {
                p+= ind[i]*stride.at(i);
            }
            return *p;
        }

        T& at(int ind[]){
            T* p=start_ptr;
            for (int i = 0; i < indices.size(); ++i) {
                assert(ind[i]<indices.at(i));
                p+= ind[i]*stride.at(i);
            }
            return *p;
        }

        T getFromVector(std::vector<size_t> ind){
            T* p=start_ptr;
            for (int i = 0; i < indices.size(); ++i) {
                assert(ind[i]<indices.at(i));
                p+= ind[i]*stride.at(i);
            }
            return *p;
        }

        void setAt(int ind[],T value){
            T* p=start_ptr;
            for (int i = 0; i < indices.size(); ++i) {
                assert(ind[i]<indices.at(i));
                p+= ind[i]*stride.at(i);
            }
            *p=value;
        }
        void setAt(std::vector<size_t> ind,T value){
            T* p=start_ptr;
            for (int i = 0; i < indices.size(); ++i) {
                assert(ind[i]<indices.at(i));
                p+= ind[i]*stride.at(i);
            }
            *p=value;
        }


        int getRank(){
            return rank;
        }

        std::vector<size_t> getIndeces(){
            return indices;
        }

        void printData(){
            int c=1;
            for (auto i = data->begin(); i !=data->end(); ++i) {
                std::cout<<"["<<c<<"]:"<<*i<<" ";
                c++;
            }
            std::cout<<std::endl;
        }
        void printIndex(){
            for (int i = 0; i < indices.size(); ++i) {
                std::cout<< indices.at(i)<<std::endl;
            }
        }

        void plusAt(std::vector<size_t> ind,T value){
            T* p=start_ptr;
            for (int i = 0; i < indices.size(); ++i) {
                assert(ind[i]<indices.at(i));
                p+= ind[i]*stride.at(i);
            }
            *p+=value;
        }
    private:


        std::shared_ptr<std::vector<T>> data;
        int rank;
        std::vector<size_t> indices;
        std::vector<size_t> stride;
        T* start_ptr;
    };

    //class that effectively does the operation with the index type associated
    template<typename T>
    class labeled_tensor{
    public:
        template<typename...Dims, int R>
        labeled_tensor<T>(tensor<T,R> t,const Dims...dims):t(t),v({static_cast<const index>(dims)...}){
        }
        template<int R>
        labeled_tensor(tensor<T,R> t,  index id[R]):t(t){

            for (int i = 0; i < R; ++i) {
                v.push_back(id[i]);
            }
        }
        template<int R>
        labeled_tensor(tensor<T,R> t,  std::vector<index> id):t(t){

            for (int i = 0; i < R; ++i) {
                v.push_back(id.at(i));
            }
        }

        labeled_tensor(NRtensor<T> t,  std::vector<index> id):t(t){

            for (int i = 0; i < t.getRank(); ++i) {
                v.push_back(id.at(i));
            }
        }

        //trace function
        T trace(){
            auto tens=t;
            std::vector<size_t> ind_dimensions=t.getIndeces();
            assert(ind_dimensions.size()>=2);
            int n=ind_dimensions[ind_dimensions.size()-1];
            int m=ind_dimensions[ind_dimensions.size()-2];
            assert(n==m);
            T ris=0;
            if(ind_dimensions.size()==2){
                for (int j = 0; j < n; ++j) {
                    for (int k = 0; k < m; ++k) {
                        std::vector<size_t> ind;
                        ind.push_back(j);
                        ind.push_back(k);
                        ris+=j==k?t.getFromVector(ind):0;
                    }
                }
                return ris;
            }
            for (int i = ind_dimensions.size()-3; i >=0; --i) {
                std::vector<size_t> nm_index;
                int q=0;
                for (int i = 0; i <ind_dimensions.size()-2 ; ++i) {
                    nm_index.push_back(0);
                }
                for (int j = 0; j < n; ++j) {
                    for (int k = 0; k < m; ++k) {
                        std::vector<size_t> position=nm_index;
                        position.push_back(j);
                        position.push_back(k);
                        ris+=j==k?t.getFromVector(nm_index):0;
                    }
                }
                while(q!=ind_dimensions.size()-3 && nm_index[q+1]!=ind_dimensions[q+1]-1){
                    q++;
                }
                while(q>=0 && nm_index[q]==ind_dimensions[q]-1){
                    nm_index[q]=0;
                    q--;

                }
                if(nm_index[q]==ind_dimensions[q]-1){
                    nm_index[q]++;
                }

            }
            return ris;
        }

        //override operator *
        labeled_tensor<T> operator*(labeled_tensor<T> input){
            std::vector<index> common=common_index(input.getInd());
            std::vector<index> non_common= non_common_index(input.getInd(),common);
            std::vector<size_t> non_common_dims=nonCommonValue(input,non_common);
            std::vector<size_t> common_dims= commonValue(common);
            if(non_common.size()==0){
                std::vector<size_t> tmp;
                tmp.push_back(1);
                NRtensor<T> t_ris(tmp);
                t_ris(0)=input.trace();
                labeled_tensor<T> ris(t_ris,common);
                return ris;

            }
            NRtensor<T> t_ris(non_common_dims);
            std::vector<size_t> nc_index;
            for (int i = 0; i <non_common_dims.size() ; ++i) {
                nc_index.push_back(0);
            }
            int k=0;
            bool flag=true;
            while(flag){
                std::vector<size_t> c_index;
                for (int i = 0; i <common_dims.size() ; ++i) {
                    c_index.push_back(0);
                }
                int q=0;

                bool flag2=true;
                while(flag2){
                    std::vector<size_t> left_nc;
                    std::vector<size_t> right_nc;

                    int left = getRank() - common.size();
                    int right = input.getRank() - common.size();
                    for (int i = 0; i < nc_index.size(); ++i) {
                        if (i < left) left_nc.push_back(nc_index.at(i));
                        else right_nc.push_back(nc_index.at(i));
                    }
                    std::vector<size_t> left_i;

                    std::vector<index> i_left = getInd();
                    int tmp = 0;
                    int tmp2 = 0;
                    for (int i = 0; i < i_left.size(); ++i) {
                        if (chek_presence(common, i_left.at(i))) {
                            left_i.push_back(c_index[tmp2]);
                            tmp2++;
                        } else {
                            left_i.push_back(left_nc.at(tmp));
                            tmp++;
                        }
                    }

                    T left_value = t.getFromVector(left_i);

                    std::vector<size_t> right_i;
                    std::vector<index> i_right = input.getInd();
                    tmp = 0;
                    tmp2 = 0;
                    for (int i = 0; i < i_right.size(); ++i) {
                        if (chek_presence(common, i_right.at(i))) {
                            right_i.push_back(c_index[tmp2]);
                            tmp2++;
                        } else {
                            right_i.push_back(right_nc.at(tmp));
                            tmp++;
                        }
                    }

                    T right_value = input.getFromTensor(right_i);
                    t_ris.plusAt(nc_index, right_value * left_value);

                    if(isMax(c_index,common_dims)){
                        flag2=false;
                    }else{

                        while(q!=common.size()-1 && c_index[q+1]!=common_dims[q+1]-1){
                            q++;
                        }
                        while(q>=0 && c_index[q]==common_dims[q]-1){
                            c_index[q]=0;
                            q--;

                        }
                        if(c_index[q]!=common_dims[q]-1){
                            c_index[q]++;
                        }
                    }

                }


                if(isMax(nc_index,non_common_dims)){
                    flag=false;
                }else{

                    while(k!=non_common.size()-1 && nc_index[k+1]!=non_common_dims[k+1]-1){
                        k++;
                    }
                    while(k>=0 && nc_index[k]==non_common_dims[k]-1){
                        nc_index[k]=0;
                        k--;

                    }
                    if(nc_index[k]!=non_common_dims[k]-1){
                        nc_index[k]++;
                    }
                }

            }

            labeled_tensor<T> ris(t_ris,non_common);
            return ris;

        }
        //for testing
        void print(){
            t.printData();
        }

        int getRank(){
            return t.getRank();
        }
        friend class NRtensor<T>;
    private:
        std::vector<index> getInd(){
            return v;
        }

        //get the position of index ind in the vector of indices
        int getPosition(index ind){
            for (int i = 0; i < v.size(); ++i) {
                if(v.at(i)==ind) return i;
            }
            return -1;
        }

        //return the value of the index at position i of indices
        int getIndexValue(int i){
            return t.getIndeces().at(i);
        }

        std::vector<size_t> fromIndexTovalue(std::vector<index> a){
            std::vector<size_t> ris;
            for (int i = 0; i < a.size(); ++i) {
                ris.push_back(getIndexValue(getPosition(a.at(i))));
            }
            return ris;
        }

        //get value of all non_common index from the two tensor multiplied
        std::vector<size_t> nonCommonValue(labeled_tensor<T> input,std::vector<index> non_common){
            std::vector<size_t> ris;
            auto tens=this;
            for (int i = 0; i < non_common.size(); ++i) {
                if(tens->chek_presence(tens->getInd(),non_common.at(i))){
                    ris.push_back(tens->getIndexValue(getPosition(non_common.at(i))));
                }else{
                    ris.push_back(input.getIndexValue(input.getPosition(non_common.at(i))));
                }
            }
            return ris;
        }
        //get value of the common index
        std::vector<size_t> commonValue(std::vector<index> common){
            std::vector<size_t> ris;
            auto tens=this;
            for (int i = 0; i < common.size(); ++i) {
                ris.push_back(tens->getIndexValue(getPosition(common.at(i))));
            }
            return ris;
        }
        //get common index
        std::vector<index> common_index(std::vector<index> inp){
            std::vector<index> res;

            for (int i = 0; i < v.size(); ++i) {
                for (int j = 0; j < inp.size(); ++j) {
                    if(inp.at(j)==v.at(i)) {
                        if(!chek_presence(res,v.at(i))){
                            res.push_back(v.at(i));

                        }
                    }
                }
            }
            return res;
        }

        //check that the index i is in ind
        bool chek_presence(std::vector<index> ind, index i){
            for (int j = 0; j < ind.size(); ++j) {
                if(ind.at(j)==i) return true;
            }
            return false;
        }

        //get the uncommon index
        std::vector<index> non_common_index(std::vector<index> inp, std::vector<index> common){
            std::vector<index> res;
            for (int i = 0; i < v.size(); ++i) {
                bool flag=true;
                int count=0;
                while(flag && count<common.size()){
                    if(v.at(i)==common.at(count)){
                        flag=false;
                    }
                    count++;
                }
                if(flag) res.push_back(v.at(i));
            }
            for (int i = 0; i < inp.size(); ++i) {
                bool flag=true;
                int count=0;
                while(flag && count<common.size()){
                    if(inp.at(i)==common.at(count)){
                        flag=false;
                    }
                    count++;
                }
                if(flag) res.push_back(inp.at(i));
            }

            return res;
        }



        T getFromTensor(std::vector<size_t> input){
            return t.getFromVector(input);
        }

        //check that the vector with the index is maxed
        bool isMax(std::vector<size_t> a,std::vector<size_t> b){
            for (int i = 0; i < a.size(); ++i) {
                if(a[i]!=(b[i]-1)) return false;
            }
            return true;
        }

        NRtensor<T> t;
        std::vector<index> v;
    };

}

#endif //AA_TENSOR_H
