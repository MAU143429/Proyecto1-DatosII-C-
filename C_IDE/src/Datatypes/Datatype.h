//
// Created by njohnson on 3/31/21.
//

#ifndef C_IDE_DATATYPE_H
#define C_IDE_DATATYPE_H
#include <string>
#endif //C_IDE_DATATYPE_H
using namespace std;

class Datatype{
private:
    string name;
    int count;
public:
    /**
     * @brief Getter for name
     * @return name in string
     */
    virtual string getName() const {
        return name;
    }

    /**
     * @brief Setter for name
     * @param the name of the variable
     */
    virtual void setName(string name) {
        Datatype::name = name;
    }

    /**
     * Getter for the count of the variable
     * @return the int variable
     */
    virtual int getCount() const {
        return count;
    }

    /**
     * @brief Setter for count
     * @param the count integer
     */
    virtual void setCount(int count) {
        Datatype::count = count;
    }
};