#include<iostream>
#include<pigpiod_if2.h>

using namespace std;
const char *port = "/dev/ttyAMA0";
int baudrate = 115200;

const uint8_t HEAD_BYTE = 0x7E;
const uint8_t ESCAPE_BYTE = 0x7D;
const uint8_t ESCAPE_MASK = 0x20;

int main(){
	int pi = pigpio_start(0, 0);
	unsigned char dummy_flag{};
	int serial_handle = serial_open(pi, const_cast<char *>(port), baudrate, dummy_flag);
	if(serial_handle < 0){
		cout << "Serial Initialize Failed" << endl;
	}else{
		cout << "Serial Initialize complete" << endl;
	}


	uint8_t got_data{};
	uint8_t byte[2]{};
	uint16_t result{};
	while(1){
		got_data = static_cast<uint8_t>(serial_read_byte(pi, serial_handle));
		if(got_data == HEAD_BYTE){
			for(int i = 0; i < 2; ++i){
				uint8_t d = static_cast<uint8_t>(serial_read_byte(pi, serial_handle));
				if(d == ESCAPE_BYTE){
					byte[i] = static_cast<uint8_t>(serial_read_byte(pi, serial_handle)) ^ ESCAPE_MASK;
				}else{
					byte[i] = d;
				}
			}
			result = static_cast<uint16_t>((byte[0] << 8) | (byte[1] << 0));
		}
		cout << result << endl;
	}
	serial_close(pi, serial_handle);
}
