# Todo
京都バスの予約,Drone,Blender持っていく

#define MainWindow::CB

## UltraPiet
File 
Linuxでの実行の対応
3D化,copy paste(Redo)
標準ライブラリは全てG/~から始まるとする(Global)
ultrapiet.exeのあるパスのGディレクトリに標準ライブラリを置くことにする。
G/~ に行くときはultrapietはカレントディレクトリを変更しない。
DLLでなく組み込みの関数は、 Div0 L/~ に任せる(Library)
2/24 (水) Kickoff 17:30~ 19:00
          Cahon
//widgettest / glgamewidget は不要？
//myglwidget
//window.ui <->testwidget
OpenGL,Audio

MODULE |  LIBRARY(組み込み含む)の作り方の指針
Div 0 はスタックの先頭要素のみを引き継ぎます。
この制約により、先頭要素以外が変更されないことが保証されます。
なのでn個の引数のものを取る関数を作る場合は、
1 呼び出し側で 3 , 5 , "Str" のようにPushしたのち Mod0で先頭を [3,5,"Str"]にします
2 呼び出された関数では先頭の [3,5,"Str"] がStackに積まれます。
3 呼び出された関数のほうでO(N){elem} を使って分解&サイズ(この場合は3)を取得します。
4 あとは好きなように関数のほうですればよいわけです。
5 関数から帰るときはその関数で使ったスタックの要素をすべて順に積んでいきます。
  例えば呼び出された関数が[3,5,"Str"] => 3,5,"Str" => "%(8)Str" のように
  加算し続けたのちMod0でパックするものだとしたら、パックした["%(8)Str"] を
  呼び出し元のスタックに積みます。
  安全性のためにも、スタックをパックしてから元の関数に戻ったほうがよいでしょう。
  つまりこの関数によって、[...,[3,5,"Str"]] が [...,["%(8)Str"]]になるというわけです
  呼び出し元の関数が何もしなかった場合、[...,[3,5,"Str"]] => [...,[3,5,"Str"]] のように
  何も変化しません。
  スタックが空で呼び出した場合は、そのまま空です。
組み込み標準関数もこの制約に従います。



DLL : 
例 Roll "user32" "MessageBoxA" "iCCui" [[0,"Hello","title",0]]
|識別| Cでの型 | Cでの別名 |
|i   | int   | INT,LONG,BOOL,HANDLE | 
|c   | char  | CHAR   |
|s   | short | SHORT  |
|u   | unsigned int   | UINT,DWORD,ULONG  |
|b   | unsigned char  | BYTE |
|w   | unsigned short | WORD |
|f   | float | FLOAT  |
|v   | void  | (返り値) |
|C   | const char* | LPSTR,LPWSTR | 木を文字列として処理する場合。
|N   | void* | HANDLE | 新たにintptr_tとしてポインタを得る場合.(PietTreeの中身は更新される)
  // 返り値に v を指定した場合は返り値はありません。
  // その他の場合はその返り値を返します。
  // N の時は更新された値を得ます。
  // 現在、帰り値の型はポインタは無効です。



Greater(LoadFile)
	[R] or [0] : 読み込み(Read)モードで開く.
	[W] or [1] : 書き込み(Write)モードで開く.
	[A] or [2] : 追加書き込み(Append)モードで開く
	[r] or [3] : Readで開いたファイルを閉じる.
	[w] or [A] or [4] : Writeで開いたファイルを閉じる.
	その他 : 現在開いているファイルをRead,Writeともに閉じる.
    Greater ["f1.txt",r] => [1] : f1.txtを開き標準入力をリダイレクトする
    Greater ["f2.txt",1] => [1] : f2.txtを開き標準出力をリダイレクトする
    Greater ["f.t",r] => [0] : f.tが開けなかった.
    Greater ["f3.txt",a] => [1] : 2.で開いていたf2.txtを閉じてf3.txtをAppendで開く
    Greater [[],[cr]] => [0] : 1.で開いていたf1.txtを閉じる.


優先順位 低
マルチスレッドはめんどいね
色18色のもめんどいね。
Fontを変更できるように

# 他の人に手伝って欲しいところ
標準ライブラリの充実

Qt:You can use various MaltiMedia & GUIs so easy.
1. Install Qt5.5 (& qmake & make) (by this,you can built only-window application.)
2. qmake this project file.

---------------------------------------------------------------------------------
Piet# : 概要
①as CUI : Piet#をPATHに通せばそれだけで実行できる.音ゲーが出来る程度の速度は欲しい
②as GUI : GUIWidgets,OpenGL,Music,... 
  Python (Bot移行):iPython,マインクラフト on Piet#?
  Piet * fork * Meta
     (スタックは共有したほうがおもしろい、
      親は0を次のはid!=0かid=1を積むことにしよう、
      同期式にしよう)
      例えばライフゲームやゲームAIのシュミレーションが書けると楽しい


RGB32 //A==255(常に)だが、Unicode値を取得処理するときはA == 0で正の値として扱う
IN  : c.unicode       (Button u)
OUT : get (pixel()()) (Pushのところ 絵のとこ) => cut A する
 => 負の数として保存したものが変わってしまうので注意 (そもそも入力できなくすればいい)

文字の例(Unicode はU+1FFFFF)までなので￮￭￬￫￪￩￨￧￦￥￤￣￢￡￠ￜￛￚￗￖￕￔￓￒￏￎￍￌￋￊￇￆￅￄￃￂﾾﾽﾼﾻﾺﾹﾸ
整数値しかとらないので大丈夫