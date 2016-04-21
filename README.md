# ゲームも作れる最強のPiet「UltraPiet」

## 遊び方
1. https://github.com/Muratam/UltraPiet/releases/ へ移動して！
1. 対応するOS版バイナリをダウンロード！
1. ./ultrapiet を実行するとIDEが起動！
1. ./ultrapiet G/C_HelloWorld.png のように引数に渡すとCUI上でも動作するよ！
1. 同梱のsampleフォルダの画像やG フォルダの画像でテスト出来ますよ！
1. sample/HangGliderGame/Sky.png で実際にultrapiet製のゲームが遊べるよ！

## UltraPietの新機能
- 既存のPietもそのまま動く！
- スタックに木構造も積むことができる！
- 強力な文字処理機構を搭載！
- DLLのインポートができる！
- 他のPietのコードを呼び出せる！
- 大きな画像も無限ループも無理なく扱えるし、追跡機能も命令履歴機能もあるよ！

## 詳細な仕様
[UltraPietWiki](https://github.com/Muratam/UltraPiet/wiki) に記載していますので
ご覧ください。

## UltraPietで新たに出来ること

##### Webアプリケーションやゲームを簡単に作れる！
旧Pietではせいぜい標準入出力になにかをするので手一杯でした.
しかし,UltraPietでは前述のDLLのインポートをサポートしているので,
気軽に素晴らしいプログラミングができます！
更に,他のPietコードを呼び出せるので,
作成した機能はモジュール化することができ,
ライブラリを揃えれば上述のことはたやすく実現できます！

##### 高い表現力により自由なコーディングが出来る！
旧Pietでは数字しかスタックに積めず。とても表現力が低かったです.
しかし,UltraPietでは,新たにスタックに木構造を積むことが出来て,
とても表現力豊かなプログラミングが出来ます.
また,新たにスタックに積んだ木構造を
文字列とみなすことで高度な文字処理が出来る機構も搭載していて,
ファイル処理や高度な文字処理にも耐えうるコーディングが可能となります.

##### 楽しく直感的なプログラミングが出来る！
旧Pietでも楽しくプログラミングが出来たのですが,
キーボードでエディットも出来ず、操作履歴も残らず、何をしているのか全く
わかりにくい問題がありましたが、
UltraPietでは専用のIDEでそれらをすべてカバーしています.
また,UltraPietのプログラミングで本質的に変わったことは,
新たに木が積めるようになったことだけなので,
学習コストも少なくて済みます！

## PietとしてのUltraPiet
UltraPietはあくまでもPietです.
色は18色しか扱わないし,
画像のみがあれば誰でも同じコードを再現できます.
これらを守ることで,Cに対するC++のように,
旧Pietのコードもそのまま動かすことが可能となります.

## CUIで使う方法(Windows)
- Windowsの場合はMinttyなどのターミナルを使って下さい。
- コマンドプロンプト及びパワーシェルでは標準入出力が上手く行われません。
  Pietをやろうとしているということは、あなたはGitをインストールしているはずです。
  「Win + R 」で「C:\Program Files\Git\usr\bin\mintty」を入力し、minttyを起動できます。
  cd ~ ののち、cd とpwd を駆使してultrapietのあるディレクトリに行き
  ./ultrapiet G/C_HelloWorld.png のようにして実行出来ます。
  ultrapietのあるディレクトリにパスを通せばperlやpythonと同じように利用できます

# How to build

## Mac OSX

NOTE: You should change all paths of qt5 unless you installed qt5 via homebrew.

### 1. Setup Environment

- Install QT5
  - `% brew install qt5`
    - `qt5` is keg-only so it is not linked.
- Add environment value
  - `% export LDFLAGS="-L/usr/local/opt/qt5/lib"`
  - `% export CPPFLAGS="-I/usr/local/opt/qt5/include"`

### 2. Apply Patch

- Apply patch for build on MacOSX
  - `% curl -s https://github.com/pastak/UltraPiet/commit/5c794d0c53557645b8a2d1edcef2d18b348e641f.patch | git apply`

### 3. qmake && make

- `% cd /path/to/ultrapiet`
- `% /usr/local/opt/qt5/bin/qmake`
- `% make`

Then there is `ultrapiet.app`.

### Create distribution dmg

- `% /usr/local/opt/qt5/bin/macdeployqt ultrapiet.app -dmg`
