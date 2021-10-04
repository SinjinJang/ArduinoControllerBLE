# ArduinoControllerBLE

### 용도

아두이노를 이용하여 각종 H/W들을 제어하는 (범용의) 컨트롤러를 만들고자 하며, Bluetooth Serial 통신을 통해 이를 제어하고자 한다.

### 사용 H/W

* Arduino Nano 33 BLE
* HC-06 (RX0, TX1 연결)

### 빌드 준비

1. ArduinoBLE 라이브러리 설치: 툴 -> 라이브러리 관리 -> `ArduinoBLE` 검색 후 설치
2. 보드 BSP 설치: 툴 -> 보드 -> 보드 매니저 -> `Arduino Mbed OS Nano Boards` 검색 후 설치
3. 보드 선택: 툴 -> 보드 -> Arduino Mbed OS Nano Boards -> Arduino Nano 33 BLE

### 프로토콜 정의

* GPIO 제어 형식: ^[포트번호]:[H|L|?]$
  * ex1: 13번 포트에 HIGH 출력: ^13:H$
  * ex2: 13번 포트에 LOW 출력: ^13:L$
  * ex3: 13번 포트의 값 읽기: ^13:?$

### 테스트 with Android Device

1. 안드로이드 장비에서 Bluetooth ON
2. Serial Bluetooth Terminal 앱 설치: https://play.google.com/store/apps/details?id=de.kai_morich.serial_bluetooth_terminal&hl=ko&gl=US
3. 앱 내 Devices 메뉴에서 `hw-controller` 선택하여 연결
4. Terminal 메뉴에서 `^13:H$` 입력 시 Nano 33 BLE 보드 status LED가 켜짐을 확인
5. Terminal 메뉴에서 `^13:L$` 입력 시 Nano 33 BLE 보드 status LED가 꺼짐을 확인