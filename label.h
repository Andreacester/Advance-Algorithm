

#ifndef PROGETTOTORSELLO_LABEL_H
#define PROGETTOTORSELLO_LABEL_H

template <typename K>
class label;

template<typename K>
class label{
private:
label(K,n,size_t d, std::vector<size_t> p): name(n),size(d),position(p){
    assert(d > static_cast<size_t>(0));
}
public:
K name;
size_t size;

};

#endif //PROGETTOTORSELLO_LABEL_H
