# Todo
京都バスの予約,Droneの予約,Blender持ってくる
①普通の Piet(In(N),In(C),Roll) の完成
IDE無し(コマンドライン・例えばサーバーでの実行用)の実行の対応

## UltraPiet
②UltraPiet拡張の実装
Tab,ProjectTree
フォーク
Pasxx の対応


## Editor
可視化(矢印を追いやすくしたり、BlockSizeを見えるようにしたり,Unicode文字を表示したり)
キーボードタイピング対応
CUI化
Redo,Save
キャンバスサイズ変更
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

stack<PietStack> MainPS;
MainPS.push ({true,3,nullptr});
MainPS.push ({true,10,nullptr});
MainPS.push ({false,1,new stack<NewPietStack>{
nonamae774 (hogejs)