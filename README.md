编译方法： 
./build.sh example smart_outlet tg7100cevb SINGAPORE ONLINE 0

第一个参数为help时，输出build.sh当前默认编译参数.
第一个参数为clean时，执行SDK目录下example目录删除，并从仓库恢复，下次编译时，会重新完整编译整个SDK及应用.
第一个参数在Products目录下找不到对应的文件夹时，会继续从Living_SDK/example目录下找，如能找到，则执行编译，成功后，复制编译结果到out目录。

要实现不输入参数，执行./build.sh编译输出需要的应用固件，可更改以下默认参数：
default_type="example"
default_app="smart_outlet"
default_board="uno-91h"
default_region=SINGAPORE
default_env=ONLINE
default_debug=0
default_args=""
以上参数分别对应：
产品类型、应用名称、模组型号、连云区域、连云环境、debug、其他参数（可不填，需要时把参数加到双引号内）。


当前已支持的board：
tg7100c:        tg7100cevb
RDA5981A: 	hf-lpb130 hf-lpb135 hf-lpt230 hf-lpt130 uno-91h
rtl8710bn: 	mk3080 mk3092
asr5502: 	mx1270


## 注意
由于安信可统一了三元组烧录指令，当用需要使用读取三元组时需要将读取API的KEY值统一更换为安信可规定的KEY值，修改如下

获取productKey：

    aos_kv_get(KV_KEY_PK, product_key, &pk_len);

获取ProduceSecret：

    aos_kv_get(KV_KEY_PS, product_secret, &ps_len);

获取DeviceName：

    aos_kv_get(KV_KEY_DN, device_name, &dn_len);

获取DeviceSecret:

    aos_kv_get(KV_KEY_DS, device_secret, &ds_len);

获取ProductID：

    aos_kv_get(KV_KEY_PD, pidStr, &len);

安信可烧录三元组AT指令格式：

    AT+LINKKEYCONFIG="ProductID","DeviceName","DeviceSecret","ProduceSecret","ProductID"
查询三元组指令格式
    AT+LINKKEYCONFIG?

