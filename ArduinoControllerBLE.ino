/**
  Bluetooth Serial 통신을 통해 각종 포트들을 제어하는 컨트롤러의 구현

  아두이노는 Peripheral 로 동작하면서 시리얼 통신으로 요청을 받아 처리하며,
  Central은 어떤 디바이스/플랫폼이 되어도 무방하며 프로토콜에 맞게 요청을 보내면 된다.

  - H/W: Arduino Nano 33 BLE
  - RX0, TX1: HC-06

  NOTE: 보드 내장된 BLE 모듈로는 시리얼 통신이 불가능하여 사용하지 않고, 별도 모듈 연결함.
*/




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
   셋업
*/
void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
  while (!Serial1);

  Serial1.print("AT+NAMEhw-controller");

  // LED 테스트용 포트
  pinMode(LED_BUILTIN, OUTPUT);
}

/**
   데이터를 수신하여 온전한 프로토콜 형식이 되었을 때 true값을 리턴해준다.
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
   BLE로부터 전달받은 명령어를 실행한다.
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
   BLE로 응답 데이터를 보낸다.
*/
void response(int val) {
  sprintf(g_res_buf, "^%d:%c$", g_pin_num, HIGH == val ? 'H' : 'L');
  Serial1.println(g_res_buf);
  Serial.println(g_res_buf);
}

/**
   메인 루프
*/
int ch;
void loop() {
  if (Serial1.available()) {
    ch = Serial1.read();
    boolean ret = parseCmd(ch);
    if (ret) {
      int val = executeCmd();
      response(val);
    }
  }
}
