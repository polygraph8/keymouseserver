# keymouseserver
A server based on c/c++ , which can response to remote command(keymouse) to operating application on windows. 一个c/c++ 写的windows 服务，
可以接收http协议发送的远程命令，来操作服务器上的应用程序。

需要进行的鼠标键盘操作可以通过嵌套ini 进行定义。

script示例：

[steps]
loginAndSendVerifycode
logout

[loginAndSendVerifycode]
clickAt:: 658,679
clickAt:: 598,345
clickAt:: 551,270

[logout]
clickAt:: 660,683
Sleep::1000
clickAt:: 669,70
Sleep::1000
clickAt:: 588,505
mouseWheel:: 588,505
Sleep::1000
clickAt:: 399,596
clickAt:: 592,395
