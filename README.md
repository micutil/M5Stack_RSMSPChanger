# M5Stack_RSMSPChanger

MSPChanger for M5Stackは、双葉さんのRS304系のサーボのコンプライアンスマージン、コンプライアンススロープ、パンチの設定を変更するためのM5Stackのプログラムです。このプログラムはSD-UploaderまたはLovyanLaouncherに対応していて、起動にはこれらもインストールされている必要があります。

使い方の簡単な動画は以下のURLをみて下さい。
https://www.youtube.com/watch?v=G_qbhUKBMIw

●インストール
M5Stackに入れるmicroSDに、同梱のMSPChanger.binファイルをコピーして下さい。

FONT.BIN, FONTLCD.BINをまだ入れてない場合は、以下の手順で入れて下さい。
ブラウザで、https://github.com/Tamakichi/Arduino-KanjiFont-Library-SD を開きます。
「Clone or download」でDownload ZIPを選び、ダウンロードする。
ダウンロードしたファイルの中に、fontbinというフォルダがあり、その中にFONT.BIN, FONTLCD.BINというファイルがあるので、これら２つのファイルをmicroSDにコピーします。

●接続
M5Stackとサーボをジャンパーワイヤなどでつなぎます。

	M5Stack		RS304系サーボ（写真の場合上から）
	T2		…	シグナル
	5V		…	パワー
	GND		…	グランド

写真のようなコネクターを作る場合は、ROBOMICのブログ
http://micono.cocolog-nifty.com/blog/2013/04/rs308md-f205.html
を参考にして下さい。

注意：複数のサーボがつながっている場合、パソコンからのUSBでは電流不足になります。ので、モバイルバッテリーをM5Stackにつなげて行ってください。



●MSPChangerを起動
LovyanLauncherを起動、SD-UploaderからIJUploaderを選択して起動させて下さい。

まだ入れてない方は以下の手順でLovyanLauncherを入れて下さい。
https://m5stack.com/download にアクセス
M5Burnerをダウンロード
M5Burnerを起動させて、LovyanLauncherを選択してインストール
https://github.com/lovyan03/M5Stack_LovyanLauncher/tree/master/LovyanLauncher にアクセス
LovyanLauncher.ino.m5stack_core_esp32.binをダウンロードして、ファイル名をmenu.binに変えて、microSDに入れる

●設定変更
コンプライアンスマージン、コンプライアンススロープ、パンチ、サーボのIDの順で設定していきます。MSPChangerが起動し、両端のボタンで設定値を変えて、真ん中のOKボタンを押すと、次、次と、設定項目が変更していきます。
IDは255を選ぶと、全てのサーボが対象になりますので、複数のサーボがつながっているサーボすべて同じ値に変更する時や、サーボのIDがわからない場合は、255を選ぶといいでしょう。

サーボIDまで設定が進むと、次にボタンが、「やり直し」、「書換え」、「接続調べ」に変わります。設定をもう一度やり直したい場合は、「やり直し」を押して最初から設定し直して下さい。

●接続テスト
「接続調べ」を押すとサーボが10度ごと回転しますので、正しく接続されていてサーボが動くか確認して下さい。

●変更を保存
設定値は変更しただけで、ROMに書き込まれていませんので、「書換え」ボタンを押して書き換えを行って下さい。この時に電源が切れるとサーボが壊れるかもしれませんのでちゃんと電源を接続して行って下さい。

問い合わせ
micono@mac.com
同梱されている書類すべてに関して、無断転載、掲載、改変を禁じます。
これらを使ったことにより生じるいかなるトラブルに関しても自己責任においておこなって下さい。
