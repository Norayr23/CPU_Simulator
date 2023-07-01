#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <map>
#include <cctype>




class Register {
   public:
     Register(std::string name = "", unsigned char address = 0) : address(address), name(name){}
     std::string name;
     unsigned char address;
     char value = 0;
};
class BinarInstruction {
    private:
      char (*operation)(char&, const char&);
    public:
      BinarInstruction(char (*operation) (char&, const char&), std::string name, unsigned char code) : code(code), name(name), operation(operation) {  }
      char getOperRes(char& a, const char& b) {
         return operation(a, b);
      }      
      std::string name;
      unsigned char code;
      
};
class ConditionalInstruction : public BinarInstruction {
   private:
     char (*operation)(char, char);
   public:
    ConditionalInstruction(char (*operation)(char, char), std::string name, unsigned char code) : operation(operation), BinarInstruction(nullptr, name, code) {  }
    char getOperRes(char a, char b) {
       return operation(a, b);
    }
};
class UnarInstruction {
   private:
      bool (*operation)(char);
   public:
      UnarInstruction(bool (*operation)(char), std::string name, unsigned char code) :name(name), code(code), operation(operation){  }
      std::string name;
      unsigned char code; 
      bool getOperRes(char input) {
         return operation(input);
      }

  };
class LogicInstruction : public UnarInstruction {
   private:
      char (*operation)(char&);
   public:
   LogicInstruction(char (*operation)(char&), std::string name, unsigned char code) : UnarInstruction(nullptr, name, code), operation(operation){  }
   char getOperRes(char& input) {
        return operation(input);
      }

};
class DataTransferInstruction : public BinarInstruction {
   public:
      DataTransferInstruction(std::string name, unsigned char code) : BinarInstruction(nullptr, name, code){  }
      void getOperRes(char& a, const char& b) {
         a = b;
      }
};
class CPU {
    public:
      CPU() {
         
       registers.emplace("AYB", Register("AYB", 128));
       registers.emplace("BEN", Register("BEN", 129));
       registers.emplace("GIM", Register("GIM", 130));
       registers.emplace("DA", Register("DA", 131));
       registers.emplace("ECH", Register("ECH", 132));
       registers.emplace("ZA", Register("ZA", 133));
     }

      std::map<std::string, Register> registers; 
      std::map<std::string, int> labels;
      BinarInstruction add = BinarInstruction([](char& a, const char& b) { return a += b; }, "ADD", 1);
      BinarInstruction sub = BinarInstruction([](char& a, const char& b) { return a -= b; }, "SUB", 2);
      BinarInstruction mul = BinarInstruction([](char& a, const char& b) { return a *= b; }, "MUL", 3);
      BinarInstruction div = BinarInstruction([](char& a, const char& b) { return a /= b; }, "DIV", 4);
      BinarInstruction _or = BinarInstruction([](char& a, const char& b) { return a |= b; }, "OR", 5);
      BinarInstruction _and = BinarInstruction([](char& a, const char& b) { return a &= b; }, "AND", 6);
      ConditionalInstruction cmp = ConditionalInstruction([](char a, char b) {return  a > b ? (char) 1 : a == b ? (char) 0 : (char) -1;}, "CMP", 7);
      UnarInstruction jmp = UnarInstruction(nullptr, "JMP", 8);
      UnarInstruction jg = UnarInstruction([](char cmpRes) {return cmpRes ==  1;}, "JG", 9);
      UnarInstruction jl = UnarInstruction([](char cmpRes) {return cmpRes ==  -1;}, "JL", 10);
      UnarInstruction je = UnarInstruction([](char cmpRes) {return cmpRes ==  0;}, "JE", 11);
      DataTransferInstruction mv = DataTransferInstruction("MV", 12);
      LogicInstruction _not = LogicInstruction([](char& a) { return a = ~a; }, "NOT", 13);
      static const int memorySize = 60;
      char memory[memorySize] = {0};
      bool isRegister(const std::string& str) {
         return str == "AYB" || str == "BEN" ||  str == "GIM" || str == "DA" ||  str == "ECH" || str == "ZA";

      }
      bool isMemoryAddress(std::string str, int avMemory, int& addDest) {
         if (str[0] != '[' || str.back() != ']') {
            return false;
         }
         std::string tmp;
         for (int i = 1; str[i] != ']'; ++i) {
            tmp.push_back(str[i]);
         }
         if (!getNumber(tmp, addDest) || addDest > memorySize - avMemory) {
            return false;
         }
         addDest += avMemory;
         return true;
      }
      void dumpMemory(int avMemory) {
         std::cout << "\nINSTRUCTIONS\n" << std::endl;
         int line = 0;
         for (int i = 0; i < memorySize; ++i ) {
            if (i == avMemory) { 
               std::cout << "\nDATA\n" << std::endl;
               line = 0;
            }
            std::cout << line++ << ": " << (int)memory[i] << std::endl;
         }
      }
      bool getNumber (const std::string& str, int& dest){
		   dest = 0;				
		   int d = 1;
		   for(int i = str.length() - 1; i >= 0; i --){
		       if (str[i] < '0' || str[i] > '9'){
		          return false;
		       }								
		       dest += d * (str[i] - '0');
		       d *= 10;
		   }								
		   return true;
      }   
      bool tokenize(const std::string& file_name, std::vector<std::vector<std::string>>& tokens, int& lineCount) {
             std::ifstream file(file_name);
             if (file.is_open()) {
                std::string line;
                 lineCount = 0;
                while (std::getline(file, line)) {
                   if (lineCount > 9) {
                      std::cout << "Not enough memory available." << std::endl;
                      return false;
                   }
                   
                  ++lineCount;
                  std::vector<std::string> subarray;
                  std::istringstream iss(line);
                  std::string word;
                  int wordCount = 0;
            
                  while (iss >> word) {
                     if (wordCount > 2 ) {
                        std::cout << "Invalid input " << word << " in line " << lineCount << std::endl;
                        return false;       
                     }
                     for (char& ch : word) {
                        ch = std::toupper(ch);
                     }
                     subarray.push_back(word);
                     wordCount++;
                     if (word == "DUMP_MEMORY()") {
                        lineCount -= 1;
                     }
                     else if (word.back() == ':') {
                        word = word.substr(0, word.size() - 1);
                        labels[word] = lineCount + 1;
                        --lineCount;
                     }
                  }
                  if (wordCount == 0) {
                     --lineCount;
                  }
                  tokens.push_back(subarray);
               }

              file.close();
           }

           return true;
       }
      bool storeMemory(const std::vector<std::vector<std::string>>& tokens, int linesCount) {
         int avMemory = 3 * linesCount;
         int memIndex = 0;
         int num = 0;
         int num2 = 0;
         for (int i = 0; i < tokens.size(); ++i) {
            for (int j = 0; j < tokens[i].size(); j = tokens[i].size()) {
               std::string token = tokens[i][0];
               if (token == "DUMP_MEMORY()" && tokens[i].size() == 1){
                  dumpMemory(avMemory);
               }

               else if (token == "MOV") {
                  int number;
                  if (isRegister(tokens[i][j + 1]) && getNumber(tokens[i][j + 2], number)) {
                     mv.getOperRes(registers[tokens[i][j + 1]].value, number);
                     memory[memIndex] = mv.code;
                     memory[memIndex + 1] = registers[token].address;
                     memory[memIndex + 2] = number;
                     memIndex += 3;

                  }
                  else if (isRegister(tokens[i][j + 1]) && isRegister(tokens[i][j + 2])) {
                     mv.getOperRes(registers[tokens[i][j + 1]].value, registers[tokens[i][j + 2]].value);
                     memory[memIndex] = mv.code;
                     memory[memIndex + 1] = registers[tokens[i][j + 1]].address;
                     memory[memIndex + 2] = registers[tokens[i][j + 2]].address;
                     memIndex += 3;
                  }
                   else if (isRegister(tokens[i][j + 1]) && isMemoryAddress(tokens[i][j + 2], avMemory, num)) {
                     mv.getOperRes(registers[tokens[i][j + 1]].value, memory[num]);
                     memory[memIndex] = mv.code;
                     memory[memIndex + 1] = registers[tokens[i][j + 1]].address;
                     memory[memIndex + 2] = num;
                     memIndex += 3;
                  }
                   else if (isMemoryAddress(tokens[i][j + 1], avMemory, num) && isRegister(tokens[i][j + 2])) {
                     mv.getOperRes(memory[num], registers[tokens[i][j + 2]].value);
                     memory[memIndex] = mv.code;
                     memory[memIndex + 1] = num;
                     memory[memIndex + 2] = registers[tokens[i][j + 2]].address;
                     memIndex += 3;
                  }
                  else if (isMemoryAddress(tokens[i][j + 1], avMemory, num) && getNumber(tokens[i][j + 2], num2)) {
                     mv.getOperRes(memory[num], num2);
                     memory[memIndex] = mv.code;
                     memory[memIndex + 1] = num;
                     memory[memIndex + 2] = num2;
                     memIndex += 3;
                  }
                  
               }
                else if (token == "ADD") {
                  int number;
                  if (isRegister(tokens[i][j + 1]) && getNumber(tokens[i][j + 2], number)) {
                     add.getOperRes(registers[tokens[i][j + 1]].value, number);
                     memory[memIndex] = add.code;
                     memory[memIndex + 1] = registers[token].address;
                     memory[memIndex + 2] = number;
                     memIndex += 3;

                  }
                  else if (isRegister(tokens[i][j + 1]) && isRegister(tokens[i][j + 2])) {
                     add.getOperRes(registers[tokens[i][j + 1]].value, registers[tokens[i][j + 2]].value);
                     memory[memIndex] = add.code;
                     memory[memIndex + 1] = registers[tokens[i][j + 1]].address;
                     memory[memIndex + 2] = registers[tokens[i][j + 2]].address;
                     memIndex += 3;
                  }
                   else if (isRegister(tokens[i][j + 1]) && isMemoryAddress(tokens[i][j + 2], avMemory, num)) {
                     add.getOperRes(registers[tokens[i][j + 1]].value, memory[num]);
                     memory[memIndex] = add.code;
                     memory[memIndex + 1] = registers[tokens[i][j + 1]].address;
                     memory[memIndex + 2] = num;
                     memIndex += 3;
                  }
                   else if (isMemoryAddress(tokens[i][j + 1], avMemory, num) && isRegister(tokens[i][j + 2])) {
                     add.getOperRes(memory[num], registers[tokens[i][j + 2]].value);
                     memory[memIndex] = add.code;
                     memory[memIndex + 1] = num;
                     memory[memIndex + 2] = registers[tokens[i][j + 2]].address;
                     memIndex += 3;
                  }
                  else if (isMemoryAddress(tokens[i][j + 1], avMemory, num) && getNumber(tokens[i][j + 2], num2)) {
                     std:: cout << (int)add.getOperRes(memory[num], num2) << std::endl;
                     memory[memIndex] = add.code;
                     memory[memIndex + 1] = num;
                     memory[memIndex + 2] = num2;
                     memIndex += 3;
                  }
                  
               }
                else if (token == "SUB") {
                  int number;
                  if (isRegister(tokens[i][j + 1]) && getNumber(tokens[i][j + 2], number)) {
                     sub.getOperRes(registers[tokens[i][j + 1]].value, number);
                     memory[memIndex] = sub.code;
                     memory[memIndex + 1] = registers[token].address;
                     memory[memIndex + 2] = number;
                     memIndex += 3;

                  }
                  else if (isRegister(tokens[i][j + 1]) && isRegister(tokens[i][j + 2])) {
                     sub.getOperRes(registers[tokens[i][j + 1]].value, registers[tokens[i][j + 2]].value);
                     memory[memIndex] = sub.code;
                     memory[memIndex + 1] = registers[tokens[i][j + 1]].address;
                     memory[memIndex + 2] = registers[tokens[i][j + 2]].address;
                     memIndex += 3;
                  }
                   else if (isRegister(tokens[i][j + 1]) && isMemoryAddress(tokens[i][j + 2], avMemory, num)) {
                     sub.getOperRes(registers[tokens[i][j + 1]].value, memory[num]);
                     memory[memIndex] = sub.code;
                     memory[memIndex + 1] = registers[tokens[i][j + 1]].address;
                     memory[memIndex + 2] = num;
                     memIndex += 3;
                  }
                   else if (isMemoryAddress(tokens[i][j + 1], avMemory, num) && isRegister(tokens[i][j + 2])) {
                     sub.getOperRes(memory[num], registers[tokens[i][j + 2]].value);
                     memory[memIndex] = sub.code;
                     memory[memIndex + 1] = num;
                     memory[memIndex + 2] = registers[tokens[i][j + 2]].address;
                     memIndex += 3;
                  }
                  else if (isMemoryAddress(tokens[i][j + 1], avMemory, num) && getNumber(tokens[i][j + 2], num2)) {
                     sub.getOperRes(memory[num], num2);
                     memory[memIndex] = sub.code;
                     memory[memIndex + 1] = num;
                     memory[memIndex + 2] = num2;
                     memIndex += 3;
                  }
                  
               }
                else if (token == "DIV") {
                  int number;
                  if (isRegister(tokens[i][j + 1]) && getNumber(tokens[i][j + 2], number)) {
                     div.getOperRes(registers[tokens[i][j + 1]].value, number);
                     memory[memIndex] = div.code;
                     memory[memIndex + 1] = registers[token].address;
                     memory[memIndex + 2] = number;
                     memIndex += 3;

                  }
                  else if (isRegister(tokens[i][j + 1]) && isRegister(tokens[i][j + 2])) {
                     div.getOperRes(registers[tokens[i][j + 1]].value, registers[tokens[i][j + 2]].value);
                     memory[memIndex] = div.code;
                     memory[memIndex + 1] = registers[tokens[i][j + 1]].address;
                     memory[memIndex + 2] = registers[tokens[i][j + 2]].address;
                     memIndex += 3;
                  }
                   else if (isRegister(tokens[i][j + 1]) && isMemoryAddress(tokens[i][j + 2], avMemory, num)) {
                     div.getOperRes(registers[tokens[i][j + 1]].value, memory[num]);
                     memory[memIndex] = div.code;
                     memory[memIndex + 1] = registers[tokens[i][j + 1]].address;
                     memory[memIndex + 2] = num;
                     memIndex += 3;
                  }
                   else if (isMemoryAddress(tokens[i][j + 1], avMemory, num) && isRegister(tokens[i][j + 2])) {
                     div.getOperRes(memory[num], registers[tokens[i][j + 2]].value);
                     memory[memIndex] = div.code;
                     memory[memIndex + 1] = num;
                     memory[memIndex + 2] = registers[tokens[i][j + 2]].address;
                     memIndex += 3;
                  }
                  else if (isMemoryAddress(tokens[i][j + 1], avMemory, num) && getNumber(tokens[i][j + 2], num2)) {
                     div.getOperRes(memory[num], num2);
                     memory[memIndex] = div.code;
                     memory[memIndex + 1] = num;
                     memory[memIndex + 2] = num2;
                     memIndex += 3;
                  }
                  
               }
                else if (token == "MUL") {
                  int number;
                  if (isRegister(tokens[i][j + 1]) && getNumber(tokens[i][j + 2], number)) {
                     mul.getOperRes(registers[tokens[i][j + 1]].value, number);
                     memory[memIndex] = mul.code;
                     memory[memIndex + 1] = registers[token].address;
                     memory[memIndex + 2] = number;
                     memIndex += 3;

                  }
                  else if (isRegister(tokens[i][j + 1]) && isRegister(tokens[i][j + 2])) {
                     mul.getOperRes(registers[tokens[i][j + 1]].value, registers[tokens[i][j + 2]].value);
                     memory[memIndex] = mul.code;
                     memory[memIndex + 1] = registers[tokens[i][j + 1]].address;
                     memory[memIndex + 2] = registers[tokens[i][j + 2]].address;
                     memIndex += 3;
                  }
                   else if (isRegister(tokens[i][j + 1]) && isMemoryAddress(tokens[i][j + 2], avMemory, num)) {
                     mul.getOperRes(registers[tokens[i][j + 1]].value, memory[num]);
                     memory[memIndex] = mul.code;
                     memory[memIndex + 1] = registers[tokens[i][j + 1]].address;
                     memory[memIndex + 2] = num;
                     memIndex += 3;
                  }
                   else if (isMemoryAddress(tokens[i][j + 1], avMemory, num) && isRegister(tokens[i][j + 2])) {
                     mul.getOperRes(memory[num], registers[tokens[i][j + 2]].value);
                     memory[memIndex] = mul.code;
                     memory[memIndex + 1] = num;
                     memory[memIndex + 2] = registers[tokens[i][j + 2]].address;
                     memIndex += 3;
                  }
                  else if (isMemoryAddress(tokens[i][j + 1], avMemory, num) && getNumber(tokens[i][j + 2], num2)) {
                     mul.getOperRes(memory[num], num2);
                     memory[memIndex] = mul.code;
                     memory[memIndex + 1] = num;
                     memory[memIndex + 2] = num2;
                     memIndex += 3;
                  }
                  
               }
                else if (token == "AND") {
                  int number;
                  if (isRegister(tokens[i][j + 1]) && getNumber(tokens[i][j + 2], number)) {
                     _and.getOperRes(registers[tokens[i][j + 1]].value, number);
                     memory[memIndex] = _and.code;
                     memory[memIndex + 1] = registers[token].address;
                     memory[memIndex + 2] = number;
                     memIndex += 3;

                  }
                  else if (isRegister(tokens[i][j + 1]) && isRegister(tokens[i][j + 2])) {
                     _and.getOperRes(registers[tokens[i][j + 1]].value, registers[tokens[i][j + 2]].value);
                     memory[memIndex] = _and.code;
                     memory[memIndex + 1] = registers[tokens[i][j + 1]].address;
                     memory[memIndex + 2] = registers[tokens[i][j + 2]].address;
                     memIndex += 3;
                  }
                   else if (isRegister(tokens[i][j + 1]) && isMemoryAddress(tokens[i][j + 2], avMemory, num)) {
                     _and.getOperRes(registers[tokens[i][j + 1]].value, memory[num]);
                     memory[memIndex] = _and.code;
                     memory[memIndex + 1] = registers[tokens[i][j + 1]].address;
                     memory[memIndex + 2] = num;
                     memIndex += 3;
                  }
                   else if (isMemoryAddress(tokens[i][j + 1], avMemory, num) && isRegister(tokens[i][j + 2])) {
                     _and.getOperRes(memory[num], registers[tokens[i][j + 2]].value);
                     memory[memIndex] = _and.code;
                     memory[memIndex + 1] = num;
                     memory[memIndex + 2] = registers[tokens[i][j + 2]].address;
                     memIndex += 3;
                  }
                  else if (isMemoryAddress(tokens[i][j + 1], avMemory, num) && getNumber(tokens[i][j + 2], num2)) {
                     _and.getOperRes(memory[num], num2);
                     memory[memIndex] = _and.code;
                     memory[memIndex + 1] = num;
                     memory[memIndex + 2] = num2;
                     memIndex += 3;
                  }
                  
               }
                else if (token == "OR") {
                  int number;
                  if (isRegister(tokens[i][j + 1]) && getNumber(tokens[i][j + 2], number)) {
                     _or.getOperRes(registers[tokens[i][j + 1]].value, number);
                     memory[memIndex] = _or.code;
                     memory[memIndex + 1] = registers[token].address;
                     memory[memIndex + 2] = number;
                     memIndex += 3;

                  }
                  else if (isRegister(tokens[i][j + 1]) && isRegister(tokens[i][j + 2])) {
                     _or.getOperRes(registers[tokens[i][j + 1]].value, registers[tokens[i][j + 2]].value);
                     memory[memIndex] = _or.code;
                     memory[memIndex + 1] = registers[tokens[i][j + 1]].address;
                     memory[memIndex + 2] = registers[tokens[i][j + 2]].address;
                     memIndex += 3;
                  }
                   else if (isRegister(tokens[i][j + 1]) && isMemoryAddress(tokens[i][j + 2], avMemory, num)) {
                     _or.getOperRes(registers[tokens[i][j + 1]].value, memory[num]);
                     memory[memIndex] = _or.code;
                     memory[memIndex + 1] = registers[tokens[i][j + 1]].address;
                     memory[memIndex + 2] = num;
                     memIndex += 3;
                  }
                   else if (isMemoryAddress(tokens[i][j + 1], avMemory, num) && isRegister(tokens[i][j + 2])) {
                     _or.getOperRes(memory[num], registers[tokens[i][j + 2]].value);
                     memory[memIndex] = _or.code;
                     memory[memIndex + 1] = num;
                     memory[memIndex + 2] = registers[tokens[i][j + 2]].address;
                     memIndex += 3;
                  }
                  else if (isMemoryAddress(tokens[i][j + 1], avMemory, num) && getNumber(tokens[i][j + 2], num2)) {
                     _or.getOperRes(memory[num], num2);
                     memory[memIndex] = _or.code;
                     memory[memIndex + 1] = num;
                     memory[memIndex + 2] = num2;
                     memIndex += 3;
                  }
                  
               }
               else if (token == "NOT") {
                  if (isRegister(tokens[i][j + 1])) {
                     _not.getOperRes(registers[tokens[i][j + 1]].value);
                     memory[memIndex] = _not.code;
                     memory[memIndex + 1] = registers[tokens[i][j + 1]].value;
                     memIndex += 3;
                  } 
                  else if (isMemoryAddress(tokens[i][j + 1], avMemory, num)) {
                     _not.getOperRes(memory[num]);
                     memory[memIndex] = _not.code;
                     memory[memIndex + 1] = num;
                     memIndex += 3;
                  } 

               }
               else if (token == "CMP") {
                  if (isRegister(tokens[i][j + 1]) && isRegister(tokens[i][j + 2])) {
                     registers["ZA"].value = cmp.getOperRes(registers[tokens[i][j + 1]].value, registers[tokens[i][j + 2]].value);
                     memory[memIndex] = cmp.code;
                     memory[memIndex + 1] = registers[tokens[i][j + 1]].address;
                     memory[memIndex + 2] = registers[tokens[i][j + 2]].address;
                     memIndex += 3; 
                  }
                 else if (isRegister(tokens[i][j + 1]) && getNumber(tokens[i][j + 2], num)) {
                     registers["ZA"].value = cmp.getOperRes(registers[tokens[i][j + 1]].value, num);
                     memory[memIndex] = cmp.code;
                     memory[memIndex + 1] = registers[tokens[i][j + 1]].address;
                     memory[memIndex + 2] = num;
                     memIndex += 3; 
                  }
                else if (getNumber(tokens[i][j + 1], num) && getNumber(tokens[i][j + 2], num2)) {
                     registers["ZA"].value = cmp.getOperRes(num, num2);
                     memory[memIndex] = cmp.code;
                     memory[memIndex + 1] = num;
                     memory[memIndex + 2] = num2;
                     memIndex += 3; 
                  }
                 else if (getNumber(tokens[i][j + 1], num) && isRegister(tokens[i][j + 2])) {
                     registers["ZA"].value = cmp.getOperRes(num, registers[tokens[i][j + 2]].value);
                     memory[memIndex] = cmp.code;
                     memory[memIndex + 1] = num;
                     memory[memIndex + 2] = registers[tokens[i][j + 2]].address;
                     memIndex += 3; 
                  }
                  else if (isRegister(tokens[i][j + 1]) && isMemoryAddress(tokens[i][j + 2], avMemory, num)) {
                     registers["ZA"].value = cmp.getOperRes(registers[tokens[i][j + 1]].value, memory[num]);
                     memory[memIndex] = cmp.code;
                     memory[memIndex + 1] = registers[tokens[i][j + 1]].address;
                     memory[memIndex + 2] = num;
                     memIndex += 3; 
                  }
                  else if (isMemoryAddress(tokens[i][j + 1], avMemory, num) && isRegister(tokens[i][j + 2]) ) {
                     registers["ZA"].value = cmp.getOperRes(memory[num], registers[tokens[i][j + 2]].value);
                     memory[memIndex] = cmp.code;
                     memory[memIndex + 1] = num;
                     memory[memIndex + 2] = registers[tokens[i][j + 2]].address;
                     memIndex += 3; 
                  }
                  else if (isMemoryAddress(tokens[i][j + 1], avMemory, num) && getNumber(tokens[i][j + 2], num2)) {
                     registers["ZA"].value = cmp.getOperRes(memory[num], num2);
                     memory[memIndex] = cmp.code;
                     memory[memIndex + 1] = num;
                     memory[memIndex + 2] = num2;
                     memIndex += 3;
                  }
                  else if (getNumber(tokens[i][j + 1], num) && isMemoryAddress(tokens[i][j + 2], avMemory, num2)) {
                     registers["ZA"].value = cmp.getOperRes(num, memory[num2]);
                     memory[memIndex] = cmp.code;
                     memory[memIndex + 1] = num;
                     memory[memIndex + 2] = num2;
                     memIndex += 3;
                  }
                 
                 }
               else if (token == "JMP" && labels[tokens[i][j + 1]]) {
                 i = labels[tokens[i][j + 1]] - 2;
                 //std::cout << tokens[labels[tokens[i][j + 1]] - 2][0] << " " << i << std::endl;
                 memory[memIndex] = jmp.code;
                 memory[memIndex + 1] = i;
                 memIndex = 3 * i;
               } 
               else if (token == "JG" && labels[tokens[i][j + 1]] && registers["ZA"].value == 1) {
                 i = labels[tokens[i][j + 1]] - 2;
                 memory[memIndex] = jg.code;
                 memory[memIndex + 1] = i;
                 memIndex = 3 * i;
               } 
               else if (token == "JE" && labels[tokens[i][j + 1]] && registers["ZA"].value == 0) {
                 i = labels[tokens[i][j + 1]] - 2;
                 memory[memIndex] = je.code;
                 memory[memIndex + 1] = i;
                 memIndex = 3 * i;
               }
               else if (token == "JL" && labels[tokens[i][j + 1]] && registers["ZA"].value == -1) {
                 i = labels[tokens[i][j + 1]] - 2;
                 memory[memIndex] = jl.code;
                 memory[memIndex + 1] = i;
                 memIndex = 3 * i;
               }  
                              
            }
            
          
         }
       return true;  
      }
      void run(std::string fileName) {
        std::vector<std::vector<std::string>> tokens;
        int lineCount = 0;
        if (tokenize(fileName, tokens, lineCount)) {
           storeMemory(tokens, lineCount);
        }
      }
   };






int main(int argc, char* argv[]) {
   if (argc != 2) {
      std::cout << "Please provide the path to the source file as an argument." << std::endl;
      return 0;
   }
   std::string fileName = argv[1];
   CPU cpu;
   cpu.run(fileName);
   return 0;
}



