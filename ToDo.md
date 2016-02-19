# Todo
京都バスの予約,Drone,Blender持っていく

## UltraPiet
Dll File 
Linuxでの実行の対応
3D化,copy paste(Redo)
標準ライブラリは全てG/~から始まるとするG/~ に行くときはultrapietはカレントディレクトリを変更しない。また、ultrapiet.exeのあるパスのGディレクトリには沢山あるのでそこに標準ライブラリを置くことにする
DLLでなく組み込みの関数は、Roll/STD~に任せる
OpenGL,Audio
|識別| Cでの型 | Cでの別名 |
|i   | int   | INT,LONG,BOOL|
|c   | char  | CHAR   |
|s   | short | SHORT  |
|u   | unsigned int   | UINT,DWORD,ULONG  |
|b   | unsigned char  | BYTE |
|w   | unsigned short | WORD |
|f   | float | FLOAT  |
|v   | void  | (返り値) |
|C   | const char* | LPSTR,LPWSTR | 木を文字列として処理する場合。
|V   | void* | HANDLE | 保持している数値をそのままポインタとして使う場合.
  // 返り値に v を指定した場合は返り値はありません。
  // その他の場合はその返り値を返します。
  // V の時は更新された値を保持したスタックも得ます。
  // 帰り値の型はポインタは無効です。
2/24 (水) Kickoff 17:30~ 19:00
          Cahon



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