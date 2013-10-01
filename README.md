Berith
==========

Javaアプリケーションを起動するランチャです。

今までのさきゅばすではCreateProcessを使ってjava.exeを丸ごと起動していましたが、
この方法だと何故かウイルスと誤判定される率が高すぎるので、
仕方なくJNIを使った方法に切り替えてみます。

How to build
=============

Visual C++ 2012でソリューションを開いてビルドしてください。

インクルードパスだけ正確に設定してください。Java本体のDLLはLoadLibraryを使って動的に読み込むので、.libを追加する必要はありません。

License
=========

Berith is licensed under GPL v3 or later.
