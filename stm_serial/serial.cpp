#include<iostream>
#include<pigpiod_if2.h>

using namespace std;
const char *port = "/dev/ttyAMA0";
int baudrate = 115200;

int main(){
	int pi = pigpio_start(0, 0);
	unsigned char dummy_flag{};
	int serial_handle = serial_open(pi, const_cast<char *>(port), baudrate, dummy_flag);
	if(serial_handle < 0){
		cout << "Serial Initialize Failed" << endl;
	}else{
		cout << "Serial Initialize complete" << endl;
	}

	//send
	uint8_t dataByte[2];
	uint8_t checksum_send;
	uint16_t mg = 0;
	//receive
	uint8_t got_data{};
	uint8_t byte_now{};
	uint8_t byte[2]{};
	uint16_t result{};
	while(1){
		//send
		checksum_send = 0;
		if(mg > 1000)mg = 0;
		mg++;
		dataByte[0] = (mg >> 8) & 0xFF;
		dataByte[1] = (mg >> 0) & 0xFF;
		serial_write_byte(pi, serial_handle, 0x02);
		for (int i = 0; i < 2; ++i) {
			if ((dataByte[i] == 0x7D) || (dataByte[i] == 0x02)) {
				serial_write_byte(pi, serial_handle, 0x7D);
				checksum_send += 0x7D;
				serial_write_byte(pi, serial_handle, dataByte[i] ^ 0x20);
				checksum_send += dataByte[i] ^ 0x20;
			} else {
				serial_write_byte(pi, serial_handle, dataByte[i]);
				checksum_send += dataByte[i];
			}
		}
		serial_write_byte(pi, serial_handle, checksum_send);
		//--------------------------------------------------------------------------------------------------------------------------------------------
		//receive
		uint8_t checksum{};
		got_data = static_cast<uint8_t>(serial_read_byte(pi, serial_handle));
		if(got_data == 0x02){
			for(int i = 0; i < 2; ++i){
				byte_now = static_cast<uint8_t>(serial_read_byte(pi, serial_handle));
				if(byte_now == 0x7D){
					checksum += 0x7D;
					uint8_t next_byte = static_cast<uint8_t>(serial_read_byte(pi, serial_handle));
					byte[i] = next_byte ^ 0x20;
					checksum += next_byte;
				}else{
					byte[i] = byte_now;
					checksum += byte_now;
				}
			}
			uint8_t checksum_receive = static_cast<uint8_t>(serial_read_byte(pi, serial_handle));
			if(checksum_receive == checksum){
				result = static_cast<uint16_t>(((byte[0] << 8) & 0xFF00) | ((byte[1] << 0) & 0x00FF));
				cout << "result : " << result << endl;
			}
		}
	}
	serial_close(pi, serial_handle);
}
