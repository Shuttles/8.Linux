# ubuntu手动配置联网(by少年宇2019.7.19)

## 一、概述

原本ubuntu是用Network Manager 来联网的，而另一种方式networking（手动配置）更加稳定。所以改用这种配置。

## 二、具体操作

1. 用命令`ifconfig` 查看自己的网卡，比如我自己的网卡是wlp2s0。 

2. 先关闭自己的NetworkManger 服务。` service NetworkManger stop` 

3. 进入超管 `sudo -i ` 

4. ` vim /etc/network/interfaces ` 进入这个文件。

5. 会发现这个文件原本有3行，接着在这三行下面添加如下代码

   ```
   auto wlp2s0
   allow-hotplug wlp2s0
   iface wlp2s0 inet dhcp
   		pre-up wpa_supplicant -Dwext -i wlp2s0 -c /etc/wpa_supplicant/wpa_supplicant.conf -B
   ```

   

   保存（划重点！！！）并退出。          

6. `vim /etc/wpa_supplicant/wpa_supplicant.conf` 进入这个文件， 会发现这原本是空的或者原本不存在，接着写下如下代码：

   ```bash
   country=CN
   
   ctrl_interface=DIR=/var/run/wpa_supplicant Group=netdev
   
   update_config=1
   
   network={
   
   ​				ssid="HaiZei_Tech"
   
   ​				psk="HaiZei731."
   
   ​				scan_ssid=1
   
   ​				#上一行是因为海贼科技这个wifi是隐藏网络
   
   ​				priority=1
   
   }
   ```

   

保存（划重点！！！！）并退出。

7. 输入命令`service networking restart`
8. 测试 首先输入命令`ping 8.8.8.8`
9. 如果成功就继续输入命令`ping www.baidu.com`

都成功就大功告成了嘿嘿嘿

## 三、遇到的问题

1. 进行了步骤7之后还是连不上网

   ​     查看NetworkManger是否关闭`service NetworkManger status`

   确保它关闭。

   

2. ping 8.8.8.8成功但是ping www.baidu.com不成功

   ​      将DNS改为8.8.8.8

   ​		即`vim /etc/resolv.conf` 将nameserver后的数字改为8.8.8.8

   ​	如果还不行，就reboot之后再作此操作 （有可能是网卡没关）