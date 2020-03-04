下载zlib源码后解压，直接找到contrib\vstudio目录下的vs工程打开即可，静态库是zlibstat，打开sln把其他的解决方案删除单独编译zlibstat即可。

文件目录组织结构基本没动，以后有新的版本照样替换覆盖即可。

去除预处理器定义：ZLIB_WINAPI，否则编译的函数带@数字

release下提示asm的文件链接找不到obj，在工程编译选项中把对这些asm文件的引入删除即可，debug版没有这个问题，只有release版有。
32位release删除：..\..\masmx86\match686.obj;..\..\masmx86\inffas32.obj;
64位release删除：..\..\masmx64\gvmat64.obj;..\..\masmx64\inffasx64.obj;


参考：nmake、cmake、Visual Studio编译zlib https://blog.csdn.net/fksec/article/details/25906419