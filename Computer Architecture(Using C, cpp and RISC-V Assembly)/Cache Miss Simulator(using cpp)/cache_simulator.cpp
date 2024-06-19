#include <bits/stdc++.h>
using namespace std;

class Row{
    vector<string> row;//will store blocks

    int getRandomNumber(int max){
        static random_device rd;
        static mt19937 gen(rd());
        uniform_int_distribution<> distribution(0, max);

        return distribution(gen);
    }

    void fetch_block(int associativity, string replacement_policy, string tag){
        if(row.size() >= associativity){//need to replace
            int index = 0;
            if(replacement_policy == "RANDOM"){
                index = getRandomNumber(associativity - 1);
            }

            row.erase(row.begin() + index);//delete acc to lru or fifo
        }
        
        row.push_back(tag);//add new tag
    }

    int get_tag_index(string tag){
        int tag_index = -1;

        for(int index = 0; index < row.size(); index++){
            if(row[index] == tag){//if found or hit
                tag_index = index;
                break;
            }
        }
        return tag_index;//if not found returns -1.
    }

    public:
    //default constructor for creation
    Row(){
    }

    bool read_access(int associativity, string replacement_policy, string tag){

        int index = get_tag_index(tag);
        if(index != -1){//found
            //actions for replacement policy
            if(replacement_policy == "LRU"){//most recently used taken to last position
                string temp = row.at(index);
                row.erase(row.begin() + index);//delete 

                row.push_back(temp);//and add to end
            }
            return true;//found
        }

        //when not found in cache
        fetch_block(associativity, replacement_policy, tag);
        return false;
    }

    bool write_access(int associativity, string replacement_policy, string write_policy,string tag){
        int index = get_tag_index(tag);
        if(index != -1){//found
            //update in cache
            return true;
        }

        if(write_policy == "WB"){
            fetch_block(associativity, replacement_policy, tag);
        }

        return false;
    }
};

class Cache{
    int size;//total cache size in bytes
	int block_size;//in bytes
    int associativity;
    string replacement_policy;
    string write_policy;

    int total_indexes;
    Row *cache_storage;//array[index][associativity]

    int tag_position = 0;
    int index_bits_position;
    int offset_position;

    int hits = 0, misses = 0;

    bool fully_associative = false;

    int binary_to_dec(string index_bits){
        int index = 0;
        for(int i=0; i<index_bits.length(); i++){
            if(index_bits[i] == '1'){
                index += pow(2, index_bits.length()-1-i);
            }
        }
        return index;
    }

    void print(string address, int index, bool hit, string tag){
        cout << "Address: ";
        cout << hex << showbase << binary_to_dec(address) << ", ";
        if(!fully_associative){
            cout << "Set: ";
            if(index == 0){cout << "0x";}
            cout << hex << showbase << index << ", ";
        }
        string hit_miss = hit == true? "Hit": "Miss";
        cout << hit_miss << ", ";
        cout << "tag: ";
        cout << hex << showbase << binary_to_dec(tag) << "\n";

        cout << dec;
    }

    public:
    //constructor
    Cache(int s, int b, int a, string r, string w){
        size = s;
        block_size = b;
        associativity = a;
        replacement_policy = r;
        write_policy = w;
        

        offset_position = 32 - log(block_size)/log(2);//holds first bit location

        if(associativity == 0){//Fully associative cache
            fully_associative = true;

            associativity = size/block_size;
            total_indexes = 1;
            index_bits_position = offset_position;//no index needed
        }
        else{
            total_indexes = (size/block_size)/associativity;

            index_bits_position = offset_position - log(total_indexes)/log(2);//holds first bit location
        }

        cache_storage = new Row[total_indexes];

    }

    void read_access(string address){
        int index_bit_location = index_bits_position == 32? 31: index_bits_position;//handle for when whole address is tag
        int index_bits_length = offset_position-index_bits_position;

        int index = binary_to_dec(address.substr(index_bit_location, index_bits_length));

        string tag = address.substr(0, index_bits_position);
        bool hit = cache_storage[index].read_access(associativity, replacement_policy, tag);

        if(hit){
            hits++;
            print(address, index, true, tag);
        }
        else{
            misses++;
            print(address, index, false, tag);
        }
    }

    void write_access(string address){
        int index_bit_location = index_bits_position == 32? 31: index_bits_position;//handle for when whole address is tag
        int index_bits_length = offset_position-index_bits_position;

        int index = binary_to_dec(address.substr(index_bit_location, index_bits_length));

        string tag = address.substr(0, index_bits_position);
        bool hit = cache_storage[index].write_access(associativity, replacement_policy, write_policy, tag);

        if(hit){
            hits++;
            print(address, index, true, tag);
        }
        else{
            misses++;
            print(address, index, false, tag);
        }
    }

    void print_hits(){
        cout << "Total Hits = " << hits << "\n";
        cout << "Total Misses = " << misses << "\n";
    }
};

string hex_to_binary(string hex){
    //Skips "0x" if present at beginning of Hex string
    int i = 0;
    if(hex[1] == 'x' || hex[1] == 'X') i=2;
    
    string result = "";
    for(int z = 0; z < 8-(hex.length()-i); z++){
        result += "0000";
    }

    while(hex[i]){
        switch(hex[i]){
            case '0':
                result += "0000";
                break;
            case '1':
                result += "0001";
                break;
            case '2':
                result += "0010";
                break;
            case '3':
                result += "0011";
                break;
            case '4':
                result += "0100";
                break;
            case '5':
                result += "0101";
                break;
            case '6':
                result += "0110";
                break;
            case '7':
                result += "0111";
                break;
            case '8':
                result += "1000";
                break;
            case '9':
                result += "1001";
                break;
            case 'A':
            case 'a':
                result += "1010";
                break;
            case 'B':
            case 'b':
                result += "1011";
                break;
            case 'C':
            case 'c':
                result += "1100";
                break;
            case 'D':
            case 'd':
                result += "1101";
                break;
            case 'E':
            case 'e':
                result += "1110";
                break;
            case 'F':
            case 'f':
                result += "1111";
                break;
        }

        i++;
    }
    return result;
}

int main(){
    string file_name;
    cout << "Please Enter Config file name : ";
    cin >> file_name;
    // file_name = "cache.config";
    ifstream file(file_name);

    string line;
    getline(file, line);
    int size = stoi(line);

    getline(file, line);
    int block_size = stoi(line);

    getline(file, line);
    int associativity = stoi(line);

    getline(file, line);
    string replacement_policy = line;

    getline(file, line);
    string write_policy = line;

    file.close();

    Cache cache_simulator(size, block_size, associativity, replacement_policy, write_policy);

    cout << "Please Enter input file name : ";
    cin >> file_name;
    // file_name = "test.access";
    ifstream file2(file_name);

    while(getline(file2, line)){
        //cout << line << "\n";
        string address = hex_to_binary(line.substr(3, line.length()-3));

        if(line[0] == 'R'){
            cache_simulator.read_access(address);
        }
        else if(line[0] == 'W'){
            cache_simulator.write_access(address);
        }
    }

    cache_simulator.print_hits();

    file2.close();
}