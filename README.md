# VideoPlayer
## 实现的功能

基于FFmpeg和SDL实现简易视频播放器，播放器目前仅仅能够显示画面，不能播放声音。
播放过程中按下F10可暂停视频，再次按下F10可继续播放视频。
详细功能见[链接](https://www.baidu.com/)

## 项目构建说明
将所有源代码文件，include，lib文件夹文件下载到项目文件夹中，将FFmpeg和SDL所
有头文件拷贝到include文件夹下，并将FFmpeg和SDL有关动态库文件放入项目文件夹下
使用cmake构建项目，项目会自动生成在build文件夹中，使用VC++打开项目，编译运行。

注意事项：应根据下载的FFmpeg和SDL版本选择32位或64位。
