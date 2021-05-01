//
// Created by mau14 on 2/4/21.
//

#ifndef C_IDE_TRANSLATE_CODE_H
#define C_IDE_TRANSLATE_CODE_H
#include <utility>
#include "src/DataStructure/SimplyLinkedList.h"
#include "src/TypeConversion/TypeMessage.h"
#include "src/TypeConversion/ObjectToJSON.h"
#include "gui_c.h"
#include "iostream"
#include "sstream"
#include "src/Socket/Client.h"

using namespace std;

static const auto INTEGER_KEY = "Integer";
static const auto FLOAT_KEY= "Float";
static const auto DOUBLE_KEY = "Double";
static const auto CHAR_KEY = "Char";
static const auto LONG_KEY= "Long";
static const auto REFERENCE_KEY = "Reference";
static const auto STRUCT_KEY = "Struct";
static const auto INTEGER_SIZE = "4";
static const auto FLOAT_SIZE= "4";
static const auto DOUBLE_SIZE = "8";
static const auto CHAR_SIZE = "1";
static const auto LONG_SIZE= "8";

static const auto EQUAL_OPERATOR = "=";
static const auto SUM_OPERATOR = "+";
static const auto SUBTRAC_OPERATOR = "-";
static const auto DIV_OPERATOR = "/";
static const auto MULTI_OPERATOR = "*";

static SimplyLinkedList<string> *Type_list;
static SimplyLinkedList<string> *Size_list;
static SimplyLinkedList<string> *Operator_list;
static SimplyLinkedList<string> *Operator_vlist;

class Translate_Code {
public:
    /**
     * @brief static method that reads a string containing the line from the IDE
     * @param line
     * @return the simply linked list of strings
     */
    static SimplyLinkedList<string> Readline(string line) {
        auto *output = new SimplyLinkedList<string>();
        int cont = 0;
        char c;
        string note;
        std::string s;

        while (cont < line.length()) {
            s.clear();
            c = line[cont];
            s.push_back(c);
            if (isblank(c)) {
                if(!note.empty()){
                    output->append(note);
                    note.clear();
                }
            } else if (c == ';' or Operator_vlist->boolSearch(s)){
                if(!note.empty()){
                    output->append(note);
                    note.clear();
                    note.push_back(c);
                    output->append(note);
                    note.clear();
                }else{
                    note.push_back(c);
                    output->append(note);
                    note.clear();
                }
            }else{
                note.push_back(c);
            }
            cont++;
        }
        output->show();
        return *output;
    }
    /**
     * @brief static method that decodifys the lines contained in the simply linked list of strings
     * @param stringlist
     * @return the method NewMessageToJSON with the message object created
     */
    static string Decodify_line(SimplyLinkedList<string> stringlist) {
        auto *message = new TypeMessage();
        // Verifica que la linea ingresada contenga un ;
        int ultpos = (stringlist.getLen()-1);
        if (stringlist.get(ultpos) != ";"){
            //std::cout << "\n FATAL ERROR " << ";" << " WASN'T DETECTED\n";
            //TODO INGRESAR ERROR 200
        }
        //Decodifica la linea de codigo
        if (Type_list->boolSearch(stringlist.get(0))) {
            // Detecta que se quiere instancear una variable nueva
            message->setType(stringlist.get(0));
            message->setSize(getSize(stringlist.get(0)));
            message->setAction("CREATE");
            // Verifica que la variable no haya sido creada antes
            if (!Type_list->boolSearch(stringlist.get(1))) {
                    message->setName(stringlist.get(1));
                    // Verifica que la lista lleve un operador =
                if (Operator_vlist->boolSearch(stringlist.get(2))) {

                    // Verifica que el valor de la variable a ingresar tenga sentido
                    if (!Type_list->boolSearch(stringlist.get(3))) {

                        //Verifica que el valor ha ingresar coincida con el tipo de valor y no sean erroneos
                        if(Verify_Type(stringlist.get(0),stringlist.get(3),Operator_list)){
                            if(stringlist.get(0) == "Char"){
                                string char1;
                                char1 = stringlist.get(3)[1];
                                message->setValue(char1);
                            }else{
                                message->setValue(stringlist.get(3));
                            }

                        }else{
                            //std::cout << "\n ERROR: INGRESE UN VALOR ADECUADO AL TIPO DE DATO QUE DESEA CREAR\n";
                            //TODO INGRESAR EL ERROR 201 y PARAR LA EJECUCION
                        }
                    } else{
                        //std::cout << "\n ERROR: EL NOMBRE DEL VALOR QUE DESEA CREAR NO ES VALIDO\n";
                        //TODO INGRESAR EL ERROR 202 y PARAR LA EJECUCION
                    }
                } else {
                    // Verifica que solo se esta declarando y asigna un valor de 0 la variable
                    if(stringlist.get(2) == ";"){
                        if(stringlist.get(0) != "Char"){
                            message->setValue("0");
                        }else{
                            message->setValue(" ");
                        }
                    }else{
                        //cout << "\nERROR CON EL OPERADOR A UTILIZAR\n";
                        //TODO INGRESAR EL ERROR 203 y PARAR LA EJECUCION
                    }
                }
            } else {
                //cout<< "\nFATAL ERROR: EL NOMBRE DE LA VARIABLE QUE INGRESO YA SE ENCUENTRA CREADO O NO ES VALIDO\n";
                //TODO INGRESAR EL ERROR 204 y PARAR LA EJECUCION
            }
            return ObjectToJSON::NewMessageToJSON(message);
        }
        // Metodo para cuando no se quiere crear una instancia
        else  {
            auto *modify_message = new TypeMessage();
            auto *print_message = new TypeMessage();
            if(Operator_vlist->boolSearch(stringlist.get(1))){
                modify_message->setAction("MODIFY");
                modify_message->setModifyvalue(stringlist.get(2));
                modify_message->setName(stringlist.get(0));
                return ObjectToJSON::NewMessageToJSON(modify_message);
            }
            if(stringlist.get(0) == "print"){
                print_message->setAction("SEARCH");
                print_message->setName(stringlist.get(1));
                return ObjectToJSON::NewMessageToJSON(print_message);
            }
        }
    }
    /**
     * @brief
     * @param key
     * @return
     */
    bool isStruct(string key) {

        return false;
    }
    /**
     * @brief Method that gets the node searched in the list
     * @param positionsize
     * @return the node
     */
    static string getSize(string positionsize){
        for (int i = 0; i < Type_list->getLen() ; ++i) {
            if(Type_list->get(i) == positionsize){
                return Size_list->get(i);
            }
        }
    }
    /**
     * @brief boolean method that searches for a dot in the string received
     * @param txt
     * @return a boolean
     */
    static bool Point_search(string txt) {
        int counter = 0;
        char character;

        while (counter < txt.length()) {
            character = txt[counter];
            if (character == '.') {
                return true;
            } else {
                character = ' ';
            }
            counter++;
        }
        return false;
    }

    /**
     * @brief method that verifies the type of the variable that gets verifies
     * @param type the type of the variable
     * @param value the value of the variable
     * @param Operator the simply linked list of the operators
     * @return true if the string contained in the type variable matches with the type contained in value, false if not
     */
    static bool Verify_Type(string type, string value, SimplyLinkedList<string> *Operator) {
        std::stringstream ss;

        if (Operator_Verify(value)) {
            cout << "\nENVIANDO DATO AL SERVIDOR PARA QUE SEA ANALIZADO\n" << endl;
            return true;
        } else {
            //int and long method
            if (type == "Integer" or type == "Long") {
                long typedata;
                ss << value;
                ss >> typedata;
                if (typedata == 0 or Point_search(value)) {
                    cout << "\nEL TIPO DE DATO INGRESADO NO ES UN\n" << type << endl;
                } else if (typedata >= -2147483648 and typedata <= 2147483647) {
                    if (type == "Integer") {
                        return true;
                    } else if(type == "Long") {
                        return true;
                    }else{
                        return false;
                    }
                } else if (typedata >= INTMAX_MIN and typedata <= INTMAX_MAX) {
                    if (type == "Long") {
                        return true;
                    } else {
                        cout << "\nDATA OUT OF RANGE\n" << endl;
                        return false;
                    }
                } else {
                    cout << "\nTIPO DE DATO NO ENCONTRADO\n" << endl;
                }
            }
            //float an double method
            if (type == "Float" or type == "Double") {
                double typedata1;
                ss << value;
                ss >> typedata1;

                if (typedata1 == 0 or !Point_search(value)) {
                    cout << "\nEL TIPO DE DATO INGRESADO NO ES UN\n" << type << endl;

                } else if (typedata1 >= 1.17549e-038 and typedata1 <= 3.40282e+038) {
                    if (type == "Float") {
                        return true;
                    } else if(type == "Long") {
                        return true;
                    }else{
                        return false;
                    }
                } else if (typedata1 >= 2.22507e-308 and typedata1 <= 1.79769e+308) {
                    if (type == "Double") {
                        return true;
                    } else {
                        cout << "\nDATA OUT OF RANGE\n" << endl;
                        return false;
                    }
                } else {
                    cout << "\nTIPO DE DATO NO ENCONTRADO\n" << endl;
                }
            }
            //char method
            if (type == "Char") {
                string char1,char2;
                char1 = value[0];
                char2 = value[2];

                if(char1 == "'" and char2 == "'"){
                    return true;
                } else{
                    return false;
                }
            }
        }
    }
    /**
     * @brief a boolean method that verifies if an operator is actually contained in the operator list
     * @param value
     * @return true if the operator is in the list of false if its not
     */
    static bool Operator_Verify(string value){
        std::stringstream mm;
        int counter1 = 0;
        char character1;
        string output;

        while (counter1 < value.length()) {
            character1 = value[counter1];
            mm << character1;
            mm >> output;
            if (Operator_list->boolSearch(output)){
                return true;
            } else {
                output.clear();
                mm.clear();
            }
            counter1++;
        }
        return false;
    }

public:
    /**
     * @brief method that initializes all the lists of the compiler and appends the elements necessary in the different lists
     * @param line
     * @return the Decodify_line method with the processed line linked list
     */
    string static compile(string line) {
        Type_list = new SimplyLinkedList<string>();
        Operator_list = new SimplyLinkedList<string>();
        Operator_vlist = new SimplyLinkedList<string>();
        Size_list = new SimplyLinkedList<string>();
        Type_list->append(INTEGER_KEY);
        Type_list->append(FLOAT_KEY);
        Type_list->append(DOUBLE_KEY);
        Type_list->append(CHAR_KEY);
        Type_list->append(LONG_KEY);
        Type_list->append(REFERENCE_KEY);
        Type_list->append(STRUCT_KEY);
        Size_list->append(INTEGER_SIZE);
        Size_list->append(FLOAT_SIZE);
        Size_list->append(DOUBLE_SIZE);
        Size_list->append(CHAR_SIZE);
        Size_list->append(LONG_SIZE);
        Operator_vlist->append(EQUAL_OPERATOR);
        Operator_list->append(SUM_OPERATOR);
        Operator_list->append(SUBTRAC_OPERATOR);
        Operator_list->append(DIV_OPERATOR);
        Operator_list->append(MULTI_OPERATOR);
        SimplyLinkedList<string> processedLine = Readline(std::move(line));
        return Decodify_line(processedLine);
    }
};

#endif //C_IDE_TRANSLATE_CODE_H


