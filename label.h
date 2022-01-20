#include <vector>

#ifndef PROGETTOTORSELLO_LABEL_H
#define PROGETTOTORSELLO_LABEL_H

#include <cstdlib>
#include <cassert>
#include <limits>

/**
 * fixed dimension of tensor.
 * @tparam L type of the label
 */

template<typename L>
class label {

private:

    /**
     * Private constructor.
     * @param n label's name
     * @param d associated dimension size
     * @param p occurrences vector of the label
     */
    label(L n, size_t d, std::vector<size_t> p) : name(n), size(d), positions(p) {
        assert(d > static_cast<size_t>(0));
    }

public:

    L name;
    size_t size;
    std::vector<size_t> positions;

    /**
     * Public constructor.
     * @param n label's name
     * @param d dimension size
     * @param p tensor's dimension of the label
     */
    label(L n, size_t d, size_t p) : name(n), size(d) {
        positions.push_back(p);
        assert(d > static_cast<size_t>(0));
    }

    bool operator<(const label<L> &other) {
        return name < other.name;
    }

    bool operator==(const label<L> &other) {
        return name == other.name && size == other.size;
    }

    bool operator!=(const label<L> &other) {
        return name != other.name || size != other.size;
    }

    /**
     * Look for the label in a labels list
     * @param labels vector of labels of the same type
     * @return first occurrence position in the list if any or the maximum size_t
     */
    size_t find_label(std::vector<label<L>> *labels) {
        size_t position = static_cast<size_t>(0);
        for (label<L> &temp : *labels) {
            if (temp == *this) {
                return position;
            }
            ++position;
        }
        return std::numeric_limits<size_t>::max();
    }

    /**
     * Static function: gets all the labels' sizes from the labels list
     * @param labels labels vector
     * @return size_t vector of label sizes
     */
    static std::vector<size_t> get_labels_sizes(std::vector<label<L>> *labels) {
        std::vector<size_t> dimensions;
        for (const label<L> l : *labels) {
            dimensions.push_back(l.size);
        }
        return dimensions;
    }
};


#endif //PROGETTOTORSELLO_LABEL_H
