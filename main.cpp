#include <iostream>
#include <array>
#include <vector>
#include "tensor.h"


void test1(){
    tensor::tensor<int,2> t(2,3);
    tensor::tensor<int,2> t2(3,2);
    t.setData();
    t2.setData();
    tensor::index n('n');
    tensor::index m('m');
    tensor::index o('o');

    tensor::labeled_tensor<int> lt(t,n,m);
    tensor::labeled_tensor<int> lt2(t2,m,o);

    tensor::labeled_tensor<int> ris=lt*lt2;
    std::cout<<"Product of two tensor with a common index"<<std::endl;
    std::cout<<"t:";
    t.printData();
    std::cout<<"t2:";
    t2.printData();
    std::cout<<"t*t2:";
    ris.print();

}
void test2(){
    tensor::tensor<int,2> t(2,3);
    tensor::tensor<int,2> t2(3,2);
    tensor::tensor<int,3> t3(2,3,2);
    t.setData();
    t2.setData();
    t3.setData();
    tensor::index n('n');
    tensor::index m('m');
    tensor::index o('o');
    tensor::index a('a');

    tensor::labeled_tensor<int> lt(t,m,n);
    tensor::labeled_tensor<int> lt2(t2,n,o);
    tensor::labeled_tensor<int> lt3(t3,n,a,o);


    tensor::labeled_tensor<int> ris=lt*lt2*lt3;
    std::cout<<"Product of three tensor with a common index"<<std::endl;
    std::cout<<"t:";
    t.printData();
    std::cout<<"t2:";
    t2.printData();
    std::cout<<"t3:";
    t3.printData();
    std::cout<<"t*t2*t3:";
    ris.print();
}
void test3(){

    tensor::tensor<int,2> t2(3,2);
    tensor::tensor<int,3> t3(2,3,2);
    t2.setData();
    t3.setData();
    tensor::index n('n');
    tensor::index o('o');
    tensor::index a('a');
    tensor::labeled_tensor<int> lt2(t2,n,o);
    tensor::labeled_tensor<int> lt3(t3,a,n,o);

    tensor::labeled_tensor<int> ris=lt2*lt3;
    std::cout<<"Product of two tensor with two common index"<<std::endl;
    std::cout<<"t2:";
    t2.printData();
    std::cout<<"t3:";
    t3.printData();
    std::cout<<"t*t2*t3:";
    ris.print();
}

void test4(){
    tensor::tensor<int,2> t_dir(2,2);
    t_dir.setData();
    tensor::index n('n');
    tensor::labeled_tensor<int> ltrace(t_dir,n,n);
    int val=ltrace.trace();
    std::cout<<"Trace of a tensor"<<std::endl;
    std::cout<<"t:";
    t_dir.printData();
    std::cout<<"Tr(t): "<<val<<std::endl;
}

int main() {
    std::cout<<"---Test1---"<<std::endl;
    test1();
    std::cout<<"---Test2---"<<std::endl;
    test2();
    std::cout<<"---Test3---"<<std::endl;
    test3();
    std::cout<<"---Test4---"<<std::endl;
    test4();
    return 0;
}
