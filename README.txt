Windowsでのselect()書き換えコンテスト課題
						2012.07.07 @penkoba


本プログラムは、Linux, cygwinなどUNIX系環境で動作するselect()の例です。

標準入力とシリアル入力を監視ししています。

コマンド起動例：
USB-serialデバイスが接続されているとして
Linux：  fancy_cui /dev/ttyUSB0
cygwin： fancy_cui /dev/com4

注１）
serial_util.cでシリアルの通信速度などを設定しています。
実デバイスで動作させてみる場合は、適切な設定に変更してください。

注２）
cygwinの場合は tcsetattr() が効かないようなので、本コマンドの前に
mode.com などでシリアル設定をしてください。

注３）
例えばキーボード入力をシリアルラインに流して実際に動作させられるデバイスを
使う場合、fancy_cui.cの44行目のコメントを外せば動作します。
一例ですがBUFFALOのRemoteStationの場合、以下のような動作が可能です。
got data on stdin: i
				got data on serial: 0x4f
got data on stdin: r
				（リモコン照射）
				got data on serial: 0x59
				got data on serial: 0x53
				got data on serial: 0xff
				got data on serial: 0xff
				:


課題：
このプログラムをWindowsで動くように書き換えてください。

条件：
・Visual Studioでコンパイルできること
　（バージョンはとりあえず問いません）
・非標準ライブラリを使用しないこと
・スレッドを使わないこと

賞典：
感謝の気持ちをお送りします。


ご応募お待ちしています ^^
