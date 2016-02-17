# Todo
京都バスの予約,Droneの予約,Blender持ってくる
Linuxでの実行の対応

## UltraPiet
②UltraPiet拡張の実装
Tab,ProjectTree
フォーク
Pasxx の対応
KeyBoard Editing
普通の移動 : ←↓↑→
push,pop,+,-,*,/,%,!,>,point,switch,dup,roll,inN,inC,oN,oC,White,Black,NonNormal,Same,DP
p,P,+,-,*,/,%,!,>,@,^,d,r,I,i,O,o,Space,b,ほげ,Shift+←↓↑→,Return,
(シフト必要？)(Switch == XOR ,point @ )
命令表示（全部の色ボタン）
NonNormalColor(入る時に対応する数字を取る)その他の動作は処理系にまかせよう

## Editor
Redo
C-N
Not save Waring
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
