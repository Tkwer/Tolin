# 基于STMF401CCU6 Tolin核心板的DFU升级工程
包含两个工程    
bootloader工程 ```stm32f401dfu```   
app工程         ```stm32f401dfu_app```  
先烧录bootloader工程，然后通过usb接口烧录app工程，显示效果每隔0.4s，oled显示"Hello，World!"和led闪烁   
具体教程参考：