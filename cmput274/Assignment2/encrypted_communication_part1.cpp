#include <Arduino.h>


void setup() {
    init();
    int server = 13;

    Serial.begin(9600);
    Serial3.begin(9600);
    pinMode(server,INPUT);
}


uint32_t mulmod(uint32_t a, uint32_t b, uint32_t m) {
    uint32_t total = 0;
    uint32_t num1 = a%m;
    uint32_t num2 = b%m;
    
    while (num2 > 0) {
        
        if (num2%2 == 1)
            total = (total + num1)%m;
            
        num1 = (num1 * 2)%m;
        
        num2 /= 2;
    }
    
    return total%m;
}


uint32_t powmod(uint32_t x, uint32_t pow, uint32_t m) {
    uint32_t ans = 1;
    uint32_t pow_x = x;

    while (pow > 0) {
        if (pow&1 == 1) {
            ans = mulmod(ans,pow_x,m);
        }

    pow_x = mulmod(pow_x,pow_x,m);

    pow >>= 1;

    }

    return ans;
}


void  uint32_to_serial3(uint32_t  num) {
    Serial3.write((char) (num  >> 0));
    Serial3.write((char) (num  >> 8));
    Serial3.write((char) (num  >> 16));
    Serial3.write((char) (num  >> 24));
}


uint32_t  uint32_from_serial3 () {
    uint32_t  num = 0;
    num = num | (( uint32_t) Serial3.read()) << 0;
    num = num | (( uint32_t) Serial3.read()) << 8;
    num = num | (( uint32_t) Serial3.read()) << 16;
    num = num | (( uint32_t) Serial3.read()) << 24;

    return  num;
}


int main() {
    setup();

    int server = 13;
    bool isserver = digitalRead(server);

    uint32_t d;
    uint32_t n;
    uint32_t e;
    uint32_t m;

    if(isserver) {
        d = 27103;
        n = 95477;
        e = 11;
        m = 84823;
    }
    else{
        d = 38291;
        n = 84823;
        e = 7;
        m = 95477;
    }

    while(true){
        if(Serial.available() > 0) {
            uint8_t byte1 = Serial.read();
            Serial.write(byte1);

            uint32_t encrypted_byte = powmod(byte1, e, m);

            uint32_to_serial3(encrypted_byte);

            if(byte1 == '\r'){
            	Serial.write('\n');
                uint8_t newline = '\n';
                uint32_t sendnewline = powmod(newline,e,m);
                uint32_to_serial3(sendnewline);
            }
        }

        if(Serial3.available() >= 4) {
            uint32_t undecrypted_byte = uint32_from_serial3();

            uint32_t byte2 = powmod(undecrypted_byte, d, n);

            Serial.write(byte2);
        }
    }

    Serial.flush();
    Serial3.flush();
    return 0;
}


