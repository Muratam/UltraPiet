# Todo
京都バスの予約,Drone,Blender持っていく
Linuxでの実行の対応

## UltraPiet
②UltraPiet拡張の実装
Tab,ProjectTree
フォーク
Pasxx の対応

優先順位 低
	## Editor
	Redo
	3D化,copy paste
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


RGB32 //A==255(常に)
=> 11111111RRRRRRRRGGGGGGGGBBBBBBBB
=> 00000000RRRRRRRRGGGGGGGGBBBBBBBB
として処理している

IN  : c.unicode       (Button u)
OUT : get (pixel()()) (Pushのところ 絵のとこ) => cut A する
 => 負の数として保存したものが変わってしまうので注意 (そもそも入力できなくすればいい)

文字の例(Unicode はU+1FFFFF)までなので￮￭￬￫￪￩￨￧￦￥￤￣￢￡￠ￜￛￚￗￖￕￔￓￒￏￎￍￌￋￊￇￆￅￄￃￂﾾﾽﾼﾻﾺﾹﾸ
整数値しかとらないので大丈夫