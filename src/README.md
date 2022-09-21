# stm32f4-slcan

for nucleo-F446RE

nucleoF446REをwindows pythonからpython-canから使用できるデバイスにします。CAN1は使用せずクロック供給のみ、CAN2を使用します。
python-canではinterface名をslcal, ttyBaudrate=921600にします。
mcp2562, TJ1050といったトランシーバICを
PB8, PB9に接続します。
PA1 = CAN-TX LED
PA0 = CAN-RX LED
PB4 = 圧電ブザー
としています。


開発環境
microsoft Visual Studio Code
libopencm3
ststm32
platformio

stm32f4シリーズなら、移植は簡単だと思います。
nucleo F446REにて動作確認

作成日付: 20220-9-18

