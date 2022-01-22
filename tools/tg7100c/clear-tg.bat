del /a /f /s /q "*.ini"
del /a /f /s /q "iot.toml"
del /a /f /s /q "mcu.toml"
del /a /f /s /q "storage.toml"
del /a /f /s /q "partition.bin"
del /a /f /s /q "ro_params.dtb"
del /a /f /s /q .\chips\tg7100c\img_create_iot\*.bin
del /a /f /s /q .\chips\tg7100c\img_create_iot\*.pack
del /a /f /s /q .\chips\tg7100c\img_create_mcu\*.bin
del /a /f /s /q .\chips\tg7100c\img_create_mcu\*.pack
rd /s /Q .\chips\tg7100c\ota
pause