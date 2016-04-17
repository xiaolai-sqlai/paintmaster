# paintmaster
上传图片并进行深度学习风格化，然后处理成对应的动态视频。主要用了flask，celery，redis，mysql，lua，opencv等
#喷一下微信
1. 微信上传文件时，后端只能获取文件内容，无法获取form中的其他input字段的内容
2. 微信解析video标签中的视频地址时，必须用nginx设置成静态资源，如果使用框架中自带的资源定位，将无法获取视频地址
