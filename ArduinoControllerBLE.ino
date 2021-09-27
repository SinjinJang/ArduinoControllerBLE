/**
 * BLE 통신을 통해 각종 포트들을 제어하는 컨트롤러의 구현
 *
 * 아두이노는 Peripheral 로 동작하면서 시리얼 통신으로 요청을 받아 처리하며,
 * Central은 어떤 디바이스/플랫폼이 되어도 무방하며 프로토콜에 맞게 요청을 보내면 된다.
 * 
 * H/W: Arduino Nano 33 BLE
 */


#include <ArduinoBLE.h>

BLEService g_CtrlService("180A"); // BLE Service

// BLE Characteristic - custom 128-bit UUID, read and writable by central
BLEByteCharacteristic g_CmdCharacteristic("2A57", BLERead | BLEWrite);


// 명령어 파트 정의
enum Part {
  NA = 0,
  PinNum = 1,
  Value = 2,
};

// 명령어 수신 정보
Part g_part = NA;
char g_pin_num = 0;
char g_value = 0;
char g_res_buf[10];


/**
 * 셋업
 */
void setup() {
  Serial.begin(9600);
  while (!Serial);

  // LED 테스트용 포트
  pinMode(LED_BUILTIN, OUTPUT);
  // TODO: 제어할 포트 추가 정의

  // begin initialization
  if (!BLE.begin()) {
    Serial.println("starting BLE failed!");
    while (1);
  }

  // set advertised local name and service UUID:
  BLE.setLocalName("Arduino Controller Nano 33 BLE");
  BLE.setAdvertisedService(g_CtrlService);

  // add the characteristic to the service
  g_CtrlService.addCharacteristic(g_CmdCharacteristic);

  // add service
  BLE.addService(g_CtrlService);

  // set the initial value for the characteristic:
  g_CmdCharacteristic.writeValue(0);

  // start advertising
  BLE.advertise();

  Serial.println("BLE LED Peripheral");
}


/**
 * 데이터를 수신하여 온전한 프로토콜 형식이 되었을 때 true값을 리턴해준다.
 */
boolean inline parseCmd(char ch) {
  if ('^' == ch) {
    g_pin_num = 0;
    g_part = PinNum;
  } else if (':' == ch) {
    g_part = Value;
  } else if ('$' == ch) {
    g_part = NA;
    return true;
  } else {
    if (PinNum == g_part) {
      g_pin_num = g_pin_num * 10 + (ch - '0');
    } else {
      g_value = ch;
    }
  }

  return false;
}

/**
 * BLE로부터 전달받은 명령어를 실행한다.
 */
int executeCmd() {
  if ('?' != g_value) {
    digitalWrite(g_pin_num, 'H' == g_value ? HIGH : LOW);
    delay(1);
  }

  // controller로의 응답을 위해 read 작업을 수행함.
  return digitalRead(g_pin_num);
}

/**
 * BLE로 응답 데이터를 보낸다.
 */
void response(int val) {
  sprintf(g_res_buf, "^%d:%c$", g_pin_num, HIGH == val ? 'H' : 'L');
  Serial.println(g_res_buf);

  int idx = 0;
  while(g_res_buf[idx++]) {
    g_CmdCharacteristic.writeValue(g_res_buf[idx]);
  }
}

/**
 * 메인 루프
 */
void loop() {
  // listen for BLE peripherals to connect:
  BLEDevice central = BLE.central();

  // if a central is connected to peripheral:
  if (central) {
    Serial.print("Connected to central: ");

    // print the central's MAC address:
    Serial.println(central.address());
    digitalWrite(LED_BUILTIN, HIGH); // turn on the LED to indicate the connection

    // while the central is still connected to peripheral:
    while (central.connected()) {
      // 다른 디바이스로부터 수신한 데이터가 있다면 명령어 해석 및 처리를 시도한다.
      while (g_CmdCharacteristic.written()) {
        char ch = g_CmdCharacteristic.value();
        boolean ret = parseCmd(ch);
        if (ret) {
          int val = executeCmd();
          response(val);
        }
      }
    }

    // when the central disconnects, print it out:
    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
    digitalWrite(LED_BUILTIN, LOW); // when the central disconnects, turn off the LED
  }
}
